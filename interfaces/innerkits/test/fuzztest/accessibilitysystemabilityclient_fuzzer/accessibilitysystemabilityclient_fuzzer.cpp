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

#include "accessibilitysystemabilityclient_fuzzer.h"
#include "accessibility_system_ability_client.h"
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
    (void)memcpy_s(&object, objectSize, data, size);
    return objectSize;
}

class ElementOperatorForFuzzTest : public AccessibilityElementOperator {
public:
    virtual ~ElementOperatorForFuzzTest() = default;
    void SearchElementInfoByAccessibilityId(const int32_t elementId,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback, const int32_t mode) override {}

    void SearchElementInfosByText(const int32_t elementId, const std::string &text,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) override {}

    void FindFocusedElementInfo(const int32_t elementId, const int32_t focusType, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}

    void FocusMoveSearch(const int32_t elementId, const int32_t direction, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) override {}

    void ExecuteAction(const int32_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) override {}
    void ClearFocus() override {}
    void OutsideTouch() override {}
};

class StateObserverForFuzzTest : public AccessibilityStateObserver {
public:
    virtual ~StateObserverForFuzzTest() = default;
    void OnStateChanged(const bool state) {}
};

static size_t CreateEventInfo(AccessibilityEventInfo &eventInfo, const uint8_t* data, size_t size)
{
    size_t position = 0;
    int32_t channelId = 0;
    position += GetObject<int32_t>(channelId, &data[position], size - position);
    eventInfo.SetChannelId(channelId);
    int32_t componentId = 0;
    position += GetObject<int32_t>(componentId, &data[position], size - position);
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

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string className(name);
    eventInfo.SetComponentType(className);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string beforeText(name);
    eventInfo.SetBeforeText(beforeText);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string content(name);
    eventInfo.AddContent(content);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string lastContent(name);
    eventInfo.SetLatestContent(lastContent);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string contentDescription(name);
    eventInfo.SetDescription(contentDescription);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string bundleName(name);
    eventInfo.SetBundleName(bundleName);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string notificationContent(name);
    eventInfo.SetNotificationContent(notificationContent);
    position += LEN;

    return position;
}

bool RegisterElementOperatorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    int32_t windowId = 0;
    position += GetObject<int32_t>(windowId, &data[position], size - position);
    std::shared_ptr<ElementOperatorForFuzzTest> elementOperator = std::make_shared<ElementOperatorForFuzzTest>();
    instance->RegisterElementOperator(windowId, elementOperator);

    position += GetObject<int32_t>(windowId, &data[position], size - position);
    instance->DeregisterElementOperator(windowId);
    return true;
}

bool GetAbilityListFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    uint32_t abilityTypes = 0;
    position += GetObject<uint32_t>(abilityTypes, &data[position], size - position);
    AbilityStateType stateType;
    position += GetObject<AbilityStateType>(stateType, &data[position], size - position);
    std::vector<AccessibilityAbilityInfo> infos;
    instance->GetAbilityList(abilityTypes, stateType, infos);
    return true;
}

bool SendEventFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    EventType eventType;
    position += GetObject<EventType>(eventType, &data[position], size - position);
    int32_t componentId = 0;
    position += GetObject<int32_t>(componentId, &data[position], size - position);
    instance->SendEvent(eventType, componentId);

    AccessibilityEventInfo eventInfo;
    (void)CreateEventInfo(eventInfo, data, size - position);
    instance->SendEvent(eventInfo);
    return true;
}

bool SubscribeStateObserverFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    uint32_t eventTypes = 0;
    position += GetObject<uint32_t>(eventTypes, &data[position], size - position);
    std::shared_ptr<StateObserverForFuzzTest> observer = std::make_shared<StateObserverForFuzzTest>();
    instance->SubscribeStateObserver(observer, eventTypes);

    position += GetObject<uint32_t>(eventTypes, &data[position], size - position);
    instance->UnsubscribeStateObserver(observer, eventTypes);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::RegisterElementOperatorFuzzTest(data, size);
    OHOS::Accessibility::GetAbilityListFuzzTest(data, size);
    OHOS::Accessibility::SendEventFuzzTest(data, size);
    OHOS::Accessibility::SubscribeStateObserverFuzzTest(data, size);
    return 0;
}