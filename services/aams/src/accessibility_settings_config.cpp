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

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
    const int32_t DEFAULT_SCALE = 100;
    const std::string ACCESSIBILITY = "accessibility";
    const std::string TOUCH_GUIDE_STATE = "touch_guide_state";
    const std::string GESTURE_KEY = "gesture_state";
    const std::string CAPTION_KEY = "caption_state";
    const std::string KEYEVENT_OBSERVER = "keyevent_observer";
    const std::string SCREEN_MAGNIFICATION_KEY = "screen_magnification";
    const std::string MOUSEKEY = "mousekey";
    const std::string HIGH_CONTRAST_TEXT_KEY = "high_text_constrast_enabled";
    const std::string DALTONIZATION_STATE = "accessibility_display_daltonizer_enabled";
    const std::string INVERT_COLOR_KEY = "accessibility_display_inversion_enabled";
    const std::string ANIMATION_OFF_KEY = "animation_off";
    const std::string AUDIO_MONO_KEY = "master_nono";
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
} // namespace
AccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("id = [%{public}d]", id);
    accountId_ = id;
}

RetError AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    enabled_ = state;
    return SetConfigState(ACCESSIBILITY, state);
}

RetError AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    eventTouchGuideState_ = state;
    return SetConfigState(TOUCH_GUIDE_STATE, state);
}

RetError AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    gesturesSimulation_ = state;
    return SetConfigState(GESTURE_KEY, state);
}

RetError AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    filteringKeyEvents_ = state;
    return SetConfigState(KEYEVENT_OBSERVER, state);
}

RetError AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isCaptionState_ = state;
    return SetConfigState(CAPTION_KEY, state);
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isScreenMagnificationState_ = state;
    return SetConfigState(SCREEN_MAGNIFICATION_KEY, state);
}

RetError AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isShortKeyState_ = state;
    return SetConfigState(SHORTCUT_ENABLED, state);
}


RetError AccessibilitySettingsConfig::SetStartFromAtoHosState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    return datashare_->PutBoolValue("AccessibilityStartFromAtoHos", state);
}

RetError AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isMouseKeyState_ = state;
    return SetConfigState(MOUSEKEY, state);
}

RetError AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}d]", time);
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    mouseAutoClick_ = time;
    return datashare_->PutIntValue("MouseAutoClick", time);
}

RetError AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("name = [%{public}s]", name.c_str());
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutStringValue("ShortkeyTarget", name);
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG();
    shortkeyMultiTarget_ = name;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    std::string stringOut = "";
    Utils::VectorToString(name, stringOut);
    return datashare_->PutStringValue(SHORTCUT_SERVICE, stringOut);
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTargetInPkgRemove(const std::string &name)
{
    HILOG_DEBUG();
    if (!datashare_) {
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
    return rtn;
}

RetError AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    highContrastTextState_ = state;
    return SetConfigState(HIGH_CONTRAST_TEXT_KEY, state);
}

RetError AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    invertColorState_ = state;
    return SetConfigState(INVERT_COLOR_KEY, state);
}

RetError AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    animationOffState_ = state;
    return SetConfigState(ANIMATION_OFF_KEY, state);
}

RetError AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    audioMonoState_ = state;
    return SetConfigState(AUDIO_MONO_KEY, state);
}

RetError AccessibilitySettingsConfig::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    daltonizationState_ = state;
    return SetConfigState(DALTONIZATION_STATE, state);
}

RetError AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("filter = [%{public}u]", filter);
    daltonizationColorFilter_ = filter;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutIntValue(DALTONIZATION_COLOR_FILTER_KEY, static_cast<int32_t>(filter));
}


RetError AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    contentTimeout_ = time;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutIntValue(CONTENT_TIMEOUT_KEY, static_cast<int32_t>(time));
}

RetError AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("discount = [%{public}f]", discount);
    brightnessDiscount_ = discount;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutFloatValue(BRIGHTNESS_DISCOUNT_KEY, discount);
}

RetError AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("balance = [%{public}f]", balance);
    audioBalance_ = balance;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutFloatValue(AUDIO_BALANCE_KEY, balance);
}

RetError AccessibilitySettingsConfig::SetClickResponseTime(const uint32_t time)
{
    HILOG_DEBUG("clickResponseTime = [%{public}u]", time);
    clickResponseTime_ = time;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    return datashare_->PutIntValue(CLICK_RESPONCE_TIME, time);
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    ignoreRepeatClickState_ = state;
    return SetConfigState(IGNORE_REPEAT_CLICK_TIME, state);
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG("ignoreRepeatClickTime = [%{public}u]", time);
    ignoreRepeatClickTime_ = time;
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }
    return datashare_->PutIntValue("ignoreRepeatClickTime", time);
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

