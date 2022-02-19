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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_PROXY_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_PROXY_H

#include <cstdint>
#include <memory>
#include "accessible_ability_manager_service_interface.h"
#include "accessible_ability_manager_service_state_interface.h"
#include "accessible_ability_manager_service_caption_property_interface.h"
#include "accessibility_event_info.h"
#include "iremote_proxy.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class AccessibleAbilityManagerServiceClientProxy : public IRemoteProxy<IAccessibleAbilityManagerServiceClient> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibleAbilityManagerServiceClientProxy(const sptr<IRemoteObject>& object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceClientProxy() override;

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @param accountId The user id.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SendEvent(const AccessibilityEventInfo& uiEvent, const int accountId) override;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @param accountId The user id.
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateCallback(
        const sptr<IAccessibleAbilityManagerServiceState>& callback, const int accountId) override;

    virtual uint32_t RegisterCaptionPropertyCallback(
        const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId) override;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status. 1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled; 4 indicates that the ability has been installed.
     * @return
     */
    virtual std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType) override;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual void RegisterElementOperator(
        const int windowId, const sptr<IAccessibilityElementOperator>& operation, const int accountId) override;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int windowId) override;

    virtual CaptionProperty GetCaptionProperty() override;
    virtual bool SetCaptionProperty(const CaptionProperty& caption) override;
    virtual bool SetCaptionState(const bool state) override;
    virtual bool SetEnabled(const bool state) override;

    virtual bool GetEnabledState() override;
    virtual bool GetCaptionState() override;
    virtual bool GetTouchGuideState() override;
    virtual bool GetGestureState() override;
    virtual bool GetKeyEventObserverState() override;

    virtual bool SetTouchGuideState(const bool state) override;
    virtual bool SetGestureState(const bool state) override;
    virtual bool SetKeyEventObserverState(const bool state) override;

    virtual bool SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it) override;
    virtual std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities() override;
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() override;

    virtual bool DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it) override;
    virtual int GetActiveWindow() override;

    virtual bool RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj) override;
    virtual bool DeregisterUITestAbilityConnectionClient() override;

    /**
     * @brief Get IPC object.
     * @param
     * @return IPC object
     */
    sptr<IRemoteObject> GetObject();

private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel& data);
    static inline BrokerDelegator<AccessibleAbilityManagerServiceClientProxy> delegator;
};

}  // namespace Accessibility
}  // namespace OHOS
#endif