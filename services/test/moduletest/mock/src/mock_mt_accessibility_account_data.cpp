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

MockAccessibilityAccountData::AccessibilityAccountData(int32_t accountId)
{
    id_ = accountId;
}

MockAccessibilityAccountData::~AccessibilityAccountData()
{}

int32_t MockAccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("start.");
    return id_;
}

// get client state.
uint32_t MockAccessibilityAccountData::GetAccessibilityState()
{
    return 0;
}

// switch the user causes state changed.
void MockAccessibilityAccountData::OnAccountSwitched()
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
void MockAccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    std::string uri = Utils::GetUri(connection->GetElementName());
    HILOG_DEBUG("URI is %{public}s", uri.c_str());
    connectedA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

// remove connect ability.
void MockAccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(Utils::GetUri(element));
}

void MockAccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    (void)callback;
}

void MockAccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    (void)callback;
    HILOG_DEBUG("start.");
}
// add AccessibilityInteractionConnection
void MockAccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

// remove AccessibilityWindowConnection
void MockAccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    auto iter = asacConnections_.find(windowId);
    if (iter != asacConnections_.end()) {
        asacConnections_.erase(iter);
    }
}

void MockAccessibilityAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    connectingA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

void MockAccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    connectingA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

// For UT
void MockAccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
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

RetError MockAccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
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
void MockAccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    (void)abilityInfo;
}

void MockAccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    (void)bundleName;
}

void MockAccessibilityAccountData::ClearInstalledAbility()
{}

const sptr<AccessibleAbilityConnection> MockAccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    (void)elementName;
    return nullptr;
}

const sptr<AccessibilityWindowConnection> MockAccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }
    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> MockAccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("GetConnectedA11yAbilities start.");
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    connectedA11yAbilities_.GetAccessibilityAbilitiesMap(connectionMap);
    return connectionMap;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> MockAccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start");
    return asacConnections_;
}

const CaptionPropertyCallbacks MockAccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start");
    return captionPropertyCallbacks_;
}

sptr<AccessibleAbilityConnection> MockAccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    return connectingA11yAbilities_.GetAccessibilityAbilityByUri(uri);
}

const std::vector<std::string>& MockAccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_'s size is %{public}zu", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

bool AccessibilityAccountData::DealWithScreenReaderState() {
    HILOG_DEBUG("start.");
    return true;
}

const std::vector<AccessibilityAbilityInfo> &MockAccessibilityAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("GetInstalledAbilities start");
    return installedAbilities_;
}

void MockAccessibilityAccountData::GetAbilitiesByState(
    AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities)
{
    (void)state;
    (void)abilities;
}

void MockAccessibilityAccountData::UpdateAccountCapabilities()
{}

void MockAccessibilityAccountData::UpdateEventTouchGuideCapability()
{}

void MockAccessibilityAccountData::UpdateGesturesSimulationCapability()
{}

void MockAccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    isFilteringKeyEvents_ = false;
}

void MockAccessibilityAccountData::UpdateMagnificationCapability()
{
    isScreenMagnification_ = false;
}

void MockAccessibilityAccountData::UpdateEnableAbilityListsState()
{
    return;
}

RetError MockAccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
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

bool MockAccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
    return true;
}

void MockAccessibilityAccountData::SetScreenReaderState(const std::string &name, const std::string &state)
{
    HILOG_DEBUG("start.");
    (void)name;
    (void)state;
}

bool MockAccessibilityAccountData::GetDefaultUserScreenReaderState()
{
    HILOG_DEBUG("start.");
    return true;
}

bool MockAccessibilityAccountData::GetScreenReaderState()
{
    HILOG_DEBUG("start.");
    return true;
}

void MockAccessibilityAccountData::Init()
{
    HILOG_DEBUG("start.");
    if (!config_) {
        config_ = std::make_shared<AccessibilitySettingsConfig>(id_);
    }
}

void MockAccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    HILOG_DEBUG("start.");
    (void)observer;
}

void MockAccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    (void)observer;
}

void MockAccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> MockAccessibilityAccountData::GetConfigCallbacks()
{
    HILOG_DEBUG("GetConfigCallbacks start");
    return configCallbacks_;
}

void MockAccessibilityAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("SetConfigCallbacks start");
    configCallbacks_ = observer;
}

void MockAccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("RemoveConfigCallback start");
    for (auto iter = configCallbacks_.begin(); iter != configCallbacks_.end(); iter++) {
        if ((*iter)->AsObject() == callback) {
            configCallbacks_.erase(iter);
            break;
        }
    }
}

std::shared_ptr<AccessibilitySettingsConfig> MockAccessibilityAccountData::GetConfig()
{
    HILOG_DEBUG("GetConfig start");
    return config_;
}

void MockAccessibilityAccountData::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG("GetImportantEnabledAbilities start");
    (void)importantEnabledAbilities;
}

void MockAccessibilityAccountData::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG("UpdateImportantEnabledAbilities start");
    (void)importantEnabledAbilities;
}

uint32_t MockAccessibilityAccountData::GetInputFilterFlag() const
{
    HILOG_DEBUG("GetInputFilterFlag start");
    return 0;
}

void MockAccessibilityAccountData::UpdateAbilities()
{
    HILOG_DEBUG("UpdateAbilities start");
}

void MockAccessibilityAccountData::AddUITestClient(const sptr<IRemoteObject> &obj,
    const std::string &bundleName, const std::string &abilityName)
{
    (void)obj;
    (void)bundleName;
    (void)abilityName;
}

void MockAccessibilityAccountData::RemoveUITestClient(
    sptr<AccessibleAbilityConnection> &connection, const std::string &bundleName)
{
    (void)connection;
    (void)bundleName;
}

void MockAccessibilityAccountData::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    (void)name;
    (void)state;
}

bool MockAccessibilityAccountData::GetAbilityAutoStartState(const std::string &key)
{
    (void)key;
    return true;
}

void MockAccessibilityAccountData::GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value)
{
    (void)value;
}

void MockAccessibilityAccountData::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
{
    (void)bundleName;
}

void MockAccessibilityAccountData::UpdateAutoStartEnabledAbilities()
{
}

sptr<AccessibleAbilityConnection> MockAccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilityByUri(
    const std::string& uri)
{
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        return iter->second;
    }

    return nullptr;
}

void MockAccessibilityAccountData::AccessibilityAbility::Clear()
{
    return connectionMap_.clear();
}

void MockAccessibilityAccountData::AccessibilityAbility::AddAccessibilityAbility(const std::string& uri,
    const sptr<AccessibleAbilityConnection>& connection)
{
    if (!connectionMap_.count(uri)) {
        connectionMap_[uri] = connection;
        return;
    }
}

void MockAccessibilityAccountData::AccessibilityAbility::RemoveAccessibilityAbilityByUri(const std::string& uri)
{
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        connectionMap_.erase(iter);
    }
}

void MockAccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilities(
    std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    for (auto& connection : connectionMap_) {
        connectionList.push_back(connection.second);
    }
}

void MockAccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap)
{
    connectionMap = connectionMap_;
}

sptr<MockAccessibilityAccountData> MockAccessibilityAccountDataMap::AddAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        HILOG_DEBUG("accountId is existed");
        return iter->second;
    }

    sptr<MockAccessibilityAccountData> accountData = new(std::nothrow) MockAccessibilityAccountData(accountId);
    if (accountData == nullptr) {
        return nullptr;
    }

    accountData->Init();
    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<MockAccessibilityAccountData> MockAccessibilityAccountDataMap::GetCurrentAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    sptr<MockAccessibilityAccountData> accountData = new(std::nothrow) MockAccessibilityAccountData(accountId);
    if (!accountData) {
        return nullptr;
    }

    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<MockAccessibilityAccountData> MockAccessibilityAccountDataMap::GetAccountData(
    int32_t accountId)
{
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    return nullptr;
}

sptr<MockAccessibilityAccountData> MockAccessibilityAccountDataMap::RemoveAccountData(
    int32_t accountId)
{
    sptr<MockAccessibilityAccountData> accountData = nullptr;
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        accountData = iter->second;
        accountDataMap_.erase(iter);
    }

    return accountData;
}

void MockAccessibilityAccountDataMap::Clear()
{
    accountDataMap_.clear();
}

AccountSA::OsAccountType MockAccessibilityAccountData::GetAccountType()
{
    return AccountSA::OsAccountType::PRIVATE;
}
} // namespace Accessibility
} // namespace OHOS