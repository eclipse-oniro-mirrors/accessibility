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
KeyEventFilter::KeyEventFilter()
{}

KeyEventFilter::~KeyEventFilter()
{}

bool KeyEventFilter::OnKeyEvent(MMI::KeyEvent& event)
{
    (void)event;
    return true;
}

void KeyEventFilter::SetServiceOnKeyEventResult(
    AccessibleAbilityConnection& connection, bool isHandled, uint32_t sequenceNum)
{
    AccessibilityAbilityHelper::GetInstance().SetTestSequence(sequenceNum);
    (void)connection;
    (void)isHandled;
}

void KeyEventFilter::ClearServiceKeyEvents(AccessibleAbilityConnection& connection)
{
    (void)connection;
}

void KeyEventFilter::DispatchKeyEvent(MMI::KeyEvent& event)
{
    (void)event;
    sequenceNum_ = 0;
}

bool KeyEventFilter::RemoveProcessingEvent(std::shared_ptr<ProcessingEvent> event)
{
    (void)event;
    return true;
}

std::shared_ptr<KeyEventFilter::ProcessingEvent> KeyEventFilter::FindProcessingEvent(
    AccessibleAbilityConnection& connection, uint32_t sequenceNum)
{
    (void)connection;
    (void)sequenceNum;
    return nullptr;
}

void KeyEventFilter::DestroyEvents()
{}

void KeyEventFilter::SendEventToNext(MMI::KeyEvent& event)
{
    (void)event;
}

KeyEventFilterEventHandler::KeyEventFilterEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner, KeyEventFilter& keyEventFilter)
    : AppExecFwk::EventHandler(runner), keyEventFilter_(keyEventFilter)
{
    (void)runner;
    (void)keyEventFilter;
    HILOG_DEBUG("KeyEventFilterEventHandler is created");
}

void KeyEventFilterEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    (void)event;
    auto processingEvent = event->GetSharedObject<KeyEventFilter::ProcessingEvent>();
    keyEventFilter_.RemoveProcessingEvent(processingEvent);
}
} // namespace Accessibility
} // namespace OHOS