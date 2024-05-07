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

#include "accessibility_touch_guider.h"
#include "accessibility_window_manager.h"
#include "accessibility_event_info.h"
#include "hilog_wrapper.h"
#include "securec.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t POINTER_COUNT_1 = 1;
    constexpr int32_t POINTER_COUNT_2 = 2;
    constexpr int32_t SCREEN_AXIS_NUM = 2;
    constexpr int32_t REMOVE_POINTER_ID_1 = 1;
    constexpr int64_t IGNORE_REPEAT_EXECUTE_INTERVAL = 300;
} // namespace

TGEventHandler::TGEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchGuider &tgServer)
    : AppExecFwk::EventHandler(runner), tgServer_(tgServer)
{
}

int64_t TouchGuider::lastDoubleTapTime = 0;

TouchGuider::TouchGuider()
{
    HILOG_DEBUG();
    currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
}

void TouchGuider::StartUp()
{
    HILOG_DEBUG();
    touchGuideListener_ = std::make_unique<TouchGuideListener>(*this);
    gestureRecognizer_.RegisterListener(*touchGuideListener_.get());
    multiFingerGestureRecognizer_.RegisterListener(*touchGuideListener_.get());

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    handler_ = std::make_shared<TGEventHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

void TGEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();
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

bool TouchGuider::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }
    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_CANCEL) {
        Clear(event);
        return true;
    }
    RecordReceivedEvent(event);
    if (!multiFingerGestureRecognizer_.IsMultiFingerGestureStarted() &&
        gestureRecognizer_.OnPointerEvent(event)) {
        return true;
    }

    if (multiFingerGestureRecognizer_.OnPointerEvent(event)) {
        return true;
    }

    switch (static_cast<TouchGuideState>(currentState_)) {
        case TouchGuideState::TOUCH_GUIDING:
            HandleTouchGuidingState(event);
            break;
        case TouchGuideState::DRAGGING:
            HandleDraggingState(event);
            break;
        case TouchGuideState::TRANSMITTING:
            HandleTransmitingState(event);
            break;
        default:
            break;
    }
    return true;
}

void TouchGuider::DestroyEvents()
{
    HILOG_DEBUG();

    Clear();
    EventTransmission::DestroyEvents();
}

void TouchGuider::SendAccessibilityEventToAA(EventType eventType)
{
    HILOG_DEBUG("eventType is 0x%{public}x.", eventType);

    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(eventType);
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().activeWindowId_;
    eventInfo.SetWindowId(windowsId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
    if (eventType == EventType::TYPE_TOUCH_GUIDE_BEGIN) {
        isTouchGuiding_ = true;
    } else if (eventType == EventType::TYPE_TOUCH_GUIDE_END) {
        isTouchGuiding_ = false;
    }
}

void TouchGuider::SendGestureEventToAA(GestureType gestureId)
{
    HILOG_DEBUG("gestureId is %{public}d.", gestureId);

    AccessibilityEventInfo eventInfo {};
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().activeWindowId_;
    eventInfo.SetWindowId(windowsId);
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    eventInfo.SetGestureType(gestureId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
}

void TouchGuider::OffsetEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("OffsetEvent");
    MMI::PointerEvent::PointerItem pointer = {};
    event.GetPointerItem(event.GetPointerId(), pointer);

    // add offset
    int32_t newDisplayX = pointer.GetDisplayX() + static_cast<int>(longPressOffsetX_);
    int32_t newDisplayY = pointer.GetDisplayY() + static_cast<int>(longPressOffsetY_);

    HILOG_DEBUG("newDisplayX: %{public}d, newDisplayY: %{public}d", newDisplayX, newDisplayY);
    pointer.SetDisplayX(newDisplayX);
    pointer.SetDisplayY(newDisplayY);
    event.RemovePointerItem(event.GetPointerId());
    event.AddPointerItem(pointer);
}

void TouchGuider::SendEventToMultimodal(MMI::PointerEvent &event, int32_t action)
{
    HILOG_DEBUG("action:%{public}d, SourceType:%{public}d.", action, event.GetSourceType());

    if (gestureRecognizer_.GetIsDoubleTap() && gestureRecognizer_.GetIsLongpress()) {
        if (!focusedElementExist_) {
            HILOG_DEBUG("send longclick event to multimodal, but no focused element.");
            return;
        }
        OffsetEvent(event);
    }

    switch (action) {
        case HOVER_MOVE:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);
            }
            break;
        case POINTER_DOWN:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
            }
            break;
        case POINTER_UP:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
            }
            break;
        case HOVER_ENTER:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER);
            }
            break;
        case HOVER_EXIT:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
            }
            break;
        default:
            break;
    }
    EventTransmission::OnPointerEvent(event);
    RecordInjectedEvent(event);
}

std::list<MMI::PointerEvent> TouchGuider::getHoverEnterAndMoveEvent()
{
    HILOG_DEBUG();

    return pointerEvents_;
}

void TouchGuider::ClearHoverEnterAndMoveEvent()
{
    HILOG_DEBUG();

    pointerEvents_.clear();
    gestureRecognizer_.Clear();
}

std::shared_ptr<MMI::PointerEvent> TouchGuider::getLastReceivedEvent()
{
    HILOG_DEBUG();

    return receivedRecorder_.lastEvent;
}

bool TouchGuider::TouchGuideListener::OnDoubleTap(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (server_.currentState_ != static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING)) {
        return false;
    }
    server_.OnTouchInteractionEnd();
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_EXIT_MSG);
    server_.ForceSendAndRemoveEvent(server_.SEND_TOUCH_GUIDE_END_MSG, event);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);

    return server_.ExecuteActionOnAccessibilityFocused(ActionType::ACCESSIBILITY_ACTION_CLICK);
}

bool TouchGuider::TouchGuideListener::OnStarted()
{
    HILOG_DEBUG();

    server_.currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
    server_.CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    server_.PostGestureRecognizeExit();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    return true;
}

void TouchGuider::TouchGuideListener::MultiFingerGestureOnStarted(bool isTwoFingerGesture)
{
    HILOG_DEBUG();

    if (!isTwoFingerGesture) {
        server_.currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
        return;
    }

    server_.CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    server_.PostGestureRecognizeExit();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
}

bool TouchGuider::TouchGuideListener::OnCompleted(GestureType gestureId)
{
    HILOG_DEBUG("gestureId is %{public}d", gestureId);

    if (server_.currentState_ != static_cast<int32_t>(TouchGuideState::TRANSMITTING)) {
        HILOG_DEBUG("OnCompleted, state is not transmitting.");
        return false;
    }
    server_.OnTouchInteractionEnd();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    server_.currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);

    // Send customize gesture type to aa
    server_.SendGestureEventToAA(gestureId);
    return true;
}

void TouchGuider::TouchGuideListener::MultiFingerGestureOnCompleted(GestureType gestureId)
{
    HILOG_DEBUG("gestureId is %{public}d", gestureId);

    server_.OnTouchInteractionEnd();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    server_.currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);

    // Send customize gesture type to aa
    server_.SendGestureEventToAA(gestureId);
}

bool TouchGuider::TouchGuideListener::OnCancelled(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (static_cast<TouchGuideState>(server_.currentState_)) {
        case TouchGuideState::TRANSMITTING:
            server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
            if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP &&
                event.GetPointerIds().size() == POINTER_COUNT_1) {
                server_.OnTouchInteractionEnd();
                server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            }
            server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
            server_.currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
            break;
        case TouchGuideState::TOUCH_GUIDING:
            server_.pointerEvents_.push_back(event);
            server_.ForceSendAndRemoveEvent(SEND_HOVER_ENTER_MOVE_MSG, event);
            server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            server_.SendEventToMultimodal(event, HOVER_MOVE);
            break;
        default:
            return false;
    }
    return true;
}

void TouchGuider::TouchGuideListener::MultiFingerGestureOnCancelled(const bool isNoDelayFlag)
{
    HILOG_DEBUG();

    if (static_cast<TouchGuideState>(server_.currentState_) == TouchGuideState::TRANSMITTING) {
        server_.currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
    }
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    if (isNoDelayFlag) {
        server_.OnTouchInteractionEnd();
        server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    }
}

void TouchGuider::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void TouchGuider::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void TouchGuider::ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void TouchGuider::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void TouchGuider::ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

void TouchGuider::HandleTouchGuidingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG("action: %{public}d", event.GetPointerAction());

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                HandleTouchGuidingStateInnerDown(event);
            } else {
                CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
                CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleTouchGuidingStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointerIds().size() == POINTER_COUNT_1 && !IsTouchInteractionEnd() &&
                !multiFingerGestureRecognizer_.IsMultiFingerRecognize()) {
                if (gestureRecognizer_.GetIsDoubleTap() && gestureRecognizer_.GetIsLongpress()) {
                    HILOG_DEBUG();
                    SendEventToMultimodal(event, NO_CHANGE);
                    Clear(event);
                    break;
                }
                OnTouchInteractionEnd();
                if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                    PostHoverExit();
                } else if (isTouchGuiding_) {
                    SendExitEvents();
                    PostHoverExit();
                }
                if (!HasEventPending(SEND_TOUCH_INTERACTION_END_MSG)) {
                    PostAccessibilityEvent(SEND_TOUCH_INTERACTION_END_MSG);
                }
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_PULL_MOVE:
            SendEventToMultimodal(event, NO_CHANGE);
            break;
        case MMI::PointerEvent::POINTER_ACTION_PULL_UP:
            SendEventToMultimodal(event, NO_CHANGE);
            break;
        default:
            break;
    }
}

void TouchGuider::HandleDraggingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                Clear(event);
            } else {
                currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
                SendAllUpEvents(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleDraggingStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                OnTouchInteractionEnd();
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
                SendEventToMultimodal(event, NO_CHANGE);
                currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
            } else {
                SendEventToMultimodal(event, NO_CHANGE);
            }
            break;
        default:
            break;
    }
}

void TouchGuider::HandleTransmitingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                Clear(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointerIds().size() == POINTER_COUNT_1 && !IsTouchInteractionEnd() &&
                !multiFingerGestureRecognizer_.IsMultiFingerRecognize()) {
                if (longPressPointId_ >= 0) {
                    // Adjust this event's location.
                    MMI::PointerEvent::PointerItem pointer = {};
                    event.GetPointerItem(event.GetPointerId(), pointer);
                    pointer.SetDisplayX(pointer.GetDisplayX() + longPressOffsetX_);
                    pointer.SetDisplayY(pointer.GetDisplayY() + longPressOffsetY_);
                    event.RemovePointerItem(event.GetPointerId());
                    event.AddPointerItem(pointer);
                    longPressPointId_ = INIT_POINT_ID;
                    longPressOffsetX_ = INIT_MMIPOINT;
                    longPressOffsetY_ = INIT_MMIPOINT;
                }
                SendEventToMultimodal(event, NO_CHANGE);
                OnTouchInteractionEnd();
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
                currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
            }
            break;
        default:
            SendEventToMultimodal(event, NO_CHANGE);
            break;
    }
}

void TouchGuider::Clear(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (currentState_ == static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING)) {
        SendExitEvents();
    } else if (currentState_ == static_cast<int32_t>(TouchGuideState::DRAGGING) ||
        currentState_ == static_cast<int32_t>(TouchGuideState::TRANSMITTING)) {
        SendUpForAllInjectedEvent(event);
    }

    CancelPostEvent(EXIT_GESTURE_REC_MSG);
    CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
    CancelPostEvent(SEND_TOUCH_GUIDE_END_MSG);
    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    ClearInjectedEventRecorder();
    ClearReceivedEventRecorder();
    pointerEvents_.clear();
    currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);
    isTouchGuiding_ = false;
    gestureRecognizer_.Clear();
    longPressPointId_ = INIT_POINT_ID;
    longPressOffsetX_ = INIT_MMIPOINT;
    longPressOffsetY_ = INIT_MMIPOINT;
    leftTopX_ = INIT_POINT_DISPLAY;
    leftTopY_ = INIT_POINT_DISPLAY;
    rightBottomX_ = INIT_POINT_DISPLAY;
    rightBottomY_ = INIT_POINT_DISPLAY;
    focusedElementExist_ = false;
    OnTouchInteractionEnd();
}

