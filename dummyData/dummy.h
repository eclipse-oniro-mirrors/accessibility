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
class IWindowManagerDisplayListenerClazz {
public:
    IWindowManagerDisplayListenerClazz() = default;
    virtual ~IWindowManagerDisplayListenerClazz() {};
    virtual void OnScreenPlugin(int32_t did) = 0;
    virtual void OnScreenPlugout(int32_t did) = 0;
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
};
// WMS dummy end

} // namespace Accessibility
} // namespace OHOS
#endif // DUMMY_H