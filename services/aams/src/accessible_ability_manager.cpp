/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#include "accessible_ability_manager.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManager::~AccessibleAbilityManager()
{
    connectedA11yAbilities_.Clear();
    connectingA11yAbilities_.Clear();
    waitDisconnectA11yAbilities_.Clear();
    appStateObserverAbilities_.Clear();
}

void AccessibleAbilityManager::Clear()
{
    connectedA11yAbilities_.Clear();
    enabledAbilities_.clear();
}

void AccessibleAbilityManager::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    std::string uri = Utils::GetUri(connection->GetElementName());
    connectedA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

void AccessibleAbilityManager::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(Utils::GetUri(element));
}

void AccessibleAbilityManager::RemoveConnectedAbilityByUri(const std::string &uri)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

void AccessibleAbilityManager::RemoveConnectedAbilityByName(const std::string &bundleName, bool& result)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByName(bundleName, result);
}

sptr<AccessibleAbilityConnection> AccessibleAbilityManager::GetConnectedAbilityByName(const std::string &elementName)
{
    return connectedA11yAbilities_.GetAccessibilityAbilityByName(elementName);
}

void AccessibleAbilityManager::GetConnectedAbilities(std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    connectedA11yAbilities_.GetAccessibilityAbilities(connectionList);
}

void AccessibleAbilityManager::GetConnectedAbilitiesInfo(std::vector<AccessibilityAbilityInfo>& abilities)
{
    connectedA11yAbilities_.GetAbilitiesInfo(abilities);
}

void AccessibleAbilityManager::GetConnectedAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>> &connectionMap)
{
    connectedA11yAbilities_.GetAccessibilityAbilitiesMap(connectionMap);
}

void AccessibleAbilityManager::ClearConnectedAbilities()
{
    connectedA11yAbilities_.Clear();
}

size_t AccessibleAbilityManager::GetConnectedAbilitiesSize()
{
    return connectedA11yAbilities_.GetSize();
}

void AccessibleAbilityManager::AddConnectingAbility(
    const std::string &uri, const sptr<AccessibleAbilityConnection> &connection)
{
    connectingA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

void AccessibleAbilityManager::RemoveConnectingAbility(const std::string &uri)
{
    connectingA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

sptr<AccessibleAbilityConnection> AccessibleAbilityManager::GetConnectingAbility(const std::string &uri)
{
    return connectingA11yAbilities_.GetAccessibilityAbilityByUri(uri);
}

size_t AccessibleAbilityManager::GetConnectingAbilitiesSize()
{
    return connectingA11yAbilities_.GetSize();
}

void AccessibleAbilityManager::AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_INFO();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    if (connection->GetIsRegisterDisconnectCallback()) {
        HILOG_INFO();
        std::string uri = Utils::GetUri(connection->GetElementName());
        waitDisconnectA11yAbilities_.AddAccessibilityAbility(uri, connection);
    }
}

void AccessibleAbilityManager::RemoveWaitDisconnectAbility(const std::string &uri)
{
    HILOG_INFO();
    waitDisconnectA11yAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

const sptr<AccessibleAbilityConnection> AccessibleAbilityManager::GetWaitDisConnectAbility(
    const std::string &elementName)
{
    return waitDisconnectA11yAbilities_.GetAccessibilityAbilityByName(elementName);
}

size_t AccessibleAbilityManager::GetWaitDisconnectAbilitiesSize()
{
    return waitDisconnectA11yAbilities_.GetSize();
}

void AccessibleAbilityManager::AddAppStateObserverAbility(
    const std::string &uri, const sptr<AccessibleAbilityConnection> &connection)
{
    if (uri.empty() || !connection) {
        HILOG_ERROR("Invalid parameters for AddAppStateObserverAbility");
        return;
    }
    appStateObserverAbilities_.AddAccessibilityAbility(uri, connection);
}

void AccessibleAbilityManager::RemoveAppStateObserverAbility(const std::string& uri)
{
    if (uri.empty()) {
        HILOG_ERROR("Invalid uri for RemoveAppStateObserverAbility");
        return;
    }
    appStateObserverAbilities_.RemoveAccessibilityAbilityByUri(uri);
}

sptr<AccessibleAbilityConnection> AccessibleAbilityManager::GetAppStateObserverAbility(const std::string& uri)
{
    return appStateObserverAbilities_.GetAccessibilityAbilityByUri(uri);
}

void AccessibleAbilityManager::AddEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("AddEnabledAbility start.");
    if (std::any_of(enabledAbilities_.begin(), enabledAbilities_.end(),
        [name](const std::string &abilityName) {
            return abilityName == name;
        })) {
        HILOG_DEBUG("The ability is already enabled, and it's name is %{public}s", name.c_str());
        return;
    }
    enabledAbilities_.push_back(name);
    UpdateEnableAbilityListsState();
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

RetError AccessibleAbilityManager::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("RemoveEnabledAbility start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            UpdateEnableAbilityListsState();
            HILOG_DEBUG("EnabledAbility size %{public}zu", enabledAbilities_.size());
            return RET_OK;
        }
    }
    HILOG_ERROR("The ability %{public}s is not enabled.", name.c_str());
    return RET_ERR_NOT_ENABLED;
}

