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
#include "parcel_util.h"
#include "hilog_wrapper.h"
#include "accessibility_operator.h"

namespace OHOS {
namespace Accessibility {
/* AccessibilityElementInfo       Parcel struct                 */
bool AccessibilityElementInfo::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, elementId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityDescription_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, &childNodeIds_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    int32_t operationsSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operationsSize);
    for (int i = 0; i < operationsSize; i++) {
        sptr<AccessibleAction> accessibleOperation = parcel.ReadStrongParcelable<AccessibleAction>();
        if (!accessibleOperation) {
            HILOG_ERROR("ReadStrongParcelable<accessibleOperation> failed");
            return false;
        }
        operations_.emplace_back(*accessibleOperation);
    }
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, channelId_);
    sptr<Rect> rect = parcel.ReadStrongParcelable<Rect>();
    if (!rect) {
        return false;
    }
    bounds_ = *rect;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityGroup_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checkable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checked_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focusable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, clickable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, longClickable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, password_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, scrollable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, editable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, popupSupported_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, multiLine_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, deletable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, hint_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEssential_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, currentIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endIndex_);
    sptr<RangeInfo> rangeInfo = parcel.ReadStrongParcelable<RangeInfo>();
    if (!rangeInfo) {
        return false;
    }
    rangeInfo_ = *rangeInfo;
    sptr<GridInfo> grid = parcel.ReadStrongParcelable<GridInfo>();
    if (!grid) {
        return false;
    }
    grid_ = *grid;
    sptr<GridItemInfo> gridItem = parcel.ReadStrongParcelable<GridItemInfo>();
    if (!gridItem) {
        return false;
    }
    gridItem_ = *gridItem;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labeled_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);

    return true;
}

bool AccessibilityElementInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, elementId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityDescription_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, childNodeIds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operations_.size());
    for (auto &operations : operations_) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &operations);
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, channelId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &bounds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityGroup_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checkable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checked_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focusable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, clickable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, longClickable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, password_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, scrollable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, editable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, popupSupported_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, multiLine_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, deletable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, hint_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEssential_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, currentIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &rangeInfo_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &grid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &gridItem_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labeled_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);
    return true;
};

sptr<AccessibilityElementInfo> AccessibilityElementInfo::Unmarshalling(Parcel& parcel)
{
    sptr<AccessibilityElementInfo> accessibilityInfo = new AccessibilityElementInfo();
    if (!accessibilityInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return accessibilityInfo;
}

void AccessibilityElementInfo::SetComponentId(const int componentId)
{
    elementId_ = componentId;
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
}

bool AccessibilityElementInfo::GetFocus(const int focus, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], focus[%{public}d]", channelId_, windowId_, elementId_, focus);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result =  instance->FindFocusedElementInfo(channelId_, windowId_, elementId_, focus, elementInfo);
    } else {
        HILOG_ERROR("Can't get AccessibilityOperator instance");
    }
    return result;
}

bool AccessibilityElementInfo::GetNext(const FocusMoveDirection direction, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], direction[%{public}d]", channelId_, windowId_, elementId_, direction);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->FocusMoveSearch(channelId_, windowId_, elementId_, direction, elementInfo);
    } else {
        HILOG_ERROR("Can't get AccessibilityOperator instance");
    }
    return result;
}

int AccessibilityElementInfo::GetChildId(const int index) const
{
    HILOG_DEBUG("index[%{public}d]", index);
    if (index >= childCount_ || index < 0) {
        HILOG_ERROR("index[%{public}d] is invalid", index);
        return 0;
    }
    return childNodeIds_[index];
}
int AccessibilityElementInfo::GetChildCount() const
{
    HILOG_DEBUG("childCount_[%{public}d]", childCount_);
    return childCount_;
}

bool AccessibilityElementInfo::GetChild(const int index, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], index[%{public}d]", channelId_, windowId_, elementId_, index);
    if (index >= childCount_ || index < 0) {
        HILOG_ERROR("index[%{public}d] is invalid", index);
        return false;
    }
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    std::vector<AccessibilityElementInfo> elementInfos {};
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_,
            windowId_, GetChildId(index), 0, elementInfos);
        if (elementInfos.empty()) {
            result = false;
        } else {
            elementInfo = elementInfos.front();
        }
    } else {
        HILOG_ERROR("Can't get AccessibilityOperator instance");
    }
    return result;
}

