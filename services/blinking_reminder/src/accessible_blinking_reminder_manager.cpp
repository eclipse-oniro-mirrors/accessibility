/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessible_blinking_reminder_manager.h"
#include "hilog_wrapper.h"
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#ifdef OHOS_BUILD_ENABLE_CAMERA
#include "input/camera_manager.h"
#endif
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "display_power_mgr_client.h"
#endif
#ifdef OHOS_BUILD_ENABLE_SCREENLOCK_MANAGER
#include "screenlock_manager.h"
#endif
#include <securec.h>

namespace OHOS {
namespace Accessibility {
namespace {
struct ScenarioMapping {
    int32_t scenario;
    const char *name;
};

const ScenarioMapping SCENARIO_MAPPINGS[] = {
    {BlinkingScenario::ALARM, "ALARM_CLOCK"},
    {BlinkingScenario::NOTIFICATION, "NOTIFY"},
    {BlinkingScenario::CALL, "INCOMING_CALL"},
};

std::set<int32_t> ParseEnabledScenarios(const std::string &enabled)
{
    std::set<int32_t> scenarios;
    for (const auto &mapping : SCENARIO_MAPPINGS) {
        if (enabled.find(mapping.name) != std::string::npos) {
            scenarios.insert(mapping.scenario);
        }
    }
    return scenarios;
}

const char *GetScenarioName(int32_t scenario)
{
    for (const auto &mapping : SCENARIO_MAPPINGS) {
        if (mapping.scenario == scenario) {
            return mapping.name;
        }
    }
    return nullptr;
}
} // namespace

constexpr int32_t SINGLE_BLINK_PHASE_COUNT = 4;
constexpr int32_t BLINK_INTERVAL_MS = 250;
constexpr int32_t FLASH_BLINK_MAX_DURATION_MS = 1800000;
constexpr int32_t DISPLAY_BLINK_MAX_DURATION_MS = 10000;
constexpr uint32_t DEFAULT_DISPLAY_ID = 0;
constexpr uint32_t BRIGHTNESS_TRANSITION_DURATION_MS = 0;
constexpr double FORCED_BRIGHTNESS_ON_VALUE = 1.0;
constexpr double FORCED_BRIGHTNESS_OFF_VALUE = -1.0;
constexpr int32_t BLINKING_MODE_IDLE = 0;
constexpr int32_t BLINKING_SCENARIO_IDLE = 0;
constexpr int64_t PROXIMITY_SENSOR_SAMPLING_PERIOD_NS = 100000000;
constexpr float PROXIMITY_THRESHOLD = 1.0f;
constexpr int64_t DISPLAY_BUSY_THRESHOLD_MS = 1000;
constexpr int32_t BLINK_CYCLE_STEP = 2;

AccessibleBlinkingReminderManager::AccessibleBlinkingReminderManager() {}

AccessibleBlinkingReminderManager::~AccessibleBlinkingReminderManager()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        state_.isRunning = false;
    }
    cv_.notify_all();
    if (blinkingThread_ && blinkingThread_->joinable()) {
        blinkingThread_->join();
    }
    blinkingThread_.reset();
}

bool AccessibleBlinkingReminderManager::IsDisplayEnabled(int32_t mode)
{
    return (mode & FlashReminderMode::DISPLAY) != 0;
}

bool AccessibleBlinkingReminderManager::IsFlashlightEnabled(int32_t mode)
{
    return (mode & FlashReminderMode::FLASHLIGHT) != 0;
}

int64_t AccessibleBlinkingReminderManager::GetCurrentTimeMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

AccessibleBlinkingReminderManager::ScenarioState AccessibleBlinkingReminderManager::MakeScenarioState(
    int32_t scenario, int32_t mode, bool displayBlocked)
{
    ScenarioState scenarioState;
    scenarioState.scenario = scenario;
    scenarioState.joinTimeMs = GetCurrentTimeMs();
    scenarioState.displayExpired = displayBlocked || !IsDisplayEnabled(mode);
    scenarioState.flashlightExpired = !IsFlashlightEnabled(mode);
    if (!scenarioState.displayExpired) {
        scenarioState.displayStartTimeMs = scenarioState.joinTimeMs;
    }
    return scenarioState;
}

