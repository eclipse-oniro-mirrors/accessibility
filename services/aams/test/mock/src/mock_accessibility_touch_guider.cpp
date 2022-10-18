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
#include "hilog_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t POINTER_COUNT_1 = 1;
    constexpr int32_t POINTER_COUNT_2 = 2;
} // namespace

TGEventHandler::TGEventHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner, TouchGuider& tgServer)
    : AppExecFwk::EventHandler(runner), tgServer_(tgServer)
{
}

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

void TGEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
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

bool TouchGuider::OnPointerEvent(MMI::PointerEvent& event)
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
    if (gestureRecognizer_.OnPointerEvent(event)) {
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

void TouchGuider::SendEventToMultimodal(MMI::PointerEvent& event, int32_t action)
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

bool TouchGuider::TouchGuideListener::OnDoubleTap(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    MMI::PointerEvent::PointerItem clickPoint = {};
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
    return false;
}

bool TouchGuider::TouchGuideListener::OnCompleted(GestureType gestureId)
{
    HILOG_DEBUG("OnCompleted, gestureId is %{public}d", gestureId);

    if (server_.currentState_ != static_cast<int32_t>(TouchGuideState::TRANSMITTING)) {
        HILOG_DEBUG("OnCompleted, state is not transmitting.");
        return false;
    }
    server_.OnTouchInteractionEnd();
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    server_.CancelPostEvent(EXIT_GESTURE_REC_MSG);
    server_.currentState_ = static_cast<int32_t>(TouchGuideState::TOUCH_GUIDING);

    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    eventInfo.SetGestureType(gestureId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
    return true;
}

bool TouchGuider::TouchGuideListener::OnCancelled(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    switch (static_cast<TouchGuideState>(server_.currentState_)) {
        case TouchGuideState::TRANSMITTING:
            server_.OnTouchInteractionEnd();
            server_.SendAccessibilityEventToAA(EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
            if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP &&
                event.GetPointerIds().size() == POINTER_COUNT_1) {
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

void TouchGuider::HandleTouchGuidingState(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

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
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
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

void TouchGuider::HandleDraggingState(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                Clear(event);
            } else {
                currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
                SendEventToMultimodal(event, POINTER_UP);
                SendAllDownEvents(event);
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

void TouchGuider::HandleTransmitingState(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
                Clear(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (event.GetPointerIds().size() == POINTER_COUNT_1) {
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

void TouchGuider::Clear(MMI::PointerEvent& event)
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

void TouchGuider::HandleTouchGuidingStateInnerDown(MMI::PointerEvent& event)
{
    (void)event;
}

void TouchGuider::HandleTouchGuidingStateInnerMove(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    switch (event.GetPointerIds().size()) {
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
                currentState_ = static_cast<int32_t>(TouchGuideState::DRAGGING);
                SendEventToMultimodal(event, POINTER_DOWN);
            } else {
                currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
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
            currentState_ = static_cast<int32_t>(TouchGuideState::TRANSMITTING);
            SendAllDownEvents(event);
            break;
    }
}

void TouchGuider::HandleDraggingStateInnerMove(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    int32_t pointCount = pIds.size();
    if (pointCount == POINTER_COUNT_1) {
        HILOG_INFO("Only two pointers can be received in the dragging state");
    } else if (pointCount == POINTER_COUNT_2 && IsDragGestureAccept(event)) {
        /* get densityPixels from WMS */
        AccessibilityDisplayManager& displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
        auto display = displayMgr.GetDefaultDisplay();
        float densityPixels = display->GetVirtualPixelRatio();
        int32_t miniZoomPointerDistance = static_cast<int32_t>(MINI_POINTER_DISTANCE_DIP * densityPixels);
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
        SendEventToMultimodal(event, POINTER_UP);
        SendAllDownEvents(event);
    }
}

float TouchGuider::GetAngleCos(float offsetX, float offsetY, bool isGetX)
{
    HILOG_DEBUG();

    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration == 0) {
        return ret;
    }
    ret = ret / duration;
    return ret;
}

bool TouchGuider::IsDragGestureAccept(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
    if (!event.GetPointerItem(pIds[0], pointerF)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[0]);
    }
    if (!event.GetPointerItem(pIds[1], pointerS)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[1]);
    }

    float xPointF = pointerF.GetDisplayX();
    float xPointS = pointerS.GetDisplayX();
    float yPointF = pointerF.GetDisplayY();
    float yPointS = pointerS.GetDisplayY();
    float xPointDownF = receivedRecorder_.pointerDownX[INDEX_0];
    float xPointDownS = receivedRecorder_.pointerDownX[INDEX_1];
    float yPointDownF = receivedRecorder_.pointerDownY[INDEX_0];
    float yPointDownS = receivedRecorder_.pointerDownY[INDEX_1];
    float firstOffsetX = xPointF - xPointDownF;
    float firstOffsetY = yPointF - yPointDownF;
    float secondOffsetX = xPointS - xPointDownS;
    float secondOffsetY = yPointS - yPointDownS;
    if ((!firstOffsetX && !firstOffsetY) || (!secondOffsetX && !secondOffsetY)) {
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

void TouchGuider::RecordInjectedEvent(MMI::PointerEvent& event)
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

void TouchGuider::RecordReceivedEvent(MMI::PointerEvent& event)
{
    HILOG_DEBUG();

    int32_t pointId = event.GetPointerId();
    MMI::PointerEvent::PointerItem pointer;
    if (!event.GetPointerItem(pointId, pointer)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pointId);
    }
    receivedRecorder_.lastEvent = std::make_shared<MMI::PointerEvent>(event);
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            receivedRecorder_.pointerDownX[pointId] = pointer.GetDisplayX();
            receivedRecorder_.pointerDownY[pointId] = pointer.GetDisplayY();
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

    (void)memset_s(receivedRecorder_.pointerDownX,
                   sizeof(receivedRecorder_.pointerDownX),
                   0,
                   sizeof(receivedRecorder_.pointerDownX));
    (void)memset_s(receivedRecorder_.pointerDownY,
                   sizeof(receivedRecorder_.pointerDownY),
                   0,
                   sizeof(receivedRecorder_.pointerDownY));
    receivedRecorder_.lastEvent = nullptr;
}

void TouchGuider::ClearInjectedEventRecorder()
{
    HILOG_DEBUG();

    injectedRecorder_.downPointerNum = 0;
    injectedRecorder_.downPointers = 0;
    injectedRecorder_.lastHoverEvent = nullptr;
}

void TouchGuider::SendAllDownEvents(MMI::PointerEvent& event)
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

void TouchGuider::SendUpForAllInjectedEvent(MMI::PointerEvent& event)
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

void TouchGuider::PostHoverEnterAndMove(MMI::PointerEvent& event)
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

void TouchGuider::ForceSendAndRemoveEvent(uint32_t innerEventID, MMI::PointerEvent& event)
{
    (void)event;
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