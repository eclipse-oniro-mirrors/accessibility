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
#include <set>
#include "accessibility_window_info.h"
#include "event_handler.h"
#include "ffrt.h"
#include "singleton.h"
#include "window_manager.h"
#include "safe_map.h"

namespace OHOS {
namespace Accessibility {

constexpr int32_t SCENE_BOARD_WINDOW_ID = 1; // default scene board window id 1
constexpr int64_t INVALID_SCENE_BOARD_ELEMENT_ID = -1; // invalid scene board element id -1

class AccessibilityWindowManager {
    DECLARE_SINGLETON(AccessibilityWindowManager)
public:
    bool Init();
    void DeInit();
    void WinDeInit();
    static AccessibilityWindowInfo CreateAccessibilityWindowInfo(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    static void UpdateAccessibilityWindowInfo(AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    static bool CheckIntegerOverflow(const Rosen::Rect& rect);
    int32_t ConvertToRealWindowId(int32_t windowId, int32_t focusType);
    void RegisterWindowListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    void DeregisterWindowListener();
    void SetActiveWindow(int32_t windowId, bool isSendEvent = true);
    int32_t GetActiveWindowId();
    void SetAccessibilityFocusedWindow(int32_t windowId);
    std::vector<AccessibilityWindowInfo> GetAccessibilityWindows();
    bool GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window);
    std::string GetA11yWindowsBundleName(int32_t windowId, std::string bundleName);
    void SetEventInfoBundleName(const AccessibilityEventInfo &uiEvent);
    void SetEventInfoBundleNameOld(const AccessibilityEventInfo &uiEvent, const int32_t windowId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    bool IsValidWindow(int32_t windowId);
    void ClearAccessibilityFocused();

    // used for window id 1, scene board
    int64_t GetSceneBoardElementId(const int32_t windowId, const int64_t elementId);
    int32_t GetRealWindowId(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    bool IsSceneBoard(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    bool IsScenePanel(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);
    bool IsKeyboardDialog(const sptr<Rosen::AccessibilityWindowInfo> windowInfo);

    // used for batch query, provide window and element id translation
    void GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId);
    void GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid);

    RetError GetFocusedWindowId(int32_t &focusedWindowId);

    // test for ut to resize a window
    void SetWindowSize(int32_t windowId, Rect rect);

    void OnWindowUpdate(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos, Rosen::WindowUpdateType type);

    bool IsInnerWindowRootElement(int64_t elementId);

    void InsertTreeIdWindowIdPair(int32_t treeId, int32_t windowId);

    void RemoveTreeIdWindowIdPair(int32_t treeId);

    // return 0 when not found
    int32_t FindTreeIdWindowIdPair(int32_t treeId);

    bool CheckEvents();

    std::map<int32_t, AccessibilityWindowInfo> a11yWindows_ {};
    int32_t activeWindowId_ = INVALID_WINDOW_ID;
    int32_t a11yFocusedWindowId_ = INVALID_WINDOW_ID;
    std::set<int32_t> subWindows_ {}; // used for window id 1, scene board

    // used for arkui windowId 1 map to WMS windowId
    class SceneBoardElementIdMap {
    public:
        SceneBoardElementIdMap() = default;
        ~SceneBoardElementIdMap() = default;
        void InsertPair(const int32_t windowId, const int64_t elementId);
        void RemovePair(const int32_t windowId);
        bool CheckWindowIdPair(const int32_t windowId);
        std::map<int32_t, int64_t> GetAllPairs();
        void Clear();
    private:
        std::map<int32_t, int64_t> windowElementMap_;
        ffrt::mutex mapMutex_;
    };
    SceneBoardElementIdMap sceneBoardElementIdMap_ = {};

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

    class WindowRotationChangeListener : public Rosen::IWindowRotationChangeListener {
    public:
        explicit WindowRotationChangeListener();
        ~WindowRotationChangeListener() = default;

        virtual void OnRotationChange(const Rosen::RotationChangeInfo& rotationChangeInfo,
            Rosen::RotationChangeResult& rotationChangeResult) override;
    };

    bool CompareRect(const Rect &rectAccessibility, const Rosen::Rect &rectWindow);
    bool EqualFocus(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> &windowInfo);
    bool EqualBound(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> &windowInfo);
    bool EqualProperty(Accessibility::AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> &windowInfo);
    bool EqualLayer(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
        const sptr<Rosen::AccessibilityWindowInfo> &windowInfo);
    void WindowUpdateAdded(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateRemoved(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateBounds(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateActive(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateFocused(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateProperty(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateTypeEvent(const int32_t realWidId, std::map<int32_t,
        AccessibilityWindowInfo> &oldA11yWindows_, Accessibility::WindowUpdateType type);
    void WindowUpdateTypeEventAdded(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateTypeEventRemoved(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateTypeEventBounds(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateTypeEventFocused(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateTypeEventProperty(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateTypeEventLayer(const int32_t realWidId,
        std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_);
    void WindowUpdateAll(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos);
    void WindowUpdateAllExec(std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_,
        int32_t realWid, const sptr<Rosen::AccessibilityWindowInfo>& window);
    void ClearOldActiveWindow();

    sptr<AccessibilityWindowListener> windowListener_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    ffrt::recursive_mutex interfaceMutex_; // mutex for interface to make sure AccessibilityWindowManager thread-safe
    SafeMap<int32_t, int32_t> windowTreeIdMap_; // map for tree id to window id
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_MANGER_H