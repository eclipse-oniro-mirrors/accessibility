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

#include "accessibility_touch_guider.h"
#include "accessibility_window_manager.h"

namespace OHOS{
namespace Accessibility{

TGEventHandler::TGEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchGuider &tgServer)
    : AppExecFwk::EventHandler(runner),tgServer_(tgServer) {

}

TouchGuider::TouchGuider() {

    currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
    pAams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
}

void TouchGuider::StartUp() {
    touchGuideListener_ = std::make_unique<TouchGuideListener>(*this);
    gestureRecognizer_.registerListener(touchGuideListener_.get());
    runner_ = pAams_->GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return ;
    }

    handler_ = std::make_shared<TGEventHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return ;
    }
}

void TGEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {
    switch (event->GetInnerEventId()) {
        case TouchGuider::EXIT_GESTURE_REC_MSG:
            tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
            break;
        case TouchGuider::SEND_HOVER_ENTER_MOVE_MSG:
            HoverEnterAndMoveRunner();
            break;
        case TouchGuider::SEND_HOVER_EXIT_MSG:
            HoverExitRunner();
            break;
        case TouchGuider::SEND_TOUCH_INTERACTION_END_MSG:
            tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            break;
        case TouchGuider::SEND_TOUCH_GUIDE_END_MSG:
            tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_END);
            break;
        default:
            break;
    }
}

void TouchGuider::OnTouchEvent(TouchEvent &event) {
    if (event.GetAction() == TouchEnum::CANCEL) {
        Clear(event);
        return;
    }

    RecordReceivedEvent(event);

    if (gestureRecognizer_.OnTouchEvent(event)) {
        return;
    }

    switch (static_cast<TouchGuideState>(currentState_)) {
        case TouchGuideState::TOUCH_GUIDING:
            HandleTouchGuidingState(event);
            break;
        case TouchGuideState::DRAGGING:
            HandleDraggingState(event);
            break;
        case TouchGuideState::TRANSMITING:
            HandleTransmitingState(event);
            break;
        case TouchGuideState::GESTURE_RECOGNIZING:
            break;
        default:
            break;
    }
}

void TouchGuider::OnAccessibilityEvent(AccessibilityEventInfo &event) {
    int eventType = event.GetEventType();

    if (eventType == EventType::TYPE_VIEW_HOVER_EXIT_EVENT) {
        if (HasEventPending(SEND_TOUCH_GUIDE_END_MSG)) {
            CancelPostEvent(SEND_TOUCH_GUIDE_END_MSG);
            SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_END);
        }
        if (HasEventPending(SEND_TOUCH_INTERACTION_END_MSG)) {
            CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
            SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
        }
    }
    EventTransmission::OnAccessibilityEvent(event);
}

void TouchGuider::ClearEvents(uint32_t inputSource) {
    if (inputSource == MMI::SourceDevice::TOUCH_PANEL) {
        Clear();
    }
    EventTransmission::ClearEvents(inputSource);
}

void TouchGuider::DestroyEvents() {
    Clear();
    EventTransmission::DestroyEvents();
}

void TouchGuider::SendAccessibilityEventToAA(EventType eventType) {
    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(eventType);
    int windowsId = AccessibilityWindowInfoManager::GetInstance().activeWindowId_;
    eventInfo.SetWindowId(windowsId);
    pAams_->SendEvent(eventInfo, pAams_->GetCurrentAccountId());
    if (eventType == EventType::TYPE_TOUCH_GUIDE_BEGIN) {
        isTouchGuiding_ = true;
    } else if (eventType == EventType::TYPE_TOUCH_GUIDE_END) {
        isTouchGuiding_ = false;
    }
}

