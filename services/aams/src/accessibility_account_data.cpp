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
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "utils.h"
#include "system_ability_definition.h"
#include "os_account_manager.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t AUTOCLICK_DELAY_TIME_MIN = 1000; // ms
    constexpr int32_t AUTOCLICK_DELAY_TIME_MAX = 5000; // ms
    constexpr int32_t INIT_DATASHARE_HELPER_SLEEP_TIME = 500;
    constexpr int DOUBLE_CLICK_RESPONSE_TIME_MEDIUM = 300;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT = 400;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_MEDIUM = 700;
    constexpr int DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG = 1000;
    constexpr int DISPLAY_DALTONIZER_GREEN = 12;
    constexpr int DISPLAY_DALTONIZER_RED = 11;
    constexpr int DISPLAY_DALTONIZER_BLUE = 13;
    constexpr int DEFAULT_ACCOUNT_ID = 100;
    const std::string HIGH_TEXT_CONTRAST_ENABLED = "high_text_contrast_enabled";
    const std::string ACCESSIBILITY_DISPLAY_INVERSION_ENABLED = "accessibility_display_inversion_enabled";
    const std::string ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED = "accessibility_display_daltonizer_enabled";
    const std::string MASTER_MONO = "master_mono";
    const std::string ACCESSIBILITY_SCREENREADER_ENABLED = "accessibility_screenreader_enabled";
    const std::string MASTER_BALENCE = "master_balance";
    const std::string CLICK_RESPONSE_TIME = "click_response_time";
    const std::string IGNORE_REPEAT_CLICK_SWITCH = "ignore_repeat_click_switch";
    const std::string IGNORE_REPEAT_CLICK_TIME = "ignore_repeat_click_time";
    const std::string ACCESSIBILITY_DISPLAY_DALTONIZER = "accessibility_display_daltonizer";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
} // namespace

AccessibilityAccountData::AccessibilityAccountData(int32_t accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int32_t AccessibilityAccountData::GetAccountId()
{
    HILOG_INFO();
    return id_;
}

uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_DEBUG();
    uint32_t state = 0;
    if (connectedA11yAbilities_.GetSize() != 0 || connectingA11yAbilities_.GetSize() != 0) {
        HILOG_DEBUG("connectingA11yAbilities %{public}zu connectedA11yAbilities %{public}zu",
            connectingA11yAbilities_.GetSize(), connectedA11yAbilities_.GetSize());
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
    connectingA11yAbilities_.Clear();
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    connectedA11yAbilities_.GetAccessibilityAbilities(connectionList);
    for (auto& connection : connectionList) {
        if (connection) {
            connection->Disconnect();
        }
    }

    connectedA11yAbilities_.Clear();
    enabledAbilities_.clear();
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_.clear();
}

void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    std::string uri = Utils::GetUri(connection->GetElementName());
    connectedA11yAbilities_.AddAccessibilityAbility(uri, connection);
}

void AccessibilityAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    connectedA11yAbilities_.RemoveAccessibilityAbilityByUri(Utils::GetUri(element));
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_DEBUG();
    captionPropertyCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();
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

void AccessibilityAccountData::UpdateInstallAbilityListsState()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityInstallAbilityListsChanged();
        }
    }
}

void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_[windowId] = interactionConnection;
}

void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    std::map<int32_t, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
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

void AccessibilityAccountData::AddEnabledAbility(const std::string &name)
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

RetError AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
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

void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
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

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    HILOG_DEBUG("start.");
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == bundleName) {
            HILOG_DEBUG("Removed %{public}s from InstalledAbility: ", bundleName.c_str());
            if (!config_) {
                it = installedAbilities_.erase(it);
                UpdateInstallAbilityListsState();
                continue;
            }
            it = installedAbilities_.erase(it);
            UpdateInstallAbilityListsState();
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
    return connectedA11yAbilities_.GetAccessibilityAbilityByName(elementName);
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    std::lock_guard lock(asacConnectionsMutex_);
    HILOG_DEBUG("window id[%{public}d] interactionOperators's size[%{public}zu]", windowId, asacConnections_.size());
    for (auto &asacConnection : asacConnections_) {
        HILOG_DEBUG("window id of asacConnection is %{public}d", asacConnection.first);
    }

    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    connectedA11yAbilities_.GetAccessibilityAbilitiesMap(connectionMap);
    return connectionMap;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start.");
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start.");
    return captionPropertyCallbacks_;
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    return connectingA11yAbilities_.GetAccessibilityAbilityByUri(uri);
}

