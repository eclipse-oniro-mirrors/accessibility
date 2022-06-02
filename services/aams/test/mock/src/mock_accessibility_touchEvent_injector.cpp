/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessibility_touchEvent_injector.h"
#include "accessible_ability_manager_service.h"

namespace OHOS {
namespace Accessibility {
TouchInjectHandler::TouchInjectHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner, TouchEventInjector& server)
    : AppExecFwk::EventHandler(runner), server_(server)
{
    (void)runner;
    (void)server;
}

void TouchInjectHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    (void)event;
    server_.InjectGesturePathInner();
}

TouchEventInjector::TouchEventInjector()
{}

void TouchEventInjector::OnPointerEvent(MMI::PointerEvent& event)
{
    (void)event;
}

void TouchEventInjector::ClearEvents(uint32_t inputSource)
{
    (void)inputSource;
    isGestureUnderway_ = false;
}

void TouchEventInjector::DestroyEvents()
{
    isDestroyEvent_ = true;
}

void TouchEventInjector::SendPointerEvent(MMI::PointerEvent& event)
{
    (void)event;
}

void TouchEventInjector::CancelGesture()
{}

void TouchEventInjector::CancelInjectedEvents()
{}

std::shared_ptr<MMI::PointerEvent> TouchEventInjector::obtainTouchEvent(
    int32_t action, MMI::PointerEvent::PointerItem point, int64_t actionTime)
{
    (void)action;
    (void)point;
    (void)actionTime;
    return nullptr;
}

int64_t TouchEventInjector::GetSystemTime()
{
    return 0;
}
} // namespace Accessibility
} // namespace OHOS