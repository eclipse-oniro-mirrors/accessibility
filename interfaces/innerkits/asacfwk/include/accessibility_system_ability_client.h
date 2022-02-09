/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "accessibility_event_info.h"
#include "accessibility_state_event.h"
#include "accessibility_interaction_operation.h"
#include "accessibility_ability_info.h"
#include "context.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
#define ACCESSIBILITY_DECLARE_IMPL() \
    struct Impl;                        \
    std::unique_ptr<Impl> pimpl

enum AccessibilityControlType : int {
    CONTENT_CONTROLS = 0x00000001,
    CONTENT_ICONS = 0x00000002,
    CONTENT_TEXT = 0x00000004,
};

enum AbilityStateType: int {
    ABILITY_STATE_INVALID = 0,
    ABILITY_STATE_ENABLE,
    ABILITY_STATE_DISABLE,
    ABILITY_STATE_INSTALLED,
};

/*
* The class register the accessibility service observer to AAMS,and
* dispatch the accessibility service status changed. such as Service Enable，
* Accessibility Enable. It calls AAMS API to send the event to AA.
* It supply sington instance for each process.
*/
class CaptionProperties {
public:
    CaptionProperties() {}
    bool CheckProperty(const std::string &property);
    int GetBackgroundColor() const;   //remained
    int GetForegroundColor() const;   //remained
    int GetEdgeType() const;          //remained
    int GetEdgeColor() const;         //remained
    int GetWindowColor() const;       //remained

private:
    bool HasBackgroundColor();
    bool HasForegroundColor();
    bool HasEdgeType();
    bool HasEdgeColor();
    bool HasWindowColor();

    bool hasBackgroundColor_ = false;
    bool hasEdgeType_ = false;
    bool hasEdgeColor_ = false;
    bool hasWindowColor_ = false;
    bool hasForegroundColor_ = false;
    int backgroundColor_ = 0; //remained
    int edgeType_ = 0;        //remained
    int edgeColor_ = 0;       //remained
    int windowColor_ = 0;     //remained
    int foregroundColor_ =0; //remained
};

class AccessibilitySystemAbilityClient {
public:
    static const int NUM_INT32 = 32;
    static const uint32_t STATE_ACCESSIBILITY_ENABLED = 0x00000001;
    static const uint32_t STATE_EXPLORATION_ENABLED = 0x00000002;
    static const uint32_t STATE_ACCESSIBILITY_DISABLED = 0x00000004;
    static const uint32_t STATE_EXPLORATION_DISABLED = 0x00000008;

    /**
     * @brief Construct.
     * @param context Indicates the context of the associated ability.
     * @param accountId User Id
     */
    AccessibilitySystemAbilityClient(const AppExecFwk::Context &context, int accountId);

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    int RegisterInteractionOperation(const int windowId,
            const std::shared_ptr<AccessibilityInteractionOperation> &operation, int accountId);

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    void DeregisterInteractionOperation(const int windowId);

    /**
     * @brief Checks whether accessibility ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    bool IsEnabled();

    /**
     * @brief Checks whether touch exploration ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    bool IsTouchExplorationEnabled();

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  4 indicates that the ability has been installed.
     * @return
     */
    std::vector<AccessibilityAbilityInfo> GetAbilityList(const int accessibilityAbilityTypes,
        const AbilityStateType stateType);

    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param abilityContext Indicates the context of the associated ability.
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance(const AppExecFwk::Context &abilityContext);

    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param -
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance();

    /**
     * @brief Obtains the suggested interval for switching the UI.
     * Remained.
     * @param timeout Indicates the interval at which the UI is changed.
     * @param contentType Indicates the type of the UI control.
     * @return Returns the interval.
     */
    int GetSuggestedInterval(const int timeout, const int contentType);

    /**
     * @brief Obtains the properties of the accessibility caption function.
     * Remained for caption.
     * @param -
     * @return Returns the properties of the accessibility caption function.
     */
    CaptionProperties GetAccessibilityCaptionProperties() const;

    /**
     * @brief Checks whether the accessibility caption function is enabled.
     * Remained for caption.
     * @param -
     * @return True if the caption function is enabled; returns False otherwise.
     */
    bool IsAccessibilityCaptionEnabled() const;

    /**
     * @brief Sends an accessibility event.
     * @param eventType  Identifies the accessibility event specified by AccessibilityEventInfo.
     * @param componentId Indicates the ID of the component to be associated with the event.
     * @return true: send ok; otherwise is refused.
     */
    bool SendEvent(const EventType eventType, const int componentId);

    /**
     * @brief Sends information about an accessibility event.
     * @param event Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    bool SendEvent(const AccessibilityEventInfo &event);

    /**
     * @brief Subscribes to the specified type of accessibility status change events.
     * @param observer Indicates the observer for listening to status events, which is specified
     *              by AccessibilityStateObserver.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    bool SubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer, const int eventType);

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer, const int eventType);

    /**
     * @brief Unsubscribe the accessibility status change events from the observer.
     * @param observer Indicates the registered accessibility status event observer.
     * @return true is succeed otherwise is failed.
     */
    bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer);

    /**
     * @brief Inner function for aams status update;
     *        Set whether accessibility ability is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    void SetEnabled(const bool enabled);

    /**
     * @brief Inner function for aams status update;
     *        Set whether touch exploration is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    void SetTouchExplorationEnabled(const bool enabled);

    /**
     * @brief Check otherwise Accessibility Ability(AA) is conflicted with other AA.
     *        Remained for setting subsystem.
     * @param abilityName The AA specified (include deviceId/bundleName/abilityName)
     * @return LAUNCH_CONFLICT_NONE: no conflict;
     *          LAUNCH_CONFLICT_TOUCH_BROWSER: conflict with touch browser;
     *          LAUNCH_CONFLICT_DISPLAY_RESIZE: conflict with display resize;
     *          LAUNCH_CONFLICT_KEY_EVENT: conflict with key event;
     *          LAUNCH_CONFLICT_FINGERPRINT_GESTURE: conflict with fingerprint gesture.
     */
    int CheckConflictWithEnabledAbility(const AppExecFwk::ElementName &abilityName) { return 0;}

    /**
     * @brief Get the AA specified capability.
     * Remained for setting subsystem.
     * @param abilityName The AA specified (include deviceId/bundleName/abilityName)
     * @return refer to AccessibilityAbilityInfo.Capability
     */
    int GetAccessibleAbilityCapability(const AppExecFwk::ElementName &abilityName) { return 0;}

    /**
     * @brief Requests feedback interruption from all accessibility services.
     *        Remained.
     * @param -
     * @return
     */
    void Interrupt();

    /**
     * @brief Inner function.
     *        Get the callback object registed by ACE.
     * @param windowId The window id related the operation object registed.
     * @return The callback object of ACE.
     */
    std::shared_ptr<AccessibilityInteractionOperation> GetInteractionObject(int windowId);

private:
    /**
     * @brief Clean the AAMS object data.
     * @param remote The object access to AAMS.
     * @return
     */
    void ResetService(const wptr<IRemoteObject>& remote);

    /**
     * @brief Notify the state of accessibility is changed.
     * @param
     * @return
     */
    void NotifyAccessibilityStateChanged();

    /**
     * @brief Notify the state of touch exploration is changed.
     * @param
     * @return
     */
    void NotifyTouchExplorationStateChanged();

    /**
     * @brief Check the event type is valid or not.
     * @param eventType The data of event type.
     * @return True: The data of event type is valid; otherwise is not.
     */
    bool CheckEventType(EventType eventType);
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersAceessibilityState_{};
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersTouchState_{};
    CaptionProperties captionProperties_ {};
    int accountId_ = 0;
    bool isEnabled_ = 0;
    bool isTouchExplorationEnabled_ = 0;
    std::recursive_mutex asacProxyLock_;
    static std::shared_ptr<AccessibilitySystemAbilityClient> instance_ ;
    std::shared_ptr<AccessibilityInteractionOperation> interactionOperator_;
    std::map<int, std::shared_ptr<AccessibilityInteractionOperation>> interactionOperators_{};
    int connectionWindowId_ = 0;
    ACCESSIBILITY_DECLARE_IMPL();
};

} //namespace Accessibility
} //namespace OHOS
#endif