bool AccessibleBlinkingReminderManager::IsScreenLocked()
{
#ifdef OHOS_BUILD_ENABLE_SCREENLOCK_MANAGER
    return ScreenLock::ScreenLockManager::GetInstance()->IsScreenLocked();
#else
    return false;
#endif
}

bool AccessibleBlinkingReminderManager::IsScenarioEnabled(int32_t scenario)
{
    const std::string &enabled = state_.flashReminderFunctionEnabled;
    if (enabled.empty()) {
        return false;
    }
    const char *name = GetScenarioName(scenario);
    return name != nullptr && enabled.find(name) != std::string::npos;
}

bool AccessibleBlinkingReminderManager::IsFlashlightSupported()
{
#ifdef OHOS_BUILD_ENABLE_CAMERA
    if (flashlightSupportChecked_) {
        return flashlightSupported_;
    }
    sptr<CameraStandard::CameraManager> cameraManager = CameraStandard::CameraManager::GetInstance();
    if (!cameraManager) {
        HILOG_ERROR("Failed to get camera manager");
        return false;
    }
    flashlightSupported_ = cameraManager->IsTorchSupported();
    flashlightSupportChecked_ = true;
    return flashlightSupported_;
#else
    return false;
#endif
}

bool AccessibleBlinkingReminderManager::IsDisplaySupported()
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    if (displayBlinkSupportChecked_) {
        return displayBlinkSupported_;
    }
    displayBlinkSupported_ = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().GetFeatureSupport(
        DisplayPowerMgr::BrightnessFeatureType::SCREEN_FLASH);
    displayBlinkSupportChecked_ = true;
    return displayBlinkSupported_;
#else
    return false;
#endif
}

int32_t AccessibleBlinkingReminderManager::ComputeEffectiveMode(const BlinkState &state)
{
    if (state.scenarioStates.empty()) {
        return FlashReminderMode::NONE;
    }
    bool anyDisplay = false;
    bool anyFlash = false;
    for (const auto &[_, scenarioState] : state.scenarioStates) {
        if (!scenarioState.displayExpired) {
            anyDisplay = true;
        }
        if (!scenarioState.flashlightExpired) {
            anyFlash = true;
        }
    }
    int32_t active = FlashReminderMode::NONE;
    if (anyDisplay) {
        active |= FlashReminderMode::DISPLAY;
    }
    if (anyFlash) {
        active |= FlashReminderMode::FLASHLIGHT;
    }
    return active & state.flashReminderMode;
}

void AccessibleBlinkingReminderManager::ResetRuntimeState()
{
    state_.currentMode = BLINKING_MODE_IDLE;
    state_.singleScenario = BLINKING_SCENARIO_IDLE;
    state_.scenarioStates.clear();
    state_.pausedByProximity = false;
}

void AccessibleBlinkingReminderManager::JoinFinishedThread(std::unique_lock<std::mutex> &lock)
{
    if (!state_.isRunning && blinkingThread_ && blinkingThread_->joinable()) {
        lock.unlock();
        blinkingThread_->join();
        lock.lock();
        blinkingThread_.reset();
        ResetRuntimeState();
    }
}

void AccessibleBlinkingReminderManager::ForceStopBlinkingInternal(
    std::unique_lock<std::mutex> &lock, int32_t cleanupMode)
{
    HILOG_INFO("cleanupMode=%{public}d", cleanupMode);
    state_.isRunning = false;
    cv_.notify_all();
    lock.unlock();
    if (blinkingThread_ && blinkingThread_->joinable()) {
        blinkingThread_->join();
    }
    blinkingThread_.reset();
    lock.lock();
    CleanupBlinking(cleanupMode);
    ResetRuntimeState();
}

void AccessibleBlinkingReminderManager::ForceStopAll()
{
    HILOG_INFO("ForceStopAll");
    std::unique_lock<std::mutex> lock(mutex_);
    if (!state_.isRunning) {
        return;
    }
    ForceStopBlinkingInternal(lock, state_.flashReminderMode);
}

