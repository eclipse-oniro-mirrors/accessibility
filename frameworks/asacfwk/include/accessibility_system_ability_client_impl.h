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

#ifndef ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_IMPL_H
#define ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_IMPL_H

#include <array>
#include <mutex>
#include "accessibility_element_operator_impl.h"
#include "accessibility_system_ability_client.h"
#include "accessible_ability_manager_caption_observer_stub.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessible_ability_manager_state_observer_stub.h"

namespace OHOS {
namespace Accessibility {
using StateObserverVector = std::vector<std::shared_ptr<AccessibilityStateObserver>>;
using StateObserversArray = std::array<StateObserverVector, AccessibilityStateEventType::EVENT_TYPE_MAX>;
class AccessibilitySystemAbilityClientImpl : public AccessibilitySystemAbilityClient {
public:
    /**
     * @brief Construct.
     * @param context Indicates the context of the associated ability.
     * @param accountId User Id
     */
    explicit AccessibilitySystemAbilityClientImpl(int32_t accountId);

    /**
     * @brief Desconstruct.
     */
    ~AccessibilitySystemAbilityClientImpl();

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual int32_t RegisterElementOperator(const int32_t windowId,
        const std::shared_ptr<AccessibilityElementOperator> &operation, int32_t accountId) override;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) override;

    /**
     * @brief Checks whether accessibility ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsEnabled() override;

    /**
     * @brief Checks whether touch exploration ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsTouchExplorationEnabled() override;

    /**
     * @brief Checks whether Caption ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsCaptionEnabled() override;

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
        const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType) override;

    /**
     * @brief Obtains the properties of the accessibility caption.
     * @param -
     * @return Returns the properties of the accessibility caption.
     */
    virtual CaptionProperty GetCaptionProperty() const override;

    /**
     * @brief Sets the properties of the accessibility caption.
     * @param caption The properties of the accessibility caption.
     * @return Returns true if sets properties successfully, else return false.
     */
    virtual bool SetCaptionProperty(const CaptionProperty &caption) override;

    /**
     * @brief Checks whether the accessibility caption function is enabled.
     * @param -
     * @return True if the caption function is enabled; returns False otherwise.
     */
    virtual bool IsAccessibilityCaptionEnabled() const override;

    /**
     * @brief Sends an accessibility event.
     * @param eventType  Identifies the accessibility event specified by AccessibilityEventInfo.
     * @param componentId Indicates the ID of the component to be associated with the event.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const EventType eventType, const int32_t componentId) override;

    /**
     * @brief Sends information about an accessibility event.
     * @param event Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const AccessibilityEventInfo &event) override;

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
        const uint32_t eventType) override;

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     *              #EVENT_SCREEN_MAGNIFIER_CHANGED and EVENT_AUTO_CLICK_CHANGED and #EVENT_SHORT_KEY_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    virtual bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) override;

    /**
     * @brief Inner function for aams status update;
     *        Set whether accessibility ability is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    virtual void UpdateEnabled(const bool enabled) override;

    /**
     * @brief Inner function for aams status update;
     *        Set whether touch exploration is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    virtual void UpdateTouchExplorationEnabled(const bool enabled) override;

    /**
     * @brief Inner function for aams status update;
     *        Set whether gesture state is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    virtual void UpdateGestureState(const bool state) override;

    /**
     * @brief Inner function for aams status update;
     *        Set whether key event observer is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    virtual void UpdateKeyEventObserverState(const bool state) override;

    /**
     * @brief Inner function for aams status update;
     *        Set whether caption is enabled.
     * @param enabled true is enabled otherwise is disabled.
     * @return -
     */
    virtual void UpdateCaptionEnabled(const bool enabled) override;

    /**
     * @brief Get eventlist that accessibility abilities are needed.
     * @return enabled eventlist mask.
     */
    virtual int32_t GetEnabledEventMask() override
    {
        return 0;
    }

    /**
     * @brief Add the listener of caption.
     * @param ob The listener of caption.
     * @param type The type of caption observer.
     * @return Return true if add listener successfully, else return false.
     */
    virtual bool AddCaptionListener(const std::shared_ptr<CaptionObserver> &ob, const int32_t type) override;

    /**
     * @brief delete the listener of caption.
     * @param ob The listener of caption.
     * @param type The type of caption observer.
     * @return Return true if delete listener successfully, else return false.
     */
    virtual bool DeleteCaptionListener(const std::shared_ptr<CaptionObserver> &ob, const int32_t type) override;

    /**
     * @brief Get the enabled state of accessibility.
     * @return Return true if accessibility is enabled, else return false.
     */
    virtual bool GetEnabledState() override;

    /**
     * @brief Get the enabled state of caption.
     * @return Return true if caption is enabled, else return false.
     */
    virtual bool GetCaptionState() override;

    /**
     * @brief Get the enabled state of touch guide.
     * @return Return true if touch guide is enabled, else return false.
     */
    virtual bool GetTouchGuideState() override;

    /**
     * @brief Get the enabled state of gesture.
     * @return Return true if gesture is enabled, else return false.
     */
    virtual bool GetGestureState() override;

    /**
     * @brief Get the enabled state of key event observer.
     * @return Return true if key event observer is enabled, else return false.
     */
    virtual bool GetKeyEventObserverState() override;

    /**
     * @brief Enabled specified abilities
     * @param abilities The specified abilities.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableAbilities(std::vector<std::string> &abilities) override;

    /**
     * @brief Get installed abilities.
     * @return Return the installed accessibility ability infos.
     */
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() override;

    /**
     * @brief Get enabled abilities.
     * @return Return the info of enabled abilities.
     */
    virtual std::vector<std::string> GetEnabledAbilities() override;

    /**
     * @brief Set caption property
     * @param caption The caption property to set.
     * @return Return true if set caption property successfully, else return false.
     */
    virtual bool SetCaptionPropertyTojson(const CaptionProperty &caption) override;

    /**
     * @brief Set caption state
     * @param state The caption state to set.
     * @return Return true if set caption state successfully, else return false.
     */
    virtual bool SetCaptionStateTojson(const bool state) override;

    /**
     * @brief Disabled specified abilities
     * @param abilities The specified abilities.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool DisableAbilities(std::vector<std::string> &abilities) override;

    /**
     * @brief Clean the AAMS object data.
     * @param remote The object access to AAMS.
     * @return
     */
    void ResetService(const wptr<IRemoteObject> &remote);

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registered.
     * @param stateType The state type and value.
     *                  state type: Refer to AccessibilityStateEventType.
     *                  value: STATE_ACCESSIBILITY_ENABLED/STATE_EXPLORATION_ENABLED/
     *                         STATE_ACCESSIBILITY_DISABLED/STATE_EXPLORATION_DISABLED
     * @return
     */
    void OnAccessibleAbilityManagerStateChanged(const uint32_t stateType);

    /**
     * @brief Receive the caption property notify from AAMS and send it to the observer registered.
     * @param caption The caption property.
     * @return
     */
    void OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty &property);

    /**
     * @brief Enable Screen Magnifier
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableScreenMagnifier() override
    {
        return false;
    }

    /**
     * @brief Disable Screen Magnifier
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableScreenMagnifier() override
    {
        return false;
    }

    /**
     * @brief Get Screen Magnifier feather state
     * @return Return the open state of screen magnifier.
     * false.
     */
    virtual bool GetScreenMagnifierState() override
    {
        return isScreenMagnifierEnabled_;
    }

    /**
     * @brief Enable AutoClick
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool EnableAutoClick() override
    {
        return false;
    }

    /**
     * @brief Disable Auto Click
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableAutoClick() override
    {
        return false;
    }

    /**
     * @brief Get AutoClick feather state
     * @return Return the open state of AutoClick.
     * false.
     */
    virtual bool GetAutoClickState() override
    {
        return isAutoClickEnabled_;
    }

    /**
     * @brief Enable Short Key
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool EnableShortKey() override
    {
        return false;
    }

    /**
     * @brief Disable Short Key
     * @return Return true if the command is sent successfully, else return
     * false.
     */
    virtual bool DisableShortKey() override
    {
        return false;
    }

    /**
     * @brief Get Short Key feather state
     * @return Return the open state of AutoClick.
     * false.
     */
    virtual bool GetShortKeyState() override
    {
        return isShortKeyEnabled_;
    }

private:
    class AccessibleAbilityManagerStateObserverImpl : public AccessibleAbilityManagerStateObserverStub {
    public:
        explicit AccessibleAbilityManagerStateObserverImpl(AccessibilitySystemAbilityClientImpl &client)
            : client_(client) {}
        ~AccessibleAbilityManagerStateObserverImpl() = default;

        virtual void OnStateChanged(const uint32_t stateType) override
        {
            client_.OnAccessibleAbilityManagerStateChanged(stateType);
        }
    private:
        AccessibilitySystemAbilityClientImpl &client_;
    };

    class AccessibleAbilityManagerCaptionObserverImpl : public AccessibleAbilityManagerCaptionObserverStub {
    public:
        explicit AccessibleAbilityManagerCaptionObserverImpl(AccessibilitySystemAbilityClientImpl &client)
            : client_(client) {}
        ~AccessibleAbilityManagerCaptionObserverImpl() = default;

        virtual void OnPropertyChanged(const CaptionProperty &property) override
        {
            client_.OnAccessibleAbilityManagerCaptionPropertyChanged(property);
        }
    private:
        AccessibilitySystemAbilityClientImpl &client_;
    };

    class DeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit DeathRecipient(AccessibilitySystemAbilityClientImpl &client) : client_(client) {}
        ~DeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(DeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote)
        {
            client_.ResetService(remote);
        }
    private:
        AccessibilitySystemAbilityClientImpl &client_;
    };

    /**
     * @brief Connect to AAMS Service.
     * @param
     * @return success : true, failed : false.
     */
    bool ConnectToService();

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
     * @brief Notify the state of key event is changed.
     * @param
     * @return
     */
    void NotifyKeyEventStateChanged();

    /**
     * @brief Notify the state of gesture is changed.
     * @param
     * @return
     */
    void NotifyGestureStateChanged();

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

    std::mutex mutex_;
    StateObserversArray stateObserversArray_;
    std::vector<std::shared_ptr<CaptionObserver>> observersCaptionProperty_;
    std::vector<std::shared_ptr<CaptionObserver>> observersCaptionEnable_;

    CaptionProperty captionProperty_ {};
    int32_t accountId_ = 0;
    bool isEnabled_ = false;
    bool isTouchExplorationEnabled_ = false;
    bool isFilteringKeyEventsEnabled_ = false;
    bool isGesturesSimulationEnabled_ = false;
    bool isScreenMagnifierEnabled_ = false;
    bool isAutoClickEnabled_ = false;
    bool isShortKeyEnabled_ = false;
    bool isCaptionEnabled_ = false;

    std::recursive_mutex asacProxyLock_;
    std::map<int32_t, sptr<AccessibilityElementOperatorImpl>> interactionOperators_;
    int32_t connectionWindowId_ = 0;

    std::vector<AccessibilityAbilityInfo> installedAbilities_;

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<AccessibleAbilityManagerServiceProxy> serviceProxy_ = nullptr;
    sptr<AccessibleAbilityManagerStateObserverImpl> stateObserver_ = nullptr;
    sptr<AccessibleAbilityManagerCaptionObserverImpl> captionObserver_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_IMPL_H