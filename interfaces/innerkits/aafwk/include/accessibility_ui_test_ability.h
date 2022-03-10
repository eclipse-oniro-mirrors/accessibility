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

#ifndef ACCESSIBILITY_UI_TEST_ABILITY_H
#define ACCESSIBILITY_UI_TEST_ABILITY_H

#include <memory>
#include "accessibility_element_info.h"
#include "accessibility_window_info.h"
#include "accessibility_ui_test_ability_listener.h"

namespace OHOS {
namespace Accessibility {
enum UiTestGlobalAction : uint32_t {
    UI_GLOBAL_ACTION_INVALID = 0x0000,
    UI_GLOBAL_ACTION_BACK = 0x0001,
    UI_GLOBAL_ACTION_HOME = 0x0002,
    UI_GLOBAL_ACTION_RECENT = 0x0003,
    UI_GLOBAL_ACTION_NOTIFICATION = 0x0004,
    UI_GLOBAL_ACTION_POP_UP_POWER_DIALOG = 0x0006,
    UI_GLOBAL_ACTION_DIVIDE_SCREEN = 0x0007,
    UI_GLOBAL_ACTION_LOCK_SCREEN = 0x0008,
    UI_GLOBAL_ACTION_CAPTURE_SCREEN = 0x0009
};

class AccessibilityUITestAbility {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~AccessibilityUITestAbility() = default;

    /**
     * @brief Gets an instance of AccessibleAbility.
     * @param
     * @return Return an instance of AccessibleAbility.
     */
    static std::shared_ptr<AccessibilityUITestAbility> GetInstance();

    /**
     * @brief RegisterListener
     * @param
     * @return
     */
    virtual bool RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener) = 0;

    /**
     * @brief Connect to AAMS.
     * @param
     * @return
     */
    virtual bool Connect() = 0;

    /**
     * @brief disconnect to AAMS.
     * @param
     * @return
     */
    virtual bool Disconnect() = 0;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetFocusElementInfo(uint32_t focusType, std::optional<AccessibilityElementInfo> &elementInfo) = 0;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetRootElementInfo(std::optional<AccessibilityElementInfo> &elementInfo) = 0;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param
     * @return The information of windows.
     */
    virtual std::vector<AccessibilityWindowInfo> GetWindows() = 0;

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::UiTestGlobalAction.
     * @return Return true if executes action successfully, else return false.
     */
    virtual bool ExecuteCommonAction(UiTestGlobalAction action) = 0;

    /**
     * @brief Dispatch the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param result The result of gesture completion.
     * @return
     */
    virtual void DispatchOnSimulationGestureResult(uint32_t sequence, bool result) = 0;

    /**
     * @brief Set channelId.
     * @param channelId The id of channel.
     * @return
     */
    virtual void SetChannelId(uint32_t channelId) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_UI_TEST_ABILITY_H