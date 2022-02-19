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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_INTERFACE_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_INTERFACE_H

#include <cstdint>
#include <list>
#include <vector>
#include "accessibility_ability_info.h"
#include "accessible_ability_manager_service_state_interface.h"
#include "accessible_ability_manager_service_caption_property_interface.h"
#include "accessibility_event_info.h"
#include "accessibility_element_operator_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "accessibility_caption.h"
#include "element_name.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class IAccessibleAbilityManagerServiceClient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerServiceClient");

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @param accountId The user id.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SendEvent(const AccessibilityEventInfo& uiEvent, const int accountId) = 0;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @param accountId The user id.
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateCallback(
        const sptr<IAccessibleAbilityManagerServiceState>& callback, const int accountId) = 0;

    virtual uint32_t RegisterCaptionPropertyCallback(
        const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId) = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status. 1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled; 4 indicates that the ability has been installed.
     * @return
     */
    virtual std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType) = 0;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual void RegisterElementOperator(
        const int windowId, const sptr<IAccessibilityElementOperator>& operation, const int accountId) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int windowId) = 0;

    virtual CaptionProperty GetCaptionProperty() = 0;
    virtual bool GetEnabledState() = 0;
    virtual bool GetCaptionState() = 0;
    virtual bool GetTouchGuideState() = 0;
    virtual bool GetGestureState() = 0;
    virtual bool GetKeyEventObserverState() = 0;

    virtual bool SetCaptionProperty(const CaptionProperty& caption) = 0;
    virtual bool SetCaptionState(const bool state) = 0;
    virtual bool SetEnabled(const bool state) = 0;
    virtual bool SetTouchGuideState(const bool state) = 0;
    virtual bool SetGestureState(const bool state) = 0;
    virtual bool SetKeyEventObserverState(const bool state) = 0;

    virtual bool SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it) = 0;
    virtual std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities() = 0;
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() = 0;

    virtual bool DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it) =0;
    virtual int GetActiveWindow() = 0;

    enum class Message {
        SEND_EVENT = 0,
        REGISTER_STATE_CALLBACK,
        GET_ABILITYLIST,
        REGISTER_INTERACTION_CONNECTION,
        DEREGISTER_INTERACTION_CONNECTION,
        GET_CAPTION_PROPERTY,
        SET_CAPTION_PROPERTY,
        SET_CAPTION_STATE,
        SET_ENABLED,
        REGISTER_CAPTION_PROPERTY_CALLBACK,
        GET_ENABLED,
        GET_CAPTION_STATE,
        SET_TOUCH_GUIDE_STATE,
        SET_GESTURE_STATE,
        SET_KEY_EVENT_OBSERVE_STATE,
        GET_TOUCH_GUIDE_STATE,
        GET_GESTURE_STATE,
        GET_KEY_EVENT_OBSERVE_STATE,
        SET_ENABLED_OBJECT,
        GET_ENABLED_OBJECT,
        GET_INSTALLED,
        DISABLE_ABILITIES,
        GET_ACTIVE_WINDOW

    };
};

}  // namespace Accessibility
}  // namespace OHOS
#endif