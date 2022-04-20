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

#include "accessible_ability_manager_service.h"

#include <new>
#include <unistd.h>
#include <functional>

#include "ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "bundle_mgr_proxy.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "input_manager.h"
#include "json_utils.h"
#include "os_account_info.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
static const std::string TASK_SEND_EVENT = "SendEvent";
static const std::string TASK_REGISTER_STATE_OBSERVER = "RegisterStateObserver";
static const std::string TASK_REGISTER_CAPTION_OBSERVER = "RegisterCaptionObserver";
static const std::string TASK_GET_ABILITY_LIST = "GetAbilityList";
static const std::string TASK_REGISTER_ELEMENT_OPERATOR = "RegisterElementOperator";
static const std::string TASK_DEREGISTER_ELEMENT_OPERATOR = "DeregisterElementOperator";
static const std::string TASK_PUBLIC_NOTICE_EVENT = "PublicNoticeEvent";
static const std::string TASK_SEND_PUBLIC_NOTICE_EVENT = "SendPublicNoticeEvent";
static const std::string TASK_GET_CAPTION_PROPERTY = "GetCaptionProperty";
static const std::string TASK_SET_CAPTION_PROPERTY = "SetCaptionProperty";
static const std::string TASK_SET_CAPTION_STATE = "SetCaptionState";
static const std::string TASK_GET_ENABLE_STATE = "GetEnableState";
static const std::string TASK_GET_CAPTION_STATE = "GetCaptionState";
static const std::string TASK_GET_TOUCH_GUIDE_STATE = "GetTouchGuideState";
static const std::string TASK_GET_GESTURE_STATE = "GetGestureState";
static const std::string TASK_GET_KEY_EVENT_OBSERVER_STATE = "GetKeyEventObserverState";
static const std::string TASK_ENABLE_ABILITIES = "EnableAbilities";
static const std::string TASK_GET_ENABLE_ABILITIES = "GetEnableAbilities";
static const std::string TASK_GET_INSTALLED_ABILITIES = "GetInstalledAbilities";
static const std::string TASK_DISABLE_ABILITIES = "DisableAbilities";
static const std::string TASK_ENABLE_UI_TEST_ABILITIES = "EnableUiTestAbilities";
static const std::string TASK_DISABLE_UI_TEST_ABILITIES = "DisableUiTestAbilities";
static const std::string TASK_GET_ACTIVE_WINDOW = "GetActiveWindow";

static const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
static const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
static const std::string UI_TEST_ABILITY_NAME = "uitestability";

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<AccessibleAbilityManagerService>::GetInstance().get());
std::mutex AccessibleAbilityManagerService::mutex_;
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

    commonEventRegistry_->UnRegister();
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

    if (!commonEventRegistry_) {
        commonEventRegistry_ = std::make_shared<AccessibilityCommonEventRegistry>(handler_);
        if (!commonEventRegistry_) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed:create CommonEventRegistry failed");
            return false;
        }
    }

    // temp deal: set current account Id to TEMP_ACCOUNT_ID.
    // This is a temporary countermeasure, after which a formal countermeasure is required.
    currentAccountId_ = TEMP_ACCOUNT_ID;
    HILOG_DEBUG("current accountId %{public}d", currentAccountId_);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return false;
    }
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

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t accountId)
{
    HILOG_DEBUG("start");
    HILOG_INFO("eventType[%{public}d]", uiEvent.GetEventType());
    HILOG_INFO("gestureId[%{public}d]", uiEvent.GetGestureType());
    if (accountId != currentAccountId_) {
        HILOG_ERROR("accountId[%{public}d] is not matched", accountId);
        return;
    }
    AccessibilityEventInfo &event = const_cast<AccessibilityEventInfo&>(uiEvent);
    UpdateAccessibilityWindowStateByEvent(event);

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }
    std::function<void()> sendEventFunc = std::bind(&AccessibleAbilityManagerService::InnerSendEvent, this, event);
    handler_->PostTask(sendEventFunc, TASK_SEND_EVENT);
}

