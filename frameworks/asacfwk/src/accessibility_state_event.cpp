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

#include "accessibility_state_event.h"

namespace OHOS {
namespace Accessibility {
AccessibilityStateEvent::AccessibilityStateEvent()
{
}

AccessibilityStateEventType AccessibilityStateEvent::GetEventType() const
{
    return eventType_;
}

int AccessibilityStateEvent::GetEventResult() const
{
    return enabled_;
}

std::string AccessibilityStateEvent::GetEventMsg() const
{
    return describeEvent_;
}

void AccessibilityStateEvent::SetEventType(const AccessibilityStateEventType eventType)
{
    eventType_ = *(const_cast<AccessibilityStateEventType*>(&eventType));
}

void AccessibilityStateEvent::SetEventResult(const int enabled)
{
    enabled_ = enabled;
}

void AccessibilityStateEvent::SetEventMsg(std::string &description)
{
    describeEvent_ = description;
}
} // namespace Accessibility
} // namespace OHOS