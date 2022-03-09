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

#include <gtest/gtest.h>
#include <vector>
#include "mock_accessible_ability_channel_impl.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
const int RECT_ONE = 1;
const int RECT_TWO = 2;
const int RECT_THREE = 3;
const int RECT_FOUR = 4;
bool MockAccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
    const long elementId, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SearchElementInfoByAccessibilityId start";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)mode;
    return true;
}

bool MockAccessibleAbilityChannel::SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
    const std::string &text, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SearchElementInfosByText start";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)requestId;
    (void)callback;
    return true;
}

bool MockAccessibleAbilityChannel::FindFocusedElementInfo(const int accessibilityWindowId, const long elementId,
    const int focusType, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::FindFocusedElementInfo start";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)requestId;
    (void)callback;
    return true;
}

bool MockAccessibleAbilityChannel::FocusMoveSearch(const int accessibilityWindowId, const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::FocusMoveSearch start";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)requestId;
    (void)callback;
    return true;
}

bool MockAccessibleAbilityChannel::ExecuteAction(const int accessibilityWindowId, const long elementId,
    const int action, std::map<std::string, std::string> &actionArguments,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::ExecuteAction start";
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    (void)requestId;
    (void)callback;
    return true;
}

std::vector<AccessibilityWindowInfo> MockAccessibleAbilityChannel::GetWindows()
{
    vector<AccessibilityWindowInfo> windows;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetWindows start";
    return windows;
}

bool MockAccessibleAbilityChannel::ExecuteCommonAction(const int action)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::ExecuteCommonAction start";
    (void)action;
    return true;
}

void MockAccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int sequence)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SetOnKeyPressEventResult start";
    (void)handled;
    (void)sequence;
}

float MockAccessibleAbilityChannel::GetDisplayResizeScale(const int displayId)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeScale start";
    (void)displayId;
    return 1.0f;
}

float MockAccessibleAbilityChannel::GetDisplayResizeCenterX(const int displayId)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeCenterX start";
    (void)displayId;
    return 1.0f;
}

float MockAccessibleAbilityChannel::GetDisplayResizeCenterY(const int displayId)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeCenterY start";
    (void)displayId;
    return 1.0f;
}

Rect MockAccessibleAbilityChannel::GetDisplayResizeRect(const int displayId)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeRect start";
    (void)displayId;
    Rect rect(RECT_ONE,  RECT_TWO,  RECT_THREE,  RECT_FOUR);
    return rect;
}

bool MockAccessibleAbilityChannel::ResetDisplayResize(const int displayId, const bool animate)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::ResetDisplayResize start";
    (void)displayId;
    (void)animate;
    return true;
}

bool MockAccessibleAbilityChannel::SetDisplayResizeScaleAndCenter(const int displayId,
    const float scale, const float centerX, const float centerY, const bool animate)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SetDisplayResizeScaleAndCenter start";
    (void)displayId;
    (void)scale;
    (void)centerX;
    (void)centerY;
    (void)animate;
    return true;
}

void MockAccessibleAbilityChannel::SendSimulateGesture(const int requestId,
    const std::vector<GesturePathDefine> &gestureSteps)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SendSimulateGesture start";
    (void)requestId;
    (void)gestureSteps;
}

sptr<IRemoteObject> MockAccessibleAbilityChannel::AsObject()
{
    sptr<IRemoteObject> object;
    return object;
}
} // namespace Accessibility
} // namespace OHOS