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

#include "accessible_ability_connection.h"

#include <algorithm>
#include <map>
#include <new>
#include <vector>

#include "ability_manager_client.h"
#include "accessibility_window_connection.h"
#include "accessibility_window_manager.h"
#include "accessibility_extension_context.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "system_ability_definition.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
std::mutex AccessibleAbilityConnection::mutex_;
AccessibleAbilityChannelStubImpl::AccessibleAbilityChannelStubImpl(
    AccessibleAbilityConnection& connection): connection_(connection)
{
}

bool AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
    const long elementId, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int mode)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                           FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId failed: no connection");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId failed: no capability");
        return false;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId failed: no proxy");
        return false;
    }
    connection->GetProxy()->SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
    HILOG_DEBUG("AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId successfully");
    return true;
}

bool AccessibleAbilityChannelStubImpl::SearchElementInfosByText(const int accessibilityWindowId,
    const long elementId, const string &text, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                           FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("SearchElementInfosByText failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfosByText failed: no capability");
        return false;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("get proxy failed");
        return false;
    }
    connection->GetProxy()->SearchElementInfosByText(elementId, text, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::FindFocusedElementInfo(const int accessibilityWindowId,
    const long elementId, const int focusType, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                           focusType);

    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("FindFocusedElementInfo failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::FindFocusedElementInfo failed: no capability");
        return false;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("get proxy failed");
        return false;
    }
    connection->GetProxy()->FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::FocusMoveSearch(const int accessibilityWindowId, const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                           FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("FocusMoveSearch failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::FocusMoveSearch failed: no capability");
        return false;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("get proxy failed");
        return false;
    }
    connection->GetProxy()->FocusMoveSearch(elementId, direction, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::ExecuteAction(const int accessibilityWindowId, const long elementId,
    const int action, std::map<std::string, std::string> &actionArguments, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("ExecuteAction accessibilityWindowId = %{public}d", accessibilityWindowId);
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                           FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("ExecuteAction failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::ExecuteAction failed: no capability");
        return false;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("get proxy failed");
        return false;
    }
    connection->GetProxy()->ExecuteAction(elementId, action, actionArguments, requestId, callback);
    return true;
}

vector<AccessibilityWindowInfo> AccessibleAbilityChannelStubImpl::GetWindows()
{
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::GetWindows failed: no capability");
        vector<AccessibilityWindowInfo> windows;
        return windows;
    }

    std::vector<AccessibilityWindowInfo> windowInfos =
        AccessibilityWindowInfoManager::GetInstance().GetAccessibilityWindows();
    size_t size = windowInfos.size();
    int currentChannelId = connection_.GetChannelId();
    for (size_t i = 0; i < size; i++) {
        windowInfos[i].SetChannelId(currentChannelId);
    }
    return windowInfos;
}

bool AccessibleAbilityChannelStubImpl::ExecuteCommonAction(int action)
{
    HILOG_DEBUG("ExecuteCommonAction action = %{public}d", action);
    bool ret = true;
    switch (action) {
        case GlobalAction::GLOBAL_ACTION_BACK:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_BACK");
            break;
        case GlobalAction::GLOBAL_ACTION_HOME:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_HOME");
            break;
        case GlobalAction::GLOBAL_ACTION_RECENT:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_RECENT");
            break;
        case GlobalAction::GLOBAL_ACTION_NOTIFICATION:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_NOTIFICATION");
            break;
        case GlobalAction::GLOBAL_ACTION_POP_UP_POWER_DIALOG:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_POP_UP_POWER_DIALOG");
            break;
        case GlobalAction::GLOBAL_ACTION_DIVIDE_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_DIVIDE_SCREEN");
            break;
        case GlobalAction::GLOBAL_ACTION_LOCK_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_LOCK_SCREEN");
            break;
        case GlobalAction::GLOBAL_ACTION_CAPTURE_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_CAPTURE_SCREEN");
            break;
        default:
            HILOG_DEBUG("The action is not exist. %{public}d", action);
            ret = false;
            break;
    }

    // temp deal: need external dependence
    return ret;
}

void AccessibleAbilityChannelStubImpl::SetOnKeyPressEventResult(const bool handled, const int sequence)
{
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    sptr<KeyEventFilter> keyEventFilter = aams->GetKeyEventFilter();
    if (!keyEventFilter) {
        return;
    }
    keyEventFilter->SetServiceOnKeyEventResult(connection_, handled, sequence);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeScale(const int displayId)
{
    return AccessibilityZoomProxy::GetInstance().GetScale(displayId);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeCenterX(const int displayId)
{
    return AccessibilityZoomProxy::GetInstance().GetCenterX(displayId);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeCenterY(const int displayId)
{
    return AccessibilityZoomProxy::GetInstance().GetCenterY(displayId);
}

Rect AccessibleAbilityChannelStubImpl::GetDisplayResizeRect(const int displayId)
{
    return AccessibilityZoomProxy::GetInstance().GetDisplayResizeRect(displayId);
}

bool AccessibleAbilityChannelStubImpl::ResetDisplayResize(const int displayId, const bool animate)
{
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::ResetDisplayResize failed: no capability");
        return false;
    }
    return AccessibilityZoomProxy::GetInstance().Reset(displayId);
}

bool AccessibleAbilityChannelStubImpl::SetDisplayResizeScaleAndCenter(const int displayId, const float scale,
    const float centerX, const float centerY, const bool animate)
{
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SetDisplayResizeScaleAndCenter failed: no capability");
        return false;
    }
    return AccessibilityZoomProxy::GetInstance().SetScaleAndCenter(displayId, scale, centerX, centerY);
}

void AccessibleAbilityChannelStubImpl::SendSimulateGesture(const int requestId,
    const std::vector<GesturePathDefine> &gestureSteps)
{
    sptr<IAccessibleAbilityClient> proxy = connection_.GetProxy();
    if (!proxy) {
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SendSimulateGesture failed: no capability");
        proxy->OnGestureSimulateResult(requestId, false);
        return;
    }
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    sptr<TouchEventInjector> touchEventInjector = aams->GetTouchEventInjector();
    if (!touchEventInjector) {
        proxy->OnGestureSimulateResult(requestId, false);
        return;
    }
    touchEventInjector->InjectEvents(gestureSteps, connection_.GetProxy(), requestId);
}

AccessibleAbilityConnection::AccessibleAbilityConnection(const sptr<AccessibilityAccountData> &accountData,
    int connectionId, AccessibilityAbilityInfo &abilityInfo)
    : connectionId_(connectionId), abilityInfo_(abilityInfo), accountData_(accountData)
{
}

AccessibleAbilityConnection::~AccessibleAbilityConnection()
{
    HILOG_DEBUG("start");
    if (proxy_ && proxy_->AsObject()) {
        proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
}

void AccessibleAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOG_DEBUG("Start. ResultCode is %{public}d", resultCode);
    if (!accountData_) {
        HILOG_ERROR("accountData_ is nullptr.");
        return;
    }
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    elementName_ = element;

    if (resultCode != NO_ERROR) {
        accountData_->RemoveEnabledAbility(elementName_);
        accountData_->RemoveConnectingA11yAbility(elementName_);
        aams->UpdateAbilities();
        // temp deal: Notify setting
        return;
    }

    if (!remoteObject) {
        HILOG_ERROR("AccessibleAbilityConnection::OnAbilityConnectDone get remoteObject failed");
        return;
    }

    if (!proxy_) {
        proxy_ = new(std::nothrow) AccessibleAbilityClientProxy(remoteObject);
        if (!proxy_) {
            HILOG_ERROR("proxy_ is null");
            return;
        }
    }

    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) AccessibleAbilityConnectionDeathRecipient(accountData_, elementName_);
        if (!deathRecipient_) {
            HILOG_ERROR("deathRecipient_ is null");
            return;
        }
    }

    if (!proxy_->AsObject() || !proxy_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }

    sptr<AccessibleAbilityConnection> pointer = this;
    accountData_->AddConnectedAbility(pointer);
    accountData_->RemoveConnectingA11yAbility(elementName_);
    aams->UpdateAccessibilityManagerService();

    stub_ = new(std::nothrow) AccessibleAbilityChannelStubImpl(*pointer);
    if (!stub_) {
        HILOG_ERROR("stub_ is null");
        return;
    }
    proxy_->Init(stub_, connectionId_);
}

void AccessibleAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_DEBUG("start.");
    if (!proxy_) {
        HILOG_ERROR("proxy_ is nullptr");
        return;
    }
    proxy_->Disconnect(connectionId_);

    if (resultCode != NO_ERROR) {
        HILOG_ERROR("Disconnect failed!");
        return;
    }

    if (!accountData_) {
        HILOG_ERROR("accountData_ is nullptr.");
        return;
    }
    sptr<AccessibleAbilityConnection> pointer = this;
    accountData_->RemoveConnectedAbility(pointer);
    accountData_->RemoveEnabledAbility(element);

    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    aams->UpdateAbilities();

    if (accountData_->GetAccountId() == aams->GetCurrentAccountId()) {
        aams->UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityConnection::OnAccessibilityEvent(AccessibilityEventInfo &eventInfo)
{
    HILOG_INFO("OnAccessibilityEvent called");
    if (!proxy_) {
        HILOG_ERROR("OnAccessibilityEvent​ failed");
        return;
    }

    bool send = false;
    if (IsAllowedListEvent(eventInfo.GetEventType())) {
        HILOG_DEBUG("EventType is in the allowed list!");
        send = true;
    } else {
        vector<std::string> filterBundleNames = abilityInfo_.GetFilterBundleNames();
        if (IsWantedEvent(eventInfo.GetEventType()) && (filterBundleNames.empty() ||
            find(filterBundleNames.begin(), filterBundleNames.end(),
                eventInfo.GetBundleName()) != filterBundleNames.end())) {
            send = true;
        }
    }

    if (send) {
        eventInfo.SetChannelId(connectionId_);
        proxy_->OnAccessibilityEvent(eventInfo);
        HILOG_INFO("windowId[%{public}d] evtType[%{public}d] windowChangeType[%{public}d] GestureId[%{public}d]",
            eventInfo.GetWindowId(), (int)eventInfo.GetEventType(), (int)eventInfo.GetWindowChangeTypes(),
            eventInfo.GetGestureType());
    }

    return;
}

bool AccessibleAbilityConnection::IsAllowedListEvent(EventType eventType)
{
    bool ret = false;
    switch (eventType) {
        case EventType::TYPE_PAGE_STATE_UPDATE:
        case EventType::TYPE_NOTIFICATION_UPDATE_EVENT:
        case EventType::TYPE_PUBLIC_NOTICE_EVENT:
        case EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN:
        case EventType::TYPE_TOUCH_GUIDE_GESTURE_END:
        case EventType::TYPE_TOUCH_GUIDE_BEGIN:
        case EventType::TYPE_TOUCH_GUIDE_END:
        case EventType::TYPE_TOUCH_BEGIN:
        case EventType::TYPE_TOUCH_END:
        case EventType::TYPE_VIEW_HOVER_ENTER_EVENT:
        case EventType::TYPE_VIEW_HOVER_EXIT_EVENT:
        case EventType::TYPE_INTERRUPT_EVENT:
        case EventType::TYPE_GESTURE_EVENT:
        case EventType::TYPE_WINDOW_UPDATE: {
            ret = true;
            break;
        }
        default:
            break;
    }
    return ret;
}

bool AccessibleAbilityConnection::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{
    if (!proxy_) {
        HILOG_ERROR("OnKeyPressEvent failed");
        return false;
    }

    if (!(abilityInfo_.GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        HILOG_ERROR("Capability failed");
        return false;
    }

    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return false;
    }
    if (aams->IsWantedKeyEvent(const_cast<MMI::KeyEvent&>(keyEvent))) {
        proxy_->OnKeyPressEvent(keyEvent, sequence);
        return true;
    }
    return false;
}

void AccessibleAbilityConnection::OnDisplayResized(const int displayId, const Rect &rect, const float scale,
    const float centerX, const float centerY)
{
    if (!proxy_) {
        HILOG_ERROR("OnDisplayResized​ failed");
        return;
    }
    proxy_->OnDisplayResized(displayId, rect, scale, centerX, centerY);
}

void AccessibleAbilityConnection::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{
    if (!proxy_) {
        HILOG_ERROR("OnGestureSimulateResult failed");
        return;
    }
    proxy_->OnGestureSimulateResult(sequence, completedSuccessfully);
}

bool AccessibleAbilityConnection::IsWantedEvent(int eventType)
{
    uint32_t type = static_cast<uint32_t>(eventType);
    if ((type & abilityInfo_.GetEventTypes()) != type) {
        HILOG_DEBUG("EventType(%{public}d) is not wanted", type);
        return false;
    }
    return true;
}

AAFwk::Want CreateWant(AppExecFwk::ElementName& element)
{
    AAFwk::Want want;
    want.SetElement(element);

    return want;
}

void AccessibleAbilityConnection::Disconnect()
{
    HILOG_DEBUG("start");
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (!abilityManagerClient) {
        HILOG_ERROR("abilityManagerClient is nullptr");
        return;
    }
    if (abilityManagerClient->DisconnectAbility(this) != ERR_OK) {
        HILOG_ERROR("Disconnect failed!");
        return;
    }
}

void AccessibleAbilityConnection::Connect(const AppExecFwk::ElementName &element)
{
    HILOG_DEBUG("start");
    if (!accountData_) {
        HILOG_ERROR("accountData_ is nullptr");
        return;
    }
    elementName_ = element;
    AAFwk::Want want = CreateWant(elementName_);
    HILOG_DEBUG("GetBundleName is %{public}s ", elementName_.GetBundleName().c_str());
    HILOG_DEBUG("GetAbilityName is %{public}s ", elementName_.GetAbilityName().c_str());
    HILOG_DEBUG("current accountId is %{public}d ", accountData_->GetAccountId());

    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams || !aams->GetBundleMgrProxy()) {
        HILOG_ERROR("get bundleMgr failed");
        return;
    }
    int uid = aams->GetBundleMgrProxy()->GetUidByBundleName(
        elementName_.GetBundleName(), accountData_->GetAccountId());
    HILOG_DEBUG("uid is %{public}d ", uid);

    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (!abilityManagerClient) {
        HILOG_ERROR("abilityManagerClient is nullptr");
        return;
    }
    if (abilityManagerClient->ConnectAbility(
        want, this, nullptr, uid / UID_MASK) != ERR_OK) {
        HILOG_ERROR("ConnectAbility failed!");
        // temp deal: Remove this enabled ability from Setting
        return;
    }
    accountData_->AddConnectingA11yAbility(elementName_);
}

int AccessibleAbilityConnection::GetChannelId()
{
    HILOG_DEBUG("connectionId_ is %{public}d", connectionId_);
    return connectionId_;
}

void AccessibleAbilityConnection::AccessibleAbilityConnectionDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!recipientAccountData_) {
        HILOG_ERROR("recipientAccountData_ is null.");
        return;
    }

    sptr<AccessibleAbilityConnection> connection =
        recipientAccountData_->GetAccessibleAbilityConnection(recipientElementName_.GetURI());
    if (!connection) {
        HILOG_ERROR("There is no connection for %{public}s.", recipientElementName_.GetURI().c_str());
        return;
    }
    recipientAccountData_->RemoveConnectedAbility(connection);
    recipientAccountData_->RemoveEnabledAbility(recipientElementName_);

    std::string uiTestUri = "/ohos.uitest/uitestability";
    if (recipientElementName_.GetURI() == uiTestUri) {
        recipientAccountData_->RemoveInstalledAbility("ohos.uitest");
    }

    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    aams->UpdateAbilities();
    aams->UpdateAccessibilityManagerService();
    // temp deal: notify setting
}
} // namespace Accessibility
} // namespace OHOS