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

#ifndef MOCK_ACCESSIBILITY_ZOOM_HANDLER_H
#define MOCK_ACCESSIBILITY_ZOOM_HANDLER_H

#include <gmock/gmock.h>
#include "accessibility_zoom_handler.h"

namespace OHOS {
namespace Accessibility {
class MockZoomObserver : public ZoomObserver {
public:
    MockZoomObserver()
    {}
    virtual ~MockZoomObserver()
    {}
    MOCK_METHOD1(OnTransitionTo, void(const int32_t state));
    MOCK_METHOD1(OnBack, void(MMI::PointerEvent& event));
    MOCK_METHOD0(OnZoomIn, void());
    MOCK_METHOD0(OnZoomOut, void());
};

class MockZoomState : public ZoomState {
public:
    MOCK_METHOD0(Enter, void());
    MOCK_METHOD0(Exit, void());
    MOCK_METHOD1(OnPointerEvent, void(MMI::PointerEvent& event));

public:
    MOCK_METHOD1(Register, void(ZoomObserver& observer));
    MOCK_METHOD0(UnRegisterAll, void());

private:
};

class MockAccessibilityZoomHandler : public AccessibilityZoomHandler {
public:
    explicit MockAccessibilityZoomHandler(int32_t displayId);
    virtual ~MockAccessibilityZoomHandler();

    MOCK_METHOD1(OnPointerEvent, void(MMI::PointerEvent& event));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ZOOM_HANDLER_H