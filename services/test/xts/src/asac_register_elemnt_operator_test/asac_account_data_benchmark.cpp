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

#include "a11y_benchmark_account_data.h"

#include <any>
#ifdef OHOS_BUILD_ENABLE_HITRACE
#include <hitrace_meter.h>
#endif // OHOS_BUILD_ENABLE_HITRACE
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "a11y_benchmark_display_manager.h"
#endif
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "utils.h"
#include "system_ability_definition.h"
#include "os_account_manager.h"

namespace OHOS {
namespace A11yBenchmark {
namespace {
    constexpr int32_t AUTOCLICK_DELAY_TIME_MIN = 1000; // ms
    constexpr int32_t AUTOCLICK_DELAY_TIME_MAX = 5000; // ms
    constexpr int32_t INIT_DATASHARE_HELPER_SLEEP_TIME = 500;
    constexpr int DEFAULT_ACCOUNT_ID = 100;
    constexpr int SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    constexpr int SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    constexpr int INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE = 2;
    const std::string HIGH_TEXT_CONTRAST_ENABLED = "high_text_contrast_enabled";
    const std::string ACCESSIBILITY_DISPLAY_INVERSION_ENABLED = "a11y_benchmark_display_inversion_enabled";
    const std::string ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED = "a11y_benchmark_display_daltonizer_enabled";
    const std::string MASTER_MONO = "master_mono";
    const std::string ACCESSIBILITY_SCREENREADER_ENABLED = "a11y_benchmark_screenreader_enabled";
    const std::string MASTER_BALENCE = "master_balance";
    const std::string CLICK_RESPONSE_TIME = "click_response_time";
    const std::string IGNORE_REPEAT_CLICK_SWITCH = "ignore_repeat_click_switch";
    const std::string IGNORE_REPEAT_CLICK_TIME = "ignore_repeat_click_time";
    const std::string ACCESSIBILITY_DISPLAY_DALTONIZER = "a11y_benchmark_display_daltonizer";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/A11yBenchmarkExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string ENABLED_ACCESSIBILITY_SERVICES = "enabled_a11y_benchmark_services";
    const std::string ACCESSIBILITY_SHORTCUT_ENABLED = "a11y_benchmark_shortcut_enabled";
    const std::string ACCESSIBILITY_SHORTCUT_ENABLED_ON_LOCK_SCREEN = "a11y_benchmark_shortcut_enabled_on_lock_screen";
    const std::string ACCESSIBILITY_SHORTCUT_ON_LOCK_SCREEN = "a11y_benchmark_shortcut_on_lock_screen";
    const std::string ACCESSIBILITY_SHORTCUT_TIMEOUT = "a11y_benchmark_shortcut_timeout";
    const std::string SCREEN_MAGNIFICATION_KEY = "a11y_benchmark_display_magnification_enabled";
    const std::string ACCESSIBILITY_CLONE_FLAG = "a11y_benchmark_config_clone";
} // namespace

A11yBenchmarkAccountData::A11yBenchmarkAccountData(int32_t accountId)
{
    id_ = accountId;
}

A11yBenchmarkAccountData::~A11yBenchmarkAccountData()
{}

int32_t A11yBenchmarkAccountData::GetAccountId()
{
    HILOG_INFO();
    return id_;
}

uint32_t A11yBenchmarkAccountData::GetA11yBenchmarkState()
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

void A11yBenchmarkAccountData::OnAccountSwitched()
{
    HILOG_INFO();
    connectingA11yAbilities_.Clear();
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    connectedA11yAbilities_.GetA11yBenchmarkAbilities(connectionList);
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

void A11yBenchmarkAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    std::string uri = Utils::GetUri(connection->GetElementName());
    connectedA11yAbilities_.AddA11yBenchmarkAbility(uri, connection);
}

void A11yBenchmarkAccountData::RemoveConnectedAbility(const AppExecFwk::ElementName &element)
{
    connectedA11yAbilities_.RemoveA11yBenchmarkAbilityByUri(Utils::GetUri(element));
}

void A11yBenchmarkAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(captionPropertyCallbacksMutex_);
    captionPropertyCallbacks_.push_back(callback);
}

void A11yBenchmarkAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(captionPropertyCallbacksMutex_);
    for (auto itr = captionPropertyCallbacks_.begin(); itr != captionPropertyCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            captionPropertyCallbacks_.erase(itr);
            break;
        }
    }
}

