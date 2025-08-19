/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "accessibility_ability_manager.h"

#include <unordered_set>
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_resource_bundle_manager.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "element_name.h"
#include "ffrt.h"
#include "hilog_wrapper.h"
#include "iaccessibility_enable_ability_lists_observer.h"
#include "iremote_object.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"
#include "utils.h"


#ifdef OHOS_BUILD_ENABLE_HITRACE
#include <hitrace_meter.h>
#endif // OHOS_BUILD_ENABLE_HITRACE

namespace OHOS {
namespace Accessibility {
AccessibilityAbilityManager::AccessibilityAbilityManager(int32_t accountId)
{
    accountId_ = accountId;
    ErrCode rtn = AccountSA::OsAccountManager::GetOsAccountType(accountId_, accountType_);
    if (rtn != ERR_OK) {
        HILOG_ERROR("get account type failed for accountId [%{public}d]", accountId_);
    }
}

AccessibilityAbilityManager::~AccessibilityAbilityManager() {}

void AccessibilityAbilityManager::AddConnectedAbility(sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    std::string uri = Utils::GetUri(connection->GetElementName());

    std::string bundleName = "";
    size_t pos = uri.find('/');
    if (pos != std::string::npos) {
        bundleName = uri.substr(0, pos);
    }
    std::vector<uint32_t> events = {};
    AddNeedEvent(bundleName, events);

    connectedA11yAbilities_.AddAccessibilityAbility(uri, connection);
    HILOG_DEBUG("Added connected ability: %{public}s", uri.c_str());
}

void AccessibilityAbilityManager::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    std::string uri = Utils::GetUri(element);
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
    HILOG_DEBUG("Removed connected ability: %{public}s", uri.c_str());
}

void AccessibilityAbilityManager::RemoveConnectingA11yAbility(const std::string &uri)
{
    HILOG_DEBUG();
    if (uri.empty()) {
        HILOG_ERROR("uri is empty");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(connectingAbilitiesMutex_);
    connectingA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
    HILOG_DEBUG("Removed connecting ability for URI: %{public}s", uri.c_str());
}

const sptr<AccessibleAbilityConnection> AccessibilityAbilityManager::GetWaitDisConnectAbility(
    const std::string &elementName)
{
    HILOG_DEBUG();
    return waitDisconnectA11yAbilities_.GetAccessibilityAbilityByName(elementName);
}

void AccessibilityAbilityManager::AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    if (connection->GetIsRegisterDisconnectCallback()) {
        HILOG_DEBUG();
        std::string uri = Utils::GetUri(connection->GetElementName());
        waitDisconnectA11yAbilities_.AddAccessibilityAbility(uri, connection);
    }
}

void AccessibilityAbilityManager::RemoveWaitDisconnectAbility(const std::string &uri)
{
    HILOG_DEBUG();
    waitDisconnectA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

void AccessibilityAbilityManager::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    if (!observer) {
        HILOG_ERROR("observer is nullptr");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(observersMutex_);
    if (std::any_of(enableAbilityListsObservers_.begin(), enableAbilityListsObservers_.end(),
        [observer](const sptr<IAccessibilityEnableAbilityListsObserver> &listObserver) {
            return listObserver == observer;
        })) {
        HILOG_ERROR("observer is already exist");
        return;
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void AccessibilityAbilityManager::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject> &observer)
{
    HILOG_DEBUG();
    if (!observer.GetRefPtr()) {
        HILOG_ERROR("observer is nullptr");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(observersMutex_);
    for (auto it = enableAbilityListsObservers_.begin(); it != enableAbilityListsObservers_.end();) {
        if ((*it)->AsObject() == observer) {
            it = enableAbilityListsObservers_.erase(it);
            HILOG_DEBUG("Removed observer");
        } else {
            ++it;
        }
    }
}

void AccessibilityAbilityManager::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG();
    if (uri.empty() || !connection) {
        HILOG_ERROR("uri is empty or connection is nullptr");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(connectingAbilitiesMutex_);
    connectingA11yAbilities_.AddAccessibilityAbility(uri, connection);
    HILOG_DEBUG("Added connecting ability for URI: %{public}s", uri.c_str());
}

// Ability-related
void AccessibilityAbilityManager::AddEnabledAbility(const std::string &name)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    if (name.empty()) {
        HILOG_ERROR("name is empty");
        return;
    }

    if (std::find(enabledAbilities_.begin(), enabledAbilities_.end(), name) == enabledAbilities_.end()) {
        enabledAbilities_.push_back(name);
        TryToSetScreenReaderState(name, true);
        UpdateEnableAbilityListsState();
        HILOG_DEBUG("Added enabled ability: %{public}s", name.c_str());
    } else {
        HILOG_WARN("Ability %{public}s already exists in enabled list", name.c_str());
    }
}

RetError AccessibilityAbilityManager::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    if (name.empty()) {
        HILOG_ERROR("name is empty");
        return RET_ERR_INVALID_PARAM;
    }

    auto it = std::remove(enabledAbilities_.begin(), enabledAbilities_.end(), name);
    if (it != enabledAbilities_.end()) {
        enabledAbilities_.erase(it, enabledAbilities_.end());
        TryToSetScreenReaderState(name, false);
        RemoveNeedEvent(name);
        UpdateEnableAbilityListsState();
        HILOG_DEBUG("Removed enabled ability: %{public}s", name.c_str());
        return RET_OK;
    }
    HILOG_WARN("Ability %{public}s not found in enabled list", name.c_str());
    return RET_ERR_NOT_ENABLED;
}

void AccessibilityAbilityManager::AddInstalledAbility(AccessibilityAbilityInfo &abilityInfo)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    if (std::find_if(installedAbilities_.begin(), installedAbilities_.end(), [&abilityInfo](const auto &item) {
            return (item.GetName() == abilityInfo.GetName()) && (item.GetPackageName() == abilityInfo.GetPackageName());
        }) == installedAbilities_.end()) {
        installedAbilities_.emplace_back(abilityInfo);
        UpdateInstallAbilityListsState();
        HILOG_DEBUG("Added installed ability: %{public}s", abilityInfo.GetName().c_str());
    } else {
        HILOG_WARN("Ability %{public}s already exists in installed list", abilityInfo.GetName().c_str());
    }
}

void AccessibilityAbilityManager::RemoveInstalledAbility(const std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    auto it = std::remove_if(installedAbilities_.begin(), installedAbilities_.end(),
        [&bundleName](const auto &ability) { return ability.GetPackageName() == bundleName; });
    if (it != installedAbilities_.end()) {
        installedAbilities_.erase(it, installedAbilities_.end());
        UpdateInstallAbilityListsState();
        HILOG_DEBUG("Removed installed ability for bundle: %{public}s", bundleName.c_str());
    }
}

void AccessibilityAbilityManager::ClearInstalledAbility()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
    installedAbilities_.clear();
    HILOG_DEBUG("Cleared all installed abilities");
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAbilityManager::GetConnectedA11yAbilities()
{
    HILOG_DEBUG();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    std::lock_guard<ffrt::mutex> lock(connectedAbilitiesMutex_);
    connectedA11yAbilities_.GetAccessibilityAbilitiesMap(connectionMap);
    return connectionMap;
}

const sptr<AccessibleAbilityConnection> AccessibilityAbilityManager::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    HILOG_DEBUG();
    if (elementName.empty()) {
        HILOG_ERROR("elementName is empty");
        return nullptr;
    }

