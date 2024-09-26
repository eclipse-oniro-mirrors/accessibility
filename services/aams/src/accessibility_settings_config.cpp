/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_settings_config.h"
#include "hilog_wrapper.h"
#include "system_ability_definition.h"
#include "utils.h"
#include "accessibility_setting_provider.h"
#include "cfloat"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
    const int32_t DEFAULT_SCALE = 100;
    const int32_t SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    const int32_t SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    const std::string ACCESSIBILITY = "accessibility";
    const std::string TOUCH_GUIDE_STATE = "touch_guide_state";
    const std::string GESTURE_KEY = "gesture_state";
    const std::string CAPTION_KEY = "caption_state";
    const std::string KEYEVENT_OBSERVER = "keyevent_observer";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
    const std::string MOUSEKEY = "mousekey";
    const std::string HIGH_CONTRAST_TEXT_KEY = "high_text_contrast_enabled";
    const std::string DALTONIZATION_STATE = "accessibility_display_daltonizer_enabled";
    const std::string INVERT_COLOR_KEY = "accessibility_display_inversion_enabled";
    const std::string ANIMATION_OFF_KEY = "animation_off";
    const std::string AUDIO_MONO_KEY = "master_mono";
    const std::string IGNORE_REPEAT_CLICK_SWITCH = "ignore_repeat_click_switch";
    const std::string SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
    const std::string SHORTCUT_SERVICE = "accessibility_shortcut_target_service";
    const std::string CLICK_RESPONCE_TIME = "click_response_time";
    const std::string IGNORE_REPEAT_CLICK_TIME = "ignore_repeat_click_time";
    const std::string DALTONIZATION_COLOR_FILTER_KEY = "accessibility_display_daltonizer";
    const std::string CONTENT_TIMEOUT_KEY = "accessibility_content_timeout";
    const std::string BRIGHTNESS_DISCOUNT_KEY = "accessibility_brightness_discount";
    const std::string AUDIO_BALANCE_KEY = "master_balance";
    const std::string FONT_FAMILY = "accessibility_font_family";
    const std::string FONT_COLOR = "accessibility_font_color";
    const std::string FONT_EDGE_TYPE = "accessibility_font_edge_type";
    const std::string BACKGROUND_COLOR = "accessibility_background_color";
    const std::string WINDOW_COLOR = "accessibility_window_color";
    const std::string FONT_SCALE = "accessibility_font_scale";
    const std::string ENABLED_ACCESSIBILITY_SERVICES = "enabled_accessibility_services";
    const std::string SHORTCUT_ENABLED_ON_LOCK_SCREEN = "accessibility_shortcut_enabled_on_lock_screen"; // HMOS key
    const std::string SHORTCUT_ON_LOCK_SCREEN = "accessibility_shortcut_on_lock_screen"; // AOS key
    const std::string SHORTCUT_TIMEOUT = "accessibility_shortcut_timeout";
    const std::string ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const std::string SCREENREADER_TAG = "screenreader";
    const std::string INVERT_COLOR_AOS_TAG = "ColorInversion";
    const std::string INVERT_COLOR_HMOS_TAG = "INVERT_COLOR";
    const std::string AUDIO_MONO_HMOS_TAG = "AUDIO_MONO";
    const std::string HIGH_CONTRAST_TEXT_HMOS_TAG = "HIGH_CONTRAST_TEXT";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string ACCESSIBILITY_SCREENREADER_ENABLED = "accessibility_screenreader_enabled";
    constexpr int DOUBLE_CLICK_RESPONSE_TIME_MEDIUM = 300;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT = 400;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_MEDIUM = 700;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG = 1000;
    constexpr int DISPLAY_DALTONIZER_GREEN = 12;
    constexpr int DISPLAY_DALTONIZER_RED = 11;
    constexpr int DISPLAY_DALTONIZER_BLUE = 13;
    constexpr int INVALID_MASTER_MONO_VALUE = -1;
    constexpr int AUDIO_BALANCE_STEP = 5;
    constexpr float INVALID_MASTER_BALANCE_VALUE = 2.0;
    constexpr int INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE = 2;
} // namespace
AccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("id = [%{public}d]", id);
    accountId_ = id;
}

