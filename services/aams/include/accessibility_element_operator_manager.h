/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H

#include <map>
#include <memory>
#include <mutex>

#include "accessibility_window_connection.h"
#include "iaccessibility_element_operator.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityAccountData;

class ElementOperatorManager {
public:
    ElementOperatorManager() = default;
    ~ElementOperatorManager();

    void AddAccessibilityWindowConnection(
        const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection);

    void RemoveAccessibilityWindowConnection(const int32_t windowId);

    sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(const int32_t windowId);

    const std::map<int32_t, sptr<AccessibilityWindowConnection>> GetAsacConnections();

    void ClearFocus(int32_t windowId);

    void OutsideTouch(int32_t windowId);

    bool GetElementOperator(const int32_t windowId, const int64_t elementId,
        sptr<IAccessibilityElementOperator>& elementOperator);

    void GetElementOperatorConnection(sptr<AccessibilityWindowConnection>& connection,
        const int64_t elementId, sptr<IAccessibilityElementOperator>& elementOperator);

    void SetTokenIdMapAndRootParentId(const sptr<AccessibilityWindowConnection> connection,
        const int32_t treeId, const int64_t nodeId, const uint32_t tokenId);

    ErrCode GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId);

    void RemoveTreeDeathRecipient(int32_t windowId, int32_t treeId,
        const sptr<AccessibilityWindowConnection> connection);

    void Clear();

    size_t GetSize();

private:
    std::map<int32_t, sptr<AccessibilityWindowConnection>> asacConnections_;
    ffrt::mutex asacConnectionsMutex_;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H