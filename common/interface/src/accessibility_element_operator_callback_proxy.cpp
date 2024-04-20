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
constexpr int32_t SINGLE_TRANSMIT = -2;
constexpr int32_t MULTI_TRANSMIT_FINISH = -1;
constexpr int32_t DATA_NUMBER_ONE_TIME = 800;

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

int32_t AccessibilityElementOperatorCallbackProxy::GetTransmitFlag(int32_t time, int32_t leftSize)
{
    int32_t flag = time;
    if (time == 0 && leftSize <= DATA_NUMBER_ONE_TIME) {
        flag = SINGLE_TRANSMIT;
    } else if (leftSize <= DATA_NUMBER_ONE_TIME) {
        flag = MULTI_TRANSMIT_FINISH;
    }

    return flag;
}

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("infos size %{public}zu, resquestId %{public}d", infos.size(), requestId);
    int32_t leftSize = static_cast<int32_t>(infos.size());
    int32_t time = 0;
    int32_t index = 0;
    while (leftSize >= 0) {
        MessageParcel data;
        MessageParcel reply;
        MessageOption type = MessageOption::TF_SYNC;
        if (leftSize <= DATA_NUMBER_ONE_TIME) {
            type = MessageOption::TF_ASYNC;
        }
        MessageOption option(type);
        data.SetMaxCapacity(IPC_MEMORY_SIZE);
        
        // when infos size is a multi of 800, do not send the last empty reply
        if (leftSize == 0 && time > 0) {
            return;
        }
        if (!WriteInterfaceToken(data)) {
            return;
        }

        int32_t flag = GetTransmitFlag(time, leftSize);
        if (!data.WriteInt32(flag)) {
            return;
        }

        int32_t writeSize = (leftSize <= DATA_NUMBER_ONE_TIME) ? leftSize : DATA_NUMBER_ONE_TIME;
        if (!data.WriteInt32(writeSize)) {
            return;
        }

        for (int32_t i = 0; i < writeSize; ++i) {
            AccessibilityElementInfoParcel infoParcel(infos[index]);
            index++;
            if (!data.WriteParcelable(&infoParcel)) {
                HILOG_ERROR("accessibility element info failed index %{public}d", index);
                return;
            }
        }

        if (!data.WriteInt32(requestId)) {
            return;
        }

        if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_BY_ACCESSIBILITY_ID, data, reply, option)) {
            HILOG_ERROR("set search element info by accessibility id result failed");
            return;
        }
    
        leftSize -= DATA_NUMBER_ONE_TIME;
        time++;
        if (static_cast<RetError>(reply.ReadInt32()) != RET_OK) {
            return;
        }
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

void AccessibilityElementOperatorCallbackProxy::SetCursorPositionResult(const int32_t cursorPosition,
    const int32_t requestId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
    HILOG_INFO(" [cursorPosition:%{public}d]", cursorPosition);
    if (!data.WriteInt32(cursorPosition)) {
        HILOG_ERROR("connection write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_RESULT_CURSOR_RESULT,
        data, reply, option)) {
        HILOG_ERROR("set cursor position result failed");
        return;
    }
}

} // namespace Accessibility
} // namespace OHOS