RetError AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(ACCESSIBILITY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set accessibility failed");
        return ret;
    }
    enabled_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(TOUCH_GUIDE_STATE, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set eventTouchGuideState_ failed");
        return ret;
    }
    eventTouchGuideState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(GESTURE_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set gesturesSimulation_ failed");
        return ret;
    }
    gesturesSimulation_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(KEYEVENT_OBSERVER, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set filteringKeyEvents_ failed");
        return ret;
    }
    filteringKeyEvents_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(CAPTION_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set isCaptionState_ failed");
        return ret;
    }
    isCaptionState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isScreenMagnificationState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationType(const uint32_t type)
{
    HILOG_DEBUG("screenMagnificationType = [%{public}u]", type);
    screenMagnificationType_ = type;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SHORTCUT_ENABLED, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set isShortKeyState_ failed");
        return ret;
    }
    isShortKeyState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetShortKeyOnLockScreenState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SHORTCUT_ENABLED_ON_LOCK_SCREEN, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set isShortKeyEnabledOnLockScreen_ failed");
        return ret;
    }
    isShortKeyEnabledOnLockScreen_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetShortKeyTimeout(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue(SHORTCUT_TIMEOUT, static_cast<int32_t>(time));
    if (ret != RET_OK) {
        HILOG_ERROR("set shortKeyTimeout_ failed");
        return ret;
    }
    shortKeyTimeout_ = time;
    return ret;
}

RetError AccessibilitySettingsConfig::SetStartFromAtoHosState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }
    return datashare_->PutBoolValue("AccessibilityStartFromAtoHos", state);
}

RetError AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(MOUSEKEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set isMouseKeyState_ failed");
        return ret;
    }
    isMouseKeyState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}d]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue("MouseAutoClick", time);
    if (ret != RET_OK) {
        HILOG_ERROR("set mouseAutoClick_ failed");
        return ret;
    }
    mouseAutoClick_ = time;
    return ret;
}

RetError AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("name = [%{public}s]", name.c_str());
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutStringValue("ShortkeyTarget", name);
    if (ret != RET_OK) {
        HILOG_ERROR("set shortkeyTarget_ failed");
        return ret;
    }
    shortkeyTarget_ = name;
    return ret;
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG();
    std::set<std::string> targets;
    std::copy_if(name.begin(), name.end(), std::inserter(targets, targets.end()),
        [&targets](const std::string &target) {
            targets.insert(target);
            return true;
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
        HILOG_ERROR("set shortkeyMultiTarget_ failed");
        return ret;
    }
    shortkeyMultiTarget_ = std::vector<std::string>(targets.begin(), targets.end());
    return ret;
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTargetInPkgRemove(const std::string &name)
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
        HILOG_ERROR("set shortkeyMultiTarget_ failed");
        shortkeyMultiTarget_.push_back(name);
    }
    return rtn;
}

RetError AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(HIGH_CONTRAST_TEXT_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set highContrastTextState_ failed");
        return ret;
    }
    highContrastTextState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(INVERT_COLOR_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set invertColorState_ failed");
        return ret;
    }
    invertColorState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(ANIMATION_OFF_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set animationOffState_ failed");
        return ret;
    }
    animationOffState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(AUDIO_MONO_KEY, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set audioMonoState_ failed");
        return ret;
    }
    audioMonoState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(DALTONIZATION_STATE, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set daltonizationState_ failed");
        return ret;
    }
    daltonizationState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
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
    }
    auto ret = datashare_->PutIntValue(DALTONIZATION_COLOR_FILTER_KEY, static_cast<int32_t>(daltonizationColorFilter));
    if (ret != RET_OK) {
        HILOG_ERROR("set daltonizationColorFilter_ failed");
        return ret;
    }
    daltonizationColorFilter_ = daltonizationColorFilter;
    return ret;
}


RetError AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutIntValue(CONTENT_TIMEOUT_KEY, static_cast<int32_t>(time));
    if (ret != RET_OK) {
        HILOG_ERROR("set contentTimeout_ failed");
        return ret;
    }
    contentTimeout_ = time;
    return ret;
}

