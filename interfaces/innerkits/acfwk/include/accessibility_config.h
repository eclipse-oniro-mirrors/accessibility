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
#include "accessibility_singleton.h"

namespace OHOS {
namespace AccessibilityConfig {
enum CONFIG_ID : int32_t {
    CONFIG_HIGH_CONTRAST_TEXT = 0,
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
     */
    virtual ~AccessibilityConfigObserver() = default;

    /**
     * @brief Called when the config value changed.
     * @param id the id of config.
     * @param value the value of config.
     */
    virtual void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) = 0;
};

class AccessibilityEnableAbilityListsObserver {
public:
    /**
     * @brief Destruct
     */
    virtual ~AccessibilityEnableAbilityListsObserver() = default;

    /**
     * @brief Called when the enable ability lists changed.
     */
    virtual void OnEnableAbilityListsStateChanged() = 0;
};

class AccessibilityConfig {
    ACCESSIBILITY_DECLARE_SINGLETON(AccessibilityConfig)
public:
    /**
     * @brief Initialize the run context.
     * @return Return true if successfully, else return false.
     */
    bool InitializeContext();

    /**
     * @brief Subscribes to accessibility config value.
     * @param id  the config id which is observed.
     * @param observer Indicates the observer for listening to accessibility
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SubscribeConfigObserver(const CONFIG_ID id,
        const std::shared_ptr<AccessibilityConfigObserver> &observer, const bool retFlag = true);

    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param id  the id which is observed.
     * @param observer Indicates the registered accessibility config observer.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError UnsubscribeConfigObserver(const CONFIG_ID id,
        const std::shared_ptr<AccessibilityConfigObserver> &observer);

    /**
     * @brief Subscribes the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);

    /**
     * @brief Unsubscribe the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError UnsubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);

    /**
     * @brief Enabled specified abilities
     * @param name The string formatted by 'bundleName/abilityName'.
     * @param capabilities The capabilities you permit.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError EnableAbility(const std::string &name, const uint32_t capabilities);

    /**
     * @brief Disabled specified ability
     * @param name The string formatted by 'bundleName/abilityName'.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError DisableAbility(const std::string &name);

    /**
     * @brief Set whether to enable the magnification function
     * @param state true:enable magnification function; false:disable magnification function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetScreenMagnificationState(const bool state);

    /**
     * @brief Set whether to enable the function of using short key to open accessibility ability
     * @param state true:enable short key function; false:disable short key function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetShortKeyState(const bool state);

    /**
     * @brief Set whether to enable the mouse key function
     * @param state true:enable mouse key function; false:disable mouse key function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetMouseKeyState(const bool state);

    /**
     * @brief Set whether to enable the caption function
     * @param state true:enable caption function; false:disable caption function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetCaptionsState(const bool state);

    /**
     * @brief Set caption properties
     * @param caption caption properties
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetCaptionsProperty(const CaptionProperty &caption);

    /**
     * @brief Set the time for the cursor to stop for the mouse to automatically perform the click action
     * @param time The valid time is 1000ms~5000ms, otherwise the automatic mouse click action will not be enabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetMouseAutoClick(const int32_t time);

    /**
     * @brief Set the name of the accessibility ability to be opened by the short key
     * @param name name The string formatted by 'bundleName/abilityName'
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetShortkeyTarget(const std::string &name);

    /**
     * @brief Set whether to enable the high-contrast text function
     * @param state true:enable high-contrast text function; false:disable high-contrast text function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetHighContrastTextState(bool state);

    /**
     * @brief Set whether to enable the invert color function
     * @param state true:enable invert color function; false:disable invert color function
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetInvertColorState(const bool state);

    /**
     * @brief Set daltonization color filter
     * @param type Daltonization color filter type
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetDaltonizationColorFilter(const DALTONIZATION_TYPE type);

    /**
     * @brief Set content duration
     * @param timer duration
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetContentTimeout(const uint32_t timer);

    /**
     * @brief Set whether to turn off animation
     * @param state true:turn off animation; false:turn on animation
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetAnimationOffState(const bool state);

    /**
     * @brief Set brightness discount
     * @param brightness The discount of brightness
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetBrightnessDiscount(const float brightness);

    /**
     * @brief Set whether to enable audio mono
     * @param state true:enable audio mono; false:disable audio mono
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetAudioMonoState(const bool state);

    /**
     * @brief Set audio balance
     * @param balance The balance of audio
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError SetAudioBalance(const float balance);

    /**
     * @brief Get the status of whether the magnification function is enabled
     * @param state(out) true:the magnification function is enabled; false:the magnification function is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetScreenMagnificationState(bool &state) const;

    /**
     * @brief Get the status of whether the use of short key to open accessibility ability is enabled
     * @param state(out) true:the short key is enabled; false:the short key is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetShortKeyState(bool &state) const;

    /**
     * @brief Get the status of whether the mouse key function is enabled
     * @param state(out) true:the mouse key function is enabled; false:the mouse key function is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetMouseKeyState(bool &state) const;

    /**
     * @brief Get the status of whether the caption function is enabled
     * @param state(out) true:the caption function is enabled; false:the caption function is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetCaptionsState(bool &state) const;

    /**
     * @brief Get caption properties
     * @param caption(out) caption properties
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetCaptionsProperty(CaptionProperty &caption) const;

    /**
     * @brief Get the time for the cursor to stop for the mouse to automatically perform the click action
     * @param time(out) the time for the cursor to stop for the mouse to automatically perform the click action
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetMouseAutoClick(int32_t &time) const;

    /**
     * @brief Get the name of the accessibility ability to be opened by the short key
     * @param name(out) The string formatted by 'bundleName/abilityName'
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetShortkeyTarget(std::string &name) const;

    /**
     * @brief Get the status of whether the invert color function is enabled
     * @param state(out) true:the invert color function is enabled; false:the invert color function is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetInvertColorState(bool &state) const;

    /**
     * @brief Get the status of whether the high-contrast text function is enabled
     * @param state(out) true:the high-contrast text function is enabled;
     *                   false:the high-contrast text function is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetHighContrastTextState(bool &state) const;

    /**
     * @brief Get daltonization color filter
     * @param type(out) Daltonization color filter type
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetDaltonizationColorFilter(DALTONIZATION_TYPE &type) const;

    /**
     * @brief Get content duration
     * @param timer(out) duration
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetContentTimeout(uint32_t &timer) const;

    /**
     * @brief Get the status of whether animation is disabled
     * @param state(out) true:animation is disabled; false:animation is enabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetAnimationOffState(bool &state) const;

    /**
     * @brief Get brightness discount parameter
     * @param brightness(out) The discount parameter of brightness
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetBrightnessDiscount(float &brightness) const;

    /**
     * @brief Get the status of whether the audio mono is enabled
     * @param state(out) true:audio mono is enabled; false:audio mono is disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetAudioMonoState(bool &state) const;

    /**
     * @brief Get the value of the audio balance
     * @param balance(out) The value of the audio balance
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    Accessibility::RetError GetAudioBalance(float &balance) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} // namespace AccessibilityConfig
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_H
