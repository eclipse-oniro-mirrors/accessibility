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
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string PREF_TEST_PATH = "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";
} // namespace

AccessibilityAccountData::AccessibilityAccountData(int32_t accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int32_t AccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("start.");
    return id_;
}

// get client state.
uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    return 0;
}

// switch the user causes state changed.
void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_DEBUG("start.");
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
}

// add connect ability.
void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    std::string uri = Utils::GetUri(connection->GetElementName());
    HILOG_DEBUG("URI is %{public}s", uri.c_str());
    if (!connectedA11yAbilities_.count(uri)) {
        connectedA11yAbilities_.insert(make_pair(uri, connection));
    }

    HILOG_DEBUG("Add ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    HILOG_DEBUG("Remove ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    HILOG_DEBUG("start.");
    stateCallbacks_.push_back(callback);
}

// remove IAccessibleAbilityManagerServiceState
void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject>& callback)
{
    (void)callback;
    HILOG_DEBUG("start.");
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    (void)callback;
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    (void)callback;
    HILOG_DEBUG("start.");
}
// add AccessibilityInteractionConnection
void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    (void)interactionConnection;
    HILOG_DEBUG("windowId(%{public}d)", windowId);
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start.");
    for (auto& ability : connectingA11yAbilities_) {
        if (ability == bundleName) {
            HILOG_ERROR("The ability is already connecting, and it's bundle name is %{public}s", bundleName.c_str());
            return;
        }
    }
    connectingA11yAbilities_.push_back(bundleName);
    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start");
    for (auto it = connectingA11yAbilities_.begin(); it != connectingA11yAbilities_.end(); it++) {
        if (*it == bundleName) {
            HILOG_DEBUG("Removed %{public}s from ConnectingA11yAbility: ", bundleName.c_str());
            connectingA11yAbilities_.erase(it);
            HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not connecting.", bundleName.c_str());
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start.");
    for (auto& ability : enabledAbilities_) {
        if (ability == bundleName) {
            HILOG_ERROR("The ability is already enabled, and it's bundle name is %{public}s", bundleName.c_str());
            return;
        }
    }
    enabledAbilities_.push_back(bundleName);
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledFromPref(const std::string &name)
{
    (void)name;
}

bool AccessibilityAccountData::RemoveEnabledAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == bundleName) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", bundleName.c_str());
            enabledAbilities_.erase(it);
            RemoveEnabledFromPref(bundleName);
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return true;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", bundleName.c_str());
    return false;
}

// For UT
void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    (void)abilityInfo;
}

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    (void)bundleName;
}

void AccessibilityAccountData::ClearInstalledAbility()
{}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    (void)elementName;
    return nullptr;
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    (void)windowId;
    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectedA11yAbilities_;
}

const std::vector<sptr<IAccessibleAbilityManagerStateObserver>> &AccessibilityAccountData::GetStateCallbacks() const
{
    HILOG_DEBUG("start.");
    return stateCallbacks_;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("start.");
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("start.");
    return captionPropertyCallbacks_;
}

const std::vector<std::string>& AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectingA11yAbilities_;
}

const std::vector<std::string>& AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_ size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName = %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("start.");
    return installedAbilities_;
}

void AccessibilityAccountData::GetAbilitiesByState(
    AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities) const
{
    (void)state;
    (void)abilities;
}

void AccessibilityAccountData::UpdateAccountCapabilities()
{}

void AccessibilityAccountData::UpdateEventTouchGuideCapability()
{}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    isScreenMagnification_ = false;
}

void AccessibilityAccountData::UpdateEnabledFromPref()
{
}

bool AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start.");
    for (auto &enabledAbility : enabledAbilities_) {
        if (enabledAbility == name) {
            HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
            return false;
        }
    }
    enabledAbilities_.push_back(name);

    UpdateEnabledFromPref();
    return true;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
    return true;
}

void AccessibilityAccountData::Init()
{
    HILOG_DEBUG("start.");
    if (!config_) {
        config_ = std::make_shared<AccessibilitySettingsConfig>(id_);
    }
}

bool AccessibilityAccountData::DisableAbility(const std::string &name)
{
    HILOG_ERROR("disable ability[%{public}s] start", name.c_str());
    for (auto iter = enabledAbilities_.begin(); iter != enabledAbilities_.end();) {
        if (*iter == name) {
            enabledAbilities_.erase(iter);
            RemoveEnabledFromPref(name);
            HILOG_DEBUG("Removed %{public}s and EnabledAbility size(%{public}zu)",
                name.c_str(), enabledAbilities_.size());
        } else {
            iter++;
        }
    }

    return true;
}

void AccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    (void)observer;
}

void AccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    (void)observer;
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("start.");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> &AccessibilityAccountData::GetConfigCallbacks() const
{
    HILOG_DEBUG("start.");
    return configCallbacks_;
}

void AccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("start.");
    for (auto itr = configCallbacks_.begin(); itr != configCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            configCallbacks_.erase(itr);
            break;
        }
    }
}

std::shared_ptr<AccessibilitySettingsConfig> AccessibilityAccountData::GetConfig()
{
    HILOG_DEBUG("start.");
    return config_;
}

void AccessibilityAccountData::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG();
    (void)importantEnabledAbilities;
}

void AccessibilityAccountData::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG();
    (void)importantEnabledAbilities;
}

uint32_t AccessibilityAccountData::GetInputFilterFlag() const
{
    return 0;
}

void AccessibilityAccountData::UpdateAbilities()
{
}

void AccessibilityAccountData::AddUITestClient(const sptr<IRemoteObject> &obj,
    const std::string &bundleName, const std::string &abilityName)
{
    (void)obj;
    (void)bundleName;
    (void)abilityName;
}

void AccessibilityAccountData::RemoveUITestClient(
    sptr<AccessibleAbilityConnection> &connection, const std::string &bundleName)
{
    (void)connection;
    (void)bundleName;
}
} // namespace Accessibility
} // namespace OHOS