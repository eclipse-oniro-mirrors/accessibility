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
#define NS_TO_US 1000
#define S_TO_US 1000000
#define MS_TO_US 1000
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
                if (server_.GetCurrentGestureService()){
                    server_.GetCurrentGestureService()->OnGestureSimulateResult(server_.GetSequence(), true);
                }
            }
            break;
        case TouchEventInjector::INJECT_EVENT_MSG:
            server_.InjectEventsInner();
            break;
        default:
            break;
    }
}

TouchEventInjector::TouchEventInjector()
{
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    runner_ = aams->GetMainRunner();
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

void TouchEventInjector::GetTapsEvents(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::GetTapsEvents: start");
    std::shared_ptr<MMI::PointerEvent> event;
    MMI::PointerEvent::PointerItem pointer = {};
    int64_t downTime = startTime;
    int64_t nowTime = startTime;
    pointer.SetPointerId(1);
    for (unsigned int i = 0; i < gesturePath_.size(); i++) {
        // Append down event
        float px = gesturePath_[i].GetStartPosition().GetPositionX();
        float py = gesturePath_[i].GetStartPosition().GetPositionY();
        pointer.SetGlobalX(px);
        pointer.SetGlobalY(py);
        pointer.SetDownTime(downTime);
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
        HILOG_INFO("append down event");
        injectedEvents_.push_back(event);

        nowTime += gesturePath_[i].GetDurationTime() * MS_TO_US;
        // Append up event
        px = gesturePath_[i].GetEndPosition().GetPositionX();
        py = gesturePath_[i].GetEndPosition().GetPositionY();
        pointer.SetGlobalX(px);
        pointer.SetGlobalY(py);
        pointer.SetDownTime(downTime);
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, nowTime);
        HILOG_INFO("append up event");
        injectedEvents_.push_back(event);
        nowTime += DOUBLE_TAP_MIN_TIME;
        downTime += DOUBLE_TAP_MIN_TIME;
    }
}

void TouchEventInjector::GetMovesEvents(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::GetMovesEvents: start");
    std::shared_ptr<MMI::PointerEvent> event;
    MMI::PointerEvent::PointerItem pointer = {};
    int64_t downTime = startTime;
    int64_t nowTime = startTime;
    // Append down event
    float px = gesturePath_[0].GetStartPosition().GetPositionX();
    float py = gesturePath_[0].GetStartPosition().GetPositionY();
    pointer.SetPointerId(1);
    pointer.SetGlobalX(px);
    pointer.SetGlobalY(py);
    pointer.SetDownTime(downTime);
    event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, pointer, downTime);
    HILOG_INFO("append down event");
    injectedEvents_.push_back(event);
    for (unsigned int i = 0; i < gesturePath_.size(); i++) {
        px = gesturePath_[i].GetEndPosition().GetPositionX();
        py = gesturePath_[i].GetEndPosition().GetPositionY();
        pointer.SetGlobalX(px);
        pointer.SetGlobalY(py);
        pointer.SetDownTime(downTime);
        HILOG_INFO("append move event");
        nowTime += gesturePath_[i].GetDurationTime() * MS_TO_US;
        event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, pointer, nowTime);
        injectedEvents_.push_back(event);
    }
    // Append up event
    px = gesturePath_[gesturePath_.size() - 1].GetEndPosition().GetPositionX();
    py = gesturePath_[gesturePath_.size() - 1].GetEndPosition().GetPositionY();
    pointer.SetGlobalX(px);
    pointer.SetGlobalY(py);
    pointer.SetDownTime(downTime);
    event = obtainTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, pointer, nowTime);
    HILOG_INFO("append up event");
    injectedEvents_.push_back(event);
}

void TouchEventInjector::InjectEvents(const std::vector<GesturePathDefine> &gesturePath,
    const sptr<IAccessibleAbilityClient> &service, int sequence)
{
    sequence_ = sequence;
    currentGestureService_ = service;
    gesturePath_ = gesturePath;
    handler_->SendEvent(INJECT_EVENT_MSG, 0, 0);
}

void TouchEventInjector::InjectEventsInner()
{
    HILOG_INFO("TouchEventInjector::InjectEventsInner: start");

    int64_t curTime = getSystemTime();
    if (isDestroyEvent_ || !GetNext()) {
        currentGestureService_->OnGestureSimulateResult(sequence_, false);
        return;
    }
    CancelInjectedEvents();
    CancelGesture();

    GetTouchEventsFromGesturePath(curTime);

    if (injectedEvents_.empty()) {
        currentGestureService_->OnGestureSimulateResult(sequence_, false);
        return;
    }
    for (unsigned int i = 0; i < injectedEvents_.size(); i++) {
        std::shared_ptr<SendEventArgs> parameters = std::make_shared<SendEventArgs>();
        parameters->isLastEvent_ = (i == injectedEvents_.size() - 1) ? true : false;
        parameters->event_ = injectedEvents_[i];
        int64_t timeout = (injectedEvents_[i]->GetActionTime() - curTime) / MS_TO_US;
        if (timeout < 0) {
            HILOG_INFO("timeout is error.%{public}lld", timeout);
        } else {
            handler_->SendEvent(SEND_TOUCH_EVENT_MSG, parameters, timeout);
        }
    }
    injectedEvents_.clear();
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
    int64_t time = getSystemTime();
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
        currentGestureService_->OnGestureSimulateResult(sequence_, false);
    }
}

void TouchEventInjector::GetTouchEventsFromGesturePath(int64_t startTime)
{
    HILOG_INFO("TouchEventInjector::GetTouchEventsFromGesturePath: start");
    if (gesturePath_[0].GetStartPosition().GetPositionX() == gesturePath_[0].GetEndPosition().GetPositionX() &&
        gesturePath_[0].GetStartPosition().GetPositionY() == gesturePath_[0].GetEndPosition().GetPositionY()) {
        GetTapsEvents(startTime);
    } else {
        GetMovesEvents(startTime);
    }
}

std::shared_ptr<MMI::PointerEvent> TouchEventInjector::obtainTouchEvent(int action,
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

int64_t TouchEventInjector::getSystemTime()
{
    HILOG_INFO("TouchEventInjector::getSystemTime: start");
    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    int64_t microsecond = (int64_t)(times.tv_sec * S_TO_US + times.tv_nsec / NS_TO_US);

    return microsecond;
}
} // namespace Accessibility
} // namespace OHOS