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
MockAccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("start.");
    accountId_ = id;
}

RetError MockAccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("start.");
    enabled_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start.");
    eventTouchGuideState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("start.");
    gesturesSimulation_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start.");
    filteringKeyEvents_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start.");
    isCaptionState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start.");
    isScreenMagnificationState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetScreenMagnificationType(const uint32_t type)
{
    HILOG_DEBUG("start.");
    screenMagnificationType_ = type;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortKeyOnLockScreenState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyEnabledOnLockScreen_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortKeyTimeout(const int32_t time)
{
    HILOG_DEBUG("start.");
    shortKeyTimeout_ = time;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isMouseKeyState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start.");
    mouseAutoClick_ = time;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start.");
    shortkeyTarget_ = name;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG("start.");
    shortkeyMultiTarget_ = name;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetShortkeyMultiTargetInPkgRemove(const std::string &name)
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

RetError MockAccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG("start.");
    daltonizationState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start.");
    invertColorState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start.");
    animationOffState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start.");
    audioMonoState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    daltonizationColorFilter_ = filter;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start.");
    contentTimeout_ = time;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
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

RetError MockAccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start.");
    audioBalance_ = balance;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start.");
    captionProperty_ = caption;
    return RET_OK;
}

bool MockAccessibilitySettingsConfig::GetCaptionState() const
{
    HILOG_DEBUG("start.");
    return isCaptionState_;
}

bool MockAccessibilitySettingsConfig::GetScreenMagnificationState() const
{
    HILOG_DEBUG("start.");
    return isScreenMagnificationState_;
}

bool MockAccessibilitySettingsConfig::GetShortKeyState() const
{
    HILOG_DEBUG("start.");
    return isShortKeyState_;
}

bool MockAccessibilitySettingsConfig::GetShortKeyOnLockScreenState() const
{
    HILOG_DEBUG("start.");
    return isShortKeyEnabledOnLockScreen_;
}

int32_t MockAccessibilitySettingsConfig::GetShortKeyTimeout() const
{
    HILOG_DEBUG("start.");
    return shortKeyTimeout_;
}

bool MockAccessibilitySettingsConfig::GetMouseKeyState() const
{
    HILOG_DEBUG("start.");
    return isMouseKeyState_;
}

int32_t MockAccessibilitySettingsConfig::GetMouseAutoClick() const
{
    HILOG_DEBUG("start.");
    return mouseAutoClick_;
}

const std::string &MockAccessibilitySettingsConfig::GetShortkeyTarget() const
{
    HILOG_DEBUG("start.");
    return shortkeyTarget_;
}

const std::vector<std::string> MockAccessibilitySettingsConfig::GetShortkeyMultiTarget()
{
    return shortkeyMultiTarget_;
}

bool MockAccessibilitySettingsConfig::GetHighContrastTextState() const
{
    HILOG_DEBUG("start.");
    return highContrastTextState_;
}

bool MockAccessibilitySettingsConfig::GetDaltonizationState() const
{
    HILOG_DEBUG("start.");
    return daltonizationState_;
}

bool MockAccessibilitySettingsConfig::GetInvertColorState() const
{
    HILOG_DEBUG("start.");
    return invertColorState_;
}

bool MockAccessibilitySettingsConfig::GetAnimationOffState() const
{
    HILOG_DEBUG("start.");
    return animationOffState_;
}

bool MockAccessibilitySettingsConfig::GetAudioMonoState() const
{
    HILOG_DEBUG("start.");
    return audioMonoState_;
}

uint32_t MockAccessibilitySettingsConfig::GetDaltonizationColorFilter() const
{
    HILOG_DEBUG("start.");
    return daltonizationColorFilter_;
}

uint32_t MockAccessibilitySettingsConfig::GetContentTimeout() const
{
    HILOG_DEBUG("start.");
    return contentTimeout_;
}

float MockAccessibilitySettingsConfig::GetBrightnessDiscount() const
{
    HILOG_DEBUG("start.");
    return brightnessDiscount_;
}

float MockAccessibilitySettingsConfig::GetAudioBalance() const
{
    HILOG_DEBUG("start.");
    return audioBalance_;
}

bool MockAccessibilitySettingsConfig::GetEnabledState() const
{
    HILOG_DEBUG("start.");
    return enabled_;
}

bool MockAccessibilitySettingsConfig::GetTouchGuideState() const
{
    HILOG_DEBUG("start.");
    return eventTouchGuideState_;
}

bool MockAccessibilitySettingsConfig::GetGestureState() const
{
    HILOG_DEBUG("start.");
    return gesturesSimulation_;
}

bool MockAccessibilitySettingsConfig::GetKeyEventObserverState() const
{
    HILOG_DEBUG("start.");
    return filteringKeyEvents_;
}

uint32_t MockAccessibilitySettingsConfig::GetClickResponseTime() const
{
    HILOG_DEBUG("start.");
    return clickResponseTime_;
}

bool MockAccessibilitySettingsConfig::GetIgnoreRepeatClickState() const
{
    HILOG_DEBUG("start.");
    return ignoreRepeatClickState_;
}

uint32_t MockAccessibilitySettingsConfig::GetIgnoreRepeatClickTime() const
{
    HILOG_DEBUG("start.");
    return ignoreRepeatClickTime_;
}

const AccessibilityConfig::CaptionProperty &MockAccessibilitySettingsConfig::GetCaptionProperty() const
{
    HILOG_DEBUG("start.");
    return captionProperty_;
};

uint32_t MockAccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;

    return state;
}

bool MockAccessibilitySettingsConfig::GetStartFromAtoHosState()
{
    return true;
}

void MockAccessibilitySettingsConfig::InitCaption()
{
    HILOG_DEBUG("start.");
}

void MockAccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG("start.");
}

void MockAccessibilitySettingsConfig::InitCapability()
{
    HILOG_DEBUG("start.");
}

void MockAccessibilitySettingsConfig::Init()
{
    HILOG_DEBUG("start.");
}

void MockAccessibilitySettingsConfig::ClearData()
{
    HILOG_DEBUG("start.");
}

RetError MockAccessibilitySettingsConfig::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG("start.");
    ignoreRepeatClickState_ = state;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG("start.");
    ignoreRepeatClickTime_ = time;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetStartFromAtoHosState(const bool state)
{
    HILOG_DEBUG("start.");
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::SetClickResponseTime(const uint32_t time)
{
    HILOG_DEBUG("start.");
    clickResponseTime_ = time;
    return RET_OK;
}

const std::vector<std::string> MockAccessibilitySettingsConfig::GetEnabledAccessibilityServices()
{
    return enabledAccessibilityServices_;
}

RetError MockAccessibilitySettingsConfig::AddEnabledAccessibilityService(const std::string &serviceName)
{
    (void)serviceName;
    return RET_OK;
}

RetError MockAccessibilitySettingsConfig::RemoveEnabledAccessibilityService(const std::string &serviceName)
{
    (void)serviceName;
    return RET_OK;
}

void MockAccessibilitySettingsConfig::CloneAudioState()
{
}

void MockAccessibilitySettingsConfig::CloneShortkeyService(bool isScreenReaderEnabled)
{
    (void)isScreenReaderEnabled;
}

void MockAccessibilitySettingsConfig::OnDataClone()
{
}
} // namespace Accessibility
} // namespace OHOS