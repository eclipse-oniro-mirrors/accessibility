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
#include "accessibility_element_info_parcel.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

#define SWITCH_BEGIN(code) switch (code) {
#define SWITCH_CASE(case_code, func) case case_code:\
    {\
        result_code = func(data, reply);\
        break;\
    }

#define SWITCH_END() default:\
    {\
        result_code = ERR_CODE_DEFAULT;\
        HILOG_WARN("AccessibilityElementOperatorCallbackStub::OnRemoteRequest, default case, need check.");\
        break;\
    }\
}

#define ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_STUB_CASES() \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_BY_ACCESSIBILITY_ID, HandleSetSearchElementInfoByAccessibilityIdResult)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_BY_TEXT, HandleSetSearchElementInfoByTextResult)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_FOCUSED_INFO, HandleSetFindFocusedElementInfoResult)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_FOCUS_MOVE, HandleSetFocusMoveSearchResult)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_PERFORM_ACTION, HandleSetExecuteActionResult)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_RESULT_CURSOR_RESULT, HandleSetCursorPositionResult)\

namespace OHOS {
namespace Accessibility {

constexpr int32_t SINGLE_TRANSMIT = -2;
constexpr int32_t MULTI_TRANSMIT_FINISH = -1;
constexpr int32_t ERR_CODE_DEFAULT = -1000;

void StoreElementData::WriteData(std::vector<AccessibilityElementInfo> &infos)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    storeData_.insert(storeData_.end(), infos.begin(), infos.end());
}

void StoreElementData::Clear()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    storeData_.clear();
}

std::vector<AccessibilityElementInfo> StoreElementData::ReadData()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    return storeData_;
}

size_t StoreElementData::Size()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    return storeData_.size();
}

AccessibilityElementOperatorCallbackStub::AccessibilityElementOperatorCallbackStub()
{
}

StoreElementData AccessibilityElementOperatorCallbackStub::storeElementData;

AccessibilityElementOperatorCallbackStub::~AccessibilityElementOperatorCallbackStub()
{
    HILOG_DEBUG();
}

int AccessibilityElementOperatorCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}u, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibilityElementOperatorCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("AccessibilityElementOperatorCallbackStub::OnRemoteRequest"
            "local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    ErrCode result_code = ERR_NONE;
    SWITCH_BEGIN(code)
    ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_STUB_CASES()
    SWITCH_END()

    if (result_code != ERR_CODE_DEFAULT) {
        return result_code;
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByAccessibilityIdResult(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t flag = data.ReadInt32();
    if (flag == SINGLE_TRANSMIT || flag == 0) {
        storeElementData.Clear();
    }

    int32_t accessibilityInfosize = data.ReadInt32();
    std::vector<AccessibilityElementInfo> tmpData;
    bool verifyResult = ContainerSecurityVerify(data, accessibilityInfosize, tmpData.max_size());
    if (!verifyResult || accessibilityInfosize < 0 || accessibilityInfosize > INT32_MAX) {
        return TRANSACTION_ERR;
    }
    for (int32_t i = 0; i < accessibilityInfosize; i++) {
        sptr<AccessibilityElementInfoParcel> accessibilityInfo =
            data.ReadStrongParcelable<AccessibilityElementInfoParcel>();
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadStrongParcelable<accessibilityInfo> failed");
            storeElementData.Clear();
            reply.WriteInt32(RET_ERR_FAILED);
            return TRANSACTION_ERR;
        }
        tmpData.push_back(*accessibilityInfo);
    }

    storeElementData.WriteData(tmpData); // get all data and push once
    int32_t requestId = data.ReadInt32();
    if (flag == SINGLE_TRANSMIT || flag == MULTI_TRANSMIT_FINISH) {
        reply.WriteInt32(0);
        HILOG_DEBUG("infos size %{public}zu, requestId %{public}d", storeElementData.Size(), requestId);
        SetSearchElementInfoByAccessibilityIdResult(storeElementData.ReadData(), requestId);
        return NO_ERROR;
    }

    reply.WriteInt32(0);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetSearchElementInfoByTextResult(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<AccessibilityElementInfo> infos {};
    int32_t accessibilityInfosize = data.ReadInt32();
    bool verifyResult = ContainerSecurityVerify(data, accessibilityInfosize, infos.max_size());
    if (!verifyResult || accessibilityInfosize < 0 || accessibilityInfosize > INT32_MAX) {
        return TRANSACTION_ERR;
    }
    for (int32_t i = 0; i < accessibilityInfosize; i++) {
        sptr<AccessibilityElementInfoParcel> accessibilityInfo =
            data.ReadStrongParcelable<AccessibilityElementInfoParcel>();
        if (!accessibilityInfo) {
            HILOG_ERROR("ReadStrongParcelable<accessibilityInfo> failed");
            return TRANSACTION_ERR;
        }
        infos.emplace_back(*accessibilityInfo);
    }
    int32_t requestId = data.ReadInt32();

    SetSearchElementInfoByTextResult(infos, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetFindFocusedElementInfoResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<AccessibilityElementInfoParcel> info = data.ReadStrongParcelable<AccessibilityElementInfoParcel>();
    if (!info) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityElementInfo> failed");
        return TRANSACTION_ERR;
    }

    int32_t requestId = data.ReadInt32();

    SetFindFocusedElementInfoResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetFocusMoveSearchResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<AccessibilityElementInfoParcel> info = data.ReadStrongParcelable<AccessibilityElementInfoParcel>();
    if (!info) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityElementInfo> failed");
        return TRANSACTION_ERR;
    }

    int32_t requestId = data.ReadInt32();

    SetFocusMoveSearchResult(*info, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetExecuteActionResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    bool succeeded = data.ReadBool();
    int32_t requestId = data.ReadInt32();

    SetExecuteActionResult(succeeded, requestId);

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorCallbackStub::HandleSetCursorPositionResult(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t cursorPosition = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    HILOG_INFO("[cursorPosition:%{public}d]", cursorPosition);
    SetCursorPositionResult(cursorPosition, requestId);

    return NO_ERROR;
}

} // namespace Accessibility
} // namespace OHOS