const std::vector<std::string> &AccessibleAbilityManager::GetEnabledAbilities() const
{
    HILOG_DEBUG("enabledAbilities_'s size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName is %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

void AccessibleAbilityManager::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    HILOG_DEBUG("abilityInfo's bundle name is %{public}s", abilityInfo.GetPackageName().c_str());
    for (size_t i = 0; i < installedAbilities_.size(); i++) {
        if ((installedAbilities_[i].GetPackageName() == abilityInfo.GetPackageName()) &&
            installedAbilities_[i].GetName() == abilityInfo.GetName()) {
            HILOG_DEBUG("the ability is already exist.");
            return;
        }
    }
    installedAbilities_.push_back(abilityInfo);
    UpdateInstallAbilityListsState();
    HILOG_DEBUG("push back installed ability successfully and installedAbilities_'s size is %{public}zu",
        installedAbilities_.size());
}

void AccessibleAbilityManager::RemoveInstalledAbility(const std::string &bundleName)
{
    HILOG_DEBUG("start.");
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == bundleName) {
            HILOG_DEBUG("Removed %{public}s from InstalledAbility: ", bundleName.c_str());
            it = installedAbilities_.erase(it);
        } else {
            ++it;
        }
    }
    UpdateInstallAbilityListsState();
}

void AccessibleAbilityManager::UpdateInstallAbilityListsState()
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityInstallAbilityListsChanged();
        }
    }
}

void AccessibleAbilityManager::ClearInstalledAbility()
{
    HILOG_DEBUG("start.");
    installedAbilities_.clear();
}

const std::vector<AccessibilityAbilityInfo> &AccessibleAbilityManager::GetInstalledAbilities() const
{
    HILOG_DEBUG("GetInstalledAbilities start.");
    return installedAbilities_;
}

RetError AccessibleAbilityManager::UpdateInstalledAbility(const std::string &name, uint32_t capabilities)
{
    for (auto iter = installedAbilities_.begin(); iter != installedAbilities_.end(); iter++) {
        if (iter->GetId() == name) {
            // Judge capabilities
            uint32_t resultCapabilities = iter->GetStaticCapabilityValues() & capabilities;
            if (resultCapabilities == 0) {
                HILOG_ERROR("the result of capabilities is wrong");
                return RET_ERR_NOT_ENABLED;
            }
            iter->SetCapabilityValues(resultCapabilities);
            return RET_OK;
        }
    }
    return RET_ERR_NOT_INSTALLED;
}

void AccessibleAbilityManager::GetAbilitiesByState(
    AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities)
{
    HILOG_DEBUG("get abilities by state %{public}d", state);
    if (state == ABILITY_STATE_ENABLE) {
        connectedA11yAbilities_.GetAbilitiesInfo(abilities);
    } else if (state == ABILITY_STATE_DISABLE) {
        GetDisableAbilities(abilities);
        HILOG_DEBUG("the size of disable abilities is %{public}zu", abilities.size());
    } else {
        abilities = installedAbilities_;
    }
}

void AccessibleAbilityManager::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    HILOG_DEBUG("get disable abilities");
    disabledAbilities = installedAbilities_;
    connectedA11yAbilities_.GetDisableAbilities(disabledAbilities);
}

void AccessibleAbilityManager::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
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

void AccessibleAbilityManager::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    for (auto itr = enableAbilityListsObservers_.begin(); itr != enableAbilityListsObservers_.end(); itr++) {
        if ((*itr)->AsObject() == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(itr);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return;
        }
    }
}

void AccessibleAbilityManager::UpdateEnableAbilityListsState()
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityEnableAbilityListsChanged();
        }
    }
}

