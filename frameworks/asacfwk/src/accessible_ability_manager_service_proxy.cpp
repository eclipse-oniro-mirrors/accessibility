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

void AccessibleAbilityManagerServiceClientProxy::SendEvent(const AccessibilityEventInfo& uiEvent, const int userId)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %{public}d", error);
        return;
    }
}

bool AccessibleAbilityManagerServiceClientProxy::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_PROPERTY),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetCaptionProperty fail, error: %{public}d", error);
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_CAPTION_STATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetCaptionState fail, error: %{public}d", error);
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetEnabled(const bool state)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetEnabled fail, error: %{public}d", error);
        return false;
    }
    return true;
}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState>& client, const int userId)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (client == nullptr) {
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %{public}d", error);
        return ErrCode::ERROR;
    }

    return reply.ReadUint32();
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetAbilityList(
    const int abilityTypes, const int stateType)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AccessibilityAbilityInfo> errorList {};
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return errorList;
    }

    if (!data.WriteInt32(abilityTypes)) {
        HILOG_ERROR("fail, connection write abilityTypes error");
        return errorList;
    }

    if (!data.WriteInt32(stateType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return errorList;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST), data, reply, option);
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

void AccessibleAbilityManagerServiceClientProxy::RegisterElementOperator(
    int windowId, const sptr<IAccessibilityElementOperator>& operation, const int userId)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (operation == nullptr) {
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Register fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerServiceClientProxy::DeregisterElementOperator(const int windowId)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Deregister fail, error: %{public}d", error);
        return;
    }
}

CaptionProperty AccessibleAbilityManagerServiceClientProxy::GetCaptionProperty()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    CaptionProperty property = {};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return property;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_PROPERTY),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return property;
    }
    return *reply.ReadParcelable<CaptionProperty>();
}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& client, const int userId)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (client == nullptr) {
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_CAPTION_PROPERTY_CALLBACK),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SendEvent fail, error: %{public}d", error);
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

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetEnabled fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetCaptionState()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_CAPTION_STATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetCaptionState fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetTouchGuideState()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_TOUCH_GUIDE_STATE),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetTouchGuideState fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetGestureState()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_GESTURE_STATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetGestureState fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_KEY_EVENT_OBSERVE_STATE),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetKeyEventObserverState fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_TOUCH_GUIDE_STATE),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetTouchGuideState fail, error: %{public}d", error);
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetGestureState(const bool state)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_GESTURE_STATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetGestureState fail, error: %{public}d", error);
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_KEY_EVENT_OBSERVE_STATE),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetKeyEventObserverState fail, error: %{public}d", error);
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceClientProxy::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SET_ENABLED_OBJECT),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetEnabledObj fail, error: %{public}d", error);
        return false;
    }
    return true;
}

std::map<std::string, AppExecFwk::ElementName> AccessibleAbilityManagerServiceClientProxy::GetEnabledAbilities()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::map<std::string, AppExecFwk::ElementName> it {};

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return it;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ENABLED_OBJECT),
        data,
        reply,
        option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return it;
    }

    int dev_num = reply.ReadInt32();
    std::vector<AppExecFwk::ElementName> temp {};
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AppExecFwk::ElementName> iter(reply.ReadParcelable<AppExecFwk::ElementName>());
        temp.push_back(*iter);
    }

    for (int i = 0; i < dev_num; i++) {
        it.insert(make_pair(temp[i].GetURI(), temp[i]));
    }
    return it;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetInstalledAbilities()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AccessibilityAbilityInfo> it {};

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return it;
    }
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_INSTALLED), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return it;
    }
    int dev_num = reply.ReadInt32();
    for (int i = dev_num; i > 0; i--) {
        std::unique_ptr<AccessibilityAbilityInfo> dev(reply.ReadParcelable<AccessibilityAbilityInfo>());
        it.push_back(*dev);
    }
    return it;
}

bool AccessibleAbilityManagerServiceClientProxy::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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
    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DISABLE_ABILITIES),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetEnabledObj fail, error: %{public}d", error);
        return false;
    }
    return true;
}

int AccessibleAbilityManagerServiceClientProxy::GetActiveWindow()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(
            IAccessibleAbilityManagerServiceClient::Message::GET_ACTIVE_WINDOW),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("GetActiveWindow fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadInt32();
}

bool AccessibleAbilityManagerServiceClientProxy::RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
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

    error = Remote()->SendRequest(static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::REGISTER_UITEST_ABILITY_CONNECTION_CLIENT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceClientProxy::DeregisterUITestAbilityConnectionClient()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>
        (IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_UITEST_ABILITY_CONNECTION_CLIENT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Interrupt fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}
} // namespace Accessibility
} // namespace OHOS
