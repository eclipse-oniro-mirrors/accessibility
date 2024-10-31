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

#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
/* AccessibilityElementInfoParcel       Parcel struct                 */
AccessibilityElementInfoParcel::AccessibilityElementInfoParcel(const AccessibilityElementInfo &elementInfo)
    : AccessibilityElementInfo(elementInfo)
{
}

bool AccessibilityElementInfoParcel::ReadFromParcelFirstPart(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);
    int32_t textMoveStep = STEP_CHARACTER;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textMoveStep);
    textMoveStep_ = static_cast<TextMoveUnit>(textMoveStep);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, itemCounts_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, elementId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, parentId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, belongTreeId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childTreeId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childWindowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentWindowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64Vector, parcel, &childNodeIds_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, textType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, offset_);
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcelSecondPart(Parcel &parcel)
{
    int32_t operationsSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operationsSize);
    bool verifyResult = ContainerSecurityVerify(parcel, operationsSize, operations_.max_size());
    if (!verifyResult || operationsSize < 0 || operationsSize > INT32_MAX) {
        return false;
    }
    for (int32_t i = 0; i < operationsSize; i++) {
        sptr<AccessibleActionParcel> accessibleOperation = parcel.ReadStrongParcelable<AccessibleActionParcel>();
        if (!accessibleOperation) {
            HILOG_ERROR("ReadStrongParcelable<accessibleOperation> failed");
            return false;
        }
        operations_.emplace_back(*accessibleOperation);
    }

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, navDestinationId_);
    sptr<RectParcel> rect = parcel.ReadStrongParcelable<RectParcel>();
    if (!rect) {
        return false;
    }
    bounds_ = *rect;

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
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isPassword_);
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
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcelThirdPart(Parcel &parcel)
{
    sptr<RangeInfoParcel> rangeInfo = parcel.ReadStrongParcelable<RangeInfoParcel>();
    if (!rangeInfo) {
        return false;
    }
    rangeInfo_ = *rangeInfo;

    sptr<GridInfoParcel> grid = parcel.ReadStrongParcelable<GridInfoParcel>();
    if (!grid) {
        return false;
    }
    grid_ = *grid;

    sptr<GridItemInfoParcel> gridItem = parcel.ReadStrongParcelable<GridItemInfoParcel>();
    if (!gridItem) {
        return false;
    }
    gridItem_ = *gridItem;

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, labeled_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, inspectorKey_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pagePath_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityGroup_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityLevel_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, zIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, opacity_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundColor_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundImage_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, blur_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hitTestBehavior_);

    sptr<ExtraElementInfoParcel> extraElementInfo = parcel.ReadStrongParcelable<ExtraElementInfoParcel>();
    if (extraElementInfo == nullptr) {
        return false;
    }
    extraElementInfo_ = *extraElementInfo;
    
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcelFourthPart(Parcel &parcel)
{
    int32_t spanListSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, spanListSize);
    if (!ContainerSecurityVerify(parcel, spanListSize, spanList_.max_size())) {
        return false;
    }

    for (int32_t i = 0; i < spanListSize; i++) {
        sptr<SpanInfoParcel> spanList = parcel.ReadStrongParcelable<SpanInfoParcel>();
        if (!spanList) {
            HILOG_ERROR("ReadStrongParcelable<spanList> failed");
            return false;
        }
        spanList_.emplace_back(*spanList);
    }
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcel(Parcel &parcel)
{
    if (!ReadFromParcelFirstPart(parcel)) {
        return false;
    }
    if (!ReadFromParcelSecondPart(parcel)) {
        return false;
    }
    if (!ReadFromParcelThirdPart(parcel)) {
        return false;
    }
    if (!ReadFromParcelFourthPart(parcel)) {
        return false;
    }
    return true;
}

bool AccessibilityElementInfoParcel::MarshallingFirstPart(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(textMoveStep_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, itemCounts_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, elementId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, parentId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, belongTreeId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childTreeId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childWindowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentWindowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64Vector, parcel, childNodeIds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, textType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, offset_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operations_.size());
    for (auto &operations : operations_) {
        AccessibleActionParcel action(operations);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &action);
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, navDestinationId_);
    return true;
}

bool AccessibilityElementInfoParcel::MarshallingSecondPart(Parcel &parcel) const
{
    RectParcel boundsParcel(bounds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &boundsParcel);
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
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isPassword_);
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
    RangeInfoParcel rangeInfoParcel(rangeInfo_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &rangeInfoParcel);
    GridInfoParcel gridParcel(grid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &gridParcel);
    GridItemInfoParcel gridItemParcel(gridItem_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &gridItemParcel);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, labeled_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, inspectorKey_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pagePath_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityGroup_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityLevel_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, zIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, opacity_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundColor_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundImage_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, blur_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hitTestBehavior_);
    return true;
}

bool AccessibilityElementInfoParcel::MarshallingThirdPart(Parcel &parcel) const
{
    ExtraElementInfoParcel extraElementInfoParcel(extraElementInfo_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &extraElementInfoParcel);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, spanList_.size());
    for (auto &span : spanList_) {
        SpanInfoParcel spanList(span);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &spanList);
    }

    return true;
}

bool AccessibilityElementInfoParcel::Marshalling(Parcel &parcel) const
{
    if (!MarshallingFirstPart(parcel)) {
        return false;
    }
    if (!MarshallingSecondPart(parcel)) {
        return false;
    }
    if (!MarshallingThirdPart(parcel)) {
        return false;
    }
    return true;
}

sptr<AccessibilityElementInfoParcel> AccessibilityElementInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<AccessibilityElementInfoParcel> accessibilityInfo = new(std::nothrow) AccessibilityElementInfoParcel();
    if (!accessibilityInfo) {
        HILOG_ERROR("Failed to create accessibilityInfo.");
        return nullptr;
    }
    if (!accessibilityInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return accessibilityInfo;
}

AccessibleActionParcel::AccessibleActionParcel(const AccessibleAction &action)
    : AccessibleAction(action)
{
}

bool AccessibleActionParcel::ReadFromParcel(Parcel &parcel)
{
    int32_t type = ActionType::ACCESSIBILITY_ACTION_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type);
    actionType_ = static_cast<ActionType>(type);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    return true;
}

bool AccessibleActionParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(actionType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    return true;
}

sptr<AccessibleActionParcel> AccessibleActionParcel::Unmarshalling(Parcel& parcel)
{
    sptr<AccessibleActionParcel> accessibleOperation = new(std::nothrow) AccessibleActionParcel();
    if (!accessibleOperation) {
        HILOG_ERROR("Failed to create accessibleOperation.");
        return nullptr;
    }
    if (!accessibleOperation->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return accessibleOperation;
}

RangeInfoParcel::RangeInfoParcel(const RangeInfo &rangeInfo)
    : RangeInfo(rangeInfo)
{
}

bool RangeInfoParcel::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, min_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, max_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, current_);

    return true;
}

bool RangeInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, min_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, max_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Double, parcel, current_);

    return true;
}

sptr<RangeInfoParcel> RangeInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<RangeInfoParcel> rangeInfo = new(std::nothrow) RangeInfoParcel();
    if (rangeInfo == nullptr) {
        HILOG_ERROR("Failed to create rangeInfo.");
        return nullptr;
    }
    if (!rangeInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return rangeInfo;
}

GridInfoParcel::GridInfoParcel(const GridInfo &gridInfo)
    : GridInfo(gridInfo)
{
}

bool GridInfoParcel::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

bool GridInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

sptr<GridInfoParcel> GridInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<GridInfoParcel> grid = new(std::nothrow) GridInfoParcel();
    if (!grid) {
        HILOG_ERROR("Failed to create grid.");
        return nullptr;
    }
    if (!grid->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return grid;
}

GridItemInfoParcel::GridItemInfoParcel(const GridItemInfo &itemInfo)
    : GridItemInfo(itemInfo)
{
}

bool GridItemInfoParcel::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
}

bool GridItemInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
}

sptr<GridItemInfoParcel> GridItemInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<GridItemInfoParcel> gridItem = new(std::nothrow) GridItemInfoParcel();
    if (!gridItem) {
        HILOG_ERROR("Failed to create gridItem.");
        return nullptr;
    }
    if (!gridItem->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return gridItem;
}

RectParcel::RectParcel(const Rect &rect)
    : Rect(rect)
{
}

bool RectParcel::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

bool RectParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

sptr<RectParcel> RectParcel::Unmarshalling(Parcel& parcel)
{
    sptr<RectParcel> rect = new(std::nothrow) RectParcel();
    if (!rect) {
        HILOG_ERROR("Failed to create rect.");
        return nullptr;
    }
    if (!rect->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return rect;
}

ExtraElementInfoParcel::ExtraElementInfoParcel(const ExtraElementInfo &extraElementInfo)
    : ExtraElementInfo(extraElementInfo)
{
}

bool ExtraElementInfoParcel::ReadFromParcel(Parcel &parcel)
{
    int32_t mapValueStr = 0;
    int32_t mapValueInt = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mapValueStr);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mapValueInt);
    HILOG_DEBUG("ReadFromParcel: size is map, mapValueStr: %{public}d,mapValueInt: %{public}d",
        mapValueStr, mapValueInt);

    if (!ContainerSecurityVerify(parcel, mapValueStr, extraElementValueStr_.max_size())) {
        HILOG_INFO("extraElementValueStr : ExtraElementInfoParcel verify is false");
        return false;
    }
    for (int32_t i = 0; i < mapValueStr; i++) {
        std::string tempMapKey;
        std::string tempMapVal;

        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapKey);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapVal);
        HILOG_INFO("ReadFromParcel: extraElementValueStr's tempMapKey: %{public}s, tempMapVal: %{public}s",
            tempMapKey.c_str(), tempMapVal.c_str());
        extraElementValueStr_[tempMapKey] = tempMapVal;
    }

    if (!ContainerSecurityVerify(parcel, mapValueInt, extraElementValueInt_.max_size())) {
        HILOG_INFO("extraElementValueInt : ExtraElementInfoParcel verify is false");
        return false;
    }
    for (int32_t i = 0; i < mapValueInt; i++) {
        std::string tempMapKey;
        int32_t tempMapVal;

        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapKey);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, tempMapVal);
        HILOG_INFO("ReadFromParcel: extraElementValueInt's tempMapKey: %{public}s, tempMapVal: %{public}d",
            tempMapKey.c_str(), tempMapVal);
        extraElementValueInt_[tempMapKey] = tempMapVal;
    }

    return true;
}

bool ExtraElementInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, extraElementValueStr_.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, extraElementValueInt_.size());
    for (auto iterStr = extraElementValueStr_.begin(); iterStr != extraElementValueStr_.end(); ++iterStr) {
        std::string tempMapKey;
        std::string tempMapVal;
        tempMapKey = iterStr->first;
        tempMapVal = iterStr->second;
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapKey);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapVal);
    }

    for (auto iterInt = extraElementValueInt_.begin(); iterInt != extraElementValueInt_.end(); ++iterInt) {
        std::string tempMapKey;
        int32_t tempMapVal;
        tempMapKey = iterInt->first;
        tempMapVal = iterInt->second;
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, tempMapKey);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, tempMapVal);
    }

    return true;
}

sptr<ExtraElementInfoParcel> ExtraElementInfoParcel::Unmarshalling(Parcel &parcel)
{
    sptr<ExtraElementInfoParcel> extraElementInfo = new(std::nothrow) ExtraElementInfoParcel();
    if (extraElementInfo == nullptr) {
        HILOG_ERROR("Failed to create extraElementInfo.");
        return nullptr;
    }
    if (!extraElementInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return extraElementInfo;
}

SpanInfoParcel::SpanInfoParcel(const SpanInfo &spanInfo)
    : SpanInfo(spanInfo)
{
}

bool SpanInfoParcel::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, spanId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, spanText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityDescription_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityLevel_);
    return true;
}

bool SpanInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, spanId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, spanText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityDescription_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityLevel_);
    return true;
}

sptr<SpanInfoParcel> SpanInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<SpanInfoParcel> spanInfo = new(std::nothrow) SpanInfoParcel();
    if (spanInfo == nullptr) {
        HILOG_ERROR("Failed to create spanInfo.");
        return nullptr;
    }
    if (!spanInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return spanInfo;
}
} // namespace Accessibility
} // namespace OHOS