RetError AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("discount = [%{public}f]", discount);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    auto ret = datashare_->PutFloatValue(BRIGHTNESS_DISCOUNT_KEY, discount);
    if (ret != RET_OK) {
        HILOG_ERROR("set brightnessDiscount_ failed");
        return ret;
    }
    brightnessDiscount_ = discount;
    return ret;
}

RetError AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("balance = [%{public}f]", balance);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    float audioBalance = round(balance * AUDIO_BALANCE_STEP) / AUDIO_BALANCE_STEP;
    auto ret = datashare_->PutFloatValue(AUDIO_BALANCE_KEY, audioBalance);
    if (ret != RET_OK) {
        HILOG_ERROR("set audioBalance_ failed");
        return ret;
    }
    audioBalance_ = audioBalance;
    return ret;
}

RetError AccessibilitySettingsConfig::SetClickResponseTime(const uint32_t time)
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
        HILOG_ERROR("set clickResponseTime_ failed");
        return ret;
    }
    clickResponseTime_ = clickResponseTime;
    return ret;
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(IGNORE_REPEAT_CLICK_SWITCH, state);
    if (ret != RET_OK) {
        HILOG_ERROR("set ignoreRepeatClickState_ failed");
        return ret;
    }
    ignoreRepeatClickState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG("ignoreRepeatClickTime = [%{public}u]", time);
    if (!datashare_) {
        HILOG_ERROR("helper is nullptr");
        return RET_ERR_NULLPTR;
    }

    uint32_t ignoreRepeatClickTime = time;
    if (time == DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT) {
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
        HILOG_ERROR("set ignoreRepeatClickTime_ failed");
        return ret;
    }
    ignoreRepeatClickTime_ = ignoreRepeatClickTime;
    return ret;
}

RetError AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG();
    captionProperty_ = caption;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    datashare_->PutStringValue(FONT_FAMILY, captionProperty_.GetFontFamily());
    datashare_->PutIntValue(FONT_COLOR, static_cast<int32_t>(captionProperty_.GetFontColor()));
    datashare_->PutStringValue(FONT_EDGE_TYPE, captionProperty_.GetFontEdgeType());
    datashare_->PutIntValue(BACKGROUND_COLOR, static_cast<int32_t>(captionProperty_.GetBackgroundColor()));
    datashare_->PutIntValue(WINDOW_COLOR, static_cast<int32_t>(captionProperty_.GetWindowColor()));
    datashare_->PutIntValue(FONT_SCALE, captionProperty_.GetFontScale());
    return RET_OK;
}

bool AccessibilitySettingsConfig::GetCaptionState() const
{
    return isCaptionState_;
}

bool AccessibilitySettingsConfig::GetScreenMagnificationState() const
{
    return isScreenMagnificationState_;
}

bool AccessibilitySettingsConfig::GetShortKeyState() const
{
    return isShortKeyState_;
}

bool AccessibilitySettingsConfig::GetShortKeyOnLockScreenState() const
{
    return isShortKeyEnabledOnLockScreen_;
}

int32_t AccessibilitySettingsConfig::GetShortKeyTimeout() const
{
    return shortKeyTimeout_;
}

bool AccessibilitySettingsConfig::GetMouseKeyState() const
{
    return isMouseKeyState_;
}

int32_t AccessibilitySettingsConfig::GetMouseAutoClick() const
{
    return mouseAutoClick_;
}

const std::string &AccessibilitySettingsConfig::GetShortkeyTarget() const
{
    return shortkeyTarget_;
}

const std::vector<std::string> AccessibilitySettingsConfig::GetShortkeyMultiTarget()
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    std::vector<std::string> rtnVec = shortkeyMultiTarget_;
    return rtnVec;
}

bool AccessibilitySettingsConfig::GetHighContrastTextState() const
{
    return highContrastTextState_;
}

bool AccessibilitySettingsConfig::GetInvertColorState() const
{
    return invertColorState_;
}

bool AccessibilitySettingsConfig::GetAnimationOffState() const
{
    return animationOffState_;
}

bool AccessibilitySettingsConfig::GetAudioMonoState() const
{
    return audioMonoState_;
}

bool AccessibilitySettingsConfig::GetDaltonizationState() const
{
    return daltonizationState_;
}

