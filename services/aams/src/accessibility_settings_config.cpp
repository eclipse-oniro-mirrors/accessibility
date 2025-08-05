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
#include "accessible_ability_manager_service.h"
#include "accessibility_account_data.h"
#include "parameters.h"
#include "accessibility_notification_helper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
    const int32_t DEFAULT_FONT_SCALE = 100;
    const int32_t SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    const int32_t SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    const int32_t DEFAULT_ACCOUNT_ID = 100;
    const char* ACCESSIBILITY = "accessibility";
    const char* TOUCH_GUIDE_STATE = "touch_guide_state";
    const char* GESTURE_KEY = "gesture_state";
    const char* CAPTION_KEY = "caption_state";
    const char* KEYEVENT_OBSERVER = "keyevent_observer";
    const char* SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const char* SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
    const char* SCREEN_MAGNIFICATION_MODE = "accessibility_magnification_mode";
    const char* SCREEN_MAGNIFICATION_SCALE = "accessibility_display_magnification_scale";
    const char* MOUSEKEY = "mousekey";
    const char* HIGH_CONTRAST_TEXT_KEY = "high_text_contrast_enabled";
    const char* DALTONIZATION_STATE = "accessibility_display_daltonizer_enabled";
    const char* INVERT_COLOR_KEY = "accessibility_display_inversion_enabled";
    const char* ANIMATION_OFF_KEY = "animation_off";
    const char* AUDIO_MONO_KEY = "master_mono";
    const char* IGNORE_REPEAT_CLICK_SWITCH = "ignore_repeat_click_switch";
    const char* SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
    const char* SHORTCUT_SERVICE = "accessibility_shortcut_target_service";
    const char* CLICK_RESPONCE_TIME = "click_response_time";
    const char* IGNORE_REPEAT_CLICK_TIME = "ignore_repeat_click_time";
    const char* DALTONIZATION_COLOR_FILTER_KEY = "accessibility_display_daltonizer";
    const char* CONTENT_TIMEOUT_KEY = "accessibility_content_timeout";
    const char* BRIGHTNESS_DISCOUNT_KEY = "accessibility_brightness_discount";
    const char* AUDIO_BALANCE_KEY = "master_balance";
    const char* FONT_FAMILY = "accessibility_font_family";
    const char* FONT_COLOR = "accessibility_font_color";
    const char* FONT_EDGE_TYPE = "accessibility_font_edge_type";
    const char* BACKGROUND_COLOR = "accessibility_background_color";
    const char* WINDOW_COLOR = "accessibility_window_color";
    const char* FONT_SCALE = "accessibility_font_scale";
    const char* ENABLED_ACCESSIBILITY_SERVICES = "enabled_accessibility_services";
    const char* SHORTCUT_ENABLED_ON_LOCK_SCREEN = "accessibility_shortcut_enabled_on_lock_screen"; // HMOS key
    const char* SHORTCUT_ON_LOCK_SCREEN = "accessibility_shortcut_on_lock_screen"; // AOS key
    const char* SHORTCUT_TIMEOUT = "accessibility_shortcut_timeout";
    const char* ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const char* SCREENREADER_TAG = "screenreader";
    const char* INVERT_COLOR_AOS_TAG = "ColorInversion";
    const char* INVERT_COLOR_HMOS_TAG = "INVERT_COLOR";
    const char* AUDIO_MONO_HMOS_TAG = "AUDIO_MONO";
    const char* HIGH_CONTRAST_TEXT_HMOS_TAG = "HIGH_CONTRAST_TEXT";
    const char* SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const char* ACCESSIBILITY_SCREENREADER_ENABLED = "accessibility_screenreader_enabled";
    const char* ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE = "accessibility_privacy_clone_or_upgrade";
    const char* IGNORE_REPEAT_CLICK_RECONFIRM = "accessibility_ignore_repeat_click_reconfirm";
    const char* ZOOM_GESTURE_ENABLED_RECONFIRM = "accessibility_zoom_gesture_enabled_reconfirm";
    const char* GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const char* ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const char* FLASH_REMINDER_SWITCH_KEY = "accessibility_flash_reminder_switch";
    const char* FLASH_REMINDER_ENABLED = "accessibility_reminder_function_enabled";
    const char* VOICE_RECOGNITION_KEY = "accessibility_sound_recognition_switch";
    const char* VOICE_RECOGNITION_TYPES = "accessibility_sound_recognition_enabled";
    const char* IGNORE_REPEAT_CLICK_TIMESTAMP = "accessibility_ignore_repeat_click_timestamp";
    const char* RECOVERY_IGNORE_REPEAT_CLICK_DATE = "recovery_ignore_repeat_click_switch_date";
    const char* IGNORE_REPEATED_CLICK_CACHE_FLAG = "accessibility_ignore_repeat_click_cache_flag";
    constexpr int DOUBLE_CLICK_RESPONSE_TIME_MEDIUM = 300;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORTEST = 100;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT = 400;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_MEDIUM = 700;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG = 1000;
    constexpr int DISPLAY_DALTONIZER_INVALID = -1;
    constexpr int DISPLAY_DALTONIZER_GREEN = 12;
    constexpr int DISPLAY_DALTONIZER_RED = 11;
    constexpr int DISPLAY_DALTONIZER_BLUE = 13;
    constexpr int INVALID_MASTER_MONO_VALUE = -1;
    constexpr int AUDIO_BALANCE_STEP = 5;
    constexpr float INVALID_MASTER_BALANCE_VALUE = 2.0;
    constexpr int INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE = 2;
    constexpr uint32_t IGNORE_REPEAT_CLICK_SHORTEST = 0;
    constexpr uint32_t IGNORE_REPEAT_CLICK_SHORT = 1;
    constexpr float DEFAULT_MAGNIFICATION_SCALE = 2.0;
    bool g_ignoreRepeatClickOnceFlag = false;
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetEnabled");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetTouchGuideState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetGestureState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetKeyEventObserverState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetCaptionState");
        HILOG_ERROR("set isCaptionState_ failed");
        return ret;
    }
    isCaptionState_ = state;
    return ret;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SCREEN_MAGNIFICATION_KEY, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetScreenMagnificationState");
        HILOG_ERROR("set SetScreenMagnificationState failed");
        return ret;
    }
    SetMagnificationState(state);
    return ret;
}