void AccessibleBlinkingReminderManager::ExecuteBlinkCycle(
    std::unique_lock<std::mutex> &lock, int32_t mode, bool isOn)
{
    bool doFlash = IsFlashlightEnabled(mode);
    bool doDisplay = !state_.pausedByProximity && IsDisplayEnabled(mode);
    lock.unlock();
    if (doFlash) {
        SetFlashlightMode(isOn);
    }
    if (doDisplay) {
        SetDisplayBlinkMode(isOn);
    }
    lock.lock();
}

void AccessibleBlinkingReminderManager::RemoveExpiredScenarios()
{
    int64_t nowMs = GetCurrentTimeMs();
    std::vector<int32_t> toRemove;
    for (auto &[scenario, scenarioState] : state_.scenarioStates) {
        if (!scenarioState.displayExpired && scenarioState.displayStartTimeMs > 0 &&
            (nowMs - scenarioState.displayStartTimeMs) >= DISPLAY_BLINK_MAX_DURATION_MS) {
            scenarioState.displayExpired = true;
        }
        if (!scenarioState.flashlightExpired &&
            (nowMs - scenarioState.joinTimeMs) >= FLASH_BLINK_MAX_DURATION_MS) {
            scenarioState.flashlightExpired = true;
        }
        if (scenarioState.displayExpired && scenarioState.flashlightExpired) {
            toRemove.push_back(scenario);
        }
    }
    for (int32_t scenario : toRemove) {
        HILOG_INFO("Removing expired scenario %{public}d", scenario);
        state_.scenarioStates.erase(scenario);
    }
}

void AccessibleBlinkingReminderManager::CleanupBlinking(int32_t flashReminderMode)
{
    if (IsDisplayEnabled(flashReminderMode)) {
        SetDisplayBlinkMode(false);
    }
    if (IsFlashlightEnabled(flashReminderMode)) {
        SetFlashlightMode(state_.originalFlashlightState);
    }
}

void AccessibleBlinkingReminderManager::BlinkingWorkSingle()
{
    HILOG_INFO("Single blinking work start");
    std::unique_lock<std::mutex> lock(mutex_);
    for (int32_t i = 0; i < SINGLE_BLINK_PHASE_COUNT; ++i) {
        if (!state_.isRunning) {
            return;
        }
        int32_t effectiveMode = ComputeEffectiveMode(state_);
        ExecuteBlinkCycle(lock, effectiveMode, i % BLINK_CYCLE_STEP == 0);
        if (cv_.wait_for(lock, std::chrono::milliseconds(BLINK_INTERVAL_MS),
            [this] { return !state_.isRunning; })) {
            return;
        }
    }
    int32_t cleanupMode = state_.flashReminderMode;
    state_.isRunning = false;
    ResetRuntimeState();
    lock.unlock();
    CleanupBlinking(cleanupMode);
}

void AccessibleBlinkingReminderManager::BlinkingWorkContinuous()
{
    HILOG_INFO("Continuous blinking work start");
    bool isOn = true;
    std::unique_lock<std::mutex> lock(mutex_);
    while (state_.isRunning) {
        RemoveExpiredScenarios();
        int32_t effectiveMode = ComputeEffectiveMode(state_);
        if (effectiveMode == FlashReminderMode::NONE || state_.scenarioStates.empty()) {
            break;
        }
        ExecuteBlinkCycle(lock, effectiveMode, isOn);
        isOn = !isOn;
        if (!state_.isRunning) {
            break;
        }
        if (cv_.wait_for(lock, std::chrono::milliseconds(BLINK_INTERVAL_MS),
            [this] { return !state_.isRunning; })) {
            break;
        }
    }
    int32_t cleanupMode = state_.flashReminderMode;
    state_.isRunning = false;
    ResetRuntimeState();
    lock.unlock();
    CleanupBlinking(cleanupMode);
}

void AccessibleBlinkingReminderManager::SaveOriginalFlashlightState()
{
#ifdef OHOS_BUILD_ENABLE_CAMERA
    sptr<CameraStandard::CameraManager> cameraManager = CameraStandard::CameraManager::GetInstance();
    if (!cameraManager) {
        HILOG_ERROR("Failed to get camera manager");
        return;
    }
    state_.originalFlashlightState =
        (cameraManager->GetTorchMode() == CameraStandard::TORCH_MODE_ON);
#endif
}