void A11yBenchmarkAccountData::AddEnableAbilityListsObserver(
    const sptr<IA11yBenchmarkEnableAbilityListsObserver>& observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    if (std::any_of(enableAbilityListsObservers_.begin(), enableAbilityListsObservers_.end(),
        [observer](const sptr<IA11yBenchmarkEnableAbilityListsObserver> &listObserver) {
            return listObserver == observer;
        })) {
        HILOG_ERROR("observer is already exist");
        return;
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void A11yBenchmarkAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
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

void A11yBenchmarkAccountData::UpdateEnableAbilityListsState()
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnA11yBenchmarkEnableAbilityListsChanged();
        }
    }
}

void A11yBenchmarkAccountData::UpdateInstallAbilityListsState()
{
    std::lock_guard<ffrt::mutex> lock(enableAbilityListObserversMutex_);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnA11yBenchmarkInstallAbilityListsChanged();
        }
    }
}

void A11yBenchmarkAccountData::AddA11yBenchmarkWindowConnection(
    const int32_t windowId, const sptr<A11yBenchmarkWindowConnection>& interactionConnection)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_[windowId] = interactionConnection;
}

void A11yBenchmarkAccountData::RemoveA11yBenchmarkWindowConnection(const int32_t windowId)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    std::map<int32_t, sptr<A11yBenchmarkWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void A11yBenchmarkAccountData::AddConnectingA11yAbility(const std::string &uri,
    const sptr<AccessibleAbilityConnection> &connection)
{
    connectingA11yAbilities_.AddA11yBenchmarkAbility(uri, connection);
}

void A11yBenchmarkAccountData::RemoveConnectingA11yAbility(const std::string &uri)
{
    connectingA11yAbilities_.RemoveA11yBenchmarkAbilityByUri(uri);
}

void A11yBenchmarkAccountData::AddEnabledAbility(const std::string &name)
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
    if (name == screenReaderAbilityName_) {
        SetScreenReaderState(screenReaderKey_, "1");
    }
    UpdateEnableAbilityListsState();
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

RetError A11yBenchmarkAccountData::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("RemoveEnabledAbility start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            if (name == screenReaderAbilityName_) {
                SetScreenReaderState(screenReaderKey_, "0");
            }
            UpdateEnableAbilityListsState();
            HILOG_DEBUG("EnabledAbility size %{public}zu", enabledAbilities_.size());
            return RET_OK;
        }
    }
    HILOG_ERROR("The ability %{public}s is not enabled.", name.c_str());
    return RET_ERR_NOT_ENABLED;
}

void A11yBenchmarkAccountData::AddInstalledAbility(A11yBenchmarkAbilityInfo& abilityInfo)
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

void A11yBenchmarkAccountData::RemoveInstalledAbility(const std::string &bundleName)
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

void A11yBenchmarkAccountData::ClearInstalledAbility()
{
    HILOG_DEBUG("start.");
    installedAbilities_.clear();
}

const sptr<AccessibleAbilityConnection> A11yBenchmarkAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    return connectedA11yAbilities_.GetA11yBenchmarkAbilityByName(elementName);
}

