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

#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_errorcode.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "accessibility_caption.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceClientProxy::AccessibleAbilityManagerServiceClientProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IAccessibleAbilityManagerServiceClient>(impl)
{}

AccessibleAbilityManagerServiceClientProxy::~AccessibleAbilityManagerServiceClientProxy()
{}

bool AccessibleAbilityManagerServiceClientProxy::WriteInterfaceToken(MessageParcel& data)
{
    HILOG_DEBUG("start");
    if (!data.WriteInterfaceToken(IAccessibleAbilityManagerServiceClient::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG("start.");

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}

void AccessibleAbilityManagerServiceClientProxy::SendEvent(const AccessibilityEventInfo& uiEvent, const int userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&uiEvent)) {
        HILOG_ERROR("fail, connection write parcelable AccessibilityEventInfo error");
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT, data, reply, option)) {
        HILOG_ERROR("SendEvent fail");
        return;
    }
}

bool AccessibleAbilityManagerServiceClientProxy::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteParcelable(&caption)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_PROPERTY, data, reply, option)) {
        HILOG_ERROR("SetCaptionProperty fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_STATE, data, reply, option)) {
        HILOG_ERROR("SetCaptionState fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetEnabled(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Enabled ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED, data, reply, option)) {
        HILOG_ERROR("SetEnabled fail");
        return false;
    }
    return true;
}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState>& client, const int userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!client) {
        HILOG_ERROR("fail, Input client is null");
        return ErrCode::ERROR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return ErrCode::ERROR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("fail, connection write client error");
        return ErrCode::ERROR;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return ErrCode::ERROR;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return ErrCode::ERROR;
    }
    return reply.ReadUint32();
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetAbilityList(
    const uint32_t abilityTypes, const int32_t stateType)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AccessibilityAbilityInfo> errorList {};
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return errorList;
    }

    if (!data.WriteUint32(abilityTypes)) {
        HILOG_ERROR("fail, connection write abilityTypes error");
        return errorList;
    }

    if (!data.WriteInt32(stateType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return errorList;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST,
        data, reply, option)) {
        HILOG_ERROR("GetAbilityList fail");
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

void AccessibleAbilityManagerServiceClientProxy::RegisterElementOperator(
    int windowId, const sptr<IAccessibilityElementOperator>& operation, const int userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!operation) {
        HILOG_ERROR("fail, Input operation is null");
        return;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("fail, connection write windowId error");
        return;
    }

    if (!data.WriteRemoteObject(operation->AsObject())) {
        HILOG_ERROR("fail, connection write parcelable operation error");
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("RegisterElementOperator fail");
        return;
    }
}

void AccessibleAbilityManagerServiceClientProxy::DeregisterElementOperator(const int windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("fail, connection write userId error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("DeregisterElementOperator fail");
        return;
    }
}

CaptionProperty AccessibleAbilityManagerServiceClientProxy::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    CaptionProperty property = {};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return property;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_PROPERTY,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionProperty fail");
        return property;
    }

    std::unique_ptr<CaptionProperty> captionProperty(reply.ReadParcelable<CaptionProperty>());
    if (!captionProperty) {
        HILOG_ERROR("ReadParcelable<CaptionProperty> failed");
        return property;
    }
    return *captionProperty;
}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& client, const int userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!client) {
        HILOG_ERROR("fail, Input client is null");
        return ErrCode::ERROR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return ErrCode::ERROR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("fail, connection write client error");
        return ErrCode::ERROR;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return ErrCode::ERROR;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::REGISTER_CAPTION_PROPERTY_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterCaptionPropertyCallback fail");
        return ErrCode::ERROR;
    }
    return reply.ReadUint32();
}

sptr<IRemoteObject> AccessibleAbilityManagerServiceClientProxy::GetObject()
{
    return this->AsObject();
}

bool AccessibleAbilityManagerServiceClientProxy::GetEnabledState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetCaptionState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_TOUCH_GUIDE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetTouchGuideState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetGestureState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_GESTURE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetGestureState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_KEY_EVENT_OBSERVE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetKeyEventObserverState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_TOUCH_GUIDE_STATE,
        data, reply, option)) {
        HILOG_ERROR("SetTouchGuideState fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetGestureState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_GESTURE_STATE,
        data, reply, option)) {
        HILOG_ERROR("SetGestureState fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_KEY_EVENT_OBSERVE_STATE,
        data, reply, option)) {
        HILOG_ERROR("SetKeyEventObserverState fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    data.WriteInt32(it.size());
    std::map<std::string, AppExecFwk::ElementName>::iterator iter;

    for (iter = it.begin(); iter != it.end(); ++iter) {
        bool ret = data.WriteParcelable(&iter->second);
        if (!ret) {
            return false;
        }
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED_OBJECT,
        data, reply, option)) {
        HILOG_ERROR("SetEnabledObj fail");
        return false;
    }
    return true;
}

std::map<std::string, AppExecFwk::ElementName> AccessibleAbilityManagerServiceClientProxy::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::map<std::string, AppExecFwk::ElementName> it {};

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return it;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED_OBJECT,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledAbilities fail");
        return it;
    }

    int dev_num = reply.ReadInt32();
    std::vector<AppExecFwk::ElementName> temp {};
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AppExecFwk::ElementName> iter(reply.ReadParcelable<AppExecFwk::ElementName>());
        if (!iter) {
            HILOG_ERROR("ReadParcelable<AppExecFwk::ElementName> failed");
            return it;
        }
        temp.push_back(*iter);
    }

    for (int j = 0; j < dev_num; j++) {
        it.insert(make_pair(temp[j].GetURI(), temp[j]));
    }
    return it;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetInstalledAbilities()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AccessibilityAbilityInfo> it {};

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return it;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_INSTALLED,
        data, reply, option)) {
        HILOG_ERROR("GetInstalledAbilities fail");
        return it;
    }

    int dev_num = reply.ReadInt32();
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AccessibilityAbilityInfo> dev(reply.ReadParcelable<AccessibilityAbilityInfo>());
        if (!dev) {
            HILOG_ERROR("ReadParcelable<accessibilityAbilityInfo> failed");
            return it;
        }
        it.push_back(*dev);
    }
    return it;
}

bool AccessibleAbilityManagerServiceClientProxy::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    data.WriteInt32(it.size());
    std::map<std::string, AppExecFwk::ElementName>::iterator iter;

    for (iter = it.begin(); iter != it.end(); ++iter) {
        bool ret = data.WriteParcelable(&iter->second);
        if (!ret) {
            return false;
        }
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::DISABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("DisableAbilities fail");
        return false;
    }
    return true;
}

int AccessibleAbilityManagerServiceClientProxy::GetActiveWindow()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::GET_ACTIVE_WINDOW,
        data, reply, option)) {
        HILOG_ERROR("GetActiveWindow fail");
        return false;
    }
    return reply.ReadInt32();
}

bool AccessibleAbilityManagerServiceClientProxy::RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteRemoteObject(obj)) {
        HILOG_ERROR("fail, connection write obj");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::REGISTER_UITEST_ABILITY_CONNECTION_CLIENT,
        data, reply, option)) {
        HILOG_ERROR("RegisterUITestAbilityConnectionClient fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::DeregisterUITestAbilityConnectionClient()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_UITEST_ABILITY_CONNECTION_CLIENT,
        data, reply, option)) {
        HILOG_ERROR("DeregisterUITestAbilityConnectionClient fail");
        return false;
    }
    return reply.ReadBool();
}
} // namespace Accessibility
} // namespace OHOS
