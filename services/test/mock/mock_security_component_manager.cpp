/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "accessibility_security_component_manager.h"
#include "hilog_wrapper.h"
#include "accessibility_element_info.h"
#ifdef ACCESSIBILITY_SECURITY_COMPONENT
#include "sec_comp_enhance_kit.h"
#include "sec_comp_enhance_adapter.h"
#endif // ACCESSIBILITY_SECURITY_COMPONENT

namespace OHOS {
namespace Accessibility {

constexpr uint32_t MAX_HMAC_SIZE = 160;

int32_t MockSecurityComponentManager::SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData)
{
    HILOG_INFO();
#ifdef ACCESSIBILITY_SECURITY_COMPONENT
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::SetEnhanceCfg(rawData.data, rawData.size);
    HILOG_INFO("SetEnhanceCfg result: %{public}d", result);
    return result;
#else
    return RET_OK;
#endif // ACCESSIBILITY_SECURITY_COMPONENT
}

std::map<std::string, std::string> MockSecurityComponentManager::GenerateActionArgumentsWithHMAC(
    const ActionType &action, int64_t uniqueId, std::string bundleName,
    const std::map<std::string, std::string> &arguments)
{
    HILOG_INFO("actionType: %{public}d", action);
#ifndef ACCESSIBILITY_SECURITY_COMPONENT
    return arguments;
#else
    std::map<std::string, std::string> actionArguments(arguments);
    if (action != ACCESSIBILITY_ACTION_CLICK) {
        return actionArguments;
    }

    std::unique_ptr<AccessibilitySecCompPoint> point = std::make_unique<AccessibilitySecCompPoint>();
    if (point == nullptr) {
        HILOG_ERROR("create point failed");
        return actionArguments;
    }

    int64_t timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::string timeStr = std::to_string(timeStamp);

    point->uniqueId = uniqueId;
    errno_t ret = memcpy_s(point->bundleName, MAX_BUNDLE_NAME_LEN, bundleName.c_str(), bundleName.size());
    if (ret != EOK) {
        HILOG_ERROR("point bundleName memcpy_s failed.");
        return actionArguments;
    }
    point->timeStamp = timeStamp;

    uint32_t dataLen = sizeof(*point);
    uint8_t outBuf[MAX_HMAC_SIZE + 1] = { 0 };
    uint8_t *enHanceData = reinterpret_cast<uint8_t *>(&outBuf[0]);
    uint32_t enHanceDataLen = MAX_HMAC_SIZE;
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::GetPointerEventEnhanceData(
        point.get(), dataLen, enHanceData, enHanceDataLen);
    HILOG_INFO("result: %{public}d", result);
    if (result != 0 || enHanceDataLen > MAX_HMAC_SIZE) {
        HILOG_ERROR("GetPointerEventEnhanceData failed!");
        return actionArguments;
    }
    std::vector<uint8_t> vecEnHanceData(enHanceData, enHanceData + enHanceDataLen);
    std::string strEnHanceData(vecEnHanceData.begin(), vecEnHanceData.end());
    actionArguments[ACTION_ARGU_CLICK_ENHANCE_DATA] = strEnHanceData;
    actionArguments[ACTION_ARGU_CLICK_TIMESTAMP] = timeStr;
    return actionArguments;
#endif // ACCESSIBILITY_SECURITY_COMPONENT
}

void MockMockAccessibilitySettingObserver::OnChange()
{
    if (update_) {
        update_(key_);
    }
}

void MockMockAccessibilitySettingObserver::SetKey(const std::string& key)
{
    key_ = key;
}

const std::string& MockMockAccessibilitySettingObserver::GetKey()
{
    return key_;
}

void MockMockAccessibilitySettingObserver::SetUpdateFunc(UpdateFunc& func)
{
    update_ = func;
}

MockSettingProvider::MockSettingProvider()
    : AccessibilityDatashareHelper(DATASHARE_TYPE::GLOBAL, DEFAULT_ACCOUNT_ID)
{
}

MockSettingProvider::~MockSettingProvider()
{
    instance_ = nullptr;
}

std::shared_ptr<MockSettingProvider> MockSettingProvider::GetInstance(int32_t systemAbilityId)
{
    HILOG_DEBUG("etInstance start, systemAbilityId = %{public}d.", systemAbilityId);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = std::make_shared<MockSettingProvider>();
        instance_->Initialize(systemAbilityId);
    }
    return instance_;
}

void MockSettingProvider::DeleteInstance()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        HILOG_INFO("instance_ is nullptr");
        return;
    }
    instance_ = nullptr;
}

RetError MockSettingProvider::GetIntValue(const std::string& key, int32_t& value, const bool readOnlyFlag)
{
    value = AccessibilityDatashareHelper::GetIntValue(key, 0, readOnlyFlag);
    return RET_OK;
}

RetError MockSettingProvider::GetLongValue(const std::string& key, int64_t& value, const bool readOnlyFlag)
{
    value = AccessibilityDatashareHelper::GetLongValue(key, 0, readOnlyFlag);
    return RET_OK;
}

RetError MockSettingProvider::GetFloatValue(const std::string& key, float& value, const bool readOnlyFlag)
{
    value = AccessibilityDatashareHelper::GetFloatValue(key, 0, readOnlyFlag);
    return RET_OK;
}

RetError MockSettingProvider::GetBoolValue(const std::string& key, bool& value, const bool readOnlyFlag)
{
    value = AccessibilityDatashareHelper::GetBoolValue(key, 0, readOnlyFlag);
    return RET_OK;
}

RetError MockSettingProvider::GetStringValue(const std::string& key, std::string& value,
    const bool readOnlyFlag)
{
    value = AccessibilityDatashareHelper::GetStringValue(key, "", readOnlyFlag);
    return RET_OK;
}

RetError MockSettingProvider::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutIntValue(key, value, needNotify);
}

RetError MockSettingProvider::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutLongValue(key, value, needNotify);
}

RetError MockSettingProvider::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutBoolValue(key, value, needNotify);
}

RetError MockSettingProvider::PutStringValue(const std::string& key, const std::string& value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutStringValue(key, value, needNotify);
}

sptr<MockAccessibilitySettingObserver> MockSettingProvider::CreateObserver(const std::string& key,
    MockAccessibilitySettingObserver::UpdateFunc& func)
{
    return AccessibilityDatashareHelper::CreateObserver(key, func);
}

RetError MockSettingProvider::RegisterObserver(const std::string& key,
    MockAccessibilitySettingObserver::UpdateFunc& func)
{
    return AccessibilityDatashareHelper::RegisterObserver(key, func);
}

RetError MockSettingProvider::UnregisterObserver(const std::string& key)
{
    return AccessibilityDatashareHelper::UnregisterObserver(key);
}

MockSettingsConfig::MockSettingsConfig(int32_t id)
{
    HILOG_DEBUG("id = [%{public}d]", id);
    accountId_ = id;
}

RetError MockSettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(ACCESSIBILITY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetEnabled");
        HILOG_ERROR("set accessibility failed");
        return ret;
    }
    enabled_ = state;
    return ret;
}

RetError MockSettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(TOUCH_GUIDE_STATE, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetTouchGuideState");
        HILOG_ERROR("set eventTouchGuideState_ failed");
        return ret;
    }
    eventTouchGuideState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(GESTURE_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetGestureState");
        HILOG_ERROR("set gesturesSimulation_ failed");
        return ret;
    }
    gesturesSimulation_ = state;
    return ret;
}

RetError MockSettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(KEYEVENT_OBSERVER, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetKeyEventObserverState");
        HILOG_ERROR("set filteringKeyEvents_ failed");
        return ret;
    }
    filteringKeyEvents_ = state;
    return ret;
}

RetError MockSettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(CAPTION_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionState");
        HILOG_ERROR("set isCaptionState_ failed");
        return ret;
    }
    isCaptionState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isScreenMagnificationState_ = state;
    return RET_OK;
}

RetError MockSettingsConfig::SetScreenMagnificationType(const uint32_t type)
{
    HILOG_DEBUG("screenMagnificationType = [%{public}u]", type);
    screenMagnificationType_ = type;
    return RET_OK;
}

RetError MockSettingsConfig::SetScreenMagnificationMode(const uint32_t mode)
{
    HILOG_DEBUG("screenMagnificationMode = [%{public}u]", mode);
    screenMagnificationMode_ = mode;
    return RET_OK;
}

RetError MockSettingsConfig::SetScreenMagnificationScale(const float scale)
{
    HILOG_DEBUG("screenMagnificationScale = [%{public}f]", scale);
    screenMagnificationScale_ = scale;
    return RET_OK;
}

RetError MockSettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SHORTCUT_ENABLED, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortKeyState");
        HILOG_ERROR("set isShortKeyState_ failed");
        return ret;
    }
    isShortKeyState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetShortKeyOnLockScreenState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SHORTCUT_ENABLED_ON_LOCK_SCREEN, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortKeyOnLockScreenState");
        HILOG_ERROR("set isShortKeyEnabledOnLockScreen_ failed");
    }
    return ret;
}

RetError MockSettingsConfig::SetShortKeyTimeout(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue(SHORTCUT_TIMEOUT, static_cast<int32_t>(time));
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortKeyTimeout");
        HILOG_ERROR("set shortKeyTimeout_ failed");
        return ret;
    }
    shortKeyTimeout_ = time;
    return ret;
}

RetError MockSettingsConfig::SetStartToHosState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }
    auto ret = datashare_->PutBoolValue("AccessibilityStartToHos", state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetStartToHosState");
        HILOG_ERROR("set startToHosState failed");
    }
    return ret;
}

RetError MockSettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(MOUSEKEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetMouseKeyState");
        HILOG_ERROR("set isMouseKeyState_ failed");
        return ret;
    }
    isMouseKeyState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}d]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue("MouseAutoClick", time);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetMouseAutoClick");
        HILOG_ERROR("set mouseAutoClick_ failed");
        return ret;
    }
    mouseAutoClick_ = time;
    return ret;
}

RetError MockSettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("name = [%{public}s]", name.c_str());
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutStringValue("ShortkeyTarget", name);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortkeyTarget");
        HILOG_ERROR("set shortkeyTarget_ failed");
        return ret;
    }
    shortkeyTarget_ = name;
    return ret;
}

RetError MockSettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG();
    std::set<std::string> targets;
    std::copy_if(name.begin(), name.end(), std::inserter(targets, targets.end()),
        [&targets](const std::string &target) {
            if (targets.find(target) == targets.end()) {
                targets.insert(target);
                return true;
            }
            return false;
        });
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    std::string stringOut = "";
    Utils::VectorToString(std::vector<std::string>(targets.begin(), targets.end()), stringOut);
    auto ret = datashare_->PutStringValue(SHORTCUT_SERVICE, stringOut);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortkeyMultiTarget");
        HILOG_ERROR("set shortkeyMultiTarget_ failed");
        return ret;
    }
    shortkeyMultiTarget_ = std::vector<std::string>(targets.begin(), targets.end());
    return ret;
}

RetError MockSettingsConfig::SetShortkeyMultiTargetInPkgRemove(const std::string &name)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }
    RetError rtn = RET_OK;

    for (auto iter = shortkeyMultiTarget_.begin(); iter != shortkeyMultiTarget_.end(); ++iter) {
        if (*iter == name) {
            shortkeyMultiTarget_.erase(iter);
            std::string stringOut = "";
            Utils::VectorToString(shortkeyMultiTarget_, stringOut);
            rtn = datashare_->PutStringValue(SHORTCUT_SERVICE, stringOut);
            break;
        }
    }
    if (rtn != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortkeyMultiTargetInPkgRemove");
        HILOG_ERROR("set shortkeyMultiTarget_ failed");
        shortkeyMultiTarget_.push_back(name);
    }
    return rtn;
}

RetError MockSettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(HIGH_CONTRAST_TEXT_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetHighContrastTextState");
        HILOG_ERROR("set highContrastTextState_ failed");
        return ret;
    }
    highContrastTextState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(INVERT_COLOR_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetInvertColorState");
        HILOG_ERROR("set invertColorState_ failed");
        return ret;
    }
    invertColorState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(ANIMATION_OFF_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAnimationOffState");
        HILOG_ERROR("set animationOffState_ failed");
        return ret;
    }
    animationOffState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(AUDIO_MONO_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAudioMonoState");
        HILOG_ERROR("set audioMonoState_ failed");
        return ret;
    }
    audioMonoState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(DALTONIZATION_STATE, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetDaltonizationState");
        HILOG_ERROR("set daltonizationState_ failed");
        return ret;
    }
    daltonizationState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("filter = [%{public}u]", filter);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    uint32_t daltonizationColorFilter = filter;
    if (filter == DISPLAY_DALTONIZER_GREEN) {
        daltonizationColorFilter = AccessibilityConfig::Deuteranomaly;
    } else if (filter == DISPLAY_DALTONIZER_RED) {
        daltonizationColorFilter = AccessibilityConfig::Protanomaly;
    } else if (filter == DISPLAY_DALTONIZER_BLUE) {
        daltonizationColorFilter = AccessibilityConfig::Tritanomaly;
    } else if (filter == static_cast<uint32_t>(DISPLAY_DALTONIZER_INVALID)) {
        daltonizationColorFilter = 0;
    }
    auto ret = datashare_->PutIntValue(DALTONIZATION_COLOR_FILTER_KEY, static_cast<int32_t>(daltonizationColorFilter));
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetDaltonizationColorFilter");
        HILOG_ERROR("set daltonizationColorFilter_ failed");
        return ret;
    }
    daltonizationColorFilter_ = daltonizationColorFilter;
    return ret;
}


RetError MockSettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue(CONTENT_TIMEOUT_KEY, static_cast<int32_t>(time));
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetContentTimeout");
        HILOG_ERROR("set contentTimeout_ failed");
        return ret;
    }
    contentTimeout_ = time;
    return ret;
}

RetError MockSettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("discount = [%{public}f]", discount);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutFloatValue(BRIGHTNESS_DISCOUNT_KEY, discount);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetBrightnessDiscount");
        HILOG_ERROR("set brightnessDiscount_ failed");
        return ret;
    }
    brightnessDiscount_ = discount;
    return ret;
}

RetError MockSettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("balance = [%{public}f]", balance);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    float audioBalance = round(balance * AUDIO_BALANCE_STEP) / AUDIO_BALANCE_STEP;
    auto ret = datashare_->PutFloatValue(AUDIO_BALANCE_KEY, audioBalance);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAudioBalance");
        HILOG_ERROR("set audioBalance_ failed");
        return ret;
    }
    audioBalance_ = audioBalance;
    return ret;
}

RetError MockSettingsConfig::SetClickResponseTime(const uint32_t time)
{
    HILOG_DEBUG("clickResponseTime = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    uint32_t clickResponseTime = time;
    if (time == DOUBLE_CLICK_RESPONSE_TIME_MEDIUM) {
        clickResponseTime = AccessibilityConfig::ResponseDelayMedium;
    } else if (time > DOUBLE_CLICK_RESPONSE_TIME_MEDIUM) {
        clickResponseTime = AccessibilityConfig::ResponseDelayLong;
    }
    auto ret = datashare_->PutIntValue(CLICK_RESPONCE_TIME, clickResponseTime);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetClickResponseTime");
        HILOG_ERROR("set clickResponseTime_ failed");
        return ret;
    }
    clickResponseTime_ = clickResponseTime;
    return ret;
}

