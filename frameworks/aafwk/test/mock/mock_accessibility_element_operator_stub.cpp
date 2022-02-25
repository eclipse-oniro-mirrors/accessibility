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
#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_element_operator_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
using AccessibilityElementOperatorCallbacks = std::map<const int, const sptr<IAccessibilityElementOperatorCallback>>;
std::map<const int, const sptr<IAccessibilityElementOperatorCallback>>
    AccessibilityElementOperatorStub::aaCallbacks_ = {};
AccessibilityElementOperatorStub::AccessibilityElementOperatorStub()
{}

AccessibilityElementOperatorStub::~AccessibilityElementOperatorStub()
{}

int AccessibilityElementOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
 MessageParcel &reply, MessageOption &option)
{
    return 0;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

void AccessibilityElementOperatorStub::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode)
{}

void AccessibilityElementOperatorStub::SearchElementInfosByText(const long elementId,
    const std::string &text,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{}

void AccessibilityElementOperatorStub::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{}

void AccessibilityElementOperatorStub::FocusMoveSearch(const long elementId,
    const int direction, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{}

void AccessibilityElementOperatorStub::ExecuteAction(const long elementId,
    const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{}

void AccessibilityElementOperatorStub::ClearFocus()
{}

void AccessibilityElementOperatorStub::OutsideTouch()
{}

AccessibilityElementOperatorStub::CallbackImpl::CallbackImpl()
{}

AccessibilityElementOperatorStub::CallbackImpl::CallbackImpl(const int requestId,
    CALL_API_NUM callNum)
{
    requestId_ = requestId;
    callNum_ = callNum;
}

void AccessibilityElementOperatorStub::CallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityElementOperatorStub::CallbackImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityElementOperatorStub::CallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityElementOperatorStub::CallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityElementOperatorStub::CallbackImpl::SetExecuteActionResult(
    const bool succeeded, const int requestId)
{}

AccessibilityElementOperatorCallbacks AccessibilityElementOperatorStub::CallbackImpl::GetAACallbackList()
{
    return aaCallbacks_;
}

void AccessibilityElementOperatorStub::CallbackImpl::RemoveAACallbackList(int requestId)
{}

} // namespace Accessibility
} // namespace OHOS