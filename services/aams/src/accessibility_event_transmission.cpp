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

#include "accessibility_event_transmission.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
bool EventTransmission::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        return next->OnKeyEvent(event);
    }
    return false;
}

bool EventTransmission::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        return next->OnPointerEvent(event);
    }
    return false;
}

void EventTransmission::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        next->OnMoveMouse(offsetX, offsetY);
    }
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

void EventTransmission::DestroyEvents()
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        next->DestroyEvents();
    }
}
} // namespace Accessibility
} // namespace OHOS
