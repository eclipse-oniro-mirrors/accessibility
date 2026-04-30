/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#include <atomic>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "accessibility_caption.h"
#include "accessibility_datashare_helper.h"
#include "magnification_def.h"

namespace OHOS {
namespace Accessibility {

enum class A11yDarkModeType : uint32_t {
    DEFAULT_DARK_MODE_STATE,
    DARK_MODE_OPEN_ALL_DAY,
    DARK_MODE_TIMED_ON
};

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
    RetError SetMagnificationState(const bool state);
    RetError SetScreenMagnificationType(const uint32_t type);
    RetError SetScreenMagnificationMode(const uint32_t mode);
    RetError SetScreenMagnificationScale(const float scale);
    RetError SetScreenMagnificationTriggerMethod(const int32_t triggerMethod);
    RetError SetShortKeyState(const bool state);
    RetError SetShortKeyOnLockScreenState(const bool state);
    RetError SetShortKeyTimeout(const int32_t time);
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
    RetError SetStartToHosState(const bool state);
    RetError SetIgnoreRepeatClickReconfirm(const bool state);
    RetError SetZoomGestureEnabledReconfirm(const bool state);
    RetError SetColorModeState(const A11yDarkModeType &type);
    RetError SetFlashReminderSwitch(const bool state);
    RetError SetSeniorModeState(const bool state);

    bool GetEnabledState() const;
    bool GetTouchGuideState() const;
    bool GetGestureState() const;
    bool GetKeyEventObserverState() const;
    bool GetCaptionState() const;
    bool GetScreenMagnificationState() const;
    bool GetShortKeyState() const;
    bool GetShortKeyOnLockScreenState() const;
    int32_t GetShortKeyTimeout() const;
    bool GetMouseKeyState() const;
    int32_t GetMouseAutoClick() const;
    const std::string &GetShortkeyTarget() const;
    const std::vector<std::string> GetShortkeyMultiTarget();
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
    uint32_t GetScreenMagnificationType() const;
    uint32_t GetScreenMagnificationMode() const;
    float GetScreenMagnificationScale() const;
    int32_t GetScreenMagnificationTriggerMethod() const;
    bool GetFlashReminderSwitch() const;
    bool GetSeniorModeState() const;

    RetError SetEnabledAccessibilityServices(const std::vector<std::string> &services);
    const std::vector<std::string> GetEnabledAccessibilityServices();
    RetError AddEnabledAccessibilityService(const std::string &serviceName);
    RetError RemoveEnabledAccessibilityService(const std::string &serviceName);
    uint32_t GetConfigState();
    bool GetStartToHosState();
    void SetDefaultShortcutKeyService();
    void InitSetting();
    void CloneShortkeyService(bool isScreenReaderEnabled);
    void OnDataClone();
    void CloneAudioState();
    void InitShortKeyConfig();
    uint32_t GetShortKeyService(std::vector<std::string> &services);
    std::shared_ptr<AccessibilityDatashareHelper> GetDbHandle()
    {
        return datashare_;
    }
    std::shared_ptr<AccessibilityDatashareHelper> GetSystemDbHandle()
    {
        return systemDatashare_;
    }

    void Init();
    bool GetInitializeState();
    void SetInitializeState(bool isInitialized);
private:
    void InitCaption();
    void InitCapability();
    void InitPrivacySpaceConfig();
    void InitAnimationOffConfig();
    void HandleIgnoreRepeatClickState();
    void HandleIgnoreRepeatClickCache();
    RetError SetConfigState(const std::string& key, bool value);

    void CloneOnDeviceCapability();
    void recoverCapability(int32_t capability);
    void recoverColorCorrection();
    void recoverInvertColor();
    void recoverAudioAdjustment();

    int32_t accountId_;
    std::atomic<bool> enabled_ = false;
    std::atomic<bool> eventTouchGuideState_ = false;
    std::atomic<bool> gesturesSimulation_ = false;
    std::atomic<bool> filteringKeyEvents_ = false;
    std::atomic<bool> isScreenMagnificationState_ = false;
    std::atomic<uint32_t> screenMagnificationType_ = 0;
    std::atomic<uint32_t> screenMagnificationMode_ = 0;
    std::atomic<float> screenMagnificationScale_ = 2.0f;
    std::atomic<int32_t> screenMagnificationTriggerMethod_ = THREE_FINGER_DOUBLE_TAP_MODE;
    std::atomic<bool> isCaptionState_ = false;
    AccessibilityConfig::CaptionProperty captionProperty_;
    std::atomic<bool> isMouseKeyState_ = false;
    std::atomic<bool> isShortKeyState_ = false;
    std::atomic<int32_t> mouseAutoClick_ = -1;
    std::string shortkeyTarget_ = "";
    std::atomic<bool> highContrastTextState_ = false;
    std::atomic<bool> invertColorState_ = false;
    std::atomic<bool> animationOffState_ = false;
    std::atomic<bool> audioMonoState_ = false;
    std::atomic<bool> flashReminderSwitch_ = false;
    std::atomic<bool> daltonizationState_ = false;
    std::atomic<bool> seniorModeState_ = false;
    std::atomic<uint32_t> daltonizationColorFilter_ = 0;
    std::atomic<uint32_t> contentTimeout_ = 0;
    std::atomic<float> brightnessDiscount_ = 0.0;
    std::atomic<float> audioBalance_ = 0.0;
    std::atomic<uint32_t> clickResponseTime_ = 0;
    std::atomic<bool> ignoreRepeatClickState_ = false;
    std::atomic<uint32_t> ignoreRepeatClickTime_ = 0;
    std::atomic<bool> isShortKeyEnabledOnLockScreen_ = false;
    std::atomic<int32_t> shortKeyTimeout_ = 3;

    std::vector<std::string> shortkeyMultiTarget_ {};
    std::vector<std::string> enabledAccessibilityServices_ {}; // bundleName/abilityName

    std::shared_ptr<AccessibilityDatashareHelper> datashare_ = nullptr;
    std::shared_ptr<AccessibilityDatashareHelper> systemDatashare_ = nullptr;
    ffrt::mutex interfaceMutex_;
    bool isInitialized_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SETTINGS_CONFIG_H