void AccessibleAbilityManager::AddEnableAbilityCallbackObserver(
    const sptr<IAccessibilityEnableAbilityCallbackObserver> &observer)
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityCallbackObserversMutex_);
    if (std::any_of(enableAbilityCallbackObservers_.begin(), enableAbilityCallbackObservers_.end(),
        [observer](const sptr<IAccessibilityEnableAbilityCallbackObserver> &listObserver) {
            return listObserver == observer;
        })) {
        HILOG_ERROR("observer is already exist");
        return;
    }
    enableAbilityCallbackObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityCallbackObservers_.size());
}

void AccessibleAbilityManager::RemoveEnableAbilityCallbackObserver(const wptr<IRemoteObject>& observer)
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityCallbackObserversMutex_);
    auto itr = enableAbilityCallbackObservers_.begin();
    for (; itr != enableAbilityCallbackObservers_.end(); itr++) {
        if ((*itr)->AsObject() == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityCallbackObservers_.erase(itr);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityCallbackObservers_.size());
            return;
        }
    }
}

void AccessibleAbilityManager::CallEnableAbilityCallback(const std::string &uri)
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityCallbackObserversMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityCallbackObservers_.size());
    for (auto &observer : enableAbilityCallbackObservers_) {
        if (observer) {
            observer->OnEnableAbilityRemoteDied(uri);
        }
    }
}

void AccessibleAbilityManager::NotifyExtensionServiceDeath(const std::string& uri)
{
    HILOG_DEBUG("Notifying extension service death: uri=%{public}s", uri.c_str());
    CallEnableAbilityCallback(uri);
}


bool AccessibleAbilityManager::RemoveAbility(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return false;
    }
    RemoveInstalledAbility(bundleName);

    bool result = false;
    for (auto& enableAbility : enabledAbilities_) {
        if (enableAbility.substr(0, enableAbility.find("/")) == bundleName) {
            RemoveEnabledAbility(enableAbility);
            RemoveConnectingAbility(enableAbility);
            result = true;
        }
    }

    RemoveConnectedAbilityByName(bundleName, result);
    return result;
}

void AccessibleAbilityManager::AddAbility(const std::string &bundleName,
    const std::vector<AccessibilityAbilityInfo>& abilityInfos,
    std::function<bool(const std::string&)> autoStartChecker)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    for (auto &newAbility : abilityInfos) {
        if (newAbility.GetPackageName() == bundleName) {
            HILOG_DEBUG("The package%{public}s added", (bundleName + "/" + newAbility.GetName()).c_str());
            AccessibilityAbilityInfo info = newAbility;
            std::string abilityId = info.GetId();
            if (autoStartChecker && autoStartChecker(abilityId)) {
                HILOG_DEBUG("auto start packageName is %{public}s.", bundleName.c_str());
                uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                    CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
                uint32_t resultCapabilities = info.GetStaticCapabilityValues() & capabilities;
                info.SetCapabilityValues(resultCapabilities);
                AddInstalledAbility(info);
                std::string uri = Utils::GetUri(bundleName, info.GetName());
                AddEnabledAbility(uri);
                RemoveConnectingAbility(uri);
                continue;
            }
            AddInstalledAbility(info);
        }
    }
}

void AccessibleAbilityManager::ChangeAbility(const std::string &bundleName,
    std::function<bool(const std::string&)> autoStartChecker,
    std::function<void(const std::string&, bool)> autoStartSetter)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    
    std::vector<std::string> autoStartAbilities;
    for (auto &ability : installedAbilities_) {
        if (ability.GetPackageName() != bundleName) {
            continue;
        }
        if (autoStartChecker && autoStartChecker(ability.GetId())) {
            autoStartAbilities.push_back(ability.GetId());
        }
    }
    
    RemoveInstalledAbility(bundleName);
    
    for (auto &name : autoStartAbilities) {
        auto iter = std::find_if(installedAbilities_.begin(), installedAbilities_.end(),
            [&name](const AccessibilityAbilityInfo& info) {
                return info.GetId() == name;
            });
        if (iter == installedAbilities_.end()) {
            if (autoStartSetter) {
                autoStartSetter(name, false);
            }
        }
        }
}

void AccessibleAbilityManager::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG("GetImportantEnabledAbilities start.");
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
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(), abilityName.c_str());
        std::string uri = Utils::GetUri(bundleName, abilityName);
        std::vector<std::string>::const_iterator iter = std::find(enabledAbilities_.begin(),
            enabledAbilities_.end(), uri);
        if (iter != enabledAbilities_.end()) {
            uint32_t capabilityValues = installAbility.GetCapabilityValues();
            importantEnabledAbilities.emplace(std::make_pair(uri, capabilityValues));
        }
    }
}

