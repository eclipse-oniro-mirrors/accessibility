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
     */
    explicit AccessibleAbilityManagerServiceProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     */
    virtual ~AccessibleAbilityManagerServiceProxy() override;

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError SendEvent(const AccessibilityEventInfo &uiEvent) override;

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
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by AccessibilityAbilityTypes.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  3 indicates that the ability has been installed.
     * @param infos accessibility ability infos by specified types.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) override;

    /**
     * @brief Register the element operator, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation) override;

    /**
     * @brief Deregister the element operator.
     * @param windowId Window ID
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError DeregisterElementOperator(const int32_t windowId) override;

    virtual RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption) override;
    virtual RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) override;
    virtual RetError SetCaptionState(const bool state) override;

    virtual bool GetEnabledState() override;
    virtual RetError GetCaptionState(bool &state) override;
    virtual bool GetTouchGuideState() override;
    virtual bool GetGestureState() override;
    virtual bool GetKeyEventObserverState() override;

    virtual RetError EnableAbility(const std::string &name, const uint32_t capabilities) override;
    virtual RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) override;
    virtual RetError DisableAbility(const std::string &name) override;
    virtual int32_t GetActiveWindow() override;

    virtual RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) override;
    virtual RetError DisableUITestAbility() override;

    virtual RetError SetScreenMagnificationState(const bool state) override;
    virtual RetError SetShortKeyState(const bool state) override;
    virtual RetError SetMouseKeyState(const bool state) override;
    virtual RetError SetMouseAutoClick(const int32_t time) override;
    virtual RetError SetShortkeyTarget(const std::string &name) override;
    virtual RetError SetHighContrastTextState(const bool state) override;
    virtual RetError SetInvertColorState(const bool state) override;
    virtual RetError SetAnimationOffState(const bool state) override;
    virtual RetError SetAudioMonoState(const bool state) override;
    virtual RetError SetDaltonizationColorFilter(const uint32_t filter) override;
    virtual RetError SetContentTimeout(const uint32_t time) override;
    virtual RetError SetBrightnessDiscount(const float discount) override;
    virtual RetError SetAudioBalance(const float balance) override;

    virtual RetError GetScreenMagnificationState(bool &state) override;
    virtual RetError GetShortKeyState(bool &state) override;
    virtual RetError GetMouseKeyState(bool &state) override;
    virtual RetError GetMouseAutoClick(int32_t &time) override;
    virtual RetError GetShortkeyTarget(std::string &name) override;
    virtual RetError GetHighContrastTextState(bool &state) override;
    virtual RetError GetInvertColorState(bool &state) override;
    virtual RetError GetAnimationOffState(bool &state) override;
    virtual RetError GetAudioMonoState(bool &state) override;
    virtual RetError GetDaltonizationColorFilter(uint32_t &type) override;
    virtual RetError GetContentTimeout(uint32_t &timer) override;
    virtual RetError GetBrightnessDiscount(float &brightness) override;
    virtual RetError GetAudioBalance(float &balance) override;
    virtual void GetAllConfigs(AccessibilityConfigData& configData) override;
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