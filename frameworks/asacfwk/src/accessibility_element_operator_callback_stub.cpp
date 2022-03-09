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

#include "accessibility_element_operator_callback_stub.h"

#include "accessibility_errorcode.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorCallbackStub::AccessibilityElementOperatorCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityElementOperatorCallback::Message::SET_RESULT_BY_ACCESSIBILITY_ID)] =
        &AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByAccessibilityIdResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperatorCallback::Message::SET_RESULT_BY_TEXT)] =
        &AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByTextResult;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityElementOperatorCallback::Message::SET_RESULT_FOCUSED_INFO)] =
        &AccessibilityElementOperatorCallbackStub::HandleSetFindFocusedElementInfoResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperatorCallback::Message::SET_RESULT_FOCUS_MOVE)] =
        &AccessibilityElementOperatorCallbackStub::HandleSetFocusMoveSearchResult;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityElementOperatorCallback::Message::SET_RESULT_PERFORM_ACTION)] =
        &AccessibilityElementOperatorCallbackStub::HandleSetExecuteActionResult;
}

AccessibilityElementOperatorCallbackStub::~AccessibilityElementOperatorCallbackStub()
{
    HILOG_DEBUG("start.");
    memberFuncMap_.clear();
}

int AccessibilityElementOperatorCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibilityElementOperatorCallbackStub::OnRemoteRequest, cmd = %d, flags= %d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibilityElementOperatorCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }
    HILOG_WARN("AccessibilityElementOperatorCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByAccessibilityIdResult(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityElementInfo> infos {};
    int32_t accessibilityInfosize = data.ReadInt32();

    if (!accessibilityInfosize) {
        HILOG_ERROR("accessibilityInfosize is 0");
        return ERROR;
    }

    HILOG_DEBUG("dataSize: %{public}d, offsetSize: %{public}d, elementSize: %{public}d",
        data.GetDataSize(), data.GetOffsetsSize(), accessibilityInfosize * sizeof(AccessibilityElementInfo));
    if (data.GetDataSize() - data.GetOffsetsSize() < accessibilityInfosize * sizeof(AccessibilityElementInfo)) {
        HILOG_ERROR("ReadParcelable invalid size");
        return ERROR;
    }

    for (int i = 0; i < accessibilityInfosize; i++) {
        sptr<AccessibilityElementInfo> accessibilityInfo = data.ReadStrongParcelable<AccessibilityElementInfo>();
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadStrongParcelable<accessibilityInfo> failed");
            return ERROR;
        }
        infos.emplace_back(*accessibilityInfo);
    }
    int requestId = data.ReadInt32();

    SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByTextResult(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityElementInfo> infos {};
    int32_t accessibilityInfosize = data.ReadInt32();
    if (!accessibilityInfosize) {
        HILOG_ERROR("accessibilityInfosize is 0");
        return ERROR;
    }

    HILOG_DEBUG("dataSize: %{public}d, offsetSize: %{public}d, elementSize: %{public}d",
        data.GetDataSize(), data.GetOffsetsSize(), accessibilityInfosize * sizeof(AccessibilityElementInfo));
    if (data.GetDataSize() - data.GetOffsetsSize() < accessibilityInfosize * sizeof(AccessibilityElementInfo)) {
        HILOG_ERROR("ReadParcelable invalid size");
        return ERROR;
    }

    for (int i = 0; i < accessibilityInfosize; i++) {
        sptr<AccessibilityElementInfo> accessibilityInfo = data.ReadStrongParcelable<AccessibilityElementInfo>();
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadStrongParcelable<accessibilityInfo> failed");
            return ERROR;
        }
        infos.emplace_back(*accessibilityInfo);
    }
    int requestId = data.ReadInt32();

    SetSearchElementInfoByTextResult(infos, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetFindFocusedElementInfoResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityElementInfo> info = data.ReadStrongParcelable<AccessibilityElementInfo>();
    if (!info) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityElementInfo> failed");
        return ERROR;
    }

    int requestId = data.ReadInt32();

    SetFindFocusedElementInfoResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetFocusMoveSearchResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityElementInfo> info = data.ReadStrongParcelable<AccessibilityElementInfo>();
    if (!info) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityElementInfo> failed");
        return ERROR;
    }

    int requestId = data.ReadInt32();

    SetFocusMoveSearchResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetExecuteActionResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool succeeded = data.ReadBool();
    int requestId = data.ReadInt32();

    SetExecuteActionResult(succeeded, requestId);

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS