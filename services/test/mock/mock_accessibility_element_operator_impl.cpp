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

#include "mock_accessibility_element_operator_impl.h"
#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_system_ability_client.h"
#include "hilog_wrapper.h"
#include "iremote_object.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t REQUEST_WINDOW_ID_MAX = 0x00007FFF;
    constexpr uint32_t REQUEST_ID_MASK = 0x0000FFFF;
    constexpr int32_t REQUEST_ID_MASK_BIT = 16;
} // namespaces

using AccessibilityElementOperatorCallbacks =
    std::map<const int32_t, const sptr<IAccessibilityElementOperatorCallback>>;
MockAccessibilityElementOperatorImpl::MockAccessibilityElementOperatorImpl(int32_t windowId,
    const std::shared_ptr<AccessibilityElementOperator> &operation,
    AccessibilityElementOperatorCallback &callback)
    : windowId_(windowId), operatorCallback_(callback), operator_(operation)
{}

MockAccessibilityElementOperatorImpl::~MockAccessibilityElementOperatorImpl()
{}

void MockAccessibilityElementOperatorImpl::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode,
    bool isFilter)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->SearchElementInfoByAccessibilityId(elementId, mRequestId, operatorCallback_, mode);
    } else {
        HILOG_ERROR("Operator is nullptr");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SearchElementInfosByText(const int64_t elementId, const std::string& text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->SearchElementInfosByText(elementId, text, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::FindFocusedElementInfo(const int64_t elementId, const int32_t focusType,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->FindFocusedElementInfo(elementId, focusType, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::FocusMoveSearch(const int64_t elementId, const int32_t direction,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->FocusMoveSearch(elementId, direction, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::ExecuteAction(const int64_t elementId, const int32_t action,
    const std::map<std::string, std::string> &actionArguments, int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->ExecuteAction(elementId, action, actionArguments, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::GetCursorPosition(const int64_t elementId, int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    int32_t mRequestId = AddRequest(requestId, callback);
    if (operator_) {
        operator_->GetCursorPosition(elementId, mRequestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::ClearFocus()
{
    if (operator_) {
        operator_->ClearFocus();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::OutsideTouch()
{
    if (operator_ != nullptr) {
        operator_->OutsideTouch();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}
int32_t MockAccessibilityElementOperatorImpl::GetWindowId()
{
    return windowId_;
}

int32_t MockAccessibilityElementOperatorImpl::AddRequest(int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    uint32_t compositionRequestId = static_cast<uint32_t>(requestId) & REQUEST_ID_MASK;

    if (windowId_ < REQUEST_WINDOW_ID_MAX && windowId_ > 0) {
        compositionRequestId &= static_cast<uint32_t>(windowId_) << REQUEST_ID_MASK_BIT;
    } else {
        HILOG_ERROR("window id[%{public}d] is wrong", windowId_);
        return -1;
    }

    requestId = static_cast<int32_t>(compositionRequestId);
    if (requests_.find(requestId) == requests_.end()) {
        requests_[requestId] = callback;
    }
    return requestId;
}

void MockAccessibilityElementOperatorImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo>& infos, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    std::vector<AccessibilityElementInfo> myInfos(infos.begin(), infos.end());
    auto iterator = requests_.find(requestId);
    if (iterator != requests_.end()) {
        if (iterator->second != nullptr) {
            iterator->second->SetSearchElementInfoByAccessibilityIdResult(myInfos, requestId);
        }
        requests_.erase(iterator);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo>& infos, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    std::vector<AccessibilityElementInfo> myInfos(infos.begin(), infos.end());
    auto iterator = requests_.find(requestId);
    if (iterator != requests_.end()) {
        if (iterator->second != nullptr) {
            iterator->second->SetSearchElementInfoByTextResult(myInfos, requestId);
        }
        requests_.erase(iterator);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo& info, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iterator = requests_.find(requestId);
    if (iterator != requests_.end()) {
        if (iterator->second != nullptr) {
            iterator->second->SetFindFocusedElementInfoResult(info, requestId);
        }
        requests_.erase(iterator);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo& info, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iterator = requests_.find(requestId);
    if (iterator != requests_.end()) {
        if (iterator->second != nullptr) {
            iterator->second->SetFocusMoveSearchResult(info, requestId);
        }
        requests_.erase(iterator);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestsMutex_);
    auto iterator = requests_.find(requestId);
    if (iterator != requests_.end()) {
        if (iterator->second != nullptr) {
            iterator->second->SetExecuteActionResult(succeeded, requestId);
        }
        requests_.erase(iterator);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetChildTreeIdAndWinId(const int64_t elementId,
    const int32_t treeId, const int32_t childWindowId)
{
    if (operator_ != nullptr) {
        operator_->SetChildTreeIdAndWinId(elementId, treeId, childWindowId);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetBelongTreeId(const int32_t treeId)
{
    if (operator_ != nullptr) {
        operator_->SetBelongTreeId(treeId);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}

void MockAccessibilityElementOperatorImpl::SetParentWindowId(const int32_t parentWindowId)
{
    if (operator_ != nullptr) {
        operator_->SetParentWindowId(parentWindowId);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
    return;
}
} // namespace Accessibility
} // namespace OHOS