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

#ifndef ACCESSIBILITY_ELEMENT_INFO_H
#define ACCESSIBILITY_ELEMENT_INFO_H

#include <map>
#include <vector>
#include "accessibility_def.h"

namespace OHOS {
namespace Accessibility {
/*
* class define the action on Accessibility info
*/
class AccessibleAction {
public:
    /**
     * @brief Construct
     */
    AccessibleAction() {}

    /**
     * @brief Construct
     * @param actionType The type of action, refer to [ActionType]
     * @param description The description message of action.
     */
    AccessibleAction(ActionType actionType, const std::string &description);

    /**
     * @brief Gets the action type.
     * @return The type of action, refer to [ActionType]
     */
    ActionType GetActionType() const;

    /**
     * @brief Gets the action description.
     * @return The description message of action.
     */
    const std::string &GetDescriptionInfo() const;

protected:
    ActionType actionType_ = ACCESSIBILITY_ACTION_INVALID;
    std::string description_ = "";
};

/**
 * @brief  Define the RangInfo for progress bar
 * @note
 * @retval None
 */
class RangeInfo {
public:
    /**
     * @brief Construct
     */
    RangeInfo() {}

    /**
     * @brief Construct
     * @param min The min value
     * @param max The max value
     * @param current current value
     */
    RangeInfo(int32_t min, int32_t max, int32_t current);

    /**
     * @brief Gets the min value.
     * @return min value
     */
    int32_t GetMin() const;

    /**
     * @brief Gets the max value.
     * @return max value.
     */
    int32_t GetMax() const;

    /**
     * @brief Gets the current value.
     * @return current value.
     */
    int32_t GetCurrent() const;

    /**
     * @brief Sets the min value.
     * @param min min value
     */
    void SetMin(int32_t min);

    /**
     * @brief Sets the max value.
     * @param max max value.
     */
    void SetMax(int32_t max);

    /**
     * @brief Sets the current value.
     * @param current current value
     */
    void SetCurrent(int32_t current);

protected:
    int32_t min_ = 0;
    int32_t max_ = 0;
    int32_t current_ = 0;
};

/**
 * @brief  Define the list/grid component
 * @note
 * @retval None
 */
class GridInfo {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    GridInfo() {}

    /**
     * @brief Construct
     * @param rowCount The number of row
     * @param columnCount The number of column
     * @param mode 0: select one line only, otherwise select multilines.
     * @since 3
     * @sysCap Accessibility
     */
    GridInfo(int32_t rowCount, int32_t columnCount, int32_t mode);

    /**
     * @brief Set the grid object
     * @param rowCount The number of row
     * @param columnCount The number of column
     * @param mode 0: select one line only, otherwise select multilines.
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(int32_t rowCount, int32_t columnCount, int32_t mode);

    /**
     * @brief Copy grid object.
     * @param other The copied grid
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(GridInfo other);

    /**
     * @brief Gets the number of rows.
     * @return number of rows.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetRowCount() const;

    /**
     * @brief Gets the number of columns.
     * @return number of columns.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetColumnCount() const;

    /**
     * @brief Get the mode of grid
     * @return 0: Selected by one line, otherwise is multilines.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetSelectionMode() const;

protected:
    int32_t rowCount_ = 0;
    int32_t columnCount_ = 0;
    int32_t selectionMode_ = 0;
};

class GridItemInfo {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    GridItemInfo() {}

    /**
     * @brief Construct
     * @param rowIndex The index of row.
     * @param rowSpan  The row spanned.
     * @param columnIndex The index of column
     * @param columnSpan THe column spanned
     * @param heading true:The item isHeading, otherwise is not
     * @param selected true:The item is selected,otherwise is not
     * @since 3
     * @sysCap Accessibility
     */
    GridItemInfo(int32_t rowIndex, int32_t rowSpan, int32_t columnIndex, int32_t columnSpan,
        bool heading, bool selected);

    /**
     * @brief Copy the GridItemInfo
     * @param other The object of GridItemInfo copied.
     * @since 3
     * @sysCap Accessibility
     */
    void SetGridItemInfo(GridItemInfo other);

    /**
     * @brief Set grid object
     * @param rowIndex The index of row.
     * @param rowSpan  The row spanned.
     * @param columnIndex The index of column
     * @param columnSpan THe column spanned
     * @param heading true:The item isHeading, otherwise is not
     * @param selected true:The item is selected,otherwise is not
     * @since 3
     * @sysCap Accessibility
     */
    void SetGridItemInfo(int32_t rowIndex, int32_t rowSpan, int32_t columnIndex, int32_t columnSpan,
        bool heading, bool selected);

    /**
     * @brief Gets the column index at which the item is located.
     * @return The column index.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetColumnIndex() const;

    /**
     * @brief Gets the row index at which the item is located.
     * @return The row index.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetRowIndex() const;

    /**
     * @brief Gets the number of columns the item spans.
     * @return The column span.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetColumnSpan() const;

    /**
     * @brief Gets the number of rows the item spans.
     * @return The row span.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetRowSpan() const;

    /**
     * @brief Checks if the grid item is a heading.
     * @return true:If the item is a heading, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsHeading() const;

    /**
     * @brief Checks if the grid item is a selected.
     * @return true:If the item is a selected, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsSelected() const;

protected:
    bool heading_ = false;
    int32_t columnIndex_ = 0;
    int32_t rowIndex_ = 0;
    int32_t columnSpan_ = 0;
    int32_t rowSpan_ = 0;
    bool selected_ = false;
};

class Rect {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    Rect() {}

    /**
     * @brief Destruct
     * @since 3
     * @sysCap Accessibility
     */
    virtual ~Rect() = default;

    /**
     * @brief Construct
     * @param leftTopX : The left top x pixel coordinates
     * @param leftTopY : The left top y pixel coordinates
     * @param rightBottomY : The right bottom y pixel coordinates
     * @param rightBottomX : The right bottom x pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    Rect(int32_t leftTopX, int32_t leftTopY, int32_t rightBottomX, int32_t rightBottomY)
    {
        leftTopX_ = leftTopX;
        leftTopY_ = leftTopY;
        rightBottomX_ = rightBottomX;
        rightBottomY_ = rightBottomY;
    }

    /**
     * @brief Get the left top point's pixel coordinates
     * @return The left top x pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetLeftTopXScreenPostion() const
    {
        return leftTopX_;
    }

    /**
     * @brief Get the left top point's pixel coordinates
     * @return The left top y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetLeftTopYScreenPostion() const
    {
        return leftTopY_;
    }

    /**
     * @brief Get the right bottom point's pixel coordinates
     * @return The bottom x pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetRightBottomXScreenPostion() const
    {
        return rightBottomX_;
    }

    /**
     * @brief Get the right bottom point's pixel coordinates
     * @return The bottom y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetRightBottomYScreenPostion() const
    {
        return rightBottomY_;
    }
    /**
     * @brief Set the left top point's pixel coordinates
     * @param leftTopX The left top x pixel coordinates
     * @param leftTopY The left top y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    void SetLeftTopScreenPostion(int32_t leftTopX, int32_t leftTopY)
    {
        leftTopY_ = leftTopY;
        leftTopX_ = leftTopX;
    }

    /**
     * @brief Set the right bottom point's pixel coordinates
     * @param rightBottomX The right bottom x pixel coordinates
     * @param rightBottomY The right bottom y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    void SetRightBottomScreenPostion(int32_t rightBottomX, int32_t rightBottomY)
    {
        rightBottomY_ = rightBottomY;
        rightBottomX_ = rightBottomX;
    }

protected:
    int32_t leftTopX_ = 0;
    int32_t leftTopY_ = 0;
    int32_t rightBottomX_ = 0;
    int32_t rightBottomY_ = 0;
};

/*
* The class supply the api to set/get ui component property
*/
class AccessibilityElementInfo {
public:
    static constexpr int32_t UNDEFINED_ACCESSIBILITY_ID = -1;
    static constexpr int32_t MAX_SIZE = 50;

    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityElementInfo();

    /**
     * @brief Set the id of AccessibilityElementInfo
     * @param componentId The id of component.
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentId(const int32_t componentId);

    /**
     * @brief Get the child's accessibility Id by index.
     * @param index The index of child
     * @return The child's accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetChildId(const int32_t index) const;

    /**
     * @brief Gets the number of children
     * @return The number of children
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetChildCount() const;

    /**
     * @brief Gets the id of children
     * @return The list of children id
     * @since 3
     * @sysCap Accessibility
     */
    const std::vector<int32_t> &GetChildIds() const;