void TouchGuider::Clear()
{
    HILOG_DEBUG();

    std::shared_ptr<MMI::PointerEvent> event = getLastReceivedEvent();
    if (event) {
        Clear(*event);
    }
}

void TouchGuider::SendExitEvents()
{
    HILOG_DEBUG();

    if (!HasEventPending(SEND_TOUCH_GUIDE_END_MSG)) {
        PostAccessibilityEvent(SEND_TOUCH_GUIDE_END_MSG);
    }
}

void TouchGuider::HandleTouchGuidingStateInnerDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    OnTouchInteractionStart();
    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    if (isTouchGuiding_) {
        SendExitEvents();
    }
    if (!gestureRecognizer_.IsfirstTap() && !multiFingerGestureRecognizer_.IsMultiFingerGestureStarted()) {
        ForceSendAndRemoveEvent(SEND_TOUCH_GUIDE_END_MSG, event);
        ForceSendAndRemoveEvent(SEND_TOUCH_INTERACTION_END_MSG, event);
        SendAccessibilityEventToAA(EventType::TYPE_TOUCH_BEGIN);
        if (!isTouchGuiding_) {
            if (!HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                PostHoverEnterAndMove(event);
            } else {
                pointerEvents_.push_back(event);
            }
        }
    } else if (gestureRecognizer_.GetIsDoubleTap() && !multiFingerGestureRecognizer_.IsMultiFingerGestureStarted()) {
        pointerEvents_.clear();

        AccessibilityElementInfo focusedElementInfo = {};
        if (!FindFocusedElement(focusedElementInfo)) {
            HILOG_ERROR("FindFocusedElement failed.");
            return;
        }
        HILOG_DEBUG("FindFocusedElement success");
        MMI::PointerEvent::PointerItem pointerIterm = {};
        if (!event.GetPointerItem(event.GetPointerId(), pointerIterm)) {
            HILOG_ERROR("event.GetPointerItem failed");
            return;
        }
        HILOG_DEBUG("GetPointerItem success");
        focusedElementExist_ = true;

        // set point x,y range and offset
        leftTopX_ = focusedElementInfo.GetRectInScreen().GetLeftTopXScreenPostion();
        leftTopY_ = focusedElementInfo.GetRectInScreen().GetLeftTopYScreenPostion();
        rightBottomX_ = focusedElementInfo.GetRectInScreen().GetRightBottomXScreenPostion();
        rightBottomY_ = focusedElementInfo.GetRectInScreen().GetRightBottomYScreenPostion();
        longPressOffsetX_ = static_cast<float>(DIVIDE_2(leftTopX_ + rightBottomX_) - pointerIterm.GetDisplayX());
        longPressOffsetY_ = static_cast<float>(DIVIDE_2(leftTopY_ + rightBottomY_) - pointerIterm.GetDisplayY());

        pointerEvents_.push_back(event);
    } else {
        CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
    }
}

