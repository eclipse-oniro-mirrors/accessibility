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
#include <vector>
#include "mock_accessible_ability_channel_impl.h"
#include <gtest/gtest.h>
// #include "hilog_wrapper.h"
// #include "ipc_skeleton.h"
// #include "ipc_types.h"
// #include "iremote_object.h"
// #include "accessibility_errorcode.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

bool MockAccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int accessibilityWindowId, 
        const long elementId, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback,
        const int mode) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SearchElementInfoByAccessibilityId start";
    return true;
}

bool MockAccessibleAbilityChannel::SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
        const std::string &text, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SearchElementInfosByText start";
    return true;
}

bool MockAccessibleAbilityChannel::FindFocusedElementInfo(const int accessibilityWindowId, const long elementId, 
        const int focusType, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::FindFocusedElementInfo start";
    return true;
}

bool MockAccessibleAbilityChannel::FocusMoveSearch(const int accessibilityWindowId, const long elementId, 
        const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::FocusMoveSearch start";
    return true;
}

bool MockAccessibleAbilityChannel::PerformAction(const int accessibilityWindowId, const long elementId, const int action, 
        std::map<std::string, std::string> &actionArguments, const int requestId, 
        const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::PerformAction start";
    return true;
}

std::vector<AccessibilityWindowInfo> MockAccessibleAbilityChannel::GetWindows() {
    vector<AccessibilityWindowInfo> windows;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetWindows start";
    return windows;
}

bool MockAccessibleAbilityChannel::PerformCommonAction(const int action) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::PerformCommonAction start";
    return true;
}

void MockAccessibleAbilityChannel::DisableAbility() {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::DisableAbility start";
}

void MockAccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int sequence) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SetOnKeyPressEventResult start";
}

float MockAccessibleAbilityChannel::GetDisplayResizeScale(const int displayId) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeScale start";
    return 1;
}

float MockAccessibleAbilityChannel::GetDisplayResizeCenterX(const int displayId) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeCenterX start";
    return 1;
}

float MockAccessibleAbilityChannel::GetDisplayResizeCenterY(const int displayId) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeCenterY start";
    return 1;
}

Rect MockAccessibleAbilityChannel::GetDisplayResizeRect(const int displayId) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::GetDisplayResizeRect start";
    Rect rect(1, 2, 3, 4);
    return rect;
}

bool MockAccessibleAbilityChannel::ResetDisplayResize(const int displayId, const bool animate) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::ResetDisplayResize start";
    return true;
}

bool MockAccessibleAbilityChannel::SetDisplayResizeScaleAndCenter(const int displayId, const float scale, const float centerX, 
        const float centerY, const bool animate) {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SetDisplayResizeScaleAndCenter start";
    return true;
}

void MockAccessibleAbilityChannel::SendSimulateGesture(const int sequence, const std::vector<GesturePathDefine> &gestureSteps) 
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::SendSimulateGesture start";
}

bool MockAccessibleAbilityChannel::IsFingerprintGestureDetectionValid() {
    GTEST_LOG_(INFO) << "MockAccessibleAbilityChannel::IsFingerprintGestureDetectionValid start";
    return true;
}

sptr<IRemoteObject> MockAccessibleAbilityChannel::AsObject() {
    sptr<IRemoteObject> object;
    return object;
}
} //namespace Accessibility
} //namespace OHOS