const std::vector<std::string> &AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_'s size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName is %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("GetInstalledAbilities start.");
    return installedAbilities_;
}

void AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state,
    std::vector<AccessibilityAbilityInfo> &abilities)
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

void AccessibilityAccountData::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    HILOG_DEBUG("get disable abilities");
    disabledAbilities = installedAbilities_;
    connectedA11yAbilities_.GetDisableAbilities(disabledAbilities);
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
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_TOUCH_GUIDE)) {
        isEventTouchGuideState_ = true;
        return;
    }

    isEventTouchGuideState_ = false;
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_GESTURE)) {
        isGesturesSimulation_ = true;
        return;
    }

    isGesturesSimulation_ = false;
}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        isFilteringKeyEvents_ = true;
        return;
    }

    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_ZOOM)) {
        isScreenMagnification_ = true;
        return;
    }

    isScreenMagnification_ = false;
}

void AccessibilityAccountData::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    HILOG_DEBUG("start and name[%{public}s], state = [%{public}s].", name.c_str(), state ? "True" : "False");
    AccessibilitySettingProvider& provider = AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    std::string strState = state ? "on" : "off";
    std::string abilityAutoStartKey = name + "/" + std::to_string(id_);
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME && GetAccountType() == AccountSA::OsAccountType::PRIVATE) {
        abilityAutoStartKey = name + "/" + std::to_string(DEFAULT_ACCOUNT_ID);
    }
    ErrCode ret = provider.PutStringValue(abilityAutoStartKey, strState, false);
    if (ret != ERR_OK) {
        HILOG_ERROR("set failed, ret=%{public}d", ret);
    }
    provider.DeleteInstance();
}

void AccessibilityAccountData::SetScreenReaderState(const std::string &name, const std::string &state)
{
    HILOG_DEBUG("set screen reader key [%{public}s], state = [%{public}s].", name.c_str(), state.c_str());
    AccessibilitySettingProvider& provider = AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    ErrCode ret = provider.PutStringValue(name, state, true);
    if (ret != ERR_OK) {
        HILOG_ERROR("set failed, ret=%{public}d", ret);
    }
}

bool AccessibilityAccountData::GetScreenReaderState()
{
    HILOG_DEBUG();
    AccessibilitySettingProvider& provider = AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    bool state = false;
    provider.GetBoolValue(ACCESSIBILITY_SCREENREADER_ENABLED, state);
    return state;
}

void AccessibilityAccountData::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
{
    HILOG_ERROR("start and bundleName[%{public}s].", bundleName.c_str());
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

bool AccessibilityAccountData::GetAbilityAutoStartState(const std::string &name)
{
    HILOG_DEBUG();
    AccessibilitySettingProvider& provider = AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    std::string strValue;
    std::string strAutoStartStateKey = name + "/" + std::to_string(id_);
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME && GetAccountType() == AccountSA::OsAccountType::PRIVATE) {
        strAutoStartStateKey = name + "/" + std::to_string(DEFAULT_ACCOUNT_ID);
    }
    ErrCode ret = provider.GetStringValue(strAutoStartStateKey, strValue);
    if (ret == ERR_NO_INIT) {
        HILOG_INFO("helper is null, retry.");
        std::this_thread::sleep_for(std::chrono::milliseconds(INIT_DATASHARE_HELPER_SLEEP_TIME));
        ret = provider.GetStringValue(strAutoStartStateKey, strValue);
    }
    if (ret != ERR_OK) {
        HILOG_ERROR("get failed, key = %{public}s, ret=%{public}d", strAutoStartStateKey.c_str(), ret);
        provider.DeleteInstance();
        return false;
    }
    HILOG_DEBUG("provider GetString = %{public}s, key = %{public}s.", strValue.c_str(), strAutoStartStateKey.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        provider.DeleteInstance();
        return true;
    }
    provider.DeleteInstance();
    return false;
}

void AccessibilityAccountData::GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value)
{
    HILOG_DEBUG();
    AccessibilitySettingProvider& provider = AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);

    ErrCode ret = provider.GetBoolValue(HIGH_TEXT_CONTRAST_ENABLED, value.highContrastText);
    if (ret == ERR_NO_INIT) {
        HILOG_INFO("helper is null, retry.");
        std::this_thread::sleep_for(std::chrono::milliseconds(INIT_DATASHARE_HELPER_SLEEP_TIME));
        provider.GetBoolValue(HIGH_TEXT_CONTRAST_ENABLED, value.highContrastText);
    }
    provider.GetBoolValue(ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, value.invertColor);
    provider.GetBoolValue(ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, value.daltonizationState);
    provider.GetBoolValue(MASTER_MONO, value.audioMono);
    provider.GetBoolValue(ACCESSIBILITY_SCREENREADER_ENABLED, value.isScreenReaderEnabled);
    provider.GetFloatValue(MASTER_BALENCE, value.audioBalance);
    int tmpClickResTime = 0;
    provider.GetIntValue(CLICK_RESPONSE_TIME, tmpClickResTime);
    if (tmpClickResTime == DOUBLE_CLICK_RESPONSE_TIME_MEDIUM) {
        value.clickResponseTime = static_cast<int>(AccessibilityConfig::ResponseDelayMedium);
    } else if (tmpClickResTime > DOUBLE_CLICK_RESPONSE_TIME_MEDIUM) {
        value.clickResponseTime = static_cast<int>(AccessibilityConfig::ResponseDelayLong);
    }
    provider.GetBoolValue(IGNORE_REPEAT_CLICK_SWITCH, value.ignoreRepeatClickState);
    int tmpIgnoreRepeatClickTime = 0;
    provider.GetIntValue(IGNORE_REPEAT_CLICK_TIME, tmpIgnoreRepeatClickTime);
    if (tmpIgnoreRepeatClickTime == DOUBLE_IGNORE_REPEAT_CLICK_TIME_SHORT) {
        value.ignoreRepeatClickTime = static_cast<int>(AccessibilityConfig::RepeatClickTimeoutShort);
    } else if (tmpIgnoreRepeatClickTime == DOUBLE_IGNORE_REPEAT_CLICK_TIME_MEDIUM) {
        value.ignoreRepeatClickTime = static_cast<int>(AccessibilityConfig::RepeatClickTimeoutMedium);
    } else if (tmpIgnoreRepeatClickTime == DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG) {
        value.ignoreRepeatClickTime = static_cast<int>(AccessibilityConfig::RepeatClickTimeoutLong);
    } else if (tmpIgnoreRepeatClickTime > DOUBLE_IGNORE_REPEAT_CLICK_TIME_LONG) {
        value.ignoreRepeatClickTime = static_cast<int>(AccessibilityConfig::RepeatClickTimeoutLongest);
    }
    int tmpDaltonizer = 0;
    provider.GetIntValue(ACCESSIBILITY_DISPLAY_DALTONIZER, tmpDaltonizer);
    if (tmpDaltonizer == DISPLAY_DALTONIZER_GREEN) {
        value.displayDaltonizer = static_cast<int>(AccessibilityConfig::Deuteranomaly);
    } else if (tmpDaltonizer == DISPLAY_DALTONIZER_RED) {
        value.displayDaltonizer = static_cast<int>(AccessibilityConfig::Protanomaly);
    } else if (tmpDaltonizer == DISPLAY_DALTONIZER_BLUE) {
        value.displayDaltonizer= static_cast<int>(AccessibilityConfig::Tritanomaly);
    }
    provider.DeleteInstance();
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

            itr->SetCapabilityValues(resultCapabilities);
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "EnableAbility:" + name);

    enabledAbilities_.push_back(name);
    SetAbilityAutoStartState(name, true);
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
    HILOG_DEBUG("Init start.");
    if (!config_) {
        config_ = std::make_shared<AccessibilitySettingsConfig>(id_);
        config_->Init();
    }
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start.");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> &AccessibilityAccountData::GetConfigCallbacks() const
{
    HILOG_DEBUG("GetConfigCallbacks start.");
    return configCallbacks_;
}

void AccessibilityAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("SetConfigCallbacks start.");
    configCallbacks_ = observer;
}

void AccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("RemoveConfigCallback start.");
    for (auto itr = configCallbacks_.begin(); itr != configCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            configCallbacks_.erase(itr);
            break;
        }
    }
}

std::shared_ptr<AccessibilitySettingsConfig> AccessibilityAccountData::GetConfig()
{
    HILOG_DEBUG("GetConfig start.");
    return config_;
}

void AccessibilityAccountData::GetImportantEnabledAbilities(
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

void AccessibilityAccountData::UpdateAutoStartEnabledAbilities()
{
    HILOG_DEBUG();
    if (id_ == -1) {
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

    uint32_t clickResponseTime = config_->GetClickResponseTime();
    bool ignoreRepeatClickState = config_->GetIgnoreRepeatClickState();
    if (clickResponseTime > 0 || ignoreRepeatClickState == true) {
        flag |= AccessibilityInputInterceptor::FEATURE_SCREEN_TOUCH;
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

        if (connectingA11yAbilities_.GetSizeByUri(Utils::GetUri(bundleName, abilityName))) {
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
    connectedA11yAbilities_.RemoveAccessibilityAbilityByName(bundleName, result);
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

            std::string abilityId = accessibilityInfo->GetId();
            if (GetAbilityAutoStartState(abilityId)) {
                HILOG_DEBUG("auto start packageName is %{public}s.", bundleName.c_str());
                uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                    CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
                uint32_t resultCapabilities = accessibilityInfo->GetStaticCapabilityValues() & capabilities;
                accessibilityInfo->SetCapabilityValues(resultCapabilities);
                AddInstalledAbility(*accessibilityInfo);
                hasNewExtensionAbility = true;
                std::string uri = Utils::GetUri(bundleName, accessibilityInfo->GetName());
                AddEnabledAbility(uri);
                RemoveConnectingA11yAbility(uri);
                continue;
            }

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
    std::vector<std::string> autoStartAbilities;
    for (auto &ability : installedAbilities_) {
        if (ability.GetPackageName() != bundleName) {
            continue;
        }
        if (GetAbilityAutoStartState(ability.GetId())) {
            autoStartAbilities.push_back(ability.GetId());
        }
    }

    RemoveInstalledAbility(bundleName);
    AddAbility(bundleName);

    for (auto &name : autoStartAbilities) {
        auto iter = installedAbilities_.begin();
        for (; iter != installedAbilities_.end(); ++iter) {
            if (name == iter->GetId()) {
                break;
            }
        }
        if (iter == installedAbilities_.end()) {
            SetAbilityAutoStartState(name, false);
        }
    }
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

AccountSA::OsAccountType AccessibilityAccountData::GetAccountType()
{
    if (accountType_ == AccountSA::OsAccountType::END) {
        ErrCode rtn = AccountSA::OsAccountManager::GetOsAccountType(id_, accountType_);
        if (rtn != ERR_OK) {
            HILOG_ERROR("get account type failed for accountId [%{public}d]", id_);
        }
    }
    return accountType_;
}

void AccessibilityAccountData::AccessibilityAbility::AddAccessibilityAbility(const std::string& uri,
    const sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_INFO("uri is %{private}s", uri.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connectionMap_.count(uri)) {
        connectionMap_[uri] = connection;
        HILOG_DEBUG("connectionMap_ size %{public}zu", connectionMap_.size());
        return;
    }
    
    HILOG_DEBUG("uri %{private}s, connectionMap_ %{public}zu", uri.c_str(), connectionMap_.size());
}

void AccessibilityAccountData::AccessibilityAbility::RemoveAccessibilityAbilityByUri(const std::string& uri)
{
    HILOG_INFO("uri is %{private}s", uri.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = connectionMap_.find(uri);
    if (it != connectionMap_.end()) {
        connectionMap_.erase(it);
    }

    HILOG_DEBUG("connectionMap_ %{public}zu", connectionMap_.size());
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilityByName(
    const std::string& elementName)
{
    HILOG_DEBUG("elementName is %{public}s", elementName.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
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

sptr<AccessibleAbilityConnection> AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilityByUri(
    const std::string& uri)
{
    HILOG_DEBUG("uri is %{private}s", uri.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = connectionMap_.find(uri);
    if (iter != connectionMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

void AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilities(
    std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        connectionList.push_back(connection.second);
    }
}

void AccessibilityAccountData::AccessibilityAbility::GetAbilitiesInfo(
    std::vector<AccessibilityAbilityInfo>& abilities)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        if (connection.second) {
            abilities.push_back(connection.second->GetAbilityInfo());
        }
    }

    HILOG_DEBUG("connectionMap_ %{public}zu and enabledAbilities %{public}zu",
        connectionMap_.size(), abilities.size());
}

bool AccessibilityAccountData::AccessibilityAbility::IsExistCapability(Capability capability)
{
    HILOG_DEBUG("capability %{public}d", capability);
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = connectionMap_.begin(); iter != connectionMap_.end(); iter++) {
        if (iter->second->GetAbilityInfo().GetCapabilityValues() & capability) {
            return true;
        }
    }

    return false;
}

void AccessibilityAccountData::AccessibilityAbility::GetAccessibilityAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap)
{
    std::lock_guard<std::mutex> lock(mutex_);
    connectionMap = connectionMap_;
}

void AccessibilityAccountData::AccessibilityAbility::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return connectionMap_.clear();
}

size_t AccessibilityAccountData::AccessibilityAbility::GetSize()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return connectionMap_.size();
}

void AccessibilityAccountData::AccessibilityAbility::GetDisableAbilities(
    std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        for (auto iter = disabledAbilities.begin(); iter != disabledAbilities.end();) {
            if (connection.second && (iter->GetId() == connection.second->GetAbilityInfo().GetId())) {
                disabledAbilities.erase(iter);
            } else {
                iter++;
            }
        }
    }
}

void AccessibilityAccountData::AccessibilityAbility::RemoveAccessibilityAbilityByName(const std::string& bundleName,
    bool& result)
{
    std::lock_guard<std::mutex> lock(mutex_);
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

int32_t AccessibilityAccountData::AccessibilityAbility::GetSizeByUri(const std::string& uri)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return connectionMap_.count(uri);
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::AddAccountData(
    int32_t accountId)
{
    std::lock_guard<std::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        HILOG_WARN("accountId is existed");
        return iter->second;
    }

    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(accountId);
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }

    accountData->Init();
    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::GetCurrentAccountData(
    int32_t accountId)
{
    std::lock_guard<std::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }

    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::GetAccountData(
    int32_t accountId)
{
    std::lock_guard<std::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    HILOG_DEBUG("accountId is not existed");
    return nullptr;
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::RemoveAccountData(
    int32_t accountId)
{
    sptr<AccessibilityAccountData> accountData = nullptr;
    std::lock_guard<std::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        accountData = iter->second;
        accountDataMap_.erase(iter);
    }

    return accountData;
}

void AccessibilityAccountDataMap::Clear()
{
    std::lock_guard<std::mutex> lock(accountDataMutex_);
    accountDataMap_.clear();
}
} // namespace Accessibility
} // namespace OHOS
