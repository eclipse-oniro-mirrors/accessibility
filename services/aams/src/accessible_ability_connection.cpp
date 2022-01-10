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

#include <algorithm>
#include <map>
#include <vector>
#include "accessible_ability_connection.h"
#include "ability_manager_client.h"
#include "accessibility_window_manager.h"
#include "accessibility_interaction_connection.h"
#include "singleton.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "accessible_ability.h"
#include "dummy.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
   
AccessibleAbilityChannelStubImpl::AccessibleAbilityChannelStubImpl(
    AccessibleAbilityConnection& connection): connection_(connection) {
    aams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams_) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::AccessibleAbilityChannelStubImpl failed: no aams");
    }
    HILOG_DEBUG("AccessibleAbilityChannelStubImpl::AccessibleAbilityChannelStubImpl successfully");
}

AccessibleAbilityChannelStubImpl::~AccessibleAbilityChannelStubImpl() {

}
bool AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId(const int accessibilityWindowId, 
    const long elementId, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, 
    const int mode) 
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                       FOCUS_TYPE_INVALID);
    sptr<AccessibilityInteractionConnection> connection = 
        accountData->GetAccessibilityInteractionConnection(realWindowId);
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
    connection->GetProxy()->SearchElementInfoByAccessibilityId(elementId, requestId, callback, 
        mode);
    HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfoByAccessibilityId successfully");
    return true;
}

bool AccessibleAbilityChannelStubImpl::SearchElementInfosByText(const int accessibilityWindowId, 
    const long elementId, const string &text, const int requestId, 
    const sptr<IAccessibilityInteractionOperationCallback> &callback) {

    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                       FOCUS_TYPE_INVALID);

    sptr<AccessibilityInteractionConnection> connection = 
        accountData->GetAccessibilityInteractionConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("SearchElementInfosByText failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SearchElementInfosByText failed: no capability");
        return false;
    }
    connection->GetProxy()->SearchElementInfosByText(elementId, text, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::FindFocusedElementInfo(const int accessibilityWindowId, 
    const long elementId, const int focusType, const int requestId, 
    const sptr<IAccessibilityInteractionOperationCallback> &callback) {

    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                       focusType);

    sptr<AccessibilityInteractionConnection> connection = 
        accountData->GetAccessibilityInteractionConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("FindFocusedElementInfo failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::FindFocusedElementInfo failed: no capability");
        return false;
    }
    connection->GetProxy()->FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::FocusMoveSearch(const int accessibilityWindowId, const long elementId, 
    const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) {

    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                       FOCUS_TYPE_INVALID);

    sptr<AccessibilityInteractionConnection> connection = 
        accountData->GetAccessibilityInteractionConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("FocusMoveSearch failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::FocusMoveSearch failed: no capability");
        return false;
    }
    connection->GetProxy()->FocusMoveSearch(elementId, direction, requestId, callback);
    return true;
}

bool AccessibleAbilityChannelStubImpl::PerformAction(const int accessibilityWindowId, const long elementId, 
    const int action, std::map<std::string, std::string> &actionArguments, const int requestId, 
    const sptr<IAccessibilityInteractionOperationCallback> &callback) {
    HILOG_DEBUG("PerformAction accessibilityWindowId = %{public}d", accessibilityWindowId);
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    int realWindowId = AccessibilityWindowInfoManager::GetInstance().ConvertToRealWindowId(accessibilityWindowId,
                                                                                       FOCUS_TYPE_INVALID);

    sptr<AccessibilityInteractionConnection> connection = 
        accountData->GetAccessibilityInteractionConnection(realWindowId);
    if (!connection) {
        HILOG_ERROR("PerformAction failed");
        return false;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::PerformAction failed: no capability");
        return false;
    }
    connection->GetProxy()->PerformAction(elementId, action, actionArguments, requestId, callback);   
    return true;
}

