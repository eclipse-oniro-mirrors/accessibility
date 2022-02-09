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
#include "accessibility_interaction_operation_callback_proxy.h"
#include "accessibility_interaction_operation_callback_stub.h"
#include "accessibility_interaction_operation_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>>
    AccessibilityInteractionOperationStub::aaCallbacks_ = {};
AccessibilityInteractionOperationStub::AccessibilityInteractionOperationStub()
{}

AccessibilityInteractionOperationStub::~AccessibilityInteractionOperationStub()
{}

int AccessibilityInteractionOperationStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
 MessageParcel &reply, MessageOption &option)
{
    return 0;
}

ErrCode AccessibilityInteractionOperationStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandlePerformAction(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

void AccessibilityInteractionOperationStub::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode)
{}

void AccessibilityInteractionOperationStub::SearchElementInfosByText(const long elementId,
    const std::string &text,
    const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{}

void AccessibilityInteractionOperationStub::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{}

void AccessibilityInteractionOperationStub::FocusMoveSearch(const long elementId,
    const int direction, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{}

void AccessibilityInteractionOperationStub::PerformAction(const long elementId,
    const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{}

void AccessibilityInteractionOperationStub::ClearFocus()
{}

void AccessibilityInteractionOperationStub::OutsideTouch()
{}

AccessibilityInteractionOperationStub::CallbackImpl::CallbackImpl()
{}

AccessibilityInteractionOperationStub::CallbackImpl::CallbackImpl(const int requestId,
    CALL_API_NUM callNum)
{
    requestId_ = requestId;
    callNum_ = callNum;
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityInteractionOperationStub::CallbackImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityInteractionOperationStub::CallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityInteractionOperationStub::CallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityInteractionOperationStub::CallbackImpl::SetPerformActionResult(
    const bool succeeded, const int requestId)
{}

std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>>
    AccessibilityInteractionOperationStub::CallbackImpl::GetAACallbackList()
{
    return aaCallbacks_;
}

void AccessibilityInteractionOperationStub::CallbackImpl::RemoveAACallbackList(int requestId)
{}

} //namespace Accessibility
} //namespace OHOS