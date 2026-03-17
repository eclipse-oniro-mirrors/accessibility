/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "accessibility_gesture_inject_path.h"
#include "accessibility_ui_test_ability.h"
#include "accessible_ability_listener.h"
#include "securec.h"

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 416;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
    constexpr size_t VEC_SIZE = 5;
    constexpr size_t MAP_SIZE = 5;
} // namespace

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

class AccessibleAbilityListenerForFuzzTest : public Accessibility::AccessibleAbilityListener {
public:
    virtual ~AccessibleAbilityListenerForFuzzTest() = default;
    void OnAbilityConnected() override {}
    void OnAbilityDisconnected() override {}
    void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo &eventInfo) override {}
    bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent) override
    {
        return false;
    }
};

static bool CheckGenerateAccessibilityWindowInfo(OHOS::Accessibility::AccessibilityWindowInfo &sourceWindowInfo)
{
    sourceWindowInfo.GetAccessibilityWindowType();
    sourceWindowInfo.GetWindowLayer();
    sourceWindowInfo.GetWindowId();
    sourceWindowInfo.GetRectInScreen();
    sourceWindowInfo.IsActive();
    sourceWindowInfo.IsFocused();
    sourceWindowInfo.IsAccessibilityFocused();
    sourceWindowInfo.GetDisplayId();
    sourceWindowInfo.GetWindowType();
    sourceWindowInfo.GetWindowMode();
    sourceWindowInfo.IsDecorEnable();
    sourceWindowInfo.GetInnerWid();
    sourceWindowInfo.GetUiNodeId();
    sourceWindowInfo.GetScaleVal();
    sourceWindowInfo.GetScaleX();
    sourceWindowInfo.GetScaleY();
    sourceWindowInfo.GetBundleName();
    sourceWindowInfo.GetTouchHotAreas();
    sourceWindowInfo.GetMainWindowId();
    return true;
}

static bool CheckGenerateAccessibilityElementInfoP1(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    int32_t index = 0;
    position += GetObject<int32_t>(index, &data[position], size - position);
    sourceElementInfo.GetChildId(index);
    sourceElementInfo.GetAccessibilityText();
    sourceElementInfo.GetTextType();
    sourceElementInfo.GetOffset();
    sourceElementInfo.GetPageId();
    sourceElementInfo.GetTextMovementStep();
    sourceElementInfo.GetItemCounts();
    sourceElementInfo.GetTriggerAction();
    sourceElementInfo.GetLatestContent();
    sourceElementInfo.GetChildTreeId();
    sourceElementInfo.GetChildWindowId();
    sourceElementInfo.GetBelongTreeId();
    sourceElementInfo.GetParentWindowId();

    std::vector<std::string> contentList;
    for (size_t count = 0; count < VEC_SIZE; count++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        for (size_t i = 0; i < LEN; i++) {
            position += GetObject<char>(name[i], &data[position], size - position);
        }
        std::string content(name);
        contentList.push_back(content);
    }
    sourceElementInfo.GetContentList(contentList);

    int64_t int64Data = 0;
    for (size_t i = 0; i < VEC_SIZE; i++) {
        position += GetObject<int64_t>(int64Data, &data[position], size - position);
        sourceElementInfo.RemoveChild(int64Data);
    }

    return true;
}

static bool CheckGenerateAccessibilityElementInfoP2(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo)
{
    sourceElementInfo.GetChildCount();
    sourceElementInfo.GetChildIds();
    sourceElementInfo.GetActionList();
    sourceElementInfo.GetTextLengthLimit();
    sourceElementInfo.GetMainWindowId();
    sourceElementInfo.GetInnerWindowId();
    sourceElementInfo.GetParentNodeId();
    sourceElementInfo.GetRectInScreen();
    sourceElementInfo.IsCheckable();
    sourceElementInfo.IsChecked();
    sourceElementInfo.IsFocusable();
    sourceElementInfo.IsFocused();
    sourceElementInfo.IsVisible();
    sourceElementInfo.HasAccessibilityFocus();
    sourceElementInfo.IsSelected();
    sourceElementInfo.IsClickable();
    sourceElementInfo.IsLongClickable();
    sourceElementInfo.IsEnabled();
    sourceElementInfo.IsPassword();
    sourceElementInfo.IsScrollable();
    sourceElementInfo.GetCurrentIndex();
    sourceElementInfo.GetBeginIndex();
    sourceElementInfo.GetEndIndex();
    sourceElementInfo.GetInputType();
    sourceElementInfo.GetInspectorKey();
    sourceElementInfo.GetPagePath();
    sourceElementInfo.IsValidElement();
    sourceElementInfo.IsEditable();
    sourceElementInfo.IsPluraLineSupported();
    sourceElementInfo.IsPopupSupported();
    sourceElementInfo.IsDeletable();
    sourceElementInfo.IsEssential();
    sourceElementInfo.IsGivingHint();
    sourceElementInfo.GetBundleName();
    sourceElementInfo.GetComponentType();
    sourceElementInfo.GetContent();
    sourceElementInfo.GetSelectedBegin();
    sourceElementInfo.GetSelectedEnd();
    sourceElementInfo.GetHint();
    sourceElementInfo.GetDescriptionInfo();
    sourceElementInfo.GetComponentResourceId();
    sourceElementInfo.GetLiveRegion();
    sourceElementInfo.GetContentInvalid();
    sourceElementInfo.GetError();
    sourceElementInfo.GetLabeledAccessibilityId();
    sourceElementInfo.GetRange();
    sourceElementInfo.GetGrid();
    sourceElementInfo.GetGridItem();

    return true;
}

