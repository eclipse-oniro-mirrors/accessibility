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
    HILOG_DEBUG("[%{public}s] elementId_[%{public}d]", __func__, elementId_);
}

bool AccessibilityElementInfo::GetFocus(const int focus, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], focus[%{public}d]",
        __func__, channelId_, windowId_, elementId_, focus);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result =  instance->FindFocusedElementInfo(channelId_, windowId_, elementId_, focus, elementInfo);
    } else {
        HILOG_ERROR("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

bool AccessibilityElementInfo::GetNext(const FocusMoveDirection direction, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], direction[%{public}d]",
        __func__, channelId_, windowId_, elementId_, direction);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->FocusMoveSearch(channelId_, windowId_, elementId_, direction, elementInfo);
    } else {
        HILOG_ERROR("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

int AccessibilityElementInfo::GetChildId(const int index) const
{
    HILOG_DEBUG("[%{public}s] index[%{public}d]", __func__, index);
    if (index >= childCount_ || index < 0) {
        HILOG_ERROR("[%{public}s] called] index[%{public}d] is invalid", __func__, index);
        return 0;
    }
    return childNodeIds_[index];
}
int AccessibilityElementInfo::GetChildCount() const
{
    HILOG_DEBUG("[%{public}s] childCount_[%{public}d]", __func__, childCount_);
    return childCount_;
}

bool AccessibilityElementInfo::GetChild(const int index, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], index[%{public}d]",
        __func__, channelId_, windowId_, elementId_, index);
    if (index >= childCount_ || index < 0) {
        HILOG_ERROR("[%{public}s] called] index[%{public}d] is invalid", __func__, index);
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
        HILOG_ERROR("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

std::vector<int> AccessibilityElementInfo::GetChildIds() const
{
    HILOG_DEBUG("[%{public}s] childCount_[%{public}d]", __func__, childCount_);
    return childNodeIds_;
}
void AccessibilityElementInfo::AddChild(const int childId)
{
    HILOG_DEBUG("[%{public}s] childId[%{public}d]", __func__, childId);
    for (int i = 0; i < childCount_; i++) {
        if (childNodeIds_[i] == childId) {
            HILOG_ERROR("[%{public}s] childId[%{public}d] is exited", __func__, childId);
            return;
        }
    }
    childCount_++;
    childNodeIds_.push_back(childId);
    HILOG_DEBUG("[%{public}s] childId[%{public}d] end", __func__, childId);
}

bool AccessibilityElementInfo::RemoveChild(const int childId)
{
    HILOG_DEBUG("[%{public}s] childId[%{public}d]", __func__, childId);
    for (auto iter = childNodeIds_.begin(); iter != childNodeIds_.end(); iter++) {
        if (*iter == childId) {
            iter = childNodeIds_.erase(iter);
            childCount_--;
            return true;
        }
    }
    HILOG_ERROR("[%{public}s] Not find childId[%{public}d]", __func__, childId);
    return false;
}

std::vector<AccessibleAction> AccessibilityElementInfo::GetActionList() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return operations_;
}

void AccessibilityElementInfo::AddAction(AccessibleAction &action)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    operations_.push_back(action);
    HILOG_DEBUG("[%{public}s] actionType[%{public}d] end", __func__, action.GetActionType());
}

void AccessibilityElementInfo::DeleteAction(AccessibleAction &action)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    for (auto iter = operations_.begin(); iter != operations_.end(); iter++) {
        HILOG_DEBUG("[%{public}s] actionType[%{public}d]", __func__, action.GetActionType());
        if (iter->GetActionType() == action.GetActionType()) {
            iter = operations_.erase(iter);
            return;
        }
    }
    HILOG_ERROR("[%{public}s] Not find actionType[%{public}d]", __func__, action.GetActionType());
}

bool AccessibilityElementInfo::DeleteAction(ActionType &actionType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    for (auto iter = operations_.begin(); iter != operations_.end(); iter++) {
        HILOG_DEBUG("[%{public}s] actionType[%{public}d]", __func__, actionType);
        if (iter->GetActionType() == actionType) {
            iter = operations_.erase(iter);
            return true;
        }
    }
    HILOG_ERROR("[%{public}s] Not find actionType[%{public}d]", __func__, actionType);
    return false;
}

