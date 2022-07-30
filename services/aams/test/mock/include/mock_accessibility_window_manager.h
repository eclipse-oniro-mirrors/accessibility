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

#ifndef MOCK_ACCESSIBILITY_WINDOW_MANGER_H
#define MOCK_ACCESSIBILITY_WINDOW_MANGER_H

#include <gmock/gmock.h>
#include "accessibility_window_manager.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityWindowListener : public AccessibilityWindowListener {
public:
    MOCK_METHOD2(OnWindowUpdate, void(const sptr<Rosen::AccessibilityWindowInfo>& windowInfo,
        Rosen::WindowUpdateType type));
};

class MockAccessibilityWindowInfoManager : public AccessibilityWindowManager {
public:
    ~MockAccessibilityWindowInfoManager() = default;
    MOCK_METHOD1(CreateAccessibilityWindowInfo, AccessibilityWindowInfo(
        sptr<Rosen::AccessibilityWindowInfo> windowInfo));
    MOCK_METHOD2(ConvertToRealWindowId, int32_t(int32_t windowId, int32_t focusType));
    MOCK_METHOD1(RegisterWindowListener, void(const std::shared_ptr<AppExecFwk::EventHandler> &handler));
    MOCK_METHOD0(DeregisterWindowListener, void());
    MOCK_METHOD1(SetActiveWindow, void(int32_t windowId));
    MOCK_METHOD1(SetAccessibilityFocusedWindow, void(int32_t windowId));
    MOCK_METHOD0(GetAccessibilityWindows, std::vector<AccessibilityWindowInfo>());
    MOCK_METHOD2(GetAccessibilityWindow, bool(int32_t windowId, AccessibilityWindowInfo& window));
    MOCK_METHOD1(IsValidWindow, bool(int32_t windowId));
    MOCK_METHOD2(SetWindowSize, void(int32_t windowId, Rect rect));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_WINDOW_MANGER_H