vector<AccessibilityWindowInfo> AccessibleAbilityChannelStubImpl::GetWindows() {
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

bool AccessibleAbilityChannelStubImpl::PerformCommonAction(int action) {
    HILOG_DEBUG("PerformCommonAction action = %{public}d", action);
    bool ret = true;
    switch (action)
    {
    case GlobalAction::GLOBAL_ACTION_BACK:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_BACK");
        break;
    case GlobalAction::GLOBAL_ACTION_HOME:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_HOME");
        break;
    case GlobalAction::GLOBAL_ACTION_RECENT:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_RECENT");
        break;
    case GlobalAction::GLOBAL_ACTION_NOTIFICATION:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_NOTIFICATION");
        break;
    case GlobalAction::GLOBAL_ACTION_POP_UP_POWER_DIALOG:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_POP_UP_POWER_DIALOG");
        break;
    case GlobalAction::GLOBAL_ACTION_DIVIDE_SCREEN:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_DIVIDE_SCREEN");
        break;
    case GlobalAction::GLOBAL_ACTION_LOCK_SCREEN:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_LOCK_SCREEN");
        break;
    case GlobalAction::GLOBAL_ACTION_CAPTURE_SCREEN:
        HILOG_DEBUG("PerformCommonAction action = GLOBAL_ACTION_CAPTURE_SCREEN");
        break;
    default:
        HILOG_DEBUG("The action is not exist. %{public}d", action);
        ret = false;
        break;
    }

    // TODO: need external dependence
    return ret;
}

void AccessibleAbilityChannelStubImpl::DisableAbility()
{
    connection_.GetAccountData()->RemoveEnabledAbility(connection_.GetElementName());

#if 0
    aams_->UpdateAbilities();

#else
    connection_.GetProxy()->Disconnect(connection_.GetChannelId());
    connection_.OnAbilityDisconnectDone(connection_.GetElementName(), 0);

#endif // 0

}

void AccessibleAbilityChannelStubImpl::SetOnKeyPressEventResult(const bool handled, const int sequence) {
    sptr<KeyEventFilter> keyEventFilter = aams_->GetKeyEventFilter();
    if (!keyEventFilter) {
        return;
    }
    keyEventFilter->SetServiceOnKeyEventResult(connection_, handled, sequence);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeScale(const int displayId) {
    return AccessibilityZoomProxy::GetInstance().GetScale(displayId);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeCenterX(const int displayId) {
    return AccessibilityZoomProxy::GetInstance().GetCenterX(displayId);
}

float AccessibleAbilityChannelStubImpl::GetDisplayResizeCenterY(const int displayId) {
    return AccessibilityZoomProxy::GetInstance().GetCenterY(displayId);
}

Rect AccessibleAbilityChannelStubImpl::GetDisplayResizeRect(const int displayId) {
    return AccessibilityZoomProxy::GetInstance().GetDisplayResizeRect(displayId);
}

bool AccessibleAbilityChannelStubImpl::ResetDisplayResize(const int displayId, const bool animate) {
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::ResetDisplayResize failed: no capability");
        return false;
    }
    return AccessibilityZoomProxy::GetInstance().Reset(displayId);
}

bool AccessibleAbilityChannelStubImpl::SetDisplayResizeScaleAndCenter(const int displayId, const float scale, 
    const float centerX, const float centerY, const bool animate) {
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SetDisplayResizeScaleAndCenter failed: no capability");
        return false;
    }
    return AccessibilityZoomProxy::GetInstance().SetScaleAndCenter(displayId, scale, centerX, centerY);
}

void AccessibleAbilityChannelStubImpl::SendSimulateGesture(const int sequence, 
    const std::vector<GesturePathDefine> &gestureSteps) {
    sptr<IAccessibleAbilityClient> proxy = connection_.GetProxy();
    if (!proxy) {
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
        HILOG_ERROR("AccessibleAbilityChannelStubImpl::SendSimulateGesture failed: no capability");
        proxy->OnGestureSimulateResult(sequence, false);
        return;
    }
    sptr<TouchEventInjector> touchEventInjector = aams_->GetTouchEventInjector();
    if (!touchEventInjector) {
        proxy->OnGestureSimulateResult(sequence, false);
        return;
    }
    touchEventInjector->InjectEvents(gestureSteps, connection_.GetProxy(), sequence);
}

bool AccessibleAbilityChannelStubImpl::IsFingerprintGestureDetectionValid() {
    // if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_TOUCH_GUIDE)) {
    //     HILOG_ERROR("AccessibleAbilityChannelStubImpl::IsFingerprintGestureDetectionValid failed: no capability");
    //     return false;
    // }
    return false;
}

AccessibleAbilityConnection::AccessibleAbilityConnection(const wptr<AccessibilityAccountData> &accountData, 
    int connectionId, AccessibilityAbilityInfo &abilityInfo) {
    
    connectionId_ = connectionId;
    abilityInfo_ = abilityInfo;
    accountData_ = accountData;
}

AccessibleAbilityConnection::~AccessibleAbilityConnection() {
    HILOG_DEBUG(" %{public}s", __func__);
    if (proxy_ && proxy_->AsObject()) {
        proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
}

void AccessibleAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int resultCode) {
    elementName_ = element;

    if (resultCode != NO_ERROR) {
        HILOG_DEBUG("Connect failed!");
        accountData_->RemoveEnabledAbility(elementName_);
        accountData_->RemoveConnectingA11yAbility(elementName_);

        // TODO: Notify setting

        return;
    }

    if (!remoteObject) {
        HILOG_ERROR("AccessibleAbilityConnection::OnAbilityConnectDone get remoteObject failed");
        return;
    }

    if (!proxy_) {
        proxy_ = new AccessibleAbilityClientProxy(remoteObject);
    }

    if (!proxy_) {
        HILOG_ERROR("AccessibleAbilityConnection::OnAbilityConnectDone get AccessibleAbilityClientProxy failed");
        return;
    }
    HILOG_ERROR("AccessibleAbilityConnection::OnAbilityConnectDone get AccessibleAbilityClientProxy successfully");

    if (!deathRecipient_) {
        deathRecipient_ = new AccessibleAbilityConnectionDeathRecipient(accountData_, elementName_);
    }

    if (!proxy_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }

    sptr<AccessibleAbilityConnection> pointer = this;
    stub_ = new AccessibleAbilityChannelStubImpl(*pointer);
    proxy_->Init(stub_, connectionId_);

    accountData_->AddConnectedAbility(pointer);
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAccessibilityManagerService();
}

void AccessibleAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) {
    HILOG_DEBUG("%{public}s start.", __func__);

    if (resultCode != NO_ERROR) {
        HILOG_ERROR("Disconnect failed!");
        return;
    }

    sptr<AccessibleAbilityConnection> pointer = this;
    accountData_->RemoveConnectedAbility(pointer);

    int32_t currentAccountId = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->GetCurrentAccountId();
    if (accountData_->GetAccountId() == currentAccountId) {
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityConnection::OnAccessibilityEvent(AccessibilityEventInfo &eventInfo) {
    HILOG_INFO("OnAccessibilityEvent called");
    if (!proxy_) {
        HILOG_ERROR("OnAccessibilityEvent​ failed");
        return;
    }

    bool send = false;
    if (IsWhiteListEvent(eventInfo.GetEventType())) {
        HILOG_DEBUG("Get event type is whilte list!");
        send = true;
    } else {
        int windowId = eventInfo.GetWindowId();
        bool validWindow = AccessibilityWindowInfoManager::GetInstance().IsValidWindow(windowId);
        vector<std::string> filterBundleNames = abilityInfo_.GetFilterBundleNames();
        if (validWindow && IsWantedEvent(eventInfo.GetEventType()) && (filterBundleNames.empty() ||
            find(filterBundleNames.begin(), filterBundleNames.end(),
            eventInfo.GetBundleName()) != filterBundleNames.end())) {
            send = true;
        }
    }

    if (send) {
        eventInfo.SetChannelId(connectionId_);
        proxy_->OnAccessibilityEvent(eventInfo);
        HILOG_INFO("windowId[%{public}d] evtType[%{public}d] windowChangeType[%{public}d]",
            eventInfo.GetWindowId(), (int)eventInfo.GetEventType(), (int)eventInfo.GetWindowChangeTypes());
    }

    return;
}


bool AccessibleAbilityConnection::IsWhiteListEvent(EventType eventType) {
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
        case EventType::TYPE_WINDOW_UPDATE: {
            ret = true;
            break;
        }
        default:
            break;
    }
    return ret;
}

void AccessibleAbilityConnection::OnInterrupt() {
    if (!proxy_) {
        HILOG_ERROR("OnInterrupt failed");
        return;
    }
    proxy_->OnInterrupt();
}

void AccessibleAbilityConnection::OnGesture(const int gestureId) {
    if (!proxy_) {
        HILOG_ERROR("OnGesture failed");
        return;
    }
    proxy_->OnGesture(gestureId);
}

bool AccessibleAbilityConnection::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) {
    if (!proxy_) {
        HILOG_ERROR("OnKeyPressEvent failed");
        return false;
    }

    if (!(abilityInfo_.GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        HILOG_ERROR("Capability failed");
        return false;
    }

    if (DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->
            IsWantedKeyEvent(const_cast<MMI::KeyEvent&>(keyEvent))) {
        proxy_->OnKeyPressEvent(keyEvent, sequence);
        return true;
    }
    return false;
}

void AccessibleAbilityConnection::OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale, 
    const float centerX, const float centerY) {
    if (!proxy_) {
        HILOG_ERROR("OnDisplayResizeChanged​ failed");
        return;
    }
    proxy_->OnDisplayResizeChanged(displayId, rect, scale, centerX, centerY);
}

