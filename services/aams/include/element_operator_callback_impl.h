/*
 * Copyright (C) 2022-2026 Huawei Device Co., Ltd.
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

#ifndef ELEMENT_OPERATOR_CALLBACK_IMPL_H
#define ELEMENT_OPERATOR_CALLBACK_IMPL_H

#include <list>
#include <vector>
#include <future>
#include <chrono>

#include "accessibility_element_info.h"
#include "accessibility_element_operator_callback_stub.h"
#include "ffrt.h"
#include "ffrt_inner.h"

namespace OHOS {
namespace Accessibility {

class ElementOperatorCallbackImpl : public AccessibilityElementOperatorCallbackStub {
public:
    ElementOperatorCallbackImpl() = default;
    virtual ~ElementOperatorCallbackImpl() = default;

    virtual void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override;
    virtual void SetSearchDefaultFocusByWindowIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override;
    virtual void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override;
    virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
        const int32_t requestId) override;
    virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) override;
    virtual void SetExecuteActionResult(const bool succeeded, const int32_t requestId) override;
    virtual void SetCursorPositionResult(const int32_t cursorPosition, const int32_t requestId) override;
;
    virtual void SetSearchElementInfoBySpecificPropertyResult(const std::list<AccessibilityElementInfo> &infos,
        const std::list<AccessibilityElementInfo> &treeInfos, const int32_t requestId) override;
    virtual void SetFocusMoveSearchWithConditionResult(const std::list<AccessibilityElementInfo> &infos,
        const FocusMoveResult &result, const int32_t requestId) override;

    ffrt::promise<void> promise_;
    bool executeActionResult_ = false;
    AccessibilityElementInfo accessibilityInfoResult_ = {};
    std::vector<AccessibilityElementInfo> elementInfosResult_;
    int32_t callCursorPosition_ = 0;

    FocusMoveResultType focusMoveResult_ = FocusMoveResultType::NOT_SUPPORT;
    int32_t nowLevelBelongTreeId_ = 0;
    int32_t parentWindowId_ = 0;
    bool changeToNewInfo_ = false;
    bool needTerminate_ = false;

    bool ValidateElementInfos(const std::list<AccessibilityElementInfo>& infos);
};

} // namespace Accessibility
} // namespace OHOS

#endif // ELEMENT_OPERATOR_CALLBACK_IMPL_H