RetError MockSettingsConfig::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(IGNORE_REPEAT_CLICK_SWITCH, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetIgnoreRepeatClickState");
        HILOG_ERROR("set ignoreRepeatClickState_ failed");
        return ret;
    }
    if (state) {
        uint64_t nowTime = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
        ret = datashare_->PutUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, nowTime);
        IgnoreRepeatClickNotification::RegisterTimers(nowTime);
    } else {
        ret = datashare_->PutUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, 0);
        IgnoreRepeatClickNotification::DestoryTimers();
        IgnoreRepeatClickNotification::CancelNotification();
    }
    ignoreRepeatClickState_ = state;
    return ret;
}

RetError MockSettingsConfig::SetIgnoreRepeatClickReconfirm(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(IGNORE_REPEAT_CLICK_RECONFIRM, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetIgnoreRepeatClickReconfirm");
        HILOG_ERROR("set ignoreRepeatClickReconfirm failed");
        return ret;
    }
    return ret;
}

RetError MockSettingsConfig::SetZoomGestureEnabledReconfirm(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(ZOOM_GESTURE_ENABLED_RECONFIRM, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetZoomGestureEnabledReconfirm");
        HILOG_ERROR("set zoomGestureEnabledReconfirm failed");
        return ret;
    }
    return ret;
}

RetError MockSettingsConfig::SetColorModeState(const A11yDarkModeType &type)
{
    uint32_t darkModeValue = static_cast<uint32_t>(type);
    HILOG_DEBUG("name = [%{public}d]", darkModeValue);
    if (!systemDatashare_) {
        HILOG_ERROR("systemDatashare_ is nullptr");
        return RET_ERR_NULLPTR;
    }
 
    auto ret = systemDatashare_->PutStringValue("settings.uiappearance.darkmode_mode",
        std::to_string(darkModeValue), true);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetColorModeState");
        HILOG_ERROR("set SetColorModeState failed");
    }
    return ret;
}

RetError MockSettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG("ignoreRepeatClickTime = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    uint32_t ignoreRepeatClickTime = time;
    if (time == DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORTEST) {
        ignoreRepeatClickTime = AccessibilityConfig::RepeatClickTimeoutShortest;
    } else if (time == DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT) {
        ignoreRepeatClickTime = AccessibilityConfig::RepeatClickTimeoutShort;
    } else if (time == DOUBLE_IGNORE_REPEAT_CLICK_TIME_MEDIUM) {
        ignoreRepeatClickTime = AccessibilityConfig::RepeatClickTimeoutMedium;
    } else if (time == DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG) {
        ignoreRepeatClickTime = AccessibilityConfig::RepeatClickTimeoutLong;
    } else if (time > DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG) {
        ignoreRepeatClickTime = AccessibilityConfig::RepeatClickTimeoutLongest;
    }
    auto ret = datashare_->PutIntValue(IGNORE_REPEAT_CLICK_TIME, ignoreRepeatClickTime);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetIgnoreRepeatClickTime");
        HILOG_ERROR("set ignoreRepeatClickTime_ failed");
        return ret;
    }
    ignoreRepeatClickTime_ = ignoreRepeatClickTime;
    return ret;
}

RetError MockSettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG();
    captionProperty_ = caption;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    auto fontFamilyRet = datashare_->PutStringValue(FONT_FAMILY, captionProperty_.GetFontFamily());
    if (fontFamilyRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.fontFamily");
    }
    auto fontColorRet = datashare_->PutIntValue(FONT_COLOR, static_cast<int32_t>(captionProperty_.GetFontColor()));
    if (fontColorRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.fontColor");
    }
    auto fontEdgeTypeRet = datashare_->PutStringValue(FONT_EDGE_TYPE, captionProperty_.GetFontEdgeType());
    if (fontEdgeTypeRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.fontEdgeType");
    }
    auto backgroundColorRet = datashare_->PutIntValue(BACKGROUND_COLOR, static_cast<int32_t>(
        captionProperty_.GetBackgroundColor()));
    if (backgroundColorRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.backgroundColor");
    }
    auto windowColorRet = datashare_->PutIntValue(WINDOW_COLOR, static_cast<int32_t>(
        captionProperty_.GetWindowColor()));
    if (windowColorRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.windowColor");
    }
    auto fontScaleRet = datashare_->PutIntValue(FONT_SCALE, captionProperty_.GetFontScale());
    if (fontScaleRet != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionProperty.fontScale");
    }
    return RET_OK;
}

bool MockSettingsConfig::GetCaptionState() const
{
    return isCaptionState_;
}

bool MockSettingsConfig::GetScreenMagnificationState() const
{
    return isScreenMagnificationState_;
}

bool MockSettingsConfig::GetShortKeyState() const
{
    return isShortKeyState_;
}

bool MockSettingsConfig::GetShortKeyOnLockScreenState() const
{
    if (!datashare_) {
        HILOG_ERROR("datashare is nullptr!");
        return false;
    }
    return datashare_->GetBoolValue(SHORTCUT_ENABLED_ON_LOCK_SCREEN, false);
}

int32_t MockSettingsConfig::GetShortKeyTimeout() const
{
    return shortKeyTimeout_;
}

bool MockSettingsConfig::GetMouseKeyState() const
{
    return isMouseKeyState_;
}

int32_t MockSettingsConfig::GetMouseAutoClick() const
{
    return mouseAutoClick_;
}

const std::string &MockSettingsConfig::GetShortkeyTarget() const
{
    return shortkeyTarget_;
}

const std::vector<std::string> MockSettingsConfig::GetShortkeyMultiTarget()
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    std::vector<std::string> rtnVec = shortkeyMultiTarget_;
    return rtnVec;
}

bool MockSettingsConfig::GetHighContrastTextState() const
{
    return highContrastTextState_;
}

bool MockSettingsConfig::GetInvertColorState() const
{
    return invertColorState_;
}

bool MockSettingsConfig::GetAnimationOffState() const
{
    return animationOffState_;
}

bool MockSettingsConfig::GetAudioMonoState() const
{
    return audioMonoState_;
}

bool MockSettingsConfig::GetDaltonizationState() const
{
    return daltonizationState_;
}

uint32_t MockSettingsConfig::GetDaltonizationColorFilter() const
{
    return daltonizationColorFilter_;
}

uint32_t MockSettingsConfig::GetContentTimeout() const
{
    return contentTimeout_;
}

float MockSettingsConfig::GetBrightnessDiscount() const
{
    return brightnessDiscount_;
}

float MockSettingsConfig::GetAudioBalance() const
{
    return audioBalance_;
}

bool MockSettingsConfig::GetEnabledState() const
{
    return enabled_;
}

bool MockSettingsConfig::GetTouchGuideState() const
{
    return eventTouchGuideState_;
}

bool MockSettingsConfig::GetGestureState() const
{
    return gesturesSimulation_;
}

bool MockSettingsConfig::GetKeyEventObserverState() const
{
    return filteringKeyEvents_;
}

const AccessibilityConfig::CaptionProperty &MockSettingsConfig::GetCaptionProperty() const
{
    return captionProperty_;
};

uint32_t MockSettingsConfig::GetClickResponseTime() const
{
    return clickResponseTime_;
}

uint32_t MockSettingsConfig::GetScreenMagnificationType() const
{
    return screenMagnificationType_;
}

