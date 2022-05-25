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

#include <ctime>
#include <functional>
#include <gtest/gtest.h>
#include <unistd.h>
#include "ability_info.h"
#include "accessibility_ability_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
static const std::string TASK_ENABLE_ABILITIES = "EnableAbilities";
static const std::string TASK_GET_ENABLE_ABILITIES = "GetEnableAbilities";
static const std::string TASK_DISABLE_ABILITIES = "DisableAbilities";
const std::string TASK_SEND_EVENT = "SendEvent";
const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
static const int32_t TEMP_ACCOUNT_ID = 100;

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<AccessibleAbilityManagerService>::GetInstance());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
    dependentServicesStatus_[ABILITY_MGR_SERVICE_ID] = false;
    dependentServicesStatus_[SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN] = false;
    dependentServicesStatus_[BUNDLE_MGR_SERVICE_SYS_ABILITY_ID] = false;
    dependentServicesStatus_[COMMON_EVENT_SERVICE_ID] = false;
    dependentServicesStatus_[DISPLAY_MANAGER_SERVICE_SA_ID] = false;
    dependentServicesStatus_[WINDOW_MANAGER_SERVICE_ID] = false;
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
    HILOG_INFO("AccessibleAbilityManagerService::~AccessibleAbilityManagerService");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");

    if (isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already start.");
        return;
    }

    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME);
        if (!runner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS runner failed");
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AAMSEventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS event handler failed");
            return;
        }
    }

    HILOG_INFO("AddAbilityListener!");
    AddSystemAbilityListener(ABILITY_MGR_SERVICE_ID);
    AddSystemAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    AddSystemAbilityListener(DISPLAY_MANAGER_SERVICE_SA_ID);
    AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);

    isRunning_ = true;
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");

    if (!isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already stop.");
        return;
    }

    a11yAccountsData_.clear();
    bundleManager_ = nullptr;
    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    stateCallbackDeathRecipient_ = nullptr;
    runner_.reset();
    handler_.reset();
    for (auto &iter : dependentServicesStatus_) {
        iter.second = false;
    }

    HILOG_INFO("AccessibleAbilityManagerService::OnStop OK.");
    isRunning_ = false;
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d added!", systemAbilityId);
    if (!handler_) {
        HILOG_DEBUG("Event handler is nullptr.");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        auto iter = dependentServicesStatus_.find(systemAbilityId);
        if (iter == dependentServicesStatus_.end()) {
            HILOG_ERROR("SystemAbilityId is not found!");
            return;
        }

        dependentServicesStatus_[systemAbilityId] = true;
        for (auto &iter : dependentServicesStatus_) {
            if (iter.second == false) {
                HILOG_DEBUG("Not all the dependence is ready!");
                return;
            }
        }

        if (Init() == false) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed!");
            return;
        }

        if (Publish(this) == false) {
            HILOG_ERROR("AccessibleAbilityManagerService::Publish failed!");
            return;
        }
        HILOG_DEBUG("AAMS is ready!");
        }), "OnAddSystemAbility");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    HILOG_INFO("OnRemoveSystemAbility AccessibleAbilityManagerService");
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG("start");
    currentAccountId_ = GetOsAccountId();
    HILOG_DEBUG("current accountId %{public}d", currentAccountId_);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return false;
    }
    accountData->Init();
    // Get installed accessibility extension ability from BMS
    if (accountData->GetInstalledAbilitiesFromBMS()) {
        UpdateAbilities();
        UpdateAccessibilityManagerService();
    } else {
        HILOG_ERROR("Get installed ExtensionAbility failed");
        return false;
    }

    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
    Singleton<AccessibilityWindowManager>::GetInstance().Init();

    return true;
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent)
{
    HILOG_INFO("AccessibleAbilityManagerService::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    AccessibilityAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    AccessibilityAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        AccessibilityAbilityHelper::GetInstance().AddSendEventTimes();
        }), "TASK_SEND_EVENT");
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return 0;
    }

    if (!stateCallbackDeathRecipient_) {
        stateCallbackDeathRecipient_ = new StateCallbackDeathRecipient();
    }
    if (callback != nullptr) {
        callback->AsObject()->AddDeathRecipient(stateCallbackDeathRecipient_);
        accountData->AddStateCallback(callback);
    }

    HILOG_INFO("AccessibleAbilityManagerService::RegisterStateCallback successfully");

    return accountData->GetAccessibilityState();
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetAbilityList(
    const uint32_t abilityTypes, const int32_t stateType)
{
    std::vector<AccessibilityAbilityInfo> infoList;
    if ((stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("stateType is out of range!!");
        return infoList;
    }

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    if (!accountData) {
        HILOG_ERROR("Get current account data failed!!");
        return infoList;
    }

    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    std::vector<AccessibilityAbilityInfo> abilities = accountData->GetAbilitiesByState(state);
    for (auto& ability : abilities) {
        if (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes)) {
            infoList.push_back(ability);
        }
    }
    return infoList;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& operation)
{
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(
        windowId, operation, GetOsAccountId());
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr && connection != nullptr) {
        accountData->AddAccessibilityWindowConnection(windowId, connection);
    }
    HILOG_INFO("AccessibleAbilityManagerService::RegisterElementOperator successfully");
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        accountData->RemoveAccessibilityWindowConnection(windowId);
        HILOG_INFO("AccessibleAbilityManagerService::DeregisterElementOperator successfully");
    }
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(currentAccountId_);
    a11yAccountsData_.insert(std::make_pair(currentAccountId_, accountData));
    return accountData;
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(int32_t windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int32_t windowId)
{
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection != nullptr) {
        connection->GetProxy()->ClearFocus();
    }
}

