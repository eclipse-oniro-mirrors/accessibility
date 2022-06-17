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

void AccessibleAbilityManagerServiceProxy::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    AccessibilityEventInfoParcel eventInfoParcel(uiEvent);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&eventInfoParcel)) {
        HILOG_ERROR("fail, connection write parcelable AccessibilityEventInfo error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SEND_EVENT, data, reply, option)) {
        HILOG_ERROR("SendEvent fail");
        return;
    }
}

void AccessibleAbilityManagerServiceProxy::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    CaptionPropertyParcel captionParcel(caption);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&captionParcel)) {
        HILOG_ERROR("fail, connection write parcelable Caption Property ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_CAPTION_PROPERTY, data, reply, option)) {
        HILOG_ERROR("SetCaptionProperty fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_CAPTION_STATE, data, reply, option)) {
        HILOG_ERROR("SetCaptionState fail");
    }
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &client)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_STATE_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}

bool AccessibleAbilityManagerServiceProxy::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return false;
    }

    if (!data.WriteUint32(abilityTypes)) {
        HILOG_ERROR("fail, connection write abilityTypes error");
        return false;
    }

    if (!data.WriteInt32(stateType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ABILITYLIST,
        data, reply, option)) {
        HILOG_ERROR("GetAbilityList fail");
        return false;
    }
    // read result
    int32_t abilityInfoSize = reply.ReadInt32();
    for (int32_t i = 0; i < abilityInfoSize; i++) {
        sptr<AccessibilityAbilityInfoParcel> info = reply.ReadStrongParcelable<AccessibilityAbilityInfoParcel>();
        if (!info) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return false;
        }
        infos.emplace_back(*info);
    }
    return reply.ReadBool();
}

void AccessibleAbilityManagerServiceProxy::RegisterElementOperator(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &operation)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

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
    MessageOption option(MessageOption::TF_ASYNC);

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

AccessibilityConfig::CaptionProperty AccessibleAbilityManagerServiceProxy::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    AccessibilityConfig::CaptionProperty property = {};
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
    const sptr<IAccessibleAbilityManagerCaptionObserver> &client)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

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

bool AccessibleAbilityManagerServiceProxy::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("name write error: %{public}s, ", name.c_str());
        return false;
    }

    if (!data.WriteUint32(capabilities)) {
        HILOG_ERROR("capabilities write error: %{public}d, ", capabilities);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::ENABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("EnableAbility fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ENABLED_OBJECT,
        data, reply, option)) {
        HILOG_ERROR("GetEnabledAbilities fail");
        return false;
    }

    int32_t dev_num = reply.ReadInt32();
    for (int32_t i = 0; i < dev_num; i++) {
        enabledAbilities.push_back(reply.ReadString());
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetInstalledAbilities(
    std::vector<AccessibilityAbilityInfo> &installedAbilities)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token error");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_INSTALLED,
        data, reply, option)) {
        HILOG_ERROR("GetInstalledAbilities fail");
        return false;
    }
    int32_t dev_num = reply.ReadInt32();
    for (int32_t i = dev_num; i > 0; i--) {
        sptr<AccessibilityAbilityInfoParcel> info = reply.ReadStrongParcelable<AccessibilityAbilityInfoParcel>();
        if (!info) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return false;
        }
        installedAbilities.push_back(*info);
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::DisableAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("name write error: %{public}s, ", name.c_str());
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::DISABLE_ABILITIES,
        data, reply, option)) {
        HILOG_ERROR("DisableAbility fail");
        return false;
    }
    return reply.ReadBool();
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

RetError AccessibleAbilityManagerServiceProxy::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG("start");
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::ENABLE_UI_TEST_ABILITY,
        data, reply, option)) {
        HILOG_ERROR("EnableUITestAbility fail");
        return RET_ERR_IPC_FAILED;
    }
    RetError result = static_cast<RetError>(reply.ReadInt32());
    HILOG_DEBUG("enable result is %{public}d", result);
    return result;
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

