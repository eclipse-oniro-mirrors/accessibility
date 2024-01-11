/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_SETTINGS_H
#define ACCESSIBILITY_SETTINGS_H

#include <string>
#include <vector>
#include "event_handler.h"

namespace OHOS {
namespace Accessibility {
class AccessibilitySettings {
public:
    AccessibilitySettings() = default;
    ~AccessibilitySettings() = default;

    void RegisterSettingsHandler(const std::shared_ptr<AppExecFwk::EventHandler> &handler);

    RetError SetScreenMagnificationState(const bool state);
    RetError SetShortKeyState(const bool state);
    RetError SetMouseKeyState(const bool state);
    RetError SetMouseAutoClick(const int32_t time);
    RetError SetShortkeyTarget(const std::string &name);
    RetError SetShortkeyMultiTarget(const std::vector<std::string> &name);
    RetError SetHighContrastTextState(const bool state);
    RetError SetInvertColorState(const bool state);
    RetError SetAnimationOffState(const bool state);
    RetError SetAudioMonoState(const bool state);
    RetError SetDaltonizationState(const bool state);
    RetError SetDaltonizationColorFilter(const uint32_t filter);
    RetError SetContentTimeout(const uint32_t time);
    RetError SetBrightnessDiscount(const float discount);
    RetError SetAudioBalance(const float balance);
    RetError SetClickResponseTime(const uint32_t time);
    RetError SetIgnoreRepeatClickState(const bool state);
    RetError SetIgnoreRepeatClickTime(const uint32_t time);

    RetError GetScreenMagnificationState(bool &state);
    RetError GetShortKeyState(bool &state);
    RetError GetMouseKeyState(bool &state);
    RetError GetMouseAutoClick(int32_t &time);
    RetError GetShortkeyTarget(std::string &name);
    RetError GetShortkeyMultiTarget(std::vector<std::string> &name);
    RetError GetHighContrastTextState(bool &state);
    RetError GetInvertColorState(bool &state);
    RetError GetAnimationOffState(bool &state);
    RetError GetAudioMonoState(bool &state);
    RetError GetDaltonizationState(bool &state);
    RetError GetDaltonizationColorFilter(uint32_t &type);
    RetError GetContentTimeout(uint32_t &timer);
    RetError GetBrightnessDiscount(float &brightness);
    RetError GetAudioBalance(float &balance);
    RetError GetClickResponseTime(uint32_t &time);
    RetError GetIgnoreRepeatClickState(bool &state);
    RetError GetIgnoreRepeatClickTime(uint32_t &time);

    void UpdateConfigState();
    void UpdateAudioBalance();
    void UpdateBrightnessDiscount();
    void UpdateContentTimeout();
    void UpdateDaltonizationColorFilter();
    void UpdateMouseAutoClick();
    void UpdateShortkeyTarget();
    void UpdateShortkeyMultiTarget();
    void UpdateClickResponseTime();
    void UpdateIgnoreRepeatClickTime();
    void UpdateSettingsInAtoHos();
    void UpdateSettingsInAtoHosStatePart(ConfigValueAtoHosUpdate &value);

    RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption);
    RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption);
    RetError SetCaptionState(const bool state);
    RetError GetCaptionState(bool &state);

    void UpdateCaptionProperty();
    void UpdateAllSetting();

    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SETTINGS_H