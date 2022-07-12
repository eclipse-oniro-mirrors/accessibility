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

#include <hitrace_meter.h>

#include "ability_manager_client.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TASK_ON_ABILITY_CONNECT_DONE = "OnAbilityConnectDone";
    const std::string TASK_ON_ABILITY_DISCONNECT_DONE = "OnAbilityDisconnectDone";
} // namespace

std::mutex AccessibleAbilityConnection::mutex_;

AccessibleAbilityConnection::AccessibleAbilityConnection(const sptr<AccessibilityAccountData> &accountData,
    int32_t connectionId, AccessibilityAbilityInfo &abilityInfo)
    : connectionId_(connectionId), abilityInfo_(abilityInfo), accountData_(accountData)
{
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

AccessibleAbilityConnection::~AccessibleAbilityConnection()
{
    HILOG_DEBUG();
    if (abilityClient_ && abilityClient_->AsObject()) {
        abilityClient_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        abilityClient_ = nullptr;
    }
}

void AccessibleAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    std::function<void()> abilityConnectDoneFunc = std::bind(&AccessibleAbilityConnection::InnerOnAbilityConnectDone,
        this, element, remoteObject, resultCode);
    eventHandler_->PostTask(abilityConnectDoneFunc, TASK_ON_ABILITY_CONNECT_DONE);
}

void AccessibleAbilityConnection::InnerOnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("ResultCode is %{public}d", resultCode);
    FinishAsyncTrace(HITRACE_TAG_ACCESSIBILITY_MANAGER, "AccessibleAbilityConnect",
        static_cast<int32_t>(TraceTaskId::ACCESSIBLE_ABILITY_CONNECT));
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    if (!accountData_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("accountData_ is nullptr.");
        return;
    }
    elementName_ = element;

    if (resultCode != NO_ERROR) {
        accountData_->RemoveEnabledAbility(Utils::GetUri(elementName_));
        accountData_->RemoveConnectingA11yAbility(Utils::GetUri(elementName_));
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAbilities();
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        return;
    }

    if (!remoteObject) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("AccessibleAbilityConnection::OnAbilityConnectDone get remoteObject failed");
        return;
    }

    if (!abilityClient_) {
        abilityClient_ = new(std::nothrow) AccessibleAbilityClientProxy(remoteObject);
        if (!abilityClient_) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            HILOG_ERROR("abilityClient_ is null");
            return;
        }
    }

    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) AccessibleAbilityConnectionDeathRecipient(
            accountData_, elementName_, eventHandler_);
        if (!deathRecipient_) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            HILOG_ERROR("deathRecipient_ is null");
            return;
        }
    }

    if (!abilityClient_->AsObject() || !abilityClient_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }

    sptr<AccessibleAbilityConnection> pointer = this;
    accountData_->AddConnectedAbility(pointer);
    accountData_->RemoveConnectingA11yAbility(Utils::GetUri(elementName_));
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();

    channel_ = new(std::nothrow) AccessibleAbilityChannel(*pointer);
    if (!channel_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("channel_ is null");
        return;
    }
    abilityClient_->Init(channel_, connectionId_);
}

void AccessibleAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    HILOG_DEBUG();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    std::function<void()> abilityDisonnectDoneFunc = std::bind(
        &AccessibleAbilityConnection::InnerOnAbilityDisconnectDone, this, element, resultCode);
    eventHandler_->PostTask(abilityDisonnectDoneFunc, TASK_ON_ABILITY_DISCONNECT_DONE);
}

void AccessibleAbilityConnection::InnerOnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    HILOG_DEBUG();
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

    if (accountData_->GetAccountId() ==
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId()) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    }

    if (!abilityClient_) {
        HILOG_ERROR("abilityClient_ is nullptr");
        return;
    }
    abilityClient_->Disconnect(connectionId_);
}