void TouchGuider::SendPointerDownEventToMultimodal(MMI::PointerEvent event, int32_t action)
{
    int32_t currentPid = event.GetPointerId();
    int32_t xPointDown = 0;
    int32_t yPointDown = 0;
    int64_t actionTime = 0;

    if (receivedRecorder_.pointerDownX.find(currentPid) != receivedRecorder_.pointerDownX.end()) {
        xPointDown = receivedRecorder_.pointerDownX.find(currentPid)->second;
        yPointDown = receivedRecorder_.pointerDownY.find(currentPid)->second;
        actionTime = receivedRecorder_.pointerActionTime.find(currentPid)->second;
    }

    HILOG_DEBUG("first down point info is: xPos: %{public}d, yPos: %{public}d, actionTime: [%{public}" PRId64 "], "
        "currentTime: [%{public}" PRId64 "]", xPointDown, yPointDown, actionTime, event.GetActionTime());
    MMI::PointerEvent::PointerItem pointer = {};
    event.GetPointerItem(currentPid, pointer);
    pointer.SetDisplayX(xPointDown);
    pointer.SetDisplayY(yPointDown);
    event.RemovePointerItem(currentPid);
    event.AddPointerItem(pointer);
    event.SetActionTime(actionTime);
    int32_t removePid = currentPid == 0 ? REMOVE_POINTER_ID_1 : 0;
    event.RemovePointerItem(removePid);
    SendEventToMultimodal(event, action);
}

void TouchGuider::HandleTouchGuidingStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (event.GetPointerIds().size()) {
        case POINTER_COUNT_1:
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                pointerEvents_.push_back(event);
            } else if (isTouchGuiding_) {
                SendEventToMultimodal(event, HOVER_MOVE);
            } else if (gestureRecognizer_.GetIsDoubleTap() && gestureRecognizer_.GetIsLongpress()) {
                HILOG_DEBUG();
                if (!pointerEvents_.empty()) {
                    HILOG_DEBUG("pointerEvents_ is not empty");
                    SendEventToMultimodal(pointerEvents_.front(), NO_CHANGE);
                    pointerEvents_.clear();
                }
                SendEventToMultimodal(event, NO_CHANGE);
            } else {
                HILOG_DEBUG("other case");
            }
            break;
        case POINTER_COUNT_2:
            CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
            CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            if (!IsRealMoveState(event)) {
                HILOG_DEBUG("not a move");
                break;
            }
            if (IsDragGestureAccept(event)) {
                currentState_ = static_cast<int32_t>(TouchGuideState::DRAGGING);
                SendPointerDownEventToMultimodal(event, POINTER_DOWN);
                SendEventToMultimodal(event, NO_CHANGE);
            } else {
                currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
            }
            break;
        default:
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
                CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            } else {
                SendExitEvents();
            }
            currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
            break;
    }
}

void TouchGuider::HandleDraggingStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    int32_t pointCount = pIds.size();
    if (pointCount == POINTER_COUNT_1) {
        HILOG_DEBUG("Only two pointers can be received in the dragging state");
    } else if (pointCount == POINTER_COUNT_2 && IsDragGestureAccept(event)) {
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
        // Get densityPixels from WMS
        AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
        auto display = displayMgr.GetDefaultDisplay();
        float densityPixels = display->GetVirtualPixelRatio();
        int32_t miniZoomPointerDistance = static_cast<int32_t>(MINI_POINTER_DISTANCE_DIP * densityPixels);
#else
        HILOG_DEBUG("not support display manager");
        int32_t miniZoomPointerDistance = static_cast<int32_t>(MINI_POINTER_DISTANCE_DIP * 1);
#endif
        MMI::PointerEvent::PointerItem pointerF = {};
        MMI::PointerEvent::PointerItem pointerS = {};
        event.GetPointerItem(pIds[INDEX_0], pointerF);
        event.GetPointerItem(pIds[INDEX_1], pointerS);
        float xPointF = pointerF.GetDisplayX();
        float xPointS = pointerS.GetDisplayX();
        float yPointF = pointerF.GetDisplayY();
        float yPointS = pointerS.GetDisplayY();
        float offsetX = abs(xPointF - xPointS);
        float offsetY = abs(yPointF - yPointS);
        double duration = hypot(offsetX, offsetY);
        if (duration > miniZoomPointerDistance) {
            // Adjust this event's location.
            MMI::PointerEvent::PointerItem pointer = {};
            event.GetPointerItem(event.GetPointerId(), pointer);
            pointer.SetDisplayX(pointer.GetDisplayX() + DIVIDE_2(offsetX));
            pointer.SetDisplayY(pointer.GetDisplayY() + DIVIDE_2(offsetY));
            event.RemovePointerItem(event.GetPointerId());
            event.AddPointerItem(pointer);
        }
        SendEventToMultimodal(event, NO_CHANGE);
    } else {
        currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
        SendAllUpEvents(event);
    }
}

float TouchGuider::GetAngleCos(float offsetX, float offsetY, bool isGetX)
{
    HILOG_DEBUG();

    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration < - EPSINON || duration > EPSINON) {
        ret = ret / duration;
    }
    return ret;
}

void TouchGuider::GetPointOffset(MMI::PointerEvent &event, std::vector<float> &firstPointOffset,
    std::vector<float> &secondPointOffset) const
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    if (pIds.size() != POINTER_COUNT_2) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
    if (!event.GetPointerItem(pIds[0], pointerF)) {
        HILOG_ERROR("GetPointerItem(%{public}d) failed", pIds[0]);
        return;
    }

    if (!event.GetPointerItem(pIds[1], pointerS)) {
        HILOG_ERROR("GetPointerItem(%{public}d) failed", pIds[1]);
        return;
    }

    float xPointF = pointerF.GetDisplayX();
    float xPointS = pointerS.GetDisplayX();
    float yPointF = pointerF.GetDisplayY();
    float yPointS = pointerS.GetDisplayY();
    float xPointDownF = 0;
    float xPointDownS = 0;
    float yPointDownF = 0;
    float yPointDownS = 0;
    if (receivedRecorder_.pointerDownX.find(INDEX_0) != receivedRecorder_.pointerDownX.end()) {
        xPointDownF = receivedRecorder_.pointerDownX.find(INDEX_0)->second;
        yPointDownF = receivedRecorder_.pointerDownY.find(INDEX_0)->second;
    }
    if (receivedRecorder_.pointerDownX.find(INDEX_1) != receivedRecorder_.pointerDownX.end()) {
        xPointDownS = receivedRecorder_.pointerDownX.find(INDEX_1)->second;
        yPointDownS = receivedRecorder_.pointerDownY.find(INDEX_1)->second;
    }

    firstPointOffset.push_back(xPointF - xPointDownF); // firstOffsetX
    firstPointOffset.push_back(yPointF - yPointDownF); // firstOffsetY
    secondPointOffset.push_back(xPointS - xPointDownS); // secondOffsetX
    secondPointOffset.push_back(yPointS - yPointDownS); // secondOffsetY
}

bool TouchGuider::IsDragGestureAccept(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<float> firstPointOffset;
    std::vector<float> secondPointOffset;
    GetPointOffset(event, firstPointOffset, secondPointOffset);
    if (firstPointOffset.size() != SCREEN_AXIS_NUM || secondPointOffset.size() != SCREEN_AXIS_NUM) {
        return false;
    }

    float firstOffsetX = firstPointOffset[0];
    float firstOffsetY = firstPointOffset[1];
    float secondOffsetX = secondPointOffset[0];
    float secondOffsetY = secondPointOffset[1];
    if ((!firstOffsetX && !firstOffsetY) ||
        (!secondOffsetX && !secondOffsetY)) {
        return true;
    }

    float firstXCos = GetAngleCos(firstOffsetX, firstOffsetY, true);
    float firstYCos = GetAngleCos(firstOffsetX, firstOffsetY, false);
    float secondXCos = GetAngleCos(secondOffsetX, secondOffsetY, true);
    float secondYCos = GetAngleCos(secondOffsetX, secondOffsetY, false);
    if ((firstXCos * secondXCos + firstYCos * secondYCos) < MAX_DRAG_GESTURE_COSINE) {
        return false;
    }
    return true;
}

bool TouchGuider::IsRealMoveState(MMI::PointerEvent &event) const
{
    HILOG_DEBUG("moveThreshold: %{public}f", multiFingerGestureRecognizer_.GetTouchSlop());

    std::vector<float> firstPointOffset;
    std::vector<float> secondPointOffset;
    GetPointOffset(event, firstPointOffset, secondPointOffset);
    if (firstPointOffset.size() != SCREEN_AXIS_NUM || secondPointOffset.size() != SCREEN_AXIS_NUM) {
        return false;
    }

    HILOG_DEBUG("offset of fisrt down points and current points: %{public}f, %{public}f,%{public}f, %{public}f",
        firstPointOffset[0], firstPointOffset[1], secondPointOffset[0], secondPointOffset[1]);
    if (hypot(firstPointOffset[0], firstPointOffset[1]) >= multiFingerGestureRecognizer_.GetTouchSlop() &&
        hypot(secondPointOffset[0], secondPointOffset[1]) >= multiFingerGestureRecognizer_.GetTouchSlop()) {
        return true;
    }
    return false;
}

void TouchGuider::RecordInjectedEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t pointerId = event.GetPointerId();
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            injectedRecorder_.downPointerNum++;
            injectedRecorder_.downPointers |= (1 << pointerId);
            injectedRecorder_.lastDownTime = event.GetActionTime() / US_TO_MS;
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            injectedRecorder_.downPointers &= ~(1 << pointerId);
            if (injectedRecorder_.downPointerNum > 0) {
                injectedRecorder_.downPointerNum--;
            }
            if (!injectedRecorder_.downPointers) {
                injectedRecorder_.lastDownTime = 0;
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            injectedRecorder_.lastHoverEvent = std::make_shared<MMI::PointerEvent>(event);
            break;
        default:
            break;
    }
}

void TouchGuider::RecordReceivedEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t pointId = event.GetPointerId();
    MMI::PointerEvent::PointerItem pointer;
    if (!event.GetPointerItem(pointId, pointer)) {
        HILOG_ERROR("GetPointerItem(%{public}d) failed", pointId);
    }
    receivedRecorder_.lastEvent = std::make_shared<MMI::PointerEvent>(event);
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            receivedRecorder_.pointerDownX[pointId] = pointer.GetDisplayX();
            receivedRecorder_.pointerDownY[pointId] = pointer.GetDisplayY();
            receivedRecorder_.pointerActionTime[pointId] = event.GetActionTime();
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            receivedRecorder_.pointerDownX.erase(pointId);
            receivedRecorder_.pointerDownY.erase(pointId);
            receivedRecorder_.pointerActionTime.erase(pointId);
            break;
        default:
            break;
    }
}

void TouchGuider::ClearReceivedEventRecorder()
{
    HILOG_DEBUG();

    receivedRecorder_.pointerDownX.clear();
    receivedRecorder_.pointerDownY.clear();
    receivedRecorder_.pointerActionTime.clear();
    receivedRecorder_.lastEvent = nullptr;
}

void TouchGuider::ClearInjectedEventRecorder()
{
    HILOG_DEBUG();

    injectedRecorder_.downPointerNum = 0;
    injectedRecorder_.downPointers = 0;
    injectedRecorder_.lastHoverEvent = nullptr;
}

void TouchGuider::SendAllDownEvents(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    for (auto& pId : pIds) {
        if (!(injectedRecorder_.downPointers & (1 << pId))) {
            event.SetPointerId(pId);
            SendEventToMultimodal(event, POINTER_DOWN);
        }
    }
}

void TouchGuider::SendAllUpEvents(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    for (auto& pId : pIds) {
        event.SetPointerId(pId);
        SendEventToMultimodal(event, POINTER_UP);
    }
}

void TouchGuider::SendUpForAllInjectedEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    for (const auto& pId : pIds) {
        if (!(injectedRecorder_.downPointers & (1 << pId))) {
            continue;
        }
        SendEventToMultimodal(event, POINTER_UP);
    }
}

void TouchGuider::PostGestureRecognizeExit()
{
    HILOG_DEBUG();

    handler_->SendEvent(EXIT_GESTURE_REC_MSG, 0, EXIT_GESTURE_REC_TIMEOUT);
}

void TouchGuider::PostHoverEnterAndMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    pointerEvents_.push_back(event);
    handler_->SendEvent(SEND_HOVER_ENTER_MOVE_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
}