void AccessibleAbilityManagerServiceProxy::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_SCREENMAGNIFIER_STATE, data, reply, option)) {
        HILOG_ERROR("SetScreenMagnificationState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_SHORTKEY_STATE, data, reply, option)) {
        HILOG_ERROR("SetShortKeyState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_MOUSEKEY_STATE, data, reply, option)) {
        HILOG_ERROR("SetMouseKeyState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(time)) {
        HILOG_ERROR("fail, connection write SetMouseAutoClick time");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_MOUSEKEY_AUTOCLICK, data, reply, option)) {
        HILOG_ERROR("SetMouseAutoClick fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteString(name)) {
        HILOG_ERROR("fail, connection write SetShortkeyTarget name");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_SHORTKEY_TARGET, data, reply, option)) {
        HILOG_ERROR("SetShortkeyTarget fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_HIGHCONTRASTTEXT_STATE, data, reply, option)) {
        HILOG_ERROR("SetHighContrastTextState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_INVERTCOLOR_STATE, data, reply, option)) {
        HILOG_ERROR("SetInvertColorState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_ANIMATIONOFF_STATE, data, reply, option)) {
        HILOG_ERROR("SetAnimationOffState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write parcelable Caption State ");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_AUDIOMONO_STATE, data, reply, option)) {
        HILOG_ERROR("SetAudioMonoState fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(filter)) {
        HILOG_ERROR("fail, connection write SetDaltonizationColorFilter time");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_DALTONIZATION_COLORFILTER,
        data, reply, option)) {
        HILOG_ERROR("SetDaltonizationColorFilter fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(time)) {
        HILOG_ERROR("fail, connection write SetContentTimeout time");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_CONTENT_TIMEOUT, data, reply, option)) {
        HILOG_ERROR("SetContentTimeout fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteFloat(discount)) {
        HILOG_ERROR("fail, connection write SetBrightnessDiscount time");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_BRIGHTNESS_DISCOUNT, data, reply, option)) {
        HILOG_ERROR("SetBrightnessDiscount fail");
    }
}

void AccessibleAbilityManagerServiceProxy::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteFloat(balance)) {
        HILOG_ERROR("fail, connection write SetAudioBalance time");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::SET_AUDIO_BALANCE, data, reply, option)) {
        HILOG_ERROR("SetAudioBalance fail");
    }
}

bool AccessibleAbilityManagerServiceProxy::GetScreenMagnificationState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_SCREENMAGNIFIER_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetScreenMagnificationState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetShortKeyState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_SHORTKEY_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetShortKeyState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetMouseKeyState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_MOUSEKEY_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetMouseKeyState fail");
        return false;
    }
    return reply.ReadBool();
}

int32_t AccessibleAbilityManagerServiceProxy::GetMouseAutoClick()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_MOUSEKEY_AUTOCLICK,
        data, reply, option)) {
        HILOG_ERROR("GetMouseAutoClick fail");
        return false;
    }
    return reply.ReadInt32();
}

std::string AccessibleAbilityManagerServiceProxy::GetShortkeyTarget()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string str = "";
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");

        return str;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_SHORTKEY_TARGET,
        data, reply, option)) {
        HILOG_ERROR("GetShortkeyTarget fail");
        return str;
    }
    return reply.ReadString();
}

bool AccessibleAbilityManagerServiceProxy::GetHighContrastTextState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_HIGHCONTRASTTEXT_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetHighContrastTextState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetInvertColorState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_INVERTCOLOR_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetInvertColorState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetAnimationOffState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_ANIMATIONOFF_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetAnimationOffState fail");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityManagerServiceProxy::GetAudioMonoState()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_AUDIOMONO_STATE,
        data, reply, option)) {
        HILOG_ERROR("GetAudioMonoState fail");
        return false;
    }
    return reply.ReadBool();
}

uint32_t AccessibleAbilityManagerServiceProxy::GetDaltonizationColorFilter()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_DALTONIZATION_COLORFILTER,
        data, reply, option)) {
        HILOG_ERROR("GetDaltonizationColorFilter fail");
        return false;
    }
    return reply.ReadUint32();
}

uint32_t AccessibleAbilityManagerServiceProxy::GetContentTimeout()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_CONTENT_TIMEOUT,
        data, reply, option)) {
        HILOG_ERROR("GetContentTimeout fail");
        return false;
    }
    return reply.ReadUint32();
}

float AccessibleAbilityManagerServiceProxy::GetBrightnessDiscount()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_BRIGHTNESS_DISCOUNT,
        data, reply, option)) {
        HILOG_ERROR("GetBrightnessDiscount fail");
        return false;
    }
    return reply.ReadFloat();
}

float AccessibleAbilityManagerServiceProxy::GetAudioBalance()
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return false;
    }
    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::GET_AUDIO_BALANCE,
        data, reply, option)) {
        HILOG_ERROR("GetAudioBalance fail");
        return false;
    }
    return reply.ReadFloat();
}

void AccessibleAbilityManagerServiceProxy::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG("start");
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_ENABLE_ABILITY_LISTS_OBSERVER,
        data, reply, option)) {
        HILOG_ERROR("RegisterEnableAbilityListsObserver fail");
        return;
    }
}

uint32_t AccessibleAbilityManagerServiceProxy::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &client)
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

    if (!SendTransactCmd(IAccessibleAbilityManagerService::Message::REGISTER_CONFIG_CALLBACK,
        data, reply, option)) {
        HILOG_ERROR("RegisterStateCallback fail");
        return TRANSACTION_ERR;
    }

    return reply.ReadUint32();
}
} // namespace Accessibility
} // namespace OHOS