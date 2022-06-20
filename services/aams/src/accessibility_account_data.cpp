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

#include "accessibility_account_data.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
AccessibilityAccountData::AccessibilityAccountData(int32_t accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int32_t AccessibilityAccountData::GetAccountId()
{
    HILOG_INFO("id_ = %{public}d", id_);
    return id_;
}

uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_INFO();
    uint32_t state = 0;
    if (!connectedA11yAbilities_.empty() || !connectingA11yAbilities_.empty()) {
        state |= STATE_ACCESSIBILITY_ENABLED;
        if (!config_->GetEnabledState()) {
            config_->SetEnabled(true);
        }
    } else {
        if (config_->GetEnabledState()) {
            config_->SetEnabled(false);
        }
    }

    if (config_->GetTouchGuideState()) {
        state |= STATE_EXPLORATION_ENABLED;
    }
    
    if (config_->GetKeyEventObserverState()) {
        state |= STATE_KEYEVENT_ENABLED;
    }

    if (config_->GetGestureState()) {
        state |= STATE_GESTURE_ENABLED;
    }
    return state;
}

void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_INFO();
    connectingA11yAbilities_.clear();
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        itr->second->Disconnect();
    }
}

void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    std::string uri = Utils::GetUri(connection->GetElementName());
    HILOG_INFO("URI is %{private}s", uri.c_str());
    if (!connectedA11yAbilities_.count(uri)) {
        connectedA11yAbilities_.insert(make_pair(uri, connection));
    }

    HILOG_INFO("Add ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    std::string uri = Utils::GetUri(connection->GetElementName());
    HILOG_INFO("URI is %{private}s", uri.c_str());
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it = connectedA11yAbilities_.find(uri);
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_INFO("Remove ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    HILOG_INFO();
    stateCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_INFO();
    for (auto itr = stateCallbacks_.begin(); itr != stateCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            stateCallbacks_.erase(itr);
            break;
        }
    }
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_INFO();
    captionPropertyCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_INFO();
    for (auto itr = captionPropertyCallbacks_.begin(); itr != captionPropertyCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            captionPropertyCallbacks_.erase(itr);
            break;
        }
    }
}

void AccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    HILOG_INFO();
    for (auto &enableAbilityListsObserver : enableAbilityListsObservers_) {
        if (enableAbilityListsObserver == observer) {
            HILOG_ERROR("observer is already exist");
            return;
        }
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void AccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    HILOG_INFO();
    for (auto itr = enableAbilityListsObservers_.begin(); itr != enableAbilityListsObservers_.end(); itr++) {
        if ((*itr)->AsObject() == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(itr);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return;
        }
    }
}

void AccessibilityAccountData::UpdateEnableAbilityListsState()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityEnableAbilityListsChanged();
        }
    }
}

void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    std::map<int32_t, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &name)
{
    HILOG_DEBUG("start.");
    for (auto &ability : connectingA11yAbilities_) {
        if (ability == name) {
            HILOG_ERROR("The ability is already connecting, and it's name is %{public}s", name.c_str());
            return;
        }
    }
    connectingA11yAbilities_.push_back(name);
    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = connectingA11yAbilities_.begin(); it != connectingA11yAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from ConnectingA11yAbility: ", name.c_str());
            connectingA11yAbilities_.erase(it);
            HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not connecting.", name.c_str());
}

void AccessibilityAccountData::AddEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start.");
    for (auto &ability : enabledAbilities_) {
        if (ability == name) {
            HILOG_ERROR("The ability is already enabled, and it's name is %{public}s", name.c_str());
            return;
        }
    }
    enabledAbilities_.push_back(name);
    UpdateEnabledFromPref();
    UpdateEnableAbilityListsState();
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledFromPref(const std::string &name)
{
    HILOG_DEBUG("start");
    std::string ability = name + "/" + std::to_string(GetAbilityCapabilities(name));

    std::vector<std::string> vecvalue;
    vecvalue = config_->GetEnabledAbilityInfos();

    for (std::vector<std::string>::iterator val = vecvalue.begin();val != vecvalue.end();) {
        if (!std::strcmp(val->c_str(), ability.c_str())) {
            val = vecvalue.erase(val);
            HILOG_DEBUG("remove val = %{public}s", val->c_str());
        } else {
            ++val;
        }
    }
    config_->UpdateEnabledAbilities(vecvalue);
}

void AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            RemoveEnabledFromPref(name);
            UpdateEnableAbilityListsState();
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", name.c_str());
}

