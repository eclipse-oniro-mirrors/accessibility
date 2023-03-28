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

#include <any>
#include <hitrace_meter.h>

#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t AUTOCLICK_DELAY_TIME_MIN = 1000; // ms
    constexpr int32_t AUTOCLICK_DELAY_TIME_MAX = 5000; // ms
} // namespace

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
        HILOG_DEBUG("connectingA11yAbilities[%{public}zu] connectedA11yAbilities[%{public}zu]",
            connectingA11yAbilities_.size(), connectedA11yAbilities_.size());
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
    connectedA11yAbilities_.clear();
    enabledAbilities_.clear();
    asacConnections_.clear();
}

void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    std::string uri = Utils::GetUri(connection->GetElementName());
    HILOG_INFO("URI is %{private}s", uri.c_str());
    if (!connectedA11yAbilities_.count(uri)) {
        connectedA11yAbilities_.insert(make_pair(uri, connection));
    }

    HILOG_INFO("Add ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    std::string uri = Utils::GetUri(element);
    HILOG_INFO("URI is %{private}s", uri.c_str());
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it = connectedA11yAbilities_.find(uri);
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_INFO("Remove ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
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
    auto iter = asacConnections_.find(windowId);
    if (iter == asacConnections_.end()) {
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

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG("start.");
    auto iter = connectingA11yAbilities_.find(uri);
    if (iter == connectingA11yAbilities_.end()) {
        connectingA11yAbilities_[uri] = connection;
        HILOG_DEBUG("Add ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
    } else {
        HILOG_ERROR("The ability is already connecting, and it's uri is %{public}s", uri.c_str());
    }
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    HILOG_DEBUG("start");
    auto iter = connectingA11yAbilities_.find(uri);
    if (iter != connectingA11yAbilities_.end()) {
        connectingA11yAbilities_.erase(iter);
        return;
    }
    HILOG_ERROR("The ability(%{public}s) is not connecting.", uri.c_str());
}

void AccessibilityAccountData::AddEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start.");
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

RetError AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            UpdateEnableAbilityListsState();
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return RET_OK;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", name.c_str());
    return RET_ERR_NOT_ENABLED;
}

void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
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
    HILOG_DEBUG("push back installed ability successfully and installedAbilities_'s size is %{public}zu",
        installedAbilities_.size());
}

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
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
    HILOG_DEBUG("connectedA11yAbilities size[%{public}zu]", connectedA11yAbilities_.size());
    return connectedA11yAbilities_;
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

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    HILOG_DEBUG("start.");
    auto iter = connectingA11yAbilities_.find(uri);
    if (iter != connectingA11yAbilities_.end()) {
        return iter->second;
    }
    return nullptr;
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

RetError AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start and name[%{public}s] capabilities[%{public}d]", name.c_str(), capabilities);

    bool isInstalled = false;
    for (auto itr = installedAbilities_.begin(); itr != installedAbilities_.end(); itr++) {
        if (itr->GetId() == name) {
            isInstalled = true;

            // Judge capabilities
            uint32_t resultCapabilities = itr->GetStaticCapabilityValues() & capabilities;
            HILOG_DEBUG("resultCapabilities is [%{public}d]", resultCapabilities);
            if (resultCapabilities == 0) {
                HILOG_ERROR("the result of capabilities is wrong");
                return RET_ERR_NO_CAPABILITY;
            }

            itr->SetCapabilityValues(capabilities);
            break;
        }
    }
    if (!isInstalled) {
        HILOG_ERROR("the ability[%{public}s] is not installed", name.c_str());
        return RET_ERR_NOT_INSTALLED;
    }

    // Add enabled ability
    if (std::any_of(enabledAbilities_.begin(), enabledAbilities_.end(),
        [name](const std::string &abilityName) {
            return abilityName == name;
        })) {
        HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
        return RET_ERR_CONNECTION_EXIST;
    }
    enabledAbilities_.push_back(name);
    UpdateEnableAbilityListsState();
    UpdateAbilities();
    Utils::RecordStartingA11yEvent(name);
    return RET_OK;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryInstalledAbilityInfo");
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bms = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bms) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_PACKAGE_INFO_FAILED);
        HILOG_ERROR("GetBundleMgrProxy failed.");
        return false;
    }
    bool ret = bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    if (!ret) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_PACKAGE_INFO_FAILED);
        HILOG_ERROR("Query extension ability information failed.");
        return false;
    }
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto& info : extensionInfos) {
        AccessibilityAbilityInitParams initParams;
        Utils::Parse(info, initParams);
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(initParams);
        AddInstalledAbility(*accessibilityInfo);
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

void AccessibilityAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("start.");
    configCallbacks_ = observer;
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
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("Current user has no installedAbilities.");
        return;
    }
    if (enabledAbilities_.empty()) {
        HILOG_DEBUG("Current user has no enabledabilities.");
        return;
    }
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
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

