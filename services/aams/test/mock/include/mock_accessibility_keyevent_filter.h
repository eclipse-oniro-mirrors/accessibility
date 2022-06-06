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

#ifndef MOCK_ACCESSIBILITY_KEYEVENT_FILTER_H
#define MOCK_ACCESSIBILITY_KEYEVENT_FILTER_H

#include <gmock/gmock.h>
#include "accessibility_keyevent_filter.h"

namespace OHOS {
namespace Accessibility {
class MockKeyEventFilter : public KeyEventFilter {
public:
    MockKeyEventFilter();
    virtual ~MockKeyEventFilter();

    MOCK_METHOD1(OnKeyEvent, void(MMI::KeyEvent& event));
    MOCK_METHOD1(SendEventToNext, void(MMI::KeyEvent& event));
    MOCK_METHOD3(SetServiceOnKeyEventResult,
        void(AccessibleAbilityConnection& connection, bool isHandled, uint32_t sequenceNum));
    MOCK_METHOD1(ClearServiceKeyEvents, void(AccessibleAbilityConnection& connection));

    MOCK_METHOD0(DestroyEvents, void());
    MOCK_METHOD1(RemoveProcessingEvent, bool(std::shared_ptr<ProcessingEvent> event));
};

class MockKeyEventFilterEventHandler : public KeyEventFilterEventHandler {
public:
    MockKeyEventFilterEventHandler(
        const std::shared_ptr<AppExecFwk::EventRunner>& runner, KeyEventFilter& keyEventFilter);
    virtual ~MockKeyEventFilterEventHandler() = default;

    MOCK_METHOD1(ProcessEvent, void(const AppExecFwk::InnerEvent::Pointer& event));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_KEYEVENT_FILTER_H