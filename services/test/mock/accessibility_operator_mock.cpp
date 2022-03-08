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

#include "accessibility_operator.h"

namespace OHOS {
namespace Accessibility {
const int LEFT_TOP_Y = 200;
const int RIGHT_BOTTOM_X = 600;
const int RIGHT_BOTTOM_Y = 800;
std::map<int, sptr<IAccessibleAbilityChannel>> AccessibilityOperator::channels_ = {};
std::vector<sptr<AccessibilityOperator>>  AccessibilityOperator::instances_ = {};
std::recursive_mutex AccessibilityOperator::mutex_ = {};

AccessibilityOperator::AccessibilityOperator()
{
    executeActionResult_ = false;
}

AccessibilityOperator::~AccessibilityOperator()
{
}

AccessibilityOperator &AccessibilityOperator::GetInstance()
{
    sptr<AccessibilityOperator> inst = new AccessibilityOperator();
    return *inst;
}

sptr<IAccessibleAbilityChannel> AccessibilityOperator::GetChannel(int channelId)
{
    auto channel = channels_.find(channelId);
    if (channel != channels_.end()) {
        HILOG_DEBUG("Find aams [channelId:%{public}d]", channelId);
        return channel->second;
    } else {
        HILOG_ERROR("Failed to find aams [channelId:%{public}d]", channelId);
        return nullptr;
    }
}

void AccessibilityOperator::AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel)
{
    HILOG_DEBUG("Add connection to aams [channelId:%{public}d]", channelId);
    int tempId = *(const_cast<int *>(&channelId));
    for (auto iter = channels_.begin(); iter != channels_.end(); iter++) {
        if (iter->first == tempId) {
            HILOG_ERROR("Connection to aams [channelId:%{public}d] is exited", channelId);
            return;
        }
    }
    sptr<IAccessibleAbilityChannel> tempChanel = const_cast<sptr<IAccessibleAbilityChannel> &>(channel);
    channels_.insert(std::pair<int, sptr<IAccessibleAbilityChannel>>(tempId, tempChanel));
}

void AccessibilityOperator::RemoveChannel(int channelId)
{
    HILOG_DEBUG("Remove connection to aams [channelId:%{public}d]", channelId);
    auto iter = channels_.find(channelId);
    if (iter != channels_.end()) {
        channels_.erase(iter);
    } else {
        HILOG_DEBUG("Failed to remove connection with aams [channelId:%{public}d]", channelId);
    }
}

bool AccessibilityOperator::GetRoot(int channelId, AccessibilityElementInfo &elementInfo)
{
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibilityOperator::GetWindows(int channelId)
{
    return windows_;
}

bool AccessibilityOperator::SearchElementInfosByAccessibilityId(int channelId,
    int accessibilityWindowId, int elementId, int mode, std::vector<AccessibilityElementInfo> &elementInfos)
{
    return true;
}

bool AccessibilityOperator::SearchElementInfosByText(int channelId, int accessibilityWindowId,
    int elementId, const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    return true;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo &elementInfo)
{
    AccessibilityElementInfo focus;
    Rect rect(0, LEFT_TOP_Y, RIGHT_BOTTOM_X, RIGHT_BOTTOM_Y);
    focus.SetRectInScreen(rect);
    elementInfo = focus;
    return true;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo &elementInfo)
{
    return true;
}

bool AccessibilityOperator::ExecuteAction(int channelId, int accessibilityWindowId,
    int elementId, int action, std::map<std::string, std::string> &actionArguments)
{
    return true;
}

void AccessibilityOperator::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
}

void AccessibilityOperator::SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
    const int requestId)
{
}

void AccessibilityOperator::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId)
{
}

void AccessibilityOperator::SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId)
{
}

void AccessibilityOperator::SetExecuteActionResult(const bool succeeded, const int requestId)
{
}

bool AccessibilityOperator::ExecuteCommonAction(const int channelId, const int action)
{
    return true;
}

void AccessibilityOperator::SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence)
{
}

float AccessibilityOperator::GetDisplayResizeScale(const int channelId, const int displayId)
{
    return 1;
}

float AccessibilityOperator::GetDisplayResizeCenterX(const int channelId, const int displayId)
{
    return 1;
}

float AccessibilityOperator::GetDisplayResizeCenterY(const int channelId, const int displayId)
{
    return 1;
}

Rect AccessibilityOperator::GetDisplayResizeRect(const int channelId, const int displayId)
{
    Rect rect {};
    return rect;
}

bool AccessibilityOperator::ResetDisplayResize(const int channelId, const int displayId, const bool animate)
{
    return false;
}

bool AccessibilityOperator::SetDisplayResizeScaleAndCenter(const int channelId,
    const int displayId, const float scale, const float centerX, const float centerY, const bool animate)
{
    return false;
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int requestId, const std::vector<GesturePathDefine> &gestureSteps)
{
}
} // namespace Accessibility
} // namespace OHOS