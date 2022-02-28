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

#include <functional>
#include <ctime>
#include <unistd.h>
#include "ability_info.h"
#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessibility_interaction_bridge.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
EventType UTeventType;
int UTgestureId;
std::vector<EventType> eventType;

const string TASK_SEND_EVENT = "SendEvent";
const string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true), bundleManager_(nullptr)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
}

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

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId)
{
    UTeventType = uiEvent.GetEventType();
    eventType.push_back(UTeventType);
    HILOG_INFO("AccessibleAbilityManagerService::SendEvent successfully");
}

void AccessibleAbilityManagerService::SendEventInner(AccessibilityEventInfo &uiEvent)
{
}

uint32_t AccessibleAbilityManagerService::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState> &callback, const int accountId)
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

    callback->AsObject()->AddDeathRecipient(stateCallbackDeathRecipient_);
    accountData->AddStateCallback(callback);

    HILOG_INFO("AccessibleAbilityManagerService::RegisterStateCallback successfully");

    return accountData->GetAccessibilityState();
}

vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetAbilityList(const int abilityTypes,
    const int stateType)
{
    vector<AccessibilityAbilityInfo> infoList;
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
    vector<AccessibilityAbilityInfo> abilities = accountData->GetAbilitiesByState(state);
    for (auto &ability : abilities) {
        if (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes)) {
            infoList.push_back(ability);
        }
    }
    return infoList;
}

void AccessibleAbilityManagerService::RegisterElementOperator(const int windowId,
    const sptr<IAccessibilityElementOperator> &operation, int accountId)
{
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    accountData->AddAccessibilityWindowConnection(windowId, connection);

    HILOG_INFO("AccessibleAbilityManagerService::RegisterElementOperator successfully");
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    accountData->RemoveAccessibilityWindowConnection(windowId);

    HILOG_INFO("AccessibleAbilityManagerService::DeregisterElementOperator successfully");
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(currentAccountId_);
    a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    return accountData;
}

bool AccessibleAbilityManagerService::IsWantedKeyEvent(MMI::KeyEvent &event)
{
    return true;
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(int displayId,
    Rect &rect, float scale, float centerX, float centerY)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto &ability : abilities) {
        ability.second->OnDisplayResized(displayId, rect, scale, centerX, centerY);
    }
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(int windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int windowId)
{
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->ClearFocus();
}

void AccessibleAbilityManagerService::OutsideTouch(int windowId)
{
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->OutsideTouch();
}

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri &uri)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    UpdateAbilities();
}

void AccessibleAbilityManagerService::UpdateAbilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    vector<AccessibilityAbilityInfo> installedAbilities = accountData->GetInstalledAbilities();
    for (auto &installAbility : installedAbilities) {
        AppExecFwk::ElementName element(installAbility.GetAbilityInfo().deviceId,
                                        installAbility.GetPackageName(), installAbility.GetName());
        std::string elementName =  element.GetURI();
        // wait for the connecting ability.
        if (accountData->GetConnectingA11yAbilities().count(elementName)) {
            continue;
        }

        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(elementName);
        if (accountData->GetEnabledAbilities().count(elementName)) {
            if (!connection) {
                connection = new AccessibleAbilityConnection(accountData, connectCounter_++, installAbility);
                connection->Connect(element);
            }
        } else {
            if (connection) {
                connection->Disconnect();
            }
        }
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService()
{
    HILOG_DEBUG("%{public}s start.", __func__);

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
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    uint32_t state = accountData->GetAccessibilityState();
    for (auto &callback: accountData->GetStateCallbacks()) {
        callback->OnStateChanged(state);
    }
}

void AccessibleAbilityManagerService::UpdateInputFilter()
{
    HILOG_DEBUG("%{public}s start.", __func__);

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

    if (flag != 0) {
        inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
        inputInterceptor_->SetAvailableFunctions(flag);
        return;
    }
    if (inputInterceptor_) {
        HILOG_DEBUG("Has InputInterceptor before.");
        inputInterceptor_->SetAvailableFunctions(0);
    }
}

void AccessibleAbilityManagerService::UpdateMagnification()
{
    HILOG_DEBUG("%{public}s start.", __func__);

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

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
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
}

void AccessibleAbilityManagerService::PresentUser()
{
}

void AccessibleAbilityManagerService::PackageChanged(std::string& bundleName)
{
}

void AccessibleAbilityManagerService::PackageRemoved(std::string &bundleName)
{
}

void AccessibleAbilityManagerService::PackageUpdateFinished(std::string &bundleName)
{
}
} // namespace OHOS
} // namespace Accessibility