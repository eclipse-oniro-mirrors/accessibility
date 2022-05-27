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

#ifndef MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H
#define MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H

#include <gmock/gmock.h>
#include "accessibility_element_operator_stub.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityElementOperatorStub : public AccessibilityElementOperatorStub {
public:
    MockAccessibilityElementOperatorStub() = default;
    virtual ~MockAccessibilityElementOperatorStub() = default;

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD4(SearchElementInfoByAccessibilityId,
        void(const int32_t elementId, const int32_t requestId,
            const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode));
    MOCK_METHOD4(
        SearchElementInfosByText, void(const int32_t elementId, const std::string& text, const int32_t requestId,
                                      const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD4(FindFocusedElementInfo, void(const int32_t elementId, const int32_t focusType, const int32_t requestId,
                                             const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD4(FocusMoveSearch, void(const int32_t elementId, const int32_t direction, const int32_t requestId,
                                      const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(ExecuteAction,
        void(const int32_t elementId, const int32_t action, const std::map<std::string, std::string> &actionArguments,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD0(ClearFocus, void());
    MOCK_METHOD0(OutsideTouch, void());
    MOCK_METHOD1(SetWindowId, void(int32_t windowId));
    MOCK_METHOD0(GetWindowId, int32_t());
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H