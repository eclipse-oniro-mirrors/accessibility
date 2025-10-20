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

#include "accessibility_element_operator_callback_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
void AccessibilityElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [elementInfoSize:%{public}zu] [requestId:%{public}d]", infos.size(), requestId);
    elementInfosResult_ = infos;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response[elementInfoSize:%{public}zu] [requestId:%{public}d]", infos.size(), requestId);
    elementInfosResult_ = infos;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response[elementInfoSize:%{public}zu] [requestId:%{public}d]", infos.size(), requestId);
    elementInfosResult_ = infos;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d] result[%{public}d]", requestId, succeeded);
    executeActionResult_ = succeeded;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetCursorPositionResult(const int32_t cursorPosition,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d] cursorPosition[%{public}d]", requestId, cursorPosition);
    CursorPosition_ = cursorPosition;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetSearchElementInfoBySpecificPropertyResult(
    const std::list<AccessibilityElementInfo> &infos, const std::list<AccessibilityElementInfo> &treeInfos,
    const int32_t requestId)
{
    HILOG_DEBUG("Response[elementInfoSize:%{public}zu] [treeInfoSize:%{public}zu] [requestId:%{public}d]",
        infos.size(), treeInfos.size(), requestId);

    elementInfosResult_.assign(infos.begin(), infos.end());
    treeInfosResult_.assign(treeInfos.begin(), treeInfos.end());
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetFocusMoveSearchWithConditionResult(
    const std::list<AccessibilityElementInfo> &infos, const FocusMoveResult& result, const int32_t requestId)
{
    elementInfosResult_.assign(infos.begin(), infos.end());
    result_ = result;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
    }
}

void AccessibilityElementOperatorCallbackImpl::SetDetectElementInfoFocusableThroughAncestorResult(
    const bool isFocusable, const int32_t requestId, const AccessibilityElementInfo &info)
{
    HILOG_DEBUG("Response [requestId:%{public}d] result[%{public}d]", requestId, isFocusable);
    isFocusable_ = isFocusable;
    if (promiseFlag_ == false) {
        promise_.set_value();
        promiseFlag_ = true;
        accessibilityInfoResult_ = info;
    }
}
} // namespace Accessibility
} // namespace OHOS