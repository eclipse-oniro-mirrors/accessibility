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

bool AccessibilityElementOperatorProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

RetError AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)requestId;
    (void)callback;
    (void)mode;
    (void)isFilter;
    return RET_OK;
}

void AccessibilityElementOperatorProxy::SearchDefaultFocusedByWindowId(const int32_t windowId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    (void)windowId;
    (void)requestId;
    (void)callback;
    (void)mode;
    (void)isFilter;
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const int64_t elementId,
    const std::string &text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)text;
    (void)requestId;
    (void)callback;
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)focusType;
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    if (callback) {
        AccessibilityElementInfo info;
        callback->SetFindFocusedElementInfoResult(info, requestId);
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    (void)direction;
    (void)requestId;
    (void)callback;
}

void AccessibilityElementOperatorProxy::ExecuteAction(const int64_t elementId, const int32_t action,
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

void AccessibilityElementOperatorProxy::GetCursorPosition(const int64_t elementId, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(elementId);
    if (callback) {
        callback->SetCursorPositionResult(1, requestId);
    }
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
}

void AccessibilityElementOperatorProxy::SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId,
    const int32_t childWindowId)
{
    (void)elementId;
    (void)treeId;
    (void)childWindowId;
}

void AccessibilityElementOperatorProxy::SetBelongTreeId(const int32_t treeId)
{
    (void)treeId;
}

void AccessibilityElementOperatorProxy::SetParentWindowId(const int32_t parentWindowId)
{
    (void)parentWindowId;
}

void AccessibilityElementOperatorProxy::SearchElementInfoBySpecificProperty(const int64_t elementId,
    const SpecificPropertyParam& param, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)elementId;
    (void)param;
    (void)requestId;
    (void)callback;
}

void FocusMoveSearchWithCondition(const int64_t elementId, const AccessibilityFocusMoveParam &param,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)elementId;
    (void)param;
    (void)requestId;
    (void)callback;
}

void DetectElementInfoFocusableThroughAncestor(const AccessibilityElementInfo &info,
    const int64_t parentId, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)info;
    (void)parentId;
    (void)requestId;
    (void)callback;
}
} // namespace Accessibility
} // namespace OHOS