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

constexpr int32_t IPC_MEMORY_SIZE = 500 * 1024; // default size is 200 * 1024B, batch query need more memory

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

bool AccessibilityElementOperatorCallbackProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> remoteObj = Remote();
    if (!remoteObj) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t ret = remoteObj->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", ret, code);
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
    data.SetMaxCapacity(IPC_MEMORY_SIZE);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt32(infos.size())) {
        HILOG_ERROR("write infos's size failed");
        return;
    }

    for (auto &info : infos) {
        AccessibilityElementInfoParcel infoParcel(info);
        if (!data.WriteParcelable(&infoParcel)) {
            HILOG_ERROR("write accessibility element info failed");
            return;
        }
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("set search element info by accessibility id result failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt32(infos.size())) {
        HILOG_ERROR("write infos's size failed");
        return;
    }

    for (auto &info : infos) {
        AccessibilityElementInfoParcel infoParcel(info);
        if (!data.WriteParcelable(&infoParcel)) {
            HILOG_ERROR("write accessibility element info failed");
            return;
        }
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_BY_TEXT,
        data, reply, option)) {
        HILOG_ERROR("set search element info by text result failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteParcelable(&infoParcel)) {
        HILOG_ERROR("connection write info failed");
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_FOCUSED_INFO,
        data, reply, option)) {
        HILOG_ERROR("set find focused element info result failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteParcelable(&infoParcel)) {
        HILOG_ERROR("connection write info failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write requestId failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_FOCUS_MOVE,
        data, reply, option)) {
        HILOG_ERROR("set focus move search result failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteBool(succeeded)) {
        HILOG_ERROR("connection write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_PERFORM_ACTION,
        data, reply, option)) {
        HILOG_ERROR("set execute action result failed");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS