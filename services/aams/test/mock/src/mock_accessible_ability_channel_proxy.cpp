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

#include "accessibility_element_info.h"
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
    AccessibilityInterfaceCode code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

RetError AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    const int32_t mode, bool isFilter, bool systemApi)
{
    (void)elementBasicInfo;
    (void)requestId;
    (void)callback;
    (void)mode;
    (void)isFilter;
    (void)systemApi;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SearchDefaultFocusedByWindowId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    const int32_t mode, bool isFilter)
{
    (void)elementBasicInfo;
    (void)requestId;
    (void)callback;
    (void)mode;
    (void)isFilter;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string& text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback, bool systemApi)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)text;
    (void)systemApi;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback, bool systemApi)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)focusType;
    (void)systemApi;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    bool systemApi)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)direction;
    (void)systemApi;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::EnableScreenCurtain(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::HoldRunningLock()
{
    return RET_OK;
}
 
RetError AccessibleAbilityChannelProxy::UnholdRunningLock()
{
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)actionArguments;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    (void)windowId;
    (void)windowInfo;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi)
{
    (void)windows;
    (void)systemApi;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows, bool systemApi)
{
    (void)displayId;
    (void)windows;
    (void)systemApi;
    return RET_OK;
}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    (void)handled;
    (void)sequence;
}

RetError AccessibleAbilityChannelProxy::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    (void)gesturePath;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    (void)targetBundleNames;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SetIsRegisterDisconnectCallback(bool isRegister)
{
    (void)isRegister;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::NotifyDisconnect()
{
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS