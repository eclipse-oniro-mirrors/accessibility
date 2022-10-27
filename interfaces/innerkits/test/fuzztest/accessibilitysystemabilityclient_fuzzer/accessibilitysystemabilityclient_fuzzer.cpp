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
#include "accessibility_system_ability_client_impl.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}
static AccessibilitySystemAbilityClientImpl g_asacImpl_;

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

    GetObject<int32_t>(windowId, &data[position], size - position);
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
    GetObject<AbilityStateType>(stateType, &data[position], size - position);
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

    GetObject<uint32_t>(eventTypes, &data[position], size - position);
    instance->UnsubscribeStateObserver(observer, eventTypes);
    return true;
}

bool IsEnabledFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    bool isEnabled = false;
    GetObject<bool>(isEnabled, &data[0], size);
    instance->IsEnabled(isEnabled);
    return true;
}

bool IsTouchExplorationEnabledFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    bool isEnabled = false;
    GetObject<bool>(isEnabled, &data[0], size);
    instance->IsTouchExplorationEnabled(isEnabled);
    return true;
}

bool GetEnabledAbilitiesFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string enabledAbility(name);
    std::vector<std::string> enabledAbilities;
    enabledAbilities.emplace_back(enabledAbility);
    instance->GetEnabledAbilities(enabledAbilities);
    return true;
}

bool SetSearchElementInfoByAccessibilityIdResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    std::list<AccessibilityElementInfo> infos;
    g_asacImpl_.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    return true;
}

bool SetSearchElementInfoByTextResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    std::list<AccessibilityElementInfo> infos;
    g_asacImpl_.SetSearchElementInfoByTextResult(infos, requestId);
    return true;
}

bool SetFindFocusedElementInfoResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    AccessibilityElementInfo info;
    g_asacImpl_.SetFindFocusedElementInfoResult(info, requestId);
    return true;
}

bool SetFocusMoveSearchResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    AccessibilityElementInfo info;
    g_asacImpl_.SetFocusMoveSearchResult(info, requestId);
    return true;
}

bool SetExecuteActionResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    bool succeeded = false;
    int32_t requestId = 0;
    position += GetObject<bool>(succeeded, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    g_asacImpl_.SetExecuteActionResult(succeeded, requestId);
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
    OHOS::Accessibility::IsEnabledFuzzTest(data, size);
    OHOS::Accessibility::IsTouchExplorationEnabledFuzzTest(data, size);
    OHOS::Accessibility::GetEnabledAbilitiesFuzzTest(data, size);
    OHOS::Accessibility::SetSearchElementInfoByAccessibilityIdResultFuzzTest(data, size);
    OHOS::Accessibility::SetSearchElementInfoByTextResultFuzzTest(data, size);
    OHOS::Accessibility::SetFindFocusedElementInfoResultFuzzTest(data, size);
    OHOS::Accessibility::SetFocusMoveSearchResultFuzzTest(data, size);
    OHOS::Accessibility::SetExecuteActionResultFuzzTest(data, size);
    return 0;
}