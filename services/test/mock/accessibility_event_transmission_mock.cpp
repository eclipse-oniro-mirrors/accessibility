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

#include "accessibility_event_transmission.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
std::vector<int> g_touchAction;
int g_uTtouchAction = -1;
bool g_isMouseEvent = false;
bool g_isClearEvents = false;
bool g_isDestroyEvents = false;

void EventTransmission::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    g_uTtouchAction = event.GetAction();
    g_touchAction.push_back(g_uTtouchAction);
    auto next = GetNext();
    if (next != nullptr) {
        next->OnPointerEvent(event);
    }
}

void EventTransmission::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();
}
void EventTransmission::OnAccessibilityEvent(AccessibilityEventInfo &event)
{
    HILOG_DEBUG();
}

void EventTransmission::SetNext(const sptr<EventTransmission> &next)
{
    HILOG_DEBUG();

    next_ = next;
}

sptr<EventTransmission> EventTransmission::GetNext()
{
    HILOG_DEBUG();

    return next_;
}

void EventTransmission::ClearEvents(uint32_t inputSource)
{
    g_isClearEvents = true;
}

void EventTransmission::DestroyEvents()
{
    g_isDestroyEvents = true;
}
} // namespace Accessibility
} // namespace OHOS