    std::lock_guard<ffrt::mutex> lock(connectedAbilitiesMutex_);
    return connectedA11yAbilities_.GetAccessibilityAbilityByName(elementName);
}

sptr<AccessibleAbilityConnection> AccessibilityAbilityManager::GetConnectingA11yAbility(const std::string &uri)
{
    HILOG_DEBUG();
    if (uri.empty()) {
        HILOG_ERROR("uri is empty");
        return nullptr;
    }

    std::lock_guard<ffrt::mutex> lock(connectingAbilitiesMutex_);
    return connectingA11yAbilities_.GetAccessibilityAbilityByUri(uri);
}

size_t AccessibilityAbilityManager::GetConnectingAbilitiesCount() const
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(connectingAbilitiesMutex_);
    return connectingA11yAbilities_.GetSize();
}

void AccessibilityAbilityManager::GetAbilitiesByState(AbilityStateType state,
    std::vector<AccessibilityAbilityInfo> &abilities)
{
    HILOG_DEBUG();
    abilities.clear();
    HILOG_DEBUG("get abilities by state %{public}d", state);

    switch (state) {
        case ABILITY_STATE_ENABLE:
            connectedA11yAbilities_.GetAbilitiesInfo(abilities);
            break;
        case ABILITY_STATE_DISABLE:
            GetDisableAbilities(abilities);
            HILOG_DEBUG("the size of disable abilities is %{public}zu", abilities.size());
            break;
        default:
            std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
            abilities = installedAbilities_;
            break;
    }
}

void AccessibilityAbilityManager::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    HILOG_DEBUG();
    HILOG_DEBUG("get disable abilities");
    std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
    disabledAbilities = installedAbilities_;
    connectedA11yAbilities_.GetDisableAbilities(disabledAbilities);
}

const std::vector<std::string> &AccessibilityAbilityManager::GetEnabledAbilities()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAbilityManager::GetInstalledAbilities() const
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(abilitiesMutex_);
    return installedAbilities_;
}

bool AccessibilityAbilityManager::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryInstalledAbilityInfo");
#endif // OHOS_BUILD_ENABLE_HITRACE
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    Singleton<AccessibilityResourceBundleManager>::GetInstance().QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, accountId_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &info : extensionInfos) {
        AccessibilityAbilityInitParams initParams;
        Utils::Parse(info, initParams);
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(initParams);
        AddInstalledAbility(*accessibilityInfo);
    }
    return true;
}

RetError AccessibilityAbilityManager::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    if (name.empty()) {
        HILOG_ERROR("name is empty");
        return RET_ERR_INVALID_PARAM;
    }

    auto it = std::find_if(installedAbilities_.begin(), installedAbilities_.end(),
        [&name](const auto &ability) { return ability.GetId() == name; });
    if (it == installedAbilities_.end()) {
        HILOG_ERROR("Ability %{public}s not found in installed list", name.c_str());
        return RET_ERR_NOT_INSTALLED;
    }

    uint32_t resultCapabilities = it->GetStaticCapabilityValues() & capabilities;
    if (!resultCapabilities) {
        HILOG_ERROR("the result of capabilities is wrong");
        return RET_ERR_NO_CAPABILITY;
    }
    it->SetCapabilityValues(resultCapabilities);

    if (std::find(enabledAbilities_.begin(), enabledAbilities_.end(), name) != enabledAbilities_.end()) {
        HILOG_WARN("Ability %{public}s already enabled", name.c_str());
        return RET_ERR_CONNECTION_EXIST;
    }

    if (GetWaitDisConnectAbility(name)) {
        HILOG_INFO("The ability[%{public}s] is disconnecting: ", name.c_str());
        sptr<AccessibleAbilityConnection> connection = GetWaitDisConnectAbility(name);
        if (connection != nullptr && connection->GetIsRegisterDisconnectCallback()) {
            connection->DisconnectAbility();
        }
        RemoveWaitDisconnectAbility(name);
    }

#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "EnableAbility:" + name);
#endif // OHOS_BUILD_ENABLE_HITRACE

    enabledAbilities_.push_back(name);
    SetAbilityAutoStartState(name, true);
    TryToSetScreenReaderState(name, true);
    UpdateAbilities();
    Utils::RecordStartingA11yEvent(name);

    HILOG_DEBUG("Enabled ability: %{public}s", name.c_str());
    return RET_OK;
}

void AccessibilityAbilityManager::UpdateEnableAbilityListsState()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(observersMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (const auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityEnableAbilityListsChanged();
        }
    }
}

void AccessibilityAbilityManager::UpdateInstallAbilityListsState()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(observersMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (const auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityInstallAbilityListsChanged();
        }
    }
}

void AccessibilityAbilityManager::UpdateAbilities()
{
    HILOG_DEBUG();
    HILOG_DEBUG("installedAbilities size: %{public}zu.", installedAbilities_.size());
    for (const auto &installAbility : installedAbilities_) {
        const std::string &bundleName = installAbility.GetPackageName();
        const std::string &abilityName = installAbility.GetName();
        std::string uri = Utils::GetUri(bundleName, abilityName);
        HILOG_DEBUG("Checking ability: packageName=%{public}s, abilityName=%{public}s", bundleName.c_str(),
            abilityName.c_str());
        // 1. Skip abilities that are currently connecting
        if (connectingA11yAbilities_.GetSizeByUri(uri)) {
            HILOG_DEBUG("Ability is connecting: %{public}s", uri.c_str());
            continue;
        }
        // 2. Already connected ability
        sptr<AccessibleAbilityConnection> connection = GetAccessibleAbilityConnection(uri);
        // 3. Check if in enabled list
        bool isEnabled = std::find(enabledAbilities_.begin(), enabledAbilities_.end(), uri) != enabledAbilities_.end();
        if (isEnabled) {
            // 3.1 Enabled and already connected, skip
            if (connection) {
                continue;
            }
            // 3.2 Enabled but not connected, try to connect
            AppExecFwk::ElementName element("", bundleName, abilityName);
            AccessibilityAbilityInfo tmpAbility = installAbility;
            connection = new (std::nothrow) AccessibleAbilityConnection(accountId_, connectCounter_++, tmpAbility);
            if (connection && connection->Connect(element)) {
                AddConnectingA11yAbility(uri, connection);
                HILOG_DEBUG("Connected enabled ability: %{public}s", uri.c_str());
            } else {
                HILOG_ERROR("Failed to connect enabled ability: %{public}s", uri.c_str());
            }
        } else {
            // 4. Not enabled
            if (connection) {
                AddWaitDisconnectAbility(connection);
                RemoveConnectedAbility(connection->GetElementName());
                connection->Disconnect();
                HILOG_DEBUG("Disconnected disabled ability: %{public}s", uri.c_str());
            }
        }
    }
}

bool AccessibilityAbilityManager::RemoveAbility(const std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    if (bundleName.empty()) {
        HILOG_ERROR("bundleName is empty");
        return false;
    }

    HILOG_DEBUG("RemoveAbility: bundleName(%{public}s)", bundleName.c_str());
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return false;
    }

    // 1. Remove installed ability
    RemoveInstalledAbility(bundleName);

    // 2. Collect all enabledAbility to be removed (starting with bundleName)
    std::vector<std::string> toRemove;
    for (const auto &enableAbility : enabledAbilities_) {
        if (enableAbility.substr(0, enableAbility.find("/")) == bundleName) {
            toRemove.push_back(enableAbility);
        }
    }

    // 3. Remove enabledAbility and connectingAbility one by one
    bool removed = false;
    for (const auto &ability : toRemove) {
        if (RemoveEnabledAbility(ability) == RET_OK) {
            removed = true;
        }
        RemoveConnectingA11yAbility(ability);
    }

    // 4. Remove connectedAbility
    bool connectedRemoved = false;
    connectedA11yAbilities_.RemoveAccessibilityAbilityByName(bundleName, connectedRemoved);
    removed = removed || connectedRemoved;

    // 5. If removed, update ability state
    if (removed) {
        UpdateAbilities();
    }

    return removed;
}

void AccessibilityAbilityManager::AddAbility(const std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    if (bundleName.empty()) {
        HILOG_ERROR("bundleName is empty");
        return;
    }
    HILOG_DEBUG("AddAbility: bundleName(%{public}s)", bundleName.c_str());
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    bool ret = Singleton<AccessibilityResourceBundleManager>::GetInstance().QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, accountId_, extensionInfos);
    if (!ret) {
        HILOG_ERROR("query extensionAbilityInfos failed");
        return;
    }
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    bool hasNewExtensionAbility = false;
    for (const auto &newAbility : extensionInfos) {
        if (newAbility.bundleName != bundleName) {
            continue;
        }
        HILOG_DEBUG("Processing package: %{public}s/%{public}s", bundleName.c_str(), newAbility.name.c_str());
        AccessibilityAbilityInitParams initParams;
        Utils::Parse(newAbility, initParams);
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(initParams);
        std::string abilityId = accessibilityInfo->GetId();
        if (GetAbilityAutoStartState(abilityId)) {
            HILOG_DEBUG("Auto start packageName: %{public}s", bundleName.c_str());
            uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
            uint32_t resultCapabilities = accessibilityInfo->GetStaticCapabilityValues() & capabilities;
            accessibilityInfo->SetCapabilityValues(resultCapabilities);
            AddInstalledAbility(*accessibilityInfo);
            hasNewExtensionAbility = true;
            std::string uri = Utils::GetUri(bundleName, accessibilityInfo->GetName());
            AddEnabledAbility(uri);
            RemoveConnectingA11yAbility(uri);
        } else {
            AddInstalledAbility(*accessibilityInfo);
            hasNewExtensionAbility = true;
        }
    }
    if (hasNewExtensionAbility) {
        HILOG_DEBUG("Added new extension ability, updating abilities.");
        UpdateAbilities();
    }
}

