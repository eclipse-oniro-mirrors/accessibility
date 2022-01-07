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

#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_errorcode.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceClientProxy::AccessibleAbilityManagerServiceClientProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerServiceClient>(impl)
{

}

AccessibleAbilityManagerServiceClientProxy::~AccessibleAbilityManagerServiceClientProxy()
{

}

bool AccessibleAbilityManagerServiceClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("%{public}s" , __func__);
    if (!data.WriteInterfaceToken(IAccessibleAbilityManagerServiceClient::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibleAbilityManagerServiceClientProxy::SendEvent(const AccessibilityEventInfo &uiEvent, const int userId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteParcelable(&uiEvent)) {
        HILOG_ERROR("%{public}s fail, connection write parcelable AccessibilityEventInfo error", __func__);
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("%{public}s fail, connection write userId error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %{public}d", error);
        return;
    }
}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState> &client, const int userId) 
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (client == nullptr) {
        HILOG_ERROR("%{public}s fail, Input client is null", __func__);
        return ErrCode::ERROR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token error", __func__);
        return ErrCode::ERROR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write client error", __func__);
        return ErrCode::ERROR;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("%{public}s fail, connection write userId error", __func__);
        return ErrCode::ERROR;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %{public}d", error);
        return ErrCode::ERROR;
    }

    return reply.ReadUint32();
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetAbilityList(const int abilityTypes,
    const int stateType)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AccessibilityAbilityInfo> errorList{};
    std::vector<AccessibilityAbilityInfo> abilityInfos{};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token error", __func__);
        return errorList;
    }

    if (!data.WriteInt32(abilityTypes)) {
        HILOG_ERROR("%{public}s fail, connection write abilityTypes error", __func__);
        return errorList;
    }

    if (!data.WriteInt32(stateType)) {
        HILOG_ERROR("%{public}s fail, connection write stateType error", __func__);
        return errorList;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %d", error);
        return errorList;
    }
    // read result
    int32_t abilityInfoSize = reply.ReadInt32();
    for (int i = 0; i < abilityInfoSize; i++) {
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityAbilityInfo(
                reply.ReadParcelable<AccessibilityAbilityInfo>());
        if (!accessibilityAbilityInfo) {
            HILOG_ERROR("ReadParcelable<accessibilityAbilityInfo> failed");
            return errorList;
        }
        abilityInfos.emplace_back(*accessibilityAbilityInfo);
    }
    return abilityInfos;
}

void AccessibleAbilityManagerServiceClientProxy::RegisterInteractionOperation(int windowId,
        const sptr <IAccessibilityInteractionOperation> &operation, const int userId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    
    if (operation == nullptr) {
        HILOG_ERROR("%{public}s fail, Input operation is null", __func__);
        return;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("%{public}s fail, connection write windowId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(operation->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write parcelable operation error", __func__);
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("%{public}s fail, connection write userId error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Register fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerServiceClientProxy::DeregisterInteractionOperation(const int windowId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("%{public}s fail, connection write userId error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Deregister fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerServiceClientProxy::Interrupt(const int userId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("%{public}s fail, connection write userId error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::INTERRUPT),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return;
    }
}

uint64_t AccessibleAbilityManagerServiceClientProxy::GetSuggestedInterval()
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return ErrCode::ERROR;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_SUGGESTED_INTERVAL),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return ErrCode::ERROR;
    }
    return reply.ReadInt64();
}

void AccessibleAbilityManagerServiceClientProxy::RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteRemoteObject(obj)) {
        HILOG_ERROR("%{public}s fail, connection write obj", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_ABILITY_CONNECTION_CLIENT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return;
    }
}

sptr<IRemoteObject> AccessibleAbilityManagerServiceClientProxy::GetObject()
{
    return this->AsObject();
}

} //namespace Accessibility
} //namespace OHOS
