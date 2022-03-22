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

#include "accessible_ability_client_proxy.h"
#include "accessibility_errorcode.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAccessibleAbilityClient>(object)
{
}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("start.");
    if (!data.WriteInterfaceToken(AccessibleAbilityClientProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientProxy::SendTransactCmd(IAccessibleAbilityClient::Message code, MessageParcel &data,
    MessageParcel &reply,  MessageOption &option)
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

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!channel) {
        HILOG_ERROR("channel is null.");
        return;
    }
    if (!data.WriteRemoteObject(channel->AsObject())) {
        HILOG_ERROR("fail, channel write parcelable error");
        return;
    }

    if (!data.WriteInt32(channelId)) {
        HILOG_ERROR("fail, channelId write int32 error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityClient::Message::INIT, data, reply, option)) {
        HILOG_ERROR("Init fail");
        return;
    }
}

void AccessibleAbilityClientProxy::Disconnect(const int channelId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteInt32(channelId)) {
        HILOG_ERROR("fail, channelId write int32 error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityClient::Message::DISCONNECT, data, reply, option)) {
        HILOG_ERROR("Disconnect fail");
        return;
    }
}

void AccessibleAbilityClientProxy::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&eventInfo)) {
        HILOG_ERROR("fail, eventInfo write parcelable error");
        return;
    }
    if (!SendTransactCmd(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT, data, reply, option)) {
        HILOG_ERROR("OnAccessibilityEvent fail");
        return;
    }
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(sequence)) {
        HILOG_ERROR("fail, sequence write int32 error");
        return;
    }

    if (!keyEvent.WriteToParcel(data)) {
        HILOG_ERROR("fail, keyEvent WriteToParcel error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT, data, reply, option)) {
        HILOG_ERROR("OnKeyPressEvent fail");
        return;
    }
}

void AccessibleAbilityClientProxy::OnDisplayResized(const int displayId, const Rect &rect, const float scale,
    const float centerX, const float centerY)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("fail, displayId write int32 error");
        return;
    }
    if (!data.WriteParcelable(&rect)) {
        HILOG_ERROR("fail, rect write parcelable error");
        return;
    }
    if (!data.WriteFloat(scale)) {
        HILOG_ERROR("fail, scale write float error");
        return;
    }
    if (!data.WriteFloat(centerX)) {
        HILOG_ERROR("fail, centerX write float error");
        return;
    }
    if (!data.WriteFloat(centerY)) {
        HILOG_ERROR("fail, centerY write float error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED, data, reply, option)) {
        HILOG_ERROR("OnDisplayResized fail");
        return;
    }
}

void AccessibleAbilityClientProxy::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("start.");

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(sequence)) {
        HILOG_ERROR("fail, sequence write int32 error");
        return;
    }
    if (!data.WriteBool(completedSuccessfully)) {
        HILOG_ERROR("fail, completedSuccessfully write bool error");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT, data, reply, option)) {
        HILOG_ERROR("OnGestureSimulateResult fail");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS