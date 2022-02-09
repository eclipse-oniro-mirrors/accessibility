/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Accessibility {

TouchInjectHandler::TouchInjectHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchEventInjector &server)
    : AppExecFwk::EventHandler(runner),server_(server) {

}

void TouchInjectHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {
    std::shared_ptr<SendEventArgs> parameters = nullptr;
    switch (event->GetInnerEventId()) {
        case TouchEventInjector::SEND_TOUCH_EVENT_MSG:
            parameters = event->GetSharedObject<SendEventArgs>();
            server_.SendTouchEvent(parameters->event_);
            if (parameters->isLastEvent_) {
                server_.GetCurrentGestureService()->OnGestureSimulateResult(server_.GetSequence(), true);
            }
            break;
        case TouchEventInjector::INJECT_EVENT_MSG:
            server_.InjectEventsInner();
            break;
        default:
            break;
    }
}

TouchEventInjector::TouchEventInjector() {
    runner_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return ;
    }
    handler_ = std::make_shared<TouchInjectHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return ;
    }

}

void TouchEventInjector::OnTouchEvent(TouchEvent &event) {
    CancelInjectedEvents();
    SendTouchEvent(event);
}

void TouchEventInjector::OnMouseEvent(MouseEvent &event) {
    if (event.GetAction() == TouchEnum::HOVER_POINTER_MOVE && isGestureUnderway_) {
        return;
    }
    CancelInjectedEvents();
    EventTransmission::OnMouseEvent(event);
}

void TouchEventInjector::ClearEvents(uint32_t inputSource) {
    if (!handler_->HasInnerEvent(SEND_TOUCH_EVENT_MSG)) {
        isGestureUnderway_ = false;
    }
    EventTransmission::ClearEvents(inputSource);
}

void TouchEventInjector::DestroyEvents() {
    HILOG_INFO("TouchEventInjector::DestroyEvents: start");
    CancelInjectedEvents();
    isDestroyEvent_ = true;
    EventTransmission::DestroyEvents();
}

void TouchEventInjector::GetTapsEvents(long startTime) {
    HILOG_INFO("TouchEventInjector::GetTapsEvents: start");
    TouchEvent event;
    MmiPoint point;
    long downTime = startTime;
    long nowTime = startTime;
    for (unsigned int i = 0; i < gesturePath_.size(); i++) {
        /* append down event */
        float px = gesturePath_[i].GetStartPosition().GetPositionX();
        float py = gesturePath_[i].GetStartPosition().GetPositionY();
        point.Setxy(px, py);
        event = obtainTouchEvent(downTime, nowTime, TouchEnum::PRIMARY_POINT_DOWN, point);
        HILOG_INFO("append down event");
        injectedEvents_.push_back(event);

        nowTime += gesturePath_[i].GetDurationTime();
        /* append up event */
        px = gesturePath_[i].GetEndPosition().GetPositionX();
        py = gesturePath_[i].GetEndPosition().GetPositionY();
        point.Setxy(px, py);
        event = obtainTouchEvent(downTime, nowTime, TouchEnum::PRIMARY_POINT_UP, point);
        HILOG_INFO("append up event");
        injectedEvents_.push_back(event);
        nowTime += DOUBLE_TAP_MIN_TIME;
    }
}

void TouchEventInjector::GetMovesEvents(long startTime) {
    HILOG_INFO("TouchEventInjector::GetTapsEvents: start");
    TouchEvent event;
    MmiPoint point;
    long downTime = startTime;
    long nowTime = startTime;
    /* append down event */
    float px = gesturePath_[0].GetStartPosition().GetPositionX();
    float py = gesturePath_[0].GetStartPosition().GetPositionY();
    point.Setxy(px, py);
    event = obtainTouchEvent(downTime, nowTime, TouchEnum::PRIMARY_POINT_DOWN, point);
    HILOG_INFO("append down event");
    injectedEvents_.push_back(event);
    for (unsigned int i = 0; i < gesturePath_.size(); i++) {
        px = gesturePath_[i].GetEndPosition().GetPositionX();
        py = gesturePath_[i].GetEndPosition().GetPositionY();
        point.Setxy(px, py);
        HILOG_INFO("append move event");
        event = obtainTouchEvent(downTime, nowTime, TouchEnum::POINT_MOVE, point);
        injectedEvents_.push_back(event);
        nowTime += gesturePath_[i].GetDurationTime();
    }
    /* append up event */
    px = gesturePath_[gesturePath_.size() - 1].GetEndPosition().GetPositionX();
    py = gesturePath_[gesturePath_.size() - 1].GetEndPosition().GetPositionY();
    point.Setxy(px, py);
    event = obtainTouchEvent(downTime, nowTime, TouchEnum::PRIMARY_POINT_UP, point);
    HILOG_INFO("append up event");
    injectedEvents_.push_back(event);
}

void TouchEventInjector::InjectEvents(const std::vector<GesturePathDefine> &gesturePath,
        const sptr<IAccessibleAbilityClient> &service, int sequence ) {
    sequence_ = sequence;
    currentGestureService_ = service;
    gesturePath_ = gesturePath;
    handler_->SendEvent(INJECT_EVENT_MSG, 0, 0);
}

void TouchEventInjector::InjectEventsInner() {
    HILOG_INFO("TouchEventInjector::InjectEventsInner: start");

    long curTime = getSystemTime();
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
        parameters->isLastEvent_ = (i == injectedEvents_.size() -1) ? true : false;
        parameters->event_ = injectedEvents_[i];
        int64_t timeout = injectedEvents_[i].GetOccurredTime() - curTime;

        handler_->SendEvent(SEND_TOUCH_EVENT_MSG, parameters, timeout);
    }
    injectedEvents_.clear();
}

void TouchEventInjector::SendTouchEvent(TouchEvent &event) {
    HILOG_INFO("TouchEventInjector::SendTouchEvent: start");
    if (GetNext() != nullptr) {
        EventTransmission::OnTouchEvent(event);
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            isGestureUnderway_ = true;
        }
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_UP) {
            isGestureUnderway_ = false;
        }
    }
}

void TouchEventInjector::CancelGesture() {
    HILOG_INFO("TouchEventInjector::CancelGesture: start");
    TouchEvent event;
    MmiPoint point(0, 0);
    long time = getSystemTime();
    if (GetNext() != nullptr && isGestureUnderway_) {
        TouchEvent event = obtainTouchEvent(time, time, TouchEnum::CANCEL, point);
        SendTouchEvent(event);
        isGestureUnderway_ = false;
    }
}

void TouchEventInjector::CancelInjectedEvents() {
    HILOG_INFO("TouchEventInjector::CancelInjectedEvents: start");
    if (handler_->HasInnerEvent(SEND_TOUCH_EVENT_MSG)) {
        handler_->RemoveEvent(SEND_TOUCH_EVENT_MSG);
        CancelGesture();
        currentGestureService_->OnGestureSimulateResult(sequence_, false);
    }
}

void TouchEventInjector::GetTouchEventsFromGesturePath(long startTime) {
    HILOG_INFO("TouchEventInjector::GetTouchEventsFromGesturePath: start");
    if (gesturePath_[0].GetStartPosition().GetPositionX() == gesturePath_[0].GetEndPosition().GetPositionX() &&
        gesturePath_[0].GetStartPosition().GetPositionY() == gesturePath_[0].GetEndPosition().GetPositionY()) {
        GetTapsEvents(startTime);
    } else {
        GetMovesEvents(startTime);
    }
}

TouchEvent TouchEventInjector::obtainTouchEvent(long startTime, long occurredTime,
    int action, MmiPoint point) {
    HILOG_INFO("TouchEventInjector::obtainTouchEvent: start");
    TouchEvent touchEvent;
#if 0 // TODO: Construct touchEvent
    struct MultimodalProperty multimodal = {
        .sourceType = SourceDevice::TOUCH_PANEL,
        .occurredTime = occurredTime,
        .deviceId = "",
        .inputDeviceId = 0,
    };
    struct ManipulationProperty manipulationProperty = {
        .startTime = startTime,
        .pointerCount = 1,
        .pointerId = 0,
        .mp = point,
        .offsetX = 0,
        .offsetY = 0
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
#endif
    return touchEvent;
}

long TouchEventInjector::getSystemTime() {
    HILOG_INFO("TouchEventInjector::getSystemTime: start");
    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    long millisecond = times.tv_sec * 1000 + times.tv_nsec / 1000000;

    return millisecond;
}

}  // namespace Accessibility
}  // namespace OHOS