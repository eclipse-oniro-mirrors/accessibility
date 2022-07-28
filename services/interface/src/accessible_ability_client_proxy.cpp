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
#include "accessibility_element_info_parcel.h"
#include "accessibility_event_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAccessibleAbilityClient>(object)
{
}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibleAbilityClientProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientProxy::SendTransactCmd(IAccessibleAbilityClient::Message code, MessageParcel &data,
    MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG();

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

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG();

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

void AccessibleAbilityClientProxy::Disconnect(const int32_t channelId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG();

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
    AccessibilityEventInfoParcel eventInfoParcel(eventInfo);

    HILOG_DEBUG();

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&eventInfoParcel)) {
        HILOG_ERROR("fail, eventInfo write parcelable error");
        return;
    }
    if (!SendTransactCmd(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT, data, reply, option)) {
        HILOG_ERROR("OnAccessibilityEvent fail");
        return;
    }
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG();

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
} // namespace Accessibility
} // namespace OHOS