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
#include "accessibility_system_ability_client.h"

namespace OHOS {
namespace Accessibility {
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
    std::thread::id tid = std::this_thread::get_id();
    HILOG_DEBUG("[%{public}s] threadId[%{public}u]", __func__, (*(uint32_t*)&tid));
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (instances_.size() >= MAX_INSTANCE) {
        for (auto iter = instances_.begin(); iter != instances_.end(); iter++) {
            if (iter->GetRefPtr() != nullptr &&
                iter->GetRefPtr()->asyncElementOperatorMng_.GetOperationStatus()) {
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

void AccessibilityOperator::AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel)
{
    HILOG_DEBUG("[%{public}s] Add channel to aams [channelId:%{public}d]", __func__, channelId);
    int tempId = *(const_cast<int *>(&channelId));
    for (auto iter = channels_.begin(); iter != channels_.end(); iter++) {
        if (iter->first == tempId) {
            HILOG_ERROR("[%{public}s] channel to aams [channelId:%{public}d] is exited", __func__, channelId);
            return;
        }
    }
    sptr<IAccessibleAbilityChannel> tempChanel = const_cast<sptr<IAccessibleAbilityChannel> &>(channel);
    channels_.insert(std::pair<int, sptr<IAccessibleAbilityChannel>>(tempId, tempChanel));
}

void AccessibilityOperator::RemoveChannel(int channelId)
{
    HILOG_DEBUG("[%{public}s] Remove channel to aams [channelId:%{public}d]", __func__, channelId);
    auto iter = channels_.find(channelId);
    if (iter != channels_.end()) {
        channels_.erase(iter);
    } else {
        HILOG_DEBUG("[%{public}s] Failed to remove channel with aams [channelId:%{public}d]", __func__, channelId);
    }
}

bool AccessibilityOperator::GetRoot(int channelId, AccessibilityElementInfo &elementInfo)
{
    AccessibilityElementInfo element {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    int activeWindow = AccessibilitySystemAbilityClient::GetInstance()->GetActiveWindow();
    HILOG_DEBUG("[%{public}s] activeWindow is %{public}d", __func__, activeWindow);
    bool result = SearchElementInfosByAccessibilityId(channelId, activeWindow, NONE_ID, 0, elementInfos);
    for (auto& info : elementInfos) {
        HILOG_DEBUG("[%{public}s] element [elementSize:%{public}d]", __func__, elementInfosResult_.size());
        elementInfo = info;
        break;
    }
    return result;
}

std::vector<AccessibilityWindowInfo> AccessibilityOperator::GetWindows(int channelId)
{
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        auto windows = channelService->GetWindows();
        for (auto &window : windows) {
            window.SetChannelId(channelId);
        }
        return windows;
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return windows_;
    }
}

bool AccessibilityOperator::SearchElementInfosByAccessibilityId(int channelId,
    int accessibilityWindowId, int elementId, int mode, std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] [windowId:%{public}d]",
                __func__, channelId, accessibilityWindowId);
    bool result = false;
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        int sequenceNum = asyncElementOperatorMng_.RecordSearchSequence();
        HILOG_DEBUG("[%{public}s] search element info [channelId:%{public}d] [sequenceNum:%{public}d]", __func__,
            channelId, sequenceNum);
        result = channelService->SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, sequenceNum,
            this, mode);
        if (!result) {
            return result;
        }
        HILOG_DEBUG("[%{public}s] search element info End[channelId:%{public}d] [sequenceNum:%{public}d]", __func__,
            channelId, sequenceNum);
        if (!asyncElementOperatorMng_.SearchElementResultTimer(sequenceNum)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }
    HILOG_DEBUG("[%{public}s] search element info OK [channelId:%{public}d]", __func__, channelId);
    for (auto& info : elementInfosResult_) {
        info.SetChannelId(channelId);
    }
    asyncElementOperatorMng_.SetOperationStatus(true);
    HILOG_DEBUG("[%{public}s] search element info End[size:%{public}d]", __func__, elementInfosResult_.size());
    elementInfos = elementInfosResult_;
    return result;
}

bool AccessibilityOperator::SearchElementInfosByText(int channelId, int accessibilityWindowId,
    int elementId, const std::string &text, std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        int sequenceNum = asyncElementOperatorMng_.RecordSearchSequence();
        result = channelService->SearchElementInfosByText(accessibilityWindowId, elementId, text, sequenceNum,
            this);
        if (!result) {
            return result;
        }
        if (!asyncElementOperatorMng_.SearchElementResultTimer(sequenceNum)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }

    for (auto& info : elementInfosResult_) {
        info.SetChannelId(channelId);
    }
    asyncElementOperatorMng_.SetOperationStatus(true);
    HILOG_DEBUG("[%{public}s] [size:%{public}d] end", __func__, elementInfosResult_.size());
    elementInfos = elementInfosResult_;

    return result;
}

bool AccessibilityOperator::FindFocusedElementInfo(int channelId, int accessibilityWindowId,
    int elementId, int focusType, AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto channelService = GetChannel(channelId);

    if (channelService != nullptr) {
        int sequenceNum = asyncElementOperatorMng_.RecordSearchSequence();
        result = channelService->FindFocusedElementInfo(accessibilityWindowId, elementId,
            focusType, sequenceNum, this);
        if (!result) {
            return result;
        }

        HILOG_DEBUG(
            "FindFocusedElementInfo channelId[%{public}d] elementId[%{public}d],\
            focusType[%{public}d] sequenceNum[%{public}d]",
            channelId, accessibilityWindowId, elementId, focusType);
        if (!asyncElementOperatorMng_.SearchElementResultTimer(sequenceNum)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }
    accessibilityInfoResult_.SetChannelId(channelId);
    asyncElementOperatorMng_.SetOperationStatus(true);
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] end", __func__, channelId);
    if (accessibilityInfoResult_.GetWindowId() == 0 &&
        accessibilityInfoResult_.GetAccessibilityId() == 0) {
        HILOG_DEBUG("[%{public}s] Can't find the component info", __func__);
        result = false;
    } else {
        elementInfo = accessibilityInfoResult_;
        result = true;
    }

    return result;
}

bool AccessibilityOperator::FocusMoveSearch(int channelId, int accessibilityWindowId,
    int elementId, int direction, AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    bool result = false;
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        int sequenceNum = asyncElementOperatorMng_.RecordSearchSequence();
        result = channelService->FocusMoveSearch(accessibilityWindowId, elementId, direction, sequenceNum, this);
        if (!result) {
            return result;
        }
        if (!asyncElementOperatorMng_.SearchElementResultTimer(sequenceNum)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            result = false;
            return result;
        }
    }

    accessibilityInfoResult_.SetChannelId(channelId);
    asyncElementOperatorMng_.SetOperationStatus(true);
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

bool AccessibilityOperator::ExecuteAction(int channelId, int accessibilityWindowId,
    int elementId, int action,  std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        int sequenceNum = asyncElementOperatorMng_.RecordSearchSequence();
        bool result = channelService->ExecuteAction(
            accessibilityWindowId, elementId, action, actionArguments,
            sequenceNum, this);
        if (!result) {
            return result;
        }

        if (!asyncElementOperatorMng_.SearchElementResultTimer(sequenceNum)) {
            HILOG_ERROR("[%{public}s] Failed to wait result", __func__);
            return false;
        }
    }
    asyncElementOperatorMng_.SetOperationStatus(true);
    HILOG_DEBUG("[%{public}s] [channelId:%{public}d] end", __func__, channelId);
    return executeActionResult_;
}

void AccessibilityOperator::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int sequenceNum)
{
    HILOG_DEBUG("[%{public}s] Response[elementInfoSize:%{public}d] [sequenceNum:%{public}d]",
        __func__, infos.size(), sequenceNum);
    asyncElementOperatorMng_.UpdateSearchFeedback(sequenceNum);
    for (auto iter = infos.begin(); iter != infos.end(); iter++) {
        HILOG_DEBUG("[%{public}s] Response", __func__);
        elementInfosResult_.push_back(*iter);
    }
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] end", __func__, sequenceNum);
}

void AccessibilityOperator::SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
    const int sequenceNum)
{
    HILOG_DEBUG("[%{public}s] Response [elementInfoSize:%{public}d] [sequenceNum:%{public}d]",
        __func__, infos.size(), sequenceNum);
    asyncElementOperatorMng_.UpdateSearchFeedback(sequenceNum);
    for (auto iter = infos.begin(); iter != infos.end(); iter++) {
        elementInfosResult_.push_back(*iter);
    }
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] end", __func__, sequenceNum);
}

void AccessibilityOperator::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
    const int sequenceNum)
{
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d]", __func__, sequenceNum);
    asyncElementOperatorMng_.UpdateSearchFeedback(sequenceNum);
    accessibilityInfoResult_ = info;
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] end", __func__, sequenceNum);
}

void AccessibilityOperator::SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int sequenceNum)
{
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d]", __func__, sequenceNum);
    asyncElementOperatorMng_.UpdateSearchFeedback(sequenceNum);
    accessibilityInfoResult_ = info;
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] end", __func__, sequenceNum);
}

void AccessibilityOperator::SetExecuteActionResult(const bool succeeded, const int sequenceNum)
{
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] result[%{public}d]",
        __func__, sequenceNum, succeeded);
    executeActionResult_ = succeeded;
    asyncElementOperatorMng_.UpdateSearchFeedback(sequenceNum);
    HILOG_DEBUG("[%{public}s] Response [sequenceNum:%{public}d] end", __func__, sequenceNum);
}

bool AccessibilityOperator::ExecuteCommonAction(const int channelId, const int action)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->ExecuteCommonAction(action);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

void AccessibilityOperator::SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        channelService->SetOnKeyPressEventResult(handled, sequence);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
    }
}

float AccessibilityOperator::GetDisplayResizeScale(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->GetDisplayResizeScale(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

float AccessibilityOperator::GetDisplayResizeCenterX(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->GetDisplayResizeCenterX(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

float AccessibilityOperator::GetDisplayResizeCenterY(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->GetDisplayResizeCenterY(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return 1;
    }
}

Rect AccessibilityOperator::GetDisplayResizeRect(const int channelId, const int displayId)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->GetDisplayResizeRect(displayId);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        Rect rect {};
        return rect;
    }
}

bool AccessibilityOperator::ResetDisplayResize(const int channelId, const int displayId, const bool animate)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->ResetDisplayResize(displayId, animate);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

bool AccessibilityOperator::SetDisplayResizeScaleAndCenter(const int channelId,
    const int displayId, const float scale, const float centerX,
    const float centerY, const bool animate)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        return channelService->SetDisplayResizeScaleAndCenter(displayId, scale, centerX,
            centerY, animate);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
        return false;
    }
}

void AccessibilityOperator::SendSimulateGesture(const int channelId,
    const int sequenceNum, const std::vector<GesturePathDefine> &gestureSteps)
{
    HILOG_INFO("[%{public}s] [channelId:%{public}d]", __func__, channelId);
    auto channelService = GetChannel(channelId);
    if (channelService != nullptr) {
        channelService->SendSimulateGesture(sequenceNum, gestureSteps);
    } else {
        HILOG_ERROR("[%{public}s] Failed to connect to aams [channelId:%{public}d]", __func__, channelId);
    }
}
} // namespace Accessibility
} // namespace OHOS