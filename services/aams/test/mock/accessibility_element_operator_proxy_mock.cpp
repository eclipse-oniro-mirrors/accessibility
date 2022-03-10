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
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

int g_testElementId = -1;
int g_testClearFocus = -1;
int g_testOutsideTouch = -1;

int g_testChannelElementId = -1;
int g_testChannelrequestId = -1;
int g_testChannelmode = -1;

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorProxy::AccessibilityElementOperatorProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperator>(impl)
{
    printf("AccessibilityElementOperatorProxymock");
}

AccessibilityElementOperatorProxy::~AccessibilityElementOperatorProxy()
{
}

bool AccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode)
{
    printf("AccessibilityElementOperatorProxymock SearchElementInfoByAccessibilityId");
    g_testElementId = elementId;
    g_testChannelElementId = elementId;
    g_testChannelrequestId  = requestId;
    g_testChannelmode = mode;
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const long elementId,
    const std::string &text, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    g_testElementId = elementId;
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    g_testElementId = elementId;
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    g_testElementId = elementId;
}

void AccessibilityElementOperatorProxy::ExecuteAction(const long elementId, const int action,
    const std::map<std::string, std::string> arguments, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    g_testElementId = elementId;
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
    g_testClearFocus = 1;
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
    g_testOutsideTouch = 1;
}
} // namespace Accessibility
} // namespace OHOS