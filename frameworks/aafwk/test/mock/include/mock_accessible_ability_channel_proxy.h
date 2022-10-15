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

#ifndef MOCK_ACCESSIBLE_ABILITY_CHANNEL_PROXY_H
#define MOCK_ACCESSIBLE_ABILITY_CHANNEL_PROXY_H

#include "accessible_ability_channel_proxy.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityChannelProxy : public AccessibleAbilityChannelProxy {
public:
    MockAccessibleAbilityChannelProxy(const sptr<IRemoteObject>& object);
    virtual ~MockAccessibleAbilityChannelProxy() = default;

    MOCK_METHOD5(SearchElementInfoByAccessibilityId,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t requestId,
            const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode));
    MOCK_METHOD5(SearchElementInfosByText,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const std::string& text,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(FindFocusedElementInfo,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t focusType,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(FocusMoveSearch, RetError(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD6(ExecuteAction, RetError(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t action, const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD2(GetWindow, RetError(const int32_t windowId, AccessibilityWindowInfo &windowInfo));
    MOCK_METHOD1(GetWindows, RetError(std::vector<AccessibilityWindowInfo> &windows));
    MOCK_METHOD2(GetWindowsByDisplayId, RetError(const uint64_t displayId,
        std::vector<AccessibilityWindowInfo> &windows));
    MOCK_METHOD2(SetOnKeyPressEventResult, void(const bool handled, const int32_t sequence));
    MOCK_METHOD1(SendSimulateGesture, RetError(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath));
    MOCK_METHOD1(SetTargetBundleName, RetError(const std::vector<std::string> &targetBundleNames));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CHANNEL_PROXY_H