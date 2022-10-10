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

#ifndef MOCK_ACCESSIBILITY_GESTURE_RECOGNIZER_H
#define MOCK_ACCESSIBILITY_GESTURE_RECOGNIZER_H

#include <gmock/gmock.h>
#include "accessibility_gesture_recognizer.h"

namespace OHOS {
namespace Accessibility {
class MockGestureHandler : public GestureHandler {
public:
    MockGestureHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner, AccessibilityGestureRecognizer& server);
    virtual ~MockGestureHandler() = default;

    MOCK_METHOD1(ProcessEvent, void(const AppExecFwk::InnerEvent::Pointer& event));
};

class MockAccessibilityGestureRecognizeListener : public AccessibilityGestureRecognizeListener {
public:
    virtual ~MockAccessibilityGestureRecognizeListener() = default;

    MOCK_METHOD1(OnDoubleTap, bool(MMI::PointerEvent& event));
    MOCK_METHOD0(OnStarted, bool());
    MOCK_METHOD1(OnCompleted, bool(GestureType gestureId));
    MOCK_METHOD1(OnCancelled, bool(MMI::PointerEvent& event));
};

class MockAccessibilityGestureRecognizer : public AccessibilityGestureRecognizer {
public:
    MockAccessibilityGestureRecognizer();
    ~MockAccessibilityGestureRecognizer()
    {}

    MOCK_METHOD1(RegisterListener, void(AccessibilityGestureRecognizeListener& listener));
    MOCK_METHOD0(UnregisterListener, void());
    MOCK_METHOD0(IsfirstTap, bool());
    MOCK_METHOD1(OnPointerEvent, bool(MMI::PointerEvent& event));
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(MaybeRecognizeLongPress, void(MMI::PointerEvent& event));
    MOCK_METHOD0(SingleTapDetected, void());
    MOCK_METHOD1(SetIsLongpress, void(bool value));
    MOCK_METHOD0(GetCurDown, std::shared_ptr<MMI::PointerEvent>());
    MOCK_METHOD0(GetContinueDown, bool());
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_GESTURE_RECOGNIZER_H