void AccessibilityAbilityManager::ChangeAbility(const std::string &bundleName)
{
    HILOG_DEBUG();
    if (bundleName.empty()) {
        HILOG_ERROR("bundleName is empty");
        return;
    }
    HILOG_DEBUG("ChangeAbility: bundleName(%{public}s)", bundleName.c_str());
    std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
    if (installedAbilities_.empty()) {
        HILOG_ERROR("There is no installed abilities.");
        return;
    }
    // 1. Collect currently auto-start abilities
    std::vector<std::string> autoStartAbilities;
    for (const auto &ability : installedAbilities_) {
        if (ability.GetPackageName() != bundleName) {
            continue;
        }
        if (GetAbilityAutoStartState(ability.GetId())) {
            autoStartAbilities.push_back(ability.GetId());
            HILOG_DEBUG("Found auto-start ability: %{public}s", ability.GetId().c_str());
        }
    }
    // 2. Remove old abilities and add new abilities
    RemoveInstalledAbility(bundleName);
    AddAbility(bundleName);
    // 3. Check if auto-start abilities still exist
    for (const auto &name : autoStartAbilities) {
        auto iter = std::find_if(installedAbilities_.begin(), installedAbilities_.end(),
            [&name](const auto &ability) { return name == ability.GetId(); });
        if (iter == installedAbilities_.end()) {
            HILOG_DEBUG("Auto-start ability no longer exists: %{public}s", name.c_str());
            SetAbilityAutoStartState(name, false);
        }
    }
}

void AccessibilityAbilityManager::AddUITestClient(const sptr<IRemoteObject> &obj, const std::string &bundleName,
    const std::string &abilityName)
{
    HILOG_DEBUG();
    if (bundleName.empty() || abilityName.empty()) {
        HILOG_ERROR("Invalid parameters: bundleName=%{public}s, abilityName=%{public}s", bundleName.c_str(),
            abilityName.c_str());
        return;
    }

    // Add installed ability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    if (abilityInfo == nullptr) {
        return;
    }
    abilityInfo->SetPackageName(bundleName);
    abilityInfo->SetName(abilityName);
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetAccessibilityAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    AddInstalledAbility(*abilityInfo);

    // Add connected ability
    sptr<AppExecFwk::ElementName> elementName = new (std::nothrow) AppExecFwk::ElementName();
    if (!elementName) {
        HILOG_ERROR("elementName is null");
        return;
    }
    elementName->SetBundleName(bundleName);
    elementName->SetAbilityName(abilityName);
    sptr<AccessibleAbilityConnection> connection =
        new (std::nothrow) AccessibleAbilityConnection(accountId_, connectCounter_++, *abilityInfo);
    if (!connection) {
        HILOG_ERROR("connection is null");
        return;
    }
    connection->OnAbilityConnectDoneSync(*elementName, obj);
    HILOG_DEBUG("Added UI test client: %{public}s/%{public}s", bundleName.c_str(), abilityName.c_str());
}

void AccessibilityAbilityManager::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection,
    const std::string &bundleName)
{
    HILOG_DEBUG();
    if (!connection || bundleName.empty()) {
        HILOG_ERROR("Invalid parameters: connection=%{public}p, bundleName=%{public}s", connection.GetRefPtr(),
            bundleName.c_str());
        return;
    }
    RemoveInstalledAbility(bundleName);
    RemoveConnectedAbility(connection->GetElementName());
    connection->OnAbilityDisconnectDoneSync(connection->GetElementName());
    HILOG_DEBUG("Removed UI test client for bundle: %{public}s", bundleName.c_str());
}

void AccessibilityAbilityManager::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(autoStartStateMutex_);
    RetError rtn = RET_OK;
    TryToSetScreenReaderState(name, state);

    if (!config_) {
        HILOG_WARN("conig_ is nullptr.");
        return;
    }
    if (state) {
        rtn = config_->AddEnabledAccessibilityService(name);
    } else {
        rtn = config_->RemoveEnabledAccessibilityService(name);
    }
    if (rtn != RET_OK) {
        HILOG_WARN("handle service %{public}s state = %{public}d failed, rtn = %{public}d", name.c_str(), state, rtn);
    }
}

