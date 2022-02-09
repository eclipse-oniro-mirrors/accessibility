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
#include <sys/time.h>
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
}

AccessibilityOperator &AccessibilityOperator::GetInstance()
{
    std::thread::id tid = std::this_thread::get_id();
    return GetInstanceForThread(tid);
}

AccessibilityOperator &AccessibilityOperator::GetInstanceForThread(std::thread::id threadId)
{
    HILOG_DEBUG("[%{public}s] threadId[%{public}u]", __func__, (*(uint32_t*)&threadId));
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
    AccessibilityElementInfo element {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = SearchElementInfosByAccessibilityId(channelId, ACTIVE_WINDOW_ID, NONE_ID, 0, elementInfos);
    for (auto& info : elementInfos) {
        HILOG_DEBUG("[%{public}s] element [elementSize:%{public}d]", __func__, accessibilityInfosResult_.size());
        elementInfo = info;
        break;
    }
    return result;
}

std::vector<AccessibilityWindowInfo> AccessibilityOperator::GetWindows(int channelId)
{
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        auto windows = connectService->GetWindows();
        for (auto &window : windows) {
            window.SetChannelId(channelId);
        }
        return windows;
    } else {
        HILOG_ERROR("[%{public}s] Failed to connection to aams [channelId:%{public}d]", __func__, channelId);
        return windows_;
    }
}

bool AccessibilityOperator::SearchElementInfosByAccessibilityId(int channelId,
    int accessibilityWindowId, int elementId, int mode, std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        int requestId = CreateRequestId();
        HILOG_DEBUG("[%{public}s] search element info [channelId:%{public}d] [requestId:%{public}d]", __func__,
            channelId, requestId);
        result = connectService->SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId,
            this, mode);
        if (!result) {
            return result;
        }
        HILOG_DEBUG("[%{public}s] search element info End[channelId:%{public}d] [requestId:%{public}d]", __func__,
            channelId, requestId);
        if (!WaitForResultTimedLocked(requestId)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }
    HILOG_DEBUG("[%{public}s] search element info OK [channelId:%{public}d]", __func__, channelId);
    for (auto& info : accessibilityInfosResult_) {
        info.SetChannelId(channelId);
    }
    completed_ = true;
    HILOG_DEBUG("[%{public}s] search element info End[size:%{public}d]", __func__, accessibilityInfosResult_.size());
    elementInfos = accessibilityInfosResult_;
    return result;
}

bool AccessibilityOperator::SearchElementInfosByText(int channelId, int accessibilityWindowId,
    int elementId, const std::string &text,std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        int requestId = CreateRequestId();
        result = connectService->SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId,
        this);
        if (!result) {
            return result;
        }
        if (!WaitForResultTimedLocked(requestId)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }

    for (auto& info : accessibilityInfosResult_) {
        info.SetChannelId(channelId);
    }
    completed_ = true;
    HILOG_DEBUG("[%{public}s] [size:%{public}d] end", __func__, accessibilityInfosResult_.size());
    elementInfos = accessibilityInfosResult_;

    return result;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        int requestId = CreateRequestId();
        result = connectService->FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId,
        this);
        if (!result) {
            return result;
        }
        HILOG_DEBUG(
            "FindFocusedElementInfo channelId[%{public}d] elementId[%{public}d],\
            focusType[%{public}d] requestId[%{public}d]",
            channelId, accessibilityWindowId, elementId, focusType);
        if (!WaitForResultTimedLocked(requestId)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }
    accessibilityInfoResult_.SetChannelId(channelId);
    completed_ = true;
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] end", __func__, channelId);
    elementInfo = accessibilityInfoResult_;
    if (accessibilityInfoResult_.GetWindowId() == 0 &&
        accessibilityInfoResult_.GetAccessibilityId() == 0) {
        HILOG_DEBUG("[%{public}s] Can't find the component info", __func__);
        result = false;
    } else {
        result = true;
    }

    return result;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        int requestId = CreateRequestId();
        result = connectService->FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, this);
        if (!result) {
            return result;
        }
        if (!WaitForResultTimedLocked(requestId)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }

    accessibilityInfoResult_.SetChannelId(channelId);
    completed_ = true;
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] end", __func__, channelId);
    elementInfo = accessibilityInfoResult_;
    if (accessibilityInfoResult_.GetWindowId() == 0 &&
        accessibilityInfoResult_.GetAccessibilityId() == 0) {
        HILOG_DEBUG("[%{public}s] Can't find the component info", __func__);
        result = false;
    } else {
        result = true;
    }
    return result;
}

