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

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorImpl::AccessibilityElementOperatorImpl(int32_t windowId,
    const std::shared_ptr<AccessibilityElementOperator> &operation)
    : windowId_(windowId), operator_(operation)
{
    HILOG_DEBUG();
}

AccessibilityElementOperatorImpl::~AccessibilityElementOperatorImpl()
{
    HILOG_DEBUG();
}

void AccessibilityElementOperatorImpl::SearchElementInfoByAccessibilityId(const int32_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode)
{
    HILOG_DEBUG();
    operatorCallback_.AddRequest(requestId, callback);
    if (operator_) {
        operator_->SearchElementInfoByAccessibilityId(elementId, requestId, operatorCallback_, mode);
    } else {
        HILOG_ERROR("Operator is nullptr");
    }
}

void AccessibilityElementOperatorImpl::SearchElementInfosByText(const int32_t elementId,
    const std::string &text, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    operatorCallback_.AddRequest(requestId, callback);
    if (operator_) {
        operator_->SearchElementInfosByText(elementId, text, requestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::FindFocusedElementInfo(const int32_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    operatorCallback_.AddRequest(requestId, callback);
    if (operator_) {
        operator_->FindFocusedElementInfo(elementId, focusType, requestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::FocusMoveSearch(const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    operatorCallback_.AddRequest(requestId, callback);
    if (operator_) {
        operator_->FocusMoveSearch(elementId, direction, requestId, operatorCallback_);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorImpl::ExecuteAction(const int32_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments,
    int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    operatorCallback_.AddRequest(requestId, callback);
    if (operator_) {
        operator_->ExecuteAction(elementId, action, actionArguments, requestId, operatorCallback_);
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

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::AddRequest(
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = requests_.find(requestId);
    if (iter == requests_.end()) {
        requests_[requestId] = callback;
    }
}

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
    auto iter = requests_.find(requestId);
    if (iter != requests_.end()) {
        if (iter->second != nullptr) {
            iter->second->SetSearchElementInfoByAccessibilityIdResult(myInfos, requestId);
        }
        requests_.erase(iter);
    } else {
        HILOG_DEBUG("Can't find the callback [requestId:%d]", requestId);
    }
}

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
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

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
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

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
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

void AccessibilityElementOperatorImpl::OperatorCallbackImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
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
} // namespace Accessibility
} // namespace OHOS