void AccessibleAbilityManager::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG();
    if (importantEnabledAbilities.empty()) {
        HILOG_DEBUG("There is no enabled abilities.");
        return;
    }
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installedAbilities.");
        return;
    }
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(), abilityName.c_str());
        std::string uri = Utils::GetUri(bundleName, abilityName);
        std::map<std::string, uint32_t>::iterator iter = importantEnabledAbilities.find(uri);
        if (iter != importantEnabledAbilities.end()) {
            AddEnabledAbility(uri);
            installAbility.SetCapabilityValues(iter->second);
        }
    }
}

void AccessibleAbilityManager::UpdateAutoStartEnabledAbilities(std::function<bool(const std::string&)> autoStartChecker)
{
    HILOG_DEBUG();
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installedAbilities.");
        return;
    }
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        std::string abilityId = bundleName + "/" + abilityName;
        if (autoStartChecker && autoStartChecker(abilityId)) {
            HILOG_DEBUG("auto start packageName is %{public}s.", bundleName.c_str());
            uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
            uint32_t resultCapabilities = installAbility.GetStaticCapabilityValues() & capabilities;
            installAbility.SetCapabilityValues(resultCapabilities);
            std::string uri = Utils::GetUri(bundleName, abilityName);
            AddEnabledAbility(uri);
        }
    }
}

bool AccessibleAbilityManager::IsExistCapability(Capability capability)
{
    return connectedA11yAbilities_.IsExistCapability(capability);
}

int32_t AccessibleAbilityManager::GetSizeByUri(const std::string& uri)
{
    return connectedA11yAbilities_.GetSizeByUri(uri);
}

void AccessibleAbilityManager::UpdateAbilities(
    const std::string& callerBundleName,
    int32_t accountId,
    uint32_t& connectCounter,
    std::function<sptr<AccessibleAbilityConnection>(
        int32_t, int32_t, AccessibilityAbilityInfo&)> connectionCreator)
{
    HILOG_DEBUG("UpdateAbilities start.");
    for (auto& installAbility : installedAbilities_) {
        std::string uri = Utils::GetUri(installAbility.GetPackageName(), installAbility.GetName());
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            installAbility.GetPackageName().c_str(), installAbility.GetName().c_str());
        if (connectingA11yAbilities_.GetSizeByUri(
            Utils::GetUri(installAbility.GetPackageName(), installAbility.GetName()))) {
            HILOG_DEBUG("The ability(bundleName[%{public}s] abilityName[%{public}s]) is connecting.",
                installAbility.GetPackageName().c_str(),
                installAbility.GetName().c_str());
            continue;
        }
        auto connection = GetConnectedAbilityByName(uri);
        bool isEnabled = IsAbilityEnabled(uri);
        if (isEnabled && !connection) {
            if (!connectionCreator) {
                HILOG_ERROR("connectionCreator is null");
                continue;
            }
            connection = connectionCreator(accountId, connectCounter++, installAbility);
            if (connection && ConnectAbility(connection, installAbility, callerBundleName)) {
                AddConnectingAbility(uri, connection);
            }
        } else if (!isEnabled && connection) {
            HILOG_DEBUG("not in enabledAbilites list.");
            DisconnectAbility(connection, uri);
        }
    }
}

bool AccessibleAbilityManager::IsAbilityEnabled(const std::string& uri)
{
    const auto& enabledAbilities = GetEnabledAbilities();
    return std::find(enabledAbilities.begin(), enabledAbilities.end(), uri) != enabledAbilities.end();
}

bool AccessibleAbilityManager::ConnectAbility(
    sptr<AccessibleAbilityConnection>& connection,
    const AccessibilityAbilityInfo& ability,
    const std::string& callerBundleName)
{
    AppExecFwk::ElementName element("", ability.GetPackageName(), ability.GetName());
    if (!connection->Connect(element)) {
        HILOG_ERROR("Connect failed for %{public}s", ability.GetId().c_str());
        return false;
    }
    if (!callerBundleName.empty()) {
        connection->SetConnectionKey(callerBundleName);
    }
    return true;
}

void AccessibleAbilityManager::DisconnectAbility(
    sptr<AccessibleAbilityConnection>& connection,
    const std::string& uri)
{
    AddWaitDisconnectAbility(connection);
    RemoveConnectedAbility(connection->GetElementName());
    connection->Disconnect();
}

