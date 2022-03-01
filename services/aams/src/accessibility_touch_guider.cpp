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
#include "accessibility_interaction_bridge.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
TGEventHandler::TGEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchGuider &tgServer)
    : AppExecFwk::EventHandler(runner), tgServer_(tgServer)
{
}

TouchGuider::TouchGuider()
{
    HILOG_DEBUG();
    currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
    pAams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
}

void TouchGuider::StartUp()
{
    HILOG_DEBUG();
    touchGuideListener_ = std::make_unique<TouchGuideListener>(*this);
    gestureRecognizer_.RegisterListener(*touchGuideListener_.get());
    runner_ = pAams_->GetMainRunner();
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

void TouchGuider::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("TouchGuider::OnPointerEvent: start");
    switch (event.GetSourceType()) {
        case MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN:
            if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_CANCEL) {
                Clear(event);
                return;
            }
            RecordReceivedEvent(event);
            if (gestureRecognizer_.OnPointerEvent(event)) {
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
            break;
        default:
            break;
    }
}

void TouchGuider::OnAccessibilityEvent(AccessibilityEventInfo &event)
{
    HILOG_DEBUG();

    int eventType = event.GetEventType();
    HILOG_DEBUG("EventType is %{public}x.", eventType);
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

void TouchGuider::ClearEvents(uint32_t inputSource)
{
    HILOG_DEBUG();

    if (inputSource == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        Clear();
    }
    EventTransmission::ClearEvents(inputSource);
}

void TouchGuider::DestroyEvents()
{
    HILOG_DEBUG();

    Clear();
    EventTransmission::DestroyEvents();
}

void TouchGuider::SendAccessibilityEventToAA(EventType eventType)
{
    HILOG_DEBUG("eventType is %{public}x.", eventType);

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

void TouchGuider::SendEventToMultimodal(MMI::PointerEvent &event, int action)
{
    HILOG_DEBUG("action is %{public}d.", action);
    HILOG_DEBUG("SourceType is %{public}d.", event.GetSourceType());

    switch (action) {
        case HOVER_MOVE:
            if (event.GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
                event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
                event.SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
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

void TouchGuider::TouchGuideListener::OnDoubleTapLongPress(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    MMI::PointerEvent::PointerItem clickPoint = {};
    if (server_.currentState_ != static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        return;
    }
    if (server_.getLastReceivedEvent() &&
        server_.getLastReceivedEvent()->GetPointersIdList().size() == 0) {
        return;
    }
    int ret = GetClickPosition(clickPoint);
    if (ret == CLICK_NONE) {
        return;
    }
    server_.longPressPointId_ = event.GetPointerId();
    MMI::PointerEvent::PointerItem eventPoint = {};
    event.GetPointerItem(server_.longPressPointId_, eventPoint);
    server_.longPressOffsetX_ = eventPoint.GetGlobalX() - clickPoint.GetGlobalX();
    server_.longPressOffsetY_ = eventPoint.GetGlobalY() - clickPoint.GetGlobalY();

    server_.SendExitEvents();
    server_.currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
    server_.SendAllDownEvents(event);
}

bool TouchGuider::TouchGuideListener::OnDoubleTap(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    MMI::PointerEvent::PointerItem clickPoint = {};
    if (server_.currentState_ != static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        return false;
    }
    server_.OnTouchInteractionEnd();
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(server_.SEND_HOVER_EXIT_MSG);
    server_.ForceSendAndRemoveEvent(server_.SEND_TOUCH_GUIDE_END_MSG, event);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);

    if (AccessibilityInteractionBridge::GetInstance().ExecuteActionOnAccessibilityFocused(
        ActionType::ACCESSIBILITY_ACTION_CLICK)) {
        return true;
    }
    int ret = GetClickPosition(clickPoint);
    if (ret == CLICK_NONE) {
        return true;
    }
    return TransformToSingleTap(event, clickPoint);
}

bool TouchGuider::TouchGuideListener::OnStarted()
{
    HILOG_DEBUG();

    server_.currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
    server_.CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
    server_.CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    server_.PostGestureRecognizeExit();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    return false;
}

bool TouchGuider::TouchGuideListener::OnCompleted(GestureType gestureId)
{
    HILOG_DEBUG("OnCompleted, gestureId is %{public}d", gestureId);

    if (server_.currentState_ != static_cast<int>(TouchGuideState::TRANSMITING)) {
        HILOG_DEBUG("OnCompleted, state is not transmitting.");
        return false;
    }
    server_.OnTouchInteractionEnd();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    server_.currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);

    // send customize gesture type to aa
    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    eventInfo.SetGestureType(gestureId);
    server_.pAams_->SendEvent(eventInfo, server_.pAams_->GetCurrentAccountId());

    return true;
}

bool TouchGuider::TouchGuideListener::OnCancelled(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (static_cast<TouchGuideState>(server_.currentState_)) {
        case TouchGuideState::TRANSMITING:
            server_.OnTouchInteractionEnd();
            server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
            if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP &&
                event.GetPointersIdList().size() == POINTER_COUNT_1) {
                server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
            }
            server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
            server_.currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
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

int TouchGuider::TouchGuideListener::GetClickPosition(MMI::PointerEvent::PointerItem &outPoint)
{
    HILOG_DEBUG();

    std::shared_ptr<MMI::PointerEvent> lastTGEvent = server_.injectedRecorder_.lastHoverEvent;
    if (lastTGEvent) {
        lastTGEvent->GetPointerItem(lastTGEvent->GetPointerId(), outPoint);
        if (AccessibilityInteractionBridge::GetInstance().GetPointerItermOfAccessibilityFocusClick(outPoint)) {
            return CLICK_ACCESSIBILITY_FOCUS;
        } else {
            return CLICK_LAST_TOUCH_GUIDE;
        }
    }
    if (AccessibilityInteractionBridge::GetInstance().GetPointerItermOfAccessibilityFocusClick(outPoint)) {
        return CLICK_ACCESSIBILITY_FOCUS;
    }

    return CLICK_NONE;
}

bool TouchGuider::TouchGuideListener::TransformToSingleTap(MMI::PointerEvent &event,
    MMI::PointerEvent::PointerItem &point)
{
    HILOG_DEBUG();

    std::vector<int32_t> pointerIds = event.GetPointersIdList();
    for (auto& pId : pointerIds) {
        event.RemovePointerItem(pId);
    }
    point.SetPointerId(event.GetPointerId());
    event.AddPointerItem(point);
    event.SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    server_.SendEventToMultimodal(event, POINTER_DOWN);
    event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    server_.SendEventToMultimodal(event, POINTER_UP);

    return true;
}

void TouchGuider::HandleTouchGuidingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
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
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
                OnTouchInteractionEnd();
                if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                    PostHoverExit();
                } else {
                    SendExitEvents();
                }
                if (!HasEventPending(SEND_TOUCH_INTERACTION_END_MSG)) {
                    PostAccessibilityEvent(SEND_TOUCH_INTERACTION_END_MSG);
                }
            }
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
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
                Clear(event);
            } else {
                currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
                SendEventToMultimodal(event, POINTER_UP);
                SendAllDownEvents(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleDraggingStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
                OnTouchInteractionEnd();
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
                SendEventToMultimodal(event, NO_CHANGE);
                currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
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
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
                Clear(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointersIdList().size() == POINTER_COUNT_1) {
                if (longPressPointId_ >= 0) {
                    // Adjust this event's location.
                    MMI::PointerEvent::PointerItem pointer = {};
                    event.GetPointerItem(event.GetPointerId(), pointer);
                    pointer.SetGlobalX(pointer.GetGlobalX() + longPressOffsetX_);
                    pointer.SetGlobalY(pointer.GetGlobalY() + longPressOffsetY_);
                    event.RemovePointerItem(event.GetPointerId());
                    event.AddPointerItem(pointer);
                    longPressPointId_ = INIT_POINT_ID;
                    longPressOffsetX_ = INIT_MMIPOINT;
                    longPressOffsetY_ = INIT_MMIPOINT;
                }
                SendEventToMultimodal(event, NO_CHANGE);
                OnTouchInteractionEnd();
                SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
                currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
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

    if (currentState_ == static_cast<int>(TouchGuideState::TOUCH_GUIDING)) {
        SendExitEvents();
    } else if (currentState_ == static_cast<int>(TouchGuideState::DRAGGING) ||
        currentState_ == static_cast<int>(TouchGuideState::TRANSMITING)) {
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
    currentState_ = static_cast<int>(TouchGuideState::TOUCH_GUIDING);
    isTouchGuiding_ = false;
    gestureRecognizer_.Clear();
    longPressPointId_ = INIT_POINT_ID;
    longPressOffsetX_ = INIT_MMIPOINT;
    longPressOffsetY_ = INIT_MMIPOINT;
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
    if (!gestureRecognizer_.IsfirstTap()) {
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
    } else {
        CancelPostEvent(SEND_TOUCH_INTERACTION_END_MSG);
    }
}

void TouchGuider::HandleTouchGuidingStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::shared_ptr<MMI::PointerEvent> lastInjectEvent = injectedRecorder_.lastHoverEvent;
    switch (event.GetPointersIdList().size()) {
        case POINTER_COUNT_1:
            if (HasEventPending(SEND_HOVER_ENTER_MOVE_MSG)) {
                pointerEvents_.push_back(event);
            } else if (isTouchGuiding_) {
                SendEventToMultimodal(event, HOVER_MOVE);
            }
            break;
        case POINTER_COUNT_2:
            CancelPostEventIfNeed(SEND_HOVER_ENTER_MOVE_MSG);
            CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
            if (IsDragGestureAccept(event)) {
                currentState_ =  static_cast<int>(TouchGuideState::DRAGGING);
                SendEventToMultimodal(event, POINTER_DOWN);
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

void TouchGuider::HandleDraggingStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointersIdList();
    int pointCount = pIds.size();
    if (pointCount == POINTER_COUNT_1) {
        HILOG_INFO("Only two pointers can be received in the draging state");
    } else if (pointCount == POINTER_COUNT_2 && IsDragGestureAccept(event)) {
        /* get densityPixels from WMS */
        AccessibilityDisplayManager &displayMgr = AccessibilityDisplayManager::GetInstance();
        auto display = displayMgr.GetDefaultDisplay();
        float densityPixels = display->GetVirtualPixelRatio();
        int miniZoomPointerDistance = (int)MINI_POINTER_DISTANCE_DIP * densityPixels;
        MMI::PointerEvent::PointerItem pointerF = {};
        MMI::PointerEvent::PointerItem pointerS = {};
        event.GetPointerItem(pIds[INDEX_0], pointerF);
        event.GetPointerItem(pIds[INDEX_1], pointerS);
        float xPointF = pointerF.GetGlobalX();
        float xPointS = pointerS.GetGlobalX();
        float yPointF = pointerF.GetGlobalY();
        float yPointS = pointerS.GetGlobalY();
        float offsetX = abs(xPointF - xPointS);
        float offsetY = abs(yPointF - yPointS);
        double duration = hypot(offsetX, offsetY);
        if (duration > miniZoomPointerDistance) {
            // Adjust this event's location.
            MMI::PointerEvent::PointerItem pointer = {};
            event.GetPointerItem(event.GetPointerId(), pointer);
            pointer.SetGlobalX(pointer.GetGlobalX() + DIVIDE_2(offsetX));
            pointer.SetGlobalY(pointer.GetGlobalY() + DIVIDE_2(offsetY));
            event.RemovePointerItem(event.GetPointerId());
            event.AddPointerItem(pointer);
        }
        SendEventToMultimodal(event, NO_CHANGE);
    } else {
        currentState_ = static_cast<int>(TouchGuideState::TRANSMITING);
        SendEventToMultimodal(event, POINTER_UP);
        SendAllDownEvents(event);
    }
}

float TouchGuider::GetAngleCos(float offsetX, float offsetY, bool isGetX)
{
    HILOG_DEBUG();

    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration != 0) {
        ret = ret / duration;
    }
    return ret;
}

bool TouchGuider::IsDragGestureAccept(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointersIdList();
    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
    if (!event.GetPointerItem(pIds[0], pointerF)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[0]);
    }
    if (!event.GetPointerItem(pIds[1], pointerS)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[1]);
    }

    float xPointF = pointerF.GetGlobalX();
    float xPointS = pointerS.GetGlobalX();
    float yPointF = pointerF.GetGlobalY();
    float yPointS = pointerS.GetGlobalY();
    float xPointDownF = receivedRecorder_.pointerDownX[INDEX_0];
    float xPointDownS = receivedRecorder_.pointerDownX[INDEX_1];
    float yPointDownF = receivedRecorder_.pointerDownY[INDEX_0];
    float yPointDownS = receivedRecorder_.pointerDownY[INDEX_1];
    float firstOffsetX = xPointF - xPointDownF;
    float firstOffsetY = yPointF - yPointDownF;
    float secondOffsetX = xPointS - xPointDownS;
    float secondOffsetY = yPointS - yPointDownS;
    if ((firstOffsetX == 0 && firstOffsetY == 0) ||
        (secondOffsetX == 0 && secondOffsetY == 0)) {
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

void TouchGuider::RecordInjectedEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int pointerId = event.GetPointerId();
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
            if (injectedRecorder_.downPointers == 0) {
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

    int pointId = event.GetPointerId();
    MMI::PointerEvent::PointerItem pointer;
    if (!event.GetPointerItem(pointId, pointer)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pointId);
    }
    receivedRecorder_.lastEvent = std::make_shared<MMI::PointerEvent>(event);
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            receivedRecorder_.pointerDownX[pointId] = pointer.GetGlobalX();
            receivedRecorder_.pointerDownY[pointId] = pointer.GetGlobalY();
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            receivedRecorder_.pointerDownX[pointId] = 0;
            receivedRecorder_.pointerDownY[pointId] = 0;
            break;
        default:
            break;
    }
}

void TouchGuider::ClearReceivedEventRecorder()
{
    HILOG_DEBUG();

    memset_s(receivedRecorder_.pointerDownX, sizeof(receivedRecorder_.pointerDownX),
             0, sizeof(receivedRecorder_.pointerDownX));
    memset_s(receivedRecorder_.pointerDownY, sizeof(receivedRecorder_.pointerDownY),
             0, sizeof(receivedRecorder_.pointerDownY));
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

    std::vector<int32_t> pIds = event.GetPointersIdList();
    for (auto& pId : pIds) {
        if (!(injectedRecorder_.downPointers & (1 << pId))) {
            event.SetPointerId(pId);
            SendEventToMultimodal(event, POINTER_DOWN);
        }
    }
}

void TouchGuider::SendUpForAllInjectedEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointersIdList();
    for (auto& pId : pIds) {
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
    handler_->SendEvent(SEND_HOVER_ENTER_MOVE_MSG, 0, DOUBLE_TAP_TIMEOUT);
}

void TouchGuider::PostHoverExit()
{
    HILOG_DEBUG();

    CancelPostEventIfNeed(SEND_HOVER_EXIT_MSG);
    handler_->SendEvent(SEND_HOVER_EXIT_MSG, 0, DOUBLE_TAP_TIMEOUT);
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
                SendEventToMultimodal(*iter, HOVER_MOVE);
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

void TGEventHandler::HoverEnterAndMoveRunner()
{
    HILOG_DEBUG();

    std::list<MMI::PointerEvent> motionEvent = tgServer_.getHoverEnterAndMoveEvent();
    tgServer_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_BEGIN);
    if (!motionEvent.empty()) {
        for (auto iter = motionEvent.begin(); iter != motionEvent.end(); ++iter) {
            tgServer_.SendEventToMultimodal(*iter, HOVER_MOVE);
        }
    }
    tgServer_.ClearHoverEnterAndMoveEvent();
}

void TGEventHandler::HoverExitRunner()
{
    HILOG_DEBUG();

    std::shared_ptr<MMI::PointerEvent> pEvent = tgServer_.getLastReceivedEvent();
    tgServer_.SendEventToMultimodal(*pEvent, HOVER_MOVE);
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