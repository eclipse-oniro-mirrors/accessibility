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

#include "accessibility_account_data.h"
#include "accessibility_display_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

AccessibilityAccountData::AccessibilityAccountData(int accountId) {
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData() {}

int AccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return id_;
}

// get client state.
uint32_t AccessibilityAccountData::GetAccessibilityState() {
    HILOG_DEBUG("%{public}s start.", __func__);
    uint32_t state = 0;
    if (!connectedA11yAbilities_.empty() || !connectingA11yAbilities_.empty()) {
        state |= AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED;
    } else {
        state |= AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED;
    }

    if (isEventTouchGuideState_) {
        state |= AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED;
    } else {
        state |= AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    }

    return state;
}

// switch the user causes state changed.
void AccessibilityAccountData::OnAccountSwitched() {
    HILOG_DEBUG("%{public}s start.", __func__);
    // reset AccessibleAbilityConnection
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        itr->second->Disconnect();
    }

    // Clear all abilities.
    connectedA11yAbilities_.clear();
    connectingA11yAbilities_.clear();
    enabledAbilities_.clear();
    installedAbilities_.clear();

    // Clear Setting State.
    isEventTouchGuideState_ = false;
    isScreenMagnification_ = false;
    isFilteringKeyEvents_ = false;
    isGesturesSimulation_ = false;

    // Clear UI timeOut.
    interactiveUiInterval_ = 0;
    nonInteractiveUiInterval_ = 0;
    accountInteractiveUiInterval_ = 0;
    accountNonInteractiveUiInterval_ = 0;
}

// add connect ability.
void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection> &connection) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!connectedA11yAbilities_.count(connection->GetElementName().GetURI())) {
        connectedA11yAbilities_.insert(make_pair(connection->GetElementName().GetURI(), connection));
    }

    HILOG_DEBUG("Add ConnectedAbility: %{public}d", connectedA11yAbilities_.size());

    UpdateInteractiveUiInterval();
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection> &connection) {
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it = connectedA11yAbilities_.find(connection->GetElementName().GetURI());
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectedAbility: %{public}d", connectedA11yAbilities_.size());

    UpdateInteractiveUiInterval();
}

// ability disconnect. 
void AccessibilityAccountData::AbilityDisconnected(sptr<AccessibleAbilityConnection> &connection) {
    HILOG_DEBUG("%{public}s start.", __func__);
    RemoveConnectedAbility(connection);
    connectingA11yAbilities_.insert(make_pair(connection->GetElementName().GetURI(), connection->GetElementName()));
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback) {
    HILOG_DEBUG("%{public}s start.", __func__);
    stateCallbacks_.push_back(callback);
}

// remove IAccessibleAbilityManagerServiceState
void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject> &callback) {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = stateCallbacks_.begin(); itr != stateCallbacks_.end(); itr++) {
        if (*itr == callback) {
            stateCallbacks_.erase(itr);
            break;
        }
    }
}

// add AccessibilityInteractionConnection
void AccessibilityAccountData::AddAccessibilityInteractionConnection(const int windowId, const sptr<AccessibilityInteractionConnection> &interactionConnection) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

// remove AccessibilityInteractionConnection
void AccessibilityAccountData::RemoveAccessibilityInteractionConnection(const int windowId) {
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<int, sptr<AccessibilityInteractionConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const AppExecFwk::ElementName &elementName) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!connectingA11yAbilities_.count(elementName.GetURI())) {
        connectingA11yAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const AppExecFwk::ElementName &elementName) {
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<std::string, AppExecFwk::ElementName>::iterator it = connectingA11yAbilities_.find(elementName.GetURI());
    if (it != connectingA11yAbilities_.end()) {
        connectingA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const AppExecFwk::ElementName &elementName) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!enabledAbilities_.count(elementName.GetURI())) {
        enabledAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add EnabledAbility: %{public}d", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledAbility(const AppExecFwk::ElementName &elementName) {
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<std::string, AppExecFwk::ElementName>::iterator it = enabledAbilities_.find(elementName.GetURI());
    if (it != enabledAbilities_.end()) {
        enabledAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove EnabledAbility: %{public}d", enabledAbilities_.size());
}

// For UT
void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo &abilityInfo) {
    HILOG_DEBUG("%{public}s start.", __func__);
    installedAbilities_.push_back(abilityInfo);
}

void AccessibilityAccountData::RemoveInstalledAbility(AccessibilityAbilityInfo &abilityInfo) {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == abilityInfo.GetPackageName()) {
            installedAbilities_.erase(it);
        } else {
            ++it;
        }
    }
}

void AccessibilityAccountData::ClearInstalledAbility() {
    HILOG_DEBUG("%{public}s start.", __func__);
    installedAbilities_.clear();
}

// get AccessibleAbilityConnection
const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(const std::string elementName) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (connectedA11yAbilities_.count(elementName) > 0) {
        return connectedA11yAbilities_[elementName];
    }

    return nullptr;
}

