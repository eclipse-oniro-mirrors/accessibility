/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef MOCK_ACCESSIBLE_ABILITY_LISTENER_H
#define MOCK_ACCESSIBLE_ABILITY_LISTENER_H

#include "accessibility_event_info.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace MMI {
class KeyEvent;
}
namespace Accessibility {
class MockAccessibleAbilityListener : public AccessibleAbilityListener {
public:
    virtual ~MockAccessibleAbilityListener() = default;
    MOCK_METHOD0(OnAbilityConnected, void());
    MOCK_METHOD0(OnAbilityDisconnected, void());
    MOCK_METHOD1(OnAccessibilityEvent, void(const AccessibilityEventInfo& eventInfo));
    MOCK_METHOD1(OnKeyPressEvent, bool(const std::shared_ptr<MMI::KeyEvent>& keyEvent));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_LISTENER_H