uint32_t MockSettingsConfig::GetScreenMagnificationMode() const
{
    return screenMagnificationMode_;
}

float MockSettingsConfig::GetScreenMagnificationScale() const
{
    return screenMagnificationScale_;
}

bool MockSettingsConfig::GetIgnoreRepeatClickState() const
{
    return ignoreRepeatClickState_;
}

uint32_t MockSettingsConfig::GetIgnoreRepeatClickTime() const
{
    return ignoreRepeatClickTime_;
}

RetError MockSettingsConfig::SetEnabledAccessibilityServices(const std::vector<std::string> &services)
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    enabledAccessibilityServices_ = services;
    if (datashare_ == nullptr) {
        HILOG_WARN("datashare_ is null.");
        return RET_ERR_NULLPTR;
    }
    std::string stringOut = "";
    Utils::VectorToString(enabledAccessibilityServices_, stringOut);
    auto ret = datashare_->PutStringValue(ENABLED_ACCESSIBILITY_SERVICES, stringOut);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetEnabledAccessibilityServices");
        HILOG_ERROR("set enabledAccessibilityServices failed");
    }
    return ret;
}

const std::vector<std::string> MockSettingsConfig::GetEnabledAccessibilityServices()
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    std::vector<std::string> rtnVec = enabledAccessibilityServices_;
    return rtnVec;
}

RetError MockSettingsConfig::AddEnabledAccessibilityService(const std::string &serviceName)
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    auto iter = std::find(enabledAccessibilityServices_.begin(), enabledAccessibilityServices_.end(), serviceName);
    if (iter != enabledAccessibilityServices_.end()) {
        return RET_OK;
    }

    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    enabledAccessibilityServices_.push_back(serviceName);
    std::string stringOut = "";
    Utils::VectorToString(enabledAccessibilityServices_, stringOut);
    return datashare_->PutStringValue(ENABLED_ACCESSIBILITY_SERVICES, stringOut);
}

RetError MockSettingsConfig::RemoveEnabledAccessibilityService(const std::string &serviceName)
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    auto iter = std::find(enabledAccessibilityServices_.begin(), enabledAccessibilityServices_.end(), serviceName);
    if (iter == enabledAccessibilityServices_.end()) {
        return RET_OK;
    }

    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    enabledAccessibilityServices_.erase(iter);
    std::string stringOut = "";
    Utils::VectorToString(enabledAccessibilityServices_, stringOut);
    return datashare_->PutStringValue(ENABLED_ACCESSIBILITY_SERVICES, stringOut);
}

bool MockSettingsConfig::GetStartToHosState()
{
    HILOG_DEBUG();
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    bool value = true;
    value = datashare_->GetBoolValue("AccessibilityStartToHos", true);
    return value;
}

uint32_t MockSettingsConfig::GetConfigState()
{
    HILOG_DEBUG();
    uint32_t state = 0;
    if (isCaptionState_) {
        state |= STATE_CAPTION_ENABLED;
    }

    if (isScreenMagnificationState_) {
        state |= STATE_SCREENMAGNIFIER_ENABLED;
    }

    if (isMouseKeyState_) {
        state |= STATE_MOUSEKEY_ENABLED;
    }

    if (isShortKeyState_) {
        state |= STATE_SHORTKEY_ENABLED;
    }

    if (highContrastTextState_) {
        state |= STATE_HIGHCONTRAST_ENABLED;
    }

    if (daltonizationState_) {
        state |= STATE_DALTONIZATION_STATE_ENABLED;
    }

    if (invertColorState_) {
        state |= STATE_INVETRTCOLOR_ENABLED;
    }

    if (animationOffState_) {
        state |= STATE_ANIMATIONOFF_ENABLED;
    }

    if (audioMonoState_) {
        state |= STATE_AUDIOMONO_ENABLED;
    }

    if (ignoreRepeatClickState_) {
        state |= STATE_IGNORE_REPEAT_CLICK_ENABLED;
    }
    return state;
}

void MockSettingsConfig::InitCaption()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }

    std::string strValue = datashare_->GetStringValue(CAPTION_KEY, "");
    HILOG_DEBUG(" pref_->GetString() = %{public}s.", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isCaptionState_ = true;
    } else {
        isCaptionState_ = false;
    }

    std::string fontFamliy = datashare_->GetStringValue(FONT_FAMILY, "default");
    HILOG_DEBUG("fontFamily = %{public}s.", fontFamliy.c_str());

    int32_t fontScale =  static_cast<int32_t>(datashare_->GetIntValue(FONT_SCALE, DEFAULT_FONT_SCALE));
    HILOG_DEBUG("fontScale = %{public}d.", fontScale);

    uint32_t fontColor = static_cast<uint32_t>(datashare_->GetIntValue(FONT_COLOR, DEFAULT_COLOR));
    HILOG_DEBUG("fontColor = 0x%{public}x.", fontColor);

    std::string fontEdgeType = datashare_->GetStringValue(FONT_EDGE_TYPE, "none");
    HILOG_DEBUG("fontEdgeType = 0x%{public}s.", fontEdgeType.c_str());

    uint32_t backgroundColor = static_cast<uint32_t>(datashare_->GetIntValue(BACKGROUND_COLOR, DEFAULT_COLOR));
    HILOG_DEBUG("backgroundColor = 0x%{public}x.", backgroundColor);

    uint32_t windowColor = static_cast<uint32_t>(datashare_->GetIntValue(WINDOW_COLOR, DEFAULT_COLOR));
    HILOG_DEBUG("windowColor = 0x%{public}x.", windowColor);

    captionProperty_.SetFontFamily(fontFamliy);
    captionProperty_.SetFontScale(fontScale);
    captionProperty_.SetFontColor(fontColor);
    captionProperty_.SetFontEdgeType(fontEdgeType);
    captionProperty_.SetBackgroundColor(backgroundColor);
    captionProperty_.SetWindowColor(windowColor);
}