void AccessibleAbilityManagerService::OutsideTouch(int32_t windowId)
{
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection != nullptr) {
        connection->GetProxy()->OutsideTouch();
    }
}

void AccessibleAbilityManagerService::UpdateAbilities()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    std::vector<AccessibilityAbilityInfo> installedAbilities = accountData->GetInstalledAbilities();
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities.size());
    for (auto& installAbility : installedAbilities) {
        std::string deviceId;
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(),
            abilityName.c_str());

        auto connectingAbilities = accountData->GetConnectingA11yAbilities();
        std::vector<std::string>::iterator iter =
            std::find(connectingAbilities.begin(), connectingAbilities.end(), bundleName);
        if (iter != connectingAbilities.end()) {
            HILOG_DEBUG("The ability(%{public}s) is connecting.", bundleName.c_str());
            continue;
        }

        AppExecFwk::ElementName connectionElement("", bundleName, "");
        sptr<AccessibleAbilityConnection> connection =
            accountData->GetAccessibleAbilityConnection(connectionElement.GetURI());

        auto enabledAbilities = accountData->GetEnabledAbilities();
        iter = std::find(enabledAbilities.begin(), enabledAbilities.end(), bundleName);
        if (iter != enabledAbilities.end()) {
            if (!connection) {
                AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
                connection =
                    new (std::nothrow) AccessibleAbilityConnection(accountData, connectCounter_++, installAbility);
                connection->Connect(element);
            }
        } else {
            HILOG_DEBUG("not in enabledAbilites list .");
            if (connection) {
                connection->Disconnect();
            }
        }
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    accountData->UpdateAccountCapabilities();
    UpdateInputFilter();
    UpdateAccessibilityState();
}

void AccessibleAbilityManagerService::UpdateAccessibilityState()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    uint32_t state = accountData->GetAccessibilityState();
    for (auto& callback : accountData->GetStateCallbacks()) {
        callback->OnStateChanged(state);
    }
}

void AccessibleAbilityManagerService::UpdateInputFilter()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    int flag = 0;
    if (accountData->GetScreenMagnificationFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    }
    if (accountData->GetEventTouchGuideStateFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;
    }
    if (accountData->GetFilteringKeyEventsFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    }
    if (accountData->GetGesturesSimulationFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    }
    HILOG_DEBUG("InputFilter flag is %{public}d", flag);

    if (flag) {
        inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
        if (inputInterceptor_ != nullptr) {
            inputInterceptor_->SetAvailableFunctions(flag);
        }
    } else if (inputInterceptor_ != nullptr) {
        HILOG_DEBUG("Has InputInterceptor before.");
        inputInterceptor_->SetAvailableFunctions(0);
    } else {
        HILOG_DEBUG("InputInterceptor is null.");
    }
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector>& touchEventInjector)
{
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter>& keyEventFilter)
{
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveStateCallback(remote);
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    (void)accountId;
}

void AccessibleAbilityManagerService::PackageChanged(std::string& bundleName)
{
    (void)bundleName;
}
void AccessibleAbilityManagerService::PackageRemoved(std::string& bundleName)
{
    (void)bundleName;
}

