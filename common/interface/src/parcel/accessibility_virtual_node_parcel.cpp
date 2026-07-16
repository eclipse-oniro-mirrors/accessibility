/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#include "accessibility_virtual_node_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityVirtualNodeParcel::AccessibilityVirtualNodeParcel(const AccessibilityVirtualNode &virtualNode)
    : AccessibilityVirtualNode(virtualNode)
{}

bool AccessibilityVirtualNodeParcel::ReadBasicInfoFromParcel(Parcel &parcel)
{
    int64_t id = -1;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, id);
    SetId(id);

    std::string text;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text);
    SetText(text);

    std::string accessibilityText;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityText);
    SetAccessibilityText(accessibilityText);

    bool accessibilityGroup = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityGroup);
    SetAccessibilityGroup(accessibilityGroup);

    std::string accessibilityLevel;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, accessibilityLevel);
    SetAccessibilityLevel(accessibilityLevel);

    int32_t leftTopX = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX);
    int32_t leftTopY = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY);
    int32_t rightBottomX = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX);
    int32_t rightBottomY = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY);
    Rect rect;
    rect.SetLeftTopScreenPostion(leftTopX, leftTopY);
    rect.SetRightBottomScreenPostion(rightBottomX, rightBottomY);
    SetRect(rect);

    return true;
}

bool AccessibilityVirtualNodeParcel::ReadBoolPropertiesFromParcel(Parcel &parcel)
{
    bool checkable = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checkable);
    SetCheckable(checkable);

    bool checked = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checked);
    SetChecked(checked);

    bool clickable = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, clickable);
    SetClickable(clickable);

    bool enabled = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enabled);
    SetEnabled(enabled);

    bool selected = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected);
    SetSelected(selected);

    return true;
}

bool AccessibilityVirtualNodeParcel::ReadExtraInfoFromParcel(Parcel &parcel)
{
    std::string customComponentType;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, customComponentType);
    SetCustomComponentType(customComponentType);

    int32_t pointX = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pointX);
    int32_t pointY = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pointY);
    AccessibilityVirtualPoint point;
    point.SetX(pointX);
    point.SetY(pointY);
    SetPoint(point);

    bool accessibilityFocused = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused);
    SetAccessibilityFocused(accessibilityFocused);

    int64_t parentId = -1;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, parentId);
    SetParentId(parentId);
    return true;
}

bool AccessibilityVirtualNodeParcel::ReadChildNodesFromParcel(Parcel &parcel)
{
    int32_t childNodeCount = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childNodeCount);
    bool verifyResult = ContainerSecurityVerify(parcel, childNodeCount, childNodeIds_.max_size());
    if (!verifyResult || childNodeCount < 0 || childNodeCount > MAX_ALLOW_SIZE) {
        HILOG_ERROR("childNodeCount is invalid: %{public}d", childNodeCount);
        return false;
    }
    std::vector<int64_t> childNodeIds;
    for (int32_t i = 0; i < childNodeCount; i++) {
        int64_t childNodeId = -1;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, childNodeId);
        childNodeIds.push_back(childNodeId);
    }
    SetChildNodeIds(childNodeIds);
    return true;
}

bool AccessibilityVirtualNodeParcel::ReadFromParcel(Parcel &parcel)
{
    if (!ReadBasicInfoFromParcel(parcel)) {
        return false;
    }
    if (!ReadBoolPropertiesFromParcel(parcel)) {
        return false;
    }
    if (!ReadExtraInfoFromParcel(parcel)) {
        return false;
    }
    if (!ReadChildNodesFromParcel(parcel)) {
        return false;
    }

    int64_t elementId = -1;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, elementId);
    SetElementId(elementId);

    int32_t windowId = -1;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId);
    SetWindowId(windowId);

    uint64_t supportAction = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint64, parcel, supportAction);
    SetSupportAction(supportAction);
    return true;
}

bool AccessibilityVirtualNodeParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetText());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetAccessibilityText());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetAccessibilityGroup());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetAccessibilityLevel());

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetRect().GetLeftTopXScreenPostion());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetRect().GetLeftTopYScreenPostion());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetRect().GetRightBottomXScreenPostion());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetRect().GetRightBottomYScreenPostion());

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetCheckable());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetChecked());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetClickable());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetEnabled());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetSelected());

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetCustomComponentType());

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetPoint().GetX());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetPoint().GetY());

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, GetAccessibilityFocused());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetParentId());

    int32_t childNodeCount = static_cast<int32_t>(GetChildNodeIds().size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childNodeCount);
    for (int32_t i = 0; i < childNodeCount; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetChildNodeIds()[i]);
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetElementId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetWindowId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint64, parcel, GetSupportAction());
    return true;
}

AccessibilityVirtualNodeParcel *AccessibilityVirtualNodeParcel::Unmarshalling(Parcel &parcel)
{
    AccessibilityVirtualNodeParcel *obj = new(std::nothrow) AccessibilityVirtualNodeParcel();
    if (obj == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityVirtualNodeParcel");
        return nullptr;
    }
    if (!obj->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel failed");
        delete obj;
        obj = nullptr;
        return nullptr;
    }
    return obj;
}
} // namespace Accessibility
} // namespace OHOS
