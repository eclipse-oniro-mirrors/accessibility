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

#ifndef ACCESSIBILITY_INFO_H
#define ACCESSIBILITY_INFO_H

#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "parcel.h"

namespace OHOS {
namespace Accessibility {
enum ActionType :int {
    ACCESSIBILITY_ACTION_INVALID = 0,
    ACCESSIBILITY_ACTION_FOCUS =  0x00000001,
    ACCESSIBILITY_ACTION_CLEAR_FOCUS = 0x00000002,
    ACCESSIBILITY_ACTION_SELECT = 0x00000004,
    ACCESSIBILITY_ACTION_CLEAR_SELECTION = 0x00000008,
    ACCESSIBILITY_ACTION_CLICK = 0x00000010,
    ACCESSIBILITY_ACTION_LONG_CLICK = 0x00000020,
    ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS = 0x00000040,
    ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS = 0x00000080,
    ACCESSIBILITY_ACTION_SCROLL_FORWARD = 0x00000100,
    ACCESSIBILITY_ACTION_SCROLL_BACKWARD = 0x00000200,
    ACCESSIBILITY_ACTION_COPY = 0x00000400,
    ACCESSIBILITY_ACTION_PASTE = 0x00000800,
    ACCESSIBILITY_ACTION_CUT = 0x00001000,
    ACCESSIBILITY_ACTION_SET_SELECTION = 0x00002000,
    ACCESSIBILITY_ACTION_SET_TEXT = 0x00004000,
    ACCESSIBILITY_ACTION_NEXT_TEXT = 0x00200000,
    ACCESSIBILITY_ACTION_PREVIOUS_TEXT = 0x00400000,
    ACCESSIBILITY_ACTION_UNFOLD = 0x00800000,
    ACCESSIBILITY_ACTION_FOLD = 0x01000000,
    ACCESSIBILITY_ACTION_NEXT_HTML_ITEM = 0x02000000,
    ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM = 0x04000000,
    ACCESSIBILITY_ACTION_DELETED = 0x08000000,
    ACCESSIBILITY_ACTION_TYPE_MASK = 0x1FFFFFFF,
};

//text move step 
enum TextMoveUnit : int {
    STEP_INVALID = 0,
    STEP_CHARACTER = 0x00000001,
    STEP_WORD = 0x00000002,
    STEP_LINE = 0x00000004,
    STEP_PAGE = 0x00000008,
    STEP_PARAGRAPH = 0x00000010,
};
//text move step 
static const std::string MOVE_UNIT_CHARACTER = "char";
static const std::string MOVE_UNIT_WORD = "word";
static const std::string MOVE_UNIT_LINE = "line";
static const std::string MOVE_UNIT_PAGE = "page";
static const std::string MOVE_UNIT_PARAGRAPH = "paragraph";

//Operation Arguments Type
static const std::string ACTION_ARGU_INVALID = "invalid";
static const std::string ACTION_ARGU_SELECT_TEXT_START = "selectTextBegin";
static const std::string ACTION_ARGU_SELECT_TEXT_END = "selectTextEnd";
static const std::string ACTION_ARGU_HTML_ELEMENT = "htmlItem";
static const std::string ACTION_ARGU_SET_TEXT = "setText";
static const std::string ACTION_ARGU_MOVE_UNIT = "textMoveUnit";

//HtmlItemType
static const std::string HTML_ITEM_INVALID = "invalid";
static const std::string HTML_ITEM_LINK = "link";
static const std::string HTML_ITEM_CONTROL = "control";
static const std::string HTML_ITEM_GRAPHIC = "graphic";
static const std::string HTML_ITEM_LIST_ITEM = "listItem";
static const std::string HTML_ITEM_LIST = "list";
static const std::string HTML_ITEM_TABLE = "table";
static const std::string HTML_ITEM_COMBOX = "combox";
static const std::string HTML_ITEM_HEADING = "heading";
static const std::string HTML_ITEM_BUTTON = "button";
static const std::string HTML_ITEM_CHECKBOX = "checkBox";
static const std::string HTML_ITEM_LANDMARK = "landmark";
static const std::string HTML_ITEM_TEXT_FIELD= "textField";
static const std::string HTML_ITEM_FOCUSABLE = "focusable";
static const std::string HTML_ITEM_H1 = "h1";
static const std::string HTML_ITEM_H2 = "h2";
static const std::string HTML_ITEM_H3= "h3";
static const std::string HTML_ITEM_H4 = "h4";
static const std::string HTML_ITEM_H5 = "h5";
static const std::string HTML_ITEM_H6 = "h6";
static const std::string HTML_ITEM_UNKOWN = "unknown";

enum FocusMoveDirection :int {
    DIRECTION_INVALID = 0,
    UP = 0x00000001,
    DOWN = 0x00000002,
    LEFT = 0x00000004,
    RIGHT = 0x00000008,
    FORWARD = 0x00000010,
    BACKWARD = 0x00000020,
};
static const int PREFETCH_PREDECESSORS = 1 << 0;
static const int PREFETCH_SIBLINGS = 1 << 1;
static const int PREFETCH_CHILDREN = 1 << 2;
static const int UNDEFINED = -1;
static const int GET_SOURCE_PREFETCH_MODE =
        PREFETCH_PREDECESSORS
        | PREFETCH_SIBLINGS
        | PREFETCH_CHILDREN;
static const int UNDEFINED_CHANNEL_ID = -1;
static const int UNDEFINED_SELECTION_INDEX = -1;
static const int UNDEFINED_ITEM_ID = 0x1FFFFFF;
static const int ROOT_ITEM_ID = -1;
static const int MAX_TEXT_LENGTH = 500; 
static const int HOST_VIEW_ID = -1;
static const int ROOT_NODE_ID = -1;

// Focus types
static const int FOCUS_TYPE_INVALID = -1;
static const int FOCUS_TYPE_INPUT = 1 << 0;
static const int FOCUS_TYPE_ACCESSIBILITY = 1 << 1;

//grid mode
static const int SELECTION_MODE_NONE = 0;
static const int SELECTION_MODE_SINGLE = 1;
static const int SELECTION_MODE_MULTIPLE = 2;
/*
* class define the action on Accessibility info
*/
class AccessibleAction : public Parcelable{
public:
    /**
     * @brief Construct
     * @param 
     * @return 
     */
    AccessibleAction() {}

    /**
     * @brief Construct
     * @param actionType The type of action, refer to [ActionType]
     * @param description The description message of action.
     * @return 
     */
    AccessibleAction(ActionType actionType, std::string description);

    /**
     * @brief Gets the action type.
     * @param -
     * @return The type of action, refer to [ActionType]
     */
    ActionType GetActionType();

    /**
     * @brief Gets the action description.
     * @param -
     * @return he description message of action.
     */
    std::string GetDescriptionInfo();

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel date successfully; otherwise is not
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibleAction from parcel data
     */
    static AccessibleAction *Unmarshalling(Parcel &parcel);
private:
    ActionType actionType_ = ACCESSIBILITY_ACTION_INVALID;
    std::string description_ = "";
};

/**
 * @brief  Define the RangInfo for progress bar
 * @note   
 * @retval None
 */
class RangeInfo : public Parcelable{
public:
    /**
     * @brief Construct
     * @param 
     * @return 
     */
    RangeInfo() {}

    /**
     * @brief Construct
     * @param min The min value
     * @param max The max value
     * @param current current value
     * @return 
     */
    RangeInfo(int min, int max, int current);

    /**
     * @brief Gets the min value.
     * @param 
     * @return min value
     */
    int GetMin();

    /**
     * @brief Gets the max value.
     * @param 
     * @return max value.
     */
    int GetMax();

    /**
     * @brief Gets the current value.
     * @param 
     * @return 
     */
    int GetCurrent();

    /**
     * @brief Sets the min value.
     * @param min min value
     * @return 
     */
    void SetMin(int min);

    /**
     * @brief Sets the max value.
     * @param max max value.
     * @return 
     */
    void SetMax(int max);

    /**
     * @brief Sets the current value.
     * @param current current value
     * @return 
     */
    void SetCurrent(int current);

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     */
    static RangeInfo *Unmarshalling(Parcel &parcel);
private:    
    int min_ = 0;
    int max_ = 0;
    int current_ = 0;
};

/**
 * @brief  Define the list/grid component
 * @note
 * @retval None
 */
class GridInfo : public Parcelable{
public:
    /**
     * @brief Construct
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    GridInfo() {}

    /**
     * @brief Construct
     * @param rowCount The number of row
     * @param columnCount The number of column
     * @param mode 0: select one line only, otherwise select multilines.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    GridInfo(int rowCount, int columnCount, int mode);

    /**
     * @brief Set the grid object
     * @param rowCount The number of row
     * @param columnCount The number of column
     * @param mode 0: select one line only, otherwise select multilines. 
     * @return grid object
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(int rowCount, int columnCount, int mode);

    /**
     * @brief Copy grid object.
     * @param other The copied grid
     * @return grid object
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(GridInfo other);

    /**
     * @brief Gets the number of rows.
     * @param -
     * @return number of rows.
     * @since 3
     * @sysCap Accessibility
     */
    int GetRowCount();

    /**
     * @brief Gets the number of columns.
     * @param 
     * @return number of columns.
     * @since 3
     * @sysCap Accessibility
     */
    int GetColumnCount();

    /**
     * @brief Get the mode of grid
     * @param 
     * @return 0: Selected by one line, otherwise is multilines.
     * @since 3
     * @sysCap Accessibility
     */
    int GetSelectionMode();

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    static GridInfo *Unmarshalling(Parcel &parcel);
private:
    int rowCount_ = 0;
    int columnCount_ = 0;
    int selectionMode_ = 0;       
};

class GridItemInfo : public Parcelable{
public:
    /**
     * @brief Construct
     * @param 
     * @return 
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
     * @param heading true: The item isHeading, otherwise is not
     * @param selected true: The item is selected,otherwise is not
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    GridItemInfo(int rowIndex, int rowSpan, int columnIndex, int columnSpan, bool heading, bool selected);

    /**
     * @brief Copy the GridItemInfo
     * @param other The object of GridItemInfo copied.
     * @return GridItemInfo object
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
     * @param heading true: The item isHeading, otherwise is not
     * @param selected true: The item is selected,otherwise is not
     * @return GridItemInfo object
     * @since 3
     * @sysCap Accessibility
     */
    void SetGridItemInfo(int rowIndex, int rowSpan,
            int columnIndex, int columnSpan, bool heading, bool selected);

    /**
     * @brief Gets the column index at which the item is located.
     * @param 
     * @return The column index. 
     * @since 3
     * @sysCap Accessibility
     */
    int GetColumnIndex();

    /**
     * @brief Gets the row index at which the item is located.
     * @param 
     * @return The row index. 
     * @since 3
     * @sysCap Accessibility
     */
    int GetRowIndex();

    /**
     * @brief Gets the number of columns the item spans.
     * @param 
     * @return The column span. 
     * @since 3
     * @sysCap Accessibility
     */
    int GetColumnSpan();

    /**
     * @brief Gets the number of rows the item spans.
     * @param 
     * @return The row span. 
     * @since 3
     * @sysCap Accessibility
     */
    int GetRowSpan();

    /**
     * @brief Checks if the grid item is a heading.
     * @param 
     * @return true: If the item is a heading, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsHeading();

    /**
     * @brief Checks if the grid item is a selected.
     * @param 
     * @return true: If the item is a selected, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsSelected();

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    static GridItemInfo *Unmarshalling(Parcel &parcel);
private:
    bool heading_ = false;
    int columnIndex_ = 0;
    int rowIndex_ = 0;
    int columnSpan_ = 0;
    int rowSpan_ = 0;
    bool selected_ = 0;
};

class Rect : public Parcelable {
public:
    /**
     * @brief Construct
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    Rect() {}

    /**
     * @brief Destruct
     * @param -
     * @return 
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
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    Rect(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY) {
        leftTopX_ = leftTopX;
        leftTopY_ = leftTopY;
        rightBottomX_ = rightBottomX;
        rightBottomY_ = rightBottomY;
    }

    /**
     * @brief Get the left top point's pixel coordinates
     * @param -
     * @return The left top x pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int GetLeftTopXScreenPostion() {
        return leftTopX_;
    }

    /**
     * @brief Get the left top point's pixel coordinates
     * @param -
     * @return The left top y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int GetLeftTopYScreenPostion() {
        return leftTopY_;
    }

    /**
     * @brief Get the right bottom point's pixel coordinates
     * @param -
     * @return The bottom x pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int GetRightBottomXScreenPostion() {
        return rightBottomX_;
    }

    /**
     * @brief Get the right bottom point's pixel coordinates
     * @param -
     * @return The bottom y pixel coordinates
     * @since 3
     * @sysCap Accessibility
     */
    int GetRightBottomYScreenPostion() {
        return rightBottomY_;
    }
    /**
     * @brief Set the left top point's pixel coordinates
     * @param leftTopX(out) The left top x pixel coordinates
     * @param leftTopY(out) The left top y pixel coordinates
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetLeftTopScreenPostion(int leftTopX, int leftTopY) {
        leftTopY_ = leftTopY;
        leftTopX_ = leftTopX;
    }

    /**
     * @brief Get the right bottom point's pixel coordinates
     * @param rightBottomX(out) The right bottom x pixel coordinates
     * @param rightBottomY(out) The right bottom y pixel coordinates
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetRightBottomScreenPostion(int rightBottomX, int rightBottomY) {
        rightBottomY_ = rightBottomY;
        rightBottomX_ = rightBottomX;
    }

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    static Rect *Unmarshalling(Parcel &parcel);

private:
    int leftTopX_ = 0;
    int leftTopY_ = 0;
    int rightBottomX_= 0;
    int rightBottomY_ = 0;
};

/*
* The class supply the api to set/get ui component property
*/
class AccessibilityElementInfo : public Parcelable {
public:
    static const int UNDEFINED_ACCESSIBILITY_ID = -1;
    static const int MAX_SIZE = 50;

    /**
     * @brief Construct
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityElementInfo();

    /**
     * @brief Set the AccessibilityElementInfo
     * @param componentId The id of component.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentId(const int componentId);

    /**
     * @brief Gets information about the node that gains accessibility focus
     * @param focus Indicates the focus type, which is described in AccessibilityElementInfo#FOCUS_TYPE_ACCESSIBILITY.
     * @return Returns information about the focused accessibility node.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetFocus(const int focus, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Gets the next focused node in the specified direction of the currently focused node.
     * @param direction Indicates the direction to obtain the next focused node. Refer to FocusMoveDirection
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    bool GetNext(const FocusMoveDirection direction, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get the childe accessibility Id by index.
     * @param index The index of child
     * @return accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int GetChildId(const int index) const;

    /**
     * @brief Gets the number of children
     * @param -
     * @return The number of children
     * @since 3
     * @sysCap Accessibility
     */
    int GetChildCount() const;

    /**
     * @brief Gets the id of children
     * @param -
     * @return The list of children id
     * @since 3
     * @sysCap Accessibility
     */
    std::vector<int> GetChildIds() const;

    /**
     * @brief Get the child node information by index
     * @param index The index of child
     * @param elementInfo The componet info of child
     * @return true: get child component info; otherwise is null
     * @since 3
     * @sysCap Accessibility
     */
    bool GetChild(const int index, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Add child node information
     * @param childId The id of child node
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void AddChild(const int childId);

    /**
     * @brief Remove child specified.
     * @param childId Remove child
     * @return true: Removed succeed, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool RemoveChild(const int childId);

    /**
     * @brief Gets an action list.
     * @param -
     * @return action list.  Refer to ActionType
     * @since 3
     * @sysCap Accessibility
     */
    std::vector<AccessibleAction> GetActionList() const;

    /**
     * @brief Add action on the component
     * @param action The action on the component.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void AddAction(AccessibleAction &action);

    /**
     * @brief Remove action on the component
     * @param action The action object.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void DeleteAction(AccessibleAction &action);

    /**
     * @brief Remove the action on the component.
     * @param actionType The action type.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    bool DeleteAction(ActionType &actionType);

    /**
     * @brief Remove all the action on the component.
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void DeleteAllActions();

    /**
     * @brief Sets the maximum length of text allowed for this node.
     * @param max The maximum length of text
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetTextLengthLimit(const int max);

    /**
     * @brief Gets the maximum length of text allowed for this node.
     * @param 
     * @return The maximum length of text
     * @since 3
     * @sysCap Accessibility
     */
    int GetTextLengthLimit() const;

    /**
     * @brief Executes a specified action.
     * @param action: the action type 
     * @param actionArguments: The parameter for action type. such as:
     *      action: ACCESSIBILITY_ACTION_NEXT_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_NEXT_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_SET_SELECTION,
     *                  actionArguments({ACTION_ARGU_SELECT_TEXT_START,"1"(start location)},
     *                                  {ACTION_ARGU_SELECT_TEXT_END,"10"(end location)})
     *      action: ACCESSIBILITY_ACTION_SET_TEXT,
     *                  actionArguments(ACTION_ARGU_SET_TEXT,"the text of setted")
     * @return true Perform action succeed, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ExecuteAction(const ActionType &action, const std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Searches for node information based on the specified content.
     * @param text specified content
     * @return The child node infomations matched the text filterred.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetByContent(const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Searches for node information by elementId.
     * @param elementId The unique id of the accessibility ID. It composed by componentId(component id) and virtualId
     *                       accessibility ID Low 32 bit: It is componentId(component id), the id of component
     *                       accessibility ID High 32 bit: virtualId, It is used when the component is complex and
     *                              componentId can't describe the component clearly otherwise the virtualId maybe is -1.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only. 
     * @return node information 
     * @since 3
     * @sysCap Accessibility
     */
    bool GetElementInfosById(const int elementId, int mode, std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Get the window Id of the component is belongs to the window.
     * @param 
     * @return window id
     * @since 3
     * @sysCap Accessibility
     */
    int GetWindowId() const;

    /**
     * @brief Set the window Id of the component is belongs to the window.
     * @param windowId
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int windowId);

    /**
     * @brief Get Parent node information
     * @param 
     * @return Parent node information
     * @since 3
     * @sysCap Accessibility
     */
    bool GetParent(AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get Parent accessibility Id.
     * @param 
     * @return accessibility Id.
     * @since 3
     * @sysCap Accessibility
     */
    int GetParentNodeId() const;

    /**
     * @brief Set Parent node information
     * @param parentId Parent node id
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetParent(const int parentId);

    /**
     * @brief Gets the rectangular area of this accessibility node control in the screen.
     * @param 
     * @return The rectangular area of this accessibility node
     * @since 3
     * @sysCap Accessibility
     */
    Rect GetRectInScreen() const;

    /**
     * @brief Set the rectangular area of this accessibility node control in the screen.
     * @param bounds The rectangular area of this accessibility node
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetRectInScreen(Rect &bounds);

    /**
     * @brief Checks whether this node (a check box as an example) is checkable.
     * @param 
     * @return whether this node (a check box as an example) is checkable.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsCheckable() const;

    /**
     * @brief Set  whether this node (a check box as an example) is checkable.
     * @param 
     * @return true : Is checkable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    void SetCheckable(const bool checkable);

    /**
     * @brief Checks whether this node is checked.
     * @param 
     * @return true : Is checked, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsChecked() const;

    /**
     * @brief Set whether this node is checked.
     * @param checked true : Is checked, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetChecked(const bool checked);

    /**
     * @brief Checks whether this node can be focused.
     * @param 
     * @return true : Can be focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocusable() const;

    /**
     * @brief Set whether this node can be focused.
     * @param focusable true : Can be focused, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocusable(const bool focusable);

    /**
     * @brief Checks whether this node has gained focus.
     * @param 
     * @return true : Focused, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsFocused();

    /**
     * @brief Set whether this node has gained focus.
     * @param focused true : Focused, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetFocused(const bool focused);

    /**
     * @brief Checks whether this node is visible to users.
     * @param 
     * @return true : visible, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsVisible() const;

    /**
     * @brief Set whether this node is visible to users.
     * @param visible true : visible, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetVisible(const bool visible);

    /**
     * @brief Checks whether this node has gained accessibility focus.
     * @param 
     * @return true : Gained accessibility focus, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool HasAccessibilityFocus() const;

    /**
     * @brief Set whether this node has gained accessibility focus.
     * @param focused true : Gained accessibility focus, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityFocus(const bool focused);

    /**
     * @brief Checks whether this node is selected.
     * @param 
     * @return true: selected, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsSelected() const;

    /**
     * @brief Set whether this node is selected.
     * @param selected true: selected, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelected(const bool selected);

    /**
     * @brief Checks whether this node is clickable.
     * @param 
     * @return true: clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsClickable() const;

    /**
     * @brief Set whether this node is clickable.
     * @param clickable true: clickable, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetClickable(const bool clickable);

    /**
     * @brief Checks whether this node is long clickable.
     * @param 
     * @return true: long clickable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsLongClickable() const;

    /**
     * @brief Set whether this node is long clickable.
     * @param longClickable true: long clickable, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetLongClickable(const bool longClickable);

    /**
     * @brief Checks whether this node is enabled.
     * @param 
     * @return true: enabled, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEnabled() const;

    /**
     * @brief Set whether this node is enabled.
     * @param enabled true: enabled, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetEnabled(const bool enabled);

    /**
     * @brief Checks whether the content in this node is a password.
     * @param 
     * @return true: password, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPassword() const;

    /**
     * @brief Set whether the content in this node is a password
     * @param password true: password, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetPassword(const bool password);

    /**
     * @brief Checks whether this node is scrollable.
     * @param 
     * @return true: scrollable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsScrollable() const;

    /**
     * @brief Set whether this node is scrollable.
     * @param scrollable true: scrollable, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetScrollable(const bool scrollable);

    /**
     * @brief Checks whether this node is editable.
     * @param 
     * @return true: editable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEditable() const;

    /**
     * @brief Set whether this node is editable.
     * @param editable true: editable, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetEditable(const bool editable);

    /**
     * @brief Checks whether this node can display text in multiple lines.
     * @param 
     * @return true: multilines, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPluraLineSupported() const;

    /**
     * @brief Set whether this node can display text in multiple lines.
     * @param multiLine true: multilines, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetPluraLineSupported(const bool multiLine);

    /**
     * @brief Checks whether pop-up windows are supported.
     * @param 
     * @return true: Support popup, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsPopupSupported() const;

    /**
     * @brief Set whether pop-up windows are supported.
     * @param supportPopup true: Support popup, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetPopupSupported(const bool supportPopup);

    /**
     * @brief Checks whether this node is deletable.
     * @param 
     * @return true: deletable, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsDeletable() const;

    /**
     * @brief Set whether this node is deletable.
     * @param deletable true: deletable, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetDeletable(const bool deletable);

    /**
     * @brief Checks whether this node is essential to users.
     * @param 
     * @return true: essential to user, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsEssential() const;

    /**
     * @brief Set whether this node is essential to users.
     * @param essential true: essential to user, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetEssential(const bool essential);

    /**
     * @brief Checks whether this node is displaying a hint.
     * @param 
     * @return true: displaying a hint, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsGivingHint() const;

    /**
     * @brief Set whether this node is displaying a hint.
     * @param hinting true: displaying a hint, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetHinting(const bool hinting);

    /**
     * @brief Gets the bundle name of application target.
     * @param 
     * @return bundle name
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetBundleName() const;

    /**
     * @brief Set the bundle name of application target.
     * @param bundleName The bundle name of application target.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetBundleName(const std::string &bundleName);

    /**
     * @brief Gets the class name.
     * @param 
     * @return class name. 
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetComponentType() const;

    /**
     * @brief Sets the class name.
     * @param className class name. 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentType(const std::string &className);

    /**
     * @brief Gets the text of node.
     * @param 
     * @return The text of node
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetContent() const;

    /**
     * @brief Set the text of node.
     * @param text The text of node
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetContent(const std::string &text);

    /**
     * @brief Gets the accessibility text of node.
     * @param 
     * @return the accessibility text of node
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetAccessibilityContent() const;

    /**
     * @brief Set the accessibility text of node.
     * @param text The accessibility text of node.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityContent(const std::string &text);

    /**
     * @brief Gets  the accessibility description of node.
     * @param 
     * @return The accessibility description of node.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetAccessibilityDescription() const;

    /**
     * @brief Set the accessibility description of node.
     * @param text The accessibility description of node.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityDescription(const std::string &text);

    /**
     * @brief Checks whether this node is accessibility group.
     * @param 
     * @return true Accessibility group, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetAccessibilityGroup() const;

    /**
     * @brief Set  whether this node is accessibility group
     * @param group true Accessibility group, otherwise is not.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityGroup(const bool group);

    /**
     * @brief Gets the hint information.
     * @param 
     * @return the hint information. 
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetHint() const;

    /**
     * @brief Sets the hint information.
     * @param hintText the hint information. 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetHint(const std::string &hintText);

    /**
     * @brief Gets the description of the accessibility node.
     * @param 
     * @return the description of the accessibility node.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetDescriptionInfo() const;

    /**
     * @brief Set the description of the accessibility node.
     * @param contentDescription the description of the accessibility node.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetDescriptionInfo(const std::string contentDescription);

    /**
     * @brief Set the resource name of the component.
     * @param viewIdResName The resource name.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentResourceId(const std::string &viewIdResName);

    /**
     * @brief Gets the resource name.
     * @param 
     * @return the resource name.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetComponentResourceId() const;

    /**
     * @brief Set whether this node has live region
     * @note If the node has live region, the changed information will be notified to user. such as:
     *       Input error password.
     * @param liveRegion live region: 0: not live region; 1: interrupt current talkback; 2: talk back by order
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetLiveRegion(const int liveRegion);
    /**
     * @brief Get whether this node has live region
     * @param 
     * @return live region
     * @since 3
     * @sysCap Accessibility
     */
    int GetLiveRegion() const;

    /**
     * @brief Set whether this node has content Invalid.
     * @note If the node has content Invalid,when input invalid information, it will be talkbacked. such as:
     * The editbox permit number only, you input character("a"), The invalid information will be talkbacked.
     * @param contentInvalid content Invalid 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetContentInvalid(const bool contentInvalid);

    /**
     * @brief Get whether this node has content Invalid.
     * @param 
     * @return true Is setted content invalid, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool GetContentInvalid() const;

    /**
     * @brief Set error information, it used with contentInvalid is setted true.
     * @param error error information
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetError(const std::string &error);

    /**
     * @brief Get error information,it used with contentInvalid is setted true.
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetError();

    /**
     * @brief Set the id of component labeled
     * @param componentId the id of component
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetLabeled(const int componentId);

    /**
     * @brief Get the node information labeled component
     * @param 
     * @return the node information labeled 
     * @since 3
     * @sysCap Accessibility
     */
    bool GetLabeled(AccessibilityElementInfo &elementInfo) const;

    /**
     * @brief Get labeled accessibility Id
     * @param 
     * @return accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int GetLabeledAccessibilityId() const;

    /**
     * @brief Get the id of the IAccessibleAbilityChannel
     * @param 
     * @return The id of the IAccessibleAbilityChannel
     * @since 3
     * @sysCap Accessibility
     */
    int GetChannelId() const;

    /**
     * @brief AAMS called to set the id of the IAccessibleAbilityChannel
     * @param channelId The id of the IAccessibleAbilityChannel
     * @param 
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetChannelId(const int channelId);

    /**
     * @brief  Set accessibility Id
     * @param componentId The id of component
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetAccessibilityId(const int componentId);

    /**
     * @brief Get accessibility Id
     * @param 
     * @return accessibility Id
     * @since 3
     * @sysCap Accessibility
     */
    int GetAccessibilityId() const;

    /**
     * @brief Get the object of RangeInfo
     * @param 
     * @return the object of RangeInfo
     * @since 3
     * @sysCap Accessibility
     */
    RangeInfo GetRange() const;

    /**
     * @brief Set the object of RangeInfo
     * @param rangeInfo the object of RangeInfo
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetRange(RangeInfo &rangeInfo);

    /**
     * @brief Set the start location of text selected.
     * @param start the end location of text selected.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelectedBegin(const int start);

    /**
     * @brief Get the start location of text selected.
     * @param 
     * @return the start location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    int GetSelectedBegin() const;

    /**
     * @brief Set the end location of text selected.
     * @param end the end location of text selected.
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetSelectedEnd(const int end);

    /**
     * @brief Get the end location of text selected.
     * @param 
     * @return the end location of text selected.
     * @since 3
     * @sysCap Accessibility
     */
    int GetSelectedEnd() const;

    /**
     * @brief Get the object of GridInfo
     * @param 
     * @return the object of GridInfo
     * @since 3
     * @sysCap Accessibility
     */
    GridInfo GetGrid() const;

    /**
     * @brief Set the object of GridInfo
     * @param grid the object of GridInfo
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetGrid(const GridInfo &grid);

    /**
     * @brief Get the object of GridItemInfo
     * @param 
     * @return the object of GridItemInfo
     * @since 3
     * @sysCap Accessibility
     */
    GridItemInfo GetGridItem() const;

    /**
     * @brief Set the object of GridItemInfo
     * @param gridItem the object of GridItemInfo
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetGridItem(const GridItemInfo &gridItem);

    /**
     * @brief Get the current index of list or location text
     * @param 
     * @return the current index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int GetCurrentIndex() const;

    /**
     * @brief Set the current index of list or location text
     * @param index the current index of list or location text
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetCurrentIndex(const int index);

    /**
     * @brief  Get the start index of list or location text
     * @param 
     * @return the start index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int GetBeginIndex() const;

    /**
     * @brief  Set the start index of list or location text
     * @param index the start index of list or location text
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeginIndex(const int index);

    /**
     * @brief Get the end index of list or location text
     * @param 
     * @return the end index of list or location text
     * @since 3
     * @sysCap Accessibility
     */
    int GetEndIndex() const;

    /**
     * @brief Set the end index of list or location text
     * @param index the end index of list or location text
     * @return 
     * @since 3
     * @sysCap Accessibility
     */
    void SetEndIndex(const int index);

    /**
     * @brief Get the input type of text
     * @param
     * @return The input type of text
     * @since 3
     * @sysCap Accessibility
     */
    int GetInputType() const;

    /**
     * @brief Set the input type of text
     * @param inputType The input type of text
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetInputType(const int inputType);

    /**
     * @brief Checks whether this node is valid
     * @param
     * @return true: valid, otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool IsValidElement() const;

    /**
     * @brief Set whether this node is valid
     * @param valid true: valid, otherwise is not.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetValidElement(const bool valid);

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /*Parcel*/
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibilityElementInfo from parcel data
     * @since 3
     * @sysCap Accessibility
     */
    static AccessibilityElementInfo *Unmarshalling(Parcel &parcel);

private:
    int windowId_ = -1;
    int elementId_ = UNDEFINED_ACCESSIBILITY_ID;
    int parentId_ = UNDEFINED_ACCESSIBILITY_ID;
    std::string bundleName_ = "";
    std::string componentType_ = "";
    std::string text_ = "";
    std::string hintText_ = "";
    std::string accessibilityText_ = "";
    std::string accessibilityDescription_ = "";
    std::string contentDescription_ = "";
    std::string resourceName_ = "";
    std::vector<int>childNodeIds_{};
    int childCount_ = 0;
    std::vector<AccessibleAction> operations_{};
    int textLengthLimit_ = -1;
    int channelId_ = UNDEFINED_CHANNEL_ID; //rename
    Rect bounds_ {};
    bool accessibilityGroup_ = false;
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
    bool password_ = false;
    bool scrollable_ = false;
    bool editable_ = false;
    bool popupSupported_ = false;
    bool multiLine_ = false;
    bool deletable_ = false;
    bool hint_ = false;
    bool isEssential_ = false;
    int currentIndex_ = 0;
    int beginIndex_ = 0;
    int endIndex_= 0;
    RangeInfo rangeInfo_ {};
    GridInfo grid_{};
    GridItemInfo gridItem_{};
    int liveRegion_ = 0;
    bool contentInvalid_ = true;
    std::string error_ = "";
    int labeled_ = 0;
    int beginSelected_ = 0;
    int endSelected_ = 0;
    int inputType_ = 0; //text input type added
    bool validElement_ = true;
};

} //namespace Accessibility
} //namespace OHOS
#endif