const sptr<A11yBenchmarkWindowConnection> A11yBenchmarkAccountData::GetA11yBenchmarkWindowConnection(
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

const std::map<std::string, sptr<AccessibleAbilityConnection>> A11yBenchmarkAccountData::GetConnectedA11yAbilities()
{
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    connectedA11yAbilities_.GetA11yBenchmarkAbilitiesMap(connectionMap);
    return connectionMap;
}

const std::map<int32_t, sptr<A11yBenchmarkWindowConnection>> A11yBenchmarkAccountData::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start.");
    return asacConnections_;
}

const CaptionPropertyCallbacks A11yBenchmarkAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(captionPropertyCallbacksMutex_);
    CaptionPropertyCallbacks rtnVec = captionPropertyCallbacks_;
    return rtnVec;
}

sptr<AccessibleAbilityConnection> A11yBenchmarkAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    return connectingA11yAbilities_.GetA11yBenchmarkAbilityByUri(uri);
}

const std::vector<std::string> &A11yBenchmarkAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_'s size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName is %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<A11yBenchmarkAbilityInfo> &A11yBenchmarkAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("GetInstalledAbilities start.");
    return installedAbilities_;
}

void A11yBenchmarkAccountData::GetAbilitiesByState(AbilityStateType state,
    std::vector<A11yBenchmarkAbilityInfo> &abilities)
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

void A11yBenchmarkAccountData::GetDisableAbilities(std::vector<A11yBenchmarkAbilityInfo> &disabledAbilities)
{
    HILOG_DEBUG("get disable abilities");
    disabledAbilities = installedAbilities_;
    connectedA11yAbilities_.GetDisableAbilities(disabledAbilities);
}

void A11yBenchmarkAccountData::UpdateAccountCapabilities()
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

void A11yBenchmarkAccountData::UpdateEventTouchGuideCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_TOUCH_GUIDE)) {
        isEventTouchGuideState_ = true;
        return;
    }

    isEventTouchGuideState_ = false;
}

void A11yBenchmarkAccountData::UpdateGesturesSimulationCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_GESTURE)) {
        isGesturesSimulation_ = true;
        return;
    }

    isGesturesSimulation_ = false;
}

void A11yBenchmarkAccountData::UpdateFilteringKeyEventsCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        isFilteringKeyEvents_ = true;
        return;
    }

    isFilteringKeyEvents_ = false;
}

void A11yBenchmarkAccountData::UpdateMagnificationCapability()
{
    if (connectedA11yAbilities_.IsExistCapability(Capability::CAPABILITY_ZOOM)) {
        isScreenMagnification_ = true;
        return;
    }

    isScreenMagnification_ = false;
}

void A11yBenchmarkAccountData::SetScreenReaderExtInAllAccounts(const bool state)
{
    RetError rtn = RET_OK;
    std::vector<int32_t> accountIds = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllAccountIds();
    for (auto accountId : accountIds) {
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
        std::shared_ptr<A11yBenchmarkSettingsConfig> config = accountData->GetConfig();
        if (config == nullptr) {
            HILOG_WARN("config is nullptr, accountId = %{public}d", accountId);
            continue;
        }
        if (state) {
            rtn = config->AddEnabledA11yBenchmarkService(SCREEN_READER_BUNDLE_ABILITY_NAME);
        } else {
            rtn = config->RemoveEnabledA11yBenchmarkService(SCREEN_READER_BUNDLE_ABILITY_NAME);
        }
        HILOG_INFO("set screenReader state = %{public}d, rtn = %{public}d, accountId = %{public}d", state, rtn,
            accountId);
    }
}

void A11yBenchmarkAccountData::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    RetError rtn = RET_OK;
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME) {
        SetScreenReaderState(screenReaderKey_, state ? "1" : "0");
        SetScreenReaderExtInAllAccounts(state);
        return;
    }
    if (!config_) {
        HILOG_WARN("conig_ is nullptr.");
        return;
    }
    if (state) {
        rtn = config_->AddEnabledA11yBenchmarkService(name);
    } else {
        rtn = config_->RemoveEnabledA11yBenchmarkService(name);
    }
    if (rtn != RET_OK) {
        HILOG_WARN("handle service %{public}s state = %{public}d failed, rtn = %{public}d", name.c_str(), state, rtn);
    }
}

