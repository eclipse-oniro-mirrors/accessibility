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
namespace Accessibility{
std::vector<int> TouchAction;
int UTtouchAction = -1;
bool isMouseEvent = false; 
bool isClearEvents = false;
bool isDestroyEvents = false;

void EventTransmission::OnTouchEvent(TouchEvent &event)
{
    HILOG_DEBUG();
    UTtouchAction = event.GetAction();
    TouchAction.push_back(UTtouchAction);
    auto next = GetNext();
    if (next != nullptr) {
        next->OnTouchEvent(event);
    }
}

void EventTransmission::OnMouseEvent(MouseEvent &event)
{
    HILOG_DEBUG();
    isMouseEvent = true;

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
    isClearEvents = true;
}

void EventTransmission::DestroyEvents()
{
    isDestroyEvents = true;
}

} // namespace Accessibility
} // namespace OHOS
