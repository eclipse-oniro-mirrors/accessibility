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
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t MS_TO_US = 1000;
    constexpr int32_t MOVE_GESTURE_MIN_PATH_COUNT = 2;
} // namespace

TouchInjectHandler::TouchInjectHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    TouchEventInjector &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

void TouchInjectHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<SendEventArgs> parameters = nullptr;
    if (!event) {
        HILOG_ERROR("event is nullptr");
        return;
    }
    switch (event->GetInnerEventId()) {
        case TouchEventInjector::SEND_TOUCH_EVENT_MSG:
            parameters = event->GetSharedObject<SendEventArgs>();
            server_.SendPointerEvent(*parameters->event_);
            if (parameters->isLastEvent_) {
                if (server_.GetCurrentGestureService()) {
                    server_.GetCurrentGestureService()->OnGestureInjectResult(server_.GetSequence(), true);
                }
            }
            break;
        case TouchEventInjector::GESTURE_INJECT_EVENT_MSG:
            server_.InjectGesturePathInner();
            break;
        default:
            break;
    }
}

TouchEventInjector::TouchEventInjector()
{
    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }
    handler_ = std::make_shared<TouchInjectHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

void TouchEventInjector::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("TouchEventInjector::OnPointerEvent: start");

    EventTransmission::OnPointerEvent(event);
}

void TouchEventInjector::ClearEvents(uint32_t inputSource)
{
    if (!handler_->HasInnerEvent(SEND_TOUCH_EVENT_MSG)) {
        isGestureUnderway_ = false;
    }
    EventTransmission::ClearEvents(inputSource);
}

void TouchEventInjector::DestroyEvents()
{
    HILOG_INFO("TouchEventInjector::DestroyEvents: start");
    CancelInjectedEvents();
    isDestroyEvent_ = true;
    EventTransmission::DestroyEvents();
}

void TouchEventInjector::SendPointerEvent(MMI::PointerEvent &event)
{
    HILOG_INFO("TouchEventInjector::SendPointerEvent: start");
    if (GetNext() != nullptr) {
        EventTransmission::OnPointerEvent(event);
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
            isGestureUnderway_ = true;
        }
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
            isGestureUnderway_ = false;
        }
    }
}

void TouchEventInjector::CancelGesture()
{
    HILOG_INFO("TouchEventInjector::CancelGesture: start");
    std::shared_ptr<MMI::PointerEvent> event;
    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    int64_t time = GetSystemTime();
    pointer.SetDownTime(time);
    pointer.SetPointerId(1);
    if (GetNext() != nullptr && isGestureUnderway_) {
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_CANCEL, pointer, time);
        SendPointerEvent(*event);
        isGestureUnderway_ = false;
    }
}

void TouchEventInjector::CancelInjectedEvents()
{
    HILOG_INFO("TouchEventInjector::CancelInjectedEvents: start");
    if (handler_->HasInnerEvent(SEND_TOUCH_EVENT_MSG)) {
        handler_->RemoveEvent(SEND_TOUCH_EVENT_MSG);
        CancelGesture();
        currentGestureService_->OnGestureInjectResult(sequence_, false);
    }
}

std::shared_ptr<MMI::PointerEvent> TouchEventInjector::obtainTouchEvent(int32_t action,
    MMI::PointerEvent::PointerItem point, int64_t actionTime)
{
    HILOG_INFO("TouchEventInjector::obtainTouchEvent: start");
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetPointerId(point.GetPointerId());
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionTime(actionTime);
    pointerEvent->SetActionStartTime(point.GetDownTime());
    pointerEvent->AddPointerItem(point);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

int64_t TouchEventInjector::GetSystemTime()
{
    HILOG_INFO("TouchEventInjector::GetSystemTime: start");

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}

void TouchEventInjector::InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath,
    const sptr<IAccessibleAbilityClient> &service, int32_t sequence)
{
    sequence_ = sequence;
    currentGestureService_ = service;
    gesturePositions_ = gesturePath;
    handler_->SendEvent(GESTURE_INJECT_EVENT_MSG, 0, 0);
}

void TouchEventInjector::InjectGesturePathInner()
{
    HILOG_INFO("TouchEventInjector::InjectGesturePathInner: start");

    int64_t curTime = GetSystemTime();
    if (isDestroyEvent_ || !GetNext()) {
        currentGestureService_->OnGestureInjectResult(sequence_, false);
        return;
    }
    CancelInjectedEvents();
    CancelGesture();

    ParseTouchEventsFromGesturePath(curTime);

    if (injectedEvents_.empty()) {
        currentGestureService_->OnGestureInjectResult(sequence_, false);
        return;
    }
    for (size_t i = 0; i < injectedEvents_.size(); i++) {
        std::shared_ptr<SendEventArgs> parameters = std::make_shared<SendEventArgs>();
        parameters->isLastEvent_ = (i == injectedEvents_.size() - 1) ? true : false;
        parameters->event_ = injectedEvents_[i];
        if (injectedEvents_[i]) {
            int64_t timeout = (injectedEvents_[i]->GetActionTime() - curTime) / MS_TO_US;
            if (timeout < 0) {
                HILOG_INFO("timeout is error.%{public}jd", timeout);
            } else {
                handler_->SendEvent(SEND_TOUCH_EVENT_MSG, parameters, timeout);
            }
        }
    }
    injectedEvents_.clear();
}

