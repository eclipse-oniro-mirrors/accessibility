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

#include <memory>
#include "accessibility_caption.h"
#include "singleton.h"

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
    DECLARE_SINGLETON(AccessibilityConfig)
public:
    /**
     * @brief Subscribes to accessibility config value.
     * @param id  the config id which is observed.
     * @param observer Indicates the observer for listening to accessibility
     * @return -
     */
    void SubscribeConfigObserver(const CONFIG_ID id, const std::shared_ptr<AccessibilityConfigObserver> &observer);

    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param id  the id which is observed.
     * @param observer Indicates the registered accessibility config observer.
     * @return -
     */
    void UnsubscribeConfigObserver(const CONFIG_ID id, const std::shared_ptr<AccessibilityConfigObserver> &observer);

    /**
     * @brief Subscribes the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    void SubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);

    /**
     * @brief Unsubscribe the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    void UnsubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);

    /**
     * @brief Enabled specified abilities
     * @param name The strings formated by 'bundleName/abilityName'.
     * @param capabilities The capabilities you permit.
     * @return Return true if the command is sent successfully, else return false.
     */
    bool EnableAbility(const std::string &name, const uint32_t capabilities);

    /**
     * @brief Disabled specified ability
     * @param name The strings formated by 'bundleName/abilityName'.
     * @return Return true if the command is sent successfully, else return false.
     */
    bool DisableAbility(const std::string &name);

    void SetScreenMagnificationState(const bool state);
    void SetShortKeyState(const bool state);
    void SetMouseKeyState(const bool state);
    void SetCaptionState(const bool state);
    void SetCaptionProperty(const CaptionProperty &caption);
    void SetMouseAutoClick(const int32_t time);
    void SetShortkeyTarget(const std::string &name);
    void SetHighContrastTextState(bool state);
    void SetInvertColorState(const bool state);
    void SetDaltonizationColorFilter(const DALTONIZATION_TYPE type);
    void SetContentTimeout(const uint32_t timer);
    void SetAnimationOffState(const bool state);
    void SetBrightnessDiscount(const float brightness);
    void SetAudioMonoState(const bool state);
    void SetAudioBalance(const float balance);

    void GetScreenMagnificationState(bool &state) const;
    void GetShortKeyState(bool &state) const;
    void GetMouseKeyState(bool &state) const;
    void GetCaptionState(bool &state) const;
    void GetCaptionProperty(CaptionProperty &caption) const;
    void GetMouseAutoClick(int32_t &time) const;
    void GetShortkeyTarget(std::string &name) const;
    void GetInvertColorState(bool &state) const;
    void GetHighContrastTextState(bool &state) const;
    void GetDaltonizationColorFilter(DALTONIZATION_TYPE &type) const;
    void GetContentTimeout(uint32_t &timer) const;
    void GetAnimationOffState(bool &state) const;
    void GetBrightnessDiscount(float &brightness) const;
    void GetAudioMonoState(bool &state) const;
    void GetAudioBalance(float &balance) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} // namespace AccessibilityConfig
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_H