void A11yBenchmarkAccountData::SetScreenReaderState(const std::string &name, const std::string &state)
{
    HILOG_DEBUG("set screen reader key [%{public}s], state = [%{public}s].", name.c_str(), state.c_str());
    std::shared_ptr<A11yBenchmarkSettingProvider> service =
        A11yBenchmarkSettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    ErrCode ret = service->PutStringValue(name, state, true);
    if (ret != ERR_OK) {
        HILOG_ERROR("set failed, ret=%{public}d", ret);
    }
}

bool A11yBenchmarkAccountData::GetDefaultUserScreenReaderState()
{
    HILOG_DEBUG();
    std::vector<std::string> services = config_->GetEnabledA11yBenchmarkServices();
    auto iter = std::find(services.begin(), services.end(), SCREEN_READER_BUNDLE_ABILITY_NAME);
    return iter != services.end();
}

void A11yBenchmarkAccountData::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
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

bool A11yBenchmarkAccountData::GetAbilityAutoStartState(const std::string &name)
{
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME && GetAccountType() == AccountSA::OsAccountType::PRIVATE) {
        return GetDefaultUserScreenReaderState();
    }
    if (!config_) {
        HILOG_WARN("config_ is nullptr.");
        return false;
    }
    std::vector<std::string> serviceList = config_->GetEnabledA11yBenchmarkServices();
    auto iter = std::find(serviceList.begin(), serviceList.end(), name);
    if (iter != serviceList.end()) {
        return true;
    } else {
        return false;
    }
}

void A11yBenchmarkAccountData::GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value)
{
    HILOG_DEBUG();
    if (config_ == nullptr) {
        HILOG_ERROR("config_ is nullptr");
        return;
    }

    if (config_->GetDbHandle() == nullptr) {
        HILOG_INFO("helper is null, retry.");
        std::this_thread::sleep_for(std::chrono::milliseconds(INIT_DATASHARE_HELPER_SLEEP_TIME));
        if (config_->GetDbHandle() == nullptr) {
            HILOG_ERROR("helper is null");
            return;
        }
    }

    value.highContrastText = config_->GetDbHandle()->GetBoolValue(HIGH_TEXT_CONTRAST_ENABLED, false);
    value.invertColor = config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, false);
    value.daltonizationState = config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, false);
    value.displayDaltonizer = config_->GetDbHandle()->GetIntValue(ACCESSIBILITY_DISPLAY_DALTONIZER, 0);
    value.shortcutEnabled = config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_SHORTCUT_ENABLED, false);
    value.shortcutEnabledOnLockScreen = config_->GetDbHandle()->GetIntValue(
        ACCESSIBILITY_SHORTCUT_ENABLED_ON_LOCK_SCREEN, INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    value.shortcutOnLockScreen = config_->GetDbHandle()->GetIntValue(
        ACCESSIBILITY_SHORTCUT_ON_LOCK_SCREEN, INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    config_->GetDbHandle()->PutIntValue(ACCESSIBILITY_SHORTCUT_ON_LOCK_SCREEN, INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE);
    value.shortcutTimeout = config_->GetDbHandle()->GetIntValue(ACCESSIBILITY_SHORTCUT_TIMEOUT,
        SHORT_KEY_TIMEOUT_BEFORE_USE);
    value.clickResponseTime = config_->GetDbHandle()->GetIntValue(CLICK_RESPONSE_TIME, 0);
    value.ignoreRepeatClickState = config_->GetDbHandle()->GetBoolValue(IGNORE_REPEAT_CLICK_SWITCH, false);
    value.ignoreRepeatClickTime = config_->GetDbHandle()->GetIntValue(IGNORE_REPEAT_CLICK_TIME, 0);
    value.screenMagnificationState = config_->GetDbHandle()->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false);

    // In Aos, the audio configuration is stored in SYSTEM and it should be copied to SECURE
    config_->CloneAudioState();
    value.audioMono = config_->GetDbHandle()->GetBoolValue(MASTER_MONO, false);
    value.audioBalance = config_->GetDbHandle()->GetFloatValue(MASTER_BALENCE, 0);

    std::shared_ptr<A11yBenchmarkSettingProvider> service =
        A11yBenchmarkSettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    service->GetBoolValue(ACCESSIBILITY_SCREENREADER_ENABLED, value.isScreenReaderEnabled);
    service->DeleteInstance();
}

