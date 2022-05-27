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

#ifndef INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H
#define INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H

#include <map>
#include <vector>
#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_event_info.h"
#include "i_accessibility_element_operator.h"
#include "i_accessibility_enable_ability_lists_observer.h"
#include "i_accessible_ability_manager_caption_observer.h"
#include "i_accessible_ability_manager_config_observer.h"
#include "i_accessible_ability_manager_state_observer.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class IAccessibleAbilityManagerService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerService");

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SendEvent(const AccessibilityEventInfo &uiEvent) = 0;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) = 0;

    virtual uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) = 0;

    virtual void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  4 indicates that the ability has been installed.
     * @param infos accessibility ability infos by specified types.
     * @return Return true if get ability lists successfully, else return false.
     */
    virtual bool GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) = 0;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual void RegisterElementOperator(
        const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) = 0;

    virtual AccessibilityConfig::CaptionProperty GetCaptionProperty() = 0;
    virtual bool GetEnabledState() = 0;
    virtual bool GetCaptionState() = 0;
    virtual bool GetTouchGuideState() = 0;
    virtual bool GetGestureState() = 0;
    virtual bool GetKeyEventObserverState() = 0;

    virtual bool SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) = 0;
    virtual bool SetCaptionState(const bool state) = 0;

    virtual bool EnableAbility(const std::string &name, const uint32_t capabilities) = 0;
    virtual bool GetEnabledAbilities(std::vector<std::string> &enabledAbilities) = 0;
    virtual bool GetInstalledAbilities(std::vector<AccessibilityAbilityInfo> &installedAbilities) = 0;

    virtual bool DisableAbility(const std::string &name) = 0;
    virtual int32_t GetActiveWindow() = 0;

    virtual RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) = 0;
    virtual bool DisableUITestAbility() = 0;

    virtual bool SetScreenMagnificationState(const bool state) = 0;
    virtual bool SetShortKeyState(const bool state) = 0;
    virtual bool SetMouseKeyState(const bool state) = 0;
    virtual bool SetMouseAutoClick(const int32_t time) = 0;
    virtual bool SetShortkeyTarget(const std::string &name) = 0;
    virtual bool SetHighContrastTextState(const bool state) = 0;
    virtual bool SetInvertColorState(const bool state) = 0;
    virtual bool SetAnimationOffState(const bool state) = 0;
    virtual bool SetAudioMonoState(const bool state) = 0;
    virtual bool SetDaltonizationColorFilter(const uint32_t filter) = 0;
    virtual bool SetContentTimeout(const uint32_t time) = 0;
    virtual bool SetBrightnessDiscount(const float discount) = 0;
    virtual bool SetAudioBalance(const float balance) = 0;

    virtual bool GetScreenMagnificationState() = 0;
    virtual bool GetShortKeyState() = 0;
    virtual bool GetMouseKeyState() = 0;
    virtual int32_t GetMouseAutoClick() = 0;
    virtual std::string GetShortkeyTarget() = 0;
    virtual bool GetHighContrastTextState() = 0;
    virtual bool GetInvertColorState() = 0;
    virtual bool GetAnimationOffState() = 0;
    virtual bool GetAudioMonoState() = 0;
    virtual uint32_t GetDaltonizationColorFilter() = 0;
    virtual uint32_t GetContentTimeout() = 0;
    virtual float GetBrightnessDiscount() = 0;
    virtual float GetAudioBalance() = 0;

    virtual uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) = 0;
    enum class Message {
        SEND_EVENT = 0,
        REGISTER_STATE_CALLBACK,
        GET_ABILITYLIST,
        REGISTER_INTERACTION_CONNECTION,
        DEREGISTER_INTERACTION_CONNECTION,
        GET_CAPTION_PROPERTY,
        SET_CAPTION_PROPERTY,
        SET_CAPTION_STATE,
        REGISTER_CAPTION_PROPERTY_CALLBACK,
        GET_ENABLED,
        GET_CAPTION_STATE,
        GET_TOUCH_GUIDE_STATE,
        GET_GESTURE_STATE,
        GET_KEY_EVENT_OBSERVE_STATE,
        ENABLE_ABILITIES,
        GET_ENABLED_OBJECT,
        GET_INSTALLED,
        DISABLE_ABILITIES,
        ENABLE_UI_TEST_ABILITY,
        DISABLE_UI_TEST_ABILITY,
        GET_ACTIVE_WINDOW,
        SET_SCREENMAGNIFIER_STATE,
        SET_SHORTKEY_STATE,
        SET_MOUSEKEY_STATE,
        SET_SHORTKEY_TARGET,
        SET_MOUSEKEY_AUTOCLICK,
        SET_INVERTCOLOR_STATE,
        SET_HIGHCONTRASTTEXT_STATE,
        SET_AUDIOMONO_STATE,
        SET_ANIMATIONOFF_STATE,
        SET_DALTONIZATION_COLORFILTER,
        SET_CONTENT_TIMEOUT,
        SET_BRIGHTNESS_DISCOUNT,
        SET_AUDIO_BALANCE,
        GET_SCREENMAGNIFIER_STATE,
        GET_SHORTKEY_STATE,
        GET_MOUSEKEY_STATE,
        GET_SHORTKEY_TARGET,
        GET_MOUSEKEY_AUTOCLICK,
        GET_INVERTCOLOR_STATE,
        GET_HIGHCONTRASTTEXT_STATE,
        GET_AUDIOMONO_STATE,
        GET_ANIMATIONOFF_STATE,
        GET_DALTONIZATION_COLORFILTER,
        GET_CONTENT_TIMEOUT,
        GET_BRIGHTNESS_DISCOUNT,
        GET_AUDIO_BALANCE,
        REGISTER_ENABLE_ABILITY_LISTS_OBSERVER,
        REGISTER_CONFIG_CALLBACK,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H