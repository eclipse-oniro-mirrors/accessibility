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

#include "accessibility_mouse_autoclick.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t POINTER_COUNT_1 = 1;
    constexpr uint32_t AUTOCLICK_TIMEOUT_MSG = 1;
} // namespace

AccessibilityMouseAutoclick::AccessibilityMouseAutoclick()
{
    HILOG_DEBUG();

    std::shared_ptr<AppExecFwk::EventRunner> runner =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner) {
        HILOG_ERROR("get runner failed");
        return;
    }

    timeoutHandler_ = std::make_shared<MouseAutoclickEventHandler>(runner, *this);
    if (!timeoutHandler_) {
        HILOG_ERROR("create event handler failed");
    }
}

AccessibilityMouseAutoclick::~AccessibilityMouseAutoclick()
{
    HILOG_DEBUG();

    timeoutHandler_ = nullptr;
    lastMouseEvent_ = nullptr;
}

bool AccessibilityMouseAutoclick::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t source = event.GetSourceType();
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointers = event.GetPointerIds();
    size_t pointerCount = pointers.size();
    if ((source != MMI::PointerEvent::SOURCE_TYPE_MOUSE) ||
        (action != MMI::PointerEvent::POINTER_ACTION_MOVE) ||
        (pointerCount != POINTER_COUNT_1)) {
        CancelAutoclick();
    } else {
        RecognizeAutoclick(event);
    }

    EventTransmission::OnPointerEvent(event);
    return false;
}

void AccessibilityMouseAutoclick::SendMouseClickEvent()
{
    HILOG_DEBUG();

    if (!lastMouseEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }

    int64_t nowTime = GetSystemTime();
    // Update event information.
    lastMouseEvent_->SetActionTime(nowTime);
    lastMouseEvent_->SetActionStartTime(nowTime);
    lastMouseEvent_->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    lastMouseEvent_->SetButtonPressed(MMI::PointerEvent::MOUSE_BUTTON_LEFT);

    // Update pointer item information.
    int32_t pointerId = lastMouseEvent_->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    lastMouseEvent_->GetPointerItem(pointerId, item);
    item.SetDownTime(nowTime);
    item.SetPressed(true);
    lastMouseEvent_->UpdatePointerItem(pointerId, item);

    // Send mouse left button down event.
    lastMouseEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    EventTransmission::OnPointerEvent(*lastMouseEvent_);

    // Send mouse left button up event.
    lastMouseEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    EventTransmission::OnPointerEvent(*lastMouseEvent_);
}

void AccessibilityMouseAutoclick::ResetAutoclickInfo()
{
    HILOG_DEBUG();

    lastMouseEvent_ = nullptr;
}

void AccessibilityMouseAutoclick::DestroyEvents()
{
    HILOG_DEBUG();

    CancelAutoclick();
    EventTransmission::DestroyEvents();
}

void AccessibilityMouseAutoclick::RecognizeAutoclick(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    bool isMove = IsMouseMovement(event);
    if (!isMove) {
        HILOG_DEBUG("Mouse is not moving.");
        return;
    }

    lastMouseEvent_ = std::make_shared<MMI::PointerEvent>(event);
    if (!timeoutHandler_) {
        HILOG_ERROR("handler is null.");
        return;
    }
    int64_t delayTime = GetDelayTime();
    timeoutHandler_->RemoveEvent(AUTOCLICK_TIMEOUT_MSG);
    timeoutHandler_->SendEvent(AUTOCLICK_TIMEOUT_MSG, 0, delayTime);
}

bool AccessibilityMouseAutoclick::IsMouseMovement(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (!lastMouseEvent_) {
        return true;
    }

    int32_t pointerId = event.GetPointerId();
    MMI::PointerEvent::PointerItem item;
    event.GetPointerItem(pointerId, item);
    int32_t newX = item.GetDisplayX();
    int32_t newY = item.GetDisplayY();

    pointerId = lastMouseEvent_->GetPointerId();
    lastMouseEvent_->GetPointerItem(pointerId, item);
    int32_t oldX = item.GetDisplayX();
    int32_t oldY = item.GetDisplayY();
    if ((newX != oldX) || (newY != oldY)) {
        HILOG_DEBUG("Mouse is moving.");
        return true;
    }
    return false;
}

void AccessibilityMouseAutoclick::CancelAutoclick()
{
    HILOG_DEBUG();

    ResetAutoclickInfo();
    timeoutHandler_->RemoveEvent(AUTOCLICK_TIMEOUT_MSG);
}

int64_t AccessibilityMouseAutoclick::GetSystemTime()
{
    HILOG_DEBUG();

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}

int64_t AccessibilityMouseAutoclick::GetDelayTime()
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return 0;
    }

    int32_t delayTime = accountData->GetConfig()->GetMouseAutoClick();
    return delayTime;
}

AccessibilityMouseAutoclick::MouseAutoclickEventHandler::MouseAutoclickEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityMouseAutoclick &mouseAutoclick)
    : AppExecFwk::EventHandler(runner), mouseAutoclick_(mouseAutoclick)
{
    HILOG_DEBUG();
}

void AccessibilityMouseAutoclick::MouseAutoclickEventHandler::ProcessEvent(
    const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    switch (event->GetInnerEventId()) {
        case AUTOCLICK_TIMEOUT_MSG:
            mouseAutoclick_.SendMouseClickEvent();
            mouseAutoclick_.ResetAutoclickInfo();
            break;
        default:
            break;
    }
}
} // namespace Accessibility
} // namespace OHOS