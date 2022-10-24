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

#include "accessibility_element_info.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
void AccessibilityElementInfo::SetComponentId(const int32_t componentId)
{
    elementId_ = componentId;
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
}

int32_t AccessibilityElementInfo::GetChildId(const int32_t index) const
{
    HILOG_DEBUG("index[%{public}d]", index);
    if (index >= childCount_ || index < 0) {
        HILOG_ERROR("index[%{public}d] is invalid", index);
        return -1;
    }
    return childNodeIds_[index];
}

int32_t AccessibilityElementInfo::GetChildCount() const
{
    HILOG_DEBUG("childCount_[%{public}d]", childCount_);
    return childCount_;
}

const std::vector<int32_t> &AccessibilityElementInfo::GetChildIds() const
{
    HILOG_DEBUG("childCount_[%{public}d]", childCount_);
    return childNodeIds_;
}

void AccessibilityElementInfo::AddChild(const int32_t childId)
{
    HILOG_DEBUG("childId[%{public}d]", childId);
    for (int32_t i = 0; i < childCount_; i++) {
        if (childNodeIds_[i] == childId) {
            HILOG_ERROR("childId[%{public}d] is exited", childId);
            return;
        }
    }
    childCount_++;
    childNodeIds_.push_back(childId);
    HILOG_DEBUG("childId[%{public}d] end", childId);
}

bool AccessibilityElementInfo::RemoveChild(const int32_t childId)
{
    HILOG_DEBUG("childId[%{public}d]", childId);
    for (auto iter = childNodeIds_.begin(); iter != childNodeIds_.end(); iter++) {
        if (*iter == childId) {
            iter = childNodeIds_.erase(iter);
            childCount_--;
            return true;
        }
    }
    HILOG_ERROR("Not find childId[%{public}d]", childId);
    return false;
}

const std::vector<AccessibleAction> &AccessibilityElementInfo::GetActionList() const
{
    HILOG_DEBUG();
    return operations_;
}

void AccessibilityElementInfo::AddAction(AccessibleAction &action)
{
    HILOG_DEBUG("actionType[%{public}d]", action.GetActionType());
    operations_.push_back(action);
}

void AccessibilityElementInfo::DeleteAction(AccessibleAction &action)
{
    HILOG_DEBUG();
    for (auto iter = operations_.begin(); iter != operations_.end(); iter++) {
        HILOG_DEBUG("actionType[%{public}d]", action.GetActionType());
        if (iter->GetActionType() == action.GetActionType()) {
            iter = operations_.erase(iter);
            return;
        }
    }
    HILOG_ERROR("Not find actionType[%{public}d]", action.GetActionType());
}

bool AccessibilityElementInfo::DeleteAction(ActionType &actionType)
{
    HILOG_DEBUG();
    for (auto iter = operations_.begin(); iter != operations_.end(); iter++) {
        HILOG_DEBUG("actionType[%{public}d]", actionType);
        if (iter->GetActionType() == actionType) {
            iter = operations_.erase(iter);
            return true;
        }
    }
    HILOG_ERROR("Not find actionType[%{public}d]", actionType);
    return false;
}

void AccessibilityElementInfo::DeleteAllActions()
{
    HILOG_DEBUG();
    operations_.clear();
}

void AccessibilityElementInfo::SetTextLengthLimit(const int32_t max)
{
    textLengthLimit_ = max;
    HILOG_DEBUG("textLengthLimit_[%{public}d]", textLengthLimit_);
}

int32_t AccessibilityElementInfo::GetTextLengthLimit() const
{
    HILOG_DEBUG("textLengthLimit_[%{public}d]", textLengthLimit_);
    return textLengthLimit_;
}

int32_t AccessibilityElementInfo::GetWindowId() const
{
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
    return windowId_;
}

void AccessibilityElementInfo::SetWindowId(const int32_t windowId)
{
    windowId_ = windowId;
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
}

int32_t AccessibilityElementInfo::GetParentNodeId() const
{
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
    return parentId_;
}

