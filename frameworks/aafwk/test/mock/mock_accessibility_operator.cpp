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

#include <gtest/gtest.h>
#include "accessibility_operator.h"
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {
std::map<int, sptr<IAccessibleAbilityChannel>> AccessibilityOperator::channels_ = {};
std::vector<sptr<AccessibilityOperator>>  AccessibilityOperator::instances_ = {};
std::recursive_mutex AccessibilityOperator::mutex_ = {};

AccessibilityOperator::AccessibilityOperator()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator";
    executeActionResult_ = true;
}

AccessibilityOperator::~AccessibilityOperator()
{
    GTEST_LOG_(INFO) << "MOCK ~AccessibilityOperator";
}

AccessibilityOperator &AccessibilityOperator::GetInstance()
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetInstance";
    static sptr<AccessibilityOperator> accessibilityOperator = nullptr;
    if (accessibilityOperator == nullptr) {
        accessibilityOperator = new AccessibilityOperator();
        return *accessibilityOperator;
    }

    return *accessibilityOperator;
}


sptr<IAccessibleAbilityChannel> AccessibilityOperator::GetChannel(int channelId)
{
    (void)channelId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetChannel";
    return nullptr;
}

void AccessibilityOperator::AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel)
{
    (void)channelId;
    (void)channel;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator AddChannel";
}

void AccessibilityOperator::RemoveChannel(int channelId)
{
    (void)channelId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator RemoveChannel";
}

bool AccessibilityOperator::GetRoot(int channelId, AccessibilityElementInfo &elementInfo)
{
    (void)channelId;
    (void)elementInfo;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetRoot";
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibilityOperator::GetWindows(int channelId)
{
    (void)channelId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetWindows";
    return windows_;
}

bool AccessibilityOperator::SearchElementInfosByAccessibilityId(int channelId,
    int accessibilityWindowId, int elementId, int mode, std::vector<AccessibilityElementInfo> &elementInfos)
{
    (void)channelId;
    (void)accessibilityWindowId;
    (void)elementId;
    (void)mode;
    (void)elementInfos;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SearchElementInfosByAccessibilityId";
    return true;
}

bool AccessibilityOperator::SearchElementInfosByText(int channelId,
    int accessibilityWindowId, int elementId, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    (void)channelId;
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)elementInfos;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SearchElementInfosByText";
    return true;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo &elementInfo)
{
    (void)channelId;
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)elementInfo;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator FindFocusedElementInfo";
    return true;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo &elementInfo)
{
    (void)channelId;
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)elementInfo;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator FocusMoveSearch";
    return true;
}

bool AccessibilityOperator::ExecuteAction(int channelId, int accessibilityWindowId,
    int elementId, int action,  std::map<std::string, std::string> &actionArguments)
{
    (void)channelId;
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator ExecuteAction";
    return executeActionResult_;
}

void AccessibilityOperator::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    (void)infos;
    (void)requestId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetSearchElementInfoByAccessibilityIdResult";
}

void AccessibilityOperator::SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
    const int requestId)
{
    (void)infos;
    (void)requestId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetSearchElementInfoByTextResult";
}

void AccessibilityOperator::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId)
{
    (void)info;
    (void)requestId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetFindFocusedElementInfoResult";
}

void AccessibilityOperator::SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId)
{
    (void)info;
    (void)requestId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetFocusMoveSearchResult";
}

void AccessibilityOperator::SetExecuteActionResult(const bool succeeded, const int requestId)
{
    (void)succeeded;
    (void)requestId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetExecuteActionResult";
}

bool AccessibilityOperator::ExecuteCommonAction(const int channelId, const int action)
{
    (void)channelId;
    (void)action;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator ExecuteCommonAction";
    return true;
}

void AccessibilityOperator::SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence)
{
    (void)channelId;
    (void)handled;
    (void)sequence;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetOnKeyPressEventResult";
}

float AccessibilityOperator::GetDisplayResizeScale(const int channelId, const int displayId)
{
    (void)channelId;
    (void)displayId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeScale";
    return 0;
}

float AccessibilityOperator::GetDisplayResizeCenterX(const int channelId, const int displayId)
{
    (void)channelId;
    (void)displayId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeCenterX";
    return 0;
}

float AccessibilityOperator::GetDisplayResizeCenterY(const int channelId, const int displayId)
{
    (void)channelId;
    (void)displayId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeCenterY";
    return 0;
}

Rect AccessibilityOperator::GetDisplayResizeRect(const int channelId, const int displayId)
{
    (void)channelId;
    (void)displayId;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator GetDisplayResizeRect";
    Rect rect {};
    rect.SetLeftTopScreenPostion(1, 1);
    return rect;
}

bool AccessibilityOperator::ResetDisplayResize(const int channelId, const int displayId, const bool animate)
{
    (void)channelId;
    (void)displayId;
    (void)animate;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator ResetDisplayResize";
    return true;
}

bool AccessibilityOperator::SetDisplayResizeScaleAndCenter(const int channelId,
    const int displayId, const float scale, const float centerX, const float centerY, const bool animate)
{
    (void)channelId;
    (void)displayId;
    (void)scale;
    (void)centerX;
    (void)centerY;
    (void)animate;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SetDisplayResizeScaleAndCenter";
    return true;
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int requestId, const std::vector<GesturePathDefine> &gestureSteps)
{
    (void)channelId;
    (void)requestId;
    (void)gestureSteps;
    GTEST_LOG_(INFO) << "MOCK AccessibilityOperator SendSimulateGesture";
}
} // namespace Accessibility
} // namespace OHOS