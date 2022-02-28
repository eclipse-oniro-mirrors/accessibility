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

#ifndef ACCESSIBILITY_EXTENSION_CONTEXT_H
#define ACCESSIBILITY_EXTENSION_CONTEXT_H

#include <map>
#include <memory>
#include <optional>
#include <stdint.h>

#include "accessibility_element_info.h"
#include "accessibility_window_info.h"
#include "display_resize_controller.h"
#include "extension_context.h"
#include "gesture_simulation.h"
#include "key_event.h"

namespace OHOS {
namespace Accessibility {
enum GlobalAction : uint32_t {
    GLOBAL_ACTION_INVALID = 0x0000,
    GLOBAL_ACTION_BACK = 0x0001,
    GLOBAL_ACTION_HOME = 0x0002,
    GLOBAL_ACTION_RECENT = 0x0003,
    GLOBAL_ACTION_NOTIFICATION = 0x0004,
    GLOBAL_ACTION_POP_UP_POWER_DIALOG = 0x0006,
    GLOBAL_ACTION_DIVIDE_SCREEN = 0x0007,
    GLOBAL_ACTION_LOCK_SCREEN = 0x0008,
    GLOBAL_ACTION_CAPTURE_SCREEN = 0x0009
};

class AccessibilityExtensionContext : public AbilityRuntime::ExtensionContext {
public:
    AccessibilityExtensionContext() = default;
    virtual ~AccessibilityExtensionContext() = default;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetFocusElementInfo(uint32_t focusType, std::optional<AccessibilityElementInfo>& elementInfo);

    /**
     * @brief Sends simulate gestures to the screen.
     * @param gesturePathDefineList The gesture which need to send.
     * @param listener The listener of the gesture.
     * @return Return true if the gesture sends successfully, else return false.
     */
    bool GestureSimulate(uint32_t sequence, const std::vector<GesturePathDefine>& gesturePathDefineList,
        const std::shared_ptr<GestureResultListener>& listener);

    /**
     * @brief Obtains the default displayResize controller.
     * @param
     * @return Return the default displayResize controller.
     */
    std::shared_ptr<DisplayResizeController>& GetDisplayResizeController();

    /**
     * @brief Obtains the specified displayResize controller by displayId.
     * @param displayId The id of display.
     * @return Return the specified displayResize controller.
     */
    std::shared_ptr<DisplayResizeController>& GetDisplayResizeController(uint32_t displayId);

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetRootElementInfo(std::optional<AccessibilityElementInfo>& elementInfo);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param
     * @return The infomation of windows.
     */
    std::vector<AccessibilityWindowInfo>& GetWindows();

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::GlobalAction.
     * @return Return true if executes action successfully, else return false.
     */
    bool ExecuteCommonAction(uint32_t action);

    /**
     * @brief Dispatch the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param result The result of gesture completion.
     * @return
     */
    void DispatchOnSimulationGestureResult(uint32_t sequence, bool result);

    /**
     * @brief Set channelId.
     * @param channelId The id of channel.
     * @return
     */
    void SetChannelId(uint32_t channelId);

private:
    /**
     * @brief Check the type of common action.
     * @param action The type of common action.
     * @return Return true if the type of common action is right, else return false.
     */
    bool CheckCommonAction(uint32_t action);

    uint32_t channelId_ = 0xFFFFFFFF;
    std::vector<AccessibilityWindowInfo> accessibilityWindow_ {};
    std::map<uint32_t, std::shared_ptr<GestureResultListener>> gestureResultListenerInfos_;
    std::map<uint32_t, std::shared_ptr<DisplayResizeController>> displayResizeControllers_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EXTENSION_CONTEXT_H