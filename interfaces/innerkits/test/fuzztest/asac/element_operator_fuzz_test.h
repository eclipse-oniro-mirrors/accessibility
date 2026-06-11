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

#ifndef ELEMENT_OPERATOR_FUZZ_H
#define ELEMENT_OPERATOR_FUZZ_H

#include "accessibility_system_ability_client_impl.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

class ElementOperatorForFuzzTest : public AccessibilityElementOperator {
public:
    virtual ~ElementOperatorForFuzzTest() = default;
    RetError SearchElementInfoByAccessibilityId(const int64_t elementId,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback,
        const int32_t mode) override { return RET_OK; }
    void SearchDefaultFocusByWindowId(const int32_t windowId,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback,
        const int32_t pageId) override {}
    void SearchElementInfosByText(const int64_t elementId, const std::string &text,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) override {}

    void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}

    void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}

    void ExecuteAction(const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) override {}
    void GetCursorPosition(const int64_t elementId, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}
    void SearchElementInfoBySpecificProperty(const int64_t elementId,
        const SpecificPropertyParam& param, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}
    void ClearFocus() override {}
    void OutsideTouch() override {}
    void SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId,
        const int32_t childWindowId) override {}
    void SetBelongTreeId(const int32_t treeId) override {}
    void SetParentWindowId(const int32_t parentWindowId) override {}
    void FocusMoveSearchWithCondition(const AccessibilityElementInfo &info,
        const AccessibilityFocusMoveParam param,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) override {}
    void UpdateCustomAccessibilityProperty(const int64_t elementId,
        const AccessibilityVirtualNode& accessibilityVirtualNode, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}
    void AddAccessibilityVirtualNode(const int64_t rootId,
        const std::vector<AccessibilityVirtualNode> &nodes, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}
    void RemoveAccessibilityVirtualNode(const int64_t id, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}
};

class StateObserverForFuzzTest : public AccessibilityStateObserver {
public:
    virtual ~StateObserverForFuzzTest() = default;
    void OnStateChanged(const bool state) {}
};


static void CreateEventInfoFirstPart(AccessibilityEventInfo &eventInfo, const uint8_t* data, size_t size,
    size_t& position)
{
    int64_t componentId = 0;
    position += GetObject<int64_t>(componentId, &data[position], size - position);
    eventInfo.SetSource(componentId);
    int32_t windowId = 0;
    position += GetObject<int32_t>(windowId, &data[position], size - position);
    eventInfo.SetWindowId(windowId);
    int32_t index = 0;
    position += GetObject<int32_t>(index, &data[position], size - position);
    eventInfo.SetCurrentIndex(index);
    position += GetObject<int32_t>(index, &data[position], size - position);
    eventInfo.SetBeginIndex(index);
    position += GetObject<int32_t>(index, &data[position], size - position);
    eventInfo.SetEndIndex(index);
    int32_t itemCounts = 0;
    position += GetObject<int32_t>(itemCounts, &data[position], size - position);
    eventInfo.SetItemCounts(itemCounts);
    WindowsContentChangeTypes changeTypes;
    position += GetObject<WindowsContentChangeTypes>(changeTypes, &data[position], size - position);
    eventInfo.SetWindowContentChangeTypes(changeTypes);
    WindowUpdateType updateTypes;
    position += GetObject<WindowUpdateType>(updateTypes, &data[position], size - position);
    eventInfo.SetWindowChangeTypes(updateTypes);
    EventType eventType;
    position += GetObject<EventType>(eventType, &data[position], size - position);
    eventInfo.SetEventType(eventType);
    int64_t timeStamp = 0;
    position += GetObject<int64_t>(timeStamp, &data[position], size - position);
    eventInfo.SetTimeStamp(timeStamp);
    TextMoveUnit granularity;
    position += GetObject<TextMoveUnit>(granularity, &data[position], size - position);
    eventInfo.SetTextMovementStep(granularity);
    ActionType action;
    position += GetObject<ActionType>(action, &data[position], size - position);
    eventInfo.SetTriggerAction(action);
    NotificationCategory category;
    position += GetObject<NotificationCategory>(category, &data[position], size - position);
    eventInfo.SetNotificationInfo(category);
    GestureType gestureType;
    position += GetObject<GestureType>(gestureType, &data[position], size - position);
    eventInfo.SetGestureType(gestureType);
    int32_t pageId = 0;
    position += GetObject<int32_t>(pageId, &data[position], size - position);
    eventInfo.SetPageId(pageId);
    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    eventInfo.SetElementMainWindowId(int32Data);
}

static void CreateEventInfoSecondPart(AccessibilityEventInfo &eventInfo, const uint8_t* data, size_t size,
    size_t& position)
{
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string className(name);
    eventInfo.SetComponentType(className);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string beforeText(name);
    eventInfo.SetBeforeText(beforeText);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string content(name);
    eventInfo.AddContent(content);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string lastContent(name);
    eventInfo.SetLatestContent(lastContent);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string contentDescription(name);
    eventInfo.SetDescription(contentDescription);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string bundleName(name);
    eventInfo.SetBundleName(bundleName);

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string notificationContent(name);
    eventInfo.SetNotificationContent(notificationContent);
}

void CheckEventInfoFuzzTest(AccessibilityEventInfo &eventInfo)
{
    eventInfo.GetViewId();
    eventInfo.GetAccessibilityId();
    eventInfo.GetWindowId();
    eventInfo.GetCurrentIndex();
    eventInfo.GetBeginIndex();
    eventInfo.GetEndIndex();
    eventInfo.GetComponentType();
    eventInfo.GetBeforeText();
    eventInfo.GetContentList();
    eventInfo.GetLatestContent();
    eventInfo.GetDescription();
    eventInfo.GetItemCounts();
    eventInfo.GetWindowContentChangeTypes();
    eventInfo.GetWindowChangeTypes();
    eventInfo.GetTimeStamp();
    eventInfo.GetResourceId();
    eventInfo.GetResourceBundleName();
    eventInfo.GetResourceModuleName();
    eventInfo.GetResourceParams();
    eventInfo.GetBundleName();
    eventInfo.GetNotificationContent();
    eventInfo.GetTextAnnouncedForAccessibility();
    eventInfo.GetInspectorKey();
    eventInfo.GetTextMovementStep();
    eventInfo.GetTriggerAction();
    eventInfo.GetGestureType();
    eventInfo.GetNotificationInfo();
    eventInfo.GetPageId();
    eventInfo.GetElementInfo();
    eventInfo.GetRequestFocusElementId();
}

static size_t CreateEventInfo(AccessibilityEventInfo &eventInfo, const uint8_t* data, size_t size)
{
    size_t position = 0;
    CreateEventInfoFirstPart(eventInfo, data, size, position);
    CreateEventInfoSecondPart(eventInfo, data, size, position);
    CheckEventInfoFuzzTest(eventInfo);
    return position;
}
} // namespace Accessibility
} // namespace OHOS
#endif // ELEMENT_OPERATOR_FUZZ_H