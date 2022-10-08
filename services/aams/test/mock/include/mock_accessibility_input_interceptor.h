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

#ifndef MOCK_ACCESSIBILITY_INPUT_INTERCEPTOR_H
#define MOCK_ACCESSIBILITY_INPUT_INTERCEPTOR_H

#include <gmock/gmock.h>
#include "accessibility_input_interceptor.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityInputEventConsumer : public AccessibilityInputEventConsumer {
public:
    MockAccessibilityInputEventConsumer();
    ~MockAccessibilityInputEventConsumer();

    MOCK_CONST_METHOD1(OnInputEvent, void(std::shared_ptr<MMI::KeyEvent> keyEvent));
    MOCK_CONST_METHOD1(OnInputEvent, void(std::shared_ptr<MMI::PointerEvent> pointerEvent));
    MOCK_CONST_METHOD1(OnInputEvent, void(std::shared_ptr<MMI::AxisEvent> axisEvent));
};

class MockAccessibilityInputInterceptor : public AccessibilityInputInterceptor {
public:
    MOCK_METHOD0(GetInstance, sptr<AccessibilityInputInterceptor>());
    MOCK_METHOD1(ProcessKeyEvent, void(std::shared_ptr<MMI::KeyEvent> event));
    MOCK_METHOD1(ProcessPointerEvent, void(std::shared_ptr<MMI::PointerEvent> event));
    MOCK_METHOD1(OnKeyEvent, void(MMI::KeyEvent& event));
    MOCK_METHOD1(OnPointerEvent, void(MMI::PointerEvent& event));
    MOCK_METHOD1(SetAvailableFunctions, void(uint32_t availableFunctions));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_INPUT_INTERCEPTOR_H