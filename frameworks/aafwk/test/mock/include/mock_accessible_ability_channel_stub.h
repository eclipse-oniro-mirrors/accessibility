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

#ifndef MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H
#define MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H

#include "accessible_ability_channel_stub.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityChannelStub : public AccessibleAbilityChannelStub {
public:
    MockAccessibleAbilityChannelStub();
    virtual ~MockAccessibleAbilityChannelStub();

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD5(SearchElementInfoByAccessibilityId,
        bool(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t requestId,
            const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode));
    MOCK_METHOD5(SearchElementInfosByText,
        bool(const int32_t accessibilityWindowId, const int32_t elementId, const std::string& text,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(FindFocusedElementInfo,
        bool(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t focusType,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(
        FocusMoveSearch, bool(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t direction,
                             const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));

    MOCK_METHOD6(ExecuteAction, bool(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t action,
                                    std::map<std::string, std::string>& actionArguments, const int32_t requestId,
                                    const sptr<IAccessibilityElementOperatorCallback>& callback));

    MOCK_METHOD0(GetWindows, std::vector<AccessibilityWindowInfo>());
    MOCK_METHOD1(ExecuteCommonAction, bool(const int32_t action));

    MOCK_METHOD2(SetOnKeyPressEventResult, void(const bool handled, const int32_t sequence));
    MOCK_METHOD1(GetDisplayResizeScale, float(const int32_t displayId));
    MOCK_METHOD1(GetDisplayResizeCenterX, float(const int32_t displayId));
    MOCK_METHOD1(GetDisplayResizeCenterY, float(const int32_t displayId));
    MOCK_METHOD1(GetDisplayResizeRect, Rect(const int32_t displayId));
    MOCK_METHOD2(ResetDisplayResize, bool(const int32_t displayId, const bool animate));
    MOCK_METHOD5(SetDisplayResizeScaleAndCenter,
        bool(const int32_t displayId, const float scale, const float centerX, const float centerY, const bool animate));
    MOCK_METHOD2(
        SendSimulateGesture, void(const int32_t requestId, const std::vector<AccessibilityGesturePath>& gestureSteps));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H