bool AccessibleBlinkingReminderManager::SetFlashlightMode(bool isOn)
{
#ifdef OHOS_BUILD_ENABLE_CAMERA
    sptr<CameraStandard::CameraManager> cameraManager = CameraStandard::CameraManager::GetInstance();
    if (!cameraManager) {
        HILOG_ERROR("Failed to get camera manager");
        return false;
    }
    auto torchMode = isOn ? CameraStandard::TORCH_MODE_ON : CameraStandard::TORCH_MODE_OFF;
    int32_t ret = cameraManager->SetTorchMode(torchMode);
    if (ret != 0) {
        HILOG_ERROR("SetTorchMode failed, mode=%{public}d, ret=%{public}d", torchMode, ret);
        return false;
    }
#else
    (void)isOn;
#endif
    return true;
}

void AccessibleBlinkingReminderManager::SetDisplayBlinkMode(bool isOn)
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    auto &client = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    double value = isOn ? FORCED_BRIGHTNESS_ON_VALUE : FORCED_BRIGHTNESS_OFF_VALUE;
    client.SetForcedBrightness(value, DEFAULT_DISPLAY_ID, BRIGHTNESS_TRANSITION_DURATION_MS,
        DisplayPowerMgr::BrightnessValueType::RELATIVE_TO_MAX_RANGE);
#else
    (void)isOn;
#endif
}

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
void DisplayBlinkListener::OnDataChanged(const std::string &params)
{
    HILOG_INFO("params=%{public}s, type=%{public}d",
        params.c_str(), static_cast<int32_t>(listenerType_));
    if (manager_ == nullptr) {
        HILOG_ERROR("manager_ is null");
        return;
    }
    if (listenerType_ == DisplayPowerMgr::DisplayDataChangeListenerType::BRIGHTNESS_SLIDER) {
        manager_->HandleBrightnessSliderEvent(params);
    } else if (listenerType_ == DisplayPowerMgr::DisplayDataChangeListenerType::FACE_DETECTION_STATE) {
        manager_->HandleFaceDetectionEvent(params);
    }
}

void AccessibleBlinkingReminderManager::HandleBrightnessSliderEvent(const std::string &params)
{
    bool isDown = params.find("down") != std::string::npos;
    bool isUp = params.find("up") != std::string::npos;
    std::unique_lock<std::mutex> lock(mutex_);
    if (isDown) {
        brightnessAdjusting_ = true;
        ExpireDisplayForAll(lock);
    } else if (isUp) {
        brightnessAdjusting_ = false;
    }
}

void AccessibleBlinkingReminderManager::HandleFaceDetectionEvent(const std::string &params)
{
    bool isEnter = params.find("enter") != std::string::npos;
    bool isExit = params.find("exit") != std::string::npos;
    std::unique_lock<std::mutex> lock(mutex_);
    if (isEnter) {
        faceDetectionAdjusting_ = true;
        ExpireDisplayForAll(lock);
    } else if (isExit) {
        faceDetectionAdjusting_ = false;
    }
}
#endif

void AccessibleBlinkingReminderManager::RegisterDisplayListener()
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    auto &client = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!brightnessListenerRegistered_) {
        displayBlinkListener_ = new DisplayBlinkListener(this,
            DisplayPowerMgr::DisplayDataChangeListenerType::BRIGHTNESS_SLIDER);
        ErrCode err = client.RegisterDataChangeListener(displayBlinkListener_,
            DisplayPowerMgr::DisplayDataChangeListenerType::BRIGHTNESS_SLIDER,
            "Accessibility_BrightnessSlider", "");
        if (err != ERR_OK) {
            HILOG_ERROR("Register BRIGHTNESS_SLIDER failed, err=%{public}d", err);
            displayBlinkListener_ = nullptr;
        } else {
            HILOG_INFO("Register BRIGHTNESS_SLIDER success");
            brightnessListenerRegistered_ = true;
        }
    }
    if (!faceDetectionListenerRegistered_) {
        faceDetectionListener_ = new DisplayBlinkListener(this,
            DisplayPowerMgr::DisplayDataChangeListenerType::FACE_DETECTION_STATE);
        ErrCode err = client.RegisterDataChangeListener(faceDetectionListener_,
            DisplayPowerMgr::DisplayDataChangeListenerType::FACE_DETECTION_STATE,
            "Accessibility_FaceDetection", "");
        if (err != ERR_OK) {
            HILOG_ERROR("Register FACE_DETECTION failed, err=%{public}d", err);
            faceDetectionListener_ = nullptr;
        } else {
            HILOG_INFO("Register FACE_DETECTION success");
            faceDetectionListenerRegistered_ = true;
        }
    }
