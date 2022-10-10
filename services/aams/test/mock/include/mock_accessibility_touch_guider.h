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

#ifndef MOCK_ACCESSIBILITY_TOUCH_GUIDER_H
#define MOCK_ACCESSIBILITY_TOUCH_GUIDER_H

#include <gmock/gmock.h>
#include "accessibility_touch_guider.h"

namespace OHOS {
namespace Accessibility {
class MockTGEventHandler : public TGEventHandler {
public:
    MockTGEventHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner, TouchGuider& tgServer);
    virtual ~MockTGEventHandler() = default;

    MOCK_METHOD1(ProcessEvent, void(const AppExecFwk::InnerEvent::Pointer& event));
};

class MockTouchGuider : public TouchGuider {
public:
    MockTouchGuider();
    ~MockTouchGuider()
    {}

    MOCK_METHOD0(StartUp, void());
    MOCK_METHOD1(OnPointerEvent, void(MMI::PointerEvent& event));
    MOCK_METHOD0(DestroyEvents, void());
    MOCK_METHOD2(SendEventToMultimodal, void(MMI::PointerEvent& event, int32_t action));
    MOCK_METHOD1(SendAccessibilityEventToAA, void(EventType eventType));
    MOCK_METHOD0(getHoverEnterAndMoveEvent, std::list<MMI::PointerEvent>());
    MOCK_METHOD0(ClearHoverEnterAndMoveEvent, void());
    MOCK_METHOD0(getLastReceivedEvent, std::shared_ptr<MMI::PointerEvent>());

    /* For TouchGuide */
    inline void OnTouchInteractionStart()
    {
        isTouchStart_ = true;
    }

    inline void OnTouchInteractionEnd()
    {
        isTouchStart_ = false;
    }
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_TOUCH_GUIDER_H