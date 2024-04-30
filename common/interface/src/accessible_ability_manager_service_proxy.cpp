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
#include "accessibility_ipc_interface_code.h"
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
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(IAccessibleAbilityManagerService::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityManagerServiceProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> remoteServiceProxy= Remote();
    if (!remoteServiceProxy) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t resultServiceProxy =
        remoteServiceProxy->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (resultServiceProxy != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", resultServiceProxy, code);
        return false;
    }
    return true;
}

RetError AccessibleAbilityManagerServiceProxy::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    AccessibilityEventInfoParcel eventInfoParcel(uiEvent);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteParcelable(&eventInfoParcel)) {
        HILOG_ERROR("fail, connection write parcelable AccessibilityEventInfo error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEND_EVENT, data, reply, option)) {
        HILOG_ERROR("SendEvent fail");
        return RET_ERR_IPC_FAILED;
    }
    return RET_OK;
}

RetError AccessibleAbilityManagerServiceProxy::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    CaptionPropertyParcel captionParcel(caption);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteParcelable(&captionParcel)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CAPTION_PROPERTY, data, reply, option)) {
        HILOG_ERROR("SetCaptionProperty fail");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetCaptionState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetCaptionState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetCaptionState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CAPTION_STATE, data, reply, option)) {
        HILOG_ERROR("SetCaptionState fail");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &client)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!client) {
        HILOG_ERROR("RegisterStateObserver fail, Input client is null");
        return TRANSACTION_ERR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("RegisterStateObserver fail, connection write Token error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("RegisterStateObserver fail, connection write client error");
        return TRANSACTION_ERR;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::REGISTER_STATE_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

RetError AccessibleAbilityManagerServiceProxy::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(abilityTypes)) {
        HILOG_ERROR("fail, connection write abilityTypes error");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(stateType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ABILITYLIST,
        data, reply, option)) {
        HILOG_ERROR("GetAbilityList fail");
        return RET_ERR_IPC_FAILED;
    }
    // read result
    int32_t abilityInfoSize = reply.ReadInt32();
    for (int32_t i = 0; i < abilityInfoSize; i++) {
        sptr<AccessibilityAbilityInfoParcel> info = reply.ReadStrongParcelable<AccessibilityAbilityInfoParcel>();
        if (!info) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return RET_ERR_IPC_FAILED;
        }
        infos.emplace_back(*info);
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::RegisterElementOperator(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &operation, bool isApp)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!operation) {
        HILOG_ERROR("fail, Input operation is null");
        return RET_ERR_INVALID_PARAM;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("fail, connection write windowId error");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(operation->AsObject())) {
        HILOG_ERROR("fail, connection write parcelable operation error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::REGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("RegisterElementOperator fail");
        return RET_ERR_IPC_FAILED;
    }
    return RET_OK;
}

RetError AccessibleAbilityManagerServiceProxy::DeregisterElementOperator(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("fail, connection write userId error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::DEREGISTER_INTERACTION_CONNECTION,
        data, reply, option)) {
        HILOG_ERROR("DeregisterElementOperator fail");
        return RET_ERR_IPC_FAILED;
    }
    return RET_OK;
}

RetError AccessibleAbilityManagerServiceProxy::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    AccessibilityConfig::CaptionProperty property = {};
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_CAPTION_PROPERTY,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionProperty fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        caption = *reply.ReadStrongParcelable<CaptionPropertyParcel>();
    }
    return ret;
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &client)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!client) {
        HILOG_ERROR("RegisterCaptionObserver fail, Input client is null");
        return TRANSACTION_ERR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("RegisterCaptionObserver fail, connection write Token error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("RegisterCaptionObserver fail, connection write client error");
        return TRANSACTION_ERR;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::REGISTER_CAPTION_PROPERTY_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterCaptionPropertyCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

bool AccessibleAbilityManagerServiceProxy::GetEnabledState()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ENABLED,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledState fail");
        return false;
    }
    return reply.ReadBool();
}

RetError AccessibleAbilityManagerServiceProxy::GetCaptionState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_CAPTION_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetCaptionState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