void TouchEventInjector::ParseTapsEvents(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::ParseTapsEvents: start");

    const std::vector<AccessibilityGesturePosition> &positions = gesturePositions_->GetPositions();
    size_t positionSize = positions.size();
    if (!positionSize) {
        HILOG_WARN("PositionSize is zero.");
        return;
    }
     int64_t durationTime = gesturePositions_->GetDurationTime();
    if (durationTime < 0) {
        HILOG_WARN("DurationTime is wrong.");
        return;
    }
    int64_t perDurationTime = static_cast<int64_t>(static_cast<uint64_t>(durationTime) / positionSize);
    int64_t downTime = startTime;
    int64_t nowTime = startTime;
    for (size_t i = 0; i < positionSize; i++) {
        std::shared_ptr<MMI::PointerEvent> event;
        MMI::PointerEvent::PointerItem pointer = {};
        pointer.SetPointerId(1);
        // Append down event
        int32_t px = static_cast<int32_t>(positions[i].positionX_);
        int32_t py = static_cast<int32_t>(positions[i].positionY_);
        pointer.SetGlobalX(px);
        pointer.SetGlobalY(py);
        pointer.SetDownTime(downTime);
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
        HILOG_INFO("append down event");
        injectedEvents_.push_back(event);

        // Append up event
        nowTime += perDurationTime * MS_TO_US;
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, nowTime);
        HILOG_INFO("append up event");
        injectedEvents_.push_back(event);
        nowTime += DOUBLE_TAP_MIN_TIME;
        downTime += DOUBLE_TAP_MIN_TIME;
    }
}

void TouchEventInjector::ParseMovesEvents(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::ParseMovesEvents: start");

    std::vector<AccessibilityGesturePosition> positions = gesturePositions_->GetPositions();
    size_t positionSize = positions.size();
    if (positionSize < MOVE_GESTURE_MIN_PATH_COUNT) {
        HILOG_WARN("PositionSize is zero.");
        return;
    }
    int64_t durationTime = gesturePositions_->GetDurationTime();
    if (durationTime < 0) {
        HILOG_WARN("DurationTime is wrong.");
        return;
    }
    int64_t perDurationTime = static_cast<int64_t>(static_cast<uint64_t>(durationTime) / (positionSize - 1));
    int64_t downTime = startTime;
    int64_t nowTime = startTime;
    for (size_t i = 0; i < positionSize; i++) {
        std::shared_ptr<MMI::PointerEvent> event;
        MMI::PointerEvent::PointerItem pointer = {};
        int32_t px = static_cast<int32_t>(positions[i].positionX_);
        int32_t py = static_cast<int32_t>(positions[i].positionY_);
        pointer.SetPointerId(1);
        pointer.SetGlobalX(px);
        pointer.SetGlobalY(py);
        pointer.SetDownTime(downTime);
        if (i == 0) { // Append down event
            HILOG_INFO("append down event");
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
            injectedEvents_.push_back(event);
        } else if (i < (positionSize - 1)) { // Append move event
            HILOG_INFO("append move event");
            nowTime += perDurationTime * MS_TO_US;
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, pointer, nowTime);
            injectedEvents_.push_back(event);
        } else { // Append up event
            HILOG_INFO("append move event");
            nowTime += perDurationTime * MS_TO_US;
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, pointer, nowTime);
            injectedEvents_.push_back(event);
            
            HILOG_INFO("append up event");
            event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, nowTime);
            injectedEvents_.push_back(event);
        }
    }
}

void TouchEventInjector::ParseTouchEventsFromGesturePath(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::GetTouchEventsFromGesturePath: start");
    if (!gesturePositions_) {
        HILOG_ERROR("gesturePositions_ is null.");
        return;
    }
    const std::vector<AccessibilityGesturePosition> &positions = gesturePositions_->GetPositions();
    if (positions.size() == 0) {
        HILOG_ERROR("position size is 0.");
        return;
    }
    if ((positions.size() == 1) ||
        ((positions[0].positionX_ == positions[1].positionX_) &&
        (positions[0].positionY_ == positions[1].positionY_))) {
        ParseTapsEvents(startTime);
    } else {
        ParseMovesEvents(startTime);
    }
}
} // namespace Accessibility
} // namespace OHOS