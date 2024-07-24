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
#ifndef MOCK_ACCESSIBILITY_WINDOW_CONNECTION_H
#define MOCK_ACCESSIBILITY_WINDOW_CONNECTION_H

#include <gmock/gmock.h>
#include "accessibility_window_connection.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityWindowConnection : public AccessibilityWindowConnection {
public:
    MockAccessibilityWindowConnection(
        const int32_t windowId, const sptr<IAccessibilityElementOperator>& connection, const int32_t accountId);
    MockAccessibilityWindowConnection(const int32_t windowId, const int32_t treeId,
        const sptr<IAccessibilityElementOperator>& connection, const int32_t accountId);
    ~MockAccessibilityWindowConnection();

    inline sptr<IAccessibilityElementOperator> GetProxy()
    {
        return proxy_;
    }

    sptr<IAccessibilityElementOperator> GetCardProxy(const int32_t treeId);

    RetError SetCardProxy(const int32_t treeId, sptr<IAccessibilityElementOperator> operation);

    void GetAllTreeId(std::vector<int32_t> &treeIds);

    RetError GetRootParentId(int32_t treeId, int64_t &elementId);

    RetError SetRootParentId(const int32_t treeId, const int64_t elementId);
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_WINDOW_CONNECTION_H