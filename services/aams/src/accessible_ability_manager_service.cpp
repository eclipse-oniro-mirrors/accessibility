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

#include <time.h>
#include <functional>
#include <unistd.h>
#include "accessible_ability_manager_service.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "hilog_wrapper.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "dummy.h"
#include "multimoduleDummy.h"
#include "ability_info.h"
#include "accessibility_interaction_bridge.h"
#include "accessibility_display_manager.h"
#include "window_manager.h"

using namespace std;

namespace OHOS {
namespace Accessibility {


const string TASK_SEND_EVENT = "SendEvent";
const string TASK_INTERRUPT = "Interrupt";
const string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
const string TASK_PUBLIC_NOTICE_EVENT = "PublicNoticeEvent";
const string TASK_SEND_PUBLIC_NOTICE_EVENT = "SendPublicNoticeEvent";

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBLE_ABILITY_MANAGER_SERVICE_ID, true),
      bundleManager_(nullptr) {
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService() {
}

void AccessibleAbilityManagerService::OnStart() {
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
}

void AccessibleAbilityManagerService::OnStop() {
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("stop AccessibleAbilityManagerService");

    if (!isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already stop.");
        return;
    }

    accessibilityCommonEventRegistry_.UnRegister();
    a11yAccountsData_.clear();
    bundleManager_ = nullptr;
    windowManager_ = nullptr;
    inputFilter_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    stateCallbackDeathRecipient_ = nullptr;
    inputManager_.reset();
    runner_.reset();
    handler_.reset();
    isTouchInteraction_ = false;
    hasInputFilter_ = false;

    HILOG_INFO("AccessibleAbilityManagerService::OnStop OK.");
    isRunning_ = false;
}

bool AccessibleAbilityManagerService::Init() {
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

    // TODO: [setting] Add listener of setting's URI.

    HILOG_INFO("AccessibleAbilityManagerService::Init OK");
    return true;
}

void AccessibleAbilityManagerService::RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj) {
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("AccessibleAbilityManagerService::RegisterAbilityConnectionClientTmp start");
    currentAccountId_ = 0;
    AppExecFwk::AbilityInfo info;
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);
    GetBundleMgrProxy()->QueryAbilityInfo(want, info);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    AppExecFwk::ElementName elementName(info.deviceId, info.bundleName, info.name);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    a11yAccountsData_.insert(make_pair(0, accountData));
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter_++,
        *abilityInfo);
    connection->OnAbilityConnectDone(elementName, obj, 0);
    accountData->AddInstalledAbility(*abilityInfo);

    HILOG_INFO("AccessibleAbilityManagerService::RegisterAbilityConnectionClientTmp successfully");
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId) {
    HILOG_DEBUG(" %{public}s", __func__);
    HILOG_INFO("eventType[%{public}d]", (int)uiEvent.GetEventType());
    if (accountId != currentAccountId_) {
        HILOG_ERROR(" %{public}s accountId[%{public}d] is not matched", __func__, accountId);
        return;
    }
    AccessibilityEventInfo& event = const_cast<AccessibilityEventInfo&>(uiEvent);
    UpdateAccessibilityWindowStateByAccessibilityEvent(event);

    std::function<void()> sendEventFunc =
        std::bind(&AccessibleAbilityManagerService::SendEventInner, this, event);

    handler_->PostTask(sendEventFunc, TASK_SEND_EVENT);
}

void AccessibleAbilityManagerService::SendEventInner(AccessibilityEventInfo &uiEvent) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto &ability : abilities) {
        ability.second->OnAccessibilityEvent(uiEvent);
    }
}

uint32_t AccessibleAbilityManagerService::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState> &callback, const int accountId) {
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
    printf("accountData  AddStateCallback %d\n",accountData->GetStateCallbacks().size());
    HILOG_INFO("AccessibleAbilityManagerService::RegisterStateCallback successfully");

    return accountData->GetAccessibilityState();
}

vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetAbilityList(const int abilityTypes,
    const int stateType) {
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
    for (auto &ability : abilities) {
        if (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes)) {
            infoList.push_back(ability);
        }
    }
    return infoList;
}

void AccessibleAbilityManagerService::RegisterInteractionOperation(const int windowId,
    const sptr<IAccessibilityInteractionOperation> &operation, int accountId)
{
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData->GetAccessibilityInteractionConnection(windowId)) {
        HILOG_WARN("This operation already exists, do not register twice!!");
        return;
    }

    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(windowId, operation,
                                                                                                 accountId);
    accountData->AddAccessibilityInteractionConnection(windowId, connection);

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
    sptr<Rosen::WindowInfo> windowInfo = new Rosen::WindowInfo();
    windowInfo->wid_ = windowId;
    AccessibilityWindowInfoManager::GetInstance().OnWindowUpdate(windowInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
}