std::vector<int> AccessibilityElementInfo::GetChildIds() const
{
    HILOG_DEBUG("childCount_[%{public}d]", childCount_);
    return childNodeIds_;
}
void AccessibilityElementInfo::AddChild(const int childId)
{
    HILOG_DEBUG("childId[%{public}d]", childId);
    for (int i = 0; i < childCount_; i++) {
        if (childNodeIds_[i] == childId) {
            HILOG_ERROR("childId[%{public}d] is exited", childId);
            return;
        }
    }
    childCount_++;
    childNodeIds_.push_back(childId);
    HILOG_DEBUG("childId[%{public}d] end", childId);
}

bool AccessibilityElementInfo::RemoveChild(const int childId)
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

std::vector<AccessibleAction> AccessibilityElementInfo::GetActionList() const
{
    HILOG_DEBUG("start");
    return operations_;
}

void AccessibilityElementInfo::AddAction(AccessibleAction &action)
{
    HILOG_DEBUG("start");
    operations_.push_back(action);
    HILOG_DEBUG("actionType[%{public}d] end", action.GetActionType());
}

void AccessibilityElementInfo::DeleteAction(AccessibleAction &action)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
    operations_.clear();
}

void AccessibilityElementInfo::SetTextLengthLimit(const int max)
{
    textLengthLimit_ = max;
    HILOG_DEBUG("textLengthLimit_[%{public}d]", textLengthLimit_);
}

int AccessibilityElementInfo::GetTextLengthLimit() const
{
    HILOG_DEBUG("textLengthLimit_[%{public}d]", textLengthLimit_);
    return textLengthLimit_;
}

bool AccessibilityElementInfo::ExecuteAction(const ActionType &action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_INFO("called] channelId_[%{public}d], windowId_[%{public}d], elementId_[%{public}d]",
        channelId_, windowId_, elementId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    if (instance != nullptr) {
        return instance->ExecuteAction(channelId_, windowId_, elementId_, action,
            const_cast<std::map<std::string, std::string> &>(actionArguments));
    } else {
        HILOG_INFO("Can't get AccessibilityOperator instance");
        return false;
    }
}

bool AccessibilityElementInfo::GetByContent(const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_INFO("called channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], text[%{public}s]", channelId_, windowId_, elementId_, text.c_str());
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByText(channelId_, windowId_, elementId_, text, elementInfos);
    } else {
        HILOG_INFO("Can't get AccessibilityOperator instance");
    }
    return result;
}

bool AccessibilityElementInfo::GetElementInfosById(const int elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("start");
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_,
            windowId_, elementId, mode, elementInfos);
    } else {
        HILOG_INFO("Can't get AccessibilityOperator instance");
    }

    return result;
}

int AccessibilityElementInfo::GetWindowId() const
{
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
    return windowId_;
}

void AccessibilityElementInfo::SetWindowId(const int windowId)
{
    windowId_ = windowId;
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
}

bool AccessibilityElementInfo::GetParent(AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("channelId_[%{public}d], windowId_[%{public}d], parentId_[%{public}d]",
        channelId_, windowId_, parentId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_,
            windowId_, parentId_, 0, elementInfos);
        if (elementInfos.empty()) {
            result = false;
        } else {
            elementInfo = elementInfos.front();
        }
    } else {
        HILOG_INFO("Can't get AccessibilityOperator instance");
    }
    return result;
}

int AccessibilityElementInfo::GetParentNodeId() const
{
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
    return parentId_;
}

void AccessibilityElementInfo::SetParent(const int parentId)
{
    parentId_ = parentId;
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
}
Rect AccessibilityElementInfo::GetRectInScreen() const
{
    HILOG_DEBUG("start");
    return bounds_;
}

void AccessibilityElementInfo::SetRectInScreen(Rect &bounds)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("password_[%{public}d]", password_);
    return password_;
}

void AccessibilityElementInfo::SetPassword(const bool password)
{
    password_ = password;
    HILOG_DEBUG("password_[%{public}d]", password_);
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

int AccessibilityElementInfo::GetCurrentIndex() const
{
    HILOG_DEBUG("currentIndex_[%{public}d]", currentIndex_);
    return currentIndex_;
}

void AccessibilityElementInfo::SetCurrentIndex(const int index)
{
    currentIndex_ = index;
    HILOG_DEBUG("currentIndex_[%{public}d]", currentIndex_);
}

int AccessibilityElementInfo::GetBeginIndex() const
{
    HILOG_DEBUG("beginIndex_[%{public}d]", beginIndex_);
    return beginIndex_;
}

void AccessibilityElementInfo::SetBeginIndex(const int index)
{
    beginIndex_ = index;
    HILOG_DEBUG("beginIndex_[%{public}d]", beginIndex_);
}

int AccessibilityElementInfo::GetEndIndex() const
{
    HILOG_DEBUG("endIndex_[%{public}d]", endIndex_);
    return endIndex_;
}

void AccessibilityElementInfo::SetEndIndex(const int index)
{
    endIndex_ = index;
    HILOG_DEBUG("endIndex_[%{public}d]", endIndex_);
}

int AccessibilityElementInfo::GetInputType() const
{
    HILOG_DEBUG("inputType_[%{public}d]", inputType_);
    return inputType_;
}

void AccessibilityElementInfo::SetInputType(const int inputType)
{
    inputType_ = inputType;
    HILOG_DEBUG("inputType_[%{public}d]", inputType_);
}

void AccessibilityElementInfo::SetValidElement(const bool valid)
{
    HILOG_DEBUG("validElevalidment_[%{public}d]", valid);
    validElement_ = valid;
}

void AccessibilityElementInfo::SetInspectorKey(const std::string key)
{
    HILOG_DEBUG("inspector key [%{public}s]", key.c_str());
    inspectorKey_ = key;
}

std::string AccessibilityElementInfo::GetInspectorKey() const
{
    HILOG_DEBUG("inspector key [%{public}s]", inspectorKey_.c_str());
    return inspectorKey_;
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

std::string AccessibilityElementInfo::GetBundleName() const
{
    HILOG_DEBUG("bundleName_[%{public}s]", bundleName_.c_str());
    return bundleName_;
}

void AccessibilityElementInfo::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
    HILOG_DEBUG("bundleName_[%{public}s]", bundleName_.c_str());
}

std::string AccessibilityElementInfo::GetComponentType() const
{
    HILOG_DEBUG("componentType_[%{public}s]", componentType_.c_str());
    return componentType_;
}

void AccessibilityElementInfo::SetComponentType(const std::string &className)
{
    componentType_ = className;
    HILOG_DEBUG("componentType_[%{public}s]", componentType_.c_str());
}

std::string AccessibilityElementInfo::GetContent() const
{
    HILOG_DEBUG("text_[%{public}s]", text_.c_str());
    return text_;
}

void AccessibilityElementInfo::SetContent(const std::string &text)
{
    text_ = text;
    HILOG_DEBUG("text_[%{public}s]", text_.c_str());
}

std::string AccessibilityElementInfo::GetAccessibilityContent() const
{
    HILOG_DEBUG("accessibilityText_[%{public}s]", accessibilityText_.c_str());
    return accessibilityText_;
}

void AccessibilityElementInfo::SetAccessibilityContent(const std::string &text)
{
    accessibilityText_ = text;
    HILOG_DEBUG("accessibilityText_[%{public}s]", accessibilityText_.c_str());
}

void AccessibilityElementInfo::SetSelectedBegin(const int start)
{
    beginSelected_ = start;
    HILOG_DEBUG("beginSelected_[%{public}d]", beginSelected_);
}

int AccessibilityElementInfo::GetSelectedBegin() const
{
    HILOG_DEBUG("beginSelected_[%{public}d]", beginSelected_);
    return beginSelected_;
}

void AccessibilityElementInfo::SetSelectedEnd(const int end)
{
    endSelected_ = end;
    HILOG_DEBUG("endSelected_[%{public}d]", endSelected_);
}

int AccessibilityElementInfo::GetSelectedEnd() const
{
    HILOG_DEBUG("endSelected_[%{public}d]", endSelected_);
    return endSelected_;
}

std::string AccessibilityElementInfo::GetAccessibilityDescription() const
{
    HILOG_DEBUG("endSelected_[%{public}s]", accessibilityDescription_.c_str());
    return accessibilityDescription_;
}

void AccessibilityElementInfo::SetAccessibilityDescription(const std::string &text)
{
    accessibilityDescription_ = text;
    HILOG_DEBUG("endSelected_[%{public}s]", accessibilityDescription_.c_str());
}

bool AccessibilityElementInfo::GetAccessibilityGroup() const
{
    HILOG_DEBUG("accessibilityGroup_[%{public}d]", accessibilityGroup_);
    return accessibilityGroup_;
}

void AccessibilityElementInfo::SetAccessibilityGroup(const bool group)
{
    accessibilityGroup_ = group;
    HILOG_DEBUG("accessibilityGroup_[%{public}d]", accessibilityGroup_);
}

std::string AccessibilityElementInfo::GetHint() const
{
    HILOG_DEBUG("hintText_[%{public}s]", hintText_.c_str());
    return hintText_;
}

void AccessibilityElementInfo::SetHint(const std::string &hintText)
{
    hintText_ = hintText;
    HILOG_DEBUG("hintText_[%{public}s]", hintText_.c_str());
}

std::string AccessibilityElementInfo::GetDescriptionInfo() const
{
    HILOG_DEBUG("contentDescription_[%{public}s]", contentDescription_.c_str());
    return contentDescription_;
}

void AccessibilityElementInfo::SetDescriptionInfo(const std::string contentDescription)
{
    contentDescription_ = contentDescription;
    HILOG_DEBUG("contentDescription_[%{public}s]", contentDescription_.c_str());
}

void AccessibilityElementInfo::SetComponentResourceId(const std::string &viewIdResName)
{
    resourceName_ = viewIdResName;
    HILOG_DEBUG("resourceName_[%{public}s]", resourceName_.c_str());
}

std::string AccessibilityElementInfo::GetComponentResourceId() const
{
    HILOG_DEBUG("resourceName_[%{public}s]", resourceName_.c_str());
    return resourceName_;
}

void AccessibilityElementInfo::SetLiveRegion(const int liveRegion)
{
    liveRegion_ = liveRegion;
    HILOG_DEBUG("liveRegion_[%{public}d]", liveRegion_);
}

int AccessibilityElementInfo::GetLiveRegion() const
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

std::string AccessibilityElementInfo::GetError() const
{
    HILOG_DEBUG("error_[%{public}s]", error_.c_str());
    return error_;
}

void AccessibilityElementInfo::SetLabeled(const int componentId)
{
    labeled_ = componentId;
    HILOG_DEBUG("labeled_[%{public}d]", labeled_);
}

bool AccessibilityElementInfo::GetLabeled(AccessibilityElementInfo &elementInfo) const
{
    HILOG_DEBUG("labeled_[%{public}d]", labeled_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();

    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_,
            windowId_, labeled_, 0, elementInfos);
        if (elementInfos.empty()) {
            result = false;
        } else {
            elementInfo = elementInfos.front();
        }
    } else {
        HILOG_INFO("Can't get AccessibilityOperator instance");
    }
    return result;
}

int AccessibilityElementInfo::GetLabeledAccessibilityId() const
{
    HILOG_DEBUG("labeled_[%{public}d]", labeled_);
    return labeled_;
}

int AccessibilityElementInfo::GetChannelId() const
{
    HILOG_DEBUG("channelId_[%{public}d]", channelId_);
    return channelId_;
}

void AccessibilityElementInfo::SetChannelId(const int channelId)
{
    channelId_ = channelId;
    HILOG_DEBUG("channelId_[%{public}d]", channelId_);
}

void AccessibilityElementInfo::SetAccessibilityId(const int componentId)
{
    elementId_ = componentId;
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
}

int AccessibilityElementInfo::GetAccessibilityId() const
{
    HILOG_DEBUG("elementId_[%{public}d]", elementId_);
    return elementId_;
}

RangeInfo AccessibilityElementInfo::GetRange() const
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

GridInfo AccessibilityElementInfo::GetGrid() const
{
    HILOG_DEBUG("start");
    return grid_;
}

void AccessibilityElementInfo::SetGrid(const GridInfo &grid)
{
    HILOG_DEBUG("start");
    grid_ = grid;
}

GridItemInfo AccessibilityElementInfo::GetGridItem() const
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

bool AccessibleAction::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start");
    int type = ActionType::ACCESSIBILITY_ACTION_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    actionType_ = ActionType(type);
    return true;
}

bool AccessibleAction::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, int32_t(actionType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    return true;
};

sptr<AccessibleAction> AccessibleAction::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("start");
    sptr<AccessibleAction> accessibleOperation = new AccessibleAction();
    if (!accessibleOperation->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete accessibleOperation;
        accessibleOperation = nullptr;
    }
    return accessibleOperation;
}

AccessibleAction::AccessibleAction(ActionType actionType, std::string description)
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

std::string AccessibleAction::GetDescriptionInfo() const
{
    HILOG_DEBUG("description_[%{public}s]", description_.c_str());
    return description_;
}

bool RangeInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

bool RangeInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

sptr<RangeInfo> RangeInfo::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("start");
    sptr<RangeInfo> rangeInfo = new RangeInfo();
    if (!rangeInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete rangeInfo;
        rangeInfo = nullptr;
    }
    return rangeInfo;
}

RangeInfo::RangeInfo(int min, int max, int current)
{
    HILOG_DEBUG("start");
    min_ = min;
    max_ = max;
    current_ = current;
    HILOG_DEBUG("min_[%{public}d]", min_);
    HILOG_DEBUG("max_[%{public}d]", max_);
    HILOG_DEBUG("current_[%{public}d]", current_);
}

int RangeInfo::GetMin() const
{
    HILOG_DEBUG("min_[%{public}d]", min_);
    return min_;
}

int RangeInfo::GetMax() const
{
    HILOG_DEBUG("max_[%{public}d]", max_);
    return max_;
}

int RangeInfo::GetCurrent() const
{
    HILOG_DEBUG("current_[%{public}d]", current_);
    return current_;
}

void RangeInfo::SetMin(int min)
{
    min_ = min;
    HILOG_DEBUG("min_[%{public}d]", min_);
}

void RangeInfo::SetMax(int max)
{
    max_ = max;
    HILOG_DEBUG("max_[%{public}d]", max_);
}

void RangeInfo::SetCurrent(int current)
{
    current_ = current;
    HILOG_DEBUG("current_[%{public}d]", current_);
}

bool GridInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

bool GridInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
};

sptr<GridInfo> GridInfo::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("start");
    sptr<GridInfo> grid = new GridInfo();
    if (!grid->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete grid;
        grid = nullptr;
    }
    return grid;
}

GridInfo::GridInfo(int rowCount, int columnCount, int mode)
{
    HILOG_DEBUG("start");
    rowCount_ = rowCount;
    columnCount_ = columnCount;
    selectionMode_ = mode;

    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
}

void GridInfo::SetGrid(int rowCount, int columnCount, int mode)
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

int GridInfo::GetRowCount() const
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("rowCount_[%{public}d]", rowCount_);
    return rowCount_;
}

int GridInfo::GetColumnCount() const
{
    HILOG_DEBUG("columnCount_[%{public}d]", columnCount_);
    return columnCount_;
}

int GridInfo::GetSelectionMode() const
{
    HILOG_DEBUG("selectionMode_[%{public}d]", selectionMode_);
    return selectionMode_;
}

bool GridItemInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
}

bool GridItemInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
};

sptr<GridItemInfo> GridItemInfo::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("start");
    sptr<GridItemInfo> gridItem = new GridItemInfo();
    if (!gridItem->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete gridItem;
        gridItem = nullptr;
    }
    return gridItem;
}

GridItemInfo::GridItemInfo(int rowIndex, int rowSpan, int columnIndex, int columnSpan, bool heading, bool selected)
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

void GridItemInfo::SetGridItemInfo(int rowIndex, int rowSpan, int columnIndex,
    int columnSpan, bool heading, bool selected)
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

int GridItemInfo::GetColumnIndex() const
{
    HILOG_DEBUG("columnIndex_[%{public}d]", columnIndex_);
    return columnIndex_;
}

int GridItemInfo::GetRowIndex() const
{
    HILOG_DEBUG("rowIndex_[%{public}d]", rowIndex_);
    return rowIndex_;
}

int GridItemInfo::GetColumnSpan() const
{
    HILOG_DEBUG("columnSpan_[%{public}d]", columnSpan_);
    return columnSpan_;
}

int GridItemInfo::GetRowSpan() const
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

bool Rect::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

bool Rect::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
};

sptr<Rect> Rect::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("start");
    sptr<Rect> rect = new Rect();
    if (!rect->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return rect;
}
} // namespace Accessibility
} // namespace OHOS