#endif
}

void AccessibleBlinkingReminderManager::UnregisterDisplayListener()
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    auto &client = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (brightnessListenerRegistered_) {
        client.UnregisterDataChangeListener(
            DisplayPowerMgr::DisplayDataChangeListenerType::BRIGHTNESS_SLIDER,
            "Accessibility_BrightnessSlider");
        displayBlinkListener_ = nullptr;
        brightnessListenerRegistered_ = false;
    }
    if (faceDetectionListenerRegistered_) {
        client.UnregisterDataChangeListener(
            DisplayPowerMgr::DisplayDataChangeListenerType::FACE_DETECTION_STATE,
            "Accessibility_FaceDetection");
        faceDetectionListener_ = nullptr;
        faceDetectionListenerRegistered_ = false;
    }
#endif
}

std::atomic<AccessibleBlinkingReminderManager*>
    AccessibleBlinkingReminderManager::callbackInstance_{nullptr};

void AccessibleBlinkingReminderManager::SubscribeProximitySensor()
{
    if (sensorSubscribed_) {
        return;
    }
    if (strcpy_s(sensorUser_.name, sizeof(sensorUser_.name), "Accessibility") != EOK) {
        HILOG_ERROR("strcpy_s error");
        return;
    }
    sensorUser_.userData = nullptr;
    sensorUser_.callback = OnProximitySensorEvent;
    callbackInstance_.store(this);

    int32_t ret = SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
    if (ret != 0) {
        HILOG_ERROR("SubscribeSensor failed, ret=%{public}d", ret);
        callbackInstance_.store(nullptr);
        return;
    }
    ret = SetBatch(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_,
        PROXIMITY_SENSOR_SAMPLING_PERIOD_NS, PROXIMITY_SENSOR_SAMPLING_PERIOD_NS);
    if (ret != 0) {
        HILOG_ERROR("SetBatch failed, ret=%{public}d", ret);
        UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
        callbackInstance_.store(nullptr);
        return;
    }
    ret = ActivateSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
    if (ret != 0) {
        HILOG_ERROR("ActivateSensor failed, ret=%{public}d", ret);
        UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
        callbackInstance_.store(nullptr);
        return;
    }
    ret = SetMode(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_, SENSOR_ON_CHANGE);
    if (ret != 0) {
        HILOG_ERROR("SetMode failed, ret=%{public}d", ret);
        DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
        UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
        callbackInstance_.store(nullptr);
        return;
    }
    sensorSubscribed_ = true;
}

void AccessibleBlinkingReminderManager::UnsubscribeProximitySensor()
{
    if (!sensorSubscribed_) {
        return;
    }
    DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
    UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY, &sensorUser_);
    sensorSubscribed_ = false;
    callbackInstance_.store(nullptr);
}

void AccessibleBlinkingReminderManager::OnProximitySensorEvent(SensorEvent *event)
{
    if (event == nullptr || event->data == nullptr) {
        return;
    }
    auto *data = reinterpret_cast<ProximityData*>(event->data);
    bool isNear = (data->distance < PROXIMITY_THRESHOLD);
    auto *instance = callbackInstance_.load();
    if (instance != nullptr) {
        instance->HandleProximityStateChange(isNear);
    }
}

void AccessibleBlinkingReminderManager::HandleProximityStateChange(bool isNear)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (isNear && !state_.proximityNear) {
        state_.proximityNear = true;
        if (state_.isRunning && !state_.pausedByProximity) {
            state_.pausedByProximity = true;
            lock.unlock();
            SetDisplayBlinkMode(false);
        }
    } else if (!isNear && state_.proximityNear) {
        state_.proximityNear = false;
        bool anyDisplayActive = false;
        for (const auto &[_, scenarioState] : state_.scenarioStates) {
            if (!scenarioState.displayExpired) {
                anyDisplayActive = true;
                break;
            }
        }
        if (anyDisplayActive && state_.pausedByProximity) {
            state_.pausedByProximity = false;
        }
    }
}