void AccessibleAbilityManagerService::DeregisterInteractionOperation(int windowId) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    sptr<AccessibilityInteractionConnection> connection = accountData->GetAccessibilityInteractionConnection(windowId);
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

    accountData->RemoveAccessibilityInteractionConnection(windowId);
    sptr<Rosen::WindowInfo> windowInfo = new Rosen::WindowInfo();
    windowInfo->wid_ = windowId;
    AccessibilityWindowInfoManager::GetInstance().OnWindowUpdate(windowInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
}

void AccessibleAbilityManagerService::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG(" %{public}s", __func__);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams->DeregisterInteractionOperation(windowId_);
}

void AccessibleAbilityManagerService::Interrupt(const int accountId) {
    HILOG_DEBUG(" %{public}s", __func__);
    if (accountId != currentAccountId_) {
        return;
    }
    std::function<void()> interruptFunc = std::bind(&AccessibleAbilityManagerService::InterruptInner, this);

    handler_->PostTask(interruptFunc, TASK_INTERRUPT);
}

void AccessibleAbilityManagerService::InterruptInner() {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto &ability : abilities) {
        ability.second->OnInterrupt();
    }
}

uint64_t AccessibleAbilityManagerService::GetSuggestedInterval() {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    return static_cast<uint64_t>(accountData->GetNonInteractiveUiInterval()) << INT32_BIT_NUM |
        accountData->GetInteractiveUiInterval();
}

void AccessibleAbilityManagerService::PersistElementNamesToSetting(std::string &bundleName, std::map<std::string, AppExecFwk::ElementName> &componentNames, int accountId) {
    // TODO: Update specified item to setting.
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData() {
    HILOG_DEBUG(" %{public}s", __func__);
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(currentAccountId_);
    a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    return accountData;
}

bool AccessibleAbilityManagerService::IsWantedKeyEvent(MMI::KeyEvent &event) {
#if 0
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return false;
    }
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();

    for (auto &ability : abilities) {
        if (ability.second->GetAbilityInfo().GetKeyEventType() == static_cast<uint32_t>(event.GetKeyCode())) {
            return true;
        }
    }
    return false;
#else
    return true;
#endif
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(int displayId, Rect &rect, float scale,
    float centerX, float centerY) {
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto &ability : abilities) {
        ability.second->OnDisplayResizeChanged(displayId, rect, scale, centerX, centerY);
    }
}

void AccessibleAbilityManagerService::OnGesture(int id) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto &ability : abilities) {
        ability.second->OnGesture(id);
    }
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy() {
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

    bundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    HILOG_INFO("AccessibleAbilityManagerService::GetBundleMgrProxy OK");

    return bundleManager_;
}

sptr<IWindowManagerService> AccessibleAbilityManagerService::GetWindowMgrProxy() {
    HILOG_DEBUG(" %{public}s", __func__);
    if (!windowManager_) {
        windowManager_ = new IWindowManagerService();
    }

    return windowManager_;
}

sptr<AccessibilityInteractionConnection> AccessibleAbilityManagerService::GetAccessibilityInteractionConnection(
    int windowId) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityInteractionConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int windowId) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityInteractionConnection> connection = GetAccessibilityInteractionConnection(windowId);
    connection->GetProxy()->ClearFocus();
}

