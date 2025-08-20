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
#include <dlfcn.h>
#ifdef OHOS_BUILD_ENABLE_HITRACE
#include <hitrace_meter.h>
#endif // OHOS_BUILD_ENABLE_HITRACE
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "utils.h"
#include "system_ability_definition.h"
#include "os_account_manager.h"
#include "parcel/accessibility_event_info_parcel.h"
#include "ipc_skeleton.h"
#include "accessibility_resource_bundle_manager.h"
#include "accessibility_notification_helper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t AUTOCLICK_DELAY_TIME_MIN = 1000; // ms
    constexpr int32_t AUTOCLICK_DELAY_TIME_MAX = 5000; // ms
    constexpr int32_t INIT_DATASHARE_HELPER_SLEEP_TIME = 500;
    constexpr int DEFAULT_ACCOUNT_ID = 100;
    constexpr int SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    constexpr int SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    constexpr int INVALID_SHORTCUT_ON_LOCK_SCREEN_STATE = 2;
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
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string ENABLED_ACCESSIBILITY_SERVICES = "enabled_accessibility_services";
    const std::string ACCESSIBILITY_SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
    const std::string ACCESSIBILITY_SHORTCUT_ENABLED_ON_LOCK_SCREEN = "accessibility_shortcut_enabled_on_lock_screen";
    const std::string ACCESSIBILITY_SHORTCUT_ON_LOCK_SCREEN = "accessibility_shortcut_on_lock_screen";
    const std::string ACCESSIBILITY_SHORTCUT_TIMEOUT = "accessibility_shortcut_timeout";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const std::string ACCESSIBILITY_TOUCH_GUIDE_ENABLED = "enableTouchGuideMode";
    const std::string SCREEN_READER_SINGLE_CLICK_MODE = "screen_reader_single_click_mode";
    const std::string ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE = "accessibility_privacy_clone_or_upgrade";
    const std::string THP_PATH = "/system/lib64/libthp_extra_innerapi.z.so";
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
    HILOG_INFO();
    return id_;
}

uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_DEBUG();
    uint32_t state = 0;

    auto abilityManager = GetAbilityManager();
    bool screenReaderState = false;
    if (abilityManager) {
        auto connectedAbilities = abilityManager->GetConnectedA11yAbilities();
        size_t connectingCount = abilityManager->GetConnectingAbilitiesCount();
        screenReaderState = abilityManager->GetScreenReaderState();

        if (!connectedAbilities.empty() || connectingCount > 0) {
            HILOG_DEBUG("Accessibility is enabled");
            state |= STATE_ACCESSIBILITY_ENABLED;
            if (!config_->GetEnabledState()) {
                config_->SetEnabled(true);
            }
        } else {
            if (config_->GetEnabledState()) {
                config_->SetEnabled(false);
            }
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

    if (screenReaderState) {
        state |= STATE_SCREENREADER_ENABLED;
    }
    if (isSingleClickMode_) {
        state |= STATE_SINGLE_CLICK_MODE_ENABLED;
    }
    return state;
}

void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_INFO();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->OnAccountSwitched();
    }
}

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
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(captionPropertyCallbacksMutex_);
    captionPropertyCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
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

void AccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    HILOG_DEBUG();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddEnableAbilityListsObserver(observer);
    }
}

void AccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    HILOG_INFO();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveEnableAbilityListsObserver(observer);
    }
}

void AccessibilityAccountData::UpdateEnableAbilityListsState()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->UpdateEnableAbilityListsState();
    }
}

void AccessibilityAccountData::UpdateInstallAbilityListsState()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->UpdateInstallAbilityListsState();
    }
}

void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    operatorManager_.AddAccessibilityWindowConnection(windowId, interactionConnection);
}

void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
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

void AccessibilityAccountData::AddEnabledAbility(const std::string &name)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddEnabledAbility(name);
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

void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddInstalledAbility(abilityInfo);
    }
}

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveInstalledAbility(bundleName);
    }
}

void AccessibilityAccountData::ClearInstalledAbility()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->ClearInstalledAbility();
    }
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetAccessibleAbilityConnection(elementName);
    }
    return nullptr;
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
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

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetWaitDisConnectAbility(const std::string &uri)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetWaitDisConnectAbility(uri);
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

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start.");
    return operatorManager_.GetAsacConnections();
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("GetCaptionPropertyCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(captionPropertyCallbacksMutex_);
    CaptionPropertyCallbacks rtnVec = captionPropertyCallbacks_;
    return rtnVec;
}

sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetConnectingA11yAbility(const std::string &uri)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetConnectingA11yAbility(uri);
    }
    return nullptr;
}

const std::vector<std::string> &AccessibilityAccountData::GetEnabledAbilities()
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

void AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state,
    std::vector<AccessibilityAbilityInfo> &abilities)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->GetAbilitiesByState(state, abilities);
    }
}

void AccessibilityAccountData::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->GetDisableAbilities(disabledAbilities);
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
    auto touchGuideState = true;
    if (config_->GetDbHandle() == nullptr) {
        HILOG_WARN("helper is null!");
    } else {
        touchGuideState = config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_TOUCH_GUIDE_ENABLED, true);
    }
    
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        if (abilityManager->IsExistCapability(Capability::CAPABILITY_TOUCH_GUIDE) && touchGuideState) {
            isEventTouchGuideState_ = true;
            return;
        }
    }

    isEventTouchGuideState_ = false;
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        if (abilityManager->IsExistCapability(Capability::CAPABILITY_GESTURE)) {
            isGesturesSimulation_ = true;
            return;
        }
    }

    isGesturesSimulation_ = false;
}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        if (abilityManager->IsExistCapability(Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
            isFilteringKeyEvents_ = true;
            return;
        }
    }

    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        if (abilityManager->IsExistCapability(Capability::CAPABILITY_ZOOM)) {
            isScreenMagnification_ = true;
            return;
        }
    }

    isScreenMagnification_ = false;
}

void AccessibilityAccountData::SetScreenReaderExtInAllAccounts(const bool state)
{
    RetError rtn = RET_OK;
    std::vector<int32_t> accountIds = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllAccountIds();
    for (auto accountId : accountIds) {
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
        std::shared_ptr<AccessibilitySettingsConfig> config = accountData->GetConfig();
        if (config == nullptr) {
            HILOG_WARN("config is nullptr, accountId = %{public}d", accountId);
            continue;
        }
        if (state) {
            rtn = config->AddEnabledAccessibilityService(SCREEN_READER_BUNDLE_ABILITY_NAME);
        } else {
            rtn = config->RemoveEnabledAccessibilityService(SCREEN_READER_BUNDLE_ABILITY_NAME);
        }
        HILOG_INFO("set screenReader state = %{public}d, rtn = %{public}d, accountId = %{public}d", state, rtn,
            accountId);
    }
}

void AccessibilityAccountData::SetAbilityAutoStartState(const std::string &name, const bool state)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->SetAbilityAutoStartState(name, state);
    }
}

void AccessibilityAccountData::SetAccessibilityStateToTP(bool state)
{
    HILOG_INFO("set accessibility state to TP, state = %{public}d", state);

    void *handle = nullptr;
    handle = dlopen(THP_PATH.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        HILOG_ERROR("thp handle is null!");
        return;
    }

    typedef const char* (*ThpExtraRunCommandFunc)(const char* command, const char* parameters);
    const char* (*ThpExtraRunCommand)(const char* command, const char* parameters) {};
    ThpExtraRunCommand = reinterpret_cast<ThpExtraRunCommandFunc>(dlsym(handle, "ThpExtraRunCommand"));
    if (ThpExtraRunCommand == nullptr) {
        HILOG_ERROR("ThpExtraRunCommand is null");
        dlclose(handle);
        return;
    }

    std::string stateValue = state ? "1" : "0";
    const std::string param = std::string("THP#").append(stateValue);
    ThpExtraRunCommand("THP_SetAccessibilityMode", param.c_str());
    dlclose(handle);
}

void AccessibilityAccountData::SetScreenReaderState(const std::string &name, const std::string &state)
{
    HILOG_DEBUG("set screen reader key [%{public}s], state = [%{public}s].", name.c_str(), state.c_str());
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->TryToSetScreenReaderState(name, "1" == state);
    }
}

void AccessibilityAccountData::SetScreenReaderState(const bool state)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->SetScreenReaderState(state);
    }
}

bool AccessibilityAccountData::GetScreenReaderState()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetScreenReaderState();
    }
    return false;
}

