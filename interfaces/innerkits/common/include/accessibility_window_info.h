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
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityWindowInfo();

    /**
     * @brief Get the title of window
     * @param -
     * @return The title of window
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetWindowTitle() const;

    /**
     * @brief Set the title of window
     * @param title The title of window
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowTitle(const std::string &title);

    /**
     * @brief Get the window type.
     * @param -
     * @return The window type. Refer to [WindowType].
     * @since 3
     * @sysCap Accessibility
     */
    WindowType GetWindowType() const;

    /**
     * @brief Set the window type.
     * @param type The window type. Refer to [WindowType].
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowType(const WindowType type);

    /**
     * @brief Get The window layer
     * @param -
     * @return The layer of window
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetWindowLayer() const;

    /**
     * @brief Set The window layer
     * @param layer The window layer
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowLayer(const int32_t layer);

    /**
     * @brief Set an anchor accessibility node that anchors this window to another window.
     * @param anchorId The accessibility id of component.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetAnchorId(const int32_t anchorId);

    /**
     * @brief Set the parent window
     * @param parentId The window id of the parent.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetParentId(const int32_t parentId);

    /**
     * @brief Get the parent window
     * @param -
     * @return The window id of the parent.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetParentId() const;

    /**
     * @brief Get the anchor window
     * @param -
     * @return The anchor id of the window.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetAnchorId() const;

    /**
     * @brief Get the parent window
     * @param -
     * @return The window id of the parent.
     * @since 3
     * @sysCap Accessibility
     */
    const std::vector<int32_t> &GetChildIds() const;

    /**
     * @brief Get the widnow id of current window.
     * @param -
     * @return The widnow id of current window.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetWindowId() const;

    /**
     * @brief Set the widnow id of current window.
     * @param id The widnow id of current window.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int32_t id);

    /**
     * @brief AAMS called to set the id of the IAccessibleAbilityChannel.
     * @param channelId The id of the IAccessibleAbilityChannel.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetChannelId(const int32_t channelId);

    /**
     * @brief Get the id of the IAccessibleAbilityChannel.
     * @param
     * @return The id of the IAccessibleAbilityChannel.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetChannelId() const;

    /**
     * @brief Get the screen pixel coordinates
     * @param -
     * @return The screen pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    Rect GetRectInScreen() const;

    /**
     * @brief Set the screen pixel coordinates
     * @param bounds The screen pixel coordinates
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetRectInScreen(const Rect &bounds);

    /**
     * @brief Check whether the window is active or not.
     * @param -
     * @return true: the window is active, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsActive() const;

    /**
     * @brief Set whether the window is active or not.
     * @param active true: the window is active, otherwise is not.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetActive(bool active);

    /**
     * @brief Checks whether this window has gained focus.
     * @param
     * @return true: the window has gained focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocused() const;

    /**
     * @brief Set whether this window has gained focus.
     * @param focused true: the window has gained focus, otherwise is not.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocused(bool focused);

    /**
     * @brief Checks whether this window has gained accessibility focus.
     * @param -
     * @return true: this window has gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsAccessibilityFocused() const;

    /**
     * @brief Set whether this window has gained accessibility focus.
     * @param accessibilityfocused true: this window has gained accessibility focus, otherwise is not.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityFocused(const bool accessibilityfocused);

    /**
     * @brief Obtains the quantity of child windows.
     * @param -
     * @return The quantity of child windows.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetChildNum() const;

    /**
     * @brief Obtains the ID of child windows.
     * @param -
     * @return The ID of child windows.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetChildIdByIndex(const int32_t index) const;

    /**
     * @brief Add the child window
     * @param childId the window id of the child
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void AddChild(const int32_t childId);

    /**
     * @brief Set the display id
     * @param displayId the display id of the window
     * @return -
     * @sysCap Accessibility
     */
    void SetDisplayId(const uint64_t displayId);

    /**
     * @brief Get the display id
     * @return Return the display id of the window.
     * @sysCap Accessibility
     */
    uint64_t GetDisplayId() const;

protected:
    uint64_t displayId_ = 0;
    WindowType windowType_ = TYPE_WINDOW_INVALID;
    int32_t windowLayer_ = INVALID_WINDOW_ID;
    int32_t windowId_ = INVALID_WINDOW_ID;
    int32_t parentId_ = INVALID_WINDOW_ID;
    std::string windowTitle_ = "";
    std::vector<int32_t> childIds_;
    int32_t anchorId_ = -1;
    int32_t childNum_ = 0;
    int32_t channelId_ = INVALID_WINDOW_ID;
    bool active_ = false;
    bool focused_ = false;
    bool accessibilityFocused_ = false;
    Rect boundsInScreen_ {};
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_INFO_H