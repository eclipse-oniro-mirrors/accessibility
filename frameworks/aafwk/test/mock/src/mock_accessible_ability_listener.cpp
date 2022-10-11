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

#include "mock_accessible_ability_listener.h"
#include "accessibility_ut_helper.h"
#include "key_event.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t CHANNEL_ID = 1;
} // namespace

void MockAccessibleAbilityListener::OnAbilityConnected()
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(CHANNEL_ID);
}

void MockAccessibleAbilityListener::OnAbilityDisconnected()
{
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(INVALID_CHANNEL_ID);
}

void MockAccessibleAbilityListener::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    AccessibilityAbilityHelper::GetInstance().SetTestEventType(static_cast<EventType>(eventInfo.GetEventType()));
}

bool MockAccessibleAbilityListener::OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    AccessibilityAbilityHelper::GetInstance().SetTestKeyPressEvent(static_cast<int>(keyEvent->GetKeyCode()));
    return true;
}
} // namespace Accessibility
} // namespace OHOS