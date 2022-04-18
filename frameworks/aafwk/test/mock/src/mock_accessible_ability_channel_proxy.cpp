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

#include "mock_accessible_ability_channel_proxy.h"
#include "accessible_ability_channel_proxy.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelProxy::AccessibleAbilityChannelProxy(const sptr<IRemoteObject>& object)
    : IRemoteProxy<IAccessibleAbilityChannel>(object)
{}

bool AccessibleAbilityChannelProxy::WriteInterfaceToken(MessageParcel& data)
{
    (void)data;
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(
    IAccessibleAbilityChannel::Message code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    const int32_t mode)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)mode;
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string& text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannelProxy::ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, std::map<std::string, std::string>& actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    (void)requestId;
    (void)callback;
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibleAbilityChannelProxy::GetWindows()
{
    std::vector<AccessibilityWindowInfo> windows {};
    return windows;
}

bool AccessibleAbilityChannelProxy::ExecuteCommonAction(const int32_t action)
{
    (void)action;
    return true;
}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    (void)handled;
    (void)sequence;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeScale(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterX(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterY(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

Rect AccessibleAbilityChannelProxy::GetDisplayResizeRect(const int32_t displayId)
{
    (void)displayId;
    Rect rect(0, 0, 0, 0);
    return rect;
}

bool AccessibleAbilityChannelProxy::ResetDisplayResize(const int32_t displayId, const bool animate)
{
    (void)displayId;
    (void)animate;
    return true;
}

bool AccessibleAbilityChannelProxy::SetDisplayResizeScaleAndCenter(
    const int32_t displayId, const float scale, const float centerX, const float centerY, const bool animate)
{
    (void)displayId;
    (void)scale;
    (void)centerX;
    (void)centerY;
    (void)animate;
    return true;
}

void AccessibleAbilityChannelProxy::SendSimulateGesture(
    const int32_t requestId, const std::vector<AccessibilityGesturePath>& gestureSteps)
{
    (void)requestId;
    (void)gestureSteps;
}

MockAccessibleAbilityChannelProxy::MockAccessibleAbilityChannelProxy(const sptr<IRemoteObject>& object)
    : AccessibleAbilityChannelProxy(object)
{}
} // namespace Accessibility
} // namespace OHOS