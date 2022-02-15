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

#include "accessible_ability_manager_service.h"

#include <unistd.h>
#include <functional>

#include "ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_interaction_bridge.h"
#include "accessibility_window_manager.h"
#include "bundle_mgr_proxy.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "input_manager.h"
#include "os_account_info.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
const string TASK_SEND_EVENT = "SendEvent";
const string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
const string TASK_PUBLIC_NOTICE_EVENT = "PublicNoticeEvent";
const string TASK_SEND_PUBLIC_NOTICE_EVENT = "SendPublicNoticeEvent";

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true),
      bundleManager_(nullptr)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
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
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");

    if (isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already start.");
        return;
    }

    if (!Init()) {
        HILOG_ERROR("AccessibleAbilityManagerService::Init failed!");
        return;
    }
    bool ret = Publish(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());
    if (!ret) {
        HILOG_ERROR("AccessibleAbilityManagerService::Publish failed!");
        return;
    }
    HILOG_INFO("AccessibleAbilityManagerService::OnStart OK.");
    isRunning_ = true;

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    HILOG_INFO("AccessibleAbilityManagerService::call accountData->init().");
    accountData->init();
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("stop AccessibleAbilityManagerService");

    if (!isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already stop.");
        return;
    }

    accessibilityCommonEventRegistry_.UnRegister();
    a11yAccountsData_.clear();
    bundleManager_ = nullptr;
    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    stateCallbackDeathRecipient_ = nullptr;
    runner_.reset();
    handler_.reset();

    HILOG_INFO("AccessibleAbilityManagerService::OnStop OK.");
    isRunning_ = false;
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG(" %{public}s", __func__);
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

    if (!accessibilityCommonEventRegistry_.StartRegister()) {
        HILOG_ERROR("AccessibleAbilityManagerService::Init failed:Failed to subscribe common event");
        return false;
    }
#if 0
    std::vector<int> ids {};
    AccountSA::OsAccountManager::QueryActiveOsAccountIds(ids);
    HILOG_DEBUG("QueryActiveOsAccountIds ids.size() is %{public}d", ids.size());
    for (auto id : ids) {
        HILOG_DEBUG("QueryActiveOsAccountIds id is %{public}d", id);
        // Todo check active userid
    }
    //Get current accountId
    AccountSA::OsAccountInfo osAccountInfo;
    OHOS::ErrCode result = AccountSA::OsAccountManager::QueryCurrentOsAccount(osAccountInfo);
    HILOG_DEBUG(" the result of QueryCurrentOsAccount is %{public}d", result);
    currentAccountId_ = osAccountInfo.GetLocalId();
#else  // TBD set currentAccountId_ user 100
    currentAccountId_ = 100;
    HILOG_DEBUG("current accountId %{public}d", currentAccountId_);
#endif

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    // Get ExtensionInfo from BMS
    accountData->GetInstalledAbilitiesFromBMS();

    // TODO: [setting] Add listener of setting's URI.

    HILOG_INFO("AccessibleAbilityManagerService::Init OK");
    return true;
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent, const int accountId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("eventType[%{public}d]", (int)uiEvent.GetEventType());
    if (accountId != currentAccountId_) {
        HILOG_ERROR(" %{public}s accountId[%{public}d] is not matched", __func__, accountId);
        return;
    }
    AccessibilityEventInfo& event = const_cast<AccessibilityEventInfo&>(uiEvent);
    UpdateAccessibilityWindowStateByEvent(event);

    std::function<void()> sendEventFunc = std::bind(&AccessibleAbilityManagerService::SendEventInner, this, event);

    handler_->PostTask(sendEventFunc, TASK_SEND_EVENT);
}

void AccessibleAbilityManagerService::SendEventInner(AccessibilityEventInfo& uiEvent)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto& ability : abilities) {
        ability.second->OnAccessibilityEvent(uiEvent);
    }
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    if (accountId != currentAccountId_) {
        HILOG_ERROR(" %{public}s accountId[%{public}d] is not matched", __func__, accountId);
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

    callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
    accountData->AddCaptionPropertyCallback(callback);
    printf("accountData  AddCaptionPropertyCallback %d\n", accountData->GetCaptionPropertyCallbacks().size());
    HILOG_INFO("AccessibleAbilityManagerService::RegisterCaptionPropertyCallback successfully");

    return NO_ERROR;
}

