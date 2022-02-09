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
#include <vector>
#include "accessibility_interaction_operation_callback_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "accessibility_errorcode.h"

namespace OHOS {
namespace Accessibility {

AccessibilityInteractionOperationCallbackStub::AccessibilityInteractionOperationCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityInteractionOperationCallback::Message::SET_RESULT_BY_ACCESSIBILITY_ID)] =
        &AccessibilityInteractionOperationCallbackStub::HandleSetSearchElementInfoByAccessibilityIdResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_BY_TEXT)] =
        &AccessibilityInteractionOperationCallbackStub::HandleSetSearchElementInfoByTextResult;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityInteractionOperationCallback::Message::SET_RESULT_FOCUSED_INFO)] =
        &AccessibilityInteractionOperationCallbackStub::HandleSetFindFocusedElementInfoResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_FOCUS_MOVE)] =
        &AccessibilityInteractionOperationCallbackStub::HandleSetFocusMoveSearchResult;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibilityInteractionOperationCallback::Message::SET_RESULT_PERFORM_ACTION)] =
        &AccessibilityInteractionOperationCallbackStub::HandleSetPerformActionResult;
}

AccessibilityInteractionOperationCallbackStub::~AccessibilityInteractionOperationCallbackStub()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int AccessibilityInteractionOperationCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
 {
    HILOG_DEBUG("AccessibilityInteractionOperationCallbackStub::OnRemoteRequest, cmd = %d, flags= %d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibilityInteractionOperationCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();

    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data,reply);
        }
    }
    HILOG_WARN("AccessibilityInteractionOperationCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityInteractionOperationCallbackStub::HandleSetSearchElementInfoByAccessibilityIdResult(
    MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    std::vector<AccessibilityElementInfo> infos{};
    int32_t accessibilityInfosize = data.ReadInt32();
    for (int i = 0; i< accessibilityInfosize; i++) {
        std::shared_ptr<AccessibilityElementInfo> accessibilityInfo(data.ReadParcelable<AccessibilityElementInfo>());
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadParcelable<accessibilityInfo> failed");
        }
        infos.emplace_back(*accessibilityInfo);
    }
    int requestId = data.ReadInt32();

    SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationCallbackStub::HandleSetSearchElementInfoByTextResult(
    MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    std::vector<AccessibilityElementInfo> infos{};
    int32_t accessibilityInfosize = data.ReadInt32();
    for (int i = 0; i< accessibilityInfosize; i++) {
        std::shared_ptr<AccessibilityElementInfo> accessibilityInfo(data.ReadParcelable<AccessibilityElementInfo>());
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadParcelable<accessibilityInfo> failed");
        }
        infos.emplace_back(*accessibilityInfo);
    }
    int requestId = data.ReadInt32();

    SetSearchElementInfoByTextResult(infos, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationCallbackStub::HandleSetFindFocusedElementInfoResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);
    std::shared_ptr<AccessibilityElementInfo> info(data.ReadParcelable<AccessibilityElementInfo>());
    if (!info) {
        HILOG_ERROR("ReadParcelable<AccessibilityElementInfo> failed");
        return ERROR;
    }

    int requestId = data.ReadInt32();

    SetFindFocusedElementInfoResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationCallbackStub::HandleSetFocusMoveSearchResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);
    std::shared_ptr<AccessibilityElementInfo> info(data.ReadParcelable<AccessibilityElementInfo>());
    if (!info) {
        HILOG_ERROR("ReadParcelable<AccessibilityElementInfo> failed");
        return ERROR;
    }

    int requestId = data.ReadInt32();

    SetFocusMoveSearchResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationCallbackStub::HandleSetPerformActionResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    bool succeeded = data.ReadBool();
    int requestId = data.ReadInt32();

    SetPerformActionResult(succeeded, requestId);

    return NO_ERROR;
}

} //namespace Accessibility
} //namespace OHOS