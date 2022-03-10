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

#ifndef ACCESSIBILITY_WINDOW_MANGER_H
#define ACCESSIBILITY_WINDOW_MANGER_H

#include <map>
#include <memory>
#include <vector>

#include "accessibility_window_info.h"
#include "singleton.h"
#include "window_manager.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowListener : public Rosen::IWindowUpdateListener {
public:
    virtual void OnWindowUpdate(const sptr<Rosen::WindowInfo>& windowInfo, Rosen::WindowUpdateType type) override;
};

class AccessibilityWindowInfoManager {
public:
    ~AccessibilityWindowInfoManager() = default;
    static AccessibilityWindowInfoManager &GetInstance();
    static AccessibilityWindowInfo CreateAccessibilityWindowInfo(Rosen::WindowInfo &windowInfo);
    int ConvertToRealWindowId(int windowId, int focusType);
    void RegisterWindowChangeListener();
    void DeregisterWindowChangeListener();
    void SetActiveWindow(int windowId);
    void SetAccessibilityFocusedWindow(int windowId);
    void SetInputFocusedWindow(int windowId);
    std::vector<AccessibilityWindowInfo> GetAccessibilityWindows();
    bool GetAccessibilityWindow(int windowId, AccessibilityWindowInfo &window);
    bool IsValidWindow(int windowId);

    // test for ut to resize a window
    void SetWindowSize(int windowId, Rect rect);

    std::map<int, AccessibilityWindowInfo> a11yWindows_ {};
    int activeWindowId_ = INVALID_WINDOW_ID;
    int a11yFocusedWindowId_ = INVALID_WINDOW_ID;
    int inputFocusedWindowId_ = 0;

    bool registerFlag_ = false;
    sptr<AccessibilityWindowListener> windowListener_ = nullptr;

private:
    AccessibilityWindowInfoManager() : windowListener_(new AccessibilityWindowListener()) {}
    DISALLOW_COPY_AND_MOVE(AccessibilityWindowInfoManager);
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_WINDOW_MANGER_H