void MockSettingsConfig::InitShortKeyConfig()
{
    isShortKeyState_ = datashare_->GetBoolValue(SHORTCUT_ENABLED, true);
    bool isShortKeyEnabledOnLockScreen = datashare_->GetBoolValue(SHORTCUT_ENABLED_ON_LOCK_SCREEN, false);
    int isShortKeyOnLockScreen = datashare_->GetIntValue(SHORTCUT_ON_LOCK_SCREEN,
        INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    shortKeyTimeout_ = static_cast<int32_t>(datashare_->GetIntValue(SHORTCUT_TIMEOUT, SHORT_KEY_TIMEOUT_BEFORE_USE));
    bool shortKeyOnLockScreenAutoOn = false;

    if (shortKeyTimeout_ == 1) {
        SetShortKeyTimeout(SHORT_KEY_TIMEOUT_AFTER_USE);
        if (isShortKeyOnLockScreen == INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE) {
            shortKeyOnLockScreenAutoOn = true;
            SetShortKeyOnLockScreenState(true);
        }
    } else if (shortKeyTimeout_ == 0) {
        SetShortKeyTimeout(SHORT_KEY_TIMEOUT_BEFORE_USE);
    }

    if (isShortKeyOnLockScreen != INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE) {
        SetShortKeyOnLockScreenState(isShortKeyOnLockScreen == 1);
    } else if (!shortKeyOnLockScreenAutoOn) {
        SetShortKeyOnLockScreenState(isShortKeyEnabledOnLockScreen);
    }

    auto ret = datashare_->PutIntValue(SHORTCUT_ON_LOCK_SCREEN, INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    HILOG_INFO("reset shortcut on lock screen ret = %{public}d", ret);

    shortkeyTarget_ = datashare_->GetStringValue("ShortkeyTarget", "none");

    std::string tmpString = datashare_->GetStringValue(SHORTCUT_SERVICE, SCREEN_READER_BUNDLE_ABILITY_NAME);
    shortkeyMultiTarget_ = {};
    Utils::StringToVector(tmpString, shortkeyMultiTarget_);

    bool isScreenReaderEnabledOriginal =
        (std::find(enabledAccessibilityServices_.begin(), enabledAccessibilityServices_.end(),
        SCREEN_READER_BUNDLE_ABILITY_NAME) != enabledAccessibilityServices_.end());
    tmpString = datashare_->GetStringValue(ENABLED_ACCESSIBILITY_SERVICES, "");
    enabledAccessibilityServices_ = {};
    Utils::StringToVector(tmpString, enabledAccessibilityServices_);
    CloneShortkeyService(isScreenReaderEnabledOriginal);
}

void MockSettingsConfig::InitPrivacySpaceConfig()
{
    // Initialization of the private space after cloning or upgrade
    std::shared_ptr<AccessibilitySettingProvider> service = AccessibilitySettingProvider::GetInstance(
        POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    bool cloneOrUpgradeFlag = false;
    service->GetBoolValue(ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE, cloneOrUpgradeFlag);
    if (cloneOrUpgradeFlag && (accountId_ != DEFAULT_ACCOUNT_ID)) {
        SetDefaultShortcutKeyService();
        SetIgnoreRepeatClickReconfirm(ignoreRepeatClickState_);
        SetZoomGestureEnabledReconfirm(isScreenMagnificationState_);
        service->PutBoolValue(ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE, false);
    }
}

void MockSettingsConfig::InitAnimationOffConfig()
{
    animationOffState_ = datashare_->GetBoolValue(ANIMATION_OFF_KEY, false);
    std::string graphicState = system::GetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "1");
    std::string arkuiState = system::GetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "1");
    bool state = (graphicState == "0" && arkuiState == "0");
    if (animationOffState_) {
        system::SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "0");
        system::SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "0");
    } else if (!animationOffState_ && state) {
        system::SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "1");
        system::SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "1");
    }
}

void MockSettingsConfig::InitSetting()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }
    IgnoreRepeatClickNotification::CancelNotification();
    
    InitShortKeyConfig();
    InitPrivacySpaceConfig();
    InitAnimationOffConfig();
    CloneAudioState();
    isScreenMagnificationState_ = datashare_->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false);
    isMouseKeyState_= datashare_->GetBoolValue(MOUSEKEY, false);
    invertColorState_ = datashare_->GetBoolValue(INVERT_COLOR_KEY, false);
    highContrastTextState_ = datashare_->GetBoolValue(HIGH_CONTRAST_TEXT_KEY, false);
    daltonizationState_ = datashare_->GetBoolValue(DALTONIZATION_STATE, false);
    audioMonoState_ = datashare_->GetBoolValue(AUDIO_MONO_KEY, false);
    ignoreRepeatClickState_ = datashare_->GetBoolValue(IGNORE_REPEAT_CLICK_SWITCH, false);
    mouseAutoClick_ = static_cast<int32_t>(datashare_->GetIntValue("MouseAutoClick", -1));
    daltonizationColorFilter_ = static_cast<uint32_t>(datashare_->GetIntValue(DALTONIZATION_COLOR_FILTER_KEY, 0));
    SetDaltonizationColorFilter(daltonizationColorFilter_);
    contentTimeout_ = static_cast<uint32_t>(datashare_->GetIntValue(CONTENT_TIMEOUT_KEY, 0));
    brightnessDiscount_ = static_cast<float>(datashare_->GetFloatValue(BRIGHTNESS_DISCOUNT_KEY, 1.0));
    audioBalance_ = static_cast<float>(datashare_->GetFloatValue(AUDIO_BALANCE_KEY, 0));
    SetAudioBalance(audioBalance_);
    screenMagnificationType_ = static_cast<uint32_t>(
        datashare_->GetIntValue(SCREEN_MAGNIFICATION_TYPE, FULL_SCREEN_MAGNIFICATION));
    screenMagnificationMode_ = static_cast<uint32_t>(
        datashare_->GetIntValue(SCREEN_MAGNIFICATION_MODE, FULL_SCREEN_MAGNIFICATION));
    screenMagnificationScale_ = static_cast<float>(
        datashare_->GetFloatValue(SCREEN_MAGNIFICATION_SCALE, DEFAULT_MAGNIFICATION_SCALE));
    clickResponseTime_ = static_cast<uint32_t>(datashare_->GetIntValue(CLICK_RESPONCE_TIME, 0));
    SetClickResponseTime(clickResponseTime_);
    ignoreRepeatClickTime_ = static_cast<uint32_t>(datashare_->GetIntValue(IGNORE_REPEAT_CLICK_TIME, 0));
    SetIgnoreRepeatClickTime(ignoreRepeatClickTime_);
    datashare_->GetStringValue(FLASH_REMINDER_SWITCH_KEY, "0");
    datashare_->GetStringValue(FLASH_REMINDER_ENABLED, "DEFAULT");
    datashare_->GetBoolValue(VOICE_RECOGNITION_KEY, false);
    datashare_->GetStringValue(VOICE_RECOGNITION_TYPES, "DEFAULT");

    uint64_t recoveryDate = datashare_->GetUnsignedLongValue(RECOVERY_IGNORE_REPEAT_CLICK_DATE, 0);
    if (ignoreRepeatClickState_ && recoveryDate == 0 &&
        (ignoreRepeatClickTime_ == IGNORE_REPEAT_CLICK_SHORTEST ||
            ignoreRepeatClickTime_ == IGNORE_REPEAT_CLICK_SHORT)) {
        ignoreRepeatClickState_ = false;
        SetIgnoreRepeatClickState(false);
        recoveryDate = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
        datashare_->PutUnsignedLongValue(RECOVERY_IGNORE_REPEAT_CLICK_DATE, recoveryDate);
        HILOG_INFO("recovery ignore repeat click %{public}" PRIu64, recoveryDate);
    }
 
    if (ignoreRepeatClickState_) {
        IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
        uint64_t timeStamp = datashare_->GetUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, 0);
        if (timeStamp == 0) {
            timeStamp = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
            datashare_->PutUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, timeStamp);
        }
        IgnoreRepeatClickNotification::RegisterTimers(timeStamp);
    }
}

void MockSettingsConfig::InitCapability()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }

    enabled_ = datashare_->GetBoolValue(ACCESSIBILITY, false);
    eventTouchGuideState_ = datashare_->GetBoolValue(TOUCH_GUIDE_STATE, false);
    gesturesSimulation_ = datashare_->GetBoolValue(GESTURE_KEY, false);
    filteringKeyEvents_ = datashare_->GetBoolValue(KEYEVENT_OBSERVER, false);
}

RetError MockSettingsConfig::SetConfigState(const std::string& key, bool value)
{
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    auto ret = datashare_->PutBoolValue(key, value);
    if (ret != RET_OK) {
        HILOG_ERROR("set configState failed");
    }
    return ret;
}

void MockSettingsConfig::Init()
{
    HILOG_DEBUG();
    datashare_ = std::make_shared<AccessibilityDatashareHelper>(DATASHARE_TYPE::SECURE, accountId_);
    if (datashare_ == nullptr) {
        return;
    }
    RetError ret = datashare_->Initialize(POWER_MANAGER_SERVICE_ID);
    InitCaption();
    InitSetting();

    systemDatashare_ = std::make_shared<AccessibilityDatashareHelper>(DATASHARE_TYPE::SYSTEM, accountId_);
    if (systemDatashare_ == nullptr) {
        return;
    }
    RetError systemRet = systemDatashare_->Initialize(POWER_MANAGER_SERVICE_ID);
    if (ret == RET_OK && systemRet == RET_OK) {
        isInitialized_ = true;
    }
}

