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
 
#ifndef DUMMY_H
#define DUMMY_H

#include <stdint.h>
#include <string>
#include <vector>
#include "refbase.h"

#define ACCESSIBLE_ABILITY_MANAGER_SERVICE_ID 801 // to be deleted

namespace OHOS {
namespace Accessibility {

class IASACStub {};
class IClient {};
// class TouchEventInjector:public OHOS::RefBase{};
 /*class AccessibilityInputFilter:public OHOS::RefBase{};
 class KeyEventFilter:public OHOS::RefBase{};

class AccessibilityZoomProxy:public OHOS::RefBase{};*/

struct AccessibilityAbilityInfoDummy
{
    uint32_t accessibilityEventTypes = 0xFFFFFFFF;   // TYPES_ALL_MASK

    // ACCESSIBILITY_ABILITY_TYPE_SPOKEN | ACCESSIBILITY_ABILITY_TYPE_HAPTIC | ACCESSIBILITY_ABILITY_TYPE_AUDIBLE
    uint32_t accessibilityAbilityTypes = 0x00000001 | 0x00000002 | 0x00000004; 

    // CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM | CAPABILITY_RETRIEVE
    uint32_t capabilities = 0x0008 | 0x0002 | 0x0010 | 0x0001; 

    std::string description =
    "When TalkBack is on, it provides spoken feedback so that you can use your device without looking at the screen.";

    uint32_t uiInteractiveTime = 10000;

    uint32_t notificationTimeout = 0;

    std::string settingsAbility = "com.ohos.talkback.TalkBackPreferencesActivity";

    // The following two properties talkback is not configured.
    uint32_t uiNoninteractiveTime = 0;
    std::vector<std::string> filterBundleNames{};
};

// WMS dummy start
enum WMError {
    WM_OK = 0,
    WM_NG
};

struct WMDisplayInfo:public OHOS::RefBase {
    int32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t vsync;
    uint32_t dpi; // 额外的像素密度，比如hi3516的mipi屏是 60mmx120mm, 480x960, dpi=203
};

struct GetFocusWindowResult:public OHOS::RefBase {
    WMError wret; // 返回结果
    int32_t wid; // 窗口id
};

class IWindowManagerDisplayListenerClazz {
public:
    IWindowManagerDisplayListenerClazz() = default;
    virtual ~IWindowManagerDisplayListenerClazz() {};
    virtual void OnScreenPlugin(int32_t did) = 0;
    virtual void OnScreenPlugout(int32_t did) = 0;
};

enum WindowMode {
    WINDOW_MODE_UNSET = 0,
    WINDOW_MODE_FREE = 1,
    WINDOW_MODE_TOP = 2,
    WINDOW_MODE_MAX,
};

enum WMWindowType {
    WINDOW_TYPE_NORMAL = 0,
    WINDOW_TYPE_STATUS_BAR = 1,
    WINDOW_TYPE_NAVI_BAR = 2,
    WINDOW_TYPE_ALARM_SCREEN = 3,
    WINDOW_TYPE_SYSTEM_UI = 4,
    WINDOW_TYPE_LAUNCHER = 5,
    WINDOW_TYPE_VIDEO = 6,
    WINDOW_TYPE_MAX,
};

struct GetWindowInfoResult:public OHOS::RefBase {
    WMError wret; // 调用返回值
    int32_t wid; // 窗口ID
    int32_t zorder; // 层级
    int32_t width; // 窗口边界
    int32_t height; // 窗口边界
    int32_t positionX; // 窗口边界
    int32_t positionY; // 窗口边界
    int32_t anchorX; // 锚点
    int32_t anchorY; // 锚点
    bool isFocused; // 是否有焦点
    WindowMode mode; // 窗口模式(画中画)
    WMWindowType type; // 窗口类型
};

class IWindowChangeListenerClazz {
public:
    IWindowChangeListenerClazz() = default;
    virtual ~IWindowChangeListenerClazz() {};
    virtual void OnWindowCreate(int32_t wid) = 0;
    virtual void OnWindowDestroy(int32_t wid) = 0;
};

class IWindowManagerService : public RefBase {
public:
    WMError GetDisplays(std::vector<struct WMDisplayInfo> &displays)
    { 
        WMDisplayInfo display;
        display.dpi = 1;
        display.width = 1000;
        display.height = 1000;

        displays.push_back(display);
        return WM_OK;
    }
    WMError AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener) {return WM_OK;}
    sptr<struct GetFocusWindowResult> GetFocusWindowID() {return nullptr;}

    sptr<struct GetWindowInfoResult> GetWindowInfo(int32_t wid) {
      
        window = new GetWindowInfoResult();
        
        window->wid = wid;
        window->type = WINDOW_TYPE_NORMAL;
        return window;
    }

    sptr<struct GetWindowInfoResult> window;
    void OnWindowListChange(IWindowChangeListenerClazz *listener) {}
};
// WMS dummy end

} // namespace Accessibility
} // namespace OHOS
#endif // DUMMY_H