void AccessibilityAccountData::UpdateImportantEnabledAbilities(
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

uint32_t AccessibilityAccountData::GetInputFilterFlag() const
{
    HILOG_DEBUG();
    if (!config_) {
        HILOG_ERROR("config is null");
        return 0;
    }
    uint32_t flag = 0;
    if (isScreenMagnification_ && config_->GetScreenMagnificationState()) {
        flag |= AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    }
    if (isEventTouchGuideState_) {
        flag |= AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;
    }
    if (isFilteringKeyEvents_) {
        flag |= AccessibilityInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    }
    if (isGesturesSimulation_) {
        flag |= AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    }
    if (config_->GetShortKeyState()) {
        flag |= AccessibilityInputInterceptor::FEATURE_SHORT_KEY;
    }
    if (config_->GetMouseKeyState()) {
        flag |= AccessibilityInputInterceptor::FEATURE_MOUSE_KEY;
    }

    int32_t autoclickTime = config_->GetMouseAutoClick();
    if (autoclickTime >= AUTOCLICK_DELAY_TIME_MIN && autoclickTime <= AUTOCLICK_DELAY_TIME_MAX) {
        flag |= AccessibilityInputInterceptor::FEATURE_MOUSE_AUTOCLICK;
    }

    return flag;
}

void AccessibilityAccountData::UpdateAbilities()
{
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities_.size());
    for (auto &installAbility : installedAbilities_) {
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(), abilityName.c_str());

        if (connectingA11yAbilities_.count(Utils::GetUri(bundleName, abilityName))) {
            HILOG_DEBUG("The ability(bundleName[%{public}s] abilityName[%{public}s]) is connecting.",
                bundleName.c_str(), abilityName.c_str());
            continue;
        }
        sptr<AccessibleAbilityConnection> connection =
            GetAccessibleAbilityConnection(Utils::GetUri(bundleName, abilityName));

        auto iter = std::find(enabledAbilities_.begin(), enabledAbilities_.end(),
            Utils::GetUri(bundleName, abilityName));
        if (iter != enabledAbilities_.end()) {
            if (connection) {
                continue;
            }
            AppExecFwk::ElementName element("", bundleName, abilityName);
            connection = new(std::nothrow) AccessibleAbilityConnection(id_, connectCounter_++, installAbility);
            if (connection) {
                connection->Connect(element);
                AddConnectingA11yAbility(Utils::GetUri(bundleName, abilityName), connection);
            }
        } else {
            HILOG_DEBUG("not in enabledAbilites list .");
            if (connection) {
                RemoveConnectedAbility(connection->GetElementName());
                connection->Disconnect();
            }
        }
    }
}

bool AccessibilityAccountData::RemoveAbility(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return false;
    }
    RemoveInstalledAbility(bundleName);

    bool result = false;
    // Remove enabled ability, remove connecting ability if it is connecting.
    for (auto& enableAbility : enabledAbilities_) {
        if (enableAbility.substr(0, enableAbility.find("/")) == bundleName) {
            RemoveEnabledAbility(enableAbility);
            RemoveConnectingA11yAbility(enableAbility);
            result = true;
        }
    }

    // Remove connected ability
    for (auto &connectedAbility : connectedA11yAbilities_) {
        std::size_t firstPos = connectedAbility.first.find_first_of('/') + 1;
        std::size_t endPos = connectedAbility.first.find_last_of('/');
        if (endPos <= firstPos) {
            HILOG_ERROR("it's a wrong ability and the uri is %{public}s", connectedAbility.first.c_str());
            continue;
        }
        std::string connectedBundleName = connectedAbility.first.substr(firstPos, endPos - firstPos);
        if (connectedBundleName == bundleName) {
            HILOG_DEBUG("Remove connected ability and it's bundle name is %{public}s", connectedBundleName.c_str());
            RemoveConnectedAbility(connectedAbility.second->GetElementName());
            result = true;
        }
    }

    if (result) {
        UpdateAbilities();
    }
    return result;
}

void AccessibilityAccountData::AddAbility(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bms = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bms) {
        HILOG_ERROR("bms is nullptr.");
        return;
    }
    bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    bool hasNewExtensionAbility = false;
    for (auto &newAbility : extensionInfos) {
        if (newAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package%{public}s added", (bundleName + "/" + newAbility.name).c_str());
            AccessibilityAbilityInitParams initParams;
            Utils::Parse(newAbility, initParams);
            std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
                std::make_shared<AccessibilityAbilityInfo>(initParams);
            AddInstalledAbility(*accessibilityInfo);
            hasNewExtensionAbility = true;
        }
    }

    if (hasNewExtensionAbility) {
        HILOG_DEBUG("add new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibilityAccountData::ChangeAbility(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    if (installedAbilities_.empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    RemoveInstalledAbility(bundleName);
    AddAbility(bundleName);
}

void AccessibilityAccountData::AddUITestClient(const sptr<IRemoteObject> &obj,
    const std::string &bundleName, const std::string &abilityName)
{
    HILOG_DEBUG();
    // Add installed ability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    abilityInfo->SetPackageName(bundleName);
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetAccessibilityAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    AddInstalledAbility(*abilityInfo);

    // Add connected ability
    sptr<AppExecFwk::ElementName> elementName = new(std::nothrow) AppExecFwk::ElementName();
    if (!elementName) {
        HILOG_ERROR("elementName is null");
        return;
    }
    elementName->SetBundleName(bundleName);
    elementName->SetAbilityName(abilityName);
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(
        id_, connectCounter_++, *abilityInfo);
    if (!connection) {
        HILOG_ERROR("connection is null");
        return;
    }
    connection->OnAbilityConnectDoneSync(*elementName, obj);
}

void AccessibilityAccountData::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection,
    const std::string &bundleName)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    RemoveInstalledAbility(bundleName);
    RemoveConnectedAbility(connection->GetElementName());
    connection->OnAbilityDisconnectDoneSync(connection->GetElementName());
}
} // namespace Accessibility
} // namespace OHOS