bool AccessibilityOperator::PerformAction(int channelId, int accessibilityWindowId,
    int elementId, int action,  std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        int requestId = CreateRequestId();
        bool result = connectService->PerformAction(accessibilityWindowId, elementId, action, actionArguments,
            requestId,
            this);
        if (!result) {
            return result;
        }

        if (!WaitForResultTimedLocked(requestId)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            return false;
        }
    }
    completed_ = true;
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] end", __func__, channelId);
    return performActionResult_;
}

void AccessibilityOperator::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d]", __func__, requestId);
    responseId_ = requestId;
    for (auto iter = infos.begin(); iter != infos.end(); iter++) {
        HILOG_DEBUG("[%{public}s] Response", __func__);
        accessibilityInfosResult_.push_back(*iter);
    }
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
}

void AccessibilityOperator::SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
    const int requestId)
{
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d]", __func__, requestId);
    responseId_ = requestId;
    for (auto iter = infos.begin(); iter != infos.end(); iter++) {
        accessibilityInfosResult_.push_back(*iter);
    }
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
}

void AccessibilityOperator::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d]", __func__, requestId);
    responseId_ = requestId;
    accessibilityInfoResult_ = info;
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
}

void AccessibilityOperator::SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d]", __func__, requestId);
    responseId_ = requestId;
    accessibilityInfoResult_ = info;
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
}

void AccessibilityOperator::SetPerformActionResult(const bool succeeded, const int requestId)
{
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] result[%{public}d]", __func__, requestId, succeeded);
    performActionResult_ = succeeded;
    responseId_ = requestId;
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
}

bool AccessibilityOperator::WaitForResultTimedLocked(const int requestId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    struct timeval getTime {};
    gettimeofday(&getTime, NULL);
    uint64_t startTime = getTime.tv_sec * SECOND_TO_MILLIS + getTime.tv_usec;
    HILOG_DEBUG("[%{public}s] element requestId[%{public}d]", __func__, requestId);

    do {
        if (responseId_ == requestId) {
            return true;
        }
        gettimeofday(&getTime, NULL);
        uint64_t endTime =  getTime.tv_sec * SECOND_TO_MILLIS + getTime.tv_usec;
        uint64_t waitTime = endTime - startTime;
        if (TIMEOUT_OPERATOR_MILLIS < waitTime) {
            completed_ = true;
            HILOG_ERROR("[%{public}s] Failed to wait requestId[%{public}d], responseId_[%{public}d]", __func__,
                requestId, responseId_);
            return false;
        }
        usleep(SLEEP_MILLIS);
    } while (true);
    HILOG_DEBUG("[%{public}s] Response [requestId:%{public}d] end", __func__, requestId);
    return true;
}

int AccessibilityOperator::CreateRequestId()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    HILOG_INFO("[%{public}s] [requestId_:%{public}d]", __func__, requestId_);
    requestId_++;
    requestId_ = requestId_ % MAX_REQUEST;
    HILOG_INFO("[%{public}s] [requestId_:%{public}d] end", __func__, requestId_);
    return requestId_;
}

bool AccessibilityOperator::PerformCommonAction(const int channelId, const int action)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->PerformCommonAction(action);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

void AccessibilityOperator::DisableAbility(const int channelId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        connectService->DisableAbility();
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
    }
}

void AccessibilityOperator::SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        connectService->SetOnKeyPressEventResult(handled, sequence);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
    }
}

float AccessibilityOperator::GetDisplayResizeScale(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->GetDisplayResizeScale(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

float AccessibilityOperator::GetDisplayResizeCenterX(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->GetDisplayResizeCenterX(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

float AccessibilityOperator::GetDisplayResizeCenterY(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->GetDisplayResizeCenterY(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

Rect AccessibilityOperator::GetDisplayResizeRect(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->GetDisplayResizeRect(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        Rect rect {};
        return rect;
    }
}

bool AccessibilityOperator::ResetDisplayResize(const int channelId, const int displayId, const bool animate)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->ResetDisplayResize(displayId, animate);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

bool AccessibilityOperator::SetDisplayResizeScaleAndCenter(const int channelId,
    const int displayId, const float scale, const float centerX,
    const float centerY, const bool animate)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->SetDisplayResizeScaleAndCenter(displayId, scale, centerX,
            centerY, animate);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int sequence, const std::vector<GesturePathDefine> &gestureSteps)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        connectService->SendSimulateGesture(sequence, gestureSteps);
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
    }
}

bool AccessibilityOperator::IsFingerprintGestureDetectionValid(const int channelId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto connectService = GetChannel(channelId);
    if (connectService != nullptr) {
        return connectService->IsFingerprintGestureDetectionValid();
    } else {
        HILOG_ERROR("[%{public}s] Failed to Get connection to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

} //namespace Accessibility
} //namespace OHOS