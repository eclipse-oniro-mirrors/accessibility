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

#ifndef ACCESSIBILITY_CONFIG_H
#define ACCESSIBILITY_CONFIG_H

#include <cstdint>
#include "accessibility_caption.h"
#include "singleton.h"
#include <string>

namespace OHOS {
namespace AccessibilityConfig {
enum CONFIG_ID : int32_t {
    CONFIG_HIGH_CONTRASTE_TEXT = 0,
    CONFIG_INVERT_COLOR,
    CONFIG_DALTONIZATION_COLOR_FILTER,
    CONFIG_CONTENT_TIMEOUT,
    CONFIG_ANIMATION_OFF,
    CONFIG_BRIGHTNESS_DISCOUNT,
    CONFIG_AUDIO_MONO,
    CONFIG_AUDIO_BALANCE,
    CONFIG_MOUSE_KEY,
    CONFIG_SHORT_KEY,
    CONFIG_CAPTION_STATE,
    CONFIG_CAPTION_STYLE,
    CONFIG_SCREEN_MAGNIFICATION,
    CONFIG_SHORT_KEY_TARGET,
    CONFIG_MOUSE_AUTOCLICK,   
    CONFIG_ID_MAX,
};

enum DALTONIZATION_TYPE : uint32_t {
    Normal = 0,
    Protanomaly,
    Deuteranomaly,
    Tritanomaly,
};
#pragma pack(4)
struct ConfigValue {
    bool highContrastText;
    bool invertColor;
    bool animationOff;
    bool screenMagnifier;
    bool audioMono;
    bool mouseKey;
    bool shortkey;
    bool captionState;
    DALTONIZATION_TYPE daltonizationColorFilter;
    uint32_t contentTimeout;
    int32_t mouseAutoClick;
    float brightnessDiscount;
    float audioBalance;
    std::string shortkey_target;
    CaptionProperty captionStyle;
};
#pragma pack()

class AccessibilityConfigObserver {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~AccessibilityConfigObserver() = default;

    /**
     * @brief Called when the config value changed.
     * @param id the id of config.
     * @param value the value of config.
     * @return
     */
    virtual void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) = 0;
};

class AccessibilityEnableAbilityListsObserver {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~AccessibilityEnableAbilityListsObserver() = default;

    /**
     * @brief Called when the enable ability lists changed.
     * @return
     */
    virtual void OnEnableAbilityListsStateChanged() = 0;
};

class AccessibilityConfig {
public:
    static std::shared_ptr<AccessibilityConfig> GetInstance();
    virtual ~AccessibilityConfig() = default;
    /**
     * @brief Subscribes to accessibility config value.
     * @param ids  the config ids which are observed.
     * @param observer Indicates the observer for listening to accessibility
     * config.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer) = 0;

    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param ids  the ids which are observed.
     * @param observer Indicates the registered accessibility config observer.
     * @return true is succeed otherwise is failed.
     */
    virtual void UnsubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer) = 0;

    /**
     * @brief Subscribes the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    virtual void SubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer) = 0;

    /**
     * @brief Unsubscribe the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    virtual void UnsubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer) = 0;

    /**
     * @brief Enabled specified abilities
     * @param name The strings formated by 'bundleName/abilityName'.
     * @param capabilities The capabilities you permit.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableAbilities(const std::string name, const uint32_t capabilities) = 0;

    /**
     * @brief Disabled specified abilities
     * @param name The strings formated by 'bundleName/abilityName'.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool DisableAbilities(const std::string name) = 0;

    virtual bool SetScreenMagnificationState(const bool state) = 0;
    virtual bool SetShortKeyState(const bool state) = 0;
    virtual bool SetMouseKeyState(const bool state) = 0;
    virtual bool SetCaptionState(const bool state) = 0;
    virtual bool SetCaptionProperty(const CaptionProperty &caption) = 0;
    virtual bool SetMouseAutoClick(const int32_t time) = 0;
    virtual bool SetShortkeyTarget(const std::string &name) = 0;
    virtual bool SetHighContrastTextState(bool state) = 0;
    virtual bool SetInvertColorState(const bool state) = 0;
    virtual bool SetDaltonizationColorFilter(const DALTONIZATION_TYPE type) = 0;
    virtual bool SetContentTimeout(const uint32_t timer) = 0;
    virtual bool SetAnimationOffState(const bool state) = 0;
    virtual bool SetBrightnessDiscount(const float brightness) = 0;
    virtual bool SetAudioMonoState(const bool state) = 0;
    virtual bool SetAudioBalance(const float balance) = 0;

    virtual bool GetScreenMagnificationState(bool &value) = 0;
    virtual bool GetShortKeyState(bool &value) = 0;
    virtual bool GetMouseKeyState(bool &value) = 0;
    virtual bool GetCaptionState(bool &value) = 0;
    virtual bool GetCaptionProperty(CaptionProperty &value) = 0;
    virtual bool GetMouseAutoClick(int32_t &value) = 0;
    virtual bool GetShortkeyTarget(std::string &value) = 0;
    virtual bool GetInvertColorState(bool &value) = 0;
    virtual bool GetHighContrastTextState(bool &value) = 0;
    virtual bool GetDaltonizationColorFilter(DALTONIZATION_TYPE &value) = 0;
    virtual bool GetContentTimeout(uint32_t &value)= 0;
    virtual bool GetAnimationOffState(bool &value) = 0;
    virtual bool GetBrightnessDiscount(float &value) = 0;
    virtual bool GetAudioMonoState(bool &value)= 0;
    virtual bool GetAudioBalance(float &value) = 0;

    virtual void UpdateScreenMagnificationEnabled(const bool enabled) = 0;
    virtual void UpdateShortKeyEnabled(const bool enabled) = 0;
    virtual void UpdateMouseKeyEnabled(const bool enabled) = 0;
    virtual void UpdateCaptionEnabled(const bool enabled) = 0;
};
} // namespace AccessibilityConfig
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_H