static bool CheckGenerateAccessibilityElementInfoP3(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    int32_t int32Data = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t count = 0; count < VEC_SIZE; count++) {
        position += GetObject<int32_t>(int32Data, &data[position], size - position);
        for (size_t i = 0; i < LEN; i++) {
            position += GetObject<char>(name[i], &data[position], size - position);
        }
        std::string description(name);
        OHOS::Accessibility::AccessibleAction action(
            static_cast<OHOS::Accessibility::ActionType>(int32Data), description);
        sourceElementInfo.DeleteAction(action);
    }

    for (size_t count = 0; count < VEC_SIZE; count++) {
        position += GetObject<int32_t>(int32Data, &data[position], size - position);
        for (size_t i = 0; i < LEN; i++) {
            position += GetObject<char>(name[i], &data[position], size - position);
        }
        std::string description(name);
        OHOS::Accessibility::AccessibleAction action(
            static_cast<OHOS::Accessibility::ActionType>(int32Data), description);
        sourceElementInfo.AddAction(action);
        OHOS::Accessibility::ActionType actionType = action.GetActionType();
        sourceElementInfo.DeleteAction(actionType);
    }

    sourceElementInfo.DeleteAllActions();
    return true;
}

static bool CheckGridItemInfo()
{
    OHOS::Accessibility::GridItemInfo gridItem;
    gridItem.GetColumnIndex();
    gridItem.GetRowIndex();
    gridItem.GetColumnSpan();
    gridItem.GetRowSpan();
    gridItem.IsHeading();
    gridItem.IsSelected();
    return true;
}

static size_t GenerateRect(OHOS::Accessibility::Rect &bounds, const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t posX = 0;
    int32_t posY = 0;
    position += GetObject<int32_t>(posX, &data[position], size - position);
    position += GetObject<int32_t>(posY, &data[position], size - position);
    bounds.SetLeftTopScreenPostion(posX, posY);

    position += GetObject<int32_t>(posX, &data[position], size - position);
    position += GetObject<int32_t>(posY, &data[position], size - position);
    bounds.SetRightBottomScreenPostion(posX, posY);
    return position;
}

static size_t GenerateRangeInfo(OHOS::Accessibility::RangeInfo &rangeInfo, const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    rangeInfo.SetMin(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    rangeInfo.SetMax(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    rangeInfo.SetCurrent(int32Data);
    return position;
}

static size_t GenerateGridInfo(OHOS::Accessibility::GridInfo &grid, const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t rowCount = 0;
    int32_t columnCount = 0;
    int32_t selectionMode = 0;
    position += GetObject<int32_t>(rowCount, &data[position], size - position);
    position += GetObject<int32_t>(columnCount, &data[position], size - position);
    position += GetObject<int32_t>(selectionMode, &data[position], size - position);
    grid.SetGrid(rowCount, columnCount, selectionMode);
    return position;
}

static size_t GenerateGridItemInfo(OHOS::Accessibility::GridItemInfo &gridItem, const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t columnIndex_ = 0;
    int32_t rowIndex = 0;
    int32_t columnSpan = 0;
    int32_t rowSpan = 0;
    position += GetObject<int32_t>(columnIndex_, &data[position], size - position);
    position += GetObject<int32_t>(rowIndex, &data[position], size - position);
    position += GetObject<int32_t>(columnSpan, &data[position], size - position);
    position += GetObject<int32_t>(rowSpan, &data[position], size - position);
    bool heading = data[position++] & 0x01;
    bool selected = data[position++] & 0x01;
    gridItem.SetGridItemInfo(rowIndex, rowSpan, columnIndex_, columnSpan, heading, selected);
    return position;
}

static void GenerateAccessibilityElementInfoP1(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    int32_t int32Data = 0;
    int64_t int64Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetPageId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetWindowId(int32Data);

    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetAccessibilityId(int64Data);

    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetComponentId(int64Data);

    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetParent(int64Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetTextLengthLimit(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetCurrentIndex(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetBeginIndex(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetEndIndex(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetLiveRegion(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetLabeled(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetSelectedBegin(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetSelectedEnd(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetInputType(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetItemCounts(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetTriggerAction(static_cast<OHOS::Accessibility::ActionType>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetTextMovementStep(static_cast<OHOS::Accessibility::TextMoveUnit>(int32Data));

    for (size_t i = 0; i < VEC_SIZE; i++) {
        position += GetObject<int64_t>(int64Data, &data[position], size - position);
        sourceElementInfo.AddChild(int64Data);
    }
}

static void GenerateAccessibilityElementInfoP2(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string bundleName(name);
    sourceElementInfo.SetBundleName(bundleName);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string componentType(name);
    sourceElementInfo.SetComponentType(componentType);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string text(name);
    sourceElementInfo.SetContent(text);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string hintText(name);
    sourceElementInfo.SetHint(hintText);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string contentDescription(name);
    sourceElementInfo.SetDescriptionInfo(contentDescription);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string resourceName(name);
    sourceElementInfo.SetComponentResourceId(resourceName);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string inspectorKey(name);
    sourceElementInfo.SetInspectorKey(inspectorKey);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string error(name);
    sourceElementInfo.SetError(error);
}

static void GenerateAccessibilityElementInfoP3(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    sourceElementInfo.SetCheckable(data[position++] & 0x01);
    sourceElementInfo.SetChecked(data[position++] & 0x01);
    sourceElementInfo.SetFocusable(data[position++] & 0x01);
    sourceElementInfo.SetFocused(data[position++] & 0x01);
    sourceElementInfo.SetVisible(data[position++] & 0x01);
    sourceElementInfo.SetAccessibilityFocus(data[position++] & 0x01);
    sourceElementInfo.SetSelected(data[position++] & 0x01);
    sourceElementInfo.SetClickable(data[position++] & 0x01);
    sourceElementInfo.SetLongClickable(data[position++] & 0x01);
    sourceElementInfo.SetEnabled(data[position++] & 0x01);
    sourceElementInfo.SetPassword(data[position++] & 0x01);
    sourceElementInfo.SetScrollable(data[position++] & 0x01);
    sourceElementInfo.SetEditable(data[position++] & 0x01);
    sourceElementInfo.SetPopupSupported(data[position++] & 0x01);
    sourceElementInfo.SetPluraLineSupported(data[position++] & 0x01);
    sourceElementInfo.SetDeletable(data[position++] & 0x01);
    sourceElementInfo.SetHinting(data[position++] & 0x01);
    sourceElementInfo.SetEssential(data[position++] & 0x01);
    sourceElementInfo.SetContentInvalid(data[position++] & 0x01);
    sourceElementInfo.SetValidElement(data[position++] & 0x01);

    OHOS::Accessibility::Rect bounds;
    position += GenerateRect(bounds, &data[position], size - position);
    sourceElementInfo.SetRectInScreen(bounds);

    OHOS::Accessibility::RangeInfo rangeInfo;
    position += GenerateRangeInfo(rangeInfo, &data[position], size - position);
    sourceElementInfo.SetRange(rangeInfo);

    OHOS::Accessibility::GridInfo grid;
    position += GenerateGridInfo(grid, &data[position], size - position);
    sourceElementInfo.SetGrid(grid);

    OHOS::Accessibility::GridItemInfo gridItem;
    position += GenerateGridItemInfo(gridItem, &data[position], size - position);
    sourceElementInfo.SetGridItem(gridItem);

    OHOS::Accessibility::GridItemInfo otherGridItem;
    otherGridItem.SetGridItemInfo(gridItem);

    int32_t int32Data = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t count = 0; count < VEC_SIZE; count++) {
        position += GetObject<int32_t>(int32Data, &data[position], size - position);
        for (size_t i = 0; i < LEN; i++) {
            position += GetObject<char>(name[i], &data[position], size - position);
        }
        std::string description(name);
        OHOS::Accessibility::AccessibleAction action(
            static_cast<OHOS::Accessibility::ActionType>(int32Data), description);
        sourceElementInfo.AddAction(action);
    }
}

static void GenerateAccessibilityElementInfoP4(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    std::vector<std::string> contentList;
    for (size_t count = 0; count < VEC_SIZE; count++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        for (size_t i = 0; i < LEN; i++) {
            position += GetObject<char>(name[i], &data[position], size - position);
        }
        std::string content(name);
        contentList.push_back(content);
    }
    sourceElementInfo.SetContentList(contentList);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string latestContent(name);
    sourceElementInfo.SetLatestContent(latestContent);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string pagePath(name);
    sourceElementInfo.SetPagePath(pagePath);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string accessibilityText(name);
    sourceElementInfo.SetAccessibilityText(accessibilityText);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string textType(name);
    sourceElementInfo.SetTextType(textType);
}

static void GenerateAccessibilityElementInfoP5(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    float offset = 0.0;
    position += GetObject<float>(offset, &data[position], size - position);
    sourceElementInfo.SetOffset(offset);

    int32_t iChildTreeId = 0;
    int32_t iChildWindowId = 0;
    position += GetObject<int32_t>(iChildTreeId, &data[position], size - position);
    position += GetObject<int32_t>(iChildWindowId, &data[position], size - position);
    sourceElementInfo.SetChildTreeIdAndWinId(iChildTreeId, iChildWindowId);

    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetBelongTreeId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetParentWindowId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetMainWindowId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetInnerWindowId(int32Data);
}

static size_t GenerateAccessibilityElementInfo(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    GenerateAccessibilityElementInfoP1(sourceElementInfo, data, size, position);
    GenerateAccessibilityElementInfoP2(sourceElementInfo, data, size, position);
    GenerateAccessibilityElementInfoP3(sourceElementInfo, data, size, position);
    GenerateAccessibilityElementInfoP4(sourceElementInfo, data, size, position);
    GenerateAccessibilityElementInfoP5(sourceElementInfo, data, size, position);
    return position;
}

static size_t GenerateAccessibilityWindowInfo(OHOS::Accessibility::AccessibilityWindowInfo &sourceWindowInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    uint64_t uint64Data = 0;
    position += GetObject<uint64_t>(uint64Data, &data[position], size - position);
    sourceWindowInfo.SetDisplayId(uint64Data);
   
    uint32_t uint32Data = 0;
    position += GetObject<uint32_t>(uint32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowMode(uint32Data);

    position += GetObject<uint32_t>(uint32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowType(uint32Data);

    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetAccessibilityWindowType(static_cast<OHOS::Accessibility::AccessibilityWindowType>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowLayer(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowId(int32Data);

    sourceWindowInfo.SetActive(data[position++] & 0x01);
    sourceWindowInfo.SetFocused(data[position++] & 0x01);
    sourceWindowInfo.SetAccessibilityFocused(data[position++] & 0x01);
    sourceWindowInfo.SetDecorEnable(data[position++] & 0x01);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetInnerWid(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetMainWindowId(int32Data);

    int64_t int64Data = 0;
    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceWindowInfo.SetUiNodeId(int32Data);

    float floatData = 0.0;
    position += GetObject<float>(floatData, &data[position], size - position);
    sourceWindowInfo.SetScaleVal(floatData);

    position += GetObject<float>(floatData, &data[position], size - position);
    sourceWindowInfo.SetScaleX(floatData);

    position += GetObject<float>(floatData, &data[position], size - position);
    sourceWindowInfo.SetScaleY(floatData);

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string bundleName(name);
    sourceWindowInfo.SetBundleName(bundleName);

    OHOS::Accessibility::Rect bounds;
    position += GenerateRect(bounds, &data[position], size - position);
    sourceWindowInfo.SetRectInScreen(bounds);
    std::vector<OHOS::Accessibility::Rect> touchHotAreas = {bounds};
    sourceWindowInfo.SetTouchHotAreas(touchHotAreas);
    return position;
}

static size_t GenerateAccessibilityEventInfo(OHOS::Accessibility::AccessibilityEventInfo &sourceEventInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    uint32_t uint32Data = 0;
    position += GetObject<uint32_t>(uint32Data, &data[position], size - position);
    sourceEventInfo.SetEventType(static_cast<OHOS::Accessibility::EventType>(uint32Data));

    position += GetObject<uint32_t>(uint32Data, &data[position], size - position);
    sourceEventInfo.SetGestureType(static_cast<OHOS::Accessibility::GestureType>(uint32Data));

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string bundleName(name);
    sourceEventInfo.SetBundleName(bundleName);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string notificationContent(name);
    sourceEventInfo.SetNotificationContent(notificationContent);

    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetTriggerAction(static_cast<OHOS::Accessibility::ActionType>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetTextMovementStep(static_cast<OHOS::Accessibility::TextMoveUnit>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetWindowContentChangeTypes(
        static_cast<OHOS::Accessibility::WindowsContentChangeTypes>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetWindowChangeTypes(static_cast<OHOS::Accessibility::WindowUpdateType>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetNotificationInfo(static_cast<OHOS::Accessibility::NotificationCategory>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetPageId(int32Data);

    int64_t int64Data = 0;
    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceEventInfo.SetTimeStamp(int64Data);

    return position;
}
} // namespace OHOS