void MockSettingsConfig::ClearData()
{
    HILOG_DEBUG();
}

void MockSettingsConfig::CloneAudioState()
{
    HILOG_DEBUG();
    if (systemDatashare_ == nullptr) {
        return;
    }

    RetError ret = RET_OK;
    int32_t monoValue = static_cast<int32_t>(systemDatashare_->GetIntValue(AUDIO_MONO_KEY, INVALID_MASTER_MONO_VALUE));
    if (monoValue != INVALID_MASTER_MONO_VALUE) {
        SetAudioMonoState(monoValue == 1);
        ret = systemDatashare_->PutIntValue(AUDIO_MONO_KEY, INVALID_MASTER_MONO_VALUE);
        if (ret != RET_OK) {
            Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "CloneAudioState.monoValue");
            HILOG_ERROR("reset monoValue in system table failed");
        }
    }

    float audioBalance = static_cast<float>(systemDatashare_->GetFloatValue(AUDIO_BALANCE_KEY,
        INVALID_MASTER_BALANCE_VALUE));
    if (abs(audioBalance - INVALID_MASTER_BALANCE_VALUE) > EPS) {
        SetAudioBalance(audioBalance);
        ret = systemDatashare_->PutFloatValue(AUDIO_BALANCE_KEY, INVALID_MASTER_BALANCE_VALUE);
        if (ret != RET_OK) {
            Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "CloneAudioState.audioBalance");
            HILOG_ERROR("reset audioBalance in system table failed");
        }
    }
}

uint32_t MockSettingsConfig::GetShortKeyService(std::vector<std::string> &services)
{
    uint32_t serviceFlag = 0;

    auto screenReader = std::find_if(services.begin(), services.end(), [&](const std::string& service) {
        return service.find(SCREENREADER_TAG) != std::string::npos;
    });
    serviceFlag = screenReader != services.end() ? STATE_EXPLORATION_ENABLED : serviceFlag;

    auto invertColor = std::find_if(services.begin(), services.end(), [&](const std::string& service) {
        return service.find(INVERT_COLOR_AOS_TAG) != std::string::npos ||
            service.find(INVERT_COLOR_HMOS_TAG) != std::string::npos;
    });
    serviceFlag = invertColor != services.end() ? (serviceFlag | STATE_INVETRTCOLOR_ENABLED) : serviceFlag;

    auto audioMono = std::find_if(services.begin(), services.end(), [&](const std::string& service) {
        return service.find(AUDIO_MONO_HMOS_TAG) != std::string::npos;
    });
    serviceFlag = audioMono != services.end() ? (serviceFlag | STATE_AUDIOMONO_ENABLED) : serviceFlag;

    auto highContrastText = std::find_if(services.begin(), services.end(), [&](const std::string& service) {
        return service.find(HIGH_CONTRAST_TEXT_HMOS_TAG) != std::string::npos;
    });
    serviceFlag = highContrastText != services.end() ? (serviceFlag | STATE_HIGHCONTRAST_ENABLED) : serviceFlag;

    return serviceFlag;
}

void MockSettingsConfig::CloneShortkeyService(bool isScreenReaderEnabled)
{
    std::vector<std::string> tmpVec = GetShortkeyMultiTarget();
    uint32_t shortkeyServiceFlag = GetShortKeyService(tmpVec);
    std::vector<std::string> shortkeyService;
    if (shortkeyServiceFlag & STATE_EXPLORATION_ENABLED) {
        shortkeyService.push_back(SCREEN_READER_BUNDLE_ABILITY_NAME);
    }
    if (shortkeyServiceFlag & STATE_INVETRTCOLOR_ENABLED) {
        shortkeyService.push_back(INVERT_COLOR_HMOS_TAG);
    }
    if (shortkeyServiceFlag & STATE_AUDIOMONO_ENABLED) {
        shortkeyService.push_back(AUDIO_MONO_HMOS_TAG);
    }
    if (shortkeyServiceFlag & STATE_HIGHCONTRAST_ENABLED) {
        shortkeyService.push_back(HIGH_CONTRAST_TEXT_HMOS_TAG);
    }
    SetShortkeyMultiTarget(shortkeyService);

    tmpVec = GetEnabledAccessibilityServices();
    shortkeyServiceFlag = GetShortKeyService(tmpVec);
    std::vector<std::string> enabledShortkeyService;
    if ((shortkeyServiceFlag & STATE_EXPLORATION_ENABLED) || (isScreenReaderEnabled == true)) {
        enabledShortkeyService.push_back(SCREEN_READER_BUNDLE_ABILITY_NAME);
    }
    SetEnabledAccessibilityServices(enabledShortkeyService);
}

void MockSettingsConfig::SetDefaultShortcutKeyService()
{
    HILOG_DEBUG();

    if (GetShortkeyMultiTarget().empty()) {
        HILOG_INFO("set default shortcut key service.");
        std::vector<std::string> defaultService;
        defaultService.push_back(SCREEN_READER_BUNDLE_ABILITY_NAME);
        SetShortkeyMultiTarget(defaultService);
    }
}

void MockSettingsConfig::OnDataClone()
{
    HILOG_INFO();

    InitSetting();
    SetDefaultShortcutKeyService();
    SetIgnoreRepeatClickReconfirm(ignoreRepeatClickState_);
    SetZoomGestureEnabledReconfirm(isScreenMagnificationState_);
    
    bool isShortkeyEnabled = GetShortKeyState();
    bool isShortkeyEnabledOnLockScreen = GetShortKeyOnLockScreenState();
    SetShortKeyState(!isShortkeyEnabled);
    SetShortKeyState(isShortkeyEnabled);
    SetShortKeyOnLockScreenState(!isShortkeyEnabledOnLockScreen);
    SetShortKeyOnLockScreenState(isShortkeyEnabledOnLockScreen);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateShortKeyRegister();

    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    bool isScreenReaderEnabled =
        (std::find(enabledAccessibilityServices_.begin(), enabledAccessibilityServices_.end(),
        SCREEN_READER_BUNDLE_ABILITY_NAME) != enabledAccessibilityServices_.end());
    if (isScreenReaderEnabled) {
        ErrCode ret = service->PutBoolValue(ACCESSIBILITY_SCREENREADER_ENABLED, true, true);
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        accountData->screenReaderState_ = true;
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "OnDataClone.screenReader");
        HILOG_INFO("set screenReader state, ret = %{public}d", ret);
    }
    service->PutBoolValue(ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE, true);
    service->PutBoolValue(ACCESSIBILITY_CLONE_FLAG, false);
}

bool MockSettingsConfig::GetInitializeState()
{
    return isInitialized_;
}

void MockSettingsConfig::SetInitializeState(bool isInitialized)
{
    isInitialized_ = isInitialized;
}

void ShortkeyAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("on ability connected");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(SHORTKEY_DIALOG_PARAM_NUM);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.ohos.settings");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"MockShortkeyDialog");
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(GetCommandString()));

    if (!data.WriteParcelable(&element)) {
        HILOG_ERROR("Connect done element error.");
        return;
    }

    if (!data.WriteRemoteObject(remoteObject)) {
        HILOG_ERROR("Connect done remote object error.");
        return;
    }

    if (!data.WriteInt32(resultCode)) {
        HILOG_ERROR("Connect done result code error.");
        return;
    }

    int32_t errCode = remoteObject->SendRequest(
        AAFwk::IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    HILOG_DEBUG("AbilityConnectionWrapperProxy::OnAbilityConnectDone result %{public}d", errCode);
}

void ShortkeyAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    HILOG_DEBUG("on ability disconnected");
}

std::string ShortkeyAbilityConnection::GetCommandString()
{
    return commandStr_;
}

void ReConfirmAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("on ability connected");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(SHORTKEY_DIALOG_PARAM_NUM);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.ohos.settings");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"AccessibilityReConfirmDialog");
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(GetCommandString()));

    if (!data.WriteParcelable(&element)) {
        HILOG_ERROR("Connect done element error.");
        return;
    }

    if (!data.WriteRemoteObject(remoteObject)) {
        HILOG_ERROR("Connect done remote object error.");
        return;
    }

    if (!data.WriteInt32(resultCode)) {
        HILOG_ERROR("Connect done result code error.");
        return;
    }

    int32_t errCode = remoteObject->SendRequest(
        AAFwk::IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    HILOG_DEBUG("AbilityConnectionWrapperProxy::OnAbilityConnectDone result %{public}d", errCode);
}

void ReConfirmAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    HILOG_DEBUG("on ability disconnected");
}

std::string ReConfirmAbilityConnection::GetCommandString()
{
    return commandStr_;
}

// dialog
MockShortkeyDialog::MockShortkeyDialog() {}

MockShortkeyDialog::~MockShortkeyDialog()
{
    HILOG_DEBUG("release MockShortkeyDialog");
}

bool MockShortkeyDialog::ConnectDialog(ShortKeyDialogType dialogType)
{
    HILOG_DEBUG("MockShortkeyDialog dialog");
    if (!ConnectExtension(dialogType)) {
        HILOG_ERROR("failed to connect dialog.");
        return false;
    }
    return true;
}

bool MockShortkeyDialog::ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr,
    ShortKeyDialogType dialogType)
{
    // reset current callingIdentify
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int32_t ret = ERR_OK;

    if (dialogType == ShortKeyDialogType::FUNCTION_SELECT) {
        functionSelectConn_ = new(std::nothrow) ShortkeyAbilityConnection(commandStr);
        if (functionSelectConn_ == nullptr) {
            HILOG_ERROR("connection_ is nullptr.");
            return false;
        }
        ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
            functionSelectConn_, nullptr, DEFAULT_VALUE_MINUS_ONE);
    } else {
        reConfirmConn_ = new(std::nothrow) ReConfirmAbilityConnection(commandStr);
        if (reConfirmConn_ == nullptr) {
            HILOG_ERROR("connection_ is nullptr.");
            return false;
        }
        ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
            reConfirmConn_, nullptr, DEFAULT_VALUE_MINUS_ONE);
    }
    HILOG_DEBUG("ret is: %{public}d.", ret);
    // set current callingIdentify back.
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        HILOG_ERROR("ret isn't ERR_OK");
        return false;
    }
    return true;
}

bool MockShortkeyDialog::ConnectExtension(ShortKeyDialogType dialogType)
{
    std::string tmp = BuildStartCommand();
    HILOG_DEBUG("start command: %{public}s", tmp.c_str());

    AAFwk::Want want;
    std::string bundleName = "com.ohos.systemui";
    std::string abilityName = "com.ohos.systemui.dialog";
    want.SetElementName(bundleName, abilityName);
    bool ret = ConnectExtensionAbility(want, tmp, dialogType);
    if (!ret) {
        HILOG_ERROR("ConnectExtensionAbility failed.");
        return false;
    }
    HILOG_DEBUG("ConnectExtensionAbility successed.");
    return true;
}

bool MockShortkeyDialog::DisconnectExtension(ShortKeyDialogType dialogType) const
{
    if (dialogType == ShortKeyDialogType::FUNCTION_SELECT) {
        if (functionSelectConn_ == nullptr) {
            return true;
        }
        ErrCode ret = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(functionSelectConn_);
        if (ret != ERR_OK) {
            HILOG_ERROR("disconnect extension ability failed ret: %{public}d.", ret);
            return false;
        }
        return true;
    } else {
        if (reConfirmConn_ == nullptr) {
            return true;
        }
        ErrCode ret = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(reConfirmConn_);
        if (ret != ERR_OK) {
            HILOG_ERROR("disconnect extension ability failed ret: %{public}d.", ret);
            return false;
        }
        return true;
    }
}

std::string MockShortkeyDialog::BuildStartCommand()
{
    nlohmann::json root;
    std::string tmp = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = tmp;
    root["sysDialogZOrder"] = SYS_DIALOG_Z_ORDER_ON_LOCK_SCREEN;
    std::string cmdData = root.dump();
    HILOG_DEBUG("cmdData is: %{public}s.", cmdData.c_str());
    return cmdData;
}

MockAccessibilityShortKey::~MockAccessibilityShortKey()
{
    HILOG_INFO();

    if (subscribeId_ < 0) {
        return;
    }

    MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId_);
}

void MockAccessibilityShortKey::SubscribeShortKey(std::set<int32_t> preKeys, int32_t finalKey, int32_t holdTime)
{
    std::shared_ptr<MMI::KeyOption> keyOption = std::make_shared<MMI::KeyOption>();
    keyOption->SetPreKeys(preKeys);
    keyOption->SetFinalKey(finalKey);
    keyOption->SetFinalKeyDown(true);
    // MMI will get the real holdTime in SettingsData, so the input parameter may not take effect here.
    keyOption->SetFinalKeyDownDuration(holdTime);

    auto keyEventCallBack = std::bind(&MockAccessibilityShortKey::OnShortKey, this);
    int32_t subscribeId = MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption, keyEventCallBack);
    subscribeId_ = subscribeId;
    if (subscribeId < 0) {
        HILOG_ERROR("Subscribe key event failed, finalKey: %{public}d id: %{public}d", finalKey, subscribeId);
        return;
    }
}

void MockAccessibilityShortKey::Register()
{
    HILOG_INFO();

    if (subscribeId_ >= 0) {
        HILOG_WARN("shortcut key is not unregistered, id: %{public}d", subscribeId_);
    }

    std::set<int32_t> preDownKeysUp;
    preDownKeysUp.insert(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    SubscribeShortKey(preDownKeysUp, MMI::KeyEvent::KEYCODE_VOLUME_DOWN, SHORTCUT_TIMEOUT);
}

void MockAccessibilityShortKey::Unregister()
{
    if (subscribeId_ < 0) {
        HILOG_INFO("shortcut key is not registered");
        return;
    }

    HILOG_INFO("unregister shortcut key, last subscribeId is %{public}d", subscribeId_);
    MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId_);
    subscribeId_ = -1;
}

void MockAccessibilityShortKey::OnShortKey()
{
    HILOG_INFO();

    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
}

TouchInjectHandler::TouchInjectHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    MockTouchEventInjector &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

void TouchInjectHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<SendEventArgs> parameters = nullptr;
    if (!event) {
        HILOG_ERROR("event is nullptr");
        return;
    }
    switch (event->GetInnerEventId()) {
        case MockTouchEventInjector::SEND_TOUCH_EVENT_MSG:
            parameters = event->GetSharedObject<SendEventArgs>();
            if (parameters == nullptr) {
                HILOG_ERROR("parameters is nullptr");
                return;
            }
            if (!parameters->event_) {
                HILOG_WARN("pointer event is nullptr");
                return;
            }
            server_.SendPointerEvent(*parameters->event_);
            break;
        default:
            break;
    }
}

MockTouchEventInjector::MockTouchEventInjector()
{
    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }
    handler_ = std::make_shared<TouchInjectHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

bool MockTouchEventInjector::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    EventTransmission::OnPointerEvent(event);
    return false;
}

