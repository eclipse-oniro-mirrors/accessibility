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

bool AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("start.");
    enabled_ = state;
    return true;
}

bool AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start.");
    eventTouchGuideState_ = state;
    return true;
}

bool AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("start.");
    gesturesSimulation_ = state;
    return true;
}

bool AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start.");
    filteringKeyEvents_ = state;
    return true;
}


void AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start.");
    isCaptionState_ = state;
}

void AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start.");
    isScreenMagnificationState_ = state;
}

void AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyState_ = state;
}

void AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isMouseKeyState_ = state;
}

void AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start.");
    mouseAutoClick_ = time;
}

void AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start.");
    shortkeyTarget_ = name;
}

void AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
}

void AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start.");
    invertColorState_ = state;
}

void AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start.");
    animationOffState_ = state;
}

void AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start.");
    audioMonoState_ = state;
}

void AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    daltonizationColorFilter_ = filter;
}

void AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start.");
    contentTimeout_ = time;
}

void AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start.");
    brightnessDiscount_ = discount;
}

void AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start.");
    audioBalance_ = balance;
}

void AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start.");
    captionProperty_ = caption;
}

bool AccessibilitySettingsConfig::SetStatePref(int32_t type)
{
    HILOG_DEBUG("start.");
    (void)type;
    return true;
}

std::string AccessibilitySettingsConfig::StateChange(bool state)
{
    HILOG_DEBUG("start.");
    std::string STATE_TRUE = "on";
        return STATE_TRUE;
}

bool AccessibilitySettingsConfig::GetCaptionState()
{
    return isCaptionState_;
}

bool AccessibilitySettingsConfig::GetScreenMagnificationState()
{
    return isScreenMagnificationState_;
}

bool AccessibilitySettingsConfig::GetShortKeyState()
{
    return isShortKeyState_;
}

bool AccessibilitySettingsConfig::GetMouseKeyState()
{
    return isMouseKeyState_;
}

int32_t AccessibilitySettingsConfig::GetMouseAutoClick()
{
    return mouseAutoClick_;
}

const std::string &AccessibilitySettingsConfig::GetShortkeyTarget()
{
    return shortkeyTarget_;
}

bool AccessibilitySettingsConfig::GetHighContrastTextState()
{
    return highContrastTextState_;
}

bool AccessibilitySettingsConfig::GetInvertColorState()
{
    return invertColorState_;
}

bool AccessibilitySettingsConfig::GetAnimationOffState()
{
    return animationOffState_;
}

bool AccessibilitySettingsConfig::GetAudioMonoState()
{
    return audioMonoState_;
}

uint32_t AccessibilitySettingsConfig::GetDaltonizationColorFilter()
{
    return daltonizationColorFilter_;
}

uint32_t AccessibilitySettingsConfig::GetContentTimeout()
{
    return contentTimeout_;
}

float AccessibilitySettingsConfig::GetBrightnessDiscount()
{
    return brightnessDiscount_;
}

float AccessibilitySettingsConfig::GetAudioBalance()
{
    return audioBalance_;
}

bool AccessibilitySettingsConfig::GetEnabledState()
{
    return enabled_;
}

bool AccessibilitySettingsConfig::GetTouchGuideState()
{
    return eventTouchGuideState_;
}

bool AccessibilitySettingsConfig::GetGestureState()
{
    return gesturesSimulation_;
}

bool AccessibilitySettingsConfig::GetKeyEventObserverState()
{
    return filteringKeyEvents_;
}

const AccessibilityConfig::CaptionProperty &AccessibilitySettingsConfig::GetCaptionProperty()
{
    return captionProperty_;
};

uint32_t AccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;

    return state;
}

void AccessibilitySettingsConfig::CaptionInit()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::SettingInit()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::CapabilityInit()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::EnabledListInit()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::Init()
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::VectorToString(const std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG("start.");
}

void AccessibilitySettingsConfig::StringToVector(const std::string &stringIn, std::vector<std::string> &vectorResult)
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
} // namespace Accessibility
} // namespace OHOS