    /**
     * @brief Add child node information
     * @param childId The id of child node
     * @since 3
     * @sysCap Accessibility
     */
    void AddChild(const int32_t childId);

    /**
     * @brief Remove child specified.
     * @param childId The child to removed.
     * @return true:Removed succeed, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool RemoveChild(const int32_t childId);

    /**
     * @brief Gets an action list.
     * @return action list.  Refer to AccessibleAction
     * @since 3
     * @sysCap Accessibility
     */
    const std::vector<AccessibleAction> &GetActionList() const;

    /**
     * @brief Add action on the component
     * @param action The action on the component.
     * @since 3
     * @sysCap Accessibility
     */
    void AddAction(AccessibleAction &action);

    /**
     * @brief Remove action on the component
     * @param action The action object.
     * @since 3
     * @sysCap Accessibility
     */
    void DeleteAction(AccessibleAction &action);

    /**
     * @brief Remove the action on the component.
     * @param actionType The action type.
     * @return true:successfully deleted, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool DeleteAction(ActionType &actionType);

    /**
     * @brief Remove all the action on the component.
     * @since 3
     * @sysCap Accessibility
     */
    void DeleteAllActions();

    /**
     * @brief Sets the maximum length of text allowed for this node.
     * @param max The maximum length of text
     * @since 3
     * @sysCap Accessibility
     */
    void SetTextLengthLimit(const int32_t max);

    /**
     * @brief Gets the maximum length of text allowed for this node.
     * @return The maximum length of text
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetTextLengthLimit() const;

    /**
     * @brief Get the window Id of the component that belongs to the window.
     * @return window id
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetWindowId() const;

    /**
     * @brief Set the window Id of the component that belongs to the window.
     * @param windowId The window Id
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int32_t windowId);

    /**
     * @brief Get parent accessibility Id.
     * @return The accessibility Id of parent.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetParentNodeId() const;

    /**
     * @brief Set parent node information
     * @param parentId Parent node id
     * @since 3
     * @sysCap Accessibility
     */
    void SetParent(const int32_t parentId);

    /**
     * @brief Gets the rectangular area of this accessibility node control in the screen.
     * @return The rectangular area of this accessibility node
     * @since 3
     * @sysCap Accessibility
     */
    const Rect &GetRectInScreen() const;

    /**
     * @brief Set the rectangular area of this accessibility node control in the screen.
     * @param bounds The rectangular area of this accessibility node
     * @since 3
     * @sysCap Accessibility
     */
    void SetRectInScreen(Rect &bounds);

    /**
     * @brief Checks whether this node (a check box as an example) is checkable.
     * @return true:the node is checkable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsCheckable() const;

    /**
     * @brief Set whether this node (a check box as an example) is checkable.
     * @param checkable true:the node is checkable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetCheckable(const bool checkable);

    /**
     * @brief Checks whether this node is checked.
     * @return true : Is checked, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsChecked() const;

    /**
     * @brief Set whether this node is checked.
     * @param checked true:Is checked, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetChecked(const bool checked);

    /**
     * @brief Checks whether this node can be focused.
     * @return true : Can be focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocusable() const;

    /**
     * @brief Set whether this node can be focused.
     * @param focusable true : Can be focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocusable(const bool focusable);

    /**
     * @brief Checks whether this node has gained focus.
     * @return true:Focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocused() const;

    /**
     * @brief Set whether this node has gained focus.
     * @param focused true : Focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocused(const bool focused);

    /**
     * @brief Checks whether this node is visible to users.
     * @return true : visible, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsVisible() const;

    /**
     * @brief Set whether this node is visible to users.
     * @param visible true:visible, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetVisible(const bool visible);

    /**
     * @brief Checks whether this node has gained accessibility focus.
     * @return true:Gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool HasAccessibilityFocus() const;

    /**
     * @brief Set whether this node has gained accessibility focus.
     * @param focused true:Gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityFocus(const bool focused);

    /**
     * @brief Checks whether this node is selected.
     * @return true:selected, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsSelected() const;

    /**
     * @brief Set whether this node is selected.
     * @param selected true: selected, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelected(const bool selected);

    /**
     * @brief Checks whether this node is clickable.
     * @return true: clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsClickable() const;

    /**
     * @brief Set whether this node is clickable.
     * @param clickable true:clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetClickable(const bool clickable);

    /**
     * @brief Checks whether this node is long clickable.
     * @return true: long clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsLongClickable() const;

    /**
     * @brief Set whether this node is long clickable.
     * @param longClickable true: long clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetLongClickable(const bool longClickable);

    /**
     * @brief Checks whether this node is enabled.
     * @return true:enabled, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEnabled() const;

    /**
     * @brief Set whether this node is enabled.
     * @param enabled true: enabled, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetEnabled(const bool enabled);

    /**
     * @brief Checks whether the content in this node is a password.
     * @return true: password, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPassword() const;

    /**
     * @brief Set whether the content in this node is a password
     * @param type true:password, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetPassword(const bool type);

    /**
     * @brief Checks whether this node is scrollable.
     * @return true: scrollable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsScrollable() const;

    /**
     * @brief Set whether this node is scrollable.
     * @param scrollable true: scrollable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetScrollable(const bool scrollable);

    /**
     * @brief Checks whether this node is editable.
     * @return true:editable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEditable() const;

    /**
     * @brief Set whether this node is editable.
     * @param editable true: editable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetEditable(const bool editable);

    /**
     * @brief Checks whether this node can display text in multiple lines.
     * @return true: multilines, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPluraLineSupported() const;

    /**
     * @brief Set whether this node can display text in multiple lines.
     * @param multiLine true:multilines, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetPluraLineSupported(const bool multiLine);

    /**
     * @brief Checks whether pop-up windows are supported.
     * @return true: Support popup, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPopupSupported() const;

    /**
     * @brief Set whether pop-up windows are supported.
     * @param supportPopup true: Support popup, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetPopupSupported(const bool supportPopup);

    /**
     * @brief Checks whether this node is deletable.
     * @return true:deletable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsDeletable() const;

    /**
     * @brief Set whether this node is deletable.
     * @param deletable true:deletable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetDeletable(const bool deletable);

    /**
     * @brief Checks whether this node is essential to users.
     * @return true: essential to user, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEssential() const;

    /**
     * @brief Set whether this node is essential to users.
     * @param essential true:essential to user, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetEssential(const bool essential);

    /**
     * @brief Checks whether this node is displaying a hint.
     * @return true:displaying a hint, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsGivingHint() const;

    /**
     * @brief Set whether this node is displaying a hint.
     * @param hinting true:displaying a hint, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetHinting(const bool hinting);

    /**
     * @brief Gets the bundle name of application target.
     * @return bundle name
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetBundleName() const;

    /**
     * @brief Set the bundle name of application target.
     * @param bundleName The bundle name of application target.
     * @since 3
     * @sysCap Accessibility
     */
    void SetBundleName(const std::string &bundleName);

    /**
     * @brief Get component type.
     * @return The component type.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetComponentType() const;

    /**
     * @brief Set component type.
     * @param className The component type.
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentType(const std::string &className);

    /**
     * @brief Gets the text of node.
     * @return The text of node
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetContent() const;

    /**
     * @brief Set the text of node.
     * @param text The text of node
     * @since 3
     * @sysCap Accessibility
     */
    void SetContent(const std::string &text);

    /**
     * @brief Gets the hint information.
     * @return the hint information.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetHint() const;

    /**
     * @brief Sets the hint information.
     * @param hintText the hint information.
     * @since 3
     * @sysCap Accessibility
     */
    void SetHint(const std::string &hintText);

    /**
     * @brief Gets the description of the accessibility node.
     * @return the description of the accessibility node.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetDescriptionInfo() const;

    /**
     * @brief Set the description of the accessibility node.
     * @param contentDescription the description of the accessibility node.
     * @since 3
     * @sysCap Accessibility
     */
    void SetDescriptionInfo(const std::string &contentDescription);

    /**
     * @brief Set the resource name of the component.
     * @param viewIdResName The resource name.
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentResourceId(const std::string &viewIdResName);

    /**
     * @brief Gets the resource name.
     * @return the resource name.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetComponentResourceId() const;

    /**
     * @brief Set whether this node has live region
     * @param liveRegion live region: 0: not live region; 1: interrupt current talk back; 2: talk back by order
     * @since 3
     * @sysCap Accessibility
     */
    void SetLiveRegion(const int32_t liveRegion);

    /**
     * @brief Get the live region of the node
     * @return The live region of the node
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetLiveRegion() const;

    /**
     * @brief Set whether this node has content Invalid.
     * @note If the node has content Invalid,when input invalid information, it will be talkbacked. such as:
     * The editbox permit number only, you input character("a"), The invalid information will be talkbacked.
     * @param contentInvalid true:the content is invalid; false:the content is valid
     * @since 3
     * @sysCap Accessibility
     */
    void SetContentInvalid(const bool contentInvalid);

    /**
     * @brief Get whether this node has content Invalid.
     * @return true:the content is invalid; false:the content is valid
     * @since 3
     * @sysCap Accessibility
     */
    bool GetContentInvalid() const;

    /**
     * @brief Set error information, it used with contentInvalid is setted true.
     * @param error error information
     * @since 3
     * @sysCap Accessibility
     */
    void SetError(const std::string &error);

    /**
     * @brief Get error information,it used with contentInvalid is setted true.
     * @return error information
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetError() const;

    /**
     * @brief Set the id of component labeled
     * @param componentId the id of component
     * @since 3
     * @sysCap Accessibility
     */
    void SetLabeled(const int32_t componentId);

    /**
     * @brief Get labeled accessibility Id
     * @return accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetLabeledAccessibilityId() const;

    /**
     * @brief Set accessibility Id
     * @param componentId The id of component
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityId(const int32_t componentId);

    /**
     * @brief Get accessibility Id
     * @return accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetAccessibilityId() const;

    /**
     * @brief Get the object of RangeInfo
     * @return the object of RangeInfo
     * @since 3
     * @sysCap Accessibility
     */
    const RangeInfo &GetRange() const;

    /**
     * @brief Set the object of RangeInfo
     * @param rangeInfo the object of RangeInfo
     * @since 3
     * @sysCap Accessibility
     */
    void SetRange(RangeInfo &rangeInfo);

    /**
     * @brief Set the start location of text selected.
     * @param start the start location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelectedBegin(const int32_t start);

    /**
     * @brief Get the start location of text selected.
     * @return the start location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetSelectedBegin() const;

    /**
     * @brief Set the end location of text selected.
     * @param end the end location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelectedEnd(const int32_t end);

    /**
     * @brief Get the end location of text selected.
     * @return the end location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetSelectedEnd() const;

    /**
     * @brief Get the object of GridInfo
     * @return the object of GridInfo
     * @since 3
     * @sysCap Accessibility
     */
    const GridInfo &GetGrid() const;

    /**
     * @brief Set the object of GridInfo
     * @param grid the object of GridInfo
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(const GridInfo &grid);

    /**
     * @brief Get the object of GridItemInfo
     * @return the object of GridItemInfo
     * @since 3
     * @sysCap Accessibility
     */
    const GridItemInfo &GetGridItem() const;

    /**
     * @brief Set the object of GridItemInfo
     * @param gridItem the object of GridItemInfo
     * @since 3
     * @sysCap Accessibility
     */
    void SetGridItem(const GridItemInfo &gridItem);

    /**
     * @brief Get the current index of list or location text
     * @return the current index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetCurrentIndex() const;

    /**
     * @brief Set the current index of list or location text
     * @param index the current index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    void SetCurrentIndex(const int32_t index);

    /**
     * @brief Get the start index of list or location text
     * @return the start index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetBeginIndex() const;

    /**
     * @brief Set the start index of list or location text
     * @param index the start index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeginIndex(const int32_t index);

    /**
     * @brief Get the end index of list or location text
     * @return the end index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetEndIndex() const;

    /**
     * @brief Set the end index of list or location text
     * @param index the end index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    void SetEndIndex(const int32_t index);

    /**
     * @brief Get the input type of text
     * @return The input type of text
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetInputType() const;

    /**
     * @brief Set the input type of text
     * @param inputType The input type of text
     * @since 3
     * @sysCap Accessibility
     */
    void SetInputType(const int32_t inputType);

    /**
     * @brief Check whether this node is valid
     * @return true:valid, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsValidElement() const;

    /**
     * @brief Set whether this node is valid
     * @param valid true:valid, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetValidElement(const bool valid);

    /**
     * @brief Set inspector key
     * @param inspector The inspector key.
     * @since 3
     * @sysCap Accessibility
     */
    void SetInspectorKey(const std::string &key);

    /**
     * @brief Get inspector key
     * @return The inspector key
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetInspectorKey() const;

    /**
     * @brief Set the path of page.
     * @param path The unique identification of one page.
     * @sysCap Accessibility
     */
    void SetPagePath(const std::string &path);