bool AccessibilityAccountData::GetDefaultUserScreenReaderState()
{
    HILOG_DEBUG();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetDefaultUserScreenReaderState();
    }
    return false;
}

void AccessibilityAccountData::DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName)
{
    HILOG_DEBUG();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->DelAutoStartPrefKeyInRemovePkg(bundleName);
    }
}

bool AccessibilityAccountData::GetAbilityAutoStartState(const std::string &name)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetAbilityAutoStartState(name);
    }
    return false;
}

void AccessibilityAccountData::GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value)
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
    value.shortcutEnabled = config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_SHORTCUT_ENABLED, true);
    value.shortcutEnabledOnLockScreen = config_->GetDbHandle()->GetBoolValue(
        ACCESSIBILITY_SHORTCUT_ENABLED_ON_LOCK_SCREEN, false);
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

    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    service->PutBoolValue(ACCESSIBILITY_PRIVACY_CLONE_OR_UPGRADE, true);
    service->GetBoolValue(ACCESSIBILITY_SCREENREADER_ENABLED, value.isScreenReaderEnabled);
    service->DeleteInstance();
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
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetInstalledAbilitiesFromBMS();
    }
    return true;
}

void AccessibilityAccountData::InitScreenReaderStateObserver()
{
    if (!config_) {
        HILOG_ERROR("config is nullptr!");
    }
    if (!config_->GetDbHandle()) {
        HILOG_ERROR("helper is null!");
        return;
    }

    AccessibilitySettingObserver::UpdateFunc callback = [this](const std::string& state) {
        OnTouchGuideStateChanged();
    };
    RetError ret = config_->GetDbHandle()->RegisterObserver(ACCESSIBILITY_TOUCH_GUIDE_ENABLED, callback);
    if (ret != RET_OK) {
        HILOG_ERROR("register touch guide observer failed, ret = %{public}d", ret);
    }

    isSingleClickMode_ = config_->GetDbHandle()->GetBoolValue(SCREEN_READER_SINGLE_CLICK_MODE, false, true);
    AccessibilitySettingObserver::UpdateFunc func = [this](const std::string& state) {
        OnSingleClickModeChanged();
    };
    ret = config_->GetDbHandle()->RegisterObserver(SCREEN_READER_SINGLE_CLICK_MODE, func);
    if (ret != RET_OK) {
        HILOG_ERROR("register touch mode observer failed, ret = %{public}d", ret);
    }
}

void AccessibilityAccountData::Init()
{
    HILOG_DEBUG("Init start.");
    if (!config_) {
        config_ = std::make_shared<AccessibilitySettingsConfig>(id_);
        config_->Init();
    } else {
        config_->InitSetting();
    }
    ErrCode rtn = AccountSA::OsAccountManager::GetOsAccountType(id_, accountType_);
    if (rtn != ERR_OK) {
        HILOG_ERROR("get account type failed for accountId [%{public}d]", id_);
    }

    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->SetConfig(config_);
    }

    InitScreenReaderStateObserver();

    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
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
    AccessibilitySettingObserver::UpdateFunc func = [ = ](const std::string& state) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnDataClone();
    };
    RetError ret = service->RegisterObserver(ACCESSIBILITY_CLONE_FLAG, func);
    if (ret != RET_OK) {
        HILOG_WARN("register clone observer failed %{public}d.", ret);
    }
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("AddConfigCallback start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> AccessibilityAccountData::GetConfigCallbacks()
{
    HILOG_DEBUG("GetConfigCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> rtnVec = configCallbacks_;
    return rtnVec;
}

void AccessibilityAccountData::SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer)
{
    HILOG_DEBUG("SetConfigCallbacks start.");
    std::lock_guard<ffrt::mutex> lock(configCallbacksMutex_);
    configCallbacks_ = observer;
}

void AccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
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

std::shared_ptr<AccessibilitySettingsConfig> AccessibilityAccountData::GetConfig()
{
    HILOG_DEBUG("GetConfig start.");
    return config_;
}

void AccessibilityAccountData::GetImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities) const
{
    HILOG_DEBUG("GetImportantEnabledAbilities start.");
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->GetImportantEnabledAbilities(importantEnabledAbilities);
    }
}

void AccessibilityAccountData::UpdateImportantEnabledAbilities(
    std::map<std::string, uint32_t> &importantEnabledAbilities)
{
    HILOG_DEBUG();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->UpdateImportantEnabledAbilities(importantEnabledAbilities);
    }
}