RetError A11yBenchmarkAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
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
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "EnableAbility:" + name);
#endif // OHOS_BUILD_ENABLE_HITRACE

    enabledAbilities_.push_back(name);
    SetAbilityAutoStartState(name, true);
    if (name == screenReaderAbilityName_) {
        SetScreenReaderState(screenReaderKey_, "1");
    }
    UpdateAbilities();
    Utils::RecordStartingA11yEvent(name);
    return RET_OK;
}

bool A11yBenchmarkAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryInstalledAbilityInfo");
#endif // OHOS_BUILD_ENABLE_HITRACE
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
        A11yBenchmarkAbilityInitParams initParams;
        Utils::Parse(info, initParams);
        std::shared_ptr<A11yBenchmarkAbilityInfo> a11y_benchmarkInfo =
            std::make_shared<A11yBenchmarkAbilityInfo>(initParams);
        AddInstalledAbility(*a11y_benchmarkInfo);
    }
    return true;
}

void A11yBenchmarkAccountData::Init()
{
    HILOG_DEBUG("Init start.");
    if (!config_) {
        config_ = std::make_shared<A11yBenchmarkSettingsConfig>(id_);
        config_->Init();
    }
    ErrCode rtn = AccountSA::OsAccountManager::GetOsAccountType(id_, accountType_);
    if (rtn != ERR_OK) {
        HILOG_ERROR("get account type failed for accountId [%{public}d]", id_);
    }
    std::shared_ptr<A11yBenchmarkSettingProvider> service =
        A11yBenchmarkSettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    bool cloneState = false;
    service->GetBoolValue(ACCESSIBILITY_CLONE_FLAG, cloneState);
    if (cloneState == true) {
        service->PutBoolValue(ACCESSIBILITY_CLONE_FLAG, false);
    }
    if (id_ != DEFAULT_ACCOUNT_ID) {
        HILOG_WARN("id != default_account_id.");
        return;
    }

    HILOG_INFO("register clone observer.");
    A11yBenchmarkSettingObserver::UpdateFunc func = [ = ](const std::string& state) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnDataClone();
    };
    RetError ret = service->RegisterObserver(ACCESSIBILITY_CLONE_FLAG, func);
    if (ret != RET_OK) {
        HILOG_WARN("register clone observer failed %{public}d.", ret);
    }
}

void A11yBenchmarkAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> A11yBenchmarkAccountData::GetConfigCallbacks()
{
    HILOG_DEBUG("GetConfigCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> rtnVec = configCallbacks_;
    return rtnVec;
}

void A11yBenchmarkAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("SetConfigCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    configCallbacks_ = observer;
}

void A11yBenchmarkAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("RemoveConfigCallback start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    for (auto itr = configCallbacks_.begin(); itr != configCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            configCallbacks_.erase(itr);
            break;
        }
    }
}

std::shared_ptr<A11yBenchmarkSettingsConfig> A11yBenchmarkAccountData::GetConfig()
{
    HILOG_DEBUG("GetConfig start.");
    return config_;
}

void A11yBenchmarkAccountData::GetImportantEnabledAbilities(
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

void A11yBenchmarkAccountData::UpdateImportantEnabledAbilities(
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

void A11yBenchmarkAccountData::UpdateAutoStartEnabledAbilities()
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

uint32_t A11yBenchmarkAccountData::GetInputFilterFlag() const
{
    HILOG_DEBUG();
    if (!config_) {
        HILOG_ERROR("config is null");
        return 0;
    }
    uint32_t flag = 0;
    if (config_->GetScreenMagnificationState()) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    }
    if (isEventTouchGuideState_) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_TOUCH_EXPLORATION;
    }
    if (isFilteringKeyEvents_) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    }
    if (isGesturesSimulation_) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    }
    if (config_->GetMouseKeyState()) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_MOUSE_KEY;
    }

    int32_t autoclickTime = config_->GetMouseAutoClick();
    if (autoclickTime >= AUTOCLICK_DELAY_TIME_MIN && autoclickTime <= AUTOCLICK_DELAY_TIME_MAX) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_MOUSE_AUTOCLICK;
    }

    uint32_t clickResponseTime = config_->GetClickResponseTime();
    bool ignoreRepeatClickState = config_->GetIgnoreRepeatClickState();
    if (clickResponseTime > 0 || ignoreRepeatClickState == true) {
        flag |= A11yBenchmarkInputInterceptor::FEATURE_SCREEN_TOUCH;
    }

    return flag;
}

void A11yBenchmarkAccountData::UpdateAbilities()
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
            if (connection != nullptr && connection->Connect(element)) {
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

bool A11yBenchmarkAccountData::RemoveAbility(const std::string &bundleName)
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
    connectedA11yAbilities_.RemoveA11yBenchmarkAbilityByName(bundleName, result);
    if (result) {
        UpdateAbilities();
    }

    return result;
}