void AccessibilityElementInfo::SetParent(const int32_t parentId)
{
    parentId_ = parentId;
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
}

const Rect &AccessibilityElementInfo::GetRectInScreen() const
{
    HILOG_DEBUG("start");
    return bounds_;
}

void AccessibilityElementInfo::SetRectInScreen(Rect &bounds)
{
    HILOG_DEBUG();
    bounds_.SetLeftTopScreenPostion(bounds.GetLeftTopXScreenPostion(), bounds.GetLeftTopYScreenPostion());
    bounds_.SetRightBottomScreenPostion(bounds.GetRightBottomXScreenPostion(), bounds.GetRightBottomYScreenPostion());
}

bool AccessibilityElementInfo::IsCheckable() const
{
    HILOG_DEBUG("checkable_[%{public}d]", checkable_);
    return checkable_;
}

void AccessibilityElementInfo::SetCheckable(const bool checkable)
{
    checkable_ = checkable;
    HILOG_DEBUG("checkable_[%{public}d]", checkable_);
}

bool AccessibilityElementInfo::IsChecked() const
{
    HILOG_DEBUG("checked_[%{public}d]", checked_);
    return checked_;
}

void AccessibilityElementInfo::SetChecked(const bool checked)
{
    checked_ = checked;
    HILOG_DEBUG("checked_[%{public}d]", checked_);
}

bool AccessibilityElementInfo::IsFocusable() const
{
    HILOG_DEBUG("focusable_[%{public}d]", focusable_);
    return focusable_;
}

void AccessibilityElementInfo::SetFocusable(const bool focusable)
{
    focusable_ = focusable;
    HILOG_DEBUG("focusable_[%{public}d]", focusable_);
}

bool AccessibilityElementInfo::IsFocused() const
{
    HILOG_DEBUG("focused_[%{public}d]", focused_);
    return focused_;
}

void AccessibilityElementInfo::SetFocused(const bool focused)
{
    focused_ = focused;
    HILOG_DEBUG("focused_[%{public}d]", focused_);
}

bool AccessibilityElementInfo::IsVisible() const
{
    HILOG_DEBUG("visible_[%{public}d]", visible_);
    return visible_;
}

void AccessibilityElementInfo::SetVisible(const bool visible)
{
    visible_ = visible;
    HILOG_DEBUG("visible_[%{public}d]", visible_);
}

bool AccessibilityElementInfo::HasAccessibilityFocus() const
{
    HILOG_DEBUG("accessibilityFocused_[%{public}d]", accessibilityFocused_);
    return accessibilityFocused_;
}

void AccessibilityElementInfo::SetAccessibilityFocus(const bool focused)
{
    accessibilityFocused_ = focused;
    HILOG_DEBUG("accessibilityFocused_[%{public}d]", accessibilityFocused_);
}

bool AccessibilityElementInfo::IsSelected() const
{
    HILOG_DEBUG("selected_[%{public}d]", selected_);
    return selected_;
}

void AccessibilityElementInfo::SetSelected(const bool selected)
{
    selected_ = selected;
    HILOG_DEBUG("selected_[%{public}d]", selected_);
}

bool AccessibilityElementInfo::IsClickable() const
{
    HILOG_DEBUG("clickable_[%{public}d]", clickable_);
    return clickable_;
}

void AccessibilityElementInfo::SetClickable(const bool clickable)
{
    clickable_ = clickable;
    HILOG_DEBUG("clickable_[%{public}d]", clickable_);
}

bool AccessibilityElementInfo::IsLongClickable() const
{
    HILOG_DEBUG("longClickable_[%{public}d]", longClickable_);
    return longClickable_;
}

void AccessibilityElementInfo::SetLongClickable(const bool longClickable)
{
    longClickable_ = longClickable;
    HILOG_DEBUG("longClickable_[%{public}d]", longClickable_);
}

bool AccessibilityElementInfo::IsEnabled() const
{
    HILOG_DEBUG("enable_[%{public}d]", enable_);
    return enable_;
}

void AccessibilityElementInfo::SetEnabled(const bool enabled)
{
    enable_ = enabled;
    HILOG_DEBUG("enable_[%{public}d]", enable_);
}

bool AccessibilityElementInfo::IsPassword() const
{
    HILOG_DEBUG();
    return isPassword_;
}

void AccessibilityElementInfo::SetPassword(const bool type)
{
    HILOG_DEBUG("type[%{public}d]", type);
    isPassword_ = type;
}

bool AccessibilityElementInfo::IsScrollable() const
{
    HILOG_DEBUG("scrollable_[%{public}d]", scrollable_);
    return scrollable_;
}

void AccessibilityElementInfo::SetScrollable(const bool scrollable)
{
    scrollable_ = scrollable;
    HILOG_DEBUG("scrollable_[%{public}d]", scrollable_);
}

int32_t AccessibilityElementInfo::GetCurrentIndex() const
{
    HILOG_DEBUG("currentIndex_[%{public}d]", currentIndex_);
    return currentIndex_;
}

void AccessibilityElementInfo::SetCurrentIndex(const int32_t index)
{
    currentIndex_ = index;
    HILOG_DEBUG("currentIndex_[%{public}d]", currentIndex_);
}

int32_t AccessibilityElementInfo::GetBeginIndex() const
{
    HILOG_DEBUG("beginIndex_[%{public}d]", beginIndex_);
    return beginIndex_;
}

void AccessibilityElementInfo::SetBeginIndex(const int32_t index)
{
    beginIndex_ = index;
    HILOG_DEBUG("beginIndex_[%{public}d]", beginIndex_);
}

int32_t AccessibilityElementInfo::GetEndIndex() const
{
    HILOG_DEBUG("endIndex_[%{public}d]", endIndex_);
    return endIndex_;
}

void AccessibilityElementInfo::SetEndIndex(const int32_t index)
{
    endIndex_ = index;
    HILOG_DEBUG("endIndex_[%{public}d]", endIndex_);
}

int32_t AccessibilityElementInfo::GetInputType() const
{
    HILOG_DEBUG("inputType_[%{public}d]", inputType_);
    return inputType_;
}

void AccessibilityElementInfo::SetInputType(const int32_t inputType)
{
    inputType_ = inputType;
    HILOG_DEBUG("inputType_[%{public}d]", inputType_);
}

void AccessibilityElementInfo::SetValidElement(const bool valid)
{
    HILOG_DEBUG("validElevalidment_[%{public}d]", valid);
    validElement_ = valid;
}

void AccessibilityElementInfo::SetInspectorKey(const std::string &key)
{
    HILOG_DEBUG("inspector key [%{public}s]", key.c_str());
    inspectorKey_ = key;
}

const std::string &AccessibilityElementInfo::GetInspectorKey() const
{
    HILOG_DEBUG("inspector key [%{public}s]", inspectorKey_.c_str());
    return inspectorKey_;
}

void AccessibilityElementInfo::SetPagePath(const std::string &path)
{
    HILOG_DEBUG("pagePath_[%{public}s]", path.c_str());
    pagePath_ = path;
}

const std::string &AccessibilityElementInfo::GetPagePath() const
{
    HILOG_DEBUG("pagePath_[%{public}s]", pagePath_.c_str());
    return pagePath_;
}

bool AccessibilityElementInfo::IsValidElement() const
{
    HILOG_DEBUG("validElement_[%{public}d]", validElement_);
    return validElement_;
}

bool AccessibilityElementInfo::IsEditable() const
{
    HILOG_DEBUG("editable_[%{public}d]", editable_);
    return editable_;
}

void AccessibilityElementInfo::SetEditable(const bool editable)
{
    editable_ = editable;
    HILOG_DEBUG("editable_[%{public}d]", editable_);
}

bool AccessibilityElementInfo::IsPluraLineSupported() const
{
    HILOG_DEBUG("multiLine_[%{public}d]", multiLine_);
    return multiLine_;
}

void AccessibilityElementInfo::SetPluraLineSupported(const bool multiLine)
{
    multiLine_ = multiLine;
    HILOG_DEBUG("multiLine_[%{public}d]", multiLine_);
}

bool AccessibilityElementInfo::IsPopupSupported() const
{
    HILOG_DEBUG("popupSupported_[%{public}d]", popupSupported_);
    return popupSupported_;
}

void AccessibilityElementInfo::SetPopupSupported(const bool supportPopup)
{
    popupSupported_ = supportPopup;
    HILOG_DEBUG("popupSupported_[%{public}d]", popupSupported_);
}

bool AccessibilityElementInfo::IsDeletable() const
{
    HILOG_DEBUG("deletable_[%{public}d]", deletable_);
    return deletable_;
}

void AccessibilityElementInfo::SetDeletable(const bool deletable)
{
    deletable_ = deletable;
    HILOG_DEBUG("deletable_[%{public}d]", deletable_);
}

bool AccessibilityElementInfo::IsEssential() const
{
    HILOG_DEBUG("isEssential_[%{public}d]", isEssential_);
    return isEssential_;
}

void AccessibilityElementInfo::SetEssential(const bool essential)
{
    isEssential_ = essential;
    HILOG_DEBUG("isEssential_[%{public}d]", isEssential_);
}

bool AccessibilityElementInfo::IsGivingHint() const
{
    HILOG_DEBUG("hint_[%{public}d]", hint_);
    return hint_;
}
void AccessibilityElementInfo::SetHinting(const bool hinting)
{
    hint_ = hinting;
    HILOG_DEBUG("hint_[%{public}d]", hint_);
}

const std::string &AccessibilityElementInfo::GetBundleName() const
{
    HILOG_DEBUG("bundleName_[%{public}s]", bundleName_.c_str());
    return bundleName_;
}

void AccessibilityElementInfo::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
    HILOG_DEBUG("bundleName_[%{public}s]", bundleName_.c_str());
}

const std::string &AccessibilityElementInfo::GetComponentType() const
{
    HILOG_DEBUG("componentType_[%{public}s]", componentType_.c_str());
    return componentType_;
}

void AccessibilityElementInfo::SetComponentType(const std::string &className)
{
    componentType_ = className;
    HILOG_DEBUG("componentType_[%{public}s]", componentType_.c_str());
}

const std::string &AccessibilityElementInfo::GetContent() const
{
    HILOG_DEBUG("text_[%{public}s]", text_.c_str());
    return text_;
}

void AccessibilityElementInfo::SetContent(const std::string &text)
{
    text_ = text;
    HILOG_DEBUG("text_[%{public}s]", text_.c_str());
}

void AccessibilityElementInfo::SetSelectedBegin(const int32_t start)
{
    beginSelected_ = start;
    HILOG_DEBUG("beginSelected_[%{public}d]", beginSelected_);
}

int32_t AccessibilityElementInfo::GetSelectedBegin() const
{
    HILOG_DEBUG("beginSelected_[%{public}d]", beginSelected_);
    return beginSelected_;
}

void AccessibilityElementInfo::SetSelectedEnd(const int32_t end)
{
    endSelected_ = end;
    HILOG_DEBUG("endSelected_[%{public}d]", endSelected_);
}

int32_t AccessibilityElementInfo::GetSelectedEnd() const
{
    HILOG_DEBUG("endSelected_[%{public}d]", endSelected_);
    return endSelected_;
}

const std::string &AccessibilityElementInfo::GetHint() const
{
    HILOG_DEBUG("hintText_[%{public}s]", hintText_.c_str());
    return hintText_;
}

void AccessibilityElementInfo::SetHint(const std::string &hintText)
{
    hintText_ = hintText;
    HILOG_DEBUG("hintText_[%{public}s]", hintText_.c_str());
}

const std::string &AccessibilityElementInfo::GetDescriptionInfo() const
{
    HILOG_DEBUG("contentDescription_[%{public}s]", contentDescription_.c_str());
    return contentDescription_;
}

void AccessibilityElementInfo::SetDescriptionInfo(const std::string &contentDescription)
{
    contentDescription_ = contentDescription;
    HILOG_DEBUG("contentDescription_[%{public}s]", contentDescription_.c_str());
}

void AccessibilityElementInfo::SetComponentResourceId(const std::string &viewIdResName)
{
    resourceName_ = viewIdResName;
    HILOG_DEBUG("resourceName_[%{public}s]", resourceName_.c_str());
}

const std::string &AccessibilityElementInfo::GetComponentResourceId() const
{
    HILOG_DEBUG("resourceName_[%{public}s]", resourceName_.c_str());
    return resourceName_;
}

void AccessibilityElementInfo::SetLiveRegion(const int32_t liveRegion)
{
    liveRegion_ = liveRegion;
    HILOG_DEBUG("liveRegion_[%{public}d]", liveRegion_);
}

int32_t AccessibilityElementInfo::GetLiveRegion() const
{
    HILOG_DEBUG("liveRegion_[%{public}d]", liveRegion_);
    return liveRegion_;
}

void AccessibilityElementInfo::SetContentInvalid(const bool contentInvalid)
{
    contentInvalid_ = contentInvalid;
    HILOG_DEBUG("contentInvalid_[%{public}d]", contentInvalid_);
}

bool AccessibilityElementInfo::GetContentInvalid() const
{
    HILOG_DEBUG("contentInvalid_[%{public}d]", contentInvalid_);
    return contentInvalid_;
}

void AccessibilityElementInfo::SetError(const std::string &error)
{
    error_ = error;
    HILOG_DEBUG("error_[%{public}s]", error_.c_str());
}

const std::string &AccessibilityElementInfo::GetError() const
{
    HILOG_DEBUG("error_[%{public}s]", error_.c_str());
    return error_;
}

void AccessibilityElementInfo::SetLabeled(const int32_t componentId)
{
    labeled_ = componentId;
    HILOG_DEBUG("labeled_[%{public}d]", labeled_);
}

int32_t AccessibilityElementInfo::GetLabeledAccessibilityId() const
{
    HILOG_DEBUG("labeled_[%{public}d]", labeled_);
    return labeled_;
}

void AccessibilityElementInfo::SetAccessibilityId(const int32_t componentId)
{
    elementId_ = componentId;
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
}

int32_t AccessibilityElementInfo::GetAccessibilityId() const
{
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
    return elementId_;
}

const RangeInfo &AccessibilityElementInfo::GetRange() const
{
    HILOG_DEBUG("start");
    return rangeInfo_;
}

void AccessibilityElementInfo::SetRange(RangeInfo &rangeInfo)
{
    HILOG_DEBUG("start");
    rangeInfo_.SetMax(rangeInfo.GetMax());
    rangeInfo_.SetMin(rangeInfo.GetMin());
    rangeInfo_.SetCurrent(rangeInfo.GetCurrent());
}

const GridInfo &AccessibilityElementInfo::GetGrid() const
{
    HILOG_DEBUG("start");
    return grid_;
}

void AccessibilityElementInfo::SetGrid(const GridInfo &grid)
{
    HILOG_DEBUG("start");
    grid_ = grid;
}

const GridItemInfo &AccessibilityElementInfo::GetGridItem() const
{
    HILOG_DEBUG("start");
    return gridItem_;
}

void AccessibilityElementInfo::SetGridItem(const GridItemInfo &gridItem)
{
    HILOG_DEBUG("start");
    gridItem_ = gridItem;
}

AccessibilityElementInfo::AccessibilityElementInfo()
{
    HILOG_DEBUG("start");
}

AccessibleAction::AccessibleAction(ActionType actionType, const std::string &description)
{
    HILOG_DEBUG("start");
    actionType_ = actionType;
    description_ = description;
}

ActionType AccessibleAction::GetActionType() const
{
    HILOG_DEBUG("actionType_[%{public}d]", actionType_);
    return actionType_;
}

const std::string &AccessibleAction::GetDescriptionInfo() const
{
    HILOG_DEBUG("description_[%{public}s]", description_.c_str());
    return description_;
}

RangeInfo::RangeInfo(int32_t min, int32_t max, int32_t current)
{
    HILOG_DEBUG("start");
    min_ = min;
    max_ = max;
    current_ = current;
    HILOG_DEBUG("min_[%{public}d]", min_);
    HILOG_DEBUG("max_[%{public}d]", max_);
    HILOG_DEBUG("current_[%{public}d]", current_);
}

int32_t RangeInfo::GetMin() const
{
    HILOG_DEBUG("min_[%{public}d]", min_);
    return min_;
}

int32_t RangeInfo::GetMax() const
{
    HILOG_DEBUG("max_[%{public}d]", max_);
    return max_;
}

int32_t RangeInfo::GetCurrent() const
{
    HILOG_DEBUG("current_[%{public}d]", current_);
    return current_;
}

void RangeInfo::SetMin(int32_t min)
{
    min_ = min;
    HILOG_DEBUG("min_[%{public}d]", min_);
}

void RangeInfo::SetMax(int32_t max)
{
    max_ = max;
    HILOG_DEBUG("max_[%{public}d]", max_);
}

void RangeInfo::SetCurrent(int32_t current)
{
    current_ = current;
    HILOG_DEBUG("current_[%{public}d]", current_);
}

GridInfo::GridInfo(int32_t rowCount, int32_t columnCount, int32_t mode)
{
    HILOG_DEBUG("start");
    rowCount_ = rowCount;
    columnCount_ = columnCount;
    selectionMode_ = mode;

    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
}

void GridInfo::SetGrid(int32_t rowCount, int32_t columnCount, int32_t mode)
{
    HILOG_DEBUG("start");
    rowCount_ = rowCount;
    columnCount_ = columnCount;
    selectionMode_ = mode;

    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
}

void GridInfo::SetGrid(GridInfo other)
{
    HILOG_DEBUG("start");
    rowCount_ = other.rowCount_;
    columnCount_ = other.columnCount_;
    selectionMode_ = other.selectionMode_;

    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
}

int32_t GridInfo::GetRowCount() const
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    return rowCount_;
}

int32_t GridInfo::GetColumnCount() const
{
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    return columnCount_;
}

int32_t GridInfo::GetSelectionMode() const
{
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
    return selectionMode_;
}

GridItemInfo::GridItemInfo(int32_t rowIndex, int32_t rowSpan, int32_t columnIndex, int32_t columnSpan,
    bool heading, bool selected)
{
    HILOG_DEBUG("start");
    rowIndex_ = rowIndex;
    rowSpan_ = rowSpan;
    columnIndex_ = columnIndex;
    columnSpan_ = columnSpan;
    heading_ = heading;
    selected_ = selected;

    HILOG_DEBUG("rowIndex_[%{public}d]", rowIndex_);
    HILOG_DEBUG("rowSpan_[%{public}d]", rowSpan_);
    HILOG_DEBUG("columnIndex_[%{public}d]", columnIndex_);
    HILOG_DEBUG("columnSpan_[%{public}d]", columnSpan_);
    HILOG_DEBUG("heading_[%{public}d]", heading_);
    HILOG_DEBUG("selected_[%{public}d]", selected_);
}

void GridItemInfo::SetGridItemInfo(GridItemInfo other)
{
    HILOG_DEBUG("start");
    rowIndex_ = other.rowIndex_;
    rowSpan_ = other.rowSpan_;
    columnIndex_ = other.columnIndex_;
    columnSpan_ = other.columnSpan_;
    heading_ = other.heading_;
    selected_ = other.selected_;

    HILOG_DEBUG("rowIndex_[%{public}d]", rowIndex_);
    HILOG_DEBUG("rowSpan_[%{public}d]", rowSpan_);
    HILOG_DEBUG("columnIndex_[%{public}d]", columnIndex_);
    HILOG_DEBUG("columnSpan_[%{public}d]", columnSpan_);
    HILOG_DEBUG("heading_[%{public}d]", heading_);
    HILOG_DEBUG("selected_[%{public}d]", selected_);
}

void GridItemInfo::SetGridItemInfo(int32_t rowIndex, int32_t rowSpan, int32_t columnIndex,
    int32_t columnSpan, bool heading, bool selected)
{
    HILOG_DEBUG("start");
    rowIndex_ = rowIndex;
    rowSpan_ = rowSpan;
    columnIndex_ = columnIndex;
    columnSpan_ = columnSpan;
    heading_ = heading;
    selected_ = selected;

    HILOG_DEBUG("rowIndex_[%{public}d]", rowIndex_);
    HILOG_DEBUG("rowSpan_[%{public}d]", rowSpan_);
    HILOG_DEBUG("columnIndex_[%{public}d]", columnIndex_);
    HILOG_DEBUG("columnSpan_[%{public}d]", columnSpan_);
    HILOG_DEBUG("heading_[%{public}d]", heading_);
    HILOG_DEBUG("selected_[%{public}d]", selected_);
}

int32_t GridItemInfo::GetColumnIndex() const
{
    HILOG_DEBUG("columnIndex_[%{public}d]", columnIndex_);
    return columnIndex_;
}

int32_t GridItemInfo::GetRowIndex() const
{
    HILOG_DEBUG("rowIndex_[%{public}d]", rowIndex_);
    return rowIndex_;
}

int32_t GridItemInfo::GetColumnSpan() const
{
    HILOG_DEBUG("columnSpan_[%{public}d]", columnSpan_);
    return columnSpan_;
}

int32_t GridItemInfo::GetRowSpan() const
{
    HILOG_DEBUG("rowSpan_[%{public}d]", rowSpan_);
    return rowSpan_;
}

bool GridItemInfo::IsHeading() const
{
    HILOG_DEBUG("heading_[%{public}d]", heading_);
    return heading_;
}

bool GridItemInfo::IsSelected() const
{
    HILOG_DEBUG("selected_[%{public}d]", selected_);
    return selected_;
}

int32_t AccessibilityElementInfo::GetPageId() const
{
    HILOG_DEBUG("pageId_[%{public}d]", pageId_);
    return pageId_;
}

void AccessibilityElementInfo::SetPageId(const int32_t pageId)
{
    pageId_ = pageId;
    HILOG_DEBUG("pageId_[%{public}d]", pageId_);
}

void AccessibilityElementInfo::SetTextMovementStep(const TextMoveUnit granularity)
{
    textMoveStep_ = granularity;
    HILOG_DEBUG("textMoveStep_[%{public}d]", textMoveStep_);
}

TextMoveUnit AccessibilityElementInfo::GetTextMovementStep() const
{
    HILOG_DEBUG("textMoveStep_[%{public}d]", textMoveStep_);
    return textMoveStep_;
}

void AccessibilityElementInfo::SetItemCounts(const int32_t itemCounts)
{
    itemCounts_ = itemCounts;
    HILOG_DEBUG("itemCounts_[%{public}d]", itemCounts_);
}

int32_t AccessibilityElementInfo::GetItemCounts() const
{
    HILOG_DEBUG("itemCounts_[%{public}d]", itemCounts_);
    return itemCounts_;
}

void AccessibilityElementInfo::SetTriggerAction(const ActionType action)
{
    HILOG_DEBUG("action[%{public}d]", action);
    triggerAction_ = action;
}

ActionType AccessibilityElementInfo::GetTriggerAction() const
{
    return triggerAction_;
}

void AccessibilityElementInfo::SetContentList(const std::vector<std::string> &contentList)
{
    contentList_.clear();
    contentList_.resize(contentList.size());
    std::copy(contentList.begin(), contentList.end(), contentList_.begin());
}

void AccessibilityElementInfo::GetContentList(std::vector<std::string> &contentList) const
{
    contentList.clear();
    contentList.resize(contentList_.size());
    std::copy(contentList_.begin(), contentList_.end(), contentList.begin());
}

void AccessibilityElementInfo::SetLatestContent(const std::string &content)
{
    latestContent_ = content;
}

const std::string &AccessibilityElementInfo::GetLatestContent() const
{
    return latestContent_;
}
} // namespace Accessibility
} // namespace OHOS