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

#include "accessibility_element_operator_impl.h"
#include "accessibility_element_operator.h"
#include "accessibility_system_ability_client.h"
#include "hilog_wrapper.h"
#include <cinttypes>
namespace OHOS {
namespace Accessibility {
std::unordered_map<int32_t,
    sptr<IAccessibilityElementOperatorCallback>> AccessibilityElementOperatorImpl::requests_ = {};
ffrt::mutex AccessibilityElementOperatorImpl::requestsMutex_;

AccessibilityElementOperatorImpl::AccessibilityElementOperatorImpl(int32_t windowId,
    const std::shared_ptr<AccessibilityElementOperator> &operation,
    AccessibilityElementOperatorCallback &callback)
    : windowId_(windowId), operatorCallback_(callback), operator_(operation)
{
    HILOG_DEBUG();
}

AccessibilityElementOperatorImpl::~AccessibilityElementOperatorImpl()
{
    HILOG_DEBUG();
}

void AccessibilityElementOperatorImpl::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("search element add requestId[%{public}d], elementId[%{public}" PRId64 "], requestId[%{public}d]",
        mRequestId, elementId, requestId);
    callback->SetIsFilter(isFilter);
    if (operator_) {
        operator_->SearchElementInfoByAccessibilityId(elementId, mRequestId, operatorCallback_, mode);
    } else {
        HILOG_ERROR("Operator is nullptr");
    }
}

void AccessibilityElementOperatorImpl::SearchDefaultFocusedByWindowId(const int32_t windowId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    int32_t pageId = -1;
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("search element add requestId[%{public}d], requestId[%{public}d], windowId is [%{public}d]",
        mRequestId, requestId, windowId);
    callback->SetIsFilter(isFilter);
    if (operator_) {
        operator_->SearchDefaultFocusByWindowId(windowId, mRequestId, operatorCallback_, pageId);
    } else {
        HILOG_ERROR("Operator is nullptr");
    }
}

void AccessibilityElementOperatorImpl::SearchElementInfosByText(const int64_t elementId,
    const std::string &text, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("add requestId[%{public}d]", mRequestId);
    if (operator_) {
        operator_->SearchElementInfosByText(elementId, text, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("add requestId[%{public}d]", mRequestId);
    if (operator_) {
        operator_->FindFocusedElementInfo(elementId, focusType, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::FocusMoveSearch(const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("add requestId[%{public}d]", mRequestId);
    if (operator_) {
        operator_->FocusMoveSearch(elementId, direction, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::ExecuteAction(const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments,
    int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("add requestId[%{public}d]", mRequestId);
    if (operator_) {
        operator_->ExecuteAction(elementId, action, actionArguments, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::GetCursorPosition(const int64_t elementId,
    int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    int32_t mRequestId = AddRequest(requestId, callback);
    HILOG_DEBUG("add requestId[%{public}d]", mRequestId);
    if (operator_) {
        operator_->GetCursorPosition(elementId, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::ClearFocus()
{
    HILOG_DEBUG();
    if (operator_) {
        operator_->ClearFocus();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::OutsideTouch()
{
    HILOG_DEBUG();
    if (operator_ != nullptr) {
        operator_->OutsideTouch();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

int32_t AccessibilityElementOperatorImpl::GetWindowId()
{
    return windowId_;
}

void AccessibilityElementOperatorImpl::SetChildTreeIdAndWinId(const int64_t elementId,
    const int32_t childTreeId, const int32_t childWindowId)
{
    HILOG_DEBUG("elementId:%{public}" PRId64 ", childTreeId:%{public}d, childWind:%{public}d",
        elementId, childTreeId, childWindowId);
    operator_->SetChildTreeIdAndWinId(elementId, childTreeId, childWindowId);
}

void AccessibilityElementOperatorImpl::SetBelongTreeId(const int32_t treeId)
{
    HILOG_DEBUG("treeId:[%{public}d]", treeId);
    operator_->SetBelongTreeId(treeId);
}

void AccessibilityElementOperatorImpl::SetParentWindowId(const int32_t parentWindowId)
{
    HILOG_DEBUG("parentWindowId:[%{public}d]", parentWindowId);
    operator_->SetParentWindowId(parentWindowId);
}

int32_t AccessibilityElementOperatorImpl::AddRequest(int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);

    auto iter = requests_.find(requestId);
    if (iter == requests_.end()) {
        requests_[requestId] = callback;
    }
    return requestId;
}

void AccessibilityElementOperatorImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("requestId is %{public}d", requestId);
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            HILOG_DEBUG("isFilter %{public}d", iter->second->GetFilter());
            if (iter->second->GetFilter()) {
                SetFiltering(filterInfos);
            }
            iter->second->SetSearchElementInfoByAccessibilityIdResult(filterInfos, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("requestId is %{public}d, infos size is %{public}zu", requestId, infos.size());
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            HILOG_DEBUG("isFilter %{public}d", iter->second->GetFilter());
            if (iter->second->GetFilter()) {
                SetFiltering(filterInfos);
            }
            iter->second->SetSearchDefaultFocusByWindowIdResult(filterInfos, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::SetFiltering(std::vector<AccessibilityElementInfo> &filterInfos)
{
    for (auto &info : filterInfos) {
        info.SetAccessibilityText("");
        info.SetComponentResourceId("");
        info.SetPagePath("");
        info.SetError("");
        info.SetLatestContent("");
        info.SetContentList({});
    }
}

void AccessibilityElementOperatorImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    std::vector<AccessibilityElementInfo> myInfos(infos.begin(), infos.end());
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetSearchElementInfoByTextResult(myInfos, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetFindFocusedElementInfoResult(info, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetFocusMoveSearchResult(info, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetExecuteActionResult(succeeded, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}


void AccessibilityElementOperatorImpl::SetCursorPositionResult(const int32_t cursorPosition, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetCursorPositionResult(cursorPosition, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

sptr<IAccessibilityElementOperatorCallback> AccessibilityElementOperatorImpl::GetCallbackByRequestId(
    const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter == requests_.end()) {
        return nullptr;
    }
    return iter->second;
}

void AccessibilityElementOperatorImpl::EraseCallback(const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%{public}d]", requestId);
    }
}
} // namespace Accessibility
} // namespace OHOS