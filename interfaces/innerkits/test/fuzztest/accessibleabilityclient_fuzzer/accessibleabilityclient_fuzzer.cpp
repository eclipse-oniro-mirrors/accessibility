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

#include "accessibleabilityclient_fuzzer.h"
#include "accessibility_element_info.h"
#include "accessibility_ui_test_ability.h"

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
    std::memcpy(&object, data, objectSize);
    return objectSize;
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

static size_t GenerateAccessibilityElementInfo(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t int32Data = 0;

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetWindowId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetAccessibilityId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetParent(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetTextLengthLimit(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetChannelId(int32Data);

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

    for (size_t i = 0; i < VEC_SIZE; i++) {
        position += GetObject<int32_t>(int32Data, &data[position], size - position);
        sourceElementInfo.AddChild(int32Data);
    }

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    std::memcpy(&name, &data[position], LEN);
    std::string bundleName(name);
    sourceElementInfo.SetBundleName(bundleName);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string componentType(name);
    sourceElementInfo.SetComponentType(componentType);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string text(name);
    sourceElementInfo.SetContent(text);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string hintText(name);
    sourceElementInfo.SetHint(hintText);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string contentDescription(name);
    sourceElementInfo.SetDescriptionInfo(contentDescription);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string resourceName(name);
    sourceElementInfo.SetComponentResourceId(resourceName);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string inspectorKey(name);
    sourceElementInfo.SetInspectorKey(inspectorKey);
    position += LEN;

    std::memcpy(&name, &data[position], LEN);
    std::string error(name);
    sourceElementInfo.SetError(error);
    position += LEN;

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

    for (size_t i = 0; i < VEC_SIZE; i++) {
        position += GetObject<int32_t>(int32Data, &data[position], size - position);
        std::memcpy(&name, &data[position], LEN);
        position += LEN;
        std::string description(name);

        OHOS::Accessibility::AccessibleAction action(
            static_cast<OHOS::Accessibility::ActionType>(int32Data), description);
        sourceElementInfo.AddAction(action);
    }
    return position;
}

static size_t GenerateAccessibilityWindowInfo(OHOS::Accessibility::AccessibilityWindowInfo &sourceWindowInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    uint64_t uint64Data = 0;
    position += GetObject<uint64_t>(uint64Data, &data[position], size - position);
    sourceWindowInfo.SetDisplayId(uint64Data);

    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowLayer(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetWindowId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetAnchorId(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceWindowInfo.SetChannelId(int32Data);

    sourceWindowInfo.SetActive(data[position++] & 0x01);
    sourceWindowInfo.SetFocused(data[position++] & 0x01);
    sourceWindowInfo.SetAccessibilityFocused(data[position++] & 0x01);

    OHOS::Accessibility::Rect bounds;
    position += GenerateRect(bounds, &data[position], size - position);
    sourceWindowInfo.SetRectInScreen(bounds);
    return position;
}

static size_t GenerateAccessibilityEventInfoPartial(OHOS::Accessibility::AccessibilityEventInfo &sourceEventInfo,
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
    std::memcpy(&name, &data[position], LEN);
    std::string bundleName(name);
    sourceEventInfo.SetBundleName(bundleName);
    position += LEN;

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
    sourceEventInfo.SetRecordCount(int32Data);

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetNotificationInfo(static_cast<OHOS::Accessibility::NotificationCategory>(int32Data));

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceEventInfo.SetPageId(int32Data);

    int64_t int64Data = 0;
    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceEventInfo.SetTimeStamp(int64Data);

    return position;
}

static size_t GenerateAccessibilityEventInfo(OHOS::Accessibility::AccessibilityEventInfo &sourceEventInfo,
    const uint8_t* data, size_t size)
{
    size_t position = 0;
    position += GenerateAccessibilityEventInfoPartial(sourceEventInfo, &data[position], size - position);

    for (size_t i = 0; i < VEC_SIZE; i++) {
        OHOS::Accessibility::AccessibilityEventInfo record;
        position += GenerateAccessibilityEventInfoPartial(record, &data[position], size - position);
        sourceEventInfo.AddRecord(record);
    }
    return position;
}

bool DoSomethingInterestingWithGetFocus(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t focusType = 0;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GetObject<int32_t>(focusType, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetFocus(focusType, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetFocusByElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t focusType = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GetObject<int32_t>(focusType, &data[startPos], size - startPos);
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetFocusByElementInfo(
        sourceElementInfo, focusType, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetRootByWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityWindowInfo sourceWindowInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GenerateAccessibilityWindowInfo(sourceWindowInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetRootByWindow(
        sourceWindowInfo, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetWindows(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    uint64_t displayId = 0;
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> resultWindowInfos;
    startPos += GetObject<uint64_t>(displayId, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetWindows(displayId, resultWindowInfos);

    return true;
}

bool DoSomethingInterestingWithExecuteCommonAction(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    uint32_t commonAction = 0;
    startPos += GetObject<uint32_t>(commonAction, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->ExecuteCommonAction(
        static_cast<OHOS::Accessibility::GlobalAction>(commonAction));

    return true;
}

bool DoSomethingInterestingWithGetNext(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t direction = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GetObject<int32_t>(direction, &data[startPos], size - startPos);
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetNext(sourceElementInfo,
        static_cast<OHOS::Accessibility::FocusMoveDirection>(direction), resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetChildElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t index = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GetObject<int32_t>(index, &data[startPos], size - startPos);
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetChildElementInfo(
        index, sourceElementInfo, resultElementInfo);
    return true;
}

bool DoSomethingInterestingWithGetByContent(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> resultElementInfos;
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    std::memcpy(&name, &data[startPos], LEN);
    std::string text(name);
    startPos += LEN;
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetByContent(
        sourceElementInfo, text, resultElementInfos);

    return true;
}

bool DoSomethingInterestingWithGetAnchor(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityWindowInfo sourceWindowInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GenerateAccessibilityWindowInfo(sourceWindowInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetAnchor(sourceWindowInfo, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetSource(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityEventInfo sourceEventInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GenerateAccessibilityEventInfo(sourceEventInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetSource(sourceEventInfo, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetParentElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetParentElementInfo(
        sourceElementInfo, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithExecuteAction(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    int32_t action = 0;
    startPos += GetObject<int32_t>(action, &data[startPos], size - startPos);
    std::map<std::string, std::string> actionArguments;
    for (size_t i = 0; i < MAP_SIZE; i++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        std::memcpy(&name, &data[startPos], LEN);
        std::string action1(name);
        startPos += LEN;

        std::memcpy(&name, &data[startPos], LEN);
        std::string action2(name);
        startPos += LEN;
        actionArguments.insert(std::make_pair(action1, action2));
    }
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->ExecuteAction(
        sourceElementInfo, static_cast<OHOS::Accessibility::ActionType>(action), actionArguments);

    return true;
}

bool DoSomethingInterestingWithSetEventTypeFilter(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    uint32_t filter = 0;
    startPos += GetObject<uint32_t>(filter, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->SetEventTypeFilter(filter);

    return true;
}

bool DoSomethingInterestingWithSetTargetBundleName(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    std::vector<std::string> targetBundleNames;
    for (size_t i = 0; i < VEC_SIZE; i++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        std::memcpy(&name, &data[startPos], LEN);
        std::string targetBundleName(name);
        startPos += LEN;
        targetBundleNames.push_back(targetBundleName);
    }
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->SetTargetBundleName(targetBundleNames);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithGetFocus(data, size);
    OHOS::DoSomethingInterestingWithGetFocusByElementInfo(data, size);
    OHOS::DoSomethingInterestingWithGetRootByWindow(data, size);
    OHOS::DoSomethingInterestingWithGetWindows(data, size);
    OHOS::DoSomethingInterestingWithExecuteCommonAction(data, size);
    OHOS::DoSomethingInterestingWithGetNext(data, size);
    OHOS::DoSomethingInterestingWithGetChildElementInfo(data, size);
    OHOS::DoSomethingInterestingWithGetByContent(data, size);
    OHOS::DoSomethingInterestingWithGetAnchor(data, size);
    OHOS::DoSomethingInterestingWithGetSource(data, size);
    OHOS::DoSomethingInterestingWithGetParentElementInfo(data, size);
    OHOS::DoSomethingInterestingWithExecuteAction(data, size);
    OHOS::DoSomethingInterestingWithSetEventTypeFilter(data, size);
    OHOS::DoSomethingInterestingWithSetTargetBundleName(data, size);
    return 0;
}