void AccessibilityAccountData::UpdateAutoStartEnabledAbilities()
{
    HILOG_DEBUG();
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->UpdateAutoStartEnabledAbilities();
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
    if (config_->GetScreenMagnificationState()) {
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
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->UpdateAbilities();
    }
}

bool AccessibilityAccountData::RemoveAbility(const std::string &bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->RemoveAbility(bundleName);
    }
    return false;
}

void AccessibilityAccountData::AddAbility(const std::string &bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddAbility(bundleName);
    }
}

void AccessibilityAccountData::ChangeAbility(const std::string &bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->ChangeAbility(bundleName);
    }
}

void AccessibilityAccountData::AddUITestClient(const sptr<IRemoteObject> &obj,
    const std::string &bundleName, const std::string &abilityName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddUITestClient(obj, bundleName, abilityName);
    }
}

void AccessibilityAccountData::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection,
    const std::string &bundleName)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveUITestClient(connection, bundleName);
    }
}

AccountSA::OsAccountType AccessibilityAccountData::GetAccountType()
{
    return accountType_;
}

void AccessibilityAccountData::OnTouchGuideStateChanged()
{
    HILOG_INFO();

    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    if (config_->GetDbHandle() == nullptr) {
        HILOG_ERROR("helper is nullptr!");
        return;
    }
    if (!config_->GetDbHandle()->GetBoolValue(ACCESSIBILITY_TOUCH_GUIDE_ENABLED, true)) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(
            ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    }
}

void AccessibilityAccountData::OnSingleClickModeChanged()
{
    if (!config_) {
        HILOG_ERROR("config is nullptr!");
        return;
    }
    if (!config_->GetDbHandle()) {
        HILOG_ERROR("helper is nullptr!");
        return;
    }

    isSingleClickMode_ = config_->GetDbHandle()->GetBoolValue(SCREEN_READER_SINGLE_CLICK_MODE, false, true);
    HILOG_INFO("screen reader single click mode = %{public}d", isSingleClickMode_);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityState();
}

sptr<AccessibilityAccountData> AccessibilityAccountDataMap::AddAccountData(
    int32_t accountId)
{
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
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
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
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
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
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
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
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
    std::lock_guard<ffrt::mutex> lock(accountDataMutex_);
    accountDataMap_.clear();
}

void AccessibilityAccountData::AddNeedEvent(std::string &name, std::vector<uint32_t> needEvents)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->AddNeedEvent(name, needEvents);
    }
}

void AccessibilityAccountData::RemoveNeedEvent(const std::string &name)
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        abilityManager->RemoveNeedEvent(name);
    }
}

std::vector<uint32_t> AccessibilityAccountData::UpdateNeedEvents()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->UpdateNeedEvents();
    }
    return {};
}

std::vector<uint32_t> AccessibilityAccountData::GetNeedEvents()
{
    auto abilityManager = GetAbilityManager();
    if (abilityManager) {
        return abilityManager->GetNeedEvents();
    }
    return {};
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

ErrCode AccessibilityAccountData::RegisterElementOperatorByWindowId(const uint32_t tokenId, const int32_t windowId,
    const sptr<IAccessibilityElementOperator>& elementOperator, bool isApp)
{
    HILOG_INFO("Register windowId[%{public}d], tokenId[%{public}u]", windowId, tokenId);

    sptr<AccessibilityWindowConnection> oldConnection = operatorManager_.GetAccessibilityWindowConnection(windowId);
    if (isApp && oldConnection) {
        HILOG_WARN("no need to register again.");
        return RET_OK;
    }

    if (oldConnection) {
        operatorManager_.DeleteConnectionAndDeathRecipient(windowId, oldConnection);
    }

    sptr<AccessibilityWindowConnection> connection =
        new(std::nothrow) AccessibilityWindowConnection(windowId, elementOperator, id_);
    if (!connection) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
        return RET_ERR_NULLPTR;
    }

    connection->SetTokenIdMap(0, tokenId);
    operatorManager_.AddAccessibilityWindowConnection(windowId, connection);
    IsCheckWindowIdEventExist(windowId);
    if (elementOperator && elementOperator->AsObject()) {
        SetElementOperatorDeathRecipient(windowId, 0, elementOperator);
    }
    return RET_OK;
}

ErrCode AccessibilityAccountData::RegisterElementOperatorByParameter(const RegistrationPara& parameter,
    const sptr<IAccessibilityElementOperator>& elementOperator, const int32_t treeId,
    const int64_t nodeId, const uint32_t tokenId, bool isApp)
{
    HILOG_INFO("Register windowId[%{public}d]", parameter.windowId);

#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "RegisterElementOperator");
#endif // OHOS_BUILD_ENABLE_HITRACE

    if (RET_OK != RegisterElementOperatorChildWork(parameter, treeId, nodeId, elementOperator,
        tokenId, isApp)) {
        return RET_ERR_FAILED;
    }

    IsCheckWindowIdEventExist(parameter.windowId);
    if (elementOperator && elementOperator->AsObject()) {
        SetElementOperatorDeathRecipient(parameter.windowId, treeId, elementOperator);
    }

    return RET_OK;
}

RetError AccessibilityAccountData::RegisterElementOperatorChildWork(const RegistrationPara &parameter,
    const int32_t treeId, const int64_t nodeId, const sptr<IAccessibilityElementOperator> &operation,
    const uint32_t tokenId, bool isApp)
{
    sptr<AccessibilityWindowConnection> parentConnection =
        operatorManager_.GetAccessibilityWindowConnection(parameter.parentWindowId);
    if (isApp && parentConnection) {
        sptr<IAccessibilityElementOperator> parentAamsOper =
            parentConnection->GetCardProxy(parameter.parentTreeId);
        if (parentAamsOper != nullptr) {
            parentAamsOper->SetChildTreeIdAndWinId(nodeId, treeId, parameter.windowId);
        } else {
            HILOG_DEBUG("parentAamsOper is nullptr");
        }

        auto cardOper = parentConnection->GetCardProxy(treeId);
        if (cardOper == nullptr) {
            parentConnection->SetCardProxy(treeId, operation);
            parentConnection->SetTokenIdMap(treeId, tokenId);
            parentConnection->SetRootParentId(treeId, nodeId);
        }
    } else {
        return RET_ERR_NO_CONNECTION;
    }

    operation->SetBelongTreeId(treeId);
    operation->SetParentWindowId(parameter.parentWindowId);
    sptr<AccessibilityWindowConnection> oldConnection =
        operatorManager_.GetAccessibilityWindowConnection(parameter.windowId);
    if (isApp && oldConnection) {
        if (oldConnection->GetCardProxy(treeId) != nullptr) {
            HILOG_WARN("no need to register again.");
            return RET_ERR_REGISTER_EXIST;
        } else {
            oldConnection->SetCardProxy(treeId, operation);
            oldConnection->SetTokenIdMap(treeId, tokenId);
            oldConnection->SetRootParentId(treeId, nodeId);
        }
    }
    return RET_OK;
}

void AccessibilityAccountData::SetElementOperatorDeathRecipient(const int32_t windowId, const int32_t treeId,
    const sptr<IAccessibilityElementOperator> &elementOperator)
{
    if (!elementOperator || !elementOperator->AsObject()) {
        HILOG_WARN("elementOperator or AsObject is null");
        return;
    }

    if (treeId == 0) {
        operatorManager_.SetElementOperatorDeathRecipient(windowId, elementOperator->AsObject(), id_);
    } else {
        operatorManager_.SetElementOperatorDeathRecipient(windowId, treeId, elementOperator->AsObject(), id_);
    }
}

int32_t AccessibilityAccountData::ApplyTreeId()
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    int32_t curTreeId = preTreeId_ + 1;
    for (int32_t index = 0; index < ACCOUNT_TREE_ID_MAX; index++) {
        if (curTreeId == ACCOUNT_TREE_ID_MAX) {
            curTreeId = 0;
        }
        if (!treeIdPool_.test(curTreeId)) {
            treeIdPool_.set(curTreeId, true);
            preTreeId_ = curTreeId;
            return curTreeId + 1;
        }
        curTreeId++;
    }
    preTreeId_ = ACCOUNT_TREE_ID_MAX - 1;
    return 0;
}

void AccessibilityAccountData::RecycleTreeId(int32_t treeId)
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    if ((treeId > 0) && (treeId <= ACCOUNT_TREE_ID_MAX)) {
        treeIdPool_.set(treeId - 1, false);
    }
}

