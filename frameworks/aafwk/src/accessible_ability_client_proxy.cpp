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
#include <stdint.h>
#include "accessible_ability_client_proxy.h"
#include "accessibility_errorcode.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {

AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object):
    IRemoteProxy<IAccessibleAbilityClient>(object)
{}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!data.WriteInterfaceToken(AccessibleAbilityClientProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!channel) {
        HILOG_ERROR("channel is null.");
        return;
    }
    if (!data.WriteRemoteObject(channel->AsObject())) {
        HILOG_ERROR("%{public}s fail, channel write parcelable error", __func__);
        return;
    }

    if (!data.WriteInt32(channelId)) {
        HILOG_ERROR("%{public}s fail, channelId write int32 error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::INIT), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Init fail, error: %d", error);
    }
}

void AccessibleAbilityClientProxy::Disconnect(const int channelId) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteInt32(channelId)) {
        HILOG_ERROR("%{public}s fail, channelId write int32 error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::DISCONNECT),
                data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Disconnect fail, error: %d", error);
    }
}

void AccessibleAbilityClientProxy::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteParcelable(&eventInfo)) {
        HILOG_ERROR("%{public}s fail, eventInfo write parcelable error", __func__);
        return;
    }
    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT),
                data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnAccessibilityEvent fail, error: %d", error);
    }
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }
#if 0 // TODO: Use AccessibilityKeyEvent to instead MMI::KeyEvent
    if (!data.WriteParcelable(&keyEvent)) {
        HILOG_ERROR("%{public}s fail, keyEvent write parcelable error", __func__);
        return;
    }
#endif
    if (!data.WriteInt32(sequence)) {
        HILOG_ERROR("%{public}s fail, sequence write int32 error", __func__);
        return;
    }
    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT),
                data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnKeyPressEvent fail, error: %d", error);
    }
}

void AccessibleAbilityClientProxy::OnDisplayResized(const int displayId, const Rect &rect, const float scale,
                                                            const float centerX, const float centerY) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("%{public}s fail, displayId write int32 error", __func__);
        return;
    }
    if (!data.WriteParcelable(&rect)) {
        HILOG_ERROR("%{public}s fail, rect write parcelable error", __func__);
        return;
    }
    if (!data.WriteFloat(scale)) {
        HILOG_ERROR("%{public}s fail, scale write float error", __func__);
        return;
    }
    if (!data.WriteFloat(centerX)) {
        HILOG_ERROR("%{public}s fail, centerX write float error", __func__);
        return;
    }
    if (!data.WriteFloat(centerY)) {
        HILOG_ERROR("%{public}s fail, centerY write float error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED),
                data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnDisplayResized fail, error: %d", error);
    }
}

void AccessibleAbilityClientProxy::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) {
    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    HILOG_DEBUG("%{public}s start.", __func__);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(sequence)) {
        HILOG_ERROR("%{public}s fail, sequence write int32 error", __func__);
        return;
    }
    if (!data.WriteBool(completedSuccessfully)) {
        HILOG_ERROR("%{public}s fail, completedSuccessfully write bool error", __func__);
        return;
    }
    error = Remote()->SendRequest(static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT),
                data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnGestureSimulateResult fail, error: %d", error);
    }
}

} // namespace Accessibility
} // namespace OHOS