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
    promise_.set_value();
}

void AccessibilityElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [elementInfoSize:%{public}zu] [requestId:%{public}d]", infos.size(), requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void AccessibilityElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response[elementInfoSize:%{public}zu] [requestId:%{public}d]", infos.size(), requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void AccessibilityElementOperatorCallbackImpl::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void AccessibilityElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d] result[%{public}d]", requestId, succeeded);
    executeActionResult_ = succeeded;
    promise_.set_value();
}
} // namespace Accessibility
} // namespace OHOS