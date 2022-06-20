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
#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorCallbackProxy::AccessibilityElementOperatorCallbackProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperatorCallback>(impl)
{}

AccessibilityElementOperatorCallbackProxy::~AccessibilityElementOperatorCallbackProxy()
{}

bool AccessibilityElementOperatorCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibilityElementOperatorCallbackProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorCallbackProxy::SendTransactCmd(IAccessibilityElementOperatorCallback::Message code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
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

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(infos.size())) {
        HILOG_ERROR("write infos's size failed");
        return;
    }

    for (auto &info : infos) {
        AccessibilityElementInfoParcel infoParcel(info);
        if (!data.WriteParcelable(&infoParcel)) {
            HILOG_ERROR("write Parcelable failed");
            return;
        }
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
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(infos.size())) {
        HILOG_ERROR("write infos's size failed");
        return;
    }

    for (auto &info : infos) {
        AccessibilityElementInfoParcel infoParcel(info);
        if (!data.WriteParcelable(&infoParcel)) {
            HILOG_ERROR("write Parcelable failed");
            return;
        }
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
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    AccessibilityElementInfoParcel infoParcel(info);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&infoParcel)) {
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
    const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    AccessibilityElementInfoParcel infoParcel(info);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&infoParcel)) {
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

void AccessibilityElementOperatorCallbackProxy::SetExecuteActionResult(const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

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
} // namespace Accessibility
} // namespace OHOS