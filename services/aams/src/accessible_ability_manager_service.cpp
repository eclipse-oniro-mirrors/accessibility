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

static const int32_t TEMP_ACCOUNT_ID = 100;

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
    HILOG_DEBUG("start");
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
    HILOG_INFO("AccessibleAbilityManagerService::Publish OK.");
    isRunning_ = true;
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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

    // temp deal: set current account Id to TEMP_ACCOUNT_ID.
    // This is a temporary countermeasure, after which a formal countermeasure is required.
    currentAccountId_ = TEMP_ACCOUNT_ID;
    HILOG_DEBUG("current accountId %{public}d", currentAccountId_);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    accountData->init();

    HILOG_INFO("AddAbilityListener begin!");
    AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    HILOG_INFO("AddAbilityListener end!");
    
    // temp deal: [setting] Add listener of setting's URI.
    HILOG_INFO("AccessibleAbilityManagerService::Init OK");
    return true;
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent, const int accountId)
{
    HILOG_DEBUG("start");
    HILOG_INFO("eventType[%{public}d]", (int)uiEvent.GetEventType());
    HILOG_INFO("gestureId[%{public}d]", (int)uiEvent.GetGestureType());
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
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto& ability : abilities) {
        ability.second->OnAccessibilityEvent(uiEvent);
    }
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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
    HILOG_DEBUG(" %{public}s  abilityTypes(%{public}d) stateType(%{public}d)",
                __func__, abilityTypes, stateType);
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
    HILOG_DEBUG(" %{public}s:abilityes count is %{public}d", __func__, abilities.size());
    for (auto& ability : abilities) {
        if (static_cast<uint32_t>(abilityTypes) == AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL ||
           (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes))) {
            infoList.push_back(ability);
        }
    }
    HILOG_DEBUG(" %{public}s:infoList count is %{public}d", __func__, infoList.size());
    return infoList;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int windowId, const sptr<IAccessibilityElementOperator>& operation, int accountId)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams->DeregisterElementOperator(windowId_);
}

CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    return accountData->GetCaptionProperty();
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetCaptionProperty(caption);
    UpdateCaptionProperty();
    return result;
}

bool AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetCaptionState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetEnabled(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetEnabled(state);
    UpdateAccessibilityState();
    return result;
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    HILOG_DEBUG(" %{public}s: currentAccoutId is %{public}d ", __func__, currentAccountId_);
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
    HILOG_DEBUG("start");

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
    HILOG_DEBUG("start");
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
    HILOG_DEBUG(" %{public}s windowId(%{public}d)", __func__, windowId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int windowId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->ClearFocus();
}

void AccessibleAbilityManagerService::OutsideTouch(int windowId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    connection->GetProxy()->OutsideTouch();
}

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri& uri)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    // temp deal: Do something for Setting
    UpdateAbilities();
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector>& touchEventInjector)
{
    HILOG_DEBUG("start");
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter>& keyEventFilter)
{
    HILOG_DEBUG("start");
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }

    sptr<AccessibilityAbilityInfo> accessibilityInfo = new AccessibilityAbilityInfo();
    accessibilityInfo->SetPackageName(bundleName);
    packageAccount->RemoveInstalledAbility(*accessibilityInfo);

    bool needUpdateAbility = false;
    std::map<std::string, AppExecFwk::ElementName> enabledAbilities = packageAccount->GetEnabledAbilities();
    if (enabledAbilities.empty()) {
        HILOG_DEBUG("There is no enabled abilities.");
        return;
    }
    for (auto& enableAbility : enabledAbilities) {
        if (enableAbility.second.GetBundleName() == bundleName) {
            packageAccount->RemoveEnabledAbility(enableAbility.second);
            packageAccount->RemoveConnectingA11yAbility(enableAbility.second);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageAdd(std::string& bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    bool hasNewExtensionAbility = false;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    GetBundleMgrProxy()->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}d.", extensionInfos.size());
    for (auto newAbility : extensionInfos) {
        if (newAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package added is an extension ability and\
                extension ability's name is %{public}s", newAbility.name.c_str());
            sptr<AccessibilityAbilityInfo> accessibilityInfo = new AccessibilityAbilityInfo(newAbility);
            GetCurrentAccountData()->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("add new extension ability successfully and installed abilities's size is %{public}d",
                GetCurrentAccountData()->GetInstalledAbilities().size());
            hasNewExtensionAbility = true;
            break;
        }
    }

    if (hasNewExtensionAbility) {
        HILOG_DEBUG("add new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageChanged(std::string& bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    bool hasChanged = false;
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    sptr<AccessibilityAbilityInfo> accessibilityInfo = new AccessibilityAbilityInfo();
    accessibilityInfo->SetPackageName(bundleName);
    packageAccount->RemoveInstalledAbility(*accessibilityInfo);

    // add installed ability
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    GetBundleMgrProxy()->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}d.", extensionInfos.size());
    for (auto changedAbility : extensionInfos) {
        if (changedAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package changed is an extension ability and\
                extension ability's name is %{public}s", changedAbility.name.c_str());
            AccessibilityAbilityInfo* accessibilityInfo = new AccessibilityAbilityInfo(changedAbility);
            GetCurrentAccountData()->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("update new extension ability successfully and installed abilities's size is %{public}d",
                GetCurrentAccountData()->GetInstalledAbilities().size());
            hasChanged = true;
            break;
        }
    }

    if (hasChanged) {
        HILOG_DEBUG("update new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageUpdateFinished(std::string& bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
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
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    accountData->UpdateAccountCapabilities();
    // This is temp dell UpdateWindowChangeListener();
    UpdateMagnification();
    UpdateInputFilter();
    UpdateAccessibilityState();
}

void AccessibleAbilityManagerService::UpdateAbilities()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    vector<AccessibilityAbilityInfo> installedAbilities = accountData->GetInstalledAbilities();
    HILOG_DEBUG("installedAbilities is %{public}d.", installedAbilities.size());
    for (auto& installAbility : installedAbilities) {
        // temp deal: deviceId
        std::string deviceId;
        AppExecFwk::ElementName element(deviceId, installAbility.GetPackageName(), installAbility.GetName());
        HILOG_DEBUG("installAbility's packageName is %{public}s", installAbility.GetPackageName().c_str());
        HILOG_DEBUG("installAbility's abilityName is %{public}s", installAbility.GetName().c_str());
        std::string elementName = "/" + installAbility.GetPackageName() + "/"; // temp deal
        // wait for the connecting ability.
        if (accountData->GetConnectingA11yAbilities().count(elementName)) {
            continue;
        }

        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(elementName);
        if (accountData->GetEnabledAbilities().count(elementName)) {
            if (!connection) {
                // this is a temp deal for ace test
                installAbility.SetCapabilityValues(Capability::CAPABILITY_RETRIEVE |
                                                   Capability::CAPABILITY_KEY_EVENT_OBSERVER |
                                                   Capability::CAPABILITY_GESTURE);
                connection = new AccessibleAbilityConnection(accountData, connectCounter_++, installAbility);
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
        callback->OnCaptionPropertyChanged(caption);
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

void AccessibleAbilityManagerService::UpdateWindowChangeListener()
{
    HILOG_DEBUG("start.");

    bool isWindowRetrieve = true; // this is a temp deal for test
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
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetEnabledState();
    return result;
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetCaptionState();
    return result;
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetTouchGuideState();
    return result;
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetGestureState();
    return result;
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->GetKeyEventObserverState();
    return result;
}

bool AccessibleAbilityManagerService::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetTouchGuideState(state);
    return result;
}

bool AccessibleAbilityManagerService::SetGestureState(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetGestureState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetKeyEventObserverState(state);
    UpdateAccessibilityState();
    return result;
}

bool AccessibleAbilityManagerService::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->SetEnabledObj(it);
    UpdateAbilities();
    return result;
}

std::map<std::string, AppExecFwk::ElementName> AccessibleAbilityManagerService::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    std::map<std::string, AppExecFwk::ElementName> it {};
    it = accountData->GetEnabledAbilities();
    return it;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetInstalledAbilities()
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    std::vector<AccessibilityAbilityInfo> it {};
    it = accountData->GetInstalledAbilities();
    return it;
}

bool AccessibleAbilityManagerService::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    bool result = accountData->DisableAbilities(it);
    UpdateAbilities();
    return result;
}

bool AccessibleAbilityManagerService::RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("start");
    sptr<AccessibleAbilityConnection> connection = GetCurrentAccountData()->GetUITestConnectedAbilityConnection();

    if (connection) {
        HILOG_ERROR(" %{public}s connection is existed!!", __func__);
        return false;
    }

    std::function<void()> addUITestClientFunc =
        std::bind(&AccessibleAbilityManagerService::AddUITestClient, this, obj);
    handler_->PostTask(addUITestClientFunc, "AddUITestClient");
    return true;
}

void AccessibleAbilityManagerService::AddUITestClient(const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("start");
    auto currentAccountData = GetCurrentAccountData();

    // add installed ability
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    abilityInfo->SetPackageName("com.example.uitest");
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetAccessibilityAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    currentAccountData->AddInstalledAbility(*abilityInfo);

    // add connected ability
    sptr<AppExecFwk::ElementName> elementName = new AppExecFwk::ElementName();
    elementName->SetBundleName("com.example.uitest");
    elementName->SetAbilityName("uitestability");
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        currentAccountData, connectCounter_++, *abilityInfo);
    currentAccountData->AddUITestConnectedAbility(connection);
    connection->OnAbilityConnectDone(*elementName, obj, 0);
}

bool AccessibleAbilityManagerService::DeregisterUITestAbilityConnectionClient()
{
    HILOG_DEBUG("start");
    sptr<AccessibleAbilityConnection> connection = GetCurrentAccountData()->GetUITestConnectedAbilityConnection();

    if (connection == nullptr) {
        HILOG_ERROR(" %{public}s connection is not existed!!", __func__);
        return false;
    }
    std::function<void()> removeUITestClientFunc =
        std::bind(&AccessibleAbilityManagerService::RemoveUITestClient, this, connection);
    handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
    return true;
}

void AccessibleAbilityManagerService::RemoveUITestClient(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("start");
    auto currentAccountData = GetCurrentAccountData();

    // remove installed ability
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    abilityInfo->SetPackageName("com.example.uitest");
    currentAccountData->RemoveInstalledAbility(*abilityInfo);

    // remove connected ability
    currentAccountData->RemoveUITestConnectedAbility(connection);
    connection->OnAbilityDisconnectDone(connection->GetElementName(), 0);
}

int AccessibleAbilityManagerService::GetActiveWindow()
{
    return AccessibilityWindowInfoManager::GetInstance().activeWindowId_;
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d added!", systemAbilityId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    switch (systemAbilityId) {
        case COMMON_EVENT_SERVICE_ID:
            // Register common event
            if (!accessibilityCommonEventRegistry_.StartRegister()) {
                HILOG_ERROR("AccessibleAbilityManagerService::Init failed:Failed to subscribe common event");
            }
            break;
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            // Get installed accessibility extension ability from BMS
            if (accountData->GetInstalledAbilitiesFromBMS()) {
                UpdateAbilities();
                UpdateAccessibilityManagerService();
            } else {
                HILOG_ERROR("Get installed ExtentionAbility failed");
            }
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            AccessibilityWindowInfoManager::GetInstance().RegisterWindowChangeListener();
            break;
        default:
            break;
    }
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d removed!", systemAbilityId);
}
} // namespace Accessibility
} // namespace OHOS