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
#include "accessibility_interaction_operation_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include  <stdio.h>

int testElementId = -1;
int testClearFocus = -1;
int testOutsideTouch = -1;

int testChannelElementId = -1;
int testChannelrequestId = -1;
int testChannelmode = -1;
std::string testText = "";
int testFocusType = 0;
int testDirection = 0;
int testAction = 0;
std::map<std::string, std::string> testActionArguments;

namespace OHOS {
namespace Accessibility {
AccessibilityInteractionOperationProxy::AccessibilityInteractionOperationProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityInteractionOperation>(impl)
{
    printf("AccessibilityInteractionOperationProxymock");
}

AccessibilityInteractionOperationProxy::~AccessibilityInteractionOperationProxy()
{}

bool AccessibilityInteractionOperationProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibilityInteractionOperationProxy::SearchElementInfoByAccessibilityId(const long elementId,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode)
{
    printf("AccessibilityInteractionOperationProxymock SearchElementInfoByAccessibilityId");
    testElementId = elementId;
    testChannelElementId = elementId;
    testChannelrequestId = requestId;
    testChannelmode = mode;
    return;
}

void AccessibilityInteractionOperationProxy::SearchElementInfosByText(const long elementId,
        const std::string &text,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    testElementId = elementId;
    testChannelElementId = elementId;
    testText = text;
    testChannelrequestId = requestId;
    return;
}

void AccessibilityInteractionOperationProxy::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    testElementId = elementId;
    testChannelElementId = elementId;
    testFocusType = focusType;
    testChannelrequestId = requestId;
    return;
}

void AccessibilityInteractionOperationProxy::FocusMoveSearch(const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    testElementId = elementId;
    testChannelElementId = elementId;
    testDirection = direction;
    testChannelrequestId = requestId;
    return;
}

void AccessibilityInteractionOperationProxy::PerformAction(const long elementId, const int action,
        const std::map<std::string, std::string> arguments, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    testElementId = elementId;
    testChannelElementId = elementId;
    testActionArguments = arguments;
    testChannelrequestId = requestId;
    testAction = action;
    return;
}

void AccessibilityInteractionOperationProxy::ClearFocus()
{
    testClearFocus = 1;
    return;
}

void AccessibilityInteractionOperationProxy::OutsideTouch()
{
    testOutsideTouch = 1;
    return;
}

} //namespace Accessibility
} //namespace OHOS