void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    HILOG_DEBUG("abilityInfo's bundle name is %{public}s", abilityInfo.GetPackageName().c_str());
    for (size_t i = 0; i < installedAbilities_.size(); i++) {
        if (installedAbilities_[i].GetPackageName() == abilityInfo.GetPackageName()) {
            HILOG_DEBUG("the ability is already exist.");
            return;
        }
    }
    installedAbilities_.push_back(abilityInfo);
    HILOG_DEBUG("push back installed ability successfully and installedAbilities_'s size is %{public}zu",
        installedAbilities_.size());
}

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    HILOG_DEBUG("start.");
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == bundleName) {
            HILOG_DEBUG("Removed %{public}s from InstalledAbility: ", bundleName.c_str());
            installedAbilities_.erase(it);
        } else {
            ++it;
        }
    }
}

void AccessibilityAccountData::ClearInstalledAbility()
{
    HILOG_DEBUG("start.");
    installedAbilities_.clear();
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    HILOG_DEBUG("URI is %{public}s", elementName.c_str());
    for (auto& connected : connectedA11yAbilities_) {
        std::string::size_type  idx = connected.first.find(elementName);
        if (idx == std::string::npos) {
            continue;
        } else {
            HILOG_DEBUG("founded URI = %{public}s ", connected.first.c_str());
            return connected.second;
        }
    }
    HILOG_DEBUG("URI %{public}s not found ", elementName.c_str());
    return nullptr;
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    HILOG_DEBUG("windowId[%{public}d] interactionOperators's size[%{public}zu]", windowId, asacConnections_.size());
    for (auto &asacConnection : asacConnections_) {
        HILOG_DEBUG("The window id of asacConnection is %{public}d", asacConnection.first);
    }

    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

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

const std::vector<std::string> &AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectingA11yAbilities_;
}

const std::vector<std::string> &AccessibilityAccountData::GetEnabledAbilities()
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

void AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state,
    std::vector<AccessibilityAbilityInfo> &abilities) const
{
    HILOG_DEBUG("state(%{public}d) start.", state);
    if (state == ABILITY_STATE_ENABLE) {
        for (auto &ability : connectedA11yAbilities_) {
            if (ability.second) {
                abilities.push_back(ability.second->GetAbilityInfo());
            }
        }
        HILOG_DEBUG("connectedA11yAbilities_ is (%{public}zu) enabledAbilities is (%{public}zu).",
            connectedA11yAbilities_.size(), abilities.size());
    } else if (state == ABILITY_STATE_DISABLE) {
        GetDisableAbilities(abilities);
        HILOG_DEBUG("the size of disable abilities is %{public}zu", abilities.size());
    } else {
        abilities = installedAbilities_;
    }
}

void AccessibilityAccountData::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities) const
{
    HILOG_DEBUG("start.");
    disabledAbilities = installedAbilities_;
    for (auto &enabledAbility : connectedA11yAbilities_) {
        for (auto itr = disabledAbilities.begin(); itr != disabledAbilities.end();) {
            if (enabledAbility.second && (itr->GetId() == enabledAbility.second->GetAbilityInfo().GetId())) {
                disabledAbilities.erase(itr);
            } else {
                itr++;
            }
        }
    }
}

void AccessibilityAccountData::UpdateAccountCapabilities()
{
    HILOG_DEBUG("start.");
    UpdateFilteringKeyEventsCapability();
    UpdateEventTouchGuideCapability();
    UpdateGesturesSimulationCapability();
    UpdateMagnificationCapability();
    config_->SetTouchGuideState(isEventTouchGuideState_);
    config_->SetGestureState(isGesturesSimulation_);
    config_->SetKeyEventObserverState(isFilteringKeyEvents_);
}

void AccessibilityAccountData::UpdateEventTouchGuideCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_TOUCH_GUIDE) {
            isEventTouchGuideState_ = true;
            return;
        }
    }
    isEventTouchGuideState_ = false;
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE) {
            isGesturesSimulation_ = true;
            return;
        }
    }
    isGesturesSimulation_ = false;
}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
            isFilteringKeyEvents_ = true;
            return;
        }
    }
    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM) {
            isScreenMagnification_ = true;
            return;
        }
    }
    isScreenMagnification_ = false;
}

void AccessibilityAccountData::UpdateEnabledFromPref()
{
    std::vector<std::string> vecvalue;
    for (auto& enabledAbility : enabledAbilities_) {
        uint32_t capabilities = GetAbilityCapabilities(enabledAbility);
        if (capabilities == 0) {
            HILOG_ERROR("capabilities is wrong!");
            continue;
        }
        std::string ability = enabledAbility + "/" + std::to_string(capabilities);
        vecvalue.push_back(ability);
        HILOG_DEBUG("ability is %{public}s ", ability.c_str());
    }
    config_->UpdateEnabledAbilities(vecvalue);
}