bool AccessibilityAbilityManager::GetAbilityAutoStartState(const std::string &name)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(autoStartStateMutex_);
    if (name == SCREEN_READER_ABILITY && accountType_ == AccountSA::OsAccountType::PRIVATE) {
        return GetDefaultUserScreenReaderState();
    }
    if (!config_) {
        HILOG_WARN("config_ is nullptr.");
        return false;
    }
    std::vector<std::string> serviceList = config_->GetEnabledAccessibilityServices();
    auto iter = std::find(serviceList.begin(), serviceList.end(), name);
    if (iter != serviceList.end()) {
        return true;
    } else {
        return false;
    }
}

void AccessibilityAbilityManager::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
{
    HILOG_DEBUG("start and bundleName[%{public}s].", bundleName.c_str());
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    for (auto &installAbility : installedAbilities_) {
        if (installAbility.GetPackageName() == bundleName) {
            std::string abilityName = installAbility.GetName();
            std::string uri = Utils::GetUri(bundleName, abilityName);
            SetAbilityAutoStartState(uri, false);
            break;
        }
    }
}

void AccessibilityAbilityManager::TryToSetScreenReaderState(const std::string &ability, bool state)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(screenReaderMutex_);
    if (ability != SCREEN_READER_ABILITY) {
        return;
    }
    HILOG_DEBUG("set screen reader key [%{public}s], state = [%{public}d].", SCREEN_READER_KEY, state);
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    ErrCode ret = service->PutStringValue(SCREEN_READER_KEY, state ? "1" : "0", true);
    if (ret != ERR_OK) {
        HILOG_ERROR("set failed, ret=%{public}d", ret);
    } else {
        screenReaderState_ = state;
    }
    if (screenReaderState_) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().InitResource(false);
    }
}

void AccessibilityAbilityManager::SetScreenReaderState(const bool state)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(screenReaderMutex_);
    screenReaderState_ = state;
}

bool AccessibilityAbilityManager::GetScreenReaderState()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(screenReaderMutex_);
    HILOG_DEBUG("screen reader sstate is %{public}d", screenReaderState_);
    return screenReaderState_;
}

bool AccessibilityAbilityManager::GetDefaultUserScreenReaderState()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(screenReaderMutex_);
    HILOG_DEBUG();
    std::vector<std::string> services = config_->GetEnabledAccessibilityServices();
    auto iter = std::find(services.begin(), services.end(), SCREEN_READER_ABILITY);
    return iter != services.end();
}

// Event-related
void AccessibilityAbilityManager::AddNeedEvent(const std::string &name, const std::vector<uint32_t> &needEvents)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(eventsMutex_);
    std::string bundleName;
    size_t pos = name.find('/');
    if (pos != std::string::npos) {
        bundleName = name.substr(0, pos);
    } else {
        bundleName = name;
    }

    if (bundleName == SCREEN_READER_BUNDLE_NAME) {
        abilityNeedEvents_[bundleName] = {TYPES_ALL_MASK};
    } else if (bundleName == UI_TEST_BUNDLE_NAME) {
        abilityNeedEvents_[bundleName] = needEvents;
    } else {
        for (auto &installAbility : installedAbilities_) {
            if (installAbility.GetPackageName() == bundleName) {
                installAbility.GetEventConfigure(abilityNeedEvents_[bundleName]);
            }
        }
    }
    HILOG_DEBUG("AddNeedEvent: bundleName=%{public}s, size=%{public}zu", bundleName.c_str(),
        abilityNeedEvents_[bundleName].size());
    UpdateNeedEvents();
}

void AccessibilityAbilityManager::RemoveNeedEvent(const std::string &name)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(eventsMutex_);
    std::string bundleName;
    size_t pos = name.find('/');
    if (pos != std::string::npos) {
        bundleName = name.substr(0, pos);
    } else {
        bundleName = name;
    }

    abilityNeedEvents_.erase(bundleName);
    HILOG_DEBUG("RemoveNeedEvent: bundleName=%{public}s, abilityNeedEvents_ size=%{public}zu", bundleName.c_str(),
        abilityNeedEvents_.size());
    UpdateNeedEvents();
}

