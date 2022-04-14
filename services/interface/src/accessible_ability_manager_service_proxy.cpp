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
#include "accessibility_ability_info_parcel.h"
#include "accessibility_caption_parcel.h"
#include "accessibility_event_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceProxy::AccessibleAbilityManagerServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IAccessibleAbilityManagerService>(impl)
{}

AccessibleAbilityManagerServiceProxy::~AccessibleAbilityManagerServiceProxy()
{}

bool AccessibleAbilityManagerServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("start");
    if (!data.WriteInterfaceToken(IAccessibleAbilityManagerService::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceProxy::SendTransactCmd(IAccessibleAbilityManagerService::Message code,
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

void AccessibleAbilityManagerServiceProxy::SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    AccessibilityEventInfoParcel eventInfoParcel(uiEvent);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&eventInfoParcel)) {
        HILOG_ERROR("fail, connection write parcelable AccessibilityEventInfo error");
        return;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SEND_EVENT, data, reply, option)) {
        HILOG_ERROR("SendEvent fail");
        return;
    }
}

bool AccessibleAbilityManagerServiceProxy::SetCaptionProperty(const CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    CaptionPropertyParcel captionParcel(caption);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteParcelable(&captionParcel)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_CAPTION_PROPERTY, data, reply, option)) {
        HILOG_ERROR("SetCaptionProperty fail");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceProxy::SetCaptionState(const bool state)
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_CAPTION_STATE, data, reply, option)) {
        HILOG_ERROR("SetCaptionState fail");
        return false;
    }
    return true;
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &client, const int32_t userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!client) {
        HILOG_ERROR("fail, Input client is null");
        return TRANSACTION_ERR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("fail, connection write client error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return TRANSACTION_ERR;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_STATE_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceProxy::GetAbilityList(
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ABILITYLIST,
        data, reply, option)) {
        HILOG_ERROR("GetAbilityList fail");
        return errorList;
    }
    // read result
    int32_t abilityInfoSize = reply.ReadInt32();
    for (int32_t i = 0; i < abilityInfoSize; i++) {
        sptr<AccessibilityAbilityInfoParcel> info = reply.ReadStrongParcelable<AccessibilityAbilityInfoParcel>();
        if (!info) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return errorList;
        }
        abilityInfos.emplace_back(*info);
    }
    return abilityInfos;
}

void AccessibleAbilityManagerServiceProxy::RegisterElementOperator(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &operation, const int32_t userId)
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("RegisterElementOperator fail");
        return;
    }
}

void AccessibleAbilityManagerServiceProxy::DeregisterElementOperator(const int32_t windowId)
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::DEREGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("DeregisterElementOperator fail");
        return;
    }
}

CaptionProperty AccessibleAbilityManagerServiceProxy::GetCaptionProperty()
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_CAPTION_PROPERTY,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionProperty fail");
        return property;
    }
    return *reply.ReadStrongParcelable<CaptionPropertyParcel>();
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &client, const int32_t userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!client) {
        HILOG_ERROR("fail, Input client is null");
        return TRANSACTION_ERR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("fail, connection write client error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("fail, connection write userId error");
        return TRANSACTION_ERR;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_CAPTION_PROPERTY_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterCaptionPropertyCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

bool AccessibleAbilityManagerServiceProxy::GetEnabledState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ENABLED,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetCaptionState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_CAPTION_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_TOUCH_GUIDE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetTouchGuideState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetGestureState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_GESTURE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetGestureState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_KEY_EVENT_OBSERVE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetKeyEventObserverState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::EnableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    data.WriteInt32(abilities.size());
    for (auto &ability : abilities) {
        if (!data.WriteString(ability)) {
            HILOG_ERROR("ability write error: %{public}s, ", ability.c_str());
            return false;
        }
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::ENABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("EnableAbilities fail");
        return false;
    }
    return true;
}

std::vector<std::string> AccessibleAbilityManagerServiceProxy::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<std::string> abilities {};

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return abilities;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ENABLED_OBJECT,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledAbilities fail");
        return abilities;
    }

    int32_t dev_num = reply.ReadInt32();
    for (int32_t i = 0; i < dev_num; i++) {
        abilities.push_back(reply.ReadString());
    }
    return abilities;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceProxy::GetInstalledAbilities()
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
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_INSTALLED,
        data, reply, option)) {
        HILOG_ERROR("GetInstalledAbilities fail");
        return it;
    }
    int32_t dev_num = reply.ReadInt32();
    for (int32_t i = dev_num; i > 0; i--) {
        sptr<AccessibilityAbilityInfoParcel> info = reply.ReadStrongParcelable<AccessibilityAbilityInfoParcel>();
        if (!info) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return it;
        }
        it.push_back(*info);
    }
    return it;
}

bool AccessibleAbilityManagerServiceProxy::DisableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    data.WriteInt32(abilities.size());
    for (auto &ability : abilities) {
        if (!data.WriteString(ability)) {
            HILOG_ERROR("ability write error: %{public}s, ", ability.c_str());
            return false;
        }
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::DISABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("DisableAbilities fail");
        return false;
    }
    return true;
}

int32_t AccessibleAbilityManagerServiceProxy::GetActiveWindow()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ACTIVE_WINDOW,
        data, reply, option)) {
        HILOG_ERROR("GetActiveWindow fail");
        return false;
    }
    return reply.ReadInt32();
}

bool AccessibleAbilityManagerServiceProxy::EnableUITestAbility(const sptr<IRemoteObject> &obj)
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::ENABLE_UI_TEST_ABILITY,
        data, reply, option)) {
        HILOG_ERROR("EnableUITestAbility fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::DisableUITestAbility()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::DISABLE_UI_TEST_ABILITY,
        data, reply, option)) {
        HILOG_ERROR("DisableUITestAbility fail");
        return false;
    }
    return reply.ReadBool();
}

sptr<IRemoteObject> AccessibleAbilityManagerServiceProxy::GetRemoteObject()
{
    return this->AsObject();
}
} // namespace Accessibility
} // namespace OHOS