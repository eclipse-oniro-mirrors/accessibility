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

#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_element_operator.h"
#include "accessibility_event_info.h"
#include "accessibility_state_event.h"
#include "context.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
enum AccessibilityControlType : int {
    CONTENT_CONTROLS = 0x00000001,
    CONTENT_ICONS = 0x00000002,
    CONTENT_TEXT = 0x00000004,
};

enum AbilityStateType : int {
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
class AccessibilitySystemAbilityClient {
public:
    static const int NUM_INT32 = 32;
    static const uint32_t STATE_ACCESSIBILITY_ENABLED = 0x00000001;
    static const uint32_t STATE_EXPLORATION_ENABLED = 0x00000002;
    static const uint32_t STATE_CAPTION_ENABLED = 0x00000004;
    static const uint32_t STATE_KEYEVENT_ENABLED = 0x00000008;
    static const uint32_t STATE_GESTURE_ENABLED = 0x00000010;

    /**
     * @brief Construct.
     * @param context Indicates the context of the associated ability.
     * @param accountId User Id
     */
    AccessibilitySystemAbilityClient(const AppExecFwk::Context& context, int accountId);

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    int RegisterElementOperator(
        const int windowId, const std::shared_ptr<AccessibilityElementOperator>& operation, int accountId);

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    void DeregisterElementOperator(const int windowId);

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

    bool IsCaptionEnabled();

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
    std::vector<AccessibilityAbilityInfo> GetAbilityList(
        const int accessibilityAbilityTypes, const AbilityStateType stateType);

    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param abilityContext Indicates the context of the associated ability.
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance(const AppExecFwk::Context& abilityContext);

    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param -
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance();

    /**
     * @brief Obtains the properties of the accessibility caption function.
     * Remained for caption.
     * @param -
     * @return Returns the properties of the accessibility caption function.
     */
    CaptionProperty GetCaptionProperty() const;

    bool SetCaptionProperty(const CaptionProperty& caption);

    bool SetCaptionState(const bool state);

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
    bool SendEvent(const AccessibilityEventInfo& event);

    /**
     * @brief Subscribes to the specified type of accessibility status change events.
     * @param observer Indicates the observer for listening to status events, which is specified
     *              by AccessibilityStateObserver.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    bool SubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver>& observer, const int eventType);

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver>& observer, const int eventType);

    /**
     * @brief Unsubscribe the accessibility status change events from the observer.
     * @param observer Indicates the registered accessibility status event observer.
     * @return true is succeed otherwise is failed.
     */
    bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver>& observer);

    /**
     * @brief Inner function for aams status update;
     *        Set whether accessibility ability is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    void UpdateEnabled(const bool enabled);

    /**
     * @brief Inner function for aams status update;
     *        Set whether touch exploration is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    void UpdateTouchExplorationEnabled(const bool enabled);

    /**
     * @brief Update the properties of caption.
     * @param
     * @return
     */
    void UpdatecaptionProperty(const CaptionProperty& property);

    void SetCaptionEnabled(const bool enabled);

    bool SetEnabled(const bool state);

    /**
     * @brief Get eventlist that accessibility abilities are needed.
     * @return enabled eventlist mask.
     */
    int GetEnabledEventMask();

    /**
     * @brief Check otherwise Accessibility Ability(AA) is conflicted with
     * other AA. Remained for setting subsystem.
     * @param abilityName The AA specified (include
     * deviceId/bundleName/abilityName)
     * @return LAUNCH_CONFLICT_NONE: no conflict;
     *          LAUNCH_CONFLICT_TOUCH_BROWSER: conflict with touch browser;
     *          LAUNCH_CONFLICT_DISPLAY_RESIZE: conflict with display resize;
     *          LAUNCH_CONFLICT_KEY_EVENT: conflict with key event;
     */
    int CheckConflictWithEnabledAbility(const AppExecFwk::ElementName& abilityName)
    {
        return 0;
    }

    /**
     * @brief Get the AA specified capability.
     * Remained for setting subsystem.
     * @param abilityName The AA specified (include deviceId/bundleName/abilityName)
     * @return refer to AccessibilityAbilityInfo.Capability
     */
    int GetAccessibleAbilityCapability(const AppExecFwk::ElementName& abilityName)
    {
        return 0;
    }

    /**
     * @brief Inner function.
     *        Get the callback object registered by ACE.
     * @param windowId The window id related the operation object registered.
     * @return The callback object of ACE.
     */
    std::shared_ptr<AccessibilityElementOperator> GetOperatorObject(int windowId);

    bool AddCaptionListener(const std::shared_ptr<CaptionObserver>& ob, const int type);
    bool DeleteCaptionListener(const std::shared_ptr<CaptionObserver>& ob, const int type);

    bool GetEnabledState();
    bool GetCaptionState();
    bool GetTouchGuideState();
    bool GetGestureState();
    bool GetKeyEventObserverState();

    bool SetTouchGuideState(const bool state);
    bool SetGestureState(const bool state);
    bool SetKeyEventObserverState(const bool state);

    bool SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it);
    std::vector<AccessibilityAbilityInfo> GetInstalledAbilities();
    std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities();
    bool SetCaptionPropertyTojson(const CaptionProperty& caption);
    bool SetCaptionStateTojson(const bool state);
    bool DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it);
    int GetActiveWindow();

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
     * @brief Notify the state of caption is changed.
     * @param
     * @return
     */
    void NotifyCaptionStateChanged();

    /**
     * @brief Notify the properties of caption is changed.
     * @param
     * @return
     */
    void NotifyCaptionChanged();

    /**
     * @brief Check the event type is valid or not.
     * @param eventType The data of event type.
     * @return True: The data of event type is valid; otherwise is not.
     */
    bool CheckEventType(EventType eventType);

    /**
     * @brief Check the action type is valid or not.
     * @param eventType The data of event type.
     * @return True: The data of event type is valid; otherwise is not.
     */
    bool CheckActionType(ActionType actionType);

    void NotifyKeyEventStateChanged();

    void NotifyGestureStateChanged();

    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersAccessibilityState_;
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersTouchState_;
    std::vector<std::shared_ptr<CaptionObserver>> observersCaptionProperty_;
    std::vector<std::shared_ptr<CaptionObserver>> observersCaptionEnable_;

    CaptionProperty captionProperty_;
    int accountId_ = 0;
    bool isEnabled_ = 0;
    bool isTouchExplorationEnabled_ = 0;
    bool isCaptionEnabled_ = 0;
    std::recursive_mutex asacProxyLock_;
    static std::shared_ptr<AccessibilitySystemAbilityClient> instance_;
    std::shared_ptr<AccessibilityElementOperator> interactionOperator_ = nullptr;
    std::map<int, std::shared_ptr<AccessibilityElementOperator>> interactionOperators_;
    int connectionWindowId_ = 0;

    std::vector<AccessibilityAbilityInfo> installedAbilities_;
    std::map<std::string, AppExecFwk::ElementName> enabledAbilities_;

    bool isFilteringKeyEventsEnabled_ = 0;
    bool isGesturesSimulationEnabled_ = 0;
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersKeyEventState_;
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observersGestureState_;

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
} // namespace Accessibility
} // namespace OHOS
#endif