void AccessibleAbilityConnection::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) {
    if (!proxy_) {
        HILOG_ERROR("OnGestureSimulateResult failed");
        return;
    }
    proxy_->OnGestureSimulateResult(sequence, completedSuccessfully);
}

void AccessibleAbilityConnection::OnFingerprintGestureValidityChanged(const bool validity) {
    if (!proxy_) {
        HILOG_ERROR("OnFingerprintGestureValidityChanged failed");
        return;
    }
    proxy_->OnFingerprintGestureValidityChanged(validity);
}

void AccessibleAbilityConnection::OnFingerprintGesture(const int gesture) {
    if (!proxy_) {
        HILOG_ERROR("OnFingerprintGesture failed");
        return;
    }
    proxy_->OnFingerprintGesture(gesture);
}

bool AccessibleAbilityConnection::IsWantedEvent(int eventType) {
    uint32_t type = static_cast<uint32_t>(eventType);
    if ((type & abilityInfo_.GetEventTypes()) != type) {
        return false;
    }
    return true;
}

AAFwk::Want CreateWant(AppExecFwk::ElementName& element) {
    AAFwk::Want want;
    want.SetElement(element);

    return want;
}

void AccessibleAbilityConnection::Disconnect() {
    proxy_->Disconnect(connectionId_);

    // TODO: 
#if 1
    if (AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this) != ERR_OK) {
        HILOG_ERROR("Disconnect failed!");
        return;
    }
#else
    OnAbilityDisconnectDone(elementName_, 0);
#endif
}

void AccessibleAbilityConnection::Connect() {
    AAFwk::Want want = CreateWant(elementName_);
    if (AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, nullptr) != ERR_OK) {
        HILOG_ERROR("ConnectAbility failed!");
        // TODO: Remove this enabled ability from Setting
        return;
    }
    accountData_->AddConnectingA11yAbility(elementName_);
}

int AccessibleAbilityConnection::GetChannelId()
{
    HILOG_DEBUG(" %{public}s connectionId_ is %{public}d", __func__, connectionId_);
    return connectionId_;
}

void AccessibleAbilityConnection::AccessibleAbilityConnectionDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG(" %{public}s", __func__);

    if (!recipientAccountData_.promote()) {
        HILOG_ERROR("recipientAccountData_ is null.");
    }

    sptr<AccessibleAbilityConnection> connection = recipientAccountData_->
        GetAccessibleAbilityConnection(recipientElementName_.GetURI());
    recipientAccountData_->RemoveConnectedAbility(connection);
    recipientAccountData_->RemoveEnabledAbility(recipientElementName_);
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAccessibilityManagerService();

    // TODO: notify setting

}

} // namespace Accessibility
} // namespace OHOS