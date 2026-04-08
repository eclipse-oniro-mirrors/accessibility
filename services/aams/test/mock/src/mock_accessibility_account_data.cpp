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
    std::vector<uint32_t> needEvents_ = {};
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
    accessibleAbilityManager_.GetConnectedAbilities(connectionList);
    for (auto& connection : connectionList) {
        connection->Disconnect();
    }

    // Clear all abilities.
    accessibleAbilityManager_.ClearConnectedAbilities();
    accessibleAbilityManager_.Clear();

    // Clear Setting State.
    isEventTouchGuideState_ = false;
    isScreenMagnification_ = false;
    isFilteringKeyEvents_ = false;
    isGesturesSimulation_ = false;
}

// add connect ability.
void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    accessibleAbilityManager_.AddConnectedAbility(connection);
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    accessibleAbilityManager_.RemoveConnectedAbility(element);
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
    elementOperatorManager_.AddAccessibilityWindowConnection(windowId, interactionConnection);
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    elementOperatorManager_.RemoveAccessibilityWindowConnection(windowId);
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    accessibleAbilityManager_.AddConnectingAbility(uri, connection);
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    accessibleAbilityManager_.RemoveConnectingAbility(uri);
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
{
    accessibleAbilityManager_.AddEnabledAbility(bundleName);
}

RetError AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    return accessibleAbilityManager_.RemoveEnabledAbility(name);
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
    return elementOperatorManager_.GetAccessibilityWindowConnection(windowId);
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("GetConnectedA11yAbilities start.");
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    accessibleAbilityManager_.GetConnectedAbilitiesMap(connectionMap);
    return connectionMap;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    return elementOperatorManager_.GetAsacConnections();
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start");
    return captionPropertyCallbacks_;
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    return accessibleAbilityManager_.GetConnectingAbility(uri);
}

const std::vector<std::string>& AccessibilityAccountData::GetEnabledAbilities()
{
    return accessibleAbilityManager_.GetEnabledAbilities();
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    return accessibleAbilityManager_.GetInstalledAbilities();
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

void AccessibilityAccountData::UpdateEnableAbilityListsState()
{
    return;
}

RetError AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities,
    const std::string &callerBundleName)
{
    HILOG_DEBUG("start.");
    const std::vector<std::string>& enabledAbilities = accessibleAbilityManager_.GetEnabledAbilities();
    if (std::any_of(enabledAbilities.begin(), enabledAbilities.end(),
        [name](const std::string &abilityName) {
            return abilityName == name;
        })) {
        HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
        return RET_ERR_CONNECTION_EXIST;
    }
    accessibleAbilityManager_.AddEnabledAbility(name);
    return RET_OK;
}

bool AccessibilityAccountData::DealWithScreenReaderState()
{
    HILOG_DEBUG("start.");
    return true;
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

bool AccessibilityAccountData::GetDefaultUserScreenReaderState()
{
    HILOG_DEBUG("start.");
    return true;
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

void AccessibilityAccountData::AddEnableAbilityCallbackObserver(
    const sptr<IAccessibilityEnableAbilityCallbackObserver>& observer)
{
    (void)observer;
}

void AccessibilityAccountData::RemoveEnableAbilityCallbackObserver(const wptr<IRemoteObject>& observer)
{
    (void)observer;
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> AccessibilityAccountData::GetConfigCallbacks()
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

void AccessibilityAccountData::UpdateAbilities(std::string callerBundleName)
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

std::vector<int32_t> AccessibilityAccountDataMap::GetAllAccountIds()
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    std::vector<int32_t> accountIds;
    for (auto it = accountDataMap_.begin(); it != accountDataMap_.end(); it++) {
        accountIds.push_back(it->first);
    }
    return accountIds;
}

void AccessibilityAccountDataMap::Clear()
{
    accountDataMap_.clear();
}

AccountSA::OsAccountType AccessibilityAccountData::GetAccountType()
{
    return AccountSA::OsAccountType::PRIVATE;
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetWaitDisConnectAbility(
    const std::string &elementName)
{
    return accessibleAbilityManager_.GetWaitDisConnectAbility(elementName);
}

void AccessibilityAccountData::AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_INFO();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    if (connection->GetIsRegisterDisconnectCallback()) {
        HILOG_INFO();
        accessibleAbilityManager_.AddWaitDisconnectAbility(connection);
    }
}

void AccessibilityAccountData::RemoveWaitDisconnectAbility(const std::string &uri)
{
    HILOG_INFO();
    accessibleAbilityManager_.RemoveWaitDisconnectAbility(uri);
}

std::vector<uint32_t> AccessibilityAccountData::UpdateNeedEvents()
{
    return needEvents_;
}

void AccessibilityAccountData::UpdateAbilityNeedEvent(const std::string &name, std::vector<uint32_t> needEvents)
{
    (void)name;
    (void)needEvents;
}

void AccessibilityAccountData::RemoveNeedEvent(const std::string &name)
{
    (void)name;
}

std::vector<uint32_t> AccessibilityAccountData::GetNeedEvents()
{
    return needEvents_;
}

void AccessibilityAccountData::isSendEvent(const AccessibilityEventInfo &eventInfo)
{
    (void)eventInfo;
}

void AccountSubscriber::OnStateChanged(const AccountSA::OsAccountStateData &data)
{
    (void)data;
}

void AccessibilityAccountData::AddAppStateObserverAbility(
    const std::string& uri, const sptr<AccessibleAbilityConnection>& connection)
{
    (void)uri;
    (void)connection;
}
void AccessibilityAccountData::RemoveAppStateObserverAbility(const std::string& uri)
{
    (void)uri;
}
sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAppStateObserverAbility(const std::string& uri)
{
    return nullptr;
}

void AccessibilityAccountData::NotifyExtensionServiceDeath(const std::string& uri)
{
    (void)uri;
}

ElementOperatorManager& AccessibilityAccountData::GetElementOperatorManager()
{
    return elementOperatorManager_;
}
AccessibleAbilityManager& AccessibilityAccountData::GetAccessibleAbilityManager()
{
    return accessibleAbilityManager_;
}
 
AccessibilityWindowManager& AccessibilityAccountData::GetWindowManager()
{
    return windowManager_;
}
 
RetError AccessibilityAccountData::ConfigureEvents(std::vector<uint32_t> needEvents)
{
    return RET_OK;
}
 
RetError AccessibilityAccountData::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &stateObserver, uint32_t &state)
{
    return RET_OK;
}
 
uint32_t AccessibilityAccountData::UpdateAccessibilityState()
{
    return 0;
}
 
void AccessibilityAccountData::RemoveStateObserver(const wptr<IRemoteObject> &remote)
{
    (void)remote;
}
} // namespace Accessibility
} // namespace OHOS