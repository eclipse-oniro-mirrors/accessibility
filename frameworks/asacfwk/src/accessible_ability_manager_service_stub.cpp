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
#include <memory>
#include "accessible_ability_manager_service_stub.h"

#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {

AccessibleAbilityManagerServiceClientStub::AccessibleAbilityManagerServiceClientStub()
{
    HILOG_DEBUG("%{public}s" , __func__);

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSendEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterStateCallback;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetAbilityList;
    memberFuncMap_[static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterAccessibilityInteractionOperation;
    memberFuncMap_[static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceClientStub::HandleDeregisterAccessibilityInteractionOperation;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::INTERRUPT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleInterrupt;
    memberFuncMap_[static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::GET_SUGGESTED_INTERVAL)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetSuggestedInterval;
    memberFuncMap_[static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::REGISTER_ABILITY_CONNECTION_CLIENT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterAbilityConnectionClientTmp;
}

AccessibleAbilityManagerServiceClientStub::~AccessibleAbilityManagerServiceClientStub()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int AccessibleAbilityManagerServiceClientStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
 MessageParcel &reply, MessageOption &option)
 {
    HILOG_DEBUG("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerServiceClientStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();

    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    
    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data,reply);
        }
    }
    HILOG_WARN("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSendEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);
    
    std::shared_ptr<AccessibilityEventInfo> uiEvent(data.ReadParcelable<AccessibilityEventInfo>());
    if (!uiEvent) {
        HILOG_DEBUG("ReadParcelable<AbilityInfo> failed");
        return ERROR;
    }
    int userId = data.ReadInt32();
    SendEvent(*uiEvent, userId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterStateCallback(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    sptr <IRemoteObject> obj = data.ReadRemoteObject();
    sptr <IAccessibleAbilityManagerServiceState> client = iface_cast<IAccessibleAbilityManagerServiceState>(obj);
    int userId = data.ReadInt32();
    uint64_t result = RegisterStateCallback(client, userId);
    reply.WriteUint64(result);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetAbilityList(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int abilityTypes = data.ReadInt32();
    int stateType = data.ReadInt32();
    std::vector<AccessibilityAbilityInfo> abilityInfos{};
    abilityInfos = GetAbilityList(abilityTypes, stateType);

    int32_t abilityInfoSize = abilityInfos.size();
    reply.WriteInt32(abilityInfoSize);
    for (auto &abilityInfo : abilityInfos) {
        if (!reply.WriteParcelable(&abilityInfo)) {
            HILOG_ERROR("ReadParcelable<accessibilityAbilityInfo> failed");
            return ERROR;
        }
    }
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterAccessibilityInteractionOperation(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int windowId = data.ReadInt32();
    sptr <IRemoteObject> obj = data.ReadRemoteObject();
    sptr <IAccessibilityInteractionOperation> operation = iface_cast<IAccessibilityInteractionOperation>(obj);
    int userId = data.ReadInt32(); 
    RegisterInteractionOperation(windowId, operation, userId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleDeregisterAccessibilityInteractionOperation(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);
    
    int windowId = data.ReadInt32();
    DeregisterInteractionOperation(windowId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleInterrupt(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int userId = data.ReadInt32(); 
    Interrupt(userId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetSuggestedInterval(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    uint64_t result = GetSuggestedInterval();

    reply.WriteUint64(result);
    
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterAbilityConnectionClientTmp(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    sptr <IRemoteObject> obj = data.ReadRemoteObject();

    RegisterAbilityConnectionClientTmp(obj);
    
    return ErrCode::NO_ERROR;
}

} //namespace Accessibility
} //namespace OHOS