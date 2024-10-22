/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef ACCESSIBILITY_ACCESSIBILITY_ELEMENT_OPERATOR_FUZZ_IMPL_H
#define ACCESSIBILITY_ACCESSIBILITY_ELEMENT_OPERATOR_FUZZ_IMPL_H

#include "accessibility_element_operator.h"

class AccessibilityElementOperatorFuzzImpl : public OHOS::Accessibility::AccessibilityElementOperator {
public:
    virtual ~AccessibilityElementOperatorFuzzImpl() = default;
    void SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback, const int32_t mode) override
    {
    }
    void SearchElementInfosByText(const int64_t elementId, const std::string &text, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback) override
    {
    }

    void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback) override
    {
    }

    void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback) override
    {
    }

    void ExecuteAction(const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback) override
    {
    }
    void GetCursorPosition(const int64_t elementId, const int32_t requestId,
        OHOS::Accessibility::AccessibilityElementOperatorCallback &callback) override
    {
    }
    void ClearFocus() override
    {
    }
    void OutsideTouch() override
    {
    }
    void SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId, const int32_t childWindowId) override
    {
    }
    void SetBelongTreeId(const int32_t treeId) override
    {
    }
    void SetParentWindowId(const int32_t parentWindowId) override
    {
    }
};
#endif //ACCESSIBILITY_ACCESSIBILITY_ELEMENT_OPERATOR_FUZZ_IMPL_H
