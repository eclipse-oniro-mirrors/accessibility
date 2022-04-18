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

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true), bundleManager_(nullptr)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");

    if (!Init()) {
        HILOG_ERROR("AccessibleAbilityManagerService::Init failed!");
        return;
    }
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    HILOG_INFO("OnAddSystemAbility AccessibleAbilityManagerService");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    HILOG_INFO("OnRemoveSystemAbility AccessibleAbilityManagerService");
}

bool AccessibleAbilityManagerService::Init()
{
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME);
        if (!runner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed:create AAMS runner failed");
            return false;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AAMSEventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed:create AAMS event handler failed");
            return false;
        }
    }

    HILOG_INFO("AccessibleAbilityManagerService::Init OK");
    return true;
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent, const int32_t accountId)
{
    (void)accountId;
    HILOG_INFO("AccessibleAbilityManagerService::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    AccessibilityAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    AccessibilityAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& callback, const int32_t accountId)
{
    if (accountId != currentAccountId_) {
        return 0;
    }
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
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& operation, int32_t accountId)
{
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
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

bool AccessibleAbilityManagerService::IsWantedKeyEvent(MMI::KeyEvent& event)
{
    if (0 == static_cast<uint32_t>(event.GetKeyCode())) {
        return true;
    }
    return false;
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(
    int32_t displayId, Rect& rect, float scale, float centerX, float centerY)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        std::map<std::string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
        for (auto& ability : abilities) {
            ability.second->OnDisplayResized(displayId, rect, scale, centerX, centerY);
        }
    }
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

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri& uri)
{
    (void)selfChange;
    (void)uri;
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    UpdateAbilities();
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
    UpdateMagnification();
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

void AccessibleAbilityManagerService::UpdateMagnification()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    std::vector<sptr<Rosen::Display>> displays = AccessibilityDisplayManager::GetInstance().GetDisplays();

    if (accountData->GetScreenMagnificationFlag()) {
        for (sptr<Rosen::Display> display : displays) {
            AccessibilityZoomProxy::GetInstance().Register(display->GetId());
        }
    } else {
        for (sptr<Rosen::Display> display : displays) {
            AccessibilityZoomProxy::GetInstance().Unregister(display->GetId());
        }
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
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
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
void AccessibleAbilityManagerService::PresentUser()
{}
void AccessibleAbilityManagerService::PackageChanged(std::string& bundleName)
{
    (void)bundleName;
}
void AccessibleAbilityManagerService::PackageRemoved(std::string& bundleName)
{
    (void)bundleName;
}
void AccessibleAbilityManagerService::PackageUpdateFinished(std::string& bundleName)
{
    (void)bundleName;
}
CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        return accountData->GetCaptionProperty();
    }
    CaptionProperty cp;
    return cp;
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const CaptionProperty& caption)
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

    CaptionProperty caption = accountData->GetCaptionProperty();
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

bool AccessibleAbilityManagerService::EnableAbilities(std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task =
        std::bind(&AccessibleAbilityManagerService::InnerEnableAbilities, this, std::ref(syncPromise), abilities);

    handler_->PostTask(task, TASK_ENABLE_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerEnableAbilities(
    std::promise<bool>& syncPromise, std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->EnableAbilities(abilities);
    UpdateAbilities();
    syncPromise.set_value(result);
}

std::vector<std::string> AccessibleAbilityManagerService::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    std::promise<std::vector<std::string>> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task =
        std::bind(&AccessibleAbilityManagerService::InnerGetEnabledAbilities, this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_ENABLE_ABILITIES);
    std::vector<std::string> ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetEnabledAbilities(std::promise<std::vector<std::string>>& syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    std::vector<std::string> abilities = accountData->GetEnabledAbilities();
    syncPromise.set_value(abilities);
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
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback, const int32_t accountId)
{
    HILOG_DEBUG("start");
    if (accountId != currentAccountId_) {
        HILOG_ERROR("accountId[%{public}d] is not matched", accountId);
        return ERR_INVALID_VALUE;
    }
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
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveCaptionPropertyCallback(remote);
}

bool AccessibleAbilityManagerService::DisableAbilities(std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task =
        std::bind(&AccessibleAbilityManagerService::InnerDisableAbilities, this, std::ref(syncPromise), abilities);

    handler_->PostTask(task, TASK_DISABLE_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerDisableAbilities(
    std::promise<bool>& syncPromise, std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->DisableAbilities(abilities);
    UpdateAbilities();
    syncPromise.set_value(result);
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
} // namespace Accessibility
} // namespace OHOS