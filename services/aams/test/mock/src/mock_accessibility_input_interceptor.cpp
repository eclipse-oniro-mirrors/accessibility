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

#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "input_event.h"
#include "key_event.h"
#include "power_mgr_client.h"

namespace OHOS {
namespace Accessibility {
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::instance_ = nullptr;
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::GetInstance()
{
    HILOG_DEBUG();

    if (!instance_) {
        instance_ = new AccessibilityInputInterceptor();
    }
    return instance_;
}

AccessibilityInputInterceptor::AccessibilityInputInterceptor()
{}

AccessibilityInputInterceptor::~AccessibilityInputInterceptor()
{}

bool AccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent& event)
{
    (void)event;
    return true;
}

void AccessibilityInputInterceptor::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    (void)offsetX;
    (void)offsetY;
}

bool AccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent& event)
{
    (void)event;
    return true;
}

void AccessibilityInputInterceptor::SetAvailableFunctions(uint32_t availableFunctions)
{
    availableFunctions_ = availableFunctions;
}

void AccessibilityInputInterceptor::CreateTransmitters()
{}

void AccessibilityInputInterceptor::UpdateInterceptor()
{
    inputManager_ = nullptr;
    interceptorId_ = 0;
}

void AccessibilityInputInterceptor::DestroyInterceptor()
{}

void AccessibilityInputInterceptor::DestroyTransmitters()
{}

void AccessibilityInputInterceptor::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event) const
{
    (void)event;
}

void AccessibilityInputInterceptor::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event) const
{
    (void)event;
}

void AccessibilityInputInterceptor::SetNextEventTransmitter(
    sptr<EventTransmission>& header, sptr<EventTransmission>& current, const sptr<EventTransmission>& next)
{
    (void)header;
    (void)current;
    (void)next;
}

AccessibilityInputEventConsumer::AccessibilityInputEventConsumer()
{}

AccessibilityInputEventConsumer::~AccessibilityInputEventConsumer()
{}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    (void)keyEvent;
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    (void)pointerEvent;
}
} // namespace Accessibility
} // namespace OHOS