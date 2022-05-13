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

#ifndef MOCK_ACCESSIBILITY_DISPLAY_MANAGER_H
#define MOCK_ACCESSIBILITY_DISPLAY_MANAGER_H

#include <gmock/gmock.h>
#include "accessibility_display_manager.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityDisplayManager : public AccessibilityDisplayManager {
public:
    ~MockAccessibilityDisplayManager() = default;

    MOCK_METHOD0(GetInstance, AccessibilityDisplayManager&());
    MOCK_METHOD1(GetDisplay, const sptr<Rosen::Display>(int32_t id));
    MOCK_METHOD0(GetDisplays, std::vector<sptr<Rosen::Display>>());
    MOCK_METHOD0(GetDefaultDisplay, const sptr<Rosen::Display>());
    MOCK_METHOD1(RegisterDisplayListener, void(const std::shared_ptr<AppExecFwk::EventHandler> &handler));
    MOCK_METHOD0(UnregisterDisplayListener, void());
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_DISPLAY_MANAGER_H