AccessibilityConfig::CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        return accountData->GetCaptionProperty();
    }
    AccessibilityConfig::CaptionProperty cp;
    return cp;
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->SetCaptionProperty(caption);
        UpdateCaptionProperty();
        return result;
    }
    return false;
}

void AccessibleAbilityManagerService::UpdateCaptionProperty()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    AccessibilityConfig::CaptionProperty caption = accountData->GetCaptionProperty();
    for (auto& callback : accountData->GetCaptionPropertyCallbacks()) {
        callback->OnPropertyChanged(caption);
    }
}

bool AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->SetCaptionState(state);
        UpdateAccessibilityState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->GetEnabledState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->GetCaptionState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->GetTouchGuideState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->GetGestureState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        bool result = accountData->GetKeyEventObserverState();
        return result;
    }
    return false;
}

bool AccessibleAbilityManagerService::EnableAbilities(const std::string name, const uint32_t capabilities)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name, &capabilities]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->EnableAbilities(name, capabilities);
        UpdateAbilities();
        syncPromise.set_value(result);
        }), "TASK_ENABLE_ABILITIES");
    return syncFuture.get();
}

std::vector<std::string> AccessibleAbilityManagerService::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    std::vector<std::string> abilities;
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return abilities;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &abilities]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value();
            return;
        }
        abilities = accountData->GetEnabledAbilities();
        syncPromise.set_value();
        }), "TASK_GET_ENABLE_ABILITIES");
    syncFuture.get();

    return abilities;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetInstalledAbilities()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        std::vector<AccessibilityAbilityInfo> it {};
        it = accountData->GetInstalledAbilities();
        return it;
    }
    std::vector<AccessibilityAbilityInfo> ret {};
    return ret;
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return ERR_INVALID_VALUE;
    }

    if (!captionPropertyCallbackDeathRecipient_) {
        captionPropertyCallbackDeathRecipient_ = new CaptionPropertyCallbackDeathRecipient();
    }
    if (callback != nullptr) {
        callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
        accountData->AddCaptionPropertyCallback(callback);
    }

    HILOG_INFO("AccessibleAbilityManagerService::RegisterCaptionPropertyCallback successfully");

    return NO_ERROR;
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start");
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveCaptionPropertyCallback(remote);
}

bool AccessibleAbilityManagerService::DisableAbilities(const std::string name)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        accountData->RemoveEnabledAbility(name);
        UpdateAbilities();
        syncPromise.set_value(true);
        }), "TASK_DISABLE_ABILITIES");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject>& obj)
{
    (void)obj;
    return true;
}

bool AccessibleAbilityManagerService::DisableUITestAbility()
{
    return true;
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    return 0;
}

void AccessibleAbilityManagerService::PackageAdd(std::string& bundleName)
{
    (void)bundleName;
}

int32_t AccessibleAbilityManagerService::GetOsAccountId()
{
    return TEMP_ACCOUNT_ID;
}

bool AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();

    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetScreenMagnificationState(state);
        UpdateAccessibilityState();
        UpdateInputFilter();
        syncPromise.set_value(result);
        }), "TASK_SET_SCREENMAGNIFICATION_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();

    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetShortKeyState(state);
        UpdateAccessibilityState();
        UpdateInputFilter();
        syncPromise.set_value(result);
        }), "TASK_SET_SHORTKEY_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        bool result = accountData->SetMouseKeyState(state);
        UpdateInputFilter();
        syncPromise.set_value(result);
        }), "TASK_SET_MOUSEKEY_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        bool result = accountData->SetMouseAutoClick(time);
        UpdateInputFilter();
        syncPromise.set_value(result);
        }), "TASK_SET_MOUSE_AUTOCLICK");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, name]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetShortkeyTarget(name);
        UpdateAccessibilityState();
        syncPromise.set_value(result);
        }), "TASK_SET_SHORTKEY_TARGET");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetHighContrastTextState(state);
        syncPromise.set_value(result);
        }), "TASK_SET_HIGHCONTRASTTEXT_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();

    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        bool result = accountData->SetInvertColorState(state);
        syncPromise.set_value(result);
        }), "TASK_SET_INVERTCOLOR_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetAnimationOffState(state);
        UpdateAccessibilityState();
        syncPromise.set_value(result);
        }), "TASK_SET_ANIMATIONOFF_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetAnimationOffState(state);
        UpdateAccessibilityState();
        syncPromise.set_value(result);
        }), "TASK_SET_AUDIOMONO_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, filter]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetDaltonizationColorFilter(filter);
        syncPromise.set_value(result);
        }), "TASK_SET_DALTONIZATION_COLORFILTER");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetContentTimeout(time);
        syncPromise.set_value(result);
        }), "TASK_SET_CONTENT_TIMEOUT");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, discount]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetBrightnessDiscount(discount);
        syncPromise.set_value(result);
        }), "TASK_SET_BRIGHTNESS_DISCOUNT");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, balance]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->SetAudioBalance(balance);
        syncPromise.set_value(result);
        }), "TASK_SET_AUDIO_BALANCE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetScreenMagnificationState()
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetScreenMagnificationState();
        syncPromise.set_value(result);
        }), "TASK_GET_SCREENMAGNIFIER_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetShortKeyState()
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetShortKeyState();
        syncPromise.set_value(result);
        }), "TASK_GET_SHORTKEY_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetMouseKeyState()
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetMouseKeyState();
        syncPromise.set_value(result);
        }), "TASK_GET_MOUSEKEY_STATE");
    return syncFuture.get();
}

int32_t AccessibleAbilityManagerService::GetMouseAutoClick()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        int32_t result = accountData->GetMouseAutoClick();
        syncPromise.set_value(result);
        }), "TASK_GET_MOUSE_AUTOCLICK");

    return syncFuture.get();
}

std::string AccessibleAbilityManagerService::GetShortkeyTarget()
{
    HILOG_DEBUG("start");
    std::promise<std::string> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value("");
            return;
        }
        std::string result = accountData->GetShortkeyTarget();
        syncPromise.set_value(result);
        }), "TASK_GET_SHORTKEY_TARGET");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetHighContrastTextState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetHighContrastTextState();
        syncPromise.set_value(result);
        }), "TASK_GET_HIGHCONTRASTTEXT_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetInvertColorState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetInvertColorState();
        syncPromise.set_value(result);
        }), "TASK_GET_INVERTCOLOR_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetAnimationOffState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetAnimationOffState();
        syncPromise.set_value(result);
        }), "TASK_GET_ANIMATIONOFF_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetAudioMonoState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetAudioMonoState();
        syncPromise.set_value(result);
        }), "TASK_GET_AUDIOMONO_STATE");

    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::GetDaltonizationColorFilter()
{
    HILOG_DEBUG("start");
    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        uint32_t result = accountData->GetDaltonizationColorFilter();
        syncPromise.set_value(result);
        }), "TASK_GET_DALTONIZATION_COLORFILTER");

    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::GetContentTimeout()
{
    HILOG_DEBUG("start");
    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        uint32_t result = accountData->GetContentTimeout();
        syncPromise.set_value(result);
        }), "TASK_GET_CONTENT_TIMEOUT");

    return syncFuture.get();
}

float AccessibleAbilityManagerService::GetBrightnessDiscount()
{
    HILOG_DEBUG("start");
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        float result = accountData->GetBrightnessDiscount();
        syncPromise.set_value(result);
        }), "TASK_GET_BRIGHTNESS_DISCOUNT");

    return syncFuture.get();
}

float AccessibleAbilityManagerService::GetAudioBalance()
{
    HILOG_DEBUG("start");
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        float result = accountData->GetAudioBalance();
        syncPromise.set_value(result);
        }), "TASK_GET_AUDIO_BALANCE");

    return syncFuture.get();
}

void AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    (void)observer;
}

uint32_t AccessibleAbilityManagerService::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    HILOG_DEBUG("start");
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed! callback:%{public}p, handler:%{public}p",
            callback.GetRefPtr(), handler_.get());
        return ERR_INVALID_VALUE;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG("start");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value(ERR_INVALID_VALUE);
            return;
        }
        if (!configCallbackDeathRecipient_) {
            configCallbackDeathRecipient_ = new(std::nothrow) ConfigCallbackDeathRecipient();
            if (!configCallbackDeathRecipient_) {
                HILOG_ERROR("configCallbackDeathRecipient_ is null");
                syncPromise.set_value(ERR_INVALID_VALUE);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise.set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(configCallbackDeathRecipient_);
        accountData->AddConfigCallback(callback);
        HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
            accountData->GetConfigCallbacks().size());
        syncPromise.set_value(NO_ERROR);
        }), "TASK_REGISTER_CONFIG_OBSERVER");
    return syncFuture.get();
}

void AccessibleAbilityManagerService::ConfigCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveConfigCallback(remote);
}
} // namespace Accessibility
} // namespace OHOS