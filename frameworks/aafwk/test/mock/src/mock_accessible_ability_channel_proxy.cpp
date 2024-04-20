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

#include <gtest/gtest.h>
#include "mock_accessible_ability_channel_proxy.h"
#include "accessible_ability_channel_proxy.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelProxy::AccessibleAbilityChannelProxy(const sptr<IRemoteObject>& remoteObj)
    : IRemoteProxy<IAccessibleAbilityChannel>(remoteObj)
{}

bool AccessibleAbilityChannelProxy::WriteInterfaceToken(MessageParcel& data)
{
    (void)data;
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(
    AccessibilityInterfaceCode interfaceCode, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SendTransactCmd";
    (void)interfaceCode;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

RetError AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    const int32_t mode, bool isFilter)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SearchElementInfoByAccessibilityId";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)mode;
    (void)isFilter;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string& text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SearchElementInfosByText";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy FindFocusedElementInfo";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy FocusMoveSearch";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::EnableScreenCurtain(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy ExecuteAction";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy GetCursorPosition";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy GetWindow";
    (void)windowId;
    (void)windowInfo;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy GetWindows";
    (void)windows;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy GetWindowsByDisplayId";
    (void)displayId;
    (void)windows;
    return RET_OK;
}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SetOnKeyPressEventResult";
    (void)handled;
    (void)sequence;
}

RetError AccessibleAbilityChannelProxy::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SendSimulateGesture";
    (void)gesturePath;
    return RET_OK;
}

RetError AccessibleAbilityChannelProxy::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelProxy SetTargetBundleName";
    (void)targetBundleNames;
    return RET_OK;
}

MockAccessibleAbilityChannelProxy::MockAccessibleAbilityChannelProxy(const sptr<IRemoteObject>& object)
    : AccessibleAbilityChannelProxy(object)
{}
} // namespace Accessibility
} // namespace OHOS