void TouchGuider::SendEventToMultimodal(TouchEvent &event, int action) {
    TouchEvent touchEvent;
    // struct MultimodalProperty multimodal = {
    //     .highLevelEvent = event.GetHighLevelEvent(),
    //     .uuid = event.GetUuid(),
    //     .sourceType = event.GetSourceDevice(),
    //     .occurredTime = event.GetOccurredTime(),
    //     .deviceId = event.GetDeviceId(),
    //     .inputDeviceId =  event.GetInputDeviceId(),
    //     .isHighLevelEvent = event.IsHighLevelInput()
    // };
    // struct ManipulationProperty manipulationProperty = {
    //     .operationState = event.GetPhase(),
    //     .pointerCount = event.GetPointerCount(),
    //     .pointerId = event.GetPointerId(event.GetIndex()),
    //     .mp = event.GetPointerScreenPosition(event.GetIndex()),
    //     .touchArea = event.GetForce(event.GetIndex()),
    //     .touchPressure = event.GetRadius(event.GetIndex())
    // };
    // struct TouchProperty touch = {
    //     .action = action,
    //     .index = event.GetIndex(),
    //     .forcePrecision = event.GetForcePrecision(),
    //     .maxForce = event.GetMaxForce(),
    //     .tapCount = event.GetTapCount()
    // };
#if 0 // TODO: Set Property
    if (action == TouchEnum::PRIMARY_POINT_DOWN) {
        manipulationProperty.startTime = event.GetStartTime();
    } else {
        manipulationProperty.startTime = injectedRecorder_.lastDownTime;
    }
    if (longPressPointId_ >= 0) {
        manipulationProperty.offsetX = longPressOffsetX_;
        manipulationProperty.offsetY = longPressOffsetY_;
    }
#endif
    // touchEvent.Initialize(multimodal, manipulationProperty, touch);
    touchEvent.Initialize(event);
    EventTransmission::OnTouchEvent(touchEvent);
    RecordInjectedEvent(touchEvent);
}

std::vector<TouchEvent> TouchGuider::getHoverEnterAndMoveEvent() {
    return motionEvent_;
}

void TouchGuider::ClearHoverEnterAndMoveEvent() {
    motionEvent_.clear();
    gestureRecognizer_.Clear();
}

std::shared_ptr<TouchEvent> TouchGuider::getLastReceivedEvent() {
    return receivedRecorder_.lastEvent;
}

void TouchGuider::TouchGuideListener::OnDoubleTapLongPress(TouchEvent &event) {
    MmiPoint clickPoint(INIT_MMIPOINT, INIT_MMIPOINT);
    if (server_.currentState_ != static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        return;
    }
    if (server_.getLastReceivedEvent() &&
        server_.getLastReceivedEvent()->GetPointerCount() == 0) {
        return;
    }
    int ret = GetClickPosition(clickPoint);
    if (ret == CLICK_NONE) {
        return;
    }
    server_.longPressPointId_ = event.GetPointerId(event.GetIndex());
    MmiPoint eventPoint = event.GetPointerPosition(event.GetIndex());
    server_.longPressOffsetX_ = eventPoint.GetX() - clickPoint.GetX();
    server_.longPressOffsetY_ = eventPoint.GetY() - clickPoint.GetY();

    server_.SendExitEvents();
    server_.currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
    server_.SendAllDownEvents(event);
}

bool TouchGuider::TouchGuideListener::OnDoubleTap(TouchEvent &event) {
    MmiPoint clickPoint(INIT_MMIPOINT, INIT_MMIPOINT);
    if (server_.currentState_ != static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        return false;
    }
    server_.pAams_->OnTouchInteractionEnd();
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_EXIT_MSG);
    server_.ForceSendAndRemoveEvent(server_.SEND_TOUCH_GUIDE_END_MSG, event);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);

    if (server_.pAams_->PerformActionOnAccessibilityFocusedItem(ActionType::ACCESSIBILITY_ACTION_CLICK)) {
        return true;
    }
    int ret = GetClickPosition(clickPoint);
    if (ret == CLICK_NONE) {
        return true;
    }
    return TransformToSingleTap(event, clickPoint);
}

bool TouchGuider::TouchGuideListener::OnStarted() {
    server_.currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
    server_.CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    server_.PostGestureRecognizeExit();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    return false;
}

bool TouchGuider::TouchGuideListener::OnCompleted(int gestureId) {
    if (server_.currentState_ != static_cast<int>(TouchGuideState::TRANSMITING)) {
        return false;
    }
    server_.pAams_->OnTouchInteractionEnd();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    server_.currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);

    server_.pAams_->OnGesture(gestureId);

    return true;
}

bool TouchGuider::TouchGuideListener::OnCancelled(TouchEvent &event) {
    switch (static_cast<TouchGuideState>(server_.currentState_)) {
        case TouchGuideState::TRANSMITING:
            server_.pAams_->OnTouchInteractionEnd();
            server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
            if (event.GetAction() == TouchEnum::PRIMARY_POINT_UP) {
                server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            }
            server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
            server_.currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
            break;
        case TouchGuideState::TOUCH_GUIDING:
            server_.motionEvent_.push_back(event);
            server_.ForceSendAndRemoveEvent(SEND_HOVER_ENTER_MOVE_MSG, event);
            server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            server_.SendEventToMultimodal(event, TouchEnum::HOVER_POINTER_MOVE);
         break;
        default:
            return false;
    }
    return true;
}

int TouchGuider::TouchGuideListener::GetClickPosition(MmiPoint &outPoint) {
    std::shared_ptr<TouchEvent> lastTGEvent = server_.injectedRecorder_.lastHoverEvent;
    if (lastTGEvent) {
        float px = lastTGEvent->GetPointerPosition(lastTGEvent->GetIndex()).GetX();
        float py = lastTGEvent->GetPointerPosition(lastTGEvent->GetIndex()).GetY();
        outPoint.Setxy(px, py);
        if (server_.pAams_->GetAccessibilityFocusClickPointInScreen(outPoint)) {
            return CLICK_ACCESSIBILITY_FOCUS;
        } else {
            return CLICK_LAST_TOUCH_GUIDE;
        }
    }
    if (server_.pAams_->GetAccessibilityFocusClickPointInScreen(outPoint)) {
        return CLICK_ACCESSIBILITY_FOCUS;
    }

    return CLICK_NONE;
}

bool TouchGuider::TouchGuideListener::TransformToSingleTap(TouchEvent &event, MmiPoint &point) {
    TouchEvent touchEvent;
#if 0 // TODO
    struct MultimodalProperty multimodal = {
        .highLevelEvent = event.GetHighLevelEvent(),
        .uuid = event.GetUuid(),
        .sourceType = event.GetSourceDevice(),
        .occurredTime = event.GetOccurredTime(),
        .deviceId = event.GetDeviceId(),
        .inputDeviceId =  event.GetInputDeviceId(),
        .isHighLevelEvent = event.IsHighLevelInput()
    };
    struct ManipulationProperty manipulationProperty = {
        .startTime = event.GetStartTime(),
        .operationState = event.GetPhase(),
        .pointerCount = event.GetPointerCount(),
        .pointerId = event.GetPointerId(event.GetIndex()),
        .mp = point,
        .touchArea = event.GetForce(event.GetIndex()),
        .touchPressure = event.GetRadius(event.GetIndex()),
        .offsetX = 0,
        .offsetY = 0
    };
    struct TouchProperty touch = {
        .action = TouchEnum::PRIMARY_POINT_DOWN,
        .index = event.GetIndex(),
        .forcePrecision = event.GetForcePrecision(),
        .maxForce = event.GetMaxForce(),
        .tapCount = event.GetTapCount()
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
#endif
    server_.SendEventToMultimodal(touchEvent, TouchEnum::PRIMARY_POINT_DOWN);
    server_.SendEventToMultimodal(touchEvent, TouchEnum::PRIMARY_POINT_UP);

    return true;
}

void TouchGuider::HandleTouchGuidingState(TouchEvent &event) {
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
            HandleTouchGuidingStateInnerDown(event);
            break;
        case TouchEnum::OTHER_POINT_DOWN:
            CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
            CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            break;
        case TouchEnum::POINT_MOVE:
            HandleTouchGuidingStateInnerMove(event);
            break;
        case TouchEnum::PRIMARY_POINT_UP:
            pAams_->OnTouchInteractionEnd();
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                PostHoverExit();
            } else {
                SendExitEvents();
            }
            if (!HasEventPending(SEND_TOUCH_INTERACTION_END_MSG)) {
                PostAccessibilityEvent(SEND_TOUCH_INTERACTION_END_MSG);
            }
            break;
        default:
            break;
    }
}

void TouchGuider::HandleDraggingState(TouchEvent &event) {
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
            Clear(event);
            break;
        case TouchEnum::OTHER_POINT_DOWN:
            currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
            SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_UP);
            SendAllDownEvents(event);
            break;
        case TouchEnum::POINT_MOVE:
            HandleDraggingStateInnerMove(event);
            break;
        case TouchEnum::OTHER_POINT_UP:
            SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_UP);
            break;
        case TouchEnum::PRIMARY_POINT_UP:
            pAams_->OnTouchInteractionEnd();
            SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_UP);
            currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
            break;
        default:
            break;
    }
}

void TouchGuider::HandleTransmitingState(TouchEvent &event) {
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
            Clear(event);
            break;
        case TouchEnum::PRIMARY_POINT_UP:
            if (longPressPointId_ >= 0) {
                event.SetScreenOffset(longPressOffsetX_, longPressOffsetY_);
                longPressPointId_ = INIT_POINT_ID;
                longPressOffsetX_ = INIT_MMIPOINT;
                longPressOffsetY_ = INIT_MMIPOINT;
            }
            SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_UP);
            pAams_->OnTouchInteractionEnd();
            SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
            break;
        default:
            SendEventToMultimodal(event, event.GetAction());
            break;
    }
}

void TouchGuider::Clear(TouchEvent &event) {
    if (currentState_ == static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        SendExitEvents();
    } else if (currentState_ == static_cast<int>(TouchGuideState::DRAGGING)
        || currentState_ == static_cast<int>(TouchGuideState::TRANSMITING)) {
        SendUpForAllInjectedEvent(event);
    }

    CancelPostEvent(EXIT_GESTURE_REC_MSG);
    CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
    CancelPostEvent(SEND_TOUCH_GUIDE_END_MSG);
    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    ClearInjectedEventRecorder();
    ClearReceivedEventRecorder();
    motionEvent_.clear();
    currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
    isTouchGuiding_ = false;
    gestureRecognizer_.Clear();
    longPressPointId_ = INIT_POINT_ID;
    longPressOffsetX_ = INIT_MMIPOINT;
    longPressOffsetY_ = INIT_MMIPOINT;
    pAams_->OnTouchInteractionEnd();
}

void TouchGuider::Clear() {
    std::shared_ptr<TouchEvent> event = getLastReceivedEvent();
    if (event) {
        Clear(*event);
    }
}

void TouchGuider::SendExitEvents() {
    std::shared_ptr<TouchEvent> event = injectedRecorder_.lastHoverEvent;
    if (event && event->GetAction() != TouchEnum::HOVER_POINTER_EXIT) {
        if (!HasEventPending(SEND_TOUCH_GUIDE_END_MSG)) {
            PostAccessibilityEvent(SEND_TOUCH_GUIDE_END_MSG);
        }
        SendEventToMultimodal(*event, TouchEnum::HOVER_POINTER_EXIT);
    }
}

void TouchGuider::HandleTouchGuidingStateInnerDown(TouchEvent &event) {
    pAams_->OnTouchInteractionStart();
    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    if (isTouchGuiding_) {
        SendExitEvents();
    }
    if (!gestureRecognizer_.IsfirstTap()) {
        ForceSendAndRemoveEvent(SEND_TOUCH_GUIDE_END_MSG, event);
        ForceSendAndRemoveEvent(SEND_TOUCH_INTERACTION_END_MSG, event);
        SendAccessibilityEventToAA(EventType::TYPE_TOUCH_BEGIN);
        if (!isTouchGuiding_) {
            if (!HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                PostHoverEnterAndMove(event);
            } else {
                motionEvent_.push_back(event);
            }
        }
    } else {
        CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
    }
}

void TouchGuider::HandleTouchGuidingStateInnerMove(TouchEvent &event) {
    std::shared_ptr<TouchEvent> lastInjectEvent = injectedRecorder_.lastHoverEvent;
    switch (event.GetPointerCount()) {
        case POINTER_COUNT_1:
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                motionEvent_.push_back(event);
            } else if (isTouchGuiding_) {
                if (lastInjectEvent && lastInjectEvent->GetAction() != TouchEnum::HOVER_POINTER_EXIT) {
                    SendEventToMultimodal(*lastInjectEvent, TouchEnum::HOVER_POINTER_ENTER);
                }
                SendEventToMultimodal(event, TouchEnum::HOVER_POINTER_MOVE);
            }
            break;
        case POINTER_COUNT_2:
                CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
                CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
                if (IsDragGestureAccept(event)) {
                    currentState_ =  static_cast<int>(TouchGuideState::DRAGGING);
                    SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_DOWN);
                } else {
                    currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
                    SendAllDownEvents(event);
                }
            break;
        default:
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
                CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            } else {
                SendExitEvents();
            }
            currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
            SendAllDownEvents(event);
            break;
    }
}

void TouchGuider::HandleDraggingStateInnerMove(TouchEvent &event) {
    int pointCount = event.GetPointerCount();
    if (pointCount == POINTER_COUNT_1) {
        HILOG_INFO("Only two pointers can be received in the draging state");
    } else if (pointCount == POINTER_COUNT_2 && IsDragGestureAccept(event)) {
        /* get densityPixels from WMS */
        AccessibilityDisplayManager &displayMgr = AccessibilityDisplayManager::GetInstance();
        float densityPixels = displayMgr.GetDefaultDisplay().dpi;
        int miniZoomPointerDistance = (int)MINI_POINTER_DISTANCE_DIP * densityPixels;
        float xPointF = event.GetPointerPosition(INDEX_0).GetX();
        float xPointS = event.GetPointerPosition(INDEX_1).GetX();
        float yPointF = event.GetPointerPosition(INDEX_0).GetY();
        float yPointS = event.GetPointerPosition(INDEX_1).GetY();
        float offsetX = abs(xPointF - xPointS);
        float offsetY = abs(yPointF - yPointS);
        double duration = hypot(offsetX , offsetY);
        if (duration > miniZoomPointerDistance) {
            event.SetScreenOffset(DIVIDE_2(offsetX), DIVIDE_2(offsetY));
        }
        SendEventToMultimodal(event, TouchEnum::POINT_MOVE);
    } else {
        currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
        SendEventToMultimodal(event, TouchEnum::PRIMARY_POINT_UP);
        SendAllDownEvents(event);
    }
}

float TouchGuider::GetAngleCos(float offsetX, float offsetY, bool isGetX) {
    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration != 0) {
        ret = ret / duration;
    }
    return ret;
}

bool TouchGuider::IsDragGestureAccept(TouchEvent &event) {
    float xPointF = event.GetPointerPosition(INDEX_0).GetX();
    float xPointS = event.GetPointerPosition(INDEX_1).GetX();
    float yPointF = event.GetPointerPosition(INDEX_0).GetY();
    float yPointS = event.GetPointerPosition(INDEX_1).GetY();
    float xPointDownF = receivedRecorder_.pointerDownX[INDEX_0];
    float xPointDownS = receivedRecorder_.pointerDownX[INDEX_1];
    float yPointDownF = receivedRecorder_.pointerDownY[INDEX_0];
    float yPointDownS = receivedRecorder_.pointerDownY[INDEX_1];

    float firstOffsetX = xPointF - xPointDownF;
    float firstOffsetY = yPointF - yPointDownF;
    float secondOffsetX = xPointS - xPointDownS;
    float secondOffsetY = yPointS - yPointDownS;

    if ((firstOffsetX == 0 && firstOffsetY == 0) ||
        (secondOffsetX == 0 && secondOffsetY == 0))
        return true;

    float firstXCos = GetAngleCos(firstOffsetX, firstOffsetY, true);
    float firstYCos = GetAngleCos(firstOffsetX, firstOffsetY, false);
    float secondXCos = GetAngleCos(secondOffsetX, secondOffsetY, true);
    float secondYCos = GetAngleCos(secondOffsetX, secondOffsetY, false);

    if ((firstXCos * secondXCos + firstYCos * secondYCos) < MAX_DRAG_GESTURE_COSINE)
        return false;

    return true;
}

void TouchGuider::RecordInjectedEvent(TouchEvent &event) {
    int pointId = event.GetPointerId(event.GetIndex());
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
        case TouchEnum::OTHER_POINT_DOWN:
            injectedRecorder_.downPointerNum++;
            injectedRecorder_.downPointers |= (1 << pointId);
            injectedRecorder_.lastDownTime = event.GetStartTime();
            break;
        case TouchEnum::PRIMARY_POINT_UP:
        case TouchEnum::OTHER_POINT_UP:
            injectedRecorder_.downPointers &= ~(1 << pointId);
            if (injectedRecorder_.downPointerNum > 0) {
                injectedRecorder_.downPointerNum--;
            }
            if (injectedRecorder_.downPointers == 0) {
                injectedRecorder_.lastDownTime = 0;
            }
            break;
        case TouchEnum::HOVER_POINTER_ENTER:
        case TouchEnum::HOVER_POINTER_MOVE:
        case TouchEnum::HOVER_POINTER_EXIT:
            injectedRecorder_.lastHoverEvent = std::make_shared<TouchEvent>(event);
            break;
        default:
            break;
    }
}

void TouchGuider::RecordReceivedEvent(TouchEvent &event) {
    int index = event.GetIndex();
    int pointId = event.GetPointerId(index);
    receivedRecorder_.lastEvent = std::make_shared<TouchEvent>(event);
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
        case TouchEnum::OTHER_POINT_DOWN:
            receivedRecorder_.pointerDownX[pointId] = event.GetPointerPosition(index).GetX();
            receivedRecorder_.pointerDownY[pointId] = event.GetPointerPosition(index).GetY();
            break;
        case TouchEnum::PRIMARY_POINT_UP:
        case TouchEnum::OTHER_POINT_UP:
            receivedRecorder_.pointerDownX[pointId] = 0;
            receivedRecorder_.pointerDownY[pointId] = 0;
            break;
        default:
            break;
    }
}

void TouchGuider::ClearReceivedEventRecorder() {
    memset(receivedRecorder_.pointerDownX, 0, sizeof(receivedRecorder_.pointerDownX));
    memset(receivedRecorder_.pointerDownY, 0, sizeof(receivedRecorder_.pointerDownY));
    receivedRecorder_.lastEvent = nullptr;
}

void TouchGuider::ClearInjectedEventRecorder() {
    injectedRecorder_.downPointerNum = 0;
    injectedRecorder_.downPointers = 0;
    injectedRecorder_.lastHoverEvent = nullptr;
}

void TouchGuider::SendAllDownEvents(TouchEvent &event) {
    int pointId = INIT_POINT_ID;
    for (int i = 0; i < event.GetPointerCount(); i++) {
        pointId = event.GetPointerId(i);
        if (!(injectedRecorder_.downPointers & (1 << pointId))) {
            int action = injectedRecorder_.downPointerNum == 0 ? TouchEnum::PRIMARY_POINT_DOWN : TouchEnum::OTHER_POINT_DOWN;
            SendEventToMultimodal(event, action);
        }
    }
}

void TouchGuider::SendUpForAllInjectedEvent(TouchEvent &event) {
    int pointId = INIT_POINT_ID;
    for (int i = 0; i < event.GetPointerCount(); i++) {
        pointId = event.GetPointerId(i);
        if (!(injectedRecorder_.downPointers & (1 << pointId))) {
            continue;
        }
        int action = injectedRecorder_.downPointerNum == 1 ? TouchEnum::PRIMARY_POINT_UP : TouchEnum::OTHER_POINT_UP;
        SendEventToMultimodal(event, action);
    }
}

void TouchGuider::PostGestureRecognizeExit() {
    handler_->SendEvent(EXIT_GESTURE_REC_MSG, 0, EXIT_GESTURE_REC_TIMEOUT);
}

void TouchGuider::PostHoverEnterAndMove(TouchEvent &event) {
    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    motionEvent_.push_back(event);
    handler_->SendEvent(SEND_HOVER_ENTER_MOVE_MSG, 0, DOUBLE_TAP_TIMEOUT);
}

void TouchGuider::PostHoverExit() {
    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    handler_->SendEvent(SEND_HOVER_EXIT_MSG, 0, DOUBLE_TAP_TIMEOUT);
}

void TouchGuider::PostAccessibilityEvent(uint32_t innerEventID) {
    handler_->SendEvent(innerEventID, 0, EXIT_GESTURE_REC_TIMEOUT );
}

void TouchGuider::CancelPostEvent(uint32_t innerEventID) {
    handler_->RemoveEvent(innerEventID);
}

void TouchGuider::CancelPostEventIfNeed(uint32_t innerEventID) {
    if (HasEventPending(innerEventID)) {
        handler_->RemoveEvent(innerEventID);
        if (innerEventID == SEND_HOVER_ENTER_MOVE_MSG) {
            motionEvent_.clear();
        }
    }
}

bool TouchGuider::HasEventPending(uint32_t innerEventID) {
    return handler_->HasInnerEvent(innerEventID);
}

void TouchGuider::ForceSendAndRemoveEvent(uint32_t innerEventID, TouchEvent &event) {
    if (HasEventPending(innerEventID)) {
        switch (innerEventID) {
            case SEND_HOVER_ENTER_MOVE_MSG:
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_BEGIN);
                if (!motionEvent_.empty()) {
                    SendEventToMultimodal(motionEvent_[INDEX_0], TouchEnum::HOVER_POINTER_ENTER);
                    for (unsigned int i = INDEX_1; i < motionEvent_.size(); i++) {
                        SendEventToMultimodal(motionEvent_[i], TouchEnum::HOVER_POINTER_MOVE);
                    }
                 }
                motionEvent_.clear();
                break;
            case SEND_TOUCH_INTERACTION_END_MSG:
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
                break;
            case SEND_TOUCH_GUIDE_END_MSG:
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_END);
                break;
            default:
                break;
        }
        CancelPostEvent(innerEventID);
    }
}

void TGEventHandler::HoverEnterAndMoveRunner() {
    std::vector<TouchEvent> motionEvent = tgServer_.getHoverEnterAndMoveEvent();
    tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_BEGIN);
    if (!motionEvent.empty()) {
        tgServer_.SendEventToMultimodal(motionEvent[INDEX_0], TouchEnum::HOVER_POINTER_ENTER);
        for (unsigned int i = INDEX_1; i < motionEvent.size(); i++) {
           tgServer_. SendEventToMultimodal(motionEvent[i], TouchEnum::HOVER_POINTER_MOVE);
        }
    }
    tgServer_.ClearHoverEnterAndMoveEvent();
}

void TGEventHandler::HoverExitRunner() {
    std::shared_ptr<TouchEvent> pEvent = tgServer_.getLastReceivedEvent();
    tgServer_.SendEventToMultimodal(*pEvent, TouchEnum::HOVER_POINTER_EXIT);
    if (!HasInnerEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG)) {
        RemoveEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG);
        SendEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG, 0, EXIT_GESTURE_REC_TIMEOUT );
    }
    if (HasInnerEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG)) {
        RemoveEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG);
        SendEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG, 0, EXIT_GESTURE_REC_TIMEOUT );
    }
}

}  // namespace Accessibility
}  // namespace OHOS