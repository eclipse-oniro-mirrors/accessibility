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

#include "accessible_ability_channel_proxy.h"
#include "parcel.h"
#include "accessibility_errorcode.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelProxy::AccessibleAbilityChannelProxy(
    const sptr<IRemoteObject> &object): IRemoteProxy<IAccessibleAbilityChannel>(object)
{}

bool AccessibleAbilityChannelProxy::WriteInterfaceToken(MessageParcel &data)
{
    (void)data;
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(IAccessibleAbilityChannel::Message code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
    const long elementId, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int mode)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)mode;
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfosByText(const int accessibilityWindowId,
    const long elementId, const std::string &text, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int accessibilityWindowId,
    const long elementId, const int focusType, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::FocusMoveSearch(const int accessibilityWindowId, const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::ExecuteAction(const int accessibilityWindowId, const long elementId,
    const int action, std::map<std::string, std::string> &actionArguments, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    (void)requestId;
    (void)callback;
    return true;
}

vector<AccessibilityWindowInfo> AccessibleAbilityChannelProxy::GetWindows()
{
    vector<AccessibilityWindowInfo> windows {};
    return windows;
}

bool AccessibleAbilityChannelProxy::ExecuteCommonAction(const int action)
{
    (void)action;
    return true;
}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int sequence)
{
    (void)handled;
    (void)sequence;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeScale(const int displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterX(const int displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterY(const int displayId)
{
    (void)displayId;
    return 0;
}

Rect AccessibleAbilityChannelProxy::GetDisplayResizeRect(const int displayId)
{
    (void)displayId;
    Rect rect(0, 0, 0, 0);
    return rect;
}

bool AccessibleAbilityChannelProxy::ResetDisplayResize(const int displayId, const bool animate)
{
    (void)displayId;
    (void)animate;
    return true;
}

bool AccessibleAbilityChannelProxy::SetDisplayResizeScaleAndCenter(const int displayId, const float scale,
    const float centerX, const float centerY, const bool animate)
{
    (void)displayId;
    (void)scale;
    (void)centerX;
    (void)centerY;
    (void)animate;
    return true;
}

void AccessibleAbilityChannelProxy::SendSimulateGesture(
    const int requestId, const std::vector<GesturePathDefine> &gestureSteps)
{
    (void)requestId;
    (void)gestureSteps;
}
} // namespace Accessibility
} // namespace OHOS