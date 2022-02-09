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
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(IAccessibleAbilityChannel::Message code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
    const long elementId, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback,
    const int mode)
{
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfosByText(const int accessibilityWindowId,
    const long elementId, const std::string &text, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    return true;
}

bool AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int accessibilityWindowId,
    const long elementId, const int focusType, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    return true;
}

bool AccessibleAbilityChannelProxy::FocusMoveSearch(const int accessibilityWindowId, const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    return true;
}

bool AccessibleAbilityChannelProxy::PerformAction(const int accessibilityWindowId, const long elementId,
    const int action, std::map<std::string, std::string> &actionArguments, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    return true;
}

vector<AccessibilityWindowInfo> AccessibleAbilityChannelProxy::GetWindows()
{
    vector<AccessibilityWindowInfo> windows {};
    return windows;
}

bool AccessibleAbilityChannelProxy::PerformCommonAction(const int action)
{
    return true;
}

void AccessibleAbilityChannelProxy::DisableAbility()
{}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int sequence)
{}

float AccessibleAbilityChannelProxy::GetDisplayResizeScale(const int displayId)
{
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterX(const int displayId)
{
    return 0;
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterY(const int displayId)
{
    return 0;
}

Rect AccessibleAbilityChannelProxy::GetDisplayResizeRect(const int displayId)
{
    Rect rect(0, 0, 0, 0);
    return rect;
}

bool AccessibleAbilityChannelProxy::ResetDisplayResize(const int displayId, const bool animate)
{
    return true;
}

bool AccessibleAbilityChannelProxy::SetDisplayResizeScaleAndCenter(const int displayId, const float scale,
                                        const float centerX, const float centerY, const bool animate)
{
    return true;
}

void AccessibleAbilityChannelProxy::SendSimulateGesture(const int sequence,
                                                            const std::vector<GesturePathDefine> &gestureSteps)
{}

bool AccessibleAbilityChannelProxy::IsFingerprintGestureDetectionValid()
{
    return true;
}

} // namespace Accessibility
} // namespace OHOS