void AccessibilityElementInfo::DeleteAllActions()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    operations_.clear();
}

void AccessibilityElementInfo::SetTextLengthLimit(const int max)
{
    textLengthLimit_ = max;
    HILOG_DEBUG("[%{public}s] textLengthLimit_[%{public}d]", __func__, textLengthLimit_);
}

int AccessibilityElementInfo::GetTextLengthLimit() const
{
    HILOG_DEBUG("[%{public}s] textLengthLimit_[%{public}d]", __func__, textLengthLimit_);
    return textLengthLimit_;
}

bool AccessibilityElementInfo::ExecuteAction(const ActionType &action,
        const std::map<std::string, std::string> &actionArguments)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d], elementId_[%{public}d]",
        __func__, channelId_, windowId_, elementId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    if (instance != nullptr) {
        return instance->ExecuteAction(channelId_, windowId_, elementId_, action,
            const_cast<std::map<std::string, std::string> &>(actionArguments));
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
        return false;
    }
}

bool AccessibilityElementInfo::GetByContent(const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d],\
        elementId_[%{public}d], text[%{public}s]",
        __func__, channelId_, windowId_, elementId_, text.c_str());
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByText(channelId_, windowId_, elementId_, text, elementInfos);
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

bool AccessibilityElementInfo::GetElementInfosById(const int elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_,
            windowId_, elementId, mode, elementInfos);
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }

    return result;
}

int AccessibilityElementInfo::GetWindowId() const
{
    HILOG_DEBUG("[%{public}s] windowId_[%{public}d]", __func__, windowId_);
    return windowId_;
}

void AccessibilityElementInfo::SetWindowId(const int windowId)
{
    windowId_ = windowId;
    HILOG_DEBUG("[%{public}s] windowId_[%{public}d]", __func__, windowId_);
}

bool AccessibilityElementInfo::GetParent(AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d], parentId_[%{public}d]",
        __func__, channelId_, windowId_, parentId_);
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
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

int AccessibilityElementInfo::GetParentNodeId() const
{
    HILOG_DEBUG("[%{public}s] parentId_[%{public}d]", __func__, parentId_);
    return parentId_;
}

void AccessibilityElementInfo::SetParent(const int parentId)
{
    parentId_ = parentId;
    HILOG_DEBUG("[%{public}s] parentId_[%{public}d]", __func__, parentId_);
}
Rect AccessibilityElementInfo::GetRectInScreen() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return bounds_;
}

void AccessibilityElementInfo::SetRectInScreen(Rect &bounds)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    bounds_.SetLeftTopScreenPostion(bounds.GetLeftTopXScreenPostion(), bounds.GetLeftTopYScreenPostion());
    bounds_.SetRightBottomScreenPostion(bounds.GetRightBottomXScreenPostion(), bounds.GetRightBottomYScreenPostion());
}

bool AccessibilityElementInfo::IsCheckable() const
{
    HILOG_DEBUG("[%{public}s] checkable_[%{public}d]", __func__, checkable_);
    return checkable_;
}

void AccessibilityElementInfo::SetCheckable(const bool checkable)
{
    checkable_ = checkable;
    HILOG_DEBUG("[%{public}s] checkable_[%{public}d]", __func__, checkable_);
}

bool AccessibilityElementInfo::IsChecked() const
{
    HILOG_DEBUG("[%{public}s] checked_[%{public}d]", __func__, checked_);
    return checked_;
}

void AccessibilityElementInfo::SetChecked(const bool checked)
{
    checked_ = checked;
    HILOG_DEBUG("[%{public}s] checked_[%{public}d]", __func__, checked_);
}

bool AccessibilityElementInfo::IsFocusable() const
{
    HILOG_DEBUG("[%{public}s] focusable_[%{public}d]", __func__, focusable_);
    return focusable_;
}

void AccessibilityElementInfo::SetFocusable(const bool focusable)
{
    focusable_ = focusable;
    HILOG_DEBUG("[%{public}s] focusable_[%{public}d]", __func__, focusable_);
}

bool AccessibilityElementInfo::IsFocused() const
{
    HILOG_DEBUG("[%{public}s] focused_[%{public}d]", __func__, focused_);
    return focused_;
}

void AccessibilityElementInfo::SetFocused(const bool focused)
{
    focused_ = focused;
    HILOG_DEBUG("[%{public}s] focused_[%{public}d]", __func__, focused_);
}

bool AccessibilityElementInfo::IsVisible() const
{
    HILOG_DEBUG("[%{public}s] visible_[%{public}d]", __func__, visible_);
    return visible_;
}

void AccessibilityElementInfo::SetVisible(const bool visible)
{
    visible_ = visible;
    HILOG_DEBUG("[%{public}s] visible_[%{public}d]", __func__, visible_);
}

bool AccessibilityElementInfo::HasAccessibilityFocus() const
{
    HILOG_DEBUG("[%{public}s] accessibilityFocused_[%{public}d]", __func__, accessibilityFocused_);
    return accessibilityFocused_;
}

void AccessibilityElementInfo::SetAccessibilityFocus(const bool focused)
{
    accessibilityFocused_ = focused;
    HILOG_DEBUG("[%{public}s] accessibilityFocused_[%{public}d]", __func__, accessibilityFocused_);
}

bool AccessibilityElementInfo::IsSelected() const
{
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
    return selected_;
}

void AccessibilityElementInfo::SetSelected(const bool selected)
{
    selected_ = selected;
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
}

bool AccessibilityElementInfo::IsClickable() const
{
    HILOG_DEBUG("[%{public}s] clickable_[%{public}d]", __func__, clickable_);
    return clickable_;
}

void AccessibilityElementInfo::SetClickable(const bool clickable)
{
    clickable_ = clickable;
    HILOG_DEBUG("[%{public}s] clickable_[%{public}d]", __func__, clickable_);
}

bool AccessibilityElementInfo::IsLongClickable() const
{
    HILOG_DEBUG("[%{public}s] longClickable_[%{public}d]", __func__, longClickable_);
    return longClickable_;
}

void AccessibilityElementInfo::SetLongClickable(const bool longClickable)
{
    longClickable_ = longClickable;
    HILOG_DEBUG("[%{public}s] longClickable_[%{public}d]", __func__, longClickable_);
}

bool AccessibilityElementInfo::IsEnabled() const
{
    HILOG_DEBUG("[%{public}s] enable_[%{public}d]", __func__, enable_);
    return enable_;
}

void AccessibilityElementInfo::SetEnabled(const bool enabled)
{
    enable_ = enabled;
    HILOG_DEBUG("[%{public}s] enable_[%{public}d]", __func__, enable_);
}

bool AccessibilityElementInfo::IsPassword() const
{
    HILOG_DEBUG("[%{public}s] password_[%{public}d]", __func__, password_);
    return password_;
}

void AccessibilityElementInfo::SetPassword(const bool password)
{
    password_ = password;
    HILOG_DEBUG("[%{public}s] password_[%{public}d]", __func__, password_);
}

bool AccessibilityElementInfo::IsScrollable() const
{
    HILOG_DEBUG("[%{public}s] scrollable_[%{public}d]", __func__, scrollable_);
    return scrollable_;
}

void AccessibilityElementInfo::SetScrollable(const bool scrollable)
{
    scrollable_ = scrollable;
    HILOG_DEBUG("[%{public}s] scrollable_[%{public}d]", __func__, scrollable_);
}

int AccessibilityElementInfo::GetCurrentIndex() const
{
    HILOG_DEBUG("[%{public}s] currentIndex_[%{public}d]", __func__, currentIndex_);
    return currentIndex_;
}

void AccessibilityElementInfo::SetCurrentIndex(const int index)
{
    currentIndex_ = index;
    HILOG_DEBUG("[%{public}s] currentIndex_[%{public}d]", __func__, currentIndex_);
}

int AccessibilityElementInfo::GetBeginIndex() const
{
    HILOG_DEBUG("[%{public}s] beginIndex_[%{public}d]", __func__, beginIndex_);
    return beginIndex_;
}

void AccessibilityElementInfo::SetBeginIndex(const int index)
{
    beginIndex_ = index;
    HILOG_DEBUG("[%{public}s] beginIndex_[%{public}d]", __func__, beginIndex_);
}

int AccessibilityElementInfo::GetEndIndex() const
{
    HILOG_DEBUG("[%{public}s] endIndex_[%{public}d]", __func__, endIndex_);
    return endIndex_;
}

void AccessibilityElementInfo::SetEndIndex(const int index)
{
    endIndex_ = index;
    HILOG_DEBUG("[%{public}s] endIndex_[%{public}d]", __func__, endIndex_);
}

int AccessibilityElementInfo::GetInputType() const
{
    HILOG_DEBUG("[%{public}s] inputType_[%{public}d]", __func__, inputType_);
    return inputType_;
}

void AccessibilityElementInfo::SetInputType(const int inputType)
{
    inputType_ = inputType;
    HILOG_DEBUG("[%{public}s] inputType_[%{public}d]", __func__, inputType_);
}

void AccessibilityElementInfo::SetValidElement(const bool valid)
{
    HILOG_DEBUG("[%{public}s] validElevalidment_[%{public}d]", __func__, valid);
    validElement_ = valid;
}

bool AccessibilityElementInfo::IsValidElement() const
{
    HILOG_DEBUG("[%{public}s] validElement_[%{public}d]", __func__, validElement_);
    return validElement_;
}

bool AccessibilityElementInfo::IsEditable() const
{
    HILOG_DEBUG("[%{public}s] editable_[%{public}d]", __func__, editable_);
    return editable_;
}

void AccessibilityElementInfo::SetEditable(const bool editable)
{
    editable_ = editable;
    HILOG_DEBUG("[%{public}s] editable_[%{public}d]", __func__, editable_);
}

bool AccessibilityElementInfo::IsPluraLineSupported() const
{
    HILOG_DEBUG("[%{public}s] multiLine_[%{public}d]", __func__, multiLine_);
    return multiLine_;
}

void AccessibilityElementInfo::SetPluraLineSupported(const bool multiLine)
{
    multiLine_ = multiLine;
    HILOG_DEBUG("[%{public}s] multiLine_[%{public}d]", __func__, multiLine_);
}

bool AccessibilityElementInfo::IsPopupSupported() const
{
    HILOG_DEBUG("[%{public}s] popupSupported_[%{public}d]", __func__, popupSupported_);
    return popupSupported_;
}

void AccessibilityElementInfo::SetPopupSupported(const bool supportPopup)
{
    popupSupported_ = supportPopup;
    HILOG_DEBUG("[%{public}s] popupSupported_[%{public}d]", __func__, popupSupported_);
}

bool AccessibilityElementInfo::IsDeletable() const
{
    HILOG_DEBUG("[%{public}s] deletable_[%{public}d]", __func__, deletable_);
    return deletable_;
}

void AccessibilityElementInfo::SetDeletable(const bool deletable)
{
    deletable_ = deletable;
    HILOG_DEBUG("[%{public}s] deletable_[%{public}d]", __func__, deletable_);
}

bool AccessibilityElementInfo::IsEssential() const
{
    HILOG_DEBUG("[%{public}s] isEssential_[%{public}d]", __func__, isEssential_);
    return isEssential_;
}

void AccessibilityElementInfo::SetEssential(const bool essential)
{
    isEssential_ = essential;
    HILOG_DEBUG("[%{public}s] isEssential_[%{public}d]", __func__, isEssential_);
}

bool AccessibilityElementInfo::IsGivingHint() const
{
    HILOG_DEBUG("[%{public}s] hint_[%{public}d]", __func__, hint_);
    return hint_;
}
void AccessibilityElementInfo::SetHinting(const bool hinting)
{
    hint_ = hinting;
    HILOG_DEBUG("[%{public}s] hint_[%{public}d]", __func__, hint_);
}

std::string AccessibilityElementInfo::GetBundleName() const
{
    HILOG_DEBUG("[%{public}s] bundleName_[%{public}s]", __func__, bundleName_.c_str());
    return bundleName_;
}

void AccessibilityElementInfo::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
    HILOG_DEBUG("[%{public}s] bundleName_[%{public}s]", __func__, bundleName_.c_str());
}

std::string AccessibilityElementInfo::GetComponentType() const
{
    HILOG_DEBUG("[%{public}s] componentType_[%{public}s]", __func__, componentType_.c_str());
    return componentType_;
}

void AccessibilityElementInfo::SetComponentType(const std::string &className)
{
    componentType_ = className;
    HILOG_DEBUG("[%{public}s] componentType_[%{public}s]", __func__, componentType_.c_str());
}

std::string AccessibilityElementInfo::GetContent() const
{
    HILOG_DEBUG("[%{public}s] text_[%{public}s]", __func__, text_.c_str());
    return text_;
}

void AccessibilityElementInfo::SetContent(const std::string &text)
{
    text_ = text;
    HILOG_DEBUG("[%{public}s] text_[%{public}s]", __func__, text_.c_str());
}

std::string AccessibilityElementInfo::GetAccessibilityContent() const
{
    HILOG_DEBUG("[%{public}s] accessibilityText_[%{public}s]", __func__, accessibilityText_.c_str());
    return accessibilityText_;
}

void AccessibilityElementInfo::SetAccessibilityContent(const std::string &text)
{
    accessibilityText_ = text;
    HILOG_DEBUG("[%{public}s] accessibilityText_[%{public}s]", __func__, accessibilityText_.c_str());
}

void AccessibilityElementInfo::SetSelectedBegin(const int start)
{
    beginSelected_ = start;
    HILOG_DEBUG("[%{public}s] beginSelected_[%{public}d]", __func__, beginSelected_);
}

int AccessibilityElementInfo::GetSelectedBegin() const
{
    HILOG_DEBUG("[%{public}s] beginSelected_[%{public}d]", __func__, beginSelected_);
    return beginSelected_;
}

void AccessibilityElementInfo::SetSelectedEnd(const int end)
{
    endSelected_ = end;
    HILOG_DEBUG("[%{public}s] endSelected_[%{public}d]", __func__, endSelected_);
}

int AccessibilityElementInfo::GetSelectedEnd() const
{
    HILOG_DEBUG("[%{public}s] endSelected_[%{public}d]", __func__, endSelected_);
    return endSelected_;
}

std::string AccessibilityElementInfo::GetAccessibilityDescription() const
{
    HILOG_DEBUG("[%{public}s] endSelected_[%{public}s]", __func__, accessibilityDescription_.c_str());
    return accessibilityDescription_;
}

void AccessibilityElementInfo::SetAccessibilityDescription(const std::string &text)
{
    accessibilityDescription_ = text;
    HILOG_DEBUG("[%{public}s] endSelected_[%{public}s]", __func__, accessibilityDescription_.c_str());
}

bool AccessibilityElementInfo::GetAccessibilityGroup() const
{
    HILOG_DEBUG("[%{public}s] accessibilityGroup_[%{public}d]", __func__, accessibilityGroup_);
    return accessibilityGroup_;
}

void AccessibilityElementInfo::SetAccessibilityGroup(const bool group)
{
    accessibilityGroup_ = group;
    HILOG_DEBUG("[%{public}s] accessibilityGroup_[%{public}d]", __func__, accessibilityGroup_);
}

std::string AccessibilityElementInfo::GetHint() const
{
    HILOG_DEBUG("[%{public}s] hintText_[%{public}s]", __func__, hintText_.c_str());
    return hintText_;
}

void AccessibilityElementInfo::SetHint(const std::string &hintText)
{
    hintText_ = hintText;
    HILOG_DEBUG("[%{public}s] hintText_[%{public}s]", __func__, hintText_.c_str());
}

std::string AccessibilityElementInfo::GetDescriptionInfo() const
{
    HILOG_DEBUG("[%{public}s] contentDescription_[%{public}s]", __func__, contentDescription_.c_str());
    return contentDescription_;
}

void AccessibilityElementInfo::SetDescriptionInfo(const std::string contentDescription)
{
    contentDescription_ = contentDescription;
    HILOG_DEBUG("[%{public}s] contentDescription_[%{public}s]", __func__, contentDescription_.c_str());
}

void AccessibilityElementInfo::SetComponentResourceId(const std::string &viewIdResName)
{
    resourceName_ = viewIdResName;
    HILOG_DEBUG("[%{public}s] resourceName_[%{public}s]", __func__, resourceName_.c_str());
}

std::string AccessibilityElementInfo::GetComponentResourceId() const
{
    HILOG_DEBUG("[%{public}s] resourceName_[%{public}s]", __func__, resourceName_.c_str());
    return resourceName_;
}

void AccessibilityElementInfo::SetLiveRegion(const int liveRegion)
{
    liveRegion_ = liveRegion;
    HILOG_DEBUG("[%{public}s] liveRegion_[%{public}d]", __func__, liveRegion_);
}

int AccessibilityElementInfo::GetLiveRegion() const
{
    HILOG_DEBUG("[%{public}s] liveRegion_[%{public}d]", __func__, liveRegion_);
    return liveRegion_;
}

void AccessibilityElementInfo::SetContentInvalid(const bool contentInvalid)
{
    contentInvalid_ = contentInvalid;
    HILOG_DEBUG("[%{public}s] contentInvalid_[%{public}d]", __func__, contentInvalid_);
}

bool AccessibilityElementInfo::GetContentInvalid() const
{
    HILOG_DEBUG("[%{public}s] contentInvalid_[%{public}d]", __func__, contentInvalid_);
    return contentInvalid_;
}

void AccessibilityElementInfo::SetError(const std::string &error)
{
    error_ = error;
    HILOG_DEBUG("[%{public}s] error_[%{public}s]", __func__, error_.c_str());
}

std::string AccessibilityElementInfo::GetError() const
{
    HILOG_DEBUG("[%{public}s] error_[%{public}s]", __func__, error_.c_str());
    return error_;
}

void AccessibilityElementInfo::SetLabeled(const int componentId)
{
    labeled_ = componentId;
    HILOG_DEBUG("[%{public}s] labeled_[%{public}d]", __func__, labeled_);
}

bool AccessibilityElementInfo::GetLabeled(AccessibilityElementInfo &elementInfo) const
{
    HILOG_DEBUG("[%{public}s] labeled_[%{public}d]", __func__, labeled_);
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
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

int AccessibilityElementInfo::GetLabeledAccessibilityId() const
{
    HILOG_DEBUG("[%{public}s] labeled_[%{public}d]", __func__, labeled_);
    return labeled_;
}

int AccessibilityElementInfo::GetChannelId() const
{
    HILOG_DEBUG("[%{public}s] channelId_[%{public}d]", __func__, channelId_);
    return channelId_;
}

void AccessibilityElementInfo::SetChannelId(const int channelId)
{
    channelId_ = channelId;
    HILOG_DEBUG("[%{public}s] channelId_[%{public}d]", __func__, channelId_);
}

void AccessibilityElementInfo::SetAccessibilityId(const int componentId)
{
    elementId_ = componentId;
    HILOG_DEBUG("[%{public}s] elementId_[%{public}d]", __func__, elementId_);
}

int AccessibilityElementInfo::GetAccessibilityId() const
{
    HILOG_DEBUG("[%{public}s] elementId_[%{public}d]", __func__, elementId_);
    return elementId_;
}

RangeInfo AccessibilityElementInfo::GetRange() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return rangeInfo_;
}

void AccessibilityElementInfo::SetRange(RangeInfo &rangeInfo)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rangeInfo_.SetMax(rangeInfo.GetMax());
    rangeInfo_.SetMin(rangeInfo.GetMin());
    rangeInfo_.SetCurrent(rangeInfo.GetCurrent());
}

GridInfo AccessibilityElementInfo::GetGrid() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return grid_;
}

void AccessibilityElementInfo::SetGrid(const GridInfo &grid)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    grid_ = grid;
}

GridItemInfo AccessibilityElementInfo::GetGridItem() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return gridItem_;
}

void AccessibilityElementInfo::SetGridItem(const GridItemInfo &gridItem)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    gridItem_ = gridItem;
}

AccessibilityElementInfo::AccessibilityElementInfo()
{
    HILOG_DEBUG("[%{public}s]", __func__);
}

bool AccessibleAction::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    int type = ActionType::ACCESSIBILITY_ACTION_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    actionType_ = ActionType(type);
    return true;
}

bool AccessibleAction::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, int32_t(actionType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    return true;
};

sptr<AccessibleAction> AccessibleAction::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
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
    HILOG_DEBUG("[%{public}s]", __func__);
    actionType_ = actionType;
    description_ = description;
}

ActionType AccessibleAction::GetActionType() const
{
    HILOG_DEBUG("[%{public}s] actionType_[%{public}d]", __func__, actionType_);
    return actionType_;
}

std::string AccessibleAction::GetDescriptionInfo() const
{
    HILOG_DEBUG("[%{public}s] description_[%{public}s]", __func__, description_.c_str());
    return description_;
}

bool RangeInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

bool RangeInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

sptr<RangeInfo> RangeInfo::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
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
    HILOG_DEBUG("[%{public}s]", __func__);
    min_ = min;
    max_ = max;
    current_ = current;
    HILOG_DEBUG("[%{public}s] min_[%{public}d]", __func__, min_);
    HILOG_DEBUG("[%{public}s] max_[%{public}d]", __func__, max_);
    HILOG_DEBUG("[%{public}s] current_[%{public}d]", __func__, current_);
}

int RangeInfo::GetMin() const
{
    HILOG_DEBUG("[%{public}s] min_[%{public}d]", __func__, min_);
    return min_;
}

int RangeInfo::GetMax() const
{
    HILOG_DEBUG("[%{public}s] max_[%{public}d]", __func__, max_);
    return max_;
}

int RangeInfo::GetCurrent() const
{
    HILOG_DEBUG("[%{public}s] current_[%{public}d]", __func__, current_);
    return current_;
}

void RangeInfo::SetMin(int min)
{
    min_ = min;
    HILOG_DEBUG("[%{public}s] min_[%{public}d]", __func__, min_);
}

void RangeInfo::SetMax(int max)
{
    max_ = max;
    HILOG_DEBUG("[%{public}s] max_[%{public}d]", __func__, max_);
}

void RangeInfo::SetCurrent(int current)
{
    current_ = current;
    HILOG_DEBUG("[%{public}s] current_[%{public}d]", __func__, current_);
}

bool GridInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

bool GridInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
};

sptr<GridInfo> GridInfo::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    sptr<GridInfo> grid = new GridInfo();
    if (!grid->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete grid;
        grid = nullptr;
    }
    return grid;
}

GridInfo::GridInfo(int rowCount, int columnCount,
                int mode)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowCount_ = rowCount;
    columnCount_ = columnCount;
    selectionMode_ = mode;

    HILOG_DEBUG("[%{public}s] rowCount_[%{public}d]", __func__, rowCount_);
    HILOG_DEBUG("[%{public}s] columnCount_[%{public}d]", __func__, columnCount_);
    HILOG_DEBUG("[%{public}s] selectionMode_[%{public}d]", __func__, selectionMode_);
}

void GridInfo::SetGrid(int rowCount, int columnCount,
            int mode)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowCount_ = rowCount;
    columnCount_ = columnCount;
    selectionMode_ = mode;

    HILOG_DEBUG("[%{public}s] rowCount_[%{public}d]", __func__, rowCount_);
    HILOG_DEBUG("[%{public}s] columnCount_[%{public}d]", __func__, columnCount_);
    HILOG_DEBUG("[%{public}s] selectionMode_[%{public}d]", __func__, selectionMode_);
}

void GridInfo::SetGrid(GridInfo other)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowCount_ = other.rowCount_;
    columnCount_ = other.columnCount_;
    selectionMode_ = other.selectionMode_;

    HILOG_DEBUG("[%{public}s] rowCount_[%{public}d]", __func__, rowCount_);
    HILOG_DEBUG("[%{public}s] columnCount_[%{public}d]", __func__, columnCount_);
    HILOG_DEBUG("[%{public}s] selectionMode_[%{public}d]", __func__, selectionMode_);
}

int GridInfo::GetRowCount() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    HILOG_DEBUG("[%{public}s] rowCount_[%{public}d]", __func__, rowCount_);
    return rowCount_;
}

int GridInfo::GetColumnCount() const
{
    HILOG_DEBUG("[%{public}s] columnCount_[%{public}d]", __func__, columnCount_);
    return columnCount_;
}

int GridInfo::GetSelectionMode() const
{
    HILOG_DEBUG("[%{public}s] selectionMode_[%{public}d]", __func__, selectionMode_);
    return selectionMode_;
}

bool GridItemInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
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
    HILOG_DEBUG("[%{public}s]", __func__);
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
    HILOG_DEBUG("[%{public}s]", __func__);
    sptr<GridItemInfo> gridItem = new GridItemInfo();
    if (!gridItem->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete gridItem;
        gridItem = nullptr;
    }
    return gridItem;
}

GridItemInfo::GridItemInfo(int rowIndex, int rowSpan, int columnIndex, int columnSpan,
            bool heading, bool selected)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowIndex_ = rowIndex;
    rowSpan_ = rowSpan;
    columnIndex_ = columnIndex;
    columnSpan_ = columnSpan;
    heading_ = heading;
    selected_ = selected;

    HILOG_DEBUG("[%{public}s] rowIndex_[%{public}d]", __func__, rowIndex_);
    HILOG_DEBUG("[%{public}s] rowSpan_[%{public}d]", __func__, rowSpan_);
    HILOG_DEBUG("[%{public}s] columnIndex_[%{public}d]", __func__, columnIndex_);
    HILOG_DEBUG("[%{public}s] columnSpan_[%{public}d]", __func__, columnSpan_);
    HILOG_DEBUG("[%{public}s] heading_[%{public}d]", __func__, heading_);
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
}

void GridItemInfo::SetGridItemInfo(GridItemInfo other)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowIndex_ = other.rowIndex_;
    rowSpan_ = other.rowSpan_;
    columnIndex_ = other.columnIndex_;
    columnSpan_ = other.columnSpan_;
    heading_ = other.heading_;
    selected_ = other.selected_;

    HILOG_DEBUG("[%{public}s] rowIndex_[%{public}d]", __func__, rowIndex_);
    HILOG_DEBUG("[%{public}s] rowSpan_[%{public}d]", __func__, rowSpan_);
    HILOG_DEBUG("[%{public}s] columnIndex_[%{public}d]", __func__, columnIndex_);
    HILOG_DEBUG("[%{public}s] columnSpan_[%{public}d]", __func__, columnSpan_);
    HILOG_DEBUG("[%{public}s] heading_[%{public}d]", __func__, heading_);
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
}

void GridItemInfo::SetGridItemInfo(int rowIndex, int rowSpan,
                                int columnIndex, int columnSpan, bool heading, bool selected)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    rowIndex_ = rowIndex;
    rowSpan_ = rowSpan;
    columnIndex_ = columnIndex;
    columnSpan_ = columnSpan;
    heading_ = heading;
    selected_ = selected;

    HILOG_DEBUG("[%{public}s] rowIndex_[%{public}d]", __func__, rowIndex_);
    HILOG_DEBUG("[%{public}s] rowSpan_[%{public}d]", __func__, rowSpan_);
    HILOG_DEBUG("[%{public}s] columnIndex_[%{public}d]", __func__, columnIndex_);
    HILOG_DEBUG("[%{public}s] columnSpan_[%{public}d]", __func__, columnSpan_);
    HILOG_DEBUG("[%{public}s] heading_[%{public}d]", __func__, heading_);
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
}

int GridItemInfo::GetColumnIndex() const
{
    HILOG_DEBUG("[%{public}s] columnIndex_[%{public}d]", __func__, columnIndex_);
    return columnIndex_;
}

int GridItemInfo::GetRowIndex() const
{
    HILOG_DEBUG("[%{public}s] rowIndex_[%{public}d]", __func__, rowIndex_);
    return rowIndex_;
}

int GridItemInfo::GetColumnSpan() const
{
    HILOG_DEBUG("[%{public}s] columnSpan_[%{public}d]", __func__, columnSpan_);
    return columnSpan_;
}

int GridItemInfo::GetRowSpan() const
{
    HILOG_DEBUG("[%{public}s] rowSpan_[%{public}d]", __func__, rowSpan_);
    return rowSpan_;
}

bool GridItemInfo::IsHeading() const
{
    HILOG_DEBUG("[%{public}s] heading_[%{public}d]", __func__, heading_);
    return heading_;
}

bool GridItemInfo::IsSelected() const
{
    HILOG_DEBUG("[%{public}s] selected_[%{public}d]", __func__, selected_);
    return selected_;
}

bool Rect::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

bool Rect::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
};

sptr<Rect> Rect::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    sptr<Rect> rect = new Rect();
    if (!rect->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return rect;
}

} // namespace Accessibility
} // namespace OHOS