void AccessibleAbilityManager::AccessibilityAbility::AddAccessibilityAbility(const std::string& uri,
    const sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_INFO("uri is %{private}s", uri.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!connectionMap_.count(uri)) {
        connectionMap_[uri] = connection;
        HILOG_DEBUG("connectionMap_ size %{public}zu", connectionMap_.size());
        return;
    }

    HILOG_DEBUG("uri %{private}s, connectionMap_ %{public}zu", uri.c_str(), connectionMap_.size());
}

sptr<AccessibleAbilityConnection> AccessibleAbilityManager::AccessibilityAbility::GetAccessibilityAbilityByName(
    const std::string& elementName)
{
    HILOG_DEBUG("elementName is %{public}s", elementName.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        std::string::size_type index = connection.first.find(elementName);
        if (index == std::string::npos) {
            continue;
        } else {
            HILOG_DEBUG("uri %{private}s ", connection.first.c_str());
            return connection.second;
        }
    }

    return nullptr;
}

sptr<AccessibleAbilityConnection> AccessibleAbilityManager::AccessibilityAbility::GetAccessibilityAbilityByUri(
    const std::string& uri)
{
    HILOG_DEBUG("uri is %{private}s", uri.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

void AccessibleAbilityManager::AccessibilityAbility::GetAccessibilityAbilities(
    std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        connectionList.push_back(connection.second);
    }
}

void AccessibleAbilityManager::AccessibilityAbility::GetAbilitiesInfo(std::vector<AccessibilityAbilityInfo>& abilities)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        if (connection.second) {
            abilities.push_back(connection.second->GetAbilityInfo());
        }
    }

    HILOG_DEBUG("connectionMap_ %{public}zu and enabledAbilities %{public}zu",
        connectionMap_.size(), abilities.size());
}

bool AccessibleAbilityManager::AccessibilityAbility::IsExistCapability(Capability capability)
{
    HILOG_DEBUG("capability %{public}d", capability);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = connectionMap_.begin(); iter != connectionMap_.end(); iter++) {
        if (iter->second->GetAbilityInfo().GetCapabilityValues() & capability) {
            return true;
        }
    }

    return false;
}

void AccessibleAbilityManager::AccessibilityAbility::GetAccessibilityAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    connectionMap = connectionMap_;
}

void AccessibleAbilityManager::AccessibilityAbility::GetDisableAbilities(
    std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        for (auto iter = disabledAbilities.begin(); iter != disabledAbilities.end();) {
            if (connection.second && (iter->GetId() == connection.second->GetAbilityInfo().GetId())) {
                iter = disabledAbilities.erase(iter);
            } else {
                iter++;
            }
        }
    }
}

int32_t AccessibleAbilityManager::AccessibilityAbility::GetSizeByUri(const std::string& uri)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.count(uri);
}

void AccessibleAbilityManager::AccessibilityAbility::RemoveAccessibilityAbilityByName(const std::string& bundleName,
    bool& result)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        std::size_t firstPos = connection.first.find_first_of('/') + 1;
        std::size_t endPos = connection.first.find_last_of('/');
        if (endPos <= firstPos) {
            HILOG_ERROR("it's a wrong ability and the uri %{public}s ", connection.first.c_str());
            continue;
        }

        std::string connectedBundleName = connection.first.substr(firstPos, endPos - firstPos);
        if (connectedBundleName == bundleName) {
            HILOG_DEBUG("remove connected ability, bundle name %{public}s", connectedBundleName.c_str());
            std::string uri = Utils::GetUri(connection.second->GetElementName());
            auto it = connectionMap_.find(uri);
            if (it != connectionMap_.end()) {
                connectionMap_.erase(it);
            }
            result = true;
        }
    }
}

void AccessibleAbilityManager::AccessibilityAbility::RemoveAccessibilityAbilityByUri(const std::string& uri)
{
    HILOG_INFO("uri is %{private}s", uri.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto it = connectionMap_.find(uri);
    if (it != connectionMap_.end()) {
        connectionMap_.erase(it);
    }

    HILOG_DEBUG("connectionMap_ %{public}zu", connectionMap_.size());
}

void AccessibleAbilityManager::AccessibilityAbility::Clear()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.clear();
}

size_t AccessibleAbilityManager::AccessibilityAbility::GetSize()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.size();
}

} // namespace Accessibility
} // namespace OHOS