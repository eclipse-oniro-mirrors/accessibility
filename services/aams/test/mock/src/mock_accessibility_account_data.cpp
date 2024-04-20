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
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    connectedA11yAbilities_.GetAccessibilityAbilities(connectionList);
    for (auto& connection : connectionList) {
        connection->Disconnect();
    }

    // Clear all abilities.
    connectedA11yAbilities_.Clear();
    connectingA11yAbilities_.Clear();
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
    connectedA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(Utils::GetUri(element));
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
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    auto iter = asacConnections_.find(windowId);
    if (iter != asacConnections_.end()) {
        asacConnections_.erase(iter);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    connectingA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    connectingA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start.");
    if (std::any_of(enabledAbilities_.begin(), enabledAbilities_.end(),
        [bundleName](const std::string &enabledAbility) {
            return enabledAbility == bundleName;
        })) {
        HILOG_ERROR("The ability is already enabled, and it's bundle name is %{public}s", bundleName.c_str());
        return;
    }
    enabledAbilities_.push_back(bundleName);
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

RetError AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return RET_OK;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", name.c_str());
    return RET_ERR_NOT_ENABLED;
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
    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }
    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("GetConnectedA11yAbilities start.");
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    connectedA11yAbilities_.GetAccessibilityAbilitiesMap(connectionMap);
    return connectionMap;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start");
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start");
    return captionPropertyCallbacks_;
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    return connectingA11yAbilities_.GetAccessibilityAbilityByUri(uri);
}

const std::vector<std::string>& AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_'s size is %{public}zu", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("GetInstalledAbilities start");
    return installedAbilities_;
}

void AccessibilityAccountData::GetAbilitiesByState(
    AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities)
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

RetError AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start.");
    if (std::any_of(enabledAbilities_.begin(), enabledAbilities_.end(),
        [name](const std::string &enabledAbility) {
            return enabledAbility == name;
        })) {
        HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
        return RET_ERR_CONNECTION_EXIST;
    }
    enabledAbilities_.push_back(name);
    return RET_OK;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
    return true;
}

void AccessibilityAccountData::SetScreenReaderState(const std::string &name, const std::string &state)
{
    HILOG_DEBUG("start.");
    (void)name;
    (void)state;
}

bool AccessibilityAccountData::GetScreenReaderState()
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

void AccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    HILOG_DEBUG("start.");
    (void)observer;
}

void AccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    (void)observer;
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> &AccessibilityAccountData::GetConfigCallbacks() const
{
    HILOG_DEBUG("GetConfigCallbacks start");
    return configCallbacks_;
}

void AccessibilityAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("SetConfigCallbacks start");
    configCallbacks_ = observer;
}

void AccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("RemoveConfigCallback start");
    for (auto iter = configCallbacks_.begin(); iter != configCallbacks_.end(); iter++) {
        if ((*iter)->AsObject() == callback) {
            configCallbacks_.erase(iter);
            break;
        }
    }
}

std::shared_ptr<AccessibilitySettingsConfig> AccessibilityAccountData::GetConfig()
{
    HILOG_DEBUG("GetConfig start");
    return config_;
}

void AccessibilityAccountData::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG("GetImportantEnabledAbilities start");
    (void)importantEnabledAbilities;
}

void AccessibilityAccountData::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG("UpdateImportantEnabledAbilities start");
    (void)importantEnabledAbilities;
}

uint32_t AccessibilityAccountData::GetInputFilterFlag() const
{
    HILOG_DEBUG("GetInputFilterFlag start");
    return 0;
}

void AccessibilityAccountData::UpdateAbilities()
{
    HILOG_DEBUG("UpdateAbilities start");
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

void AccessibilityAccountData::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    (void)name;
    (void)state;
}

bool AccessibilityAccountData::GetAbilityAutoStartState(const std::string &key)
{
    (void)key;
    return true;
}

void AccessibilityAccountData::GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value)
{
    (void)value;
}

void AccessibilityAccountData::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
{
    (void)bundleName;
}

void AccessibilityAccountData::UpdateAutoStartEnabledAbilities()
{
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilityByUri(
    const std::string& uri)
{
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        return iter->second;
    }

    return nullptr;
}

void AccessibilityAccountData::AccessibilityAbility::Clear()
{
    return connectionMap_.clear();
}

void AccessibilityAccountData::AccessibilityAbility::AddAccessibilityAbility(const std::string& uri,
    const sptr<AccessibleAbilityConnection>& connection)
{
    if (!connectionMap_.count(uri)) {
        connectionMap_[uri] = connection;
        return;
    }
}

void AccessibilityAccountData::AccessibilityAbility::RemoveAccessibilityAbilityByUri(const std::string& uri)
{
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        connectionMap_.erase(iter);
    }
}

void AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilities(
    std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    for (auto& connection : connectionMap_) {
        connectionList.push_back(connection.second);
    }
}

void AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap)
{
    connectionMap = connectionMap_;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::AddAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        HILOG_DEBUG("accountId is existed");
        return iter->second;
    }

    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(accountId);
    if (accountData == nullptr) {
        return nullptr;
    }

    accountData->Init();
    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::GetCurrentAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(accountId);
    if (!accountData) {
        return nullptr;
    }

    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::GetAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    return nullptr;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::RemoveAccountData(
    int32_t accountId)
{
    sptr<AccessibilityAccountData> accountData = nullptr;
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        accountData = iter->second;
        accountDataMap_.erase(iter);
    }

    return accountData;
}

void AccessibilityAccountDataMap::Clear()
{
    accountDataMap_.clear();
}
} // namespace Accessibility
} // namespace OHOS