RetError AccessibilitySettingsConfig::SetMagnificationState(const bool state)
{
    isScreenMagnificationState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationType(const uint32_t type)
{
    HILOG_DEBUG("screenMagnificationType = [%{public}u]", type);
    screenMagnificationType_ = type;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationMode(const uint32_t mode)
{
    HILOG_DEBUG("screenMagnificationMode = [%{public}u]", mode);
    screenMagnificationMode_ = mode;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationScale(const float scale)
{
    HILOG_DEBUG("screenMagnificationScale = [%{public}f]", scale);
    screenMagnificationScale_ = scale;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortKeyState(const bool state)
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

RetError AccessibilitySettingsConfig::SetShortKeyOnLockScreenState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    auto ret = SetConfigState(SHORTCUT_ENABLED_ON_LOCK_SCREEN, state);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortKeyOnLockScreenState");
        HILOG_ERROR("set isShortKeyEnabledOnLockScreen_ failed");
    }
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortKeyTimeout");
        HILOG_ERROR("set shortKeyTimeout_ failed");
        return ret;
    }
    shortKeyTimeout_ = time;
    return ret;
}

RetError AccessibilitySettingsConfig::SetStartToHosState(const bool state)
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

RetError AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
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

RetError AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
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

RetError AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
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

RetError AccessibilitySettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetShortkeyMultiTargetInPkgRemove");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetHighContrastTextState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetInvertColorState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAnimationOffState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAudioMonoState");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetDaltonizationState");
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


RetError AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
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

RetError AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetAudioBalance");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetClickResponseTime");
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
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetIgnoreRepeatClickState");
        HILOG_ERROR("set ignoreRepeatClickState_ failed");
        return ret;
    }
    if (state) {
        uint64_t nowTime = IgnoreRepeatClickNotification::GetWallTimeMs();
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

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickReconfirm(const bool state)
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

RetError AccessibilitySettingsConfig::SetZoomGestureEnabledReconfirm(const bool state)
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

RetError AccessibilitySettingsConfig::SetColorModeState(const A11yDarkModeType &type)
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

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
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

RetError AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
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
    if (!datashare_) {
        HILOG_ERROR("datashare is nullptr!");
        return false;
    }
    return datashare_->GetBoolValue(SHORTCUT_ENABLED_ON_LOCK_SCREEN, false);
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

uint32_t AccessibilitySettingsConfig::GetScreenMagnificationMode() const
{
    return screenMagnificationMode_;
}

float AccessibilitySettingsConfig::GetScreenMagnificationScale() const
{
    return screenMagnificationScale_;
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
    auto ret = datashare_->PutStringValue(ENABLED_ACCESSIBILITY_SERVICES, stringOut);
    if (ret != RET_OK) {
        Utils::RecordDatashareInteraction(A11yDatashareValueType::UPDATE, "SetEnabledAccessibilityServices");
        HILOG_ERROR("set enabledAccessibilityServices failed");
    }
    return ret;
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

bool AccessibilitySettingsConfig::GetStartToHosState()
{
    HILOG_DEBUG();
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    bool value = true;
    value = datashare_->GetBoolValue("AccessibilityStartToHos", true);
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

void AccessibilitySettingsConfig::InitShortKeyConfig()
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

void AccessibilitySettingsConfig::InitPrivacySpaceConfig()
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

void AccessibilitySettingsConfig::InitAnimationOffConfig()
{
    animationOffState_ = datashare_->GetBoolValue(ANIMATION_OFF_KEY, false);
    std::string graphicState = system::GetParameter(GRAPHIC_ANIMATION_SCALE_NAME, "1");
    std::string arkuiState = system::GetParameter(ARKUI_ANIMATION_SCALE_NAME, "1");
    bool state = (graphicState == "0" && arkuiState == "0");
    if (animationOffState_) {
        system::SetParameter(GRAPHIC_ANIMATION_SCALE_NAME, "0");
        system::SetParameter(ARKUI_ANIMATION_SCALE_NAME, "0");
    } else if (!animationOffState_ && state) {
        system::SetParameter(GRAPHIC_ANIMATION_SCALE_NAME, "1");
        system::SetParameter(ARKUI_ANIMATION_SCALE_NAME, "1");
    }
}

void AccessibilitySettingsConfig::HandleIgnoreRepeatClickState()
{
    uint64_t recoveryDate = datashare_->GetUnsignedLongValue(RECOVERY_IGNORE_REPEAT_CLICK_DATE, 0);
    if (ignoreRepeatClickState_ && recoveryDate == 0 &&
        (ignoreRepeatClickTime_ == IGNORE_REPEAT_CLICK_SHORTEST ||
            ignoreRepeatClickTime_ == IGNORE_REPEAT_CLICK_SHORT)) {
        ignoreRepeatClickState_ = false;
        SetIgnoreRepeatClickState(false);
        recoveryDate = IgnoreRepeatClickNotification::GetWallTimeMs();
        datashare_->PutUnsignedLongValue(RECOVERY_IGNORE_REPEAT_CLICK_DATE, recoveryDate);
        HILOG_INFO("recovery ignore repeat click %{public}" PRIu64, recoveryDate);
    }
 
    if (ignoreRepeatClickState_) {
        IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
        uint64_t timeStamp = datashare_->GetUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, 0);
        if (timeStamp == 0) {
            timeStamp = IgnoreRepeatClickNotification::GetWallTimeMs();
            datashare_->PutUnsignedLongValue(IGNORE_REPEAT_CLICK_TIMESTAMP, timeStamp);
        }
        if (timeStamp > 0) {
            IgnoreRepeatClickNotification::RegisterTimers(timeStamp);
        }
        if (g_ignoreRepeatClickOnceFlag) {
            IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
        } else {
            g_ignoreRepeatClickOnceFlag = true;
        }
    }
}

void AccessibilitySettingsConfig::HandleIgnoreRepeatClickCache()
{
    bool value = false;
    value = datashare_->GetBoolValue(IGNORE_REPEATED_CLICK_CACHE_FLAG, false);
    if (value) {
        ignoreRepeatClickState_ = true;
        datashare_->PutBoolValue(IGNORE_REPEAT_CLICK_SWITCH, true);
        datashare_->PutBoolValue(IGNORE_REPEATED_CLICK_CACHE_FLAG, false);
    } else {
        ignoreRepeatClickState_ = datashare_->GetBoolValue(IGNORE_REPEAT_CLICK_SWITCH, false);
    }
}

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }
    
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
    HandleIgnoreRepeatClickCache();
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
    HandleIgnoreRepeatClickState();
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
    auto ret = datashare_->PutBoolValue(key, value);
    if (ret != RET_OK) {
        HILOG_ERROR("set configState failed");
    }
    return ret;
}

void AccessibilitySettingsConfig::Init()
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

void AccessibilitySettingsConfig::SetDefaultShortcutKeyService()
{
    HILOG_DEBUG();

    if (GetShortkeyMultiTarget().empty()) {
        HILOG_INFO("set default shortcut key service.");
        std::vector<std::string> defaultService;
        defaultService.push_back(SCREEN_READER_BUNDLE_ABILITY_NAME);
        SetShortkeyMultiTarget(defaultService);
    }
}

void AccessibilitySettingsConfig::OnDataClone()
{
    HILOG_INFO();
    if (ignoreRepeatClickState_) {
        IgnoreRepeatClickNotification::CancelNotification();
    }

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

bool AccessibilitySettingsConfig::GetInitializeState()
{
    return isInitialized_;
}

void AccessibilitySettingsConfig::SetInitializeState(bool isInitialized)
{
    isInitialized_ = isInitialized;
}

} // namespace Accessibility
} // namespace OHOS