void AccessibilityAccountData::InsertWindowIdEventPair(int32_t windowId, const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("insert event, windowId: %{public}d", windowId);
    windowFocusEventMap_.EnsureInsert(windowId, event);
}

bool AccessibilityAccountData::CheckWindowIdEventExist(int32_t windowId)
{
    AccessibilityEventInfo eventInfo;
    return windowFocusEventMap_.Find(windowId, eventInfo);
}

AccessibilityEventInfoParcel AccessibilityAccountData::GetWindowFocusEvent(int32_t windowId)
{
    return AccessibilityEventInfoParcel(windowFocusEventMap_.ReadVal(windowId));
}

void AccessibilityAccountData::EraseWindowFocusEvent(int32_t windowId)
{
    windowFocusEventMap_.Erase(windowId);
}

void AccessibilityAccountData::IsCheckWindowIdEventExist(const int32_t windowId)
{
    if (CheckWindowIdEventExist(windowId)) {
        AccessibilityEventInfoParcel eventInfoParcel = GetWindowFocusEvent(windowId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfoParcel, 0);
        EraseWindowFocusEvent(windowId);
    }
}

ErrCode AccessibilityAccountData::DeregisterElementOperatorByWindowId(const int32_t windowId)
{
    HILOG_INFO("Deregister windowId[%{public}d]", windowId);
    sptr<AccessibilityWindowConnection> connection = operatorManager_.GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
        return RET_OK;
    }
    operatorManager_.StopCallbackWait(windowId);

    if (!connection->GetProxy()) {
        HILOG_ERROR("proxy is null");
        return RET_ERR_NULLPTR;
    }

    auto object = connection->GetProxy()->AsObject();
    if (object) {
        operatorManager_.RemoveElementOperatorDeathRecipient(windowId, object);
    }

    connection->SetProxy(nullptr);
    RemoveTreeDeathRecipient(windowId, 0, connection);

    std::vector<int32_t> treeIds {};
    connection->GetAllTreeId(treeIds);
    for (int32_t treeId : treeIds) {
        RecycleTreeId(treeId);
        operatorManager_.StopCallbackWait(windowId, treeId);
        RemoveTreeDeathRecipient(windowId, treeId, connection);
    }
    operatorManager_.RemoveAccessibilityWindowConnection(windowId);
    return RET_OK;
}

void AccessibilityAccountData::RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
    const sptr<AccessibilityWindowConnection> connection)
{
    operatorManager_.RemoveTreeDeathRecipient(windowId, treeId, connection);
}

ErrCode AccessibilityAccountData::DeregisterElementOperatorByWindowIdAndTreeId(const int32_t windowId,
    const int32_t treeId)
{
    HILOG_INFO("Deregister windowId[%{public}d], treeId[%{public}d]", windowId, treeId);
    RecycleTreeId(treeId);
    sptr<AccessibilityWindowConnection> connection = operatorManager_.GetAccessibilityWindowConnection(windowId);
    if (connection == nullptr) {
        HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
        return RET_OK;
    }
    operatorManager_.StopCallbackWait(windowId, treeId);

    RemoveTreeDeathRecipient(windowId, treeId, connection);
    if (connection->GetProxy() == nullptr && connection->GetCardProxySize() == 0) {
        operatorManager_.RemoveAccessibilityWindowConnection(windowId);
    }
    return RET_OK;
}

ErrCode AccessibilityAccountData::ClearFocus()
{
    HILOG_DEBUG();
    int32_t focusWindowId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetFocusWindowId();
    if (focusWindowId == INVALID_WINDOW_ID) {
        HILOG_WARN("No focused window to clear");
        return RET_OK;
    }
    return operatorManager_.ClearFocus(focusWindowId);
}

RetError AccessibilityAccountData::VerifyingToKenId(const uint32_t tokenId, const int32_t windowId,
    const int64_t elementId)
{
    return operatorManager_.VerifyingToKenId(tokenId, windowId, elementId);
}

ElementOperatorManager& AccessibilityAccountData::GetOperatorManager()
{
    return operatorManager_;
}

void AccountSubscriber::OnStateChanged(const AccountSA::OsAccountStateData &data)
{
    if (data.state == AccountSA::OsAccountState::SWITCHING) {
        HILOG_INFO("account switching.");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OffZoomGesture();
    }
}
} // namespace Accessibility
} // namespace OHOS