std::vector<uint32_t> AccessibilityAbilityManager::UpdateNeedEvents()
{
    HILOG_DEBUG();
    std::unordered_set<uint32_t> uniqueEvents;
    for (const auto &pair : abilityNeedEvents_) {
        uniqueEvents.insert(pair.second.begin(), pair.second.end());
    }

    needEvents_.clear();
    if (uniqueEvents.count(TYPES_ALL_MASK)) {
        needEvents_.push_back(TYPES_ALL_MASK);
    } else if (uniqueEvents.size() == 1 && uniqueEvents.count(TYPE_VIEW_INVALID)) {
        needEvents_.push_back(TYPE_VIEW_INVALID);
    } else {
        needEvents_.assign(uniqueEvents.begin(), uniqueEvents.end());
    }
    HILOG_INFO("UpdateNeedEvents: needEvents_ size=%{public}zu", needEvents_.size());
    return needEvents_;
}

std::vector<uint32_t> AccessibilityAbilityManager::GetNeedEvents() const
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(eventsMutex_);
    return needEvents_;
}

void AccessibilityAbilityManager::OnAccountSwitched()
{
    HILOG_INFO("Account switch cleanup completed for accountId: %{public}d", accountId_);
}

bool AccessibilityAbilityManager::IsExistCapability(Capability capability)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(connectedAbilitiesMutex_);
    return connectedA11yAbilities_.IsExistCapability(capability);
}

void AccessibilityAbilityManager::SetConfig(std::shared_ptr<AccessibilitySettingsConfig> config)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(managerMutex_);
    config_ = config;
}


void AccessibilityAbilityManager::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installed Abilities.");
        return;
    }
    if (enabledAbilities_.empty()) {
        HILOG_DEBUG("Current user has no enabled abilities.");
        return;
    }
    HILOG_DEBUG("installedAbilities_'s is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        if (!installAbility.IsImportant()) {
            HILOG_DEBUG("The ability is not important.");
            continue;
        }
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s", bundleName.c_str(),
            abilityName.c_str());
        std::string uri = Utils::GetUri(bundleName, abilityName);
        std::vector<std::string>::const_iterator iter =
            std::find(enabledAbilities_.begin(), enabledAbilities_.end(), uri);
        if (iter != enabledAbilities_.end()) {
            uint32_t capabilityValues = installAbility.GetCapabilityValues();
            importantEnabledAbilities.emplace(std::make_pair(uri, capabilityValues));
        }
    }
}

void AccessibilityAbilityManager::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG();

    if (importantEnabledAbilities.empty()) {
        HILOG_DEBUG("There is no enabled abilities.");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(installedAbilitiesMutex_);
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installedAbilities.");
        return;
    }
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s", bundleName.c_str(),
            abilityName.c_str());
        std::string uri = Utils::GetUri(bundleName, abilityName);
        std::map<std::string, uint32_t>::iterator iter = importantEnabledAbilities.find(uri);
        if (iter != importantEnabledAbilities.end()) {
            AddEnabledAbility(uri);
            installAbility.SetCapabilityValues(iter->second);
        }
    }
}

void AccessibilityAbilityManager::UpdateAutoStartEnabledAbilities()
{
    HILOG_DEBUG();
    if (accountId_ == -1) {
        HILOG_DEBUG("Current user is -1.");
        return;
    }
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installedAbilities.");
        return;
    }
    if (!config_) {
        HILOG_DEBUG("config_ is null.");
        return;
    }
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        std::string abilityId = bundleName + "/" + abilityName;
        if (GetAbilityAutoStartState(abilityId)) {
            HILOG_INFO("auto start packageName is %{public}s.", bundleName.c_str());
            uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
            uint32_t resultCapabilities = installAbility.GetStaticCapabilityValues() & capabilities;
            installAbility.SetCapabilityValues(resultCapabilities);
            std::string uri = Utils::GetUri(bundleName, abilityName);
            AddEnabledAbility(uri);
        }
    }
}

// AccessibilityAbility nested class implementation
void AccessibilityAbilityManager::AccessibilityAbility::AddAccessibilityAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG();
    if (uri.empty() || !connection) {
        HILOG_ERROR("Invalid parameters: uri=%{public}s, connection=%{public}p", uri.c_str(), connection.GetRefPtr());
        return;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!connectionMap_.count(uri)) {
        connectionMap_[uri] = connection;
        HILOG_DEBUG("connectionMap_ size %{public}zu", connectionMap_.size());
        return;
    }
    HILOG_DEBUG("Added accessibility ability: %{private}s", uri.c_str());
}