void A11yBenchmarkAccountData::AddAbility(const std::string &bundleName)
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
            A11yBenchmarkAbilityInitParams initParams;
            Utils::Parse(newAbility, initParams);
            std::shared_ptr<A11yBenchmarkAbilityInfo> a11y_benchmarkInfo =
                std::make_shared<A11yBenchmarkAbilityInfo>(initParams);

            std::string abilityId = a11y_benchmarkInfo->GetId();
            if (GetAbilityAutoStartState(abilityId)) {
                HILOG_DEBUG("auto start packageName is %{public}s.", bundleName.c_str());
                uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
                    CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
                uint32_t resultCapabilities = a11y_benchmarkInfo->GetStaticCapabilityValues() & capabilities;
                a11y_benchmarkInfo->SetCapabilityValues(resultCapabilities);
                AddInstalledAbility(*a11y_benchmarkInfo);
                hasNewExtensionAbility = true;
                std::string uri = Utils::GetUri(bundleName, a11y_benchmarkInfo->GetName());
                AddEnabledAbility(uri);
                RemoveConnectingA11yAbility(uri);
                continue;
            }

            AddInstalledAbility(*a11y_benchmarkInfo);
            hasNewExtensionAbility = true;
        }
    }

    if (hasNewExtensionAbility) {
        HILOG_DEBUG("add new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void A11yBenchmarkAccountData::ChangeAbility(const std::string &bundleName)
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

void A11yBenchmarkAccountData::AddUITestClient(const sptr<IRemoteObject> &obj,
    const std::string &bundleName, const std::string &abilityName)
{
    HILOG_DEBUG();
    // Add installed ability
    std::shared_ptr<A11yBenchmarkAbilityInfo> abilityInfo = std::make_shared<A11yBenchmarkAbilityInfo>();
    abilityInfo->SetPackageName(bundleName);
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetA11yBenchmarkAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
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

void A11yBenchmarkAccountData::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection,
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

AccountSA::OsAccountType A11yBenchmarkAccountData::GetAccountType()
{
    return accountType_;
}

void A11yBenchmarkAccountData::A11yBenchmarkAbility::AddA11yBenchmarkAbility(const std::string& uri,
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

void A11yBenchmarkAccountData::A11yBenchmarkAbility::RemoveA11yBenchmarkAbilityByUri(const std::string& uri)
{
    HILOG_INFO("uri is %{private}s", uri.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto it = connectionMap_.find(uri);
    if (it != connectionMap_.end()) {
        connectionMap_.erase(it);
    }

    HILOG_DEBUG("connectionMap_ %{public}zu", connectionMap_.size());
}

sptr<AccessibleAbilityConnection> A11yBenchmarkAccountData::A11yBenchmarkAbility::GetA11yBenchmarkAbilityByName(
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

sptr<AccessibleAbilityConnection> A11yBenchmarkAccountData::A11yBenchmarkAbility::GetA11yBenchmarkAbilityByUri(
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

void A11yBenchmarkAccountData::A11yBenchmarkAbility::GetA11yBenchmarkAbilities(
    std::vector<sptr<AccessibleAbilityConnection>>& connectionList)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto& connection : connectionMap_) {
        connectionList.push_back(connection.second);
    }
}

void A11yBenchmarkAccountData::A11yBenchmarkAbility::GetAbilitiesInfo(
    std::vector<A11yBenchmarkAbilityInfo>& abilities)
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

bool A11yBenchmarkAccountData::A11yBenchmarkAbility::IsExistCapability(Capability capability)
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

void A11yBenchmarkAccountData::A11yBenchmarkAbility::GetA11yBenchmarkAbilitiesMap(
    std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    connectionMap = connectionMap_;
}

void A11yBenchmarkAccountData::A11yBenchmarkAbility::Clear()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.clear();
}

size_t A11yBenchmarkAccountData::A11yBenchmarkAbility::GetSize()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.size();
}

void A11yBenchmarkAccountData::A11yBenchmarkAbility::GetDisableAbilities(
    std::vector<A11yBenchmarkAbilityInfo> &disabledAbilities)
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

void A11yBenchmarkAccountData::A11yBenchmarkAbility::RemoveA11yBenchmarkAbilityByName(const std::string& bundleName,
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

int32_t A11yBenchmarkAccountData::A11yBenchmarkAbility::GetSizeByUri(const std::string& uri)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return connectionMap_.count(uri);
}

sptr<A11yBenchmarkAccountData> A11yBenchmarkAccountDataMap::AddAccountData(
    int32_t accountId)
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        HILOG_WARN("accountId is existed");
        return iter->second;
    }

    sptr<A11yBenchmarkAccountData> accountData = new(std::nothrow) A11yBenchmarkAccountData(accountId);
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }

    accountData->Init();
    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<A11yBenchmarkAccountData> A11yBenchmarkAccountDataMap::GetCurrentAccountData(
    int32_t accountId)
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    sptr<A11yBenchmarkAccountData> accountData = new(std::nothrow) A11yBenchmarkAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }

    accountDataMap_[accountId] = accountData;
    return accountData;
}

sptr<A11yBenchmarkAccountData> A11yBenchmarkAccountDataMap::GetAccountData(
    int32_t accountId)
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        return iter->second;
    }

    HILOG_DEBUG("accountId is not existed");
    return nullptr;
}

sptr<A11yBenchmarkAccountData> A11yBenchmarkAccountDataMap::RemoveAccountData(
    int32_t accountId)
{
    sptr<A11yBenchmarkAccountData> accountData = nullptr;
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    auto iter = accountDataMap_.find(accountId);
    if (iter != accountDataMap_.end()) {
        accountData = iter->second;
        accountDataMap_.erase(iter);
    }

    return accountData;
}

std::vector<int32_t> A11yBenchmarkAccountDataMap::GetAllAccountIds()
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    std::vector<int32_t> accountIds;
    for (auto it = accountDataMap_.begin(); it != accountDataMap_.end(); it++) {
        accountIds.push_back(it->first);
    }
    return accountIds;
}

void A11yBenchmarkAccountDataMap::Clear()
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    accountDataMap_.clear();
}
} // namespace A11yBenchmark
} // namespace OHOS