uint32_t AccessibleAbilityManagerService::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState>& callback, const int accountId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    if (accountId != currentAccountId_) {
        HILOG_ERROR(" %{public}s accountId[%{public}d] is not matched", __func__, accountId);
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

vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetAbilityList(
    const int abilityTypes, const int stateType)
{
    HILOG_DEBUG(" %{public}s", __func__);
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
    for (auto& ability : abilities) {
        if (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes)) {
            infoList.push_back(ability);
        }
    }
    return infoList;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int windowId, const sptr<IAccessibilityElementOperator>& operation, int accountId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData->GetAccessibilityWindowConnection(windowId)) {
        HILOG_WARN("This operation already exists, do not register twice!!");
        return;
    }

    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    accountData->AddAccessibilityWindowConnection(windowId, connection);

    if (!interactionOperationDeathRecipient_) {
        interactionOperationDeathRecipient_ = new InteractionOperationDeathRecipient(windowId);
    }

    if (connection->GetProxy()) {
        auto object = connection->GetProxy()->AsObject();
        if (object) {
            HILOG_DEBUG("Add death recipient of operation");
            bool result = object->AddDeathRecipient(interactionOperationDeathRecipient_);
            HILOG_DEBUG("The result of adding operation's death recipient is %{public}d", result);
        }
    }
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int windowId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
        return;
    }

    if (connection->GetProxy()) {
        auto object = connection->GetProxy()->AsObject();
        if (object) {
            HILOG_DEBUG("Delete death recipient of operation");
            bool result = object->RemoveDeathRecipient(interactionOperationDeathRecipient_);
            HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
        }
    }

    accountData->RemoveAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG(" %{public}s", __func__);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams->DeregisterElementOperator(windowId_);
}

CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    return accountData->GetCaptionProperty();
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetCaptionProperty(caption);
    UpdateCaptionProperty();
    return result;
}

bool AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetCaptionState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetEnabled(const bool state)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetEnabled(state);
    UpdateAccessibilityState();
    return result;
}

void AccessibleAbilityManagerService::PersistElementNamesToSetting(
    const std::string& bundleName, std::map<std::string, AppExecFwk::ElementName>& componentNames, int accountId)
{
    // TODO: Update specified item to setting.
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    HILOG_DEBUG(" %{public}s", __func__);
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(currentAccountId_);
    a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    return accountData;
}

bool AccessibleAbilityManagerService::IsWantedKeyEvent(MMI::KeyEvent& event)
{
    HILOG_DEBUG(" %{public}s", __func__);

    int32_t keyCode = event.GetKeyCode();
    if (keyCode == MMI::KeyEvent::KEYCODE_VOLUME_UP || keyCode == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
        return true;
    }
    return false;
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(
    int displayId, Rect& rect, float scale, float centerX, float centerY)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto& ability : abilities) {
        ability.second->OnDisplayResized(displayId, rect, scale, centerX, centerY);
    }
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    HILOG_DEBUG(" %{public}s", __func__);
    if (bundleManager_) {
        return bundleManager_;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        HILOG_ERROR("failed:fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        HILOG_ERROR("failed:fail to get bundle manager proxy.");
        return nullptr;
    }

    bundleManager_ = iface_cast<AppExecFwk::BundleMgrProxy>(remoteObject);
    if (bundleManager_ == nullptr) {
        HILOG_ERROR("fail to new bundle manager.");
    } else {
        HILOG_INFO("AccessibleAbilityManagerService::GetBundleMgrProxy OK");
    }
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(
    int windowId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int windowId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->ClearFocus();
}

void AccessibleAbilityManagerService::OutsideTouch(int windowId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->OutsideTouch();
}

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri& uri)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    // TODO: Do something for Setting
    UpdateAbilities();
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector>& touchEventInjector)
{
    HILOG_DEBUG(" %{public}s", __func__);
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter>& keyEventFilter)
{
    HILOG_DEBUG(" %{public}s", __func__);
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG(" %{public}s", __func__);
    remote->RemoveDeathRecipient(this);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveStateCallback(remote);
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG(" %{public}s", __func__);
    remote->RemoveDeathRecipient(this);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveCaptionPropertyCallback(remote);
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG("%{public}s start", __func__);
    a11yAccountsData_.erase(accountId);
}

void AccessibleAbilityManagerService::PresentUser()
{
    HILOG_DEBUG("%{public}s start", __func__);

    GetCurrentAccountData()->ReadConfigurationForAccountData();
    UpdateAbilities();
}

void AccessibleAbilityManagerService::PackageRemoved(std::string& bundleName)
{
    HILOG_DEBUG("%{public}s start", __func__);

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetEnabledAbilities().empty()) {
        return;
    }

    bool needUpdateAbility = false;
    std::map<std::string, AppExecFwk::ElementName> enabledAbilities = packageAccount->GetEnabledAbilities();
    for (auto& enableAbility : enabledAbilities) {
        if (enableAbility.second.GetBundleName() == bundleName) {
            packageAccount->RemoveEnabledAbility(enableAbility.second);
            packageAccount->RemoveConnectingA11yAbility(enableAbility.second);
            PersistElementNamesToSetting(bundleName, enabledAbilities, currentAccountId_);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageChanged()
{
    HILOG_DEBUG("%{public}s start", __func__);

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetInstalledAbilities().empty()) {
        return;
    }

    packageAccount->ClearInstalledAbility();
    if (packageAccount->ReadConfigurationForAccountData()) {
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageUpdateFinished(std::string& bundleName)
{
    HILOG_DEBUG("%{public}s start", __func__);
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetConnectingA11yAbilities().empty()) {
        return;
    }

    bool needUpdateAbility = false;
    std::map<std::string, AppExecFwk::ElementName> connectingAbilities = packageAccount->GetConnectingA11yAbilities();
    for (auto& ability : connectingAbilities) {
        if (ability.second.GetBundleName() == bundleName) {
            packageAccount->RemoveConnectingA11yAbility(ability.second);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo& event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("%{public}s start %{public}d", __func__, event.GetWindowId());
    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
            AccessibilityWindowInfoManager::GetInstance().SetActiveWindow(event.GetWindowId());
            AccessibilityWindowInfoManager::GetInstance().SetAccessibilityFocusedWindow(event.GetWindowId());
            break;
        default:
            break;
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
    UpdateWindowChangeListener();
    UpdateMagnification();
    UpdateInputFilter();
    UpdateAccessibilityState();
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
    HILOG_DEBUG("installedAbilities is %{public}d.", installedAbilities.size());
    for (auto& installAbility : installedAbilities) {
        // TODO: deviceId
        // AppExecFwk::ElementName element(installAbility.GetAbilityInfo().deviceId,
        //                                 installAbility.GetPackageName(),
        //                                 installAbility.GetName());
        std::string deviceId;
        AppExecFwk::ElementName element(deviceId, installAbility.GetPackageName(), installAbility.GetName());
        HILOG_DEBUG("installAbility's packageName is %{public}s", installAbility.GetPackageName().c_str());
        HILOG_DEBUG("installAbility's abilityName is %{public}s", installAbility.GetName().c_str());
        std::string elementName = "/" + installAbility.GetPackageName() + "/"; // TODO

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

void AccessibleAbilityManagerService::UpdateAccessibilityState()
{
    HILOG_DEBUG("%{public}s start.", __func__);

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

void AccessibleAbilityManagerService::UpdateCaptionProperty()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    CaptionProperty caption = accountData->GetCaptionProperty();
    for (auto& callback : accountData->GetCaptionPropertyCallbacks()) {
        callback->OnCaptionPropertyChanged(caption);
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
    HILOG_DEBUG("InputInterceptor flag is %{public}d", flag);

    if (flag != 0) {
        inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
        inputInterceptor_->SetAvailableFunctions(flag);
        return;
    }
    if (inputInterceptor_ != nullptr) {
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

    std::vector<const sptr<Rosen::Display>> displays = AccessibilityDisplayManager::GetInstance().GetDisplays();

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

void AccessibleAbilityManagerService::UpdateWindowChangeListener()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    bool isWindowRetrieve = false;
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    auto connectedA11yAbilities = accountData->GetConnectedA11yAbilities();
    for (auto itr = connectedA11yAbilities.begin(); itr != connectedA11yAbilities.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE) {
            isWindowRetrieve = true;
            break;
        }
    }
    if (isWindowRetrieve) {
        HILOG_INFO("%{public}s RegisterWindowChangeListener.", __func__);
        AccessibilityWindowInfoManager::GetInstance().RegisterWindowChangeListener();
    } else {
        HILOG_INFO("%{public}s DeregisterWindowChangeListener.", __func__);
        AccessibilityWindowInfoManager::GetInstance().DeregisterWindowChangeListener();
        AccessibilityWindowInfoManager::GetInstance().GetAccessibilityWindows().clear();
    }
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetEnabledState();
    return result;
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetCaptionState();
    return result;
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetTouchGuideState();
    return result;
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetGestureState();
    return result;
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetKeyEventObserverState();
    return result;
}

bool AccessibleAbilityManagerService::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetTouchGuideState(state);
    return result;
}

bool AccessibleAbilityManagerService::SetGestureState(const bool state)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetGestureState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetKeyEventObserverState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetEnabledObj(it);
    UpdateAbilities();
    return result;
}

bool AccessibleAbilityManagerService::SetInstalled(std::vector<AccessibilityAbilityInfo> it)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetInstalled(it);
    return result;
}

// std::vector<WMDisplayInfo> AccessibleAbilityManagerService::GetDisplayList()
// {
//     std::vector<WMDisplayInfo> displays = AccessibilityDisplayManager::GetInstance().GetDisplays();
//     return displays;
// }

std::map<std::string, AppExecFwk::ElementName> AccessibleAbilityManagerService::GetEnabledAbilities()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    std::map<std::string, AppExecFwk::ElementName> it{};
    it = accountData->GetEnabledAbilities();
    return it;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetInstalledAbilities()
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    std::vector<AccessibilityAbilityInfo> it{};
    it = accountData->GetInstalledAbilities();
    return it;
}

}  // namespace Accessibility
}  // namespace OHOS
