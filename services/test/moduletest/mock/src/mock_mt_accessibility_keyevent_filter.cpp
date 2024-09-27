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

#include "accessibility_keyevent_filter.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
MockKeyEventFilter::KeyEventFilter()
{}

MockKeyEventFilter::~KeyEventFilter()
{}

bool MockKeyEventFilter::OnKeyEvent(MMI::KeyEvent& event)
{
    (void)event;
    return true;
}

void MockKeyEventFilter::SetServiceOnKeyEventResult(
    AccessibleAbilityConnection& connection, bool isHandled, uint32_t sequenceNum)
{
    AccessibilityAbilityHelper::GetInstance().SetTestSequence(sequenceNum);
    (void)connection;
    (void)isHandled;
}

void MockKeyEventFilter::ClearServiceKeyEvents(AccessibleAbilityConnection& connection)
{
    (void)connection;
}

void MockKeyEventFilter::DispatchKeyEvent(MMI::KeyEvent& event)
{
    (void)event;
    sequenceNum_ = 0;
}

bool MockKeyEventFilter::RemoveProcessingEvent(std::shared_ptr<ProcessingEvent> event)
{
    (void)event;
    return true;
}

std::shared_ptr<MockKeyEventFilter::ProcessingEvent> MockKeyEventFilter::FindProcessingEvent(
    AccessibleAbilityConnection& connection, uint32_t sequenceNum)
{
    (void)connection;
    (void)sequenceNum;
    return nullptr;
}

void MockKeyEventFilter::DestroyEvents()
{}

void MockKeyEventFilter::SendEventToNext(MMI::KeyEvent& event)
{
    (void)event;
}

MockKeyEventFilterEventHandler::MockKeyEventFilterEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner, MockKeyEventFilter& MockKeyEventFilter)
    : AppExecFwk::EventHandler(runner), MockKeyEventFilter_(MockKeyEventFilter)
{
    (void)runner;
    (void)MockKeyEventFilter;
    HILOG_DEBUG("MockKeyEventFilterEventHandler is created");
}

void MockKeyEventFilterEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    (void)event;
    auto processingEvent = event->GetSharedObject<MockKeyEventFilter::ProcessingEvent>();
    MockKeyEventFilter_.RemoveProcessingEvent(processingEvent);
}
} // namespace Accessibility
} // namespace OHOS