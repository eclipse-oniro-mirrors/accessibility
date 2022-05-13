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

#ifndef MOCK_ACCESSIBILITY_COMMON_EVENT_REGISTRY_H
#define MOCK_ACCESSIBILITY_COMMON_EVENT_REGISTRY_H

#include <gmock/gmock.h>
#include "accessibility_common_event.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityCommonEventSubscriber : public AccessibilityCommonEventSubscriber {
public:
    MockAccessibilityCommonEventSubscriber(
        const EventFwk::CommonEventSubscribeInfo& subscriberInfo, const std::map<std::string, EventHandle>& handles);
    ~MockAccessibilityCommonEventSubscriber();
    MOCK_METHOD1(OnReceiveEvent, void(const EventFwk::CommonEventData& data));
};

class MockAccessibilityCommonEvent : public AccessibilityCommonEvent {
public:
    MockAccessibilityCommonEvent();
    ~MockAccessibilityCommonEvent();
    MOCK_METHOD0(StartRegister, bool());
    MOCK_METHOD0(UnRegister, void());
} // namespace Accessibility
} // namespace Accessibility
#endif // MOCK_ACCESSIBILITY_COMMON_EVENT_REGISTRY_H