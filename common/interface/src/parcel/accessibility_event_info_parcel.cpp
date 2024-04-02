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

#include "accessibility_event_info_parcel.h"
#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityEventInfoParcel::AccessibilityEventInfoParcel(const AccessibilityEventInfo &eventInfo)
{
    AccessibilityEventInfo *self = this;
    *self = eventInfo;
}

bool AccessibilityEventInfoParcel::ReadFromParcelFirstPart(Parcel &parcel)
{
    uint32_t eventType = TYPE_VIEW_INVALID;
    uint32_t gestureType = GESTURE_INVALID;
    int32_t triggerAction = ACCESSIBILITY_ACTION_INVALID;
    int32_t textMoveStep = STEP_CHARACTER;
    int32_t windowContentChangeTypes = CONTENT_CHANGE_TYPE_INVALID;
    int32_t windowChangeTypes = WINDOW_UPDATE_INVALID;
    int32_t category = CATEGORY_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, eventType);
    eventType_ = static_cast<EventType>(eventType);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, gestureType);
    gestureType_ = static_cast<GestureType>(gestureType);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, triggerAction);
    triggerAction_ = static_cast<ActionType>(triggerAction);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, timeStamp_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textMoveStep);
    textMoveStep_ = static_cast<TextMoveUnit>(textMoveStep);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowContentChangeTypes);
    windowContentChangeTypes_ = static_cast<WindowsContentChangeTypes>(windowContentChangeTypes);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowChangeTypes);
    windowChangeTypes_ = static_cast<WindowUpdateType>(windowChangeTypes);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, category);
    category_ = static_cast<NotificationCategory>(category);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);
    return true;
}

bool AccessibilityEventInfoParcel::ReadFromParcelSecondPart(Parcel &parcel)
{
    int64_t componentId = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, componentId);
    SetSource(componentId);
    int32_t windowId = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId);
    SetWindowId(windowId);
    int32_t currentIndex = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, currentIndex);
    SetCurrentIndex(currentIndex);
    int32_t beginIndex = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginIndex);
    SetBeginIndex(beginIndex);
    int32_t endIndex = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endIndex);
    SetEndIndex(endIndex);
    int32_t contentSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, contentSize);
    std::string content;
    ContainerSecurityVerify(parcel, contentSize, contents_.max_size());
    for (auto i = 0 ; i < contentSize; i++) {
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, content);
        AddContent(content);
    }
    std::string componentType;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType);
    SetComponentType(componentType);
    std::string description;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description);
    SetDescription(description);
    std::string beforeText;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, beforeText);
    SetBeforeText(beforeText);
    std::string latestConent;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, latestConent);
    SetLatestContent(latestConent);
    int64_t elementId = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, elementId);

    int32_t itemCounts = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, itemCounts);
    SetItemCounts(itemCounts);
    sptr<AccessibilityElementInfoParcel> elementInfoParcel =
	    parcel.ReadStrongParcelable<AccessibilityElementInfoParcel>();
    if (elementInfoParcel == nullptr) {
        HILOG_ERROR("ReadStrongParcelable elementInfo failed.");
        return false;
    }
    SetElementInfo(*elementInfoParcel);
    return true;
}

bool AccessibilityEventInfoParcel::ReadFromParcelThirdPart(Parcel &parcel)
{
    std::string textAnnouncedForAccessibility;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, textAnnouncedForAccessibility);
    SetTextAnnouncedForAccessibility(textAnnouncedForAccessibility);

    std::string inspectorKey;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, inspectorKey);
    SetInspectorKey(inspectorKey);

    int32_t requestFocusElementId = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, requestFocusElementId);
    SetRequestFocusElementId(requestFocusElementId);
    return true;
}

bool AccessibilityEventInfoParcel::ReadFromParcel(Parcel &parcel)
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
    return true;
}

bool AccessibilityEventInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, static_cast<uint32_t>(eventType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, static_cast<uint32_t>(gestureType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(triggerAction_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, timeStamp_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(textMoveStep_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(windowContentChangeTypes_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(windowChangeTypes_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(category_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetViewId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetWindowId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetCurrentIndex());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetBeginIndex());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetEndIndex());
    auto contentList = GetContentList();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, contentList.size());
    for (auto &content : contentList) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, content);
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetComponentType());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetDescription());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetBeforeText());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetLatestContent());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, GetAccessibilityId());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetItemCounts());
    AccessibilityElementInfoParcel elementInfoParcel(elementInfo_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &elementInfoParcel);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetTextAnnouncedForAccessibility());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, GetInspectorKey());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, GetRequestFocusElementId());
    return true;
}

sptr<AccessibilityEventInfoParcel> AccessibilityEventInfoParcel::Unmarshalling(Parcel& parcel)
{
    sptr<AccessibilityEventInfoParcel> accessibilityEventInfo = new(std::nothrow) AccessibilityEventInfoParcel();
    if (!accessibilityEventInfo) {
        HILOG_ERROR("Failed to create accessibilityEventInfo.");
        return nullptr;
    }
    if (!accessibilityEventInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return accessibilityEventInfo;
}
} // namespace Accessibility
} // namespace OHOS
