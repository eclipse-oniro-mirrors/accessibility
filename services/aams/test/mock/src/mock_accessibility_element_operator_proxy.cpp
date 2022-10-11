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

#include "accessibility_element_operator_proxy.h"
#include "accessibility_ut_helper.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorProxy::AccessibilityElementOperatorProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperator>(impl)
{}

bool AccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel &data)
{
    (void)data;
    return true;
}

bool AccessibilityElementOperatorProxy::SendTransactCmd(IAccessibilityElementOperator::Message code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

void AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int32_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)requestId;
    (void)callback;
    (void)mode;
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const int32_t elementId,
    const std::string &text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)text;
    (void)requestId;
    (void)callback;
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const int32_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)focusType;
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    if (callback) {
        AccessibilityElementInfo info;
        callback->SetFindFocusedElementInfoResult(info, requestId);
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)direction;
    (void)requestId;
    (void)callback;
}

void AccessibilityElementOperatorProxy::ExecuteAction(const int32_t elementId, const int32_t action,
    const std::map<std::string, std::string> &arguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)arguments;
    AccessibilityAbilityHelper::GetInstance().SetExecuteAction(action);
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    if (callback) {
        callback->SetExecuteActionResult(true, requestId);
    }
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
}
} // namespace Accessibility
} // namespace OHOS