bool AccessibleAbilityManagerServiceProxy::GetTouchGuideState()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_TOUCH_GUIDE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetTouchGuideState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetGestureState()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_GESTURE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetGestureState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetKeyEventObserverState()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_KEY_EVENT_OBSERVE_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetKeyEventObserverState fail");
        return false;
    }
    return reply.ReadBool();
}

RetError AccessibleAbilityManagerServiceProxy::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("EnableAbility fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("name write error: %{public}s, ", name.c_str());
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(capabilities)) {
        HILOG_ERROR("capabilities write error: %{public}d, ", capabilities);
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::ENABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("EnableAbility fail");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ENABLED_OBJECT,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledAbilities fail");
        return RET_ERR_IPC_FAILED;
    }

    int32_t dev_num = reply.ReadInt32();
    for (int32_t i = 0; i < dev_num; i++) {
        enabledAbilities.push_back(reply.ReadString());
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::DisableAbility(const std::string &name)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("DisableAbility fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("name write error: %{public}s, ", name.c_str());
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::DISABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("DisableAbility fail");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

int32_t AccessibleAbilityManagerServiceProxy::GetActiveWindow()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ACTIVE_WINDOW,
        data, reply, option)) {
        HILOG_ERROR("GetActiveWindow fail");
        return false;
    }
    return reply.ReadInt32();
}

