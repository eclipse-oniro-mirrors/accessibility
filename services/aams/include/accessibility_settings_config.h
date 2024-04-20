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
#ifndef ACCESSIBILITY_SETTINGS_CONFIG_H
#define ACCESSIBILITY_SETTINGS_CONFIG_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "accessibility_caption.h"
#include "accessibility_datashare_helper.h"

namespace OHOS {
namespace Accessibility {

class AccessibilitySettingsConfig final {
public:
    explicit AccessibilitySettingsConfig(int id);
    ~AccessibilitySettingsConfig() = default;

    RetError SetEnabled(const bool state);
    RetError SetTouchGuideState(const bool state);
    RetError SetGestureState(const bool state);
    RetError SetKeyEventObserverState(const bool state);
    RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption);
    RetError SetCaptionState(const bool state);
    RetError SetScreenMagnificationState(const bool state);
    RetError SetShortKeyState(const bool state);
    RetError SetMouseKeyState(const bool state);
    RetError SetMouseAutoClick(const int32_t time);
    RetError SetShortkeyTarget(const std::string &name);
    RetError SetShortkeyMultiTarget(const std::vector<std::string> &name);
    RetError SetShortkeyMultiTargetInPkgRemove(const std::string &name);
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
    RetError SetStartFromAtoHosState(const bool state);

    bool GetEnabledState() const;
    bool GetTouchGuideState() const;
    bool GetGestureState() const;
    bool GetKeyEventObserverState() const;
    bool GetCaptionState() const;
    bool GetScreenMagnificationState() const;
    bool GetShortKeyState() const;
    bool GetMouseKeyState() const;
    int32_t GetMouseAutoClick() const;
    const std::string &GetShortkeyTarget() const;
    const std::vector<std::string> &GetShortkeyMultiTarget() const;
    bool GetHighContrastTextState() const;
    bool GetInvertColorState() const;
    bool GetAnimationOffState() const;
    bool GetAudioMonoState() const;
    bool GetDaltonizationState() const;
    uint32_t GetDaltonizationColorFilter() const;
    uint32_t GetContentTimeout() const;
    float GetBrightnessDiscount() const;
    float GetAudioBalance() const;
    const AccessibilityConfig::CaptionProperty &GetCaptionProperty() const;
    uint32_t GetClickResponseTime() const;
    bool GetIgnoreRepeatClickState() const;
    uint32_t GetIgnoreRepeatClickTime() const;

    void UpdateEnabledAbilities(const std::vector<std::string> &vecvalue);
    const std::vector<std::string> &GetEnabledAbilityInfos();
    uint32_t GetConfigState();
    bool GetStartFromAtoHosState();

    void Init();
    void ClearData();
private:
    void InitCaption();
    void InitSetting();
    void InitCapability();
    void InitEnabledList();
    RetError SetConfigState(const std::string& key, bool value);

    int32_t accountId_;
    bool enabled_ = false;
    bool eventTouchGuideState_ = false;
    bool gesturesSimulation_ = false;
    bool filteringKeyEvents_ = false;
    bool isScreenMagnificationState_ = false;
    bool isCaptionState_ = false;
    AccessibilityConfig::CaptionProperty captionProperty_;
    bool isMouseKeyState_ = false;
    bool isShortKeyState_ = false;
    int32_t mouseAutoClick_ = -1;
    std::string shortkeyTarget_ = "";
    bool highContrastTextState_ = false;
    bool invertColorState_ = false;
    bool animationOffState_ = false;
    bool audioMonoState_ = false;
    bool daltonizationState_ = false;
    uint32_t daltonizationColorFilter_ = 0;
    uint32_t contentTimeout_ = 0;
    float brightnessDiscount_ = 0.0;
    float audioBalance_ = 0.0;
    uint32_t clickResponseTime_ = 0;
    bool ignoreRepeatClickState_ = false;
    uint32_t ignoreRepeatClickTime_ = 0;

    std::vector<std::string> shortkeyMultiTarget_ {};
    std::vector<std::string> enabledAbilityInfos_; // bundleName/abilityName/capabilities

    std::shared_ptr<AccessibilityDatashareHelper> datashare_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SETTINGS_CONFIG_H