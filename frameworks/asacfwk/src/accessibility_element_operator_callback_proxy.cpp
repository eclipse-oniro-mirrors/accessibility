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

#include "accessibility_element_operator_callback_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorCallbackProxy::AccessibilityElementOperatorCallbackProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperatorCallback>(impl)
{}

AccessibilityElementOperatorCallbackProxy::~AccessibilityElementOperatorCallbackProxy()
{}
bool AccessibilityElementOperatorCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("start");
    if (!data.WriteInterfaceToken(AccessibilityElementOperatorCallbackProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorCallbackProxy::SendTransactCmd(IAccessibilityElementOperatorCallback::Message code,
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

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!WriteParcelableVector(infos, data)) {
        HILOG_ERROR("write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId");
        return;
    }

    if (!SendTransactCmd(IAccessibilityElementOperatorCallback::Message::SET_RESULT_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("SetSearchElementInfoByAccessibilityIdResult failed");
        return;
    }
}

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!WriteParcelableVector(infos, data)) {
        HILOG_ERROR("write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId");
        return;
    }

    if (!SendTransactCmd(IAccessibilityElementOperatorCallback::Message::SET_RESULT_BY_TEXT,
        data, reply, option)) {
        HILOG_ERROR("SetSearchElementInfoByTextResult failed");
        return;
    }
}

void AccessibilityElementOperatorCallbackProxy::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&info)) {
        HILOG_ERROR("fail, connection write info");
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId");
        return;
    }

    if (!SendTransactCmd(IAccessibilityElementOperatorCallback::Message::SET_RESULT_FOCUSED_INFO,
        data, reply, option)) {
        HILOG_ERROR("SetFindFocusedElementInfoResult failed");
        return;
    }
}

void AccessibilityElementOperatorCallbackProxy::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int requestId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&info)) {
        HILOG_ERROR("fail, connection write info");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId");
        return;
    }

    if (!SendTransactCmd(IAccessibilityElementOperatorCallback::Message::SET_RESULT_FOCUS_MOVE,
        data, reply, option)) {
        HILOG_ERROR("SetFocusMoveSearchResult failed");
        return;
    }
}

void AccessibilityElementOperatorCallbackProxy::SetExecuteActionResult(const bool succeeded, const int requestId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteBool(succeeded)) {
        HILOG_ERROR("fail, connection write succeeded");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId");
        return;
    }

    if (!SendTransactCmd(IAccessibilityElementOperatorCallback::Message::SET_RESULT_PERFORM_ACTION,
        data, reply, option)) {
        HILOG_ERROR("SetExecuteActionResult failed");
        return;
    }
}

template<typename T>
bool AccessibilityElementOperatorCallbackProxy::WriteParcelableVector(const std::vector<T> &parcelableVector,
    Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOG_ERROR("write ParcelableVector failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(&parcelable)) {
            HILOG_ERROR("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}
} // namespace Accessibility
} // namespace OHOS