RetError AccessibleAbilityManagerServiceProxy::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(obj)) {
        HILOG_ERROR("fail, connection write obj");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::ENABLE_UI_TEST_ABILITY,
        data, reply, option)) {
        HILOG_ERROR("EnableUITestAbility fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError result = static_cast<RetError>(reply.ReadInt32());
    HILOG_DEBUG("enable result is %{public}d", result);
    return result;
}

RetError AccessibleAbilityManagerServiceProxy::DisableUITestAbility()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::DISABLE_UI_TEST_ABILITY,
        data, reply, option)) {
        HILOG_ERROR("DisableUITestAbility fail");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetScreenMagnificationState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetScreenMagnificationState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_SCREENMAGNIFIER_STATE, data, reply, option)) {
        HILOG_ERROR("SetScreenMagnificationState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetShortKeyState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetShortKeyState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetShortKeyState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_SHORTKEY_STATE, data, reply, option)) {
        HILOG_ERROR("SetShortKeyState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetMouseKeyState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetMouseKeyState fail, connection write parcelable Caption State");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_MOUSEKEY_STATE, data, reply, option)) {
        HILOG_ERROR("SetMouseKeyState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(time)) {
        HILOG_ERROR("fail, connection write SetMouseAutoClick time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_MOUSEKEY_AUTOCLICK, data, reply, option)) {
        HILOG_ERROR("SetMouseAutoClick fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("DisableAbility fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("fail, connection write SetShortkeyTarget name");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_SHORTKEY_TARGET, data, reply, option)) {
        HILOG_ERROR("SetShortkeyTarget fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("DisableAbility fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteStringVector(name)) {
        HILOG_ERROR("fail, connection write SetShortkeyMultiTarget name");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_SHORTKEY_MULTI_TARGET, data, reply, option)) {
        HILOG_ERROR("send SetShortkeyMultiTarget fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetHighContrastTextState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetHighContrastTextState fail, connection write parcelable Caption State");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_HIGHCONTRASTTEXT_STATE, data, reply, option)) {
        HILOG_ERROR("SetHighContrastTextState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetInvertColorState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetInvertColorState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetInvertColorState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_INVERTCOLOR_STATE, data, reply, option)) {
        HILOG_ERROR("SetInvertColorState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetAnimationOffState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetAnimationOffState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_ANIMATIONOFF_STATE, data, reply, option)) {
        HILOG_ERROR("SetAnimationOffState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetAudioMonoState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetAudioMonoState fail, connection write parcelable Caption State ");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_AUDIOMONO_STATE, data, reply, option)) {
        HILOG_ERROR("SetAudioMonoState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetDaltonizationState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("SetDaltonizationState fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("SetDaltonizationState fail, connection write parcelable Caption State");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_DALTONIZATION_STATE, data, reply, option)) {
        HILOG_ERROR("SetDaltonizationState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(filter)) {
        HILOG_ERROR("fail, connection write SetDaltonizationColorFilter time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_DALTONIZATION_COLORFILTER,
        data, reply, option)) {
        HILOG_ERROR("SetDaltonizationColorFilter fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(time)) {
        HILOG_ERROR("fail, connection write SetContentTimeout time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CONTENT_TIMEOUT, data, reply, option)) {
        HILOG_ERROR("SetContentTimeout fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteFloat(discount)) {
        HILOG_ERROR("fail, connection write SetBrightnessDiscount time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_BRIGHTNESS_DISCOUNT, data, reply, option)) {
        HILOG_ERROR("SetBrightnessDiscount fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetAudioBalance(const float balance)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteFloat(balance)) {
        HILOG_ERROR("fail, connection write SetAudioBalance time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_AUDIO_BALANCE, data, reply, option)) {
        HILOG_ERROR("SetAudioBalance fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetClickResponseTime(const uint32_t time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(time)) {
        HILOG_ERROR("fail, connection write SetAudioBalance time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CLICK_RESPONSE_TIME, data, reply, option)) {
        HILOG_ERROR("SetClickResponseTime fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write SetAudioBalance time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_IGNORE_REPEAT_CLICK_STATE, data, reply, option)) {
        HILOG_ERROR("SetIgnoreRepeatClickState fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint32(time)) {
        HILOG_ERROR("fail, connection write SetAudioBalance time");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_IGNORE_REPEAT_CLICK_TIME, data, reply, option)) {
        HILOG_ERROR("SetIgnoreRepeatClickTime fail");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityManagerServiceProxy::GetScreenMagnificationState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_SCREENMAGNIFIER_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetScreenMagnificationState fail");
        return RET_ERR_IPC_FAILED;
    }

    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetShortKeyState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_SHORTKEY_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetShortKeyState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetMouseKeyState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_MOUSEKEY_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetMouseKeyState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetMouseAutoClick(int32_t &time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_MOUSEKEY_AUTOCLICK,
        data, reply, option)) {
        HILOG_ERROR("GetMouseAutoClick fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        time = reply.ReadInt32();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetShortkeyTarget(std::string &name)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_SHORTKEY_TARGET,
        data, reply, option)) {
        HILOG_ERROR("GetShortkeyTarget fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        name = reply.ReadString();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_SHORTKEY_MULTI_TARGET,
        data, reply, option)) {
        HILOG_ERROR("GetShortkeyMultiTarget fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret != RET_OK) {
        return ret;
    }
    if (!reply.ReadStringVector(&name)) {
        return RET_ERR_IPC_FAILED;
    }
    return RET_OK;
}

RetError AccessibleAbilityManagerServiceProxy::GetHighContrastTextState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_HIGHCONTRASTTEXT_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetHighContrastTextState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetInvertColorState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_INVERTCOLOR_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetInvertColorState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetAnimationOffState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ANIMATIONOFF_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetAnimationOffState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetAudioMonoState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_AUDIOMONO_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetAudioMonoState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetDaltonizationState(bool &state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_DALTONIZATION_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetDaltonizationState fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetDaltonizationColorFilter(uint32_t &type)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_DALTONIZATION_COLORFILTER,
        data, reply, option)) {
        HILOG_ERROR("GetDaltonizationColorFilter fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        type = reply.ReadUint32();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetContentTimeout(uint32_t &timer)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_CONTENT_TIMEOUT,
        data, reply, option)) {
        HILOG_ERROR("GetContentTimeout fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        timer = reply.ReadUint32();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetBrightnessDiscount(float &brightness)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_BRIGHTNESS_DISCOUNT,
        data, reply, option)) {
        HILOG_ERROR("GetBrightnessDiscount fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        brightness = reply.ReadFloat();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetAudioBalance(float &balance)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_AUDIO_BALANCE,
        data, reply, option)) {
        HILOG_ERROR("GetAudioBalance fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        balance = reply.ReadFloat();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetClickResponseTime(uint32_t& time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_CLICK_RESPONSE_TIME,
        data, reply, option)) {
        HILOG_ERROR("GetAudioBalance fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        time = reply.ReadUint32();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetIgnoreRepeatClickState(bool& state)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_IGNORE_REPEAT_CLICK_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetAudioBalance fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        state = reply.ReadBool();
    }
    return ret;
}

RetError AccessibleAbilityManagerServiceProxy::GetIgnoreRepeatClickTime(uint32_t& time)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_IGNORE_REPEAT_CLICK_TIME,
        data, reply, option)) {
        HILOG_ERROR("GetAudioBalance fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret == RET_OK) {
        time = reply.ReadUint32();
    }
    return ret;
}

void AccessibleAbilityManagerServiceProxy::GetAllConfigs(AccessibilityConfigData& configData)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_ALL_CONFIGS,
        data, reply, option)) {
        HILOG_ERROR("GetAllConfigs fail");
        return;
    }

    RetError ret = static_cast<RetError>(reply.ReadInt32());
    if (ret != RET_OK) {
        HILOG_ERROR("GetAllConfigs failed %{public}d", ret);
        return;
    }
    std::vector<std::string> tmpMultiTarget;
    configData.highContrastText_ = reply.ReadBool();
    configData.invertColor_ = reply.ReadBool();
    configData.animationOff_ = reply.ReadBool();
    configData.audioMono_ = reply.ReadBool();
    configData.mouseKey_ = reply.ReadBool();
    configData.captionState_ = reply.ReadBool();
    configData.screenMagnifier_ = reply.ReadBool();
    configData.shortkey_ = reply.ReadBool();
    configData.mouseAutoClick_ = reply.ReadInt32();
    configData.daltonizationState_ = reply.ReadBool();
    configData.daltonizationColorFilter_ = reply.ReadUint32();
    configData.contentTimeout_ = reply.ReadUint32();
    configData.brightnessDiscount_ = reply.ReadFloat();
    configData.audioBalance_ = reply.ReadFloat();
    configData.shortkeyTarget_ = reply.ReadString();
    configData.captionProperty_ = *reply.ReadStrongParcelable<CaptionPropertyParcel>();
    reply.ReadStringVector(&tmpMultiTarget);
    configData.shortkeyMultiTarget_ = tmpMultiTarget;
}

void AccessibleAbilityManagerServiceProxy::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!observer) {
        HILOG_ERROR("observer is nullptr");
        return;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write interface token error");
        return;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOG_ERROR("observer write remote object error");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::REGISTER_ENABLE_ABILITY_LISTS_OBSERVER,
        data, reply, option)) {
        HILOG_ERROR("RegisterEnableAbilityListsObserver fail");
        return;
    }
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &client)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!client) {
        HILOG_ERROR("RegisterConfigObserver fail, Input client is null");
        return TRANSACTION_ERR;
    }

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("RegisterConfigObserver fail, connection write Token error");
        return TRANSACTION_ERR;
    }

    if (!data.WriteRemoteObject(client->AsObject())) {
        HILOG_ERROR("RegisterConfigObserver fail, connection write client error");
        return TRANSACTION_ERR;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::REGISTER_CONFIG_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

void AccessibleAbilityManagerServiceProxy::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write Token fail");
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("write windowId fail");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("write elementId fail");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_WINDOW_AND_ELEMENT_ID,
        data, reply, option)) {
        HILOG_ERROR("GetRealWindowAndElementId fail");
        return;
    }

    windowId = reply.ReadInt32();
    elementId = reply.ReadInt64();
}

void AccessibleAbilityManagerServiceProxy::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write Token fail");
        return;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("write windowId fail");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("write elementId fail");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_SCENE_BOARD_INNER_WINDOW_ID,
        data, reply, option)) {
        HILOG_ERROR("GetSceneBoardInnerWinId fail");
        return;
    }

    innerWid = reply.ReadInt32();
}

RetError AccessibleAbilityManagerServiceProxy::GetFocusedWindowId(int32_t &focusedWindowId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write token fail");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_FOCUSED_WINDOW_ID, data, reply, option)) {
        HILOG_ERROR("GetFocusedWindowId fail");
        return RET_ERR_IPC_FAILED;
    }

    focusedWindowId = reply.ReadInt32();
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS