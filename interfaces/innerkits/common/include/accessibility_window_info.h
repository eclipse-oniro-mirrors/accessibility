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

#ifndef ACCESSIBILITY_WINDOW_INFO_H
#define ACCESSIBILITY_WINDOW_INFO_H

#include <vector>
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowInfo {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityWindowInfo();

    /**
     * @brief Get the window type.
     * @return The accessibility window type. Refer to [AccessibilityWindowType].
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityWindowType GetAccessibilityWindowType() const;

    /**
     * @brief Set the window type.
     * @param type The accessibility window type. Refer to [AccessibilityWindowType].
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityWindowType(const AccessibilityWindowType type);

    /**
     * @brief Get The window layer
     * @return The layer of window
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetWindowLayer() const;

    /**
     * @brief Set The window layer
     * @param layer The window layer
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowLayer(const int32_t layer);

    /**
     * @brief Get the window id of current window.
     * @return The window id of current window.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetWindowId() const;

    /**
     * @brief Set the window id of current window.
     * @param id The window id of current window.
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int32_t id);

    /**
     * @brief Get the screen pixel coordinates
     * @return The screen pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    const Rect &GetRectInScreen() const;

    /**
     * @brief Set the screen pixel coordinates
     * @param bounds The screen pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    void SetRectInScreen(const Rect &bounds);

    /**
     * @brief Check whether the window is active or not.
     * @return true: the window is active, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsActive() const;

    /**
     * @brief Set whether the window is active or not.
     * @param active true: the window is active, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetActive(bool active);

    /**
     * @brief Checks whether this window has gained focus.
     * @return true: the window has gained focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocused() const;

    /**
     * @brief Set whether this window has gained focus.
     * @param focused true: the window has gained focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocused(bool focused);

    /**
     * @brief Checks whether this window has gained accessibility focus.
     * @return true: this window has gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsAccessibilityFocused() const;

    /**
     * @brief Set whether this window has gained accessibility focus.
     * @param accessibilityFocused true: this window has gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityFocused(const bool accessibilityFocused);

    /**
     * @brief Set the display id
     * @param displayId the display id of the window
     * @sysCap Accessibility
     */
    void SetDisplayId(const uint64_t displayId);

    /**
     * @brief Get the display id
     * @return Return the display id of the window.
     * @sysCap Accessibility
     */
    uint64_t GetDisplayId() const;

    /**
     * @brief Get window type
     * @return The type of window. Refer to [OHOS::Rosen::WindowType].
     * @sysCap Accessibility
     */
    uint32_t GetWindowType() const;

    /**
     * @brief Set window type
     * @param type The type of window. Refer to [OHOS::Rosen::WindowType].
     * @sysCap Accessibility
     */
    void SetWindowType(const uint32_t type);

    /**
     * @brief Get window mode
     * @return The mode of window. Refer to [OHOS::Rosen::WindowMode].
     * @sysCap Accessibility
     */
    uint32_t GetWindowMode() const;

    /**
     * @brief Set window mode
     * @param mode The mode of window. Refer to [OHOS::Rosen::WindowMode].
     * @sysCap Accessibility
     */
    void SetWindowMode(const uint32_t mode);

    /**
     * @brief Get whether the decoration state is enabled
     * @return true:the decoration is enabled; false:the decoration is disabled
     * @sysCap Accessibility
     */
    bool IsDecorEnable() const;

    /**
     * @brief Set the decoration state
     * @param isDecorEnable true:enable decoration; false:disable decoration
     * @sysCap Accessibility
     */
    void SetDecorEnable(const bool isDecorEnable);

protected:
    uint64_t displayId_ = 0;
    uint32_t windowMode_ = 0;
    uint32_t windowType_ = 0;
    AccessibilityWindowType accessibilityWindowType_ = TYPE_WINDOW_INVALID;
    int32_t windowLayer_ = INVALID_WINDOW_ID;
    int32_t windowId_ = INVALID_WINDOW_ID;
    bool active_ = false;
    bool focused_ = false;
    bool accessibilityFocused_ = false;
    Rect boundsInScreen_ {};
    bool isDecorEnable_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_INFO_H