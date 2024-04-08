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

namespace OHOS {
namespace Accessibility {
AccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("start.");
    accountId_ = id;
}

RetError AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("start.");
    enabled_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start.");
    eventTouchGuideState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("start.");
    gesturesSimulation_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start.");
    filteringKeyEvents_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start.");
    isCaptionState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start.");
    isScreenMagnificationState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isMouseKeyState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start.");
    mouseAutoClick_ = time;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start.");
    shortkeyTarget_ = name;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG("start.");
    shortkeyMultiTarget_ = name;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortkeyMultiTargetInPkgRemove(const std::string &name)
{
    HILOG_DEBUG("start.");

    for (auto iter = shortkeyMultiTarget_.begin(); iter != shortkeyMultiTarget_.end(); ++iter) {
        if (*iter == name) {
            shortkeyMultiTarget_.erase(iter);
            break;
        }
    }
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG("start.");
    daltonizationState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start.");
    invertColorState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start.");
    animationOffState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start.");
    audioMonoState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    daltonizationColorFilter_ = filter;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start.");
    contentTimeout_ = time;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start.");
    brightnessDiscount_ = discount;
    return RET_OK;
}

RetError SetStartFromAtoHosState(const bool state)
{
    HILOG_DEBUG("start.");
    (void)state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start.");
    audioBalance_ = balance;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start.");
    captionProperty_ = caption;
    return RET_OK;
}

bool AccessibilitySettingsConfig::GetCaptionState() const
{
    HILOG_DEBUG("start.");
    return isCaptionState_;
}

bool AccessibilitySettingsConfig::GetScreenMagnificationState() const
{
    HILOG_DEBUG("start.");
    return isScreenMagnificationState_;
}

bool AccessibilitySettingsConfig::GetShortKeyState() const
{
    HILOG_DEBUG("start.");
    return isShortKeyState_;
}

bool AccessibilitySettingsConfig::GetMouseKeyState() const
{
    HILOG_DEBUG("start.");
    return isMouseKeyState_;
}

int32_t AccessibilitySettingsConfig::GetMouseAutoClick() const
{
    HILOG_DEBUG("start.");
    return mouseAutoClick_;
}

const std::string &AccessibilitySettingsConfig::GetShortkeyTarget() const
{
    HILOG_DEBUG("start.");
    return shortkeyTarget_;
}

const std::vector<std::string> &AccessibilitySettingsConfig::GetShortkeyMultiTarget() const
{
    return shortkeyMultiTarget_;
}

bool AccessibilitySettingsConfig::GetHighContrastTextState() const
{
    HILOG_DEBUG("start.");
    return highContrastTextState_;
}

bool AccessibilitySettingsConfig::GetDaltonizationState() const
{
    HILOG_DEBUG("start.");
    return daltonizationState_;
}

bool AccessibilitySettingsConfig::GetInvertColorState() const
{
    HILOG_DEBUG("start.");
    return invertColorState_;
}

bool AccessibilitySettingsConfig::GetAnimationOffState() const
{
    HILOG_DEBUG("start.");
    return animationOffState_;
}

bool AccessibilitySettingsConfig::GetAudioMonoState() const
{
    HILOG_DEBUG("start.");
    return audioMonoState_;
}

uint32_t AccessibilitySettingsConfig::GetDaltonizationColorFilter() const
{
    HILOG_DEBUG("start.");
    return daltonizationColorFilter_;
}

uint32_t AccessibilitySettingsConfig::GetContentTimeout() const
{
    HILOG_DEBUG("start.");
    return contentTimeout_;
}

float AccessibilitySettingsConfig::GetBrightnessDiscount() const
{
    HILOG_DEBUG("start.");
    return brightnessDiscount_;
}

float AccessibilitySettingsConfig::GetAudioBalance() const
{
    HILOG_DEBUG("start.");
    return audioBalance_;
}

bool AccessibilitySettingsConfig::GetEnabledState() const
{
    HILOG_DEBUG("start.");
    return enabled_;
}

bool AccessibilitySettingsConfig::GetTouchGuideState() const
{
    HILOG_DEBUG("start.");
    return eventTouchGuideState_;
}

bool AccessibilitySettingsConfig::GetGestureState() const
{
    HILOG_DEBUG("start.");
    return gesturesSimulation_;
}

bool AccessibilitySettingsConfig::GetKeyEventObserverState() const
{
    HILOG_DEBUG("start.");
    return filteringKeyEvents_;
}

uint32_t AccessibilitySettingsConfig::GetClickResponseTime() const
{
    HILOG_DEBUG("start.");
    return clickResponseTime_;
}

bool AccessibilitySettingsConfig::GetIgnoreRepeatClickState() const
{
    HILOG_DEBUG("start.");
    return ignoreRepeatClickState_;
}

uint32_t AccessibilitySettingsConfig::GetIgnoreRepeatClickTime() const
{
    HILOG_DEBUG("start.");
    return ignoreRepeatClickTime_;
}

const AccessibilityConfig::CaptionProperty &AccessibilitySettingsConfig::GetCaptionProperty() const
{
    HILOG_DEBUG("start.");
    return captionProperty_;
};

uint32_t AccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;

    return state;
}

bool AccessibilitySettingsConfig::GetStartFromAtoHosState()
{
    return true;
}

void AccessibilitySettingsConfig::InitCaption()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::InitCapability()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::InitEnabledList()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::Init()
{
    HILOG_DEBUG("start.");
}

const std::vector<std::string> &AccessibilitySettingsConfig::GetEnabledAbilityInfos()
{
    HILOG_DEBUG("start.");
    return enabledAbilityInfos_;
}

void AccessibilitySettingsConfig::UpdateEnabledAbilities(const std::vector<std::string> &vecvalue)
{
    HILOG_DEBUG("start.");
    enabledAbilityInfos_ = vecvalue;
}

void AccessibilitySettingsConfig::ClearData()
{
    HILOG_DEBUG("start.");
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG("start.");
    ignoreRepeatClickState_ = state;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG("start.");
    ignoreRepeatClickTime_ = time;
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetStartFromAtoHosState(const bool state)
{
    HILOG_DEBUG("start.");
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetClickResponseTime(const uint32_t time)
{
    HILOG_DEBUG("start.");
    clickResponseTime_ = time;
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS