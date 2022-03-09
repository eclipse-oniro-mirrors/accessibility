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

#include "accessible_ability_manager_service_stub.h"
#include "accessibility_caption.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceClientStub::AccessibleAbilityManagerServiceClientStub()
{
    HILOG_DEBUG("start");

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSendEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterStateCallback;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetAbilityList;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterAccessibilityElementOperator;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceClientStub::HandleDeregisterAccessibilityElementOperator;

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_PROPERTY)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetCaptionProperty;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_PROPERTY)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetCaptionProperty;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetCaptionState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetEnabled;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetEnabled;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetCaptionState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_TOUCH_GUIDE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetTouchGuideState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_GESTURE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetGestureState;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::GET_KEY_EVENT_OBSERVE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetKeyEventObserverState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_TOUCH_GUIDE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetTouchGuideState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_GESTURE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetGestureState;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::SET_KEY_EVENT_OBSERVE_STATE)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetKeyEventObserverState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED_OBJECT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleSetEnabledObj;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED_OBJECT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetEnabledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_INSTALLED)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetInstalledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DISABLE_ABILITIES)] =
        &AccessibleAbilityManagerServiceClientStub::HandleDisableAbilities;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::REGISTER_CAPTION_PROPERTY_CALLBACK)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterCaptionPropertyCallback;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::REGISTER_UITEST_ABILITY_CONNECTION_CLIENT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleRegisterUITestAbilityConnectionClient;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_UITEST_ABILITY_CONNECTION_CLIENT)] =
        &AccessibleAbilityManagerServiceClientStub::HandleDeregisterUITestAbilityConnectionClient;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerServiceClient::Message::GET_ACTIVE_WINDOW)] =
        &AccessibleAbilityManagerServiceClientStub::HandleGetActiveWindow;
}

AccessibleAbilityManagerServiceClientStub::~AccessibleAbilityManagerServiceClientStub()
{
    HILOG_DEBUG("start.");
    memberFuncMap_.clear();
}

int AccessibleAbilityManagerServiceClientStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    HILOG_DEBUG("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
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
            return (this->*func)(data, reply);
        }
    }
    HILOG_WARN("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSendEvent(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    std::shared_ptr<AccessibilityEventInfo> uiEvent(data.ReadParcelable<AccessibilityEventInfo>());
    if (!uiEvent) {
        HILOG_DEBUG("ReadParcelable<AbilityInfo> failed");
        return ERROR;
    }
    int userId = data.ReadInt32();
    SendEvent(*uiEvent, userId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterStateCallback(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerServiceState> client = iface_cast<IAccessibleAbilityManagerServiceState>(obj);
    int userId = data.ReadInt32();
    uint64_t result = RegisterStateCallback(client, userId);
    reply.WriteUint64(result);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetAbilityList(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    int abilityTypes = data.ReadInt32();
    int stateType = data.ReadInt32();
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    abilityInfos = GetAbilityList(abilityTypes, stateType);

    int32_t abilityInfoSize = abilityInfos.size();
    reply.WriteInt32(abilityInfoSize);
    for (auto& abilityInfo : abilityInfos) {
        if (!reply.WriteParcelable(&abilityInfo)) {
            HILOG_ERROR("ReadParcelable<accessibilityAbilityInfo> failed");
            return ERROR;
        }
    }
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterAccessibilityElementOperator(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    int windowId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperator> operation = iface_cast<IAccessibilityElementOperator>(obj);
    int userId = data.ReadInt32();
    RegisterElementOperator(windowId, operation, userId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleDeregisterAccessibilityElementOperator(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    int windowId = data.ReadInt32();
    DeregisterElementOperator(windowId);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetCaptionProperty(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    CaptionProperty caption = GetCaptionProperty();

    reply.WriteParcelable(&caption);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetCaptionProperty(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    std::shared_ptr<CaptionProperty> caption(data.ReadParcelable<CaptionProperty>());
    if (!caption) {
        HILOG_DEBUG("ReadParcelable<CaptionProperty> failed");
        return ERROR;
    }
    SetCaptionProperty(*caption);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetCaptionState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetCaptionState(state);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetEnabled(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetEnabled(state);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterCaptionPropertyCallback(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerServiceCaptionProperty> client =
        iface_cast<IAccessibleAbilityManagerServiceCaptionProperty>(obj);
    int userId = data.ReadInt32();
    uint64_t result = RegisterCaptionPropertyCallback(client, userId);
    reply.WriteUint64(result);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetEnabled(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetEnabledState();
    reply.WriteBool(result);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetCaptionState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetCaptionState();
    reply.WriteBool(result);
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetTouchGuideState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetTouchGuideState();
    reply.WriteBool(result);
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetGestureState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetGestureState();
    reply.WriteBool(result);
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetKeyEventObserverState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetKeyEventObserverState();
    reply.WriteBool(result);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetTouchGuideState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetTouchGuideState(state);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetGestureState(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetGestureState(state);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetKeyEventObserverState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetKeyEventObserverState(state);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleSetEnabledObj(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");
    std::map<std::string, AppExecFwk::ElementName> it {};
    int dev_num = data.ReadInt32();
    if (!dev_num) {
        HILOG_DEBUG("ReadParcelable failed, dev_num is 0");
        return ERROR;
    }
    HILOG_DEBUG("dataSize: %{public}d, offsetSize: %{public}d, elementSize: %{public}d",
        data.GetDataSize(), data.GetOffsetsSize(), dev_num * sizeof(AppExecFwk::ElementName));
    std::vector<AppExecFwk::ElementName> temp {};
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AppExecFwk::ElementName> iter(data.ReadParcelable<AppExecFwk::ElementName>());
        if (!iter) {
            HILOG_ERROR("ReadParcelable<AppExecFwk::ElementName> failed");
            return ERROR;
        }
        temp.push_back(*iter);
    }
    for (int i = 0; i < dev_num; i++) {
        it.insert(make_pair(temp[i].GetURI(), temp[i]));
    }
    SetEnabledObj(it);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetEnabledAbilities(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    std::map<std::string, AppExecFwk::ElementName> it = GetEnabledAbilities();
    std::map<std::string, AppExecFwk::ElementName>::iterator iter;

    reply.WriteInt32(it.size());
    for (iter = it.begin(); iter != it.end(); ++iter) {
        bool ret = reply.WriteParcelable(&iter->second);
        if (!ret) {
            return ERROR;
        }
    }

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetInstalledAbilities(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityAbilityInfo> it = GetInstalledAbilities();
    int num = it.size();

    reply.WriteInt32(it.size());
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&it[i]);
        if (!ret) {
            return ErrCode::ERROR;
        }
    }
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleDisableAbilities(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    std::map<std::string, AppExecFwk::ElementName> it {};
    int dev_num = data.ReadInt32();
    if (!dev_num) {
        HILOG_DEBUG("ReadParcelable failed, dev_num is 0");
        return ERROR;
    }
    HILOG_DEBUG("dataSize: %{public}d, offsetSize: %{public}d, elementSize: %{public}d",
        data.GetDataSize(), data.GetOffsetsSize(), dev_num * sizeof(AppExecFwk::ElementName));
    std::vector<AppExecFwk::ElementName> temp {};
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AppExecFwk::ElementName> iter(data.ReadParcelable<AppExecFwk::ElementName>());
        if (!iter) {
            HILOG_ERROR("ReadParcelable<AppExecFwk::ElementName> failed");
            return ERROR;
        }
        temp.push_back(*iter);
    }
    for (int i = 0; i < dev_num; i++) {
        it.insert(make_pair(temp[i].GetURI(), temp[i]));
    }
    DisableAbilities(it);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleRegisterUITestAbilityConnectionClient(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    bool result = RegisterUITestAbilityConnectionClient(obj);
    if (!reply.WriteBool(result)) {
        HILOG_ERROR("WriteBool failed");
        return ErrCode::ERROR;
    }
    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleGetActiveWindow(MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    int activeWindow = GetActiveWindow();
    reply.WriteInt32(activeWindow);

    return ErrCode::NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceClientStub::HandleDeregisterUITestAbilityConnectionClient(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    bool result = DeregisterUITestAbilityConnectionClient();
    if (!reply.WriteBool(result)) {
        HILOG_ERROR("WriteBool failed");
        return ErrCode::ERROR;
    }
    return ErrCode::NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS
