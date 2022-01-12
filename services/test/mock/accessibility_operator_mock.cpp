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

#include <unistd.h>

namespace OHOS {
namespace Accessibility {
std::map<int, sptr<IAccessibleAbilityChannel>> AccessibilityOperator::channels_ = {};
std::vector<sptr<AccessibilityOperator>>  AccessibilityOperator::instances_ = {};
std::recursive_mutex AccessibilityOperator::mutex_ = {};
int AccessibilityOperator::requestId_ = 0;

AccessibilityOperator::AccessibilityOperator()
{
    performActionResult_ = false;
    responseId_ = 1;
}

AccessibilityOperator &AccessibilityOperator::GetInstance()
{
    sptr<AccessibilityOperator> inst = new AccessibilityOperator();
    return *inst;
}

AccessibilityOperator &AccessibilityOperator::GetInstanceForThread(std::thread::id threadId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (instances_.size() >= MAX_INSTANCE) {
        for (auto iter = instances_.begin(); iter != instances_.end();iter++) {
            if (iter->GetRefPtr() != nullptr &&
                iter->GetRefPtr()->GetOperationStatus()) {
                HILOG_DEBUG("[%{public}s] complete instance is removed", __func__);
                instances_.erase(iter);
                break;
            }
        }
    }
    HILOG_DEBUG("[%{public}s] new instance instanceSize[%{public}u]", __func__, instances_.size());
    sptr<AccessibilityOperator> inst(new AccessibilityOperator());
    instances_.push_back(inst);

    HILOG_DEBUG("[%{public}s] End instanceSize[%{public}u]", __func__, instances_.size());
    return *(inst.GetRefPtr());
}

sptr<IAccessibleAbilityChannel> AccessibilityOperator::GetChannel(int channelId)
{
    auto channel = channels_.find(channelId);
    if (channel != channels_.end()) {
        HILOG_DEBUG("[%{public}s] Find aams [channelId:%{public}d]", __func__, channelId);
        return channel->second;
    } else {
        HILOG_ERROR("[%{public}s] Failed to find aams [channelId:%{public}d]", __func__, channelId);
        return nullptr;
    }
}

bool AccessibilityOperator::GetOperationStatus()
{
    HILOG_DEBUG("[%{public}s] [completed_:%{public}d]", __func__, completed_);
    return completed_;
}

void AccessibilityOperator::SetOperationStatus(bool status)
{
    completed_ = status;
}

void AccessibilityOperator::AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel)
{
    HILOG_DEBUG("[%{public}s] Add connection to aams [channelId:%{public}d]", __func__, channelId);
    int tempId = *(const_cast<int *>(&channelId));
    for (auto iter = channels_.begin(); iter != channels_.end(); iter++) {
        if (iter->first == tempId) {
            HILOG_ERROR("[%{public}s] Connection to aams [channelId:%{public}d] is exited", __func__, channelId);
            return;
        }
    }
    sptr<IAccessibleAbilityChannel> tempChanel = const_cast<sptr<IAccessibleAbilityChannel> &>(channel);
    channels_.insert(std::pair<int, sptr<IAccessibleAbilityChannel>>(tempId, tempChanel));
}

void AccessibilityOperator::RemoveChannel(int channelId)
{
    HILOG_DEBUG("[%{public}s] Remove connection to aams [channelId:%{public}d]", __func__, channelId);
    auto iter = channels_.find(channelId);
    if (iter != channels_.end()) {
        channels_.erase(iter);
    } else {
        HILOG_DEBUG("[%{public}s] Failed to remove connection with aams [channelId:%{public}d]", __func__, channelId);
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

bool AccessibilityOperator::SearchElementInfosByText(int channelId,
    int accessibilityWindowId, int elementId, const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    return true;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo &elementInfo)
{
    AccessibilityElementInfo focus;
    Rect rect(0,200,600,800);
    focus.SetRectInScreen(rect);
    elementInfo = focus;
    return true;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo &elementInfo)
{
    return true;
}

bool AccessibilityOperator::PerformAction(int channelId, int accessibilityWindowId,
    int elementId, int action,  std::map<std::string, std::string> &actionArguments)
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

void AccessibilityOperator::SetPerformActionResult(const bool succeeded, const int requestId)
{
}

bool AccessibilityOperator::WaitForResultTimedLocked(const int requestId)
{
    return true;
}

int AccessibilityOperator::CreateRequestId()
{
    return requestId_;
}

bool AccessibilityOperator::PerformCommonAction(const int channelId, const int action)
{
    return true;
}

void AccessibilityOperator::DisableAbility(const int channelId)
{
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
    const int displayId, const float scale, const float centerX, 
    const float centerY, const bool animate)
{
    return false;
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int sequence, const std::vector<GesturePathDefine> &gestureSteps)
{
}

bool AccessibilityOperator::IsFingerprintGestureDetectionValid(const int channelId)
{
    return false;
}

} //namespace Accessibility
} //namespace OHOS