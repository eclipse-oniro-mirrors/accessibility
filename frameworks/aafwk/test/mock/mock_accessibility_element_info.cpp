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

#include <gtest/gtest.h>
#include "accessibility_element_info.h"
#include "accessibility_operator.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
bool AccessibilityElementInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool AccessibilityElementInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<AccessibilityElementInfo> AccessibilityElementInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

void AccessibilityElementInfo::SetComponentId(const int componentId)
{
    (void)componentId;
}

bool AccessibilityElementInfo::GetFocus(const int focus, AccessibilityElementInfo &elementInfo)
{
    (void)focus;
    (void)elementInfo;
    return true;
}

bool AccessibilityElementInfo::GetNext(const FocusMoveDirection direction, AccessibilityElementInfo &elementInfo)
{
    (void)direction;
    (void)elementInfo;
    return true;
}

int AccessibilityElementInfo::GetChildId(const int index) const
{
    return childNodeIds_[index];
}
int AccessibilityElementInfo::GetChildCount() const
{
    return childCount_;
}

bool AccessibilityElementInfo::GetChild(const int index, AccessibilityElementInfo &elementInfo)
{
    (void)index;
    (void)elementInfo;
    return true;
}

std::vector<int> AccessibilityElementInfo::GetChildIds() const
{
    return childNodeIds_;
}

void AccessibilityElementInfo::AddChild(const int childId)
{
    (void)childId;
}

bool AccessibilityElementInfo::RemoveChild(const int childId)
{
    (void)childId;
    return true;
}

std::vector<AccessibleAction> AccessibilityElementInfo::GetActionList() const
{
    return operations_;
}

void AccessibilityElementInfo::AddAction(AccessibleAction &operation)
{
    (void)operation;
}

void AccessibilityElementInfo::DeleteAction(AccessibleAction &operation)
{
    (void)operation;
}

bool AccessibilityElementInfo::DeleteAction(ActionType &operationType)
{
    (void)operationType;
    return true;
}

void AccessibilityElementInfo::DeleteAllActions()
{}

void AccessibilityElementInfo::SetTextLengthLimit(const int max)
{
    (void)max;
}

int AccessibilityElementInfo::GetTextLengthLimit() const
{
    return textLengthLimit_;
}

bool AccessibilityElementInfo::ExecuteAction(const ActionType &operation,
    const std::map<std::string, std::string> &actionArguments)
{
    (void)operation;
    (void)actionArguments;
    return true;
}

bool AccessibilityElementInfo::GetByContent(const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    (void)text;
    (void)elementInfos;
    return true;
}

bool AccessibilityElementInfo::GetElementInfosById(const int elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    (void)elementId;
    (void)mode;
    (void)elementInfos;
    return true;
}

int AccessibilityElementInfo::GetWindowId() const
{
    return windowId_;
}

void AccessibilityElementInfo::SetWindowId(const int windowId)
{
    (void)windowId;
}

bool AccessibilityElementInfo::GetParent(AccessibilityElementInfo &elementInfo)
{
    (void)elementInfo;
    return true;
}

int AccessibilityElementInfo::GetParentNodeId() const
{
    return parentId_;
}

void AccessibilityElementInfo::SetParent(const int parentId)
{
    (void)parentId;
}

Rect AccessibilityElementInfo::GetRectInScreen() const
{
    return bounds_;
}

void AccessibilityElementInfo::SetRectInScreen(Rect &bounds)
{
    (void)bounds;
}

bool AccessibilityElementInfo::IsCheckable() const
{
    return checkable_;
}

void AccessibilityElementInfo::SetCheckable(const bool checkable)
{
    (void)checkable;
}

bool AccessibilityElementInfo::IsChecked() const
{
    return checked_;
}

void AccessibilityElementInfo::SetChecked(const bool checked)
{
    (void)checked;
}

bool AccessibilityElementInfo::IsFocusable() const
{
    return focusable_;
}

void AccessibilityElementInfo::SetFocusable(const bool focusable)
{
    (void)focusable;
}

bool AccessibilityElementInfo::IsFocused() const
{
    return focused_;
}

void AccessibilityElementInfo::SetFocused(const bool focused)
{
    (void)focused;
}

bool AccessibilityElementInfo::IsVisible() const
{
    return visible_;
}

void AccessibilityElementInfo::SetVisible(const bool visible)
{
    (void)visible;
}

bool AccessibilityElementInfo::HasAccessibilityFocus() const
{
    return accessibilityFocused_;
}

void AccessibilityElementInfo::SetAccessibilityFocus(const bool focused)
{
    (void)focused;
}

bool AccessibilityElementInfo::IsSelected() const
{
    return selected_;
}

void AccessibilityElementInfo::SetSelected(const bool selected)
{
    (void)selected;
}

bool AccessibilityElementInfo::IsClickable() const
{
    return clickable_;
}

void AccessibilityElementInfo::SetClickable(const bool clickable)
{
    (void)clickable;
}

bool AccessibilityElementInfo::IsLongClickable() const
{
    return longClickable_;
}

void AccessibilityElementInfo::SetLongClickable(const bool longClickable)
{
    (void)longClickable;
}

bool AccessibilityElementInfo::IsEnabled() const
{
    return enable_;
}

void AccessibilityElementInfo::SetEnabled(const bool enabled)
{
    (void)enabled;
}

bool AccessibilityElementInfo::IsPassword() const
{
    return isPassword_;
}

void AccessibilityElementInfo::SetPassword(const bool type)
{
    (void)type;
}

bool AccessibilityElementInfo::IsScrollable() const
{
    return scrollable_;
}

void AccessibilityElementInfo::SetScrollable(const bool scrollable)
{
    (void)scrollable;
}

int AccessibilityElementInfo::GetCurrentIndex() const
{
    return currentIndex_;
}

void AccessibilityElementInfo::SetCurrentIndex(const int index)
{
    (void)index;
}

int AccessibilityElementInfo::GetBeginIndex() const
{
    return beginIndex_;
}

void AccessibilityElementInfo::SetBeginIndex(const int index)
{
    (void)index;
}

int AccessibilityElementInfo::GetEndIndex() const
{
    return endIndex_;
}

void AccessibilityElementInfo::SetEndIndex(const int index)
{
    (void)index;
}

int AccessibilityElementInfo::GetInputType() const
{
    return inputType_;
}

void AccessibilityElementInfo::SetInputType(const int inputType)
{
    (void)inputType;
}

bool AccessibilityElementInfo::IsEditable() const
{
    return editable_;
}

void AccessibilityElementInfo::SetEditable(const bool editable)
{
    (void)editable;
}

bool AccessibilityElementInfo::IsPluraLineSupported() const
{
    return multiLine_;
}

void AccessibilityElementInfo::SetPluraLineSupported(const bool multiLine)
{
    (void)multiLine;
}

bool AccessibilityElementInfo::IsPopupSupported() const
{
    return popupSupported_;
}

void AccessibilityElementInfo::SetPopupSupported(const bool supportPopup)
{
    (void)supportPopup;
}

bool AccessibilityElementInfo::IsDeletable() const
{
    return deletable_;
}

void AccessibilityElementInfo::SetDeletable(const bool deletable)
{
    (void)deletable;
}

bool AccessibilityElementInfo::IsEssential() const
{
    return isEssential_;
}

void AccessibilityElementInfo::SetEssential(const bool important)
{
    (void)important;
}

bool AccessibilityElementInfo::IsGivingHint() const
{
    return hint_;
}

void AccessibilityElementInfo::SetHinting(const bool hinting)
{
    (void)hinting;
}

std::string AccessibilityElementInfo::GetBundleName() const
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityElementInfo GetBundleName";
    return bundleName_;
}

void AccessibilityElementInfo::SetBundleName(const std::string &bundleName)
{
    GTEST_LOG_(INFO) << "MOCK AccessibilityElementInfo SetBundleName";
    bundleName_ = bundleName;
}

std::string AccessibilityElementInfo::GetComponentType() const
{
    return componentType_;
}

void AccessibilityElementInfo::SetComponentType(const std::string &className)
{
    (void)className;
}

std::string AccessibilityElementInfo::GetContent() const
{
    return text_;
}

void AccessibilityElementInfo::SetContent(const std::string &text)
{
    (void)text;
}

std::string AccessibilityElementInfo::GetAccessibilityContent() const
{
    return accessibilityText_;
}

void AccessibilityElementInfo::SetAccessibilityContent(const std::string &text)
{
    (void)text;
}

void AccessibilityElementInfo::SetSelectedBegin(const int start)
{
    (void)start;
}

int AccessibilityElementInfo::GetSelectedBegin() const
{
    return beginSelected_;
}

void AccessibilityElementInfo::SetSelectedEnd(const int end)
{
    (void)end;
}

int AccessibilityElementInfo::GetSelectedEnd() const
{
    return endSelected_;
}

std::string AccessibilityElementInfo::GetAccessibilityDescription() const
{
    return accessibilityDescription_;
}

void AccessibilityElementInfo::SetAccessibilityDescription(const std::string &text)
{
    (void)text;
}

bool AccessibilityElementInfo::GetAccessibilityGroup() const
{
    return accessibilityGroup_;
}

void AccessibilityElementInfo::SetAccessibilityGroup(const bool group)
{
    (void)group;
}

std::string AccessibilityElementInfo::GetHint() const
{
    return hintText_;
}

void AccessibilityElementInfo::SetHint(const std::string &hintText)
{
    (void)hintText;
}

std::string AccessibilityElementInfo::GetDescriptionInfo() const
{
    return contentDescription_;
}

void AccessibilityElementInfo::SetDescriptionInfo(const std::string contentDescription)
{
    (void)contentDescription;
}

void AccessibilityElementInfo::SetComponentResourceId(const std::string &viewIdResName)
{
    (void)viewIdResName;
}

std::string AccessibilityElementInfo::GetComponentResourceId() const
{
    return resourceName_;
}

void AccessibilityElementInfo::SetLiveRegion(const int liveRegion)
{
    (void)liveRegion;
}

int AccessibilityElementInfo::GetLiveRegion() const
{
    return liveRegion_;
}

void AccessibilityElementInfo::SetContentInvalid(const bool contentInvalid)
{
    (void)contentInvalid;
}

bool AccessibilityElementInfo::GetContentInvalid() const
{
    return contentInvalid_;
}

void AccessibilityElementInfo::SetError(const std::string &error)
{
    (void)error;
}

std::string AccessibilityElementInfo::GetError() const
{
    return error_;
}

void AccessibilityElementInfo::SetLabeled(const int componentId)
{
    (void)componentId;
}

bool AccessibilityElementInfo::GetLabeled(AccessibilityElementInfo &elementInfo) const
{
    (void)elementInfo;
    return true;
}

int AccessibilityElementInfo::GetLabeledAccessibilityId() const
{
    return labeled_;
}

int AccessibilityElementInfo::GetChannelId() const
{
    return channelId_;
}

void AccessibilityElementInfo::SetChannelId(const int channelId)
{
    (void)channelId;
}

void AccessibilityElementInfo::SetAccessibilityId(const int componentId)
{
    (void)componentId;
}

int AccessibilityElementInfo::GetAccessibilityId() const
{
    return elementId_;
}

RangeInfo AccessibilityElementInfo::GetRange() const
{
    return rangeInfo_;
}

void AccessibilityElementInfo::SetRange(RangeInfo &rangeInfo)
{
    (void)rangeInfo;
}

GridInfo AccessibilityElementInfo::GetGrid() const
{
    return grid_;
}

void AccessibilityElementInfo::SetGrid(const GridInfo &grid)
{
    (void)grid;
}

GridItemInfo AccessibilityElementInfo::GetGridItem() const
{
    return gridItem_;
}

void AccessibilityElementInfo::SetGridItem(const GridItemInfo &gridItem)
{
    (void)gridItem;
}

AccessibilityElementInfo::AccessibilityElementInfo()
{}

bool AccessibleAction::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool AccessibleAction::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<AccessibleAction> AccessibleAction::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

AccessibleAction::AccessibleAction(ActionType operationType, std::string description)
{
    (void)operationType;
    (void)description;
}

ActionType AccessibleAction::GetActionType() const
{
    return actionType_;
}

std::string AccessibleAction::GetDescriptionInfo() const
{
    return description_;
}

bool RangeInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool RangeInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<RangeInfo> RangeInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

RangeInfo::RangeInfo(int min, int max, int current)
{
    (void)min;
    (void)max;
    (void)current;
}

int RangeInfo::GetMin() const
{
    return min_;
}

int RangeInfo::GetMax() const
{
    return max_;
}

int RangeInfo::GetCurrent() const
{
    return current_;
}

void RangeInfo::SetMin(int min)
{
    (void)min;
}

void RangeInfo::SetMax(int max)
{
    (void)max;
}

void RangeInfo::SetCurrent(int current)
{
    (void)current;
}

bool GridInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool GridInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<GridInfo> GridInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

GridInfo::GridInfo(int rowCount, int columnCount, int mode)
{
    (void)rowCount;
    (void)columnCount;
    (void)mode;
}

void GridInfo::SetGrid(int rowCount, int columnCount, int mode)
{
    (void)rowCount;
    (void)columnCount;
    (void)mode;
}

void GridInfo::SetGrid(GridInfo other)
{
    (void)other;
}

int GridInfo::GetRowCount() const
{
    return rowCount_;
}

int GridInfo::GetColumnCount() const
{
    return columnCount_;
}

int GridInfo::GetSelectionMode() const
{
    return selectionMode_;
}

bool GridItemInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool GridItemInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<GridItemInfo> GridItemInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

GridItemInfo::GridItemInfo(int rowIndex, int rowSpan, int columnIndex, int columnSpan, bool heading, bool selected)
{
    (void)rowIndex;
    (void)rowSpan;
    (void)columnIndex;
    (void)columnSpan;
    (void)heading;
    (void)selected;
}

void GridItemInfo::SetGridItemInfo(GridItemInfo other)
{
    (void)other;
}

void GridItemInfo::SetGridItemInfo(int rowIndex, int rowSpan,
    int columnIndex, int columnSpan, bool heading, bool selected)
{
    (void)rowIndex;
    (void)rowSpan;
    (void)columnIndex;
    (void)columnSpan;
    (void)heading;
    (void)selected;
}

int GridItemInfo::GetColumnIndex() const
{
    return columnIndex_;
}

int GridItemInfo::GetRowIndex() const
{
    return rowIndex_;
}

int GridItemInfo::GetColumnSpan() const
{
    return columnSpan_;
}

int GridItemInfo::GetRowSpan() const
{
    return rowSpan_;
}

bool GridItemInfo::IsHeading() const
{
    return heading_;
}

bool GridItemInfo::IsSelected() const
{
    return selected_;
}

bool Rect::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool Rect::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

sptr<Rect> Rect::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    sptr<Rect> rect = new Rect();
    return rect;
}
} // namespace Accessibility
} // namespace OHOS