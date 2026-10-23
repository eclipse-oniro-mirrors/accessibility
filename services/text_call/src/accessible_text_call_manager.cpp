/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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

#include "accessible_text_call_manager.h"

#include "extension_manager_client.h"
#include "hilog_wrapper.h"
#include <hisysevent.h>
#include "ipc_skeleton.h"

namespace OHOS {
namespace Accessibility {

namespace {
    const std::string_view AI_CALL_SERVICE_BUNDLE_NAME = "com.ohos.callaiservice";
    const std::string_view AI_CALL_SERVICE_ABILITY_NAME = "CallAIServiceExtensionAbility";
    constexpr int32_t DEFAULT_VALUE_MINUS_ONE = -1;
    constexpr uint32_t REQUEST_ID_SET_CALL_STATE = 3001;
    constexpr int32_t TIMEOUT_1000MS = 1000;
    const std::u16string AI_CALL_SERVICE = u"idl.IIdlCallAIService";
}

#ifdef OHOS_BUILD_TEXT_CALL
int32_t CallServiceListener::OnCallDetailsChange(const Telephony::CallAttributeInfo &info)
{
    HILOG_INFO("OnCallDetailsChange callType=%{public}d callState=%{public}d", info.callType, info.callState);
    if (info.callType != Telephony::CallType::TYPE_VOIP) {
        return ERR_OK;
    }
    std::ostringstream oss;
    oss << "text call details change: callType: " << static_cast<int32_t>(info.callType)
        << ", callState: " << static_cast<int32_t>(info.callState) << ";";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "STARTING_FUNCTION",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "MSG", oss.str());
    if (ret != 0) {
        HILOG_ERROR("Write TEXT_CALL_DETAILS_CHANGE HiSysEvent error, ret:%{public}d", ret);
    }
    OHOS::Singleton<OHOS::Accessibility::AccessibleTextCallManager>::GetInstance().SendRequest(info.callState);
    return ERR_OK;
}
#endif // OHOS_BUILD_TEXT_CALL

AccessibleTextCallManager::AccessibleTextCallManager()
{

}

AccessibleTextCallManager::~AccessibleTextCallManager()
{
}

void AccessibleTextCallManager::SetServiceState(bool state)
{
    HILOG_INFO("SetServiceState %{public}d", state);
    if (state) {
        RegisterCallServiceListener();
    } else {
        sptr<AICallServiceConnection> connToDisconnect;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            DeregisterCallServiceListener();
            connToDisconnect = serviceConn_;
            serviceConn_ = nullptr;
            serviceState_ = ServiceState::DISCONNECTED;
        }
        if (connToDisconnect) {
            AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(connToDisconnect);
        }
    }
}

bool AccessibleTextCallManager::RegisterCallServiceListener()
{
#ifdef OHOS_BUILD_TEXT_CALL
    auto callManager = DelayedSingleton<OHOS::Telephony::CallManagerClient>::GetInstance();
    if (!callManager) {
        HILOG_ERROR("callManager is nullptr");
        return false;
    }
    callManager->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    int ret = callManager->RegisterCallBack(std::make_unique<CallServiceListener>());
    HILOG_INFO("RegisterCallBack ret=%{public}d", ret);
    return ret == 0;
#else
    return true;
#endif // OHOS_BUILD_TEXT_CALL
}

void AccessibleTextCallManager::DeregisterCallServiceListener()
{
#ifdef OHOS_BUILD_TEXT_CALL
    auto callManager = DelayedSingleton<OHOS::Telephony::CallManagerClient>::GetInstance();
    if (!callManager) {
        return;
    }
    callManager->UnRegisterCallBack();
    callManager->UnInit();
#endif // OHOS_BUILD_TEXT_CALL
}

bool AccessibleTextCallManager::ConnectAICallService()
{
    if (serviceState_ != ServiceState::DISCONNECTED) {
        return true;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    serviceConn_ = new (std::nothrow) AICallServiceConnection();
    if (!serviceConn_) {
        IPCSkeleton::SetCallingIdentity(identity);
        std::ostringstream oss;
        oss << "AI call service connect failed: ConnectServiceExtensionAbility ret: " << ret << ";";
        int32_t hisyseventRet = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
            "UNAVAILABLE",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "MSG", oss.str());
        if (hisyseventRet != 0) {
            HILOG_ERROR("Write AI_CALL_SERVICE_CONNECT HiSysEvent error, ret:%{public}d", hisyseventRet);
        }
        return false;
    }
    AAFwk::Want want;
    want.SetElementName(std::string(AI_CALL_SERVICE_BUNDLE_NAME), std::string(AI_CALL_SERVICE_ABILITY_NAME));
    int ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
        serviceConn_, nullptr, DEFAULT_VALUE_MINUS_ONE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        HILOG_ERROR("ConnectServiceExtensionAbility failed, ret=%{public}d", ret);
        serviceConn_ = nullptr;
        return false;
    }
    serviceState_ = ServiceState::CONNECTING;
    return true;
}

void AccessibleTextCallManager::OnServiceConnectDone()
{
    std::lock_guard<std::mutex> lock(mutex_);
    serviceState_ = ServiceState::CONNECTED;
}

void AccessibleTextCallManager::OnServiceDisconnectDone()
{
    std::lock_guard<std::mutex> lock(mutex_);
    serviceState_ = ServiceState::DISCONNECTED;
    serviceConn_ = nullptr;
}

#ifdef OHOS_BUILD_TEXT_CALL
bool AccessibleTextCallManager::SendRequest(Telephony::TelCallState callState)
{
    sptr<AICallServiceConnection> conn;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (serviceState_ == ServiceState::DISCONNECTED) {
            bool connectResult = ConnectAICallService();
            if (!connectResult) {
                HILOG_ERROR("connect AI call service failed");
                return false;
            }
        }
        if (!serviceConn_) {
            HILOG_ERROR("serviceConn_ is nullptr");
            return false;
        }
        conn = serviceConn_;
    }

    sptr<IRemoteObject> remote = conn->GetRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("cannot get remote object");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(AI_CALL_SERVICE)) {
        HILOG_ERROR("write message failed!");
        return false;
    }
    if (!data.WriteInt32(static_cast<int32_t>(callState))) {
        HILOG_ERROR("connection write request id failed");
        return false;
    }

    int ret = remote->SendRequest(REQUEST_ID_SET_CALL_STATE, data, reply, option);
    HILOG_INFO("sendMessageRequest ret=%{public}d", ret);
    std::ostringstream oss;
    oss << "AI call service send request failed: " << ret << ";";
    int32_t hisyseventRet = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "UNAVAILABLE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "MSG", oss.str());
    if (hisyseventRet != 0) {
        HILOG_ERROR("Write SendRequest HiSysEvent error, ret:%{public}d", hisyseventRet);
    }
    return ret == ERR_OK;
}
#endif // OHOS_BUILD_TEXT_CALL

void AICallServiceConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_INFO("on ability connected done");
    if (resultCode != ERR_OK) {
        HILOG_ERROR("ability connect failed: %{public}d", resultCode);
        OHOS::Singleton<OHOS::Accessibility::AccessibleTextCallManager>::GetInstance().OnServiceDisconnectDone();
        return;
    }
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is null");
        OHOS::Singleton<OHOS::Accessibility::AccessibleTextCallManager>::GetInstance().OnServiceDisconnectDone();
        return;
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        remoteObject_ = remoteObject;
    }
    OHOS::Singleton<OHOS::Accessibility::AccessibleTextCallManager>::GetInstance().OnServiceConnectDone();
    cv_.notify_all();
}

void AICallServiceConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    HILOG_INFO("on ability disconnected done");
    {
        std::unique_lock<std::mutex> lock(mutex_);
        remoteObject_ = nullptr;
    }
    OHOS::Singleton<OHOS::Accessibility::AccessibleTextCallManager>::GetInstance().OnServiceDisconnectDone();
}

sptr<IRemoteObject> AICallServiceConnection::GetRemoteObject()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.wait_for(lock, std::chrono::milliseconds(TIMEOUT_1000MS), [this]() { return remoteObject_ != nullptr; })) {
        HILOG_ERROR("wait for remote object timeout");
        std::ostringstream oss;
        oss << "get remote object timeout;";
        int32_t ret = HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
            "UNAVAILABLE",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "MSG", oss.str());
        if (ret != 0) {
            HILOG_ERROR("Write GET_REMOTE_OBJECT HiSysEvent error, ret:%{public}d", ret);
        }
        return nullptr;
    }
    return remoteObject_;
}
} // namespace Accessibility
} // namespace OHOS
