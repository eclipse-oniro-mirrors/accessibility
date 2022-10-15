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
std::shared_ptr<AccessibilityInterceptorManager> AccessibilityInterceptorManager::instance_ = nullptr;
std::shared_ptr<AccessibilityInterceptorManager> AccessibilityInterceptorManager::GetInstance()
{
    if (!instance_) {
        instance_ = std::make_shared<AccessibilityInterceptorManager>();
    }
    return instance_;
}

AccessibilityInterceptorManager::AccessibilityInterceptorManager()
{}

AccessibilityInterceptorManager::~AccessibilityInterceptorManager()
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

void AccessibilityInterceptorManager::SetAvailableFunctions(uint32_t availableFunctions)
{
    availableFunctions_ = availableFunctions;
}

void AccessibilityInterceptorManager::CreateTransmitters()
{}

void AccessibilityInterceptorManager::UpdateInterceptor()
{
    interceptorId_ = 0;
}

void AccessibilityInterceptorManager::DestroyInterceptor()
{}

void AccessibilityInterceptorManager::DestroyTransmitters()
{}

void AccessibilityInterceptorManager::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event)
{
    (void)event;
}

void AccessibilityInterceptorManager::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event)
{
    (void)event;
}

void AccessibilityInterceptorManager::SetNextEventTransmitter(
    std::shared_ptr<EventTransmission>& header, std::shared_ptr<EventTransmission>& current,
    const std::shared_ptr<EventTransmission>& next)
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