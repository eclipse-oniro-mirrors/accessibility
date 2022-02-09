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
#ifndef ACCESSIBILITY_ACCOUNT_DATA_H
#define ACCESSIBILITY_ACCOUNT_DATA_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include "accessibility_interaction_connection.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service_state_interface.h"
#include "element_name.h"
#include "refbase.h"
#include "accessibility_state_event.h"
#include "accessibility_system_ability_client.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityConnection;

class IAccessibleAbilityManagerServiceState;

class AccessibilityInteractionConnection;

class AccessibilityAccountData final : public RefBase {
public:
    AccessibilityAccountData(int accountId);
    ~AccessibilityAccountData();

    /**
     * @brief Get the ID of the account.
     * @param
     * @return Returns accountId.
     */
    int GetAccountId();

    /**
     * @brief Get Accessibility state.
     * @param
     * @return returns whether accessibility is connected and whether can touch browse.
     */
    uint32_t GetAccessibilityState();

    /**
     * @brief Switch users and clear old user data.
     * @param
     * @return
     */
    void OnAccountSwitched();

    /**
     * @brief Add connected accessibility services.
     * @param connection Accessible ability connection.
     * @return
     */
    void AddConnectedAbility(sptr<AccessibleAbilityConnection> &connection);

    /**
     * @brief Remove connected accessibility services.
     * @param connection Accessible ability connection.
     * @return
     */
    void RemoveConnectedAbility(sptr<AccessibleAbilityConnection> &connection);

    /**
     * @brief Accessibility services disconnected, accessibility changes from connected state to connecting state.
     * @param connection Accessible ability connection.
     * @return
     */
    void AbilityDisconnected(sptr<AccessibleAbilityConnection> &connection);

    /**
     * @brief Remove connecting accessibility services.
     * @param elementName Accessibility corresponding elementName.
     * @return
     */
    void RemoveConnectingA11yAbility(const AppExecFwk::ElementName &elementName);

    /**
     * @brief Add accessibility monitoring connection.
     * @param callback Accessibility monitoring connection.
     * @return
     */
    void AddStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback);

    /**
     * @brief Remove accessibility monitoring connection.
     * @param callback Accessibility monitoring connection.
     * @return
     */
    void RemoveStateCallback(const wptr<IRemoteObject> &callback);

    /**
     * @brief Add interface operation interactive connection.
     * @param windowId Interface operation interactive connection the corresponding window id.
     * @param interactionConnection Interface interface operation interactive connection.
     * @return
     */
    void AddAccessibilityInteractionConnection(const int windowId, const sptr<AccessibilityInteractionConnection> &interactionConnection);

    /**
     * @brief Remove interface operation interactive connection.
     * @param windowId Interface operation interactive connection the corresponding window id.
     * @return
     */
    void RemoveAccessibilityInteractionConnection(const int windowId);

    /**
     * @brief Add connecting accessibility.
     * @param elementName Accessibility corresponding elementName.
     * @return
     */
    void AddConnectingA11yAbility(const AppExecFwk::ElementName &elementName);

    void AddEnabledAbility(const AppExecFwk::ElementName &elementName);  // For UT

    /**
     * @brief Remove accessibility that have been opened.
     * @param elementName Accessibility corresponding elementName.
     * @return
     */
    void RemoveEnabledAbility(const AppExecFwk::ElementName &elementName);

    void AddInstalledAbility(AccessibilityAbilityInfo &abilityInfo);  // For UT

    /**
     * @brief Empty installed accessibility list.
     * @param abilityInfo Accessibility ability info.
     * @return
     */
    void RemoveInstalledAbility(AccessibilityAbilityInfo &abilityInfo);

    /**
     * @brief The real procedure for add connecting ability.
     * @param
     * @return
     */
    void ClearInstalledAbility();

    /**
     * @brief Get connected accessibility list.
     * @param
     * @return Store map of connected accessibility abilities.
     */
    const std::map<std::string, sptr<AccessibleAbilityConnection>> GetConnectedA11yAbilities();

    /**
     * @brief Get accessibility listening connection list.
     * @param
     * @return Store vector of accessibility listening connection.
     */
    const std::vector<sptr<IAccessibleAbilityManagerServiceState>> GetStateCallbacks();

    /**
     * @brief Get interface operation interactive connection list.
     * @param
     * @return Store map of interface operation interactive connection.
     */
    const std::map<int, sptr<AccessibilityInteractionConnection>> GetAsacConnections();

    /**
     * @brief Query accessible ability connection through elementName URI.
     * @param elementName Accessibility elementName URI
     * @return Accessible ability connection corresponding to elementName URI.
     */
    const sptr<AccessibleAbilityConnection> GetAccessibleAbilityConnection(const std::string elementName);

    /**
     * @brief Query interface operation interactive connection through window id.
     * @param windowId Interface operation interactive connection the corresponding window id.
     * @return Interface operation interactive connection corresponding to window id.
     */
    const sptr<AccessibilityInteractionConnection> GetAccessibilityInteractionConnection(const int windowId);

    /**
     * @brief Get connecting abilities list.
     * @param
     * @return Store map of connecting abilities.
     */
    const std::map<std::string, AppExecFwk::ElementName> GetConnectingA11yAbilities();

    /**
     * @brief Get the accessibility ability info of the corresponding state according to the
     *        ability state type.
     * @param state Ability state type.
     * @return
     */
    const std::vector<AccessibilityAbilityInfo> GetAbilitiesByState(AbilityStateType state);

    /**
     * @brief Get enable accessibility list.
     * @param
     * @return Store map of enable accessibility abilities.
     */
    const std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities();

    /**
     * @brief Get install accessibility list.
     * @param
     * @return Store vector of install accessibility abilities.
     */
    const std::vector<AccessibilityAbilityInfo> GetInstalledAbilities();

    /**
     * @brief Get interactive Ui interval time.
     * @param
     * @return Interactive Ui interval time.
     */
    uint32_t GetInteractiveUiInterval();

    /**
     * @brief Get noninteractive Ui interval time.
     * @param
     * @return NonInteractive Ui interval time.
     */
    uint32_t GetNonInteractiveUiInterval();

    /**
     * @brief Gets the interactive Ui interval time set by the user.
     * @param
     * @return User set the Interactive Ui interval time.
     */
    uint32_t GetAccountInteractiveUiInterval();

    /**
     * @brief Gets the noninteractive Ui interval time set by the user.
     * @param
     * @return User set the noninteractive Ui interval time.
     */
    uint32_t GetAccountNonInteractiveUiInterval();

    /**
     * @brief Update interactive Ui interval time.
     * @param
     * @return
     */
    void UpdateInteractiveUiInterval();

    /**
     * @brief Update user enabled accessibility capabilities.
     * @param
     * @return
     */
    void UpdateAccountCapabilities();

    /**
     * @brief Read accessibility information in settings.
     * @param
     * @return Success return true, failed return false.
     */
    bool ReadConfigurationForAccountData();

    /**
     * @brief Get flag with whether have zoom capability.
     * @param
     * @return isScreenMagnification_.
     */
    bool GetScreenMagnificationFlag() {return isScreenMagnification_;}

    /**
     * @brief Get flag with whether have touch guide capability.
     * @param
     * @return isEventTouchGuideState_.
     */
    bool GetEventTouchGuideStateFlag() {return isEventTouchGuideState_;}

    /**
     * @brief Get flag with whether have key event observer capability.
     * @param
     * @return isFilteringKeyEvents_.
     */
    bool GetFilteringKeyEventsFlag() {return isFilteringKeyEvents_;}

    /**
     * @brief Get flag with whether have gesture capability.
     * @param
     * @return isGesturesSimulation_.
     */
    bool GetGesturesSimulationFlag() {return isGesturesSimulation_;}