void AccessibleBlinkingReminderManager::ExpireFlashlightForAll(std::unique_lock<std::mutex> &lock)
{
    HILOG_INFO();
    bool anyActive = false;
    for (auto &[_, scenarioState] : state_.scenarioStates) {
        if (!scenarioState.flashlightExpired) {
            scenarioState.flashlightExpired = true;
            anyActive = true;
        }
    }
    if (anyActive) {
        lock.unlock();
        SetFlashlightMode(state_.originalFlashlightState);
        lock.lock();
    }
}

void AccessibleBlinkingReminderManager::ExpireDisplayForAll(std::unique_lock<std::mutex> &lock)
{
    HILOG_INFO();
    bool anyActive = false;
    for (auto &[_, scenarioState] : state_.scenarioStates) {
        if (!scenarioState.displayExpired) {
            scenarioState.displayExpired = true;
            anyActive = true;
        }
    }
    if (anyActive) {
        lock.unlock();
        SetDisplayBlinkMode(false);
        lock.lock();
    }
}

bool AccessibleBlinkingReminderManager::HasAnyActiveComponent()
{
    for (const auto &[_, scenarioState] : state_.scenarioStates) {
        if (!scenarioState.displayExpired || !scenarioState.flashlightExpired) {
            return true;
        }
    }
    return false;
}

void AccessibleBlinkingReminderManager::StopByUnlockDisabled(std::unique_lock<std::mutex> &lock)
{
    if (!state_.isRunning) {
        return;
    }
    if (state_.currentMode == BlinkingMode::SINGLE_BLINK &&
        state_.singleScenario != BlinkingScenario::TESTING) {
        ForceStopBlinkingInternal(lock, state_.flashReminderMode);
    } else if (state_.currentMode == BlinkingMode::CONTINUOUS_BLINK) {
        std::vector<int32_t> toRemove;
        for (const auto &[scenario, _] : state_.scenarioStates) {
            if (scenario != BlinkingScenario::TESTING) {
                toRemove.push_back(scenario);
            }
        }
        for (int32_t scenario : toRemove) {
            state_.scenarioStates.erase(scenario);
        }
        if (state_.scenarioStates.empty()) {
            ForceStopBlinkingInternal(lock, state_.flashReminderMode);
        }
    }
}

BlinkResultCode AccessibleBlinkingReminderManager::ValidateStartBlinking(
    int32_t scenario, int32_t &flashReminderMode, bool &displayBlocked)
{
    if (!state_.flashReminderSwitch) {
        return BlinkResultCode::FEATURE_DISABLE;
    }
    flashReminderMode = state_.flashReminderMode;
    if (flashReminderMode == FlashReminderMode::NONE) {
        return BlinkResultCode::FEATURE_DISABLE;
    }
    if (scenario != BlinkingScenario::TESTING) {
        if (!state_.flashReminderUnlock && !IsScreenLocked()) {
            return BlinkResultCode::FEATURE_DISABLE;
        }
        if (!IsScenarioEnabled(scenario)) {
            return BlinkResultCode::FEATURE_DISABLE;
        }
    }
    if (IsDisplayEnabled(flashReminderMode) && !IsDisplaySupported()) {
        return BlinkResultCode::SCREEN_BLINKING_UNSUPPORTED;
    }
    if (IsFlashlightEnabled(flashReminderMode) && !IsFlashlightSupported()) {
        return BlinkResultCode::FLASH_BLINKING_UNSUPPORTED;
    }
    if (IsDisplayEnabled(flashReminderMode)) {
        bool blocked = state_.proximityNear || brightnessAdjusting_ || faceDetectionAdjusting_;
        if (blocked) {
            if (flashReminderMode == FlashReminderMode::DISPLAY) {
                return BlinkResultCode::DEVICE_IN_USE;
            }
            displayBlocked = true;
        }
    }
    return BlinkResultCode::BLINK_SUCCESS;
}