uint32_t AccessibilitySettingsConfig::GetDaltonizationColorFilter() const
{
    return daltonizationColorFilter_;
}

uint32_t AccessibilitySettingsConfig::GetContentTimeout() const
{
    return contentTimeout_;
}

float AccessibilitySettingsConfig::GetBrightnessDiscount() const
{
    return brightnessDiscount_;
}

float AccessibilitySettingsConfig::GetAudioBalance() const
{
    return audioBalance_;
}

bool AccessibilitySettingsConfig::GetEnabledState() const
{
    return enabled_;
}

bool AccessibilitySettingsConfig::GetTouchGuideState() const
{
    return eventTouchGuideState_;
}

bool AccessibilitySettingsConfig::GetGestureState() const
{
    return gesturesSimulation_;
}

bool AccessibilitySettingsConfig::GetKeyEventObserverState() const
{
    return filteringKeyEvents_;
}

const AccessibilityConfig::CaptionProperty &AccessibilitySettingsConfig::GetCaptionProperty() const
{
    return captionProperty_;
};

uint32_t AccessibilitySettingsConfig::GetClickResponseTime() const
{
    return clickResponseTime_;
}

uint32_t AccessibilitySettingsConfig::GetScreenMagnificationType() const
{
    return screenMagnificationType_;
}

bool AccessibilitySettingsConfig::GetIgnoreRepeatClickState() const
{
    return ignoreRepeatClickState_;
}

uint32_t AccessibilitySettingsConfig::GetIgnoreRepeatClickTime() const
{
    return ignoreRepeatClickTime_;
}

RetError AccessibilitySettingsConfig::SetEnabledAccessibilityServices(const std::vector<std::string> &services)
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    enabledAccessibilityServices_ = services;
    if (datashare_ == nullptr) {
        HILOG_WARN("datashare_ is null.");
        return RET_ERR_NULLPTR;
    }
    std::string stringOut = "";
    Utils::VectorToString(enabledAccessibilityServices_, stringOut);
    return datashare_->PutStringValue(ENABLED_ACCESSIBILITY_SERVICES, stringOut);
}

const std::vector<std::string> AccessibilitySettingsConfig::GetEnabledAccessibilityServices()
{
    std::lock_guard<ffrt::mutex> lock(interfaceMutex_);
    std::vector<std::string> rtnVec = enabledAccessibilityServices_;
    return rtnVec;
}

RetError AccessibilitySettingsConfig::AddEnabledAccessibilityService(const std::string &serviceName)
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

RetError AccessibilitySettingsConfig::RemoveEnabledAccessibilityService(const std::string &serviceName)
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

bool AccessibilitySettingsConfig::GetStartFromAtoHosState()
{
    HILOG_DEBUG();
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    bool value = true;
    value = datashare_->GetBoolValue("AccessibilityStartFromAtoHos", true);
    return value;
}

uint32_t AccessibilitySettingsConfig::GetConfigState()
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

void AccessibilitySettingsConfig::InitCaption()
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

    int32_t fontScale =  static_cast<int32_t>(datashare_->GetIntValue(FONT_SCALE, DEFAULT_SCALE));
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

void AccessibilitySettingsConfig::InitShortKeyConfig()
{
    isShortKeyState_ = datashare_->GetBoolValue(SHORTCUT_ENABLED, true);
    int isShortKeyEnabledOnLockScreen = datashare_->GetIntValue(SHORTCUT_ENABLED_ON_LOCK_SCREEN,
        INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
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
        SetShortKeyOnLockScreenState(isShortKeyEnabledOnLockScreen == 1);
    }

    auto ret = datashare_->PutIntValue(SHORTCUT_ON_LOCK_SCREEN, INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    if (ret != RET_OK) {
        HILOG_ERROR("reset shortcut on lock screen failed");
    }
}

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }

    InitShortKeyConfig();
    CloneAudioState();
    isScreenMagnificationState_ = datashare_->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false);
    isMouseKeyState_= datashare_->GetBoolValue(MOUSEKEY, false);
    animationOffState_ = datashare_->GetBoolValue(ANIMATION_OFF_KEY, false);
    invertColorState_ = datashare_->GetBoolValue(INVERT_COLOR_KEY, false);
    highContrastTextState_ = datashare_->GetBoolValue(HIGH_CONTRAST_TEXT_KEY, false);
    daltonizationState_ = datashare_->GetBoolValue(DALTONIZATION_STATE, false);
    audioMonoState_ = datashare_->GetBoolValue(AUDIO_MONO_KEY, false);
    ignoreRepeatClickState_ = datashare_->GetBoolValue(IGNORE_REPEAT_CLICK_SWITCH, false);

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

    mouseAutoClick_ = static_cast<int32_t>(datashare_->GetIntValue("MouseAutoClick", -1));
    daltonizationColorFilter_ = static_cast<uint32_t>(datashare_->GetIntValue(DALTONIZATION_COLOR_FILTER_KEY, 0));
    SetDaltonizationColorFilter(daltonizationColorFilter_);
    contentTimeout_ = static_cast<uint32_t>(datashare_->GetIntValue(CONTENT_TIMEOUT_KEY, 0));
    brightnessDiscount_ = static_cast<float>(datashare_->GetFloatValue(BRIGHTNESS_DISCOUNT_KEY, 1.0));
    audioBalance_ = static_cast<float>(datashare_->GetFloatValue(AUDIO_BALANCE_KEY, 0));
    SetAudioBalance(audioBalance_);
    screenMagnificationType_ = static_cast<uint32_t>(datashare_->GetIntValue(SCREEN_MAGNIFICATION_TYPE, 0));
    clickResponseTime_ = static_cast<uint32_t>(datashare_->GetIntValue(CLICK_RESPONCE_TIME, 0));
    SetClickResponseTime(clickResponseTime_);
    ignoreRepeatClickTime_ = static_cast<uint32_t>(datashare_->GetIntValue(IGNORE_REPEAT_CLICK_TIME, 0));
    SetIgnoreRepeatClickTime(ignoreRepeatClickTime_);
}

void AccessibilitySettingsConfig::InitCapability()
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

RetError AccessibilitySettingsConfig::SetConfigState(const std::string& key, bool value)
{
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    return datashare_->PutBoolValue(key, value);
}

void AccessibilitySettingsConfig::Init()
{
    HILOG_DEBUG();
    datashare_ = std::make_shared<AccessibilityDatashareHelper>(DATASHARE_TYPE::SECURE, accountId_);
    if (datashare_ == nullptr) {
        return;
    }
    datashare_->Initialize(POWER_MANAGER_SERVICE_ID);
    InitCaption();
    InitSetting();

    systemDatashare_ = std::make_shared<AccessibilityDatashareHelper>(DATASHARE_TYPE::SYSTEM, accountId_);
    if (systemDatashare_ == nullptr) {
        return;
    }
    systemDatashare_->Initialize(POWER_MANAGER_SERVICE_ID);
}

void AccessibilitySettingsConfig::ClearData()
{
    HILOG_DEBUG();
}

void AccessibilitySettingsConfig::CloneAudioState()
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
            HILOG_ERROR("reset monoValue in system table failed");
        }
    }

    float audioBalance = static_cast<float>(systemDatashare_->GetFloatValue(AUDIO_BALANCE_KEY,
        INVALID_MASTER_BALANCE_VALUE));
    if (fabs(audioBalance - INVALID_MASTER_BALANCE_VALUE) < FLT_EPSILON) {
        SetAudioBalance(audioBalance);
        ret = systemDatashare_->PutFloatValue(AUDIO_BALANCE_KEY, INVALID_MASTER_BALANCE_VALUE);
        if (ret != RET_OK) {
            HILOG_ERROR("reset audioBalance in system table failed");
        }
    }
}

uint32_t AccessibilitySettingsConfig::GetShortKeyService(std::vector<std::string> &services)
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

void AccessibilitySettingsConfig::CloneShortkeyService(bool isScreenReaderEnabled)
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

void AccessibilitySettingsConfig::OnDataClone()
{
    HILOG_INFO();
    InitSetting();

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
        HILOG_INFO("set screenReader state, ret = %{public}d", ret);
    }
    service->PutBoolValue(ACCESSIBILITY_CLONE_FLAG, false);
}
} // namespace Accessibility
} // namespace OHOS