void AccessibleAbilityManagerService::OutsideTouch(int windowId) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityInteractionConnection> connection = GetAccessibilityInteractionConnection(windowId);
    connection->GetProxy()->OutsideTouch();
}

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri &uri) {
    HILOG_DEBUG(" %{public}s", __func__);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    // TODO: Do something for Setting
    UpdateAbilities();
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector) {
    HILOG_DEBUG(" %{public}s", __func__);
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter) {
    HILOG_DEBUG(" %{public}s", __func__);
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote) {
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

void AccessibleAbilityManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG("%{public}s start", __func__);

    if (accountId == GetCurrentAccountId()) {
        HILOG_ERROR("The account id is same as current account id.");
        return;
    }

    // Disconnect the old account.
    sptr<AccessibilityAccountData> oldAccountData = GetCurrentAccountData();
    if (!oldAccountData) {
        HILOG_ERROR("Get oldAccountData failed.");
        return;
    }
    oldAccountData->OnAccountSwitched();
    printf("oldAccountData->OnAccountSwitched %d\n",oldAccountData->GetStateCallbacks().size());
    if (oldAccountData->GetStateCallbacks().size() > 0) {
        HILOG_DEBUG("Send state to callbacks start.");
        for (auto &callback: GetCurrentAccountData()->GetStateCallbacks()) {
            callback->OnStateChanged(0);
        }
    }

    // Change to the new account.
    currentAccountId_ = accountId;
    sptr<AccessibilityAccountData> newAccountData = GetCurrentAccountData();
    if (!newAccountData) {
        HILOG_ERROR("Get newAccountData failed.");
        return;
    }

    newAccountData->ReadConfigurationForAccountData();
    UpdateAbilities();

    if (GetConnectCounter() >= 1 &&
        (!newAccountData->GetConnectedA11yAbilities().empty() ||
        !newAccountData->GetConnectingA11yAbilities().empty())) {
        HILOG_DEBUG("Post task PublicNoticeEvent start.");
        std::function<void()> PublicNoticeEventFunc =
            std::bind(&AccessibleAbilityManagerService::PublicNoticeEventInner, this);
        handler_->PostTask(PublicNoticeEventFunc, TASK_PUBLIC_NOTICE_EVENT);
    }
}

void AccessibleAbilityManagerService::PublicNoticeEventInner()
{
    HILOG_DEBUG("%{public}s start", __func__);
    sptr<AccessibilityEventInfo> event = new AccessibilityEventInfo();
    event->SetEventType(EventType::TYPE_PUBLIC_NOTICE_EVENT);

    // TODO: event add message

	struct timespec times = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &times);
	long timeStamp = times.tv_sec * 1000 + times.tv_nsec / 1000000;
    event->SetTimeStamp(timeStamp);

    HILOG_DEBUG("Post task SendEvent start.");
    std::function<void()> SendEventFunc =
        std::bind(&AccessibleAbilityManagerService::SendEventInner, this, *event);
    handler_->PostTask(SendEventFunc, TASK_SEND_PUBLIC_NOTICE_EVENT);
}

void AccessibleAbilityManagerService::UnlockedUser(int32_t accountId)
{
    HILOG_DEBUG("%{public}s start", __func__);

    if (accountId == GetCurrentAccountId()) {
        UpdateAbilities();
    }
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

void AccessibleAbilityManagerService::PackageRemoved(std::string &bundleName) {
    HILOG_DEBUG("%{public}s start", __func__);

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetEnabledAbilities().empty()) {
        return;
    }

    bool needUpdateAbility = false;
    std::map<std::string, AppExecFwk::ElementName> enabledAbilities = packageAccount->GetEnabledAbilities();
    for (auto &enableAbility : enabledAbilities) {
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

void AccessibleAbilityManagerService::PackageChanged() {
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

void AccessibleAbilityManagerService::PackageUpdateFinished(std::string &bundleName) {
    HILOG_DEBUG("%{public}s start", __func__);
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (packageAccount->GetConnectingA11yAbilities().empty()) {
        return;
    }

    bool needUpdateAbility = false;
    std::map<std::string, AppExecFwk::ElementName> connectingAbilities = packageAccount->GetConnectingA11yAbilities();
    for (auto &ability : connectingAbilities) {
        if (ability.second.GetBundleName() == bundleName) {
            packageAccount->RemoveConnectingA11yAbility(ability.second);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByAccessibilityEvent(const AccessibilityEventInfo &event) {
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("%{public}s start %{public}d", __func__, event.GetWindowId());
    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
        case TYPE_VIEW_FOCUSED_EVENT:
            AccessibilityWindowInfoManager::GetInstance().SetActiveWindow(event.GetWindowId());
            AccessibilityWindowInfoManager::GetInstance().SetAccessibilityFocusedWindow(event.GetWindowId());
            break;
        default:
            break;
    }
}

AccessibilityElementInfo AccessibleAbilityManagerService::FindFocusedElementInfo()
{
    HILOG_DEBUG("[%{public}s]", __func__);

    return AccessibilityInteractionBridge::GetInstance().FindFocusedElementInfo();
}

bool AccessibleAbilityManagerService::PerformActionOnAccessibilityFocusedItem(const ActionType& action)
{
    HILOG_DEBUG("[%{public}s]", __func__);

    return AccessibilityInteractionBridge::GetInstance().PerformActionOnAccessibilityFocusedItem(action);
}

bool AccessibleAbilityManagerService::GetAccessibilityFocusClickPointInScreen(MmiPoint &point)
{
    HILOG_DEBUG("[%{public}s]", __func__);

    return AccessibilityInteractionBridge::GetInstance().GetAccessibilityFocusClickPointInScreen(point);
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService() {
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

void AccessibleAbilityManagerService::UpdateAbilities() {
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    vector<AccessibilityAbilityInfo> installedAbilities = accountData->GetInstalledAbilities();
    for (auto &installAbility : installedAbilities) {
        AppExecFwk::ElementName element(installAbility.GetAbilityInfo().deviceId,
                                        installAbility.GetPackageName(),
                                        installAbility.GetName());
        std::string elementName =  element.GetURI();
        // wait for the connecting ability.
        if (accountData->GetConnectingA11yAbilities().count(elementName)) {
            continue;
        }

        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(elementName);
        if (accountData->GetEnabledAbilities().count(elementName)) {
            if (!connection) {
                connection = new AccessibleAbilityConnection(accountData, connectCounter_++, installAbility);
                connection->Connect();
            }
        } else {
            if (connection) {
                connection->Disconnect();
            }
        }
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityState() {
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

void AccessibleAbilityManagerService::UpdateInputFilter() {
    HILOG_DEBUG("%{public}s start.", __func__);

    bool notifyInputManager = false;

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    int flag = 0;
    if (accountData->GetScreenMagnificationFlag()) {
        flag |= AccessibilityInputFilter::FEATURE_SCREEN_MAGNIFICATION;
    }
    if (accountData->GetEventTouchGuideStateFlag()) {
        flag |= AccessibilityInputFilter::FEATURE_TOUCH_EXPLORATION;
    }
    if (accountData->GetFilteringKeyEventsFlag()) {
        flag |= AccessibilityInputFilter::FEATURE_FILTER_KEY_EVENTS;
    }
    if (accountData->GetGesturesSimulationFlag()) {
        flag |= AccessibilityInputFilter::FEATURE_INJECT_TOUCH_EVENTS;
    }
    HILOG_DEBUG("InputFilter flag is %{public}d", flag);

    if (flag != 0) {
        if (!hasInputFilter_) {
            HILOG_DEBUG("Has no InputFilter before.");
            hasInputFilter_ = true;
            if (inputFilter_ == nullptr) {
                inputFilter_ = new AccessibilityInputFilter();
            }
            notifyInputManager = true;
        }
        inputFilter_->SetUser(currentAccountId_);
        inputFilter_->SetAvailableFunctions(flag);
    } else {
        if (hasInputFilter_) {
            HILOG_DEBUG("Has InputFilter before.");
            hasInputFilter_ = false;
            inputFilter_->SetUser(currentAccountId_);
            inputFilter_->SetAvailableFunctions(0);
            notifyInputManager = true;
        }
    }

    if (notifyInputManager) {
        if (hasInputFilter_) {
            GetInputManager()->RegisterMultimodalInputFilter(*inputFilter_);
        } else {
            GetInputManager()->DeregisterMultimodalInputFilter();
        }
    }
}

void AccessibleAbilityManagerService::UpdateMagnification() {
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    std::vector<WMDisplayInfo> displays = AccessibilityDisplayManager::GetInstance().GetDisplays();

    if (accountData->GetScreenMagnificationFlag()) {
        for (WMDisplayInfo& display : displays) {
            AccessibilityZoomProxy::GetInstance().Register(display.id);
        }
    } else {
        for (WMDisplayInfo& display : displays) {
            AccessibilityZoomProxy::GetInstance().Unregister(display.id);
        }
    }

}

void AccessibleAbilityManagerService::UpdateWindowChangeListener() {
    HILOG_DEBUG("%{public}s start.", __func__);

    bool isWindowRetrieve = false;
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    std::map<std::string, sptr<AccessibleAbilityConnection>> connectedA11yAbilities = accountData->GetConnectedA11yAbilities();
    for (auto itr = connectedA11yAbilities.begin(); itr != connectedA11yAbilities.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE) {
            isWindowRetrieve = true;
            break;
        }
    }
    if (isWindowRetrieve) {
        AccessibilityWindowInfoManager::GetInstance().RegisterWindowChangeListener();
    } else {
        AccessibilityWindowInfoManager::GetInstance().DeregisterWindowChangeListener();
        AccessibilityWindowInfoManager::GetInstance().GetAccessibilityWindows().clear();
    }
}

}   // namespace OHOS
}   // Accessibility