void MockTouchEventInjector::DestroyEvents()
{
    HILOG_DEBUG();
    CancelInjectedEvents();
    isDestroyEvent_ = true;
    EventTransmission::DestroyEvents();
}

void MockTouchEventInjector::SendPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (GetNext() != nullptr) {
        EventTransmission::OnPointerEvent(event);
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
            isGestureUnderway_ = true;
        }
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
            isGestureUnderway_ = false;
        }
    }
}

void MockTouchEventInjector::CancelGesture()
{
    HILOG_DEBUG();
    std::shared_ptr<MMI::PointerEvent> event;
    MMI::PointerEvent::PointerItem pointer = {};
    int64_t time = GetSystemTime();
    pointer.SetDownTime(time);
    pointer.SetPointerId(0);
    if (GetNext() != nullptr && isGestureUnderway_) {
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_CANCEL, pointer, time);
        if (event == nullptr) {
            HILOG_ERROR("event is nullptr");
            return;
        }

        SendPointerEvent(*event);
        isGestureUnderway_ = false;
    }
}

void MockTouchEventInjector::CancelInjectedEvents()
{
    HILOG_DEBUG();
    if (handler_ == nullptr) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    if (handler_->HasInnerEvent(SEND_TOUCH_EVENT_MSG)) {
        handler_->RemoveEvent(SEND_TOUCH_EVENT_MSG);
        CancelGesture();
    }
}

std::shared_ptr<MMI::PointerEvent> MockTouchEventInjector::obtainTouchEvent(int32_t action,
    MMI::PointerEvent::PointerItem point, int64_t actionTime)
{
    HILOG_DEBUG();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    if (pointerEvent == nullptr) {
        HILOG_ERROR("pointerEvent is nullptr");
        return nullptr;
    }

    pointerEvent->SetPointerId(point.GetPointerId());
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionTime(actionTime);
    pointerEvent->SetActionStartTime(point.GetDownTime());
    pointerEvent->AddPointerItem(point);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

int64_t MockTouchEventInjector::GetSystemTime()
{
    HILOG_DEBUG();

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}

void MockTouchEventInjector::InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();

    int64_t curTime = GetSystemTime();
    if (isDestroyEvent_ || !GetNext()) {
        HILOG_WARN("Inject gesture fail");
        return;
    }
    CancelInjectedEvents();
    CancelGesture();

    ParseTouchEventsFromGesturePath(curTime, gesturePath);

    if (injectedEvents_.empty()) {
        HILOG_WARN("No injected events");
        return;
    }
    for (size_t i = 0; i < injectedEvents_.size(); i++) {
        std::shared_ptr<SendEventArgs> parameters = std::make_shared<SendEventArgs>();
        parameters->event_ = injectedEvents_[i];
        if (injectedEvents_[i]) {
            int64_t timeout = (injectedEvents_[i]->GetActionTime() - curTime) / MS_TO_US;
            if (timeout < 0) {
                HILOG_WARN("timeout is error.%{public}" PRId64 "", timeout);
            } else {
                handler_->SendEvent(SEND_TOUCH_EVENT_MSG, parameters, timeout);
            }
        }
    }
    injectedEvents_.clear();
}

void MockTouchEventInjector::ParseTapsEvents(int64_t startTime,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();

    if (!gesturePath) {
        HILOG_ERROR("gesturePath is null.");
        return;
    }
    const std::vector<AccessibilityGesturePosition> &positions = gesturePath->GetPositions();
    size_t positionSize = positions.size();
    if (!positionSize) {
        HILOG_WARN("PositionSize is zero.");
        return;
    }
    int64_t durationTime = gesturePath->GetDurationTime();
    if (durationTime < 0) {
        HILOG_WARN("DurationTime is wrong.");
        return;
    }
    int64_t perDurationTime = static_cast<int64_t>(static_cast<uint64_t>(durationTime) / positionSize);
    int64_t downTime = startTime;
    for (size_t i = 0; i < positionSize; i++) {
        std::shared_ptr<MMI::PointerEvent> event;
        MMI::PointerEvent::PointerItem pointer = {};
        pointer.SetPointerId(0);
        // Append down event
        int32_t px = static_cast<int32_t>(positions[i].positionX_);
        int32_t py = static_cast<int32_t>(positions[i].positionY_);
        pointer.SetDisplayX(px);
        pointer.SetDisplayY(py);
        pointer.SetDownTime(downTime);
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
        if (event == nullptr) {
            HILOG_ERROR("event is nullptr");
            return;
        }

        HILOG_DEBUG("append down event");
        injectedEvents_.push_back(event);

        // Append up event
        int64_t upTime = downTime + perDurationTime * MS_TO_US;
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, upTime);
        if (event == nullptr) {
            HILOG_ERROR("event is nullptr");
            return;
        }

        HILOG_DEBUG("append up event");
        injectedEvents_.push_back(event);
        downTime = upTime + DOUBLE_TAP_MIN_TIME;
    }
}

void MockTouchEventInjector::ParseMovesEvents(int64_t startTime,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();

    if (!gesturePath) {
        HILOG_ERROR("gesturePath is null.");
        return;
    }
    std::vector<AccessibilityGesturePosition> positions = gesturePath->GetPositions();
    size_t positionSize = positions.size();
    if (positionSize < MOVE_GESTURE_MIN_PATH_COUNT) {
        HILOG_WARN("PositionSize is wrong.");
        return;
    }
    int64_t durationTime = gesturePath->GetDurationTime();
    if (durationTime < 0) {
        HILOG_WARN("DurationTime is wrong.");
        return;
    }
    int64_t perDurationTime = static_cast<int64_t>(static_cast<uint64_t>(durationTime) / (positionSize - 1));
    int64_t downTime = startTime;
    int64_t nowTime = startTime;
    for (size_t i = 0; i < positionSize; i++) {
        std::shared_ptr<MMI::PointerEvent> event;
        MMI::PointerEvent::PointerItem pointer = {};
        int32_t px = static_cast<int32_t>(positions[i].positionX_);
        int32_t py = static_cast<int32_t>(positions[i].positionY_);
        pointer.SetPointerId(0);
        pointer.SetDisplayX(px);
        pointer.SetDisplayY(py);
        pointer.SetDownTime(downTime);
        if (i == 0) { // Append down event
            HILOG_DEBUG("append down event");
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
            if (event == nullptr) {
                HILOG_ERROR("event is nullptr");
                return;
            }

            injectedEvents_.push_back(event);
        } else if (i < (positionSize - 1)) { // Append move event
            HILOG_DEBUG("append move event");
            nowTime += perDurationTime * MS_TO_US;
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, pointer, nowTime);
            if (event == nullptr) {
                HILOG_ERROR("event is nullptr");
                return;
            }

            injectedEvents_.push_back(event);
        } else { // Append up event
            HILOG_DEBUG("append move event");
            nowTime += perDurationTime * MS_TO_US;
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, pointer, nowTime);
            if (event == nullptr) {
                HILOG_ERROR("event is nullptr");
                return;
            }

            injectedEvents_.push_back(event);
            
            HILOG_DEBUG("append up event");
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, nowTime);
            if (event == nullptr) {
                HILOG_ERROR("event is nullptr");
                return;
            }
            
            injectedEvents_.push_back(event);
        }
    }
}

void MockTouchEventInjector::ParseTouchEventsFromGesturePath(int64_t startTime,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    if (!gesturePath) {
        HILOG_ERROR("gesturePath is null.");
        return;
    }
    const std::vector<AccessibilityGesturePosition> &positions = gesturePath->GetPositions();
    if (positions.size() == 0) {
        HILOG_ERROR("position size is 0.");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS