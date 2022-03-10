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
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
bool AccessibilityElementInfo::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool AccessibilityElementInfo::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<AccessibilityElementInfo> AccessibilityElementInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

void AccessibilityElementInfo::SetComponentId(const int componentId)
{}

bool AccessibilityElementInfo::GetFocus(const int focus, AccessibilityElementInfo &elementInfo)
{
    return true;
}

bool AccessibilityElementInfo::GetNext(const FocusMoveDirection direction, AccessibilityElementInfo &elementInfo)
{
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
    return true;
}

std::vector<int> AccessibilityElementInfo::GetChildIds() const
{
    return childNodeIds_;
}

void AccessibilityElementInfo::AddChild(const int childId)
{}

bool AccessibilityElementInfo::RemoveChild(const int childId)
{
    return true;
}

std::vector<AccessibleAction> AccessibilityElementInfo::GetActionList() const
{
    return operations_;
}

void AccessibilityElementInfo::AddAction(AccessibleAction &operation)
{}

void AccessibilityElementInfo::DeleteAction(AccessibleAction &operation)
{}

bool AccessibilityElementInfo::DeleteAction(ActionType &operationType)
{
    return true;
}

void AccessibilityElementInfo::DeleteAllActions()
{}

void AccessibilityElementInfo::SetTextLengthLimit(const int max)
{}

int AccessibilityElementInfo::GetTextLengthLimit() const
{
    return textLengthLimit_;
}

bool AccessibilityElementInfo::ExecuteAction(const ActionType &operation,
    const std::map<std::string, std::string> &actionArguments)
{
    return false;
}

bool AccessibilityElementInfo::GetByContent(const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    return true;
}

bool AccessibilityElementInfo::GetElementInfosById(const int elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfo)
{
    return true;
}

int AccessibilityElementInfo::GetWindowId() const
{
    return windowId_;
}

void AccessibilityElementInfo::SetWindowId(const int windowId)
{}

bool AccessibilityElementInfo::GetParent(AccessibilityElementInfo &elementInfo)
{
    return true;
}

int AccessibilityElementInfo::GetParentNodeId() const
{
    return parentId_;
}

void AccessibilityElementInfo::SetParent(const int parentId)
{}

Rect AccessibilityElementInfo::GetRectInScreen() const
{
    return bounds_;
}

void AccessibilityElementInfo::SetRectInScreen(Rect &bounds)
{
    bounds_.SetLeftTopScreenPostion(bounds.GetLeftTopXScreenPostion(), bounds.GetLeftTopYScreenPostion());
    bounds_.SetRightBottomScreenPostion(bounds.GetRightBottomXScreenPostion(), bounds.GetRightBottomYScreenPostion());
}

bool AccessibilityElementInfo::IsCheckable() const
{
    return checkable_;
}

void AccessibilityElementInfo::SetCheckable(const bool checkable)
{}

bool AccessibilityElementInfo::IsChecked() const
{
    return checked_;
}

void AccessibilityElementInfo::SetChecked(const bool checked)
{}

bool AccessibilityElementInfo::IsFocusable() const
{
    return focusable_;
}

void AccessibilityElementInfo::SetFocusable(const bool focusable)
{}

bool AccessibilityElementInfo::IsFocused() const
{
    return focused_;
}

void AccessibilityElementInfo::SetFocused(const bool focused)
{}

bool AccessibilityElementInfo::IsVisible() const
{
    return visible_;
}

void AccessibilityElementInfo::SetVisible(const bool visible)
{}

bool AccessibilityElementInfo::HasAccessibilityFocus() const
{
    return accessibilityFocused_;
}

void AccessibilityElementInfo::SetAccessibilityFocus(const bool focused)
{}

bool AccessibilityElementInfo::IsSelected() const
{
    return selected_;
}

void AccessibilityElementInfo::SetSelected(const bool selected)
{}

bool AccessibilityElementInfo::IsClickable() const
{
    return clickable_;
}

void AccessibilityElementInfo::SetClickable(const bool clickable)
{}

bool AccessibilityElementInfo::IsLongClickable() const
{
    return longClickable_;
}

void AccessibilityElementInfo::SetLongClickable(const bool longClickable)
{}

bool AccessibilityElementInfo::IsEnabled() const
{
    return enable_;
}

void AccessibilityElementInfo::SetEnabled(const bool enabled)
{}

bool AccessibilityElementInfo::IsPassword() const
{
    return isPassword_;
}

void AccessibilityElementInfo::SetPassword(const bool type)
{}

bool AccessibilityElementInfo::IsScrollable() const
{
    return scrollable_;
}

void AccessibilityElementInfo::SetScrollable(const bool scrollable)
{}

int AccessibilityElementInfo::GetCurrentIndex() const
{
    return currentIndex_;
}

void AccessibilityElementInfo::SetCurrentIndex(const int index)
{}

int AccessibilityElementInfo::GetBeginIndex() const
{
    return beginIndex_;
}

void AccessibilityElementInfo::SetBeginIndex(const int index)
{}

int AccessibilityElementInfo::GetEndIndex() const
{
    return endIndex_;
}

void AccessibilityElementInfo::SetEndIndex(const int index)
{}

int AccessibilityElementInfo::GetInputType() const
{
    return inputType_;
}

void AccessibilityElementInfo::SetInputType(const int inputType)
{}

bool AccessibilityElementInfo::IsEditable() const
{
    return editable_;
}

void AccessibilityElementInfo::SetEditable(const bool editable)
{}

bool AccessibilityElementInfo::IsPluraLineSupported() const
{
    return multiLine_;
}

void AccessibilityElementInfo::SetPluraLineSupported(const bool multiLine)
{}

bool AccessibilityElementInfo::IsPopupSupported() const
{
    return popupSupported_;
}

void AccessibilityElementInfo::SetPopupSupported(const bool supportPopup)
{}

bool AccessibilityElementInfo::IsDeletable() const
{
    return deletable_;
}

void AccessibilityElementInfo::SetDeletable(const bool deletable)
{}

bool AccessibilityElementInfo::IsEssential() const
{
    return isEssential_;
}

void AccessibilityElementInfo::SetEssential(const bool important)
{}

bool AccessibilityElementInfo::IsGivingHint() const
{
    return hint_;
}

void AccessibilityElementInfo::SetHinting(const bool hinting)
{}

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
{}

std::string AccessibilityElementInfo::GetContent() const
{
    return text_;
}

void AccessibilityElementInfo::SetContent(const std::string &text)
{}

std::string AccessibilityElementInfo::GetAccessibilityContent() const
{
    return accessibilityText_;
}

void AccessibilityElementInfo::SetAccessibilityContent(const std::string &text)
{}

void AccessibilityElementInfo::SetSelectedBegin(const int start)
{}

int AccessibilityElementInfo::GetSelectedBegin() const
{
    return beginSelected_;
}

void AccessibilityElementInfo::SetSelectedEnd(const int end)
{}

int AccessibilityElementInfo::GetSelectedEnd() const
{
    return endSelected_;
}

std::string AccessibilityElementInfo::GetAccessibilityDescription() const
{
    return accessibilityDescription_;
}

void AccessibilityElementInfo::SetAccessibilityDescription(const std::string &text)
{}

bool AccessibilityElementInfo::GetAccessibilityGroup() const
{
    return accessibilityGroup_;
}

void AccessibilityElementInfo::SetAccessibilityGroup(const bool group)
{}

std::string AccessibilityElementInfo::GetHint() const
{
    return hintText_;
}

void AccessibilityElementInfo::SetHint(const std::string &hintText)
{}

std::string AccessibilityElementInfo::GetDescriptionInfo() const
{
    return contentDescription_;
}

void AccessibilityElementInfo::SetDescriptionInfo(const std::string contentDescription)
{}

void AccessibilityElementInfo::SetComponentResourceId(const std::string &viewIdResName)
{}

std::string AccessibilityElementInfo::GetComponentResourceId() const
{
    return resourceName_;
}

void AccessibilityElementInfo::SetLiveRegion(const int liveRegion)
{}

int AccessibilityElementInfo::GetLiveRegion() const
{
    return liveRegion_;
}

void AccessibilityElementInfo::SetContentInvalid(const bool contentInvalid)
{}

bool AccessibilityElementInfo::GetContentInvalid() const
{
    return contentInvalid_;
}

void AccessibilityElementInfo::SetError(const std::string &error)
{}

std::string AccessibilityElementInfo::GetError() const
{
    return error_;
}

void AccessibilityElementInfo::SetLabeled(const int componentId)
{}

bool AccessibilityElementInfo::GetLabeled(AccessibilityElementInfo &elementInfo) const
{
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
{}

void AccessibilityElementInfo::SetAccessibilityId(const int componentId)
{}

int AccessibilityElementInfo::GetAccessibilityId() const
{
    return elementId_;
}

RangeInfo AccessibilityElementInfo::GetRange() const
{
    return rangeInfo_;
}

void AccessibilityElementInfo::SetRange(RangeInfo &rangeInfo)
{}

GridInfo AccessibilityElementInfo::GetGrid() const
{
    return grid_;
}

void AccessibilityElementInfo::SetGrid(const GridInfo &grid)
{}

GridItemInfo AccessibilityElementInfo::GetGridItem() const
{
    return gridItem_;
}

void AccessibilityElementInfo::SetGridItem(const GridItemInfo &gridItem)
{}

AccessibilityElementInfo::AccessibilityElementInfo()
{}

bool AccessibleAction::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool AccessibleAction::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<AccessibleAction> AccessibleAction::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

AccessibleAction::AccessibleAction(ActionType operationType, std::string description)
{}

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
    return true;
}

bool RangeInfo::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<RangeInfo> RangeInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

RangeInfo::RangeInfo(int min, int max, int current)
{}

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
{}

void RangeInfo::SetMax(int max)
{}

void RangeInfo::SetCurrent(int current)
{}

bool GridInfo::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool GridInfo::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<GridInfo> GridInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

GridInfo::GridInfo(int rowCount, int columnCount, int mode)
{}

void GridInfo::SetGrid(int rowCount, int columnCount, int mode)
{}

void GridInfo::SetGrid(GridInfo other)
{}

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
    return true;
}

bool GridItemInfo::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<GridItemInfo> GridItemInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

GridItemInfo::GridItemInfo(int rowIndex, int rowSpan, int columnIndex, int columnSpan,
    bool heading, bool selected)
{}

void GridItemInfo::SetGridItemInfo(GridItemInfo other)
{}

void GridItemInfo::SetGridItemInfo(int rowIndex, int rowSpan,
    int columnIndex, int columnSpan, bool heading, bool selected)
{}

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
    return true;
}

bool Rect::Marshalling(Parcel &parcel) const
{
    return true;
};

sptr<Rect> Rect::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}
} // namespace Accessibility
} // namespace OHOS