void TouchGuider::PostHoverExit()
{
    HILOG_DEBUG();

    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    handler_->SendEvent(SEND_HOVER_EXIT_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
}

void TouchGuider::PostAccessibilityEvent(uint32_t innerEventID)
{
    HILOG_DEBUG();

    handler_->SendEvent(innerEventID, 0, EXIT_GESTURE_REC_TIMEOUT);
}

void TouchGuider::CancelPostEvent(uint32_t innerEventID)
{
    HILOG_DEBUG();

    handler_->RemoveEvent(innerEventID);
}

void TouchGuider::CancelPostEventIfNeed(uint32_t innerEventID)
{
    HILOG_DEBUG();

    if (HasEventPending(innerEventID)) {
        handler_->RemoveEvent(innerEventID);
        if (innerEventID == SEND_HOVER_ENTER_MOVE_MSG) {
            pointerEvents_.clear();
        }
    }
}

bool TouchGuider::HasEventPending(uint32_t innerEventID)
{
    HILOG_DEBUG();

    return handler_->HasInnerEvent(innerEventID);
}

void TouchGuider::ForceSendAndRemoveEvent(uint32_t innerEventID, MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (!HasEventPending(innerEventID)) {
        HILOG_DEBUG("No pending event.");
        return;
    }

    switch (innerEventID) {
        case SEND_HOVER_ENTER_MOVE_MSG:
            SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_BEGIN);
            if (pointerEvents_.empty()) {
                break;
            }
            for (auto iter = pointerEvents_.begin(); iter != pointerEvents_.end(); ++iter) {
                if (iter->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
                    SendEventToMultimodal(*iter, HOVER_ENTER);
                } else {
                    SendEventToMultimodal(*iter, HOVER_MOVE);
                }
            }
            pointerEvents_.clear();
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

bool TouchGuider::IgnoreRepeatExecuteAction()
{
    HILOG_DEBUG();
    int64_t time = Utils::GetSystemTime();
    if (time - lastDoubleTapTime < IGNORE_REPEAT_EXECUTE_INTERVAL) {
        HILOG_DEBUG("time interval < 300ms");
        lastDoubleTapTime = time;
        return true;
    }

    lastDoubleTapTime = time;
    return false;
}

bool TouchGuider::ExecuteActionOnAccessibilityFocused(const ActionType &action)
{
    HILOG_DEBUG();
    if (IgnoreRepeatExecuteAction()) {
        return true;
    }
    return Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(action);
}

bool TouchGuider::FindFocusedElement(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    return Singleton<AccessibleAbilityManagerService>::GetInstance().FindFocusedElement(elementInfo);
}

void TGEventHandler::HoverEnterAndMoveRunner()
{
    HILOG_DEBUG();

    std::list<MMI::PointerEvent> motionEvent = tgServer_.getHoverEnterAndMoveEvent();
    tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_BEGIN);
    if (!motionEvent.empty()) {
        for (auto iter = motionEvent.begin(); iter != motionEvent.end(); ++iter) {
            if (iter->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
                tgServer_.SendEventToMultimodal(*iter, HOVER_ENTER);
            } else {
                tgServer_.SendEventToMultimodal(*iter, HOVER_MOVE);
            }
        }
    }
    tgServer_.ClearHoverEnterAndMoveEvent();
}

void TGEventHandler::HoverExitRunner()
{
    HILOG_DEBUG();

    std::shared_ptr<MMI::PointerEvent> pEvent = tgServer_.getLastReceivedEvent();
    tgServer_.SendEventToMultimodal(*pEvent, HOVER_EXIT);
    if (!HasInnerEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG)) {
        RemoveEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG);
        SendEvent(TouchGuider::SEND_TOUCH_GUIDE_END_MSG, 0, EXIT_GESTURE_REC_TIMEOUT);
    }
    if (HasInnerEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG)) {
        RemoveEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG);
        SendEvent(TouchGuider::SEND_TOUCH_INTERACTION_END_MSG, 0, EXIT_GESTURE_REC_TIMEOUT);
    }
}
} // namespace Accessibility
} // namespace OHOS