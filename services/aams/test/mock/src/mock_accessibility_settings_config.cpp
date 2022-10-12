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

RetError AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
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

uint32_t AccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;

    return state;
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