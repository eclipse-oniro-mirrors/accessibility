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
#include "accessibility_window_info.h"
#include "event_handler.h"
#include "singleton.h"
#include "window_manager.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowManager {
    DECLARE_SINGLETON(AccessibilityWindowManager)
public:
    bool Init();
    void DeInit();
    static AccessibilityWindowInfo CreateAccessibilityWindowInfo(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    static void UpdateAccessibilityWindowInfo(AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    int32_t ConvertToRealWindowId(int32_t windowId, int32_t focusType);
    void RegisterWindowListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    void DeregisterWindowListener();
    void SetActiveWindow(int32_t windowId);
    void SetAccessibilityFocusedWindow(int32_t windowId);
    std::vector<AccessibilityWindowInfo> GetAccessibilityWindows();
    bool GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window);
    bool IsValidWindow(int32_t windowId);
    void ClearAccessibilityFocused();

    // test for ut to resize a window
    void SetWindowSize(int32_t windowId, Rect rect);

    void OnWindowUpdate(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos, Rosen::WindowUpdateType type);

    std::map<int32_t, AccessibilityWindowInfo> a11yWindows_ {};
    int32_t activeWindowId_ = INVALID_WINDOW_ID;
    int32_t a11yFocusedWindowId_ = INVALID_WINDOW_ID;

private:
    class AccessibilityWindowListener : public Rosen::IWindowUpdateListener {
    public:
        explicit AccessibilityWindowListener(AccessibilityWindowManager &windInfoMgr)
            : windInfoMgr_(windInfoMgr) {}
        ~AccessibilityWindowListener() = default;

        virtual void OnWindowUpdate(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos,
            Rosen::WindowUpdateType type) override
        {
            windInfoMgr_.OnWindowUpdate(infos, type);
        }

    private:
        AccessibilityWindowManager &windInfoMgr_;
    };

    void WindowUpdateAdded(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateRemoved(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateBounds(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateActive(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateFocused(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateProperty(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void ClearOldActiveWindow();

    sptr<AccessibilityWindowListener> windowListener_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_MANGER_H