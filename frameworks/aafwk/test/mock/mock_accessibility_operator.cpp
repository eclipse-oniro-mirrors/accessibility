/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_operator.h"
#include "accessibility_element_info.h"
#include <sys/time.h>
#include <unistd.h>
#include <gtest/gtest.h>

namespace OHOS {
namespace Accessibility {
std::map<int, sptr<IAccessibleAbilityChannel>> AccessibilityOperator::channels_ = {};
std::vector<sptr<AccessibilityOperator>>  AccessibilityOperator::instances_ = {};
std::recursive_mutex AccessibilityOperator::mutex_ = {};
int AccessibilityOperator::requestId_ = 0;

AccessibilityOperator::AccessibilityOperator()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator";
    performActionResult_ = true;
    responseId_ = 0;
}

AccessibilityOperator &AccessibilityOperator::GetInstance()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetInstance";
    static AccessibilityOperator accessibilityOperator;
    return accessibilityOperator;
}

AccessibilityOperator &AccessibilityOperator::GetInstanceForThread(std::thread::id threadId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetInstanceForThread";
    static AccessibilityOperator accessibilityOperator;
    return accessibilityOperator;
}

sptr<IAccessibleAbilityChannel> AccessibilityOperator::GetChannel(int channelId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetChannel";
    return nullptr;
}

bool AccessibilityOperator::GetOperationStatus()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetOperationStatus";
    return completed_;
}

void AccessibilityOperator::AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator AddChannel";
}

void AccessibilityOperator::RemoveChannel(int channelId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator RemoveChannel";
}

bool AccessibilityOperator::GetRoot(int channelId, AccessibilityElementInfo &elementInfo)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetRoot";
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibilityOperator::GetWindows(int channelId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetWindows";
    return windows_;
}

bool AccessibilityOperator::SearchElementInfosByAccessibilityId(int channelId,
    int accessibilityWindowId, int elementId, int mode, std::vector<AccessibilityElementInfo> &elementInfos)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SearchElementInfosByAccessibilityId";
    return true;
}

bool AccessibilityOperator::SearchElementInfosByText(int channelId,
    int accessibilityWindowId, int elementId, const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SearchElementInfosByText";
    return true;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo &elementInfo)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator FindFocusedElementInfo";
    return true;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo &elementInfo)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator FocusMoveSearch";
    return true;
}

bool AccessibilityOperator::PerformAction(int channelId, int accessibilityWindowId,
    int elementId, int action,  std::map<std::string, std::string> &actionArguments)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator PerformAction";
    return performActionResult_;
}

void AccessibilityOperator::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetSearchElementInfoByAccessibilityIdResult";
}

void AccessibilityOperator::SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
    const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetSearchElementInfoByTextResult";
}

void AccessibilityOperator::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetFindFocusedElementInfoResult";
}

void AccessibilityOperator::SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetFocusMoveSearchResult";
}

void AccessibilityOperator::SetPerformActionResult(const bool succeeded, const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetPerformActionResult";
}

bool AccessibilityOperator::WaitForResultTimedLocked(const int requestId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator WaitForResultTimedLocked";
    return true;
}

int AccessibilityOperator::CreateRequestId()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator CreateRequestId";
    return requestId_;
}

bool AccessibilityOperator::PerformCommonAction(const int channelId, const int action)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator PerformCommonAction";
    return true;
}

void AccessibilityOperator::DisableAbility(const int channelId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator DisableAbility";
}

void AccessibilityOperator::SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetOnKeyPressEventResult";
}

float AccessibilityOperator::GetDisplayResizeScale(const int channelId, const int displayId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeScale";
    return 0;
}

float AccessibilityOperator::GetDisplayResizeCenterX(const int channelId, const int displayId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeCenterX";
    return 0;
}

float AccessibilityOperator::GetDisplayResizeCenterY(const int channelId, const int displayId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeCenterY";
    return 0;
}

Rect AccessibilityOperator::GetDisplayResizeRect(const int channelId, const int displayId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeRect";
    Rect rect {};
    rect.SetLeftTopScreenPostion(1, 1);
    return rect;
}

bool AccessibilityOperator::ResetDisplayResize(const int channelId, const int displayId, const bool animate)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator ResetDisplayResize";
    return true;
}

bool AccessibilityOperator::SetDisplayResizeScaleAndCenter(const int channelId,
    const int displayId, const float scale, const float centerX, 
    const float centerY, const bool animate)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetDisplayResizeScaleAndCenter";
    return true;
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int sequence, const std::vector<GesturePathDefine> &gestureSteps)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SendSimulateGesture";
}

bool AccessibilityOperator::IsFingerprintGestureDetectionValid(const int channelId)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator IsFingerprintGestureDetectionValid";
    return true;
}

}  // namespace Accessibility
}  // namespace OHOS