const std::vector<std::string> &AccessibilitySettingsConfig::GetShortkeyMultiTarget() const
{
    return shortkeyMultiTarget_;
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

bool AccessibilitySettingsConfig::GetIgnoreRepeatClickState() const
{
    return ignoreRepeatClickState_;
}

uint32_t AccessibilitySettingsConfig::GetIgnoreRepeatClickTime() const
{
    return ignoreRepeatClickTime_;
}

bool AccessibilitySettingsConfig::GetStartFromAtoHosState()
{
    HILOG_DEBUG();
    if (!datashare_) {
        return RET_ERR_NULLPTR;
    }

    bool value = false;
    datashare_->GetBoolValue("AccessibilityStartFromAtoHos", value);
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

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }

    isScreenMagnificationState_ = datashare_->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false);
    isMouseKeyState_= datashare_->GetBoolValue(MOUSEKEY, false);
    isShortKeyState_ = datashare_->GetBoolValue(SHORTCUT_ENABLED, false);
    animationOffState_ = datashare_->GetBoolValue(ANIMATION_OFF_KEY, false);
    invertColorState_ = datashare_->GetBoolValue(INVERT_COLOR_KEY, false);
    highContrastTextState_ = datashare_->GetBoolValue(HIGH_CONTRAST_TEXT_KEY, false);
    daltonizationState_ = datashare_->GetBoolValue(DALTONIZATION_STATE, false);
    audioMonoState_ = datashare_->GetBoolValue(AUDIO_MONO_KEY, false);
    ignoreRepeatClickState_ = datashare_->GetBoolValue(IGNORE_REPEAT_CLICK_TIME, false);

    shortkeyTarget_ = datashare_->GetStringValue("ShortkeyTarget", "none");

    std::string tmpString = datashare_->GetStringValue(SHORTCUT_SERVICE, "");
    Utils::StringToVector(tmpString, shortkeyMultiTarget_);

    mouseAutoClick_ = static_cast<int32_t>(datashare_->GetIntValue("MouseAutoClick", -1));
    daltonizationColorFilter_ = static_cast<uint32_t>(datashare_->GetIntValue(DALTONIZATION_COLOR_FILTER_KEY, 0));
    contentTimeout_ = static_cast<uint32_t>(datashare_->GetIntValue(CONTENT_TIMEOUT_KEY, 0));
    brightnessDiscount_ = static_cast<float>(datashare_->GetFloatValue(BRIGHTNESS_DISCOUNT_KEY, 1.0));
    audioBalance_ = static_cast<float>(datashare_->GetFloatValue(AUDIO_BALANCE_KEY, 0));
    clickResponseTime_ = static_cast<uint32_t>(datashare_->GetIntValue(CLICK_RESPONCE_TIME, 0));
    ignoreRepeatClickTime_ = static_cast<uint32_t>(datashare_->GetIntValue(IGNORE_REPEAT_CLICK_TIME, 0));
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

void AccessibilitySettingsConfig::InitEnabledList()
{
    HILOG_DEBUG();
    if (datashare_ == nullptr) {
        return;
    }

    std::string strValue = datashare_->GetStringValue("BundleName/AbilityName/Capabilities", "");
    HILOG_DEBUG("Capabilities = %{public}s", strValue.c_str());
    Utils::StringToVector(strValue, enabledAbilityInfos_);
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
}

const std::vector<std::string> &AccessibilitySettingsConfig::GetEnabledAbilityInfos()
{
    HILOG_DEBUG();
    return enabledAbilityInfos_;
}

void AccessibilitySettingsConfig::UpdateEnabledAbilities(const std::vector<std::string> &vecvalue)
{
    HILOG_DEBUG();
    enabledAbilityInfos_ = vecvalue;
    if (datashare_ == nullptr) {
        return;
    }

    std::string stringOut = "";
    Utils::VectorToString(enabledAbilityInfos_, stringOut);
    datashare_->PutStringValue("BundleName/AbilityName/Capabilities", stringOut);
}

void AccessibilitySettingsConfig::ClearData()
{
    HILOG_DEBUG();
}
} // namespace Accessibility
} // namespace OHOS