BlinkResultCode AccessibleBlinkingReminderManager::HandleStartSingleBlink(
    int32_t scenario, int32_t mode, bool displayBlocked)
{
    HILOG_INFO("scenario=%{public}d, mode=%{public}d, displayBlocked=%{public}d",
        scenario, mode, displayBlocked);
    if (state_.isRunning) {
        return BlinkResultCode::ALREADY_FLASHING;
    }
    state_.currentMode = BlinkingMode::SINGLE_BLINK;
    state_.singleScenario = scenario;
    state_.flashReminderMode = mode;
    state_.scenarioStates[scenario] = MakeScenarioState(scenario, mode, displayBlocked);
    state_.isRunning = true;
    blinkingThread_ = std::make_unique<std::thread>(
        &AccessibleBlinkingReminderManager::BlinkingWorkSingle, this);
    return BlinkResultCode::BLINK_SUCCESS;
}

BlinkResultCode AccessibleBlinkingReminderManager::HandleStartContinuousBlink(
    std::unique_lock<std::mutex> &lock, int32_t scenario, int32_t mode, bool displayBlocked)
{
    HILOG_INFO("scenario=%{public}d, mode=%{public}d, displayBlocked=%{public}d",
        scenario, mode, displayBlocked);
    if (state_.isRunning && state_.currentMode == BlinkingMode::SINGLE_BLINK) {
        ForceStopBlinkingInternal(lock, state_.flashReminderMode);
    }
    if (state_.isRunning && state_.currentMode == BlinkingMode::CONTINUOUS_BLINK) {
        if (state_.scenarioStates.find(scenario) != state_.scenarioStates.end()) {
            return BlinkResultCode::BLINK_SUCCESS;
        }
        state_.scenarioStates[scenario] = MakeScenarioState(scenario, mode, displayBlocked);
        return BlinkResultCode::BLINK_SUCCESS;
    }
    state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    state_.flashReminderMode = mode;
    state_.scenarioStates[scenario] = MakeScenarioState(scenario, mode, displayBlocked);
    state_.isRunning = true;
    blinkingThread_ = std::make_unique<std::thread>(
        &AccessibleBlinkingReminderManager::BlinkingWorkContinuous, this);
    return BlinkResultCode::BLINK_SUCCESS;
}

BlinkResultCode AccessibleBlinkingReminderManager::HandleStopSingleBlink(
    std::unique_lock<std::mutex> &lock, int32_t scenario)
{
    HILOG_INFO("scenario=%{public}d", scenario);
    if (state_.currentMode != BlinkingMode::SINGLE_BLINK) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    if (scenario != state_.singleScenario) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    ForceStopBlinkingInternal(lock, state_.flashReminderMode);
    return BlinkResultCode::BLINK_SUCCESS;
}

BlinkResultCode AccessibleBlinkingReminderManager::HandleStopContinuousBlink(
    std::unique_lock<std::mutex> &lock, int32_t scenario)
{
    HILOG_INFO("scenario=%{public}d", scenario);
    if (state_.currentMode != BlinkingMode::CONTINUOUS_BLINK) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    auto it = state_.scenarioStates.find(scenario);
    if (it == state_.scenarioStates.end()) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    state_.scenarioStates.erase(it);
    if (state_.scenarioStates.empty()) {
        ForceStopBlinkingInternal(lock, state_.flashReminderMode);
    }
    return BlinkResultCode::BLINK_SUCCESS;
}

void AccessibleBlinkingReminderManager::SetFlashReminderSwitch(bool state)
{
    HILOG_INFO("state=%{public}d", state);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        state_.flashReminderSwitch = state;
    }
    if (state) {
        SubscribeProximitySensor();
        RegisterDisplayListener();
    } else {
        ForceStopAll();
        UnsubscribeProximitySensor();
        UnregisterDisplayListener();
    }
}

void AccessibleBlinkingReminderManager::SetFlashReminderMode(int32_t mode)
{
    HILOG_INFO("mode=%{public}d", mode);
    std::unique_lock<std::mutex> lock(mutex_);
    int32_t oldMode = state_.flashReminderMode;
    if (state_.isRunning) {
        if (mode == FlashReminderMode::NONE) {
            ForceStopBlinkingInternal(lock, oldMode);
        } else {
            if (!IsFlashlightEnabled(mode)) {
                ExpireFlashlightForAll(lock);
            }
            if (!IsDisplayEnabled(mode)) {
                ExpireDisplayForAll(lock);
            }
            if (!HasAnyActiveComponent()) {
                ForceStopBlinkingInternal(lock, oldMode);
            }
        }
    }
    state_.flashReminderMode = mode;
}

void AccessibleBlinkingReminderManager::SetFlashReminderFunctionEnabled(const std::string &enabled)
{
    HILOG_INFO("enabled=%{public}s", enabled.c_str());
    auto enabledScenarios = ParseEnabledScenarios(enabled);
    std::unique_lock<std::mutex> lock(mutex_);
    state_.flashReminderFunctionEnabled = enabled;
    if (!state_.isRunning) {
        return;
    }
    if (state_.currentMode == BlinkingMode::SINGLE_BLINK &&
        state_.singleScenario != BlinkingScenario::TESTING &&
        enabledScenarios.find(state_.singleScenario) == enabledScenarios.end()) {
        ForceStopBlinkingInternal(lock, state_.flashReminderMode);
    } else if (state_.currentMode == BlinkingMode::CONTINUOUS_BLINK) {
        std::vector<int32_t> toRemove;
        for (const auto &[scenario, _] : state_.scenarioStates) {
            if (scenario != BlinkingScenario::TESTING &&
                enabledScenarios.find(scenario) == enabledScenarios.end()) {
                toRemove.push_back(scenario);
            }
        }
        for (int32_t scenario : toRemove) {
            state_.scenarioStates.erase(scenario);
        }
        if (state_.scenarioStates.empty()) {
            ForceStopBlinkingInternal(lock, state_.flashReminderMode);
        }
    }
}

void AccessibleBlinkingReminderManager::SetFlashReminderUnlock(bool state)
{
    HILOG_INFO("state=%{public}d", state);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        state_.flashReminderUnlock = state;
    }
#ifdef OHOS_BUILD_ENABLE_SCREENLOCK_MANAGER
    if (!state && !IsScreenLocked()) {
        std::unique_lock<std::mutex> lock(mutex_);
        StopByUnlockDisabled(lock);
    }
#else
    (void)state;
#endif
}

BlinkResultCode AccessibleBlinkingReminderManager::StartBlinking(int32_t mode, int32_t scenario)
{
    HILOG_INFO("mode=%{public}d, scenario=%{public}d", mode, scenario);
    std::unique_lock<std::mutex> lock(mutex_);
    int32_t flashReminderMode = FlashReminderMode::NONE;
    bool displayBlocked = false;
    BlinkResultCode ret = ValidateStartBlinking(scenario, flashReminderMode, displayBlocked);
    if (ret != BlinkResultCode::BLINK_SUCCESS) {
        return ret;
    }
    if (!state_.isRunning && IsFlashlightEnabled(flashReminderMode)) {
        SaveOriginalFlashlightState();
    }
    JoinFinishedThread(lock);
    if (mode == BlinkingMode::SINGLE_BLINK) {
        return HandleStartSingleBlink(scenario, flashReminderMode, displayBlocked);
    } else if (mode == BlinkingMode::CONTINUOUS_BLINK) {
        return HandleStartContinuousBlink(lock, scenario, flashReminderMode, displayBlocked);
    }
    return BlinkResultCode::BLINK_SUCCESS;
}

BlinkResultCode AccessibleBlinkingReminderManager::StopBlinking(int32_t mode, int32_t scenario)
{
    HILOG_INFO("mode=%{public}d, scenario=%{public}d", mode, scenario);
    std::unique_lock<std::mutex> lock(mutex_);
    JoinFinishedThread(lock);
    if (!state_.isRunning) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    if (state_.currentMode != mode) {
        return BlinkResultCode::BLINK_SUCCESS;
    }
    if (mode == BlinkingMode::SINGLE_BLINK) {
        return HandleStopSingleBlink(lock, scenario);
    } else if (mode == BlinkingMode::CONTINUOUS_BLINK) {
        return HandleStopContinuousBlink(lock, scenario);
    }
    return BlinkResultCode::BLINK_SUCCESS;
}
} // namespace Accessibility
} // namespace OHOS