bool AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start and name[%{public}s] capabilities[%{public}d]", name.c_str(), capabilities);

    // Parse name to bundle name and ability name
    std::string bundleName = name.substr(0, name.find("/"));
    std::string abilityName = name.substr(name.find("/") + 1);
    HILOG_DEBUG("bundleName[%{public}s], abilityName[%{public}s]", bundleName.c_str(), abilityName.c_str());

    // Parse config from bms according to bundle name and ability name
    uint32_t configCapabilities = GetConfigCapabilitiesFromBms(bundleName, abilityName);

    // Judge capabilities
    uint32_t resultCapabilities = configCapabilities & capabilities;
    HILOG_DEBUG("resultCapabilities is [%{public}d]", resultCapabilities);
    if (resultCapabilities == 0) {
        HILOG_ERROR("the result of capabilities is wrong");
        return false;
    }

    bool result = SetAbilityCapabilities(name, resultCapabilities);
    if (!result) {
        HILOG_ERROR("Reset capabilities failed");
        return false;
    }

    // Add enabled ability
    for (auto &enabledAbility : enabledAbilities_) {
        if (enabledAbility == name) {
            HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
            return false;
        }
    }
    enabledAbilities_.push_back(name);
    UpdateEnabledFromPref();
    UpdateEnableAbilityListsState();
    return true;
}

uint32_t AccessibilityAccountData::GetConfigCapabilitiesFromBms(const std::string &bundleName,
    const std::string &abilityName) const
{
    HILOG_DEBUG("start");
    sptr<AppExecFwk::IBundleMgr> bmsMgr =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bmsMgr) {
        HILOG_ERROR("bmsMgr is nullptr.");
        return 0;
    }

    uint32_t configCapabilities = 0;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    bmsMgr->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &info : extensionInfos) {
        if (info.bundleName == bundleName && info.name == abilityName) {
            AccessibilityAbilityInitParams initParams;
            Utils::Parse(info, initParams);
            configCapabilities = initParams.capabilities;
            HILOG_DEBUG("configCapabilities is [%{public}d]", configCapabilities);
            return configCapabilities;
        }
    }
    HILOG_ERROR("Query ability from bms failed. bundleName[%{public}s] abilityName[%{public}s]",
        bundleName.c_str(), abilityName.c_str());
    return 0;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bms = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bms) {
        HILOG_ERROR("GetBundleMgrProxy failed.");
        return false;
    }
    bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto& info : extensionInfos) {
        AccessibilityAbilityInitParams initParams;
        Utils::Parse(info, initParams);
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(initParams);
        AddInstalledAbility(*accessibilityInfo);
    }

    std::map<std::string, uint32_t> abilityCapabilities;
    GetCapabilitiesFromConfig(abilityCapabilities);

    for (auto &ability : abilityCapabilities) {
        bool result = SetAbilityCapabilities(ability.first, ability.second);
        if (!result) {
            HILOG_ERROR("set ability capabilities failed!");
            continue;
        }
        enabledAbilities_.push_back(ability.first);
        UpdateEnabledFromPref();
        UpdateEnableAbilityListsState();
    }
    return true;
}

void AccessibilityAccountData::Init()
{
    HILOG_DEBUG("start.");
    if (!config_) {
        config_ = std::make_shared<AccessibilitySettingsConfig>(id_);
        config_->Init();
    }
}

bool AccessibilityAccountData::SetAbilityCapabilities(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start. name[%{public}s] capabilities[%{public}d]", name.c_str(), capabilities);
    for (auto &installedAbility : installedAbilities_) {
        if (installedAbility.GetId() == name) {
            HILOG_DEBUG("reset ability capabilities");
            installedAbility.SetCapabilityValues(capabilities);
            return true;
        }
    }
    HILOG_ERROR("not found the ability %{public}s", name.c_str());
    return false;
}

uint32_t AccessibilityAccountData::GetAbilityCapabilities(const std::string &name) const
{
    HILOG_DEBUG("start. name[%{public}s]", name.c_str());
    for (auto &installedAbility : installedAbilities_) {
        if (installedAbility.GetId() == name) {
            return installedAbility.GetCapabilityValues();
        }
    }
    HILOG_ERROR("not found the ability %{public}s", name.c_str());
    return 0;
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

void AccessibilityAccountData::GetCapabilitiesFromConfig(std::map<std::string, uint32_t> &abilityCapabilities)
{
    HILOG_DEBUG("start.");
    std::vector<std::string> vecvalue = config_->GetEnabledAbilityInfos();
    for (auto &value : vecvalue) {
        HILOG_DEBUG("BundleName/AbilityName/Capabilities = %{public}s", value.c_str());
        std::string name = value.substr(0, value.find_last_of("/"));
        std::string capabilities = value.substr(value.find_last_of("/") + 1);
        uint32_t capability = static_cast<uint32_t>(std::atoi(capabilities.c_str()));
        HILOG_DEBUG("name[%{public}s] capability[%{public}d]", name.c_str(), capability);
        if (capability == 0) {
            HILOG_ERROR("capability is wrong!");
            continue;
        }
        abilityCapabilities.insert(std::make_pair(name, capability));
    }
}
} // namespace Accessibility
} // namespace OHOS
