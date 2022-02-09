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
#include "accessibility_event_info.h"
#include "accessibility_interaction_operation_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"

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
    virtual void SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId) = 0;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @param accountId The user id.
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback,
        const int accountId) = 0;

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
    virtual void RegisterInteractionOperation(const int windowId,
            const sptr<IAccessibilityInteractionOperation> &operation,
            const int accountId) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterInteractionOperation(const int windowId) = 0;

    /**
     * @brief Requests feedback interruption from all accessibility services.
     *      Remained.
     * @param -
     * @return
     */
    virtual void Interrupt(const int accountId) = 0; //Remained

    /**
     * @brief Register AA connection to AAMS.
     *      Temp Solution.
     * @param obj IPC object
     * @return
     */
    virtual void RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj) = 0;

    /**
     * @brief Obtains the suggested interval for switching the UI.
     * @param
     * @return Returns the interval.
     */
    virtual uint64_t GetSuggestedInterval() = 0;

    enum class Message {
        SEND_EVENT = 0,
        REGISTER_STATE_CALLBACK,
        GET_ABILITYLIST,
        REGISTER_INTERACTION_CONNECTION,
        DEREGISTER_INTERACTION_CONNECTION,
        INTERRUPT,
        GET_SUGGESTED_INTERVAL,
        REGISTER_ABILITY_CONNECTION_CLIENT  // Temp Solution
    };
};

} //namespace Accessibility
} //namespace OHOS
#endif