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

#ifndef ACCESSIBILITY_WINDOW_H
#define ACCESSIBILITY_WINDOW_H

#include <cstdint>
#include <vector>
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
enum WindowType : int {
    TYPE_WINDOW_INVALID = 0,
    TYPE_ACCESSIBILITY_OVERLAY = 0x00000001,
    TYPE_APPLICATION = 0x00000002,
    TYPE_INPUT_METHOD = 0x00000004,
    TYPE_SPLIT_SCREEN_DIVIDER = 0x00000008,
    TYPE_SYSTEM = 0x00000010,
};
static const int INVALID_WINDOW_ID = -1;
static const int ANY_WINDOW_ID = -2;
static const int ACTIVE_WINDOW_ID = 0x7FFFFFFF;

class AccessibilityWindowInfo : public Parcelable {
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
    void SetWindowTitle(std::string &title);

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
    int GetWindowLayer() const;

    /**
     * @brief Set The window layer
     * @param layer The window layer
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowLayer(const int layer);

    /**
     * @brief Get the root node information of the window.
     * @param -
     * @return The root node information.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetRootAccessibilityInfo(AccessibilityElementInfo &elementInfo);

    /**
     * @brief Set an anchor accessibility node that anchors this window to another window.
     * @param anchorId The accessibility id of component.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetAnchorId(const int anchorId);

    /**
     * @brief Obtains an anchor accessibility node that anchors this window to another window.
     * @param -
     * @return The node information of the anchored component.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetAnchor(AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get the parent window
     * @param -
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityWindowInfo GetParent();

    /**
     * @brief Set the parent window
     * @param parentId The window id of the parent.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetParentId(const int parentId);

    /**
     * @brief Get the parent window
     * @param -
     * @return The window id of the parent.
     * @since 3
     * @sysCap Accessibility
     */
    int GetParentId() const;

    /**
     * @brief Get the anchor window
     * @param -
     * @return The anchor id of the window.
     * @since 3
     * @sysCap Accessibility
     */
    int GetAnchorId() const;

    /**
     * @brief Get the parent window
     * @param -
     * @return The window id of the parent.
     * @since 3
     * @sysCap Accessibility
     */
    std::vector<int>  GetChildIds() const;

    /**
     * @brief Get the widnow id of current window.
     * @param -
     * @return The widnow id of current window.
     * @since 3
     * @sysCap Accessibility
     */
    int GetWindowId() const;

    /**
     * @brief Set the widnow id of current window.
     * @param id The widnow id of current window.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int id);

    /**
     * @brief AAMS called to set the id of the IAccessibleAbilityChannel.
     * @param channelId The id of the IAccessibleAbilityChannel.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetChannelId(const int channelId);

    /**
     * @brief Get the id of the IAccessibleAbilityChannel.
     * @param
     * @return The id of the IAccessibleAbilityChannel.
     * @since 3
     * @sysCap Accessibility
     */
    int GetChannelId() const;

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
    int GetChildNum() const;

    /**
     * @brief Obtains information about the accessibility child window at a specified index.
     * @param index The index of the child
     * @return The child window specified by index.
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityWindowInfo GetChild(const int index);

    /**
     * @brief Add the child window
     * @param childId the window id of the child
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void AddChild(const int childId);

    /*Parcel*/
    /**
     * @brief Used for IPC communication.
     * @param parcel Serializable data.
     * @return true: Read parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief  Used for IPC communication.
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief  Used for IPC communication.
     * @param parcel
     * @return The data of AccessibilityWindowInfo
     * @since 3
     * @sysCap Accessibility
     */
    static AccessibilityWindowInfo *Unmarshalling(Parcel &parcel);

private:
    WindowType windowType_ = TYPE_WINDOW_INVALID;
    int windowLayer_ = INVALID_WINDOW_ID;
    int windowId_ = INVALID_WINDOW_ID;
    int parentId_ = INVALID_WINDOW_ID;
    std::string windowTitle_ = "";
    std::vector<int> childIds_;
    int anchorId_ = -1;
    int childNum_ = 0;
    int channelId_ = INVALID_WINDOW_ID;
    bool active_ = false;
    bool focused_ = false;
    bool accessibilityFocused_ = false;
    Rect boundsInScreen_ {};
};
} // namespace Accessibility
} // namespace OHOS
#endif
