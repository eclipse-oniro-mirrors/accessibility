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

#ifndef ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H
#define ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H

#include <map>
#include <vector>
#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_element_operator.h"
#include "accessibility_event_info.h"
#include "accessibility_state_event.h"
#include "accessibility_window_info.h"

namespace OHOS {
namespace Accessibility {
enum AccessibilityControlType : int32_t {
    CONTENT_CONTROLS = 0x00000001,
    CONTENT_ICONS = 0x00000002,
    CONTENT_TEXT = 0x00000004,
};

/*
 * The class register the accessibility service observer to AAMS,and
 * dispatch the accessibility service status changed. such as Service Enable，
 * Accessibility Enable. It calls AAMS API to send the event to AA.
 * It supply sington instance for each process.
 */
class AccessibilitySystemAbilityClient {
public:
    static const int32_t NUM_INT32 = 32;

    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param -
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance();

    /**
     * @brief Desconstruct.
     * @param
     * @return
     */
    virtual ~AccessibilitySystemAbilityClient() = default;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual int32_t RegisterElementOperator(const int32_t windowId,
        const std::shared_ptr<AccessibilityElementOperator> &operation, int32_t accountId) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) = 0;

    /**
     * @brief Checks whether accessibility ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsEnabled() = 0;

    /**
     * @brief Checks whether touch exploration ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsTouchExplorationEnabled() = 0;

    /**
     * @brief Checks whether Caption ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsCaptionEnabled() = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  4 indicates that the ability has been installed.
     * @return Return accessibility ability infos by specified types.
     */
    virtual std::vector<AccessibilityAbilityInfo> GetAbilityList(
        const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType) = 0;

    /**
     * @brief Obtains the properties of the accessibility caption function.
     * Remained for caption.
     * @param -
     * @return Returns the properties of the accessibility caption function.
     */
    virtual CaptionProperty GetCaptionProperty() = 0;

    /**
     * @brief Set the properties of the accessibility caption function.
     * Remained for caption.
     * @param - The properties of the accessibility caption.
     * @return Returns success : true, failed : false.
     */
    virtual bool SetCaptionProperty(const CaptionProperty &caption) = 0;

    /**
     * @brief Checks whether the accessibility caption function is enabled.
     * Remained for caption.
     * @param -
     * @return True if the caption function is enabled; returns False otherwise.
     */
    virtual bool IsAccessibilityCaptionEnabled() const = 0;

    /**
     * @brief Sends an accessibility event.
     * @param eventType  Identifies the accessibility event specified by AccessibilityEventInfo.
     * @param componentId Indicates the ID of the component to be associated with the event.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const EventType eventType, const int32_t componentId) = 0;

    /**
     * @brief Sends information about an accessibility event.
     * @param event Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const AccessibilityEventInfo &event) = 0;

    /**
     * @brief Subscribes to the specified type of accessibility status change events.
     * @param observer Indicates the observer for listening to status events, which is specified
     *              by AccessibilityStateObserver.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     *              #EVENT_SCREEN_MAGNIFIER_CHANGED and EVENT_AUTO_CLICK_CHANGED and #EVENT_SHORT_KEY_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     *              #EVENT_SCREEN_MAGNIFIER_CHANGED and EVENT_AUTO_CLICK_CHANGED and #EVENT_SHORT_KEY_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    virtual bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Get eventlist that accessibility abilities are needed.
     * @return enabled eventlist mask.
     */
    virtual int32_t GetEnabledEventMask() = 0;

    /**
     * @brief Add the listener of caption.
     * @param ob The listener of caption.
     * @param type The type of caption observer.
     * @return Return true if add listener successfully, else return false.
     */
    virtual bool AddCaptionListener(const std::shared_ptr<CaptionObserver> &ob, const int32_t type) = 0;

    /**
     * @brief delete the listener of caption.
     * @param ob The listener of caption.
     * @param type The type of caption observer.
     * @return Return true if delete listener successfully, else return false.
     */
    virtual bool DeleteCaptionListener(const std::shared_ptr<CaptionObserver> &ob, const int32_t type) = 0;

    /**
     * @brief Get the enabled state of accessibility.
     * @return Return true if accessibility is enabled, else return false.
     */
    virtual bool GetEnabledState() = 0;

    /**
     * @brief Get the enabled state of caption.
     * @return Return true if caption is enabled, else return false.
     */
    virtual bool GetCaptionState() = 0;

    /**
     * @brief Get the enabled state of touch guide.
     * @return Return true if touch guide is enabled, else return false.
     */
    virtual bool GetTouchGuideState() = 0;

    /**
     * @brief Get the enabled state of gesture.
     * @return Return true if gesture is enabled, else return false.
     */
    virtual bool GetGestureState() = 0;

    /**
     * @brief Get the enabled state of key event observer.
     * @return Return true if key event observer is enabled, else return false.
     */
    virtual bool GetKeyEventObserverState() = 0;

    /**
     * @brief Enabled specified abilities
     * @param abilities The specified abilities.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableAbilities(std::vector<std::string> &abilities) = 0;

    /**
     * @brief Get installed abilities.
     * @return Return the installed accessibility ability infos.
     */
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() = 0;

    /**
     * @brief Get enabled abilities.
     * @return Return the info of enabled abilities.
     */
    virtual std::vector<std::string> GetEnabledAbilities() = 0;

    /**
     * @brief Set caption property
     * @param caption The caption property to set.
     * @return Return true if set caption property successfully, else return false.
     */
    virtual bool SetCaptionPropertyTojson(const CaptionProperty &caption) = 0;

    /**
     * @brief Set caption state
     * @param state The caption state to set.
     * @return Return true if set caption state successfully, else return false.
     */
    virtual bool SetCaptionStateTojson(const bool state) = 0;

    /**
     * @brief Disabled specified abilities
     * @param abilities The specified abilities.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool DisableAbilities(std::vector<std::string> &abilities) = 0;

    /**
     * @brief Enable Screen Magnifier
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableScreenMagnifier() = 0;
    /**
     * @brief Disable Screen Magnifier
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableScreenMagnifier() = 0;
    /**
     * @brief Get Screen Magnifier feather state
     * @return Return the open state of screen magnifier.
     * false.
     */
    virtual bool GetScreenMagnifierState() = 0;

    /**
     * @brief Enable AutoClick
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableAutoClick() = 0;
    /**
     * @brief Disable AutoClick
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableAutoClick() = 0;
    /**
     * @brief Get AutoClick feather state
     * @return Return the open state of screen magnifier.
     * false.
     */
    virtual bool GetAutoClickState() = 0;

    /**
     * @brief Enable short key
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableShortKey() = 0;
    /**
     * @brief Disable short key
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableShortKey() = 0;
    /**
     * @brief Get short key feather state
     * @return Return the open state of screen magnifier.
     * false.
     */
    virtual bool GetShortKeyState() = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H