void AccessibleAbilityManagerService::InnerSendEvent(AccessibilityEventInfo &uiEvent)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto& ability : abilities) {
        if (ability.second) {
            ability.second->OnAccessibilityEvent(uiEvent);
        }
    }
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback, const int32_t accountId)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return ERR_INVALID_VALUE;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerRegisterCaptionObserver,
        this, std::ref(syncPromise), callback, accountId);

    handler_->PostTask(task, TASK_REGISTER_CAPTION_OBSERVER);
    uint32_t ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerRegisterCaptionObserver(std::promise<uint32_t> &syncPromise,
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback, const int32_t accountId)
{
    HILOG_DEBUG("start");
    if (accountId != currentAccountId_) {
        HILOG_ERROR("accountId[%{public}d] is not matched", accountId);
        syncPromise.set_value(ERR_INVALID_VALUE);
        return;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        syncPromise.set_value(ERR_INVALID_VALUE);
        return;
    }

    if (!captionPropertyCallbackDeathRecipient_) {
        captionPropertyCallbackDeathRecipient_ = new(std::nothrow) CaptionPropertyCallbackDeathRecipient();
        if (!captionPropertyCallbackDeathRecipient_) {
            HILOG_ERROR("captionPropertyCallbackDeathRecipient_ is null");
            syncPromise.set_value(ERR_INVALID_VALUE);
            return;
        }
    }

    if (!callback) {
        HILOG_ERROR("callback is null");
        syncPromise.set_value(0);
        return;
    }
    if (!callback->AsObject()) {
        HILOG_ERROR("object is null");
        syncPromise.set_value(0);
        return;
    }
    callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
    accountData->AddCaptionPropertyCallback(callback);
    HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
        accountData->GetCaptionPropertyCallbacks().size());
    syncPromise.set_value(NO_ERROR);
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &callback, const int32_t accountId)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return 0;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerRegisterStateObserver,
        this, std::ref(syncPromise), callback, accountId);

    handler_->PostTask(task, TASK_REGISTER_STATE_OBSERVER);
    uint32_t ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerRegisterStateObserver(std::promise<uint32_t> &syncPromise,
    const sptr<IAccessibleAbilityManagerStateObserver> &callback, const int32_t accountId)
{
    HILOG_DEBUG("start");
    if (accountId != currentAccountId_) {
        HILOG_ERROR("accountId[%{public}d] is not matched", accountId);
        syncPromise.set_value(0);
        return;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        syncPromise.set_value(0);
        return;
    }

    if (!stateCallbackDeathRecipient_) {
        stateCallbackDeathRecipient_ = new(std::nothrow) StateCallbackDeathRecipient();
        if (!stateCallbackDeathRecipient_) {
            HILOG_ERROR("stateCallbackDeathRecipient_ is null");
            syncPromise.set_value(0);
            return;
        }
    }

    if (!callback) {
        HILOG_ERROR("callback is null");
        syncPromise.set_value(0);
        return;
    }
    if (!callback->AsObject()) {
        HILOG_ERROR("object is null");
        syncPromise.set_value(0);
        return;
    }
    callback->AsObject()->AddDeathRecipient(stateCallbackDeathRecipient_);
    accountData->AddStateCallback(callback);
    HILOG_INFO("AccessibleAbilityManagerService::RegisterStateObserver successfully");

    uint32_t state = accountData->GetAccessibilityState();
    syncPromise.set_value(state);
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerService::GetAbilityList(
    const uint32_t abilityTypes, const int32_t stateType)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        std::vector<AccessibilityAbilityInfo> temp;
        return temp;
    }

    std::promise<vector<AccessibilityAbilityInfo>> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetAbilityList,
        this, std::ref(syncPromise), abilityTypes, stateType);

    handler_->PostTask(task, TASK_GET_ABILITY_LIST);
    vector<AccessibilityAbilityInfo> ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetAbilityList(
    std::promise<std::vector<AccessibilityAbilityInfo>> &syncPromise,
    const uint32_t abilityTypes, const int32_t stateType)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    vector<AccessibilityAbilityInfo> infoList;
    if ((stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("stateType is out of range!!");
        syncPromise.set_value(infoList);
        return;
    }

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get current account data failed!!");
        syncPromise.set_value(infoList);
        return;
    }

    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    vector<AccessibilityAbilityInfo> abilities = accountData->GetAbilitiesByState(state);
    HILOG_DEBUG("abilityes count is %{public}zu", abilities.size());
    for (auto& ability : abilities) {
        if (abilityTypes == AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL ||
           (ability.GetAccessibilityAbilityType() & abilityTypes)) {
            infoList.push_back(ability);
        }
    }
    HILOG_DEBUG("infoList count is %{public}zu", infoList.size());
    syncPromise.set_value(infoList);
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation, int32_t accountId)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerRegisterElementOperator,
        this, windowId, operation, accountId);

    handler_->PostTask(task, TASK_REGISTER_ELEMENT_OPERATOR);
}

void AccessibleAbilityManagerService::InnerRegisterElementOperator(const int32_t windowId,
    const sptr<IAccessibilityElementOperator> operation, const int32_t accountId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData->GetAccessibilityWindowConnection(windowId)) {
        HILOG_WARN("This operation already exists, do not register twice!!");
        return;
    }

    sptr<AccessibilityWindowConnection> connection = new(std::nothrow) AccessibilityWindowConnection(windowId,
        operation, accountId);
    if (!connection) {
        HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
        return;
    }
    accountData->AddAccessibilityWindowConnection(windowId, connection);

    if (!interactionOperationDeathRecipient_) {
        interactionOperationDeathRecipient_ = new(std::nothrow) InteractionOperationDeathRecipient(windowId);
        if (!interactionOperationDeathRecipient_) {
            HILOG_ERROR("interactionOperationDeathRecipient_ is null");
            return;
        }
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

void AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerDeregisterElementOperator,
        this, windowId);

    handler_->PostTask(task, TASK_DEREGISTER_ELEMENT_OPERATOR);
}

void AccessibleAbilityManagerService::InnerDeregisterElementOperator(const int32_t windowId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
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
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    aams->DeregisterElementOperator(windowId_);
}

CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        CaptionProperty temp;
        return temp;
    }

    std::promise<CaptionProperty> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetCaptionProperty,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_CAPTION_PROPERTY);
    CaptionProperty ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetCaptionProperty(std::promise<CaptionProperty> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    CaptionProperty property = accountData->GetCaptionProperty();

    syncPromise.set_value(property);
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerSetCaptionProperty,
        this, std::ref(syncPromise), caption);

    handler_->PostTask(task, TASK_SET_CAPTION_PROPERTY);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerSetCaptionProperty(std::promise<bool> &syncPromise,
    const CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    bool result = accountData->SetCaptionProperty(caption);
    UpdateCaptionProperty();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerSetCaptionState,
        this, std::ref(syncPromise), state);

    handler_->PostTask(task, TASK_SET_CAPTION_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerSetCaptionState(std::promise<bool> &syncPromise, const bool state)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    bool result = accountData->SetCaptionState(state);
    UpdateAccessibilityState();
    syncPromise.set_value(result);
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    HILOG_DEBUG("currentAccoutId is %{public}d ", currentAccountId_);
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(currentAccountId_);
    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }
    a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    return accountData;
}

bool AccessibleAbilityManagerService::IsWantedKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG("start");

    int32_t keyCode = event.GetKeyCode();
    if (keyCode == MMI::KeyEvent::KEYCODE_VOLUME_UP || keyCode == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
        return true;
    }
    return false;
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(
    int32_t displayId, Rect& rect, float scale, float centerX, float centerY)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
    for (auto& ability : abilities) {
        if (ability.second) {
            ability.second->OnDisplayResized(displayId, rect, scale, centerX, centerY);
        }
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
    if (!bundleManager_) {
        HILOG_ERROR("fail to new bundle manager.");
    } else {
        HILOG_INFO("AccessibleAbilityManagerService::GetBundleMgrProxy OK");
    }
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(
    int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void AccessibleAbilityManagerService::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

void AccessibleAbilityManagerService::OnChanging(bool selfChange, Uri &uri)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    // temp deal: Do something for Setting
    UpdateAbilities();
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG("start");
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG("start");
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveStateCallback(remote);
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveCaptionPropertyCallback(remote);
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);
    a11yAccountsData_.erase(accountId);
}

void AccessibleAbilityManagerService::PresentUser()
{
    HILOG_DEBUG();

    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->ReadConfigurationForAccountData();
    UpdateAbilities();
}

void AccessibleAbilityManagerService::PackageRemoved(std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr.");
        return;
    }
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    packageAccount->RemoveInstalledAbility(bundleName);

    // Remove enabled ability, remove connecting ability if it is connecting.
    bool needUpdateAbility = false;
    std::vector<std::string> enabledAbilities = packageAccount->GetEnabledAbilities();
    if (enabledAbilities.empty()) {
        HILOG_DEBUG("There is no enabled abilities.");
        return;
    }
    for (auto& enableAbility : enabledAbilities) {
        if (enableAbility == bundleName) {
            packageAccount->RemoveEnabledAbility(enableAbility);
            packageAccount->RemoveConnectingA11yAbility(enableAbility);
            needUpdateAbility = true;
        }
    }

    // Remove connected ability
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectedAbilities =
        packageAccount->GetConnectedA11yAbilities();
    if (connectedAbilities.empty()) {
        HILOG_DEBUG("There is no connected abilities.");
        return;
    }
    for (auto &connectedAbility : connectedAbilities) {
        std::size_t firstPos = connectedAbility.first.find_first_of('/') + 1;
        std::size_t endPos = connectedAbility.first.find_last_of('/');
        if (endPos <= firstPos) {
            HILOG_ERROR("it's a wrong ability and the uri is %{public}s", connectedAbility.first.c_str());
            continue;
        }
        std::string connectedBundleName = connectedAbility.first.substr(firstPos, endPos - firstPos);
        if (connectedBundleName == bundleName) {
            HILOG_DEBUG("Remove connected ability and it's bundle name is %{public}s", connectedBundleName.c_str());
            packageAccount->RemoveConnectedAbility(connectedAbility.second);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAbilities();
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageAdd(std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    bool hasNewExtensionAbility = false;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        HILOG_ERROR("bundleMgrProxy is nullptr.");
        return;
    }
    bundleMgrProxy->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &newAbility : extensionInfos) {
        if (newAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package added is an extension ability and\
                extension ability's name is %{public}s", newAbility.name.c_str());
            AccessibilityAbilityInitParams initParams;
            AccessibilityConfigParse::Parse(newAbility, initParams);
            std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
                std::make_shared<AccessibilityAbilityInfo>(initParams);
            if (!accessibilityInfo) {
                HILOG_ERROR("accessibilityInfo is nullptr");
                return;
            }
            packageAccount->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("add new extension ability successfully and installed abilities's size is %{public}zu",
                packageAccount->GetInstalledAbilities().size());
            hasNewExtensionAbility = true;
            break;
        }
    }

    if (hasNewExtensionAbility) {
        HILOG_DEBUG("add new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageChanged(std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    bool hasChanged = false;
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    packageAccount->RemoveInstalledAbility(bundleName);

    // Add installed ability
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        HILOG_ERROR("bundleMgrProxy is nullptr.");
        return;
    }
    bundleMgrProxy->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &changedAbility : extensionInfos) {
        if (changedAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package changed is an extension ability and\
                extension ability's name is %{public}s", changedAbility.name.c_str());
            AccessibilityAbilityInitParams initParams;
            AccessibilityConfigParse::Parse(changedAbility, initParams);
            std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
                std::make_shared<AccessibilityAbilityInfo>(initParams);
            packageAccount->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("update new extension ability successfully and installed abilities's size is %{public}zu",
                packageAccount->GetInstalledAbilities().size());
            hasChanged = true;
            break;
        }
    }

    if (hasChanged) {
        HILOG_DEBUG("update new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageUpdateFinished(std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    if (packageAccount->GetConnectingA11yAbilities().empty()) {
        return;
    }

    bool needUpdateAbility = false;
    std::vector<std::string> connectingAbilities = packageAccount->GetConnectingA11yAbilities();
    for (auto &ability : connectingAbilities) {
        if (ability == bundleName) {
            packageAccount->RemoveConnectingA11yAbility(ability);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("windowId is %{public}d", event.GetWindowId());
    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
            AccessibilityWindowManager::GetInstance().SetActiveWindow(event.GetWindowId());
            AccessibilityWindowManager::GetInstance().SetAccessibilityFocusedWindow(event.GetWindowId());
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
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities.size());
    for (auto &installAbility : installedAbilities) {
        std::string deviceId;
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(), abilityName.c_str());

        auto connectingAbilities = accountData->GetConnectingA11yAbilities();
        vector<string>::iterator iter = std::find(connectingAbilities.begin(), connectingAbilities.end(), bundleName);
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
                connection = new(std::nothrow) AccessibleAbilityConnection(accountData,
                    connectCounter_++, installAbility);
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
    for (auto &callback : accountData->GetStateCallbacks()) {
        if (callback) {
            callback->OnStateChanged(state);
        }
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
    for (auto &callback : accountData->GetCaptionPropertyCallbacks()) {
        if (callback) {
            callback->OnPropertyChanged(caption);
        }
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

    uint32_t flag = 0;
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

    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
    if (!inputInterceptor_) {
        HILOG_ERROR("inputInterceptor_ is null.");
        return;
    }
    inputInterceptor_->SetAvailableFunctions(flag);
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
        for (sptr<Rosen::Display> &displayReg : displays) {
            if (displayReg) {
                AccessibilityZoomProxy::GetInstance().Register(displayReg->GetId());
            }
        }
    } else {
        for (sptr<Rosen::Display> &displayUnreg : displays) {
            if (displayUnreg) {
                AccessibilityZoomProxy::GetInstance().Unregister(displayUnreg->GetId());
            }
        }
    }
}

void AccessibleAbilityManagerService::UpdateWindowChangeListener()
{
    HILOG_DEBUG("start.");

    bool isWindowRetrieve = true; // This is a temp deal for test
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    auto connectedA11yAbilities = accountData->GetConnectedA11yAbilities();
    for (auto itr = connectedA11yAbilities.begin(); itr != connectedA11yAbilities.end(); itr++) {
        if (!itr->second) {
            HILOG_ERROR("ability is null");
            continue;
        }
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE) {
            isWindowRetrieve = true;
            break;
        }
    }
    if (isWindowRetrieve) {
        HILOG_INFO("RegisterWindowChangeListener.");
        AccessibilityWindowManager::GetInstance().RegisterWindowChangeListener();
    } else {
        HILOG_INFO("DeregisterWindowChangeListener.");
        AccessibilityWindowManager::GetInstance().DeregisterWindowChangeListener();
        AccessibilityWindowManager::GetInstance().GetAccessibilityWindows().clear();
    }
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetEnabledState,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_ENABLE_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetEnabledState(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->GetEnabledState();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetCaptionState,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_CAPTION_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetCaptionState(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->GetCaptionState();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetTouchGuideState,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_TOUCH_GUIDE_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetTouchGuideState(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->GetTouchGuideState();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetGestureState,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_GESTURE_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetGestureState(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->GetGestureState();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetKeyEventObserverState,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_KEY_EVENT_OBSERVER_STATE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetKeyEventObserverState(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    bool result = accountData->GetKeyEventObserverState();
    syncPromise.set_value(result);
}

bool AccessibleAbilityManagerService::EnableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerEnableAbilities,
        this, std::ref(syncPromise), abilities);

    handler_->PostTask(task, TASK_ENABLE_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerEnableAbilities(std::promise<bool> &syncPromise,
    std::vector<std::string> &abilities)
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
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetEnabledAbilities,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_ENABLE_ABILITIES);
    std::vector<std::string> ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetEnabledAbilities(
    std::promise<std::vector<std::string>> &syncPromise)
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
    std::promise<std::vector<AccessibilityAbilityInfo>> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetInstalledAbilities,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_INSTALLED_ABILITIES);
    std::vector<AccessibilityAbilityInfo> ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetInstalledAbilities(
    std::promise<std::vector<AccessibilityAbilityInfo>> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    std::vector<AccessibilityAbilityInfo> it {};
    it = accountData->GetInstalledAbilities();
    syncPromise.set_value(it);
}

bool AccessibleAbilityManagerService::DisableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerDisableAbilities,
        this, std::ref(syncPromise), abilities);

    handler_->PostTask(task, TASK_DISABLE_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerDisableAbilities(std::promise<bool> &syncPromise,
    std::vector<std::string> &abilities)
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

bool AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerEnableUITestAbility,
        this, std::ref(syncPromise), obj);

    handler_->PostTask(task, TASK_ENABLE_UI_TEST_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerEnableUITestAbility(std::promise<bool> &syncPromise,
    const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    std::string uiTestUri = "/" + UI_TEST_BUNDLE_NAME + "/" + UI_TEST_ABILITY_NAME;
    sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
    if (connection) {
        HILOG_ERROR("connection is existed!!");
        syncPromise.set_value(false);
        return;
    }

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }
    std::function<void()> addUITestClientFunc =
        std::bind(&AccessibleAbilityManagerService::AddUITestClient, this, obj);
    handler_->PostTask(addUITestClientFunc, "AddUITestClient");
    syncPromise.set_value(true);
}

void AccessibleAbilityManagerService::AddUITestClient(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG("start");
    auto currentAccountData = GetCurrentAccountData();
    if (!currentAccountData) {
        HILOG_ERROR("currentAccountData is nullptr");
        return;
    }

    // Add installed ability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    if (!abilityInfo) {
        HILOG_ERROR("abilityInfo is null");
        return;
    }
    abilityInfo->SetPackageName(UI_TEST_BUNDLE_NAME);
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetAccessibilityAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    currentAccountData->AddInstalledAbility(*abilityInfo);

    // Add connected ability
    sptr<AppExecFwk::ElementName> elementName = new(std::nothrow) AppExecFwk::ElementName();
    if (!elementName) {
        HILOG_ERROR("elementName is null");
        return;
    }
    elementName->SetBundleName(UI_TEST_BUNDLE_NAME);
    elementName->SetAbilityName(UI_TEST_ABILITY_NAME);
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(
        currentAccountData, connectCounter_++, *abilityInfo);
    if (!connection) {
        HILOG_ERROR("connection is null");
        return;
    }
    connection->OnAbilityConnectDoneSync(*elementName, obj, 0);
}

bool AccessibleAbilityManagerService::DisableUITestAbility()
{
    HILOG_DEBUG("start");
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerDisableUITestAbility,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_DISABLE_UI_TEST_ABILITIES);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerDisableUITestAbility(std::promise<bool> &syncPromise)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    std::string uiTestUri = "/" + UI_TEST_BUNDLE_NAME + "/" + UI_TEST_ABILITY_NAME;
    sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
    if (!connection) {
        HILOG_ERROR("connection is not existed!!");
        syncPromise.set_value(false);
        return;
    }
    std::function<void()> removeUITestClientFunc =
        std::bind(&AccessibleAbilityManagerService::RemoveUITestClient, this, connection);
    handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
    syncPromise.set_value(true);
}

void AccessibleAbilityManagerService::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG("start");
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    accountData->RemoveInstalledAbility(UI_TEST_BUNDLE_NAME);
    connection->OnAbilityDisconnectDoneSync(connection->GetElementName(), 0);
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    HILOG_DEBUG("start");
    std::promise<int32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityManagerService::InnerGetActiveWindow,
        this, std::ref(syncPromise));

    handler_->PostTask(task, TASK_GET_ACTIVE_WINDOW);
    int32_t ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityManagerService::InnerGetActiveWindow(std::promise<int32_t> &syncPromise)
{
    HILOG_DEBUG("start");
    int32_t ret = AccessibilityWindowManager::GetInstance().activeWindowId_;
    syncPromise.set_value(ret);
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d added!", systemAbilityId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    switch (systemAbilityId) {
        case COMMON_EVENT_SERVICE_ID:
            // Register common event
            if (!commonEventRegistry_->StartRegister()) {
                HILOG_ERROR("AccessibleAbilityManagerService::Init failed:Failed to subscribe common event");
            }
            break;
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            // Get installed accessibility extension ability from BMS
            if (accountData->GetInstalledAbilitiesFromBMS()) {
                UpdateAbilities();
                UpdateAccessibilityManagerService();
            } else {
                HILOG_ERROR("Get installed ExtensionAbility failed");
            }
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            AccessibilityWindowManager::GetInstance().RegisterWindowChangeListener();
            break;
        default:
            break;
    }
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d removed!", systemAbilityId);
}
} // namespace Accessibility
} // namespace OHOS