void AccessibleAbilityConnection::OnAccessibilityEvent(AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG();
    if (!abilityClient_) {
        HILOG_ERROR("OnAccessibilityEvent​ failed");
        return;
    }

    bool send = false;
    if (IsAllowedListEvent(eventInfo.GetEventType())) {
        HILOG_DEBUG("EventType is in the allowed list!");
        send = true;
    } else {
        std::vector<std::string> filterBundleNames = abilityInfo_.GetFilterBundleNames();
        if (IsWantedEvent(eventInfo.GetEventType()) && (filterBundleNames.empty() ||
            find(filterBundleNames.begin(), filterBundleNames.end(),
                eventInfo.GetBundleName()) != filterBundleNames.end())) {
            send = true;
        }
    }

    if (send) {
        eventInfo.SetChannelId(connectionId_);
        abilityClient_->OnAccessibilityEvent(eventInfo);
        HILOG_INFO("windowId[%{public}d] evtType[%{public}d] windowChangeType[%{public}d] GestureId[%{public}d]",
            eventInfo.GetWindowId(), eventInfo.GetEventType(), eventInfo.GetWindowChangeTypes(),
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

bool AccessibleAbilityConnection::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
    if (!abilityClient_) {
        HILOG_ERROR("OnKeyPressEvent failed");
        return false;
    }

    if (!(abilityInfo_.GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        HILOG_ERROR("Capability failed");
        return false;
    }

    abilityClient_->OnKeyPressEvent(keyEvent, sequence);
    return true;
}

void AccessibleAbilityConnection::OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully)
{
    if (!abilityClient_) {
        HILOG_ERROR("OnGestureInjectResult failed");
        return;
    }
    abilityClient_->OnGestureInjectResult(sequence, completedSuccessfully);
}

void AccessibleAbilityConnection::SetAbilityInfoEventTypeFilter(const uint32_t eventTypes)
{
    HILOG_DEBUG();
    abilityInfo_.SetEventTypes(eventTypes);
}

void AccessibleAbilityConnection::SetAbilityInfoTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    abilityInfo_.SetFilterBundleNames(targetBundleNames);
}

bool AccessibleAbilityConnection::IsWantedEvent(int32_t eventType)
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
    HILOG_DEBUG();
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
    HILOG_DEBUG();
    StartAsyncTrace(HITRACE_TAG_ACCESSIBILITY_MANAGER, "AccessibleAbilityConnect",
        static_cast<int32_t>(TraceTaskId::ACCESSIBLE_ABILITY_CONNECT));
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    if (!accountData_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("accountData_ is nullptr");
        return;
    }
    elementName_ = element;
    int32_t accountId = accountData_->GetAccountId();
    HILOG_DEBUG("bundleName[%{public}s], abilityName [%{public}s], accountId [%{public}d]",
        bundleName.c_str(), abilityName.c_str(), accountId);

    auto bundleMgr = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bundleMgr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("get bundleMgr failed");
        return;
    }
    int uid = bundleMgr->GetUidByBundleName(bundleName, accountId);
    HILOG_DEBUG("uid is %{public}d ", uid);

    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (!abilityManagerClient) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("abilityManagerClient is nullptr");
        return;
    }
    AAFwk::Want want = CreateWant(elementName_);
    if (abilityManagerClient->ConnectAbility(want, this, nullptr, uid / UID_MASK) != ERR_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("ConnectAbility failed!");
        return;
    }
    accountData_->AddConnectingA11yAbility(Utils::GetUri(bundleName, abilityName));
}

int32_t AccessibleAbilityConnection::GetChannelId()
{
    HILOG_DEBUG("connectionId_ is %{public}d", connectionId_);
    return connectionId_;
}

void AccessibleAbilityConnection::AccessibleAbilityConnectionDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_A11Y_APPLICATION_DISCONNECT_ABNORMALLY,
        recipientElementName_.GetBundleName(), recipientElementName_.GetAbilityName());
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG();
        if (!recipientAccountData_) {
            HILOG_ERROR("recipientAccountData_ is null.");
            return;
        }

        std::string uri = Utils::GetUri(recipientElementName_);
        sptr<AccessibleAbilityConnection> connection = recipientAccountData_->GetAccessibleAbilityConnection(uri);
        if (!connection) {
            HILOG_ERROR("There is no connection for %{public}s.", uri.c_str());
            return;
        }
        recipientAccountData_->RemoveConnectedAbility(connection);
        recipientAccountData_->RemoveEnabledAbility(Utils::GetUri(recipientElementName_));

        std::string uiTestUri = Utils::GetUri("ohos.uitest", "uitestability");
        if (uri == uiTestUri) {
            recipientAccountData_->RemoveInstalledAbility("ohos.uitest");
        }

        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        aams.UpdateAbilities();
        aams.UpdateAccessibilityManagerService();
        }), "OnRemoteDied");
}
} // namespace Accessibility
} // namespace OHOS