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

#ifndef MOCK_ACCESSIBILITY_EVENT_TRANSMISSION_H
#define MOCK_ACCESSIBILITY_EVENT_TRANSMISSION_H

#include <gmock/gmock.h>
#include "accessibility_event_transmission.h.h"

namespace OHOS {
namespace Accessibility {
class MockEventTransmission : public EventTransmission {
public:
    MockEventTransmission() {}
    virtual ~MockEventTransmission() {}

    MOCK_METHOD1(OnKeyEvent, void(MMI::KeyEvent& event));
    MOCK_METHOD1(OnPointerEvent, void(MMI::KeyEvent& event));
    MOCK_METHOD1(SetNext, void(const sptr<EventTransmission>& next));
    MOCK_METHOD0(GetNext, sptr<EventTransmission>());
    MOCK_METHOD0(DestroyEvents, void());
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_EVENT_TRANSMISSION_H