private:
    /**
     * @brief Update connected accessibility whether have touch guide capability.
     *        Have touch guide capability isEventTouchGuideState_ is true,
     *        otherwise isEventTouchGuideState_ is false.
     * @param
     * @return
     */
    void UpdateEventTouchGuideCapability();

    /**
     * @brief Update connected accessibility whether have gesture capability.
     *        Have gesture capability isGesturesSimulation_ is true,
     *        otherwise isGesturesSimulation_ is false.
     * @param
     * @return
     */
    void UpdateGesturesSimulationCapability();

    /**
     * @brief Update connected accessibility whether have key event observer capability.
     *        Have key event observer capability isFilteringKeyEvents_ is true,
     *        otherwise isFilteringKeyEvents_ is false.
     * @param
     * @return
     */
    void UpdateFilteringKeyEventsCapability();

    /**
     * @brief Update connected accessibility whether have zoom capability.
     *        Have zoom capability isScreenMagnification_ is true,
     *        otherwise isScreenMagnification_ is false.
     * @param
     * @return
     */
    void UpdateMagnificationCapability();

    int id_;
    uint32_t interactiveUiInterval_ = 0;
    uint32_t nonInteractiveUiInterval_ = 0;
    uint32_t accountInteractiveUiInterval_ = 0;
    uint32_t accountNonInteractiveUiInterval_ = 0;
    bool isEventTouchGuideState_ = false;
    bool isScreenMagnification_ = false;
    bool isFilteringKeyEvents_ = false;
    bool isGesturesSimulation_ = false;
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectedA11yAbilities_ {}; // key: The URI of the ElementName.
    std::vector<sptr<IAccessibleAbilityManagerServiceState>> stateCallbacks_ {};
    std::map<int, sptr<AccessibilityInteractionConnection>> asacConnections_ {}; // key: windowId
    std::vector<AccessibilityAbilityInfo> installedAbilities_ {};
    std::map<std::string, AppExecFwk::ElementName> enabledAbilities_ {}; // key: The URI of the ElementName.
    std::map<std::string, AppExecFwk::ElementName> connectingA11yAbilities_ {}; // key: The URI of the ElementName.
};
}  // namespace Accessibility
}  // namespace OHOS

#endif  // ACCESSIBILITY_ACCOUNT_DATA_H