sptr<AccessibleAbilityConnection> AccessibilityAbilityManager::AccessibilityAbility::GetAccessibilityAbilityByName(
    const std::string &elementName)
{
    HILOG_DEBUG();
    if (elementName.empty()) {
        HILOG_ERROR("elementName is empty");
        return nullptr;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (const auto &pair : connectionMap_) {
        std::string::size_type index = pair.first.find(elementName);
        if (index == std::string::npos) {
            continue;
        } else {
            HILOG_DEBUG("uri %{private}s ", pair.first.c_str());
            return pair.second;
        }
    }
    return nullptr;
}

sptr<AccessibleAbilityConnection> AccessibilityAbilityManager::AccessibilityAbility::GetAccessibilityAbilityByUri(
    const std::string &uri)
{
    HILOG_DEBUG();
    if (uri.empty()) {
        HILOG_ERROR("uri is empty");
        return nullptr;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto it = connectionMap_.find(uri);
    return (it != connectionMap_.end()) ? it->second : nullptr;
}

void AccessibilityAbilityManager::AccessibilityAbility::GetAccessibilityAbilities(
    std::vector<sptr<AccessibleAbilityConnection>> &connectionList)
{
    HILOG_DEBUG();
    connectionList.clear();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    connectionList.reserve(connectionMap_.size());
    for (const auto &pair : connectionMap_) {
        if (pair.second) {
            connectionList.push_back(pair.second);
        }
    }
}

void AccessibilityAbilityManager::AccessibilityAbility::GetAbilitiesInfo(
    std::vector<AccessibilityAbilityInfo> &abilities)
{
    HILOG_DEBUG();
    abilities.clear();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    abilities.reserve(connectionMap_.size());
    for (const auto &pair : connectionMap_) {
        if (pair.second) {
            abilities.push_back(pair.second->GetAbilityInfo());
        }
    }
}

bool AccessibilityAbilityManager::AccessibilityAbility::IsExistCapability(Capability capability)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (const auto &pair : connectionMap_) {
        if (pair.second) {
            auto info = pair.second->GetAbilityInfo();
            if (info.GetCapabilityValues() & capability) {
                return true;
            }
        }
    }
    return false;
}

void AccessibilityAbilityManager::AccessibilityAbility::GetAccessibilityAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>> &connectionMap)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    connectionMap = connectionMap_;
}

void AccessibilityAbilityManager::AccessibilityAbility::GetDisableAbilities(
    std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    std::unordered_set<std::string> connectedIds;
    for (const auto &connection : connectionMap_) {
        if (connection.second) {
            connectedIds.insert(connection.second->GetAbilityInfo().GetId());
        }
    }
    disabledAbilities.erase(std::remove_if(disabledAbilities.begin(), disabledAbilities.end(),
        [&connectedIds](const AccessibilityAbilityInfo &info) { return connectedIds.count(info.GetId()) > 0; }),
        disabledAbilities.end());
}

int32_t AccessibilityAbilityManager::AccessibilityAbility::GetSizeByUri(const std::string &uri)
{
    HILOG_DEBUG();
    if (uri.empty()) {
        return 0;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.count(uri);
}

void AccessibilityAbilityManager::AccessibilityAbility::RemoveAccessibilityAbilityByName(const std::string &bundleName,
    bool &result)
{
    HILOG_DEBUG();
    if (bundleName.empty()) {
        result = false;
        return;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    result = false;
    auto it = connectionMap_.begin();
    while (it != connectionMap_.end()) {
        if (it->second && it->second->GetElementName().GetBundleName() == bundleName) {
            it = connectionMap_.erase(it);
            result = true;
        } else {
            ++it;
        }
    }
}

void AccessibilityAbilityManager::AccessibilityAbility::RemoveAccessibilityAbilityByUri(const std::string &uri)
{
    HILOG_DEBUG();
    if (uri.empty()) {
        HILOG_ERROR("uri is empty");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (connectionMap_.erase(uri) > 0) {
        HILOG_DEBUG("Removed accessibility ability: %{public}s", uri.c_str());
    }
}

void AccessibilityAbilityManager::AccessibilityAbility::Clear()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    connectionMap_.clear();
    HILOG_DEBUG("Cleared all accessibility abilities");
}

size_t AccessibilityAbilityManager::AccessibilityAbility::GetSize() const
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.size();
}
} // namespace Accessibility
} // namespace OHOS