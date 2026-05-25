/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_FOR_ELEMENT_OPERATOR_MANAGER_H
#define MOCK_FOR_ELEMENT_OPERATOR_MANAGER_H

#include <gmock/gmock.h>
#include "accessibility_window_connection.h"
#include "iaccessibility_element_operator.h"
#include "iaccessibility_element_operator_callback.h"
#include "accessibility_element_info.h"
#include "accessibility_def.h"

#define MOCK_ROOT_PARENT_ELEMENT_ID 1000

struct SpecificPropertyParam {};

namespace OHOS {
namespace Accessibility {

class MockElementOperatorManagerWindowConnection : public AccessibilityWindowConnection {
public:
    MockElementOperatorManagerWindowConnection(int32_t windowId, const sptr<IAccessibilityElementOperator>& connection,
        int32_t accountId) : AccessibilityWindowConnection(windowId, connection, accountId) {}
    virtual ~MockElementOperatorManagerWindowConnection() = default;

    sptr<IAccessibilityElementOperator> GetProxy() { return mockOperator_; }
    sptr<IAccessibilityElementOperator> GetCardProxy(const int32_t treeId) { return mockOperator_; }
    RetError SetCardProxy(const int32_t treeId, sptr<IAccessibilityElementOperator> operation)
    {
        return RET_OK;
    }
    void EraseProxy(const int32_t treeId) {}
    void GetAllTreeId(std::vector<int32_t> &treeIds) {}
    void GetRootParentId(int32_t treeId, int64_t &elementId) { elementId = MOCK_ROOT_PARENT_ELEMENT_ID; }
    RetError SetRootParentId(const int32_t treeId, const int64_t elementId) { return RET_OK; }

private:
    sptr<IAccessibilityElementOperator> mockOperator_ = nullptr;
};

class MockElementOperatorManagerElementOperator : public IAccessibilityElementOperator {
public:
    MockElementOperatorManagerElementOperator() = default;
    virtual ~MockElementOperatorManagerElementOperator() = default;

    RetError SearchElementInfoByAccessibilityId(const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter) override { return RET_OK; }

    void SearchDefaultFocusedByWindowId(const int32_t windowId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode, bool isFilter) override {}

    void SearchElementInfosByText(const int64_t elementId, const std::string &text,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void FocusMoveSearch(const int64_t elementId, const int32_t direction,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void ExecuteAction(const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void GetCursorPosition(const int64_t elementId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void SearchElementInfoBySpecificProperty(const int64_t elementId,
        const SpecificPropertyParam& param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    void ClearFocus() override {}

    void OutsideTouch() override {}

    void SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId,
        const int32_t childWindowId) override {}

    void SetBelongTreeId(const int32_t treeId) override {}

    void SetParentWindowId(const int32_t parentWindowId) override {}

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_FOR_ELEMENT_OPERATOR_MANAGER_H