// get AccessibilityInteractionConnection.
const sptr<AccessibilityInteractionConnection> AccessibilityAccountData::GetAccessibilityInteractionConnection(const int windowId) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

    return nullptr;
}

// get map<std::string, sptr<AccessibleAbilityConnection>> connectedA11yAbilities_
const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return connectedA11yAbilities_;
}

const std::vector<sptr<IAccessibleAbilityManagerServiceState>> AccessibilityAccountData::GetStateCallbacks() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return stateCallbacks_;
}

// get map<int, sptr<AccessibilityInteractionConnection>> asacConnections_
const std::map<int, sptr<AccessibilityInteractionConnection>> AccessibilityAccountData::GetAsacConnections() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return asacConnections_;
}

// get connectingA11yAbilities_.
const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetConnectingA11yAbilities() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return connectingA11yAbilities_;
}

// get enabledAbilities_.
const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetEnabledAbilities() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return enabledAbilities_;
}

// get installedAbilities_.
const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetInstalledAbilities() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return installedAbilities_;
}

const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state) {
    HILOG_DEBUG("%{public}s start.", __func__);
    if (state == ABILITY_STATE_ENABLE) {
        std::vector<AccessibilityAbilityInfo> enabledAbilities;
        for (auto ability : connectedA11yAbilities_) {
            enabledAbilities.push_back(ability.second->GetAbilityInfo());
        }
        return enabledAbilities;
    } else if (state == ABILITY_STATE_DISABLE) {
        std::vector<AccessibilityAbilityInfo> disabledAbilities = installedAbilities_;
        for (auto enabledAbility : connectedA11yAbilities_) {
            for (auto itr = disabledAbilities.begin(); itr != disabledAbilities.end();) {
                if (itr->GetId() == enabledAbility.second->GetAbilityInfo().GetId()) {
                    disabledAbilities.erase(itr);
                } else {
                    itr++;
                }
            }
        }
        return disabledAbilities;
    } else {
        return installedAbilities_;
    }
}

// get interactiveUiInterval_.
uint32_t AccessibilityAccountData::GetInteractiveUiInterval() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return interactiveUiInterval_;
}

// get nonInteractiveUiInterval_.
uint32_t AccessibilityAccountData::GetNonInteractiveUiInterval() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return nonInteractiveUiInterval_;
}

// get accountInteractiveUiInterval_.
uint32_t AccessibilityAccountData::GetAccountInteractiveUiInterval() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return accountInteractiveUiInterval_;
}

// get accountNonInteractiveUiInterval_.
uint32_t AccessibilityAccountData::GetAccountNonInteractiveUiInterval() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return accountNonInteractiveUiInterval_;
}

void AccessibilityAccountData::UpdateAccountCapabilities() {
    HILOG_DEBUG("%{public}s start.", __func__);
    UpdateFilteringKeyEventsCapability();
    UpdateEventTouchGuideCapability();
    UpdateGesturesSimulationCapability();
    UpdateMagnificationCapability();
}

void AccessibilityAccountData::UpdateEventTouchGuideCapability() {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_TOUCH_GUIDE) {
            isEventTouchGuideState_ = true;
            return;
        }
    }
    isEventTouchGuideState_ = false;
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability() {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE) {
            isGesturesSimulation_ = true;
            return;
        }
    }
    isGesturesSimulation_ = false;
}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability() {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
            isFilteringKeyEvents_ = true;
            return;
        }
    }
    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability() {
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM) {
            isScreenMagnification_ = true;
            return;
        }
    }
    isScreenMagnification_ = false;
}

void AccessibilityAccountData::UpdateInteractiveUiInterval() {
    HILOG_DEBUG("%{public}s start.", __func__);
    // User setting value.
    interactiveUiInterval_ = accountInteractiveUiInterval_;
    nonInteractiveUiInterval_ = accountNonInteractiveUiInterval_;
    // User does not setting value.
    if (accountInteractiveUiInterval_ == 0 || accountNonInteractiveUiInterval_ == 0) {
        // read from a11y.
        uint32_t abilityInterval = 0;
        uint32_t abilityNonInterval = 0;
        for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
            uint32_t interval = itr->second->GetAbilityInfo().GetUiInteractiveTime();
            uint32_t nonInterval = itr->second->GetAbilityInfo().GetUiNoninteractiveTime();
            if (interval > abilityInterval) {
                abilityInterval = interval;
            }
            if (nonInterval > abilityNonInterval) {
                abilityNonInterval = nonInterval;
            }
        }
        // User only setting nonInterval.
        if (accountInteractiveUiInterval_ == 0) {
            interactiveUiInterval_ = abilityInterval;
        }
        // User only setting interval.
        if (accountNonInteractiveUiInterval_ == 0) {
            nonInteractiveUiInterval_ = abilityNonInterval;
        }
    }
}

bool AccessibilityAccountData::ReadConfigurationForAccountData() {
    HILOG_DEBUG("%{public}s start.", __func__);
    return true;
    // TODO: readConfigurationForUserStateLocked.
}

}  // namespace Accessibility
}  // namespace OHOS