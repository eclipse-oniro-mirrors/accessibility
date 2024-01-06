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

#include "mock_accessibility_element_operator_proxy.h"
#include <cstdio>
#include <gtest/gtest.h>
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "ipc_types.h"

namespace OHOS {
namespace Accessibility {
MockAccessibilityElementOperatorProxy::MockAccessibilityElementOperatorProxy(const sptr<IRemoteObject>& object)
    : IRemoteProxy<IAccessibilityElementOperator>(object)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy";
}

bool MockAccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel& data)
{
    return true;
}

void MockAccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int32_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy SearchElementInfoByAccessibilityId";
    testElementId_ = elementId;
    testChannelElementId_ = elementId;
    testChannelRequestId_ = requestId;
    testChannelMode_ = mode;
    return;
}

void MockAccessibilityElementOperatorProxy::SearchElementInfosByText(const int32_t elementId, const std::string& text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy SearchElementInfosByText";
    testElementId_ = elementId;
    testChannelElementId_ = elementId;
    testText_ = text;
    testChannelRequestId_ = requestId;
    return;
}

void MockAccessibilityElementOperatorProxy::FindFocusedElementInfo(const int32_t elementId, const int32_t focusType,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy FindFocusedElementInfo";
    testElementId_ = elementId;
    testChannelElementId_ = elementId;
    testFocusType_ = focusType;
    testChannelRequestId_ = requestId;
    return;
}

void MockAccessibilityElementOperatorProxy::FocusMoveSearch(const int32_t elementId, const int32_t direction,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy FocusMoveSearch";
    testElementId_ = elementId;
    testChannelElementId_ = elementId;
    testDirection_ = direction;
    testChannelRequestId_ = requestId;
    return;
}

void MockAccessibilityElementOperatorProxy::ExecuteAction(const int32_t elementId, const int32_t action,
    const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy ExecuteAction";
    testElementId_ = elementId;
    testChannelElementId_ = elementId;
    testActionArguments_ = actionArguments;
    testChannelRequestId_ = requestId;
    testAction_ = action;
    return;
}

void MockAccessibilityElementOperatorProxy::ClearFocus()
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy ClearFocus";
    testClearFocus_ = 1;
    return;
}

void MockAccessibilityElementOperatorProxy::OutsideTouch()
{
    GTEST_LOG_(INFO) << "MockAccessibilityElementOperatorProxy OutsideTouch";
    testOutsideTouch_ = 1;
    return;
}
} // namespace Accessibility
} // namespace OHOS