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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_PROXY_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_PROXY_H

#include "i_accessible_ability_manager_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class AccessibleAbilityManagerServiceProxy : public IRemoteProxy<IAccessibleAbilityManagerService> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibleAbilityManagerServiceProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceProxy() override;

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SendEvent(const AccessibilityEventInfo &uiEvent) override;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) override;

    virtual uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) override;

    virtual void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) override;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status. 1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled; 4 indicates that the ability has been installed.
     * @return
     */
    virtual std::vector<AccessibilityAbilityInfo> GetAbilityList(
        const uint32_t abilityTypes, const int32_t stateType) override;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual void RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation) override;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) override;

    virtual AccessibilityConfig::CaptionProperty GetCaptionProperty() override;
    virtual bool SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) override;
    virtual bool SetCaptionState(const bool state) override;

    virtual bool GetEnabledState() override;
    virtual bool GetCaptionState() override;
    virtual bool GetTouchGuideState() override;
    virtual bool GetGestureState() override;
    virtual bool GetKeyEventObserverState() override;

    virtual bool EnableAbilities(const std::string name, const uint32_t capabilities) override;
    virtual std::vector<std::string> GetEnabledAbilities() override;
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() override;

    virtual bool DisableAbilities(const std::string name) override;
    virtual int32_t GetActiveWindow() override;

    virtual RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) override;
    virtual bool DisableUITestAbility() override;

    virtual bool SetScreenMagnificationState(const bool state) override;
    virtual bool SetShortKeyState(const bool state) override;
    virtual bool SetMouseKeyState(const bool state) override;
    virtual bool SetMouseAutoClick(const int32_t time) override;
    virtual bool SetShortkeyTarget(const std::string &name) override;
    virtual bool SetHighContrastTextState(const bool state) override;
    virtual bool SetInvertColorState(const bool state) override;
    virtual bool SetAnimationOffState(const bool state) override;
    virtual bool SetAudioMonoState(const bool state) override;
    virtual bool SetDaltonizationColorFilter(const uint32_t filter) override;
    virtual bool SetContentTimeout(const uint32_t time) override;
    virtual bool SetBrightnessDiscount(const float discount) override;
    virtual bool SetAudioBalance(const float balance) override;

    virtual bool GetScreenMagnificationState() override;
    virtual bool GetShortKeyState() override;
    virtual bool GetMouseKeyState() override;
    virtual int32_t GetMouseAutoClick() override;
    virtual std::string GetShortkeyTarget() override;
    virtual bool GetHighContrastTextState() override;
    virtual bool GetInvertColorState() override;
    virtual bool GetAnimationOffState() override;
    virtual bool GetAudioMonoState() override;
    virtual uint32_t GetDaltonizationColorFilter() override;
    virtual uint32_t GetContentTimeout() override;
    virtual float GetBrightnessDiscount() override;
    virtual float GetAudioBalance() override;
    virtual uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) override;
private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel& data);

    /**
     * @brief Send the command data from proxy to stub in IPC mechanism.
     * @param code The code matched the function called.
     * @param data Serializable data
     * @param reply The response of IPC
     * @param option The option parameter of IPC,such as: async,sync
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool SendTransactCmd(IAccessibleAbilityManagerService::Message code, MessageParcel &data,
        MessageParcel &reply,  MessageOption &option);
    static inline BrokerDelegator<AccessibleAbilityManagerServiceProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_MANAGER_SERVICE_PROXY_H