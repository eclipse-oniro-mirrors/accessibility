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
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_system_ability_client.h"
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
using AccessibilityElementOperatorCallbacks =
    std::map<const int32_t, const sptr<IAccessibilityElementOperatorCallback>>;
AccessibilityElementOperatorStub::AccessibilityElementOperatorStub()
{}

AccessibilityElementOperatorStub::~AccessibilityElementOperatorStub()
{
    memberFuncMap_.clear();
}

int AccessibilityElementOperatorStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return 0;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfoByAccessibilityId(
    MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfosByText(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFindFocusedElementInfo(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusFind(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleExecuteAction(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleClearFocus(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleOutsideTouch(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS