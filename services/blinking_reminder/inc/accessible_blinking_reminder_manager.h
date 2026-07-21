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

#ifndef ACCESSIBLE_BLINKING_REMINDER_MANAGER_H
#define ACCESSIBLE_BLINKING_REMINDER_MANAGER_H

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <memory>
#include <string>
#include <thread>
#include "singleton.h"
#include "accessibility_def.h"
#ifdef OHOS_BUILD_ENABLE_SENSOR
#include "sensor_agent.h"
#endif
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "display_power_mgr_client.h"
#include "display_brightness_listener_stub.h"
#endif

namespace OHOS {
namespace Accessibility {

class AccessibleBlinkingReminderManager;

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
class DisplayBlinkListener : public DisplayPowerMgr::DisplayBrightnessListenerStub {
public:
    explicit DisplayBlinkListener(AccessibleBlinkingReminderManager *manager,
        DisplayPowerMgr::DisplayDataChangeListenerType type)
        : manager_(manager), listenerType_(type) {}
    ~DisplayBlinkListener() override = default;
    void OnDataChanged(const std::string &params) override;
private:
    AccessibleBlinkingReminderManager *manager_ = nullptr;
    DisplayPowerMgr::DisplayDataChangeListenerType listenerType_;
};
#endif

class AccessibleBlinkingReminderManager {
    DECLARE_SINGLETON(AccessibleBlinkingReminderManager);
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    friend class DisplayBlinkListener;
#endif

public:
    void SetFlashReminderSwitch(bool state);
    void SetFlashReminderMode(int32_t mode);
    void SetFlashReminderFunctionEnabled(const std::string &enabled);
    void SetFlashReminderUnlock(bool state);
    BlinkResultCode StartBlinking(int32_t mode, int32_t scenario);
    BlinkResultCode StopBlinking(int32_t mode, int32_t scenario);

private:
    struct ScenarioState {
        int32_t scenario = 0;
        int64_t joinTimeMs = 0;
        int64_t displayStartTimeMs = 0;
        bool displayExpired = false;
        bool flashlightExpired = false;
    };

    struct BlinkState {
        bool flashReminderSwitch = false;
        int32_t flashReminderMode = FlashReminderMode::NONE;
        std::string flashReminderFunctionEnabled;
        bool flashReminderUnlock = false;
        bool isRunning = false;
        int32_t currentMode = 0;
        int32_t singleScenario = 0;
        bool pausedByProximity = false;
        bool proximityNear = false;
        bool originalFlashlightState = false;
        std::map<int32_t, ScenarioState> scenarioStates;
    };

    BlinkState state_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::unique_ptr<std::thread> blinkingThread_;

    bool flashlightSupported_ = false;
    bool flashlightSupportChecked_ = false;
    bool displayBlinkSupported_ = false;
    bool displayBlinkSupportChecked_ = false;

    bool sensorSubscribed_ = false;
#ifdef OHOS_BUILD_ENABLE_SENSOR
    SensorUser sensorUser_ {};
#endif
    bool brightnessListenerRegistered_ = false;
    bool faceDetectionListenerRegistered_ = false;
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    sptr<DisplayPowerMgr::IDisplayBrightnessListener> displayBlinkListener_;
    sptr<DisplayPowerMgr::IDisplayBrightnessListener> faceDetectionListener_;
#endif
    int64_t lastDisplayBusyTimeMs_ = 0;
    bool brightnessAdjusting_ = false;
    bool faceDetectionAdjusting_ = false;

    static std::atomic<AccessibleBlinkingReminderManager*> callbackInstance_;

    static bool IsDisplayEnabled(int32_t mode);
    static bool IsFlashlightEnabled(int32_t mode);
    static int64_t GetCurrentTimeMs();
    static ScenarioState MakeScenarioState(int32_t scenario, int32_t mode, bool displayBlocked);

    bool IsScreenLocked();
    bool IsScenarioEnabled(int32_t scenario);
    bool IsFlashlightSupported();
    bool IsDisplaySupported();

    static int32_t ComputeEffectiveMode(const BlinkState &state);

    void JoinFinishedThread(std::unique_lock<std::mutex> &lock);
    void ForceStopBlinkingInternal(std::unique_lock<std::mutex> &lock, int32_t cleanupMode);
    void ForceStopAll();
    void ResetRuntimeState();

    void BlinkingWorkSingle();
    void BlinkingWorkContinuous();
    void ExecuteBlinkCycle(std::unique_lock<std::mutex> &lock, int32_t mode, bool isOn);
    void CleanupBlinking(int32_t flashReminderMode);
    void RemoveExpiredScenarios();

    bool SetFlashlightMode(bool isOn);
    void SetDisplayBlinkMode(bool isOn);
    void SaveOriginalFlashlightState();

    void SubscribeProximitySensor();
    void UnsubscribeProximitySensor();
    void RegisterDisplayListener();
    void UnregisterDisplayListener();
    static void OnProximitySensorEvent(SensorEvent *event);
    void HandleProximityStateChange(bool isNear);
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    void HandleBrightnessSliderEvent(const std::string &params);
    void HandleFaceDetectionEvent(const std::string &params);
#endif

    void ExpireFlashlightForAll(std::unique_lock<std::mutex> &lock);
    void ExpireDisplayForAll(std::unique_lock<std::mutex> &lock);
    bool HasAnyActiveComponent();
    void StopByUnlockDisabled(std::unique_lock<std::mutex> &lock);

    BlinkResultCode HandleStartSingleBlink(int32_t scenario, int32_t mode, bool displayBlocked);
    BlinkResultCode HandleStartContinuousBlink(std::unique_lock<std::mutex> &lock,
        int32_t scenario, int32_t mode, bool displayBlocked);
    BlinkResultCode HandleStopSingleBlink(std::unique_lock<std::mutex> &lock, int32_t scenario);
    BlinkResultCode HandleStopContinuousBlink(std::unique_lock<std::mutex> &lock, int32_t scenario);

    BlinkResultCode ValidateStartBlinking(int32_t scenario, int32_t &flashReminderMode, bool &displayBlocked);
};

} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_BLINKING_REMINDER_MANAGER_H
