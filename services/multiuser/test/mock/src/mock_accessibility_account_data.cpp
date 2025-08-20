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
#include "element_operator_manager.h"
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
    {
        std::lock_guard<ffrt::mutex> lock(abilityManagerMutex_);
        abilityManagerMap_[id_] = std::make_shared<AccessibilityAbilityManager>(id_);
    }
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
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->OnAccountSwitched();
    }

    // Clear Setting State.
    isEventTouchGuideState_ = false;
    isScreenMagnification_ = false;
    isFilteringKeyEvents_ = false;
    isGesturesSimulation_ = false;
}

// add connect ability.
void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddConnectedAbility(connection);
    }
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveConnectedAbility(element);
    }
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
    HILOG_DEBUG("Mock: AddAccessibilityWindowConnection windowId(%{public}d)", windowId);
    operatorManager_.AddAccessibilityWindowConnection(windowId, interactionConnection);
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("Mock: RemoveAccessibilityWindowConnection windowId(%{public}d)", windowId);
    operatorManager_.RemoveAccessibilityWindowConnection(windowId);
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddConnectingA11yAbility(uri, connection);
    }
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveConnectingA11yAbility(uri);
    }
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddEnabledAbility(bundleName);
    }
}

RetError AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->RemoveEnabledAbility(name);
    }
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
    HILOG_DEBUG("Mock: GetAccessibilityWindowConnection windowId(%{public}d)", windowId);
    return operatorManager_.GetAccessibilityWindowConnection(windowId);
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetConnectedA11yAbilities();
    }
    return {};
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("Mock: GetAsacConnections start");
    return operatorManager_.GetAsacConnections();
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start");
    return captionPropertyCallbacks_;
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetConnectingA11yAbility(uri);
    }
    return nullptr;
}

const std::vector<std::string>& AccessibilityAccountData::GetEnabledAbilities()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetEnabledAbilities();
    }
    static std::vector<std::string> emptyVector;
    return emptyVector;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetInstalledAbilities();
    }
    static std::vector<AccessibilityAbilityInfo> emptyVector;
    return emptyVector;
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

RetError AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->EnableAbility(name, capabilities);
    }
    return RET_ERR_INVALID_PARAM;
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

    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->SetConfig(config_);
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

AccountSA::OsAccountType AccessibilityAccountData::GetAccountType()
{
    return AccountSA::OsAccountType::PRIVATE;
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetWaitDisConnectAbility(
    const std::string &elementName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetWaitDisConnectAbility(elementName);
    }
    return nullptr;
}

void AccessibilityAccountData::AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_INFO();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddWaitDisconnectAbility(connection);
    }
}

void AccessibilityAccountData::RemoveWaitDisconnectAbility(const std::string &uri)
{
    HILOG_INFO();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveWaitDisconnectAbility(uri);
    }
}

std::vector<uint32_t> AccessibilityAccountData::UpdateNeedEvents()
{
    return needEvents_;
}

void AccessibilityAccountData::AddNeedEvent(std::string &name, std::vector<uint32_t> needEvents)
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

std::shared_ptr<AccessibilityAbilityManager> AccessibilityAccountData::GetAbilityManager() const
{
    std::lock_guard<ffrt::mutex> lock(abilityManagerMutex_);
    auto it = abilityManagerMap_.find(id_);
    if (it != abilityManagerMap_.end()) {
        return it->second;
    }
    return nullptr;
}

void AccountSubscriber::OnStateChanged(const AccountSA::OsAccountStateData &data)
{
    (void)data;
}

ErrCode AccessibilityAccountData::RegisterElementOperatorByWindowId(const uint32_t tokenId, const int32_t windowId,
    const sptr<IAccessibilityElementOperator>& elementOperator, bool isApp)
{
    (void)tokenId;
    (void)windowId;
    (void)elementOperator;
    (void)isApp;
    return RET_OK;
}

ErrCode AccessibilityAccountData::RegisterElementOperatorByParameter(const RegistrationPara& parameter,
    const sptr<IAccessibilityElementOperator>& elementOperator, const int32_t treeId,
    const int64_t nodeId, const uint32_t tokenId, bool isApp)
{
    (void)parameter;
    (void)elementOperator;
    (void)treeId;
    (void)nodeId;
    (void)tokenId;
    (void)isApp;
    return RET_OK;
}

ErrCode AccessibilityAccountData::DeregisterElementOperatorByWindowId(const int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

ErrCode AccessibilityAccountData::DeregisterElementOperatorByWindowIdAndTreeId(const int32_t windowId,
    const int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return RET_OK;
}

RetError AccessibilityAccountData::VerifyingToKenId(const uint32_t tokenId, const int32_t windowId,
    const int64_t elementId)
{
    (void)tokenId;
    (void)windowId;
    (void)elementId;
    return RET_OK;
}

ElementOperatorManager& AccessibilityAccountData::GetOperatorManager()
{
    static ElementOperatorManager mockOperatorManager;
    return mockOperatorManager;
}

int32_t AccessibilityAccountData::ApplyTreeId()
{
    static int32_t mockTreeId = 1;
    return mockTreeId++;
}

void AccessibilityAccountData::RecycleTreeId(int32_t treeId)
{
    (void)treeId;
}

void AccessibilityAccountData::InsertWindowIdEventPair(int32_t windowId, const AccessibilityEventInfo &event)
{
    (void)windowId;
    (void)event;
}

bool AccessibilityAccountData::CheckWindowIdEventExist(int32_t windowId)
{
    (void)windowId;
    return false;
}

AccessibilityEventInfoParcel AccessibilityAccountData::GetWindowFocusEvent(int32_t windowId)
{
    (void)windowId;
    AccessibilityEventInfo emptyEvent;
    return AccessibilityEventInfoParcel(emptyEvent);
}

void AccessibilityAccountData::EraseWindowFocusEvent(int32_t windowId)
{
    (void)windowId;
}

void AccessibilityAccountData::IsCheckWindowIdEventExist(const int32_t windowId)
{
    (void)windowId;
}
} // namespace Accessibility
} // namespace OHOS