    /**
     * @brief Get the path of page
     * @return Page path
     * @sysCap Accessibility
     */
    const std::string &GetPagePath() const;

    /**
     * @brief Set page id
     * @param pageId page id.
     * @sysCap Accessibility
     */
    void SetPageId(const int32_t pageId);

    /**
     * @brief Get page id
     * @return page id
     * @sysCap Accessibility
     */
    int32_t GetPageId() const;

    /**
     * @brief Set the text movement step
     * @param granularity text moving unit
     * @sysCap Accessibility
     */
    void SetTextMovementStep(const TextMoveUnit granularity);

    /**
     * @brief Get the text movement step
     * @return Text moving unit
     * @sysCap Accessibility
     */
    TextMoveUnit GetTextMovementStep() const;

    /**
     * @brief Set item count
     * @param itemCounts The count of item
     * @sysCap Accessibility
     */
    void SetItemCounts(const int32_t itemCounts);

    /**
     * @brief Get item count
     * @return The count of item
     * @sysCap Accessibility
     */
    int32_t GetItemCounts() const;

    // The following methods are only used when the target application uses
    // the sendEvent interface to send event data.
    /**
     * @brief Set trigger action
     * @param action The trigger action
     * @sysCap Accessibility
     */
    void SetTriggerAction(const ActionType action);

    /**
     * @brief Get trigger action
     * @return The trigger action
     * @sysCap Accessibility
     */
    ActionType GetTriggerAction() const;

    /**
     * @brief Set content list
     * @param contentList The list of content
     * @sysCap Accessibility
     */
    void SetContentList(const std::vector<std::string> &contentList);

    /**
     * @brief Get content list
     * @param contentList(out) The list of content
     * @sysCap Accessibility
     */
    void GetContentList(std::vector<std::string> &contentList) const;

    /**
     * @brief Set latest content
     * @param content The latest content
     * @sysCap Accessibility
     */
    void SetLatestContent(const std::string &content);

    /**
     * @brief Get latest content
     * @return The latest content
     * @sysCap Accessibility
     */
    const std::string &GetLatestContent() const;

protected:
    int32_t pageId_ = -1;
    int32_t windowId_ = -1;
    int32_t elementId_ = UNDEFINED_ACCESSIBILITY_ID;
    int32_t parentId_ = UNDEFINED_ACCESSIBILITY_ID;
    std::string bundleName_ = "";
    std::string componentType_ = "";
    std::string text_ = "";
    std::string hintText_ = "";
    std::string contentDescription_ = "";
    std::string resourceName_ = "";
    std::string inspectorKey_ = "";
    std::string pagePath_ = "";
    std::vector<int32_t> childNodeIds_;
    int32_t childCount_ = 0;
    std::vector<AccessibleAction> operations_;
    int32_t textLengthLimit_ = -1;
    Rect bounds_ {};
    bool checkable_ = false;
    bool checked_ = false;
    bool focusable_ = false;
    bool focused_ = false;
    bool visible_ = false;
    bool accessibilityFocused_ = false;
    bool selected_ = false;
    bool clickable_ = false;
    bool longClickable_ = false;
    bool enable_ = false;
    bool isPassword_ = false;
    bool scrollable_ = false;
    bool editable_ = false;
    bool popupSupported_ = false;
    bool multiLine_ = false;
    bool deletable_ = false;
    bool hint_ = false;
    bool isEssential_ = false;
    int32_t currentIndex_ = 0;
    int32_t beginIndex_ = 0;
    int32_t endIndex_ = 0;
    RangeInfo rangeInfo_ {};
    GridInfo grid_ {};
    GridItemInfo gridItem_ {};
    int32_t liveRegion_ = 0;
    bool contentInvalid_ = true;
    std::string error_ = "";
    int32_t labeled_ = 0;
    int32_t beginSelected_ = 0;
    int32_t endSelected_ = 0;
    int32_t inputType_ = 0; // text input type added
    bool validElement_ = true;
    TextMoveUnit textMoveStep_ = STEP_CHARACTER;
    int32_t itemCounts_ = 0;
    ActionType triggerAction_ = ACCESSIBILITY_ACTION_INVALID;
    std::vector<std::string> contentList_ {};
    std::string latestContent_ = "";
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ELEMENT_INFO_H