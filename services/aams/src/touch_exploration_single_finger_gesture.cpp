/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "accessibility_touch_exploration.h"
#include "accessibility_window_manager.h"
#include "accessibility_event_info.h"
#include "hilog_wrapper.h"
#include "securec.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {

void TouchExploration::InitOneFingerGestureFuncMap()
{
    handleEventFuncMap_ = {
        {TouchExplorationState::TOUCH_INIT, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleInitStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleInitStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleInitStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)},
            {MMI::PointerEvent::POINTER_ACTION_PULL_MOVE, BIND(HandleInitStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_PULL_UP, BIND(HandleInitStateUp)}}
        },
        {TouchExplorationState::ONE_FINGER_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::ONE_FINGER_LONG_PRESS, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerLongPressStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerLongPressStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerLongPressStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::ONE_FINGER_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::ONE_FINGER_SINGLE_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSingleTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSingleTapThenDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerSingleTapThenDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerSingleTapThenDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    };
}

TouchExplorationEventHandler::TouchExplorationEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchExploration &server): AppExecFwk::EventHandler(runner),
    server_(server)
{
}

void TouchExplorationEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_INFO("TEhandler process event id = %{public}d, currentState is %{public}d", event->GetInnerEventId(),
        server_.GetCurrentState());

    TouchExplorationMsg msg = static_cast<TouchExplorationMsg>(event->GetInnerEventId());
    switch (msg) {
        case TouchExplorationMsg::SEND_HOVER_MSG:
            server_.HoverEventRunner();
            server_.Clear();
            server_.SetCurrentState(TouchExplorationState::TOUCH_INIT);
            break;
        case TouchExplorationMsg::LONG_PRESS_MSG:
            server_.CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
            server_.HoverEventRunner();
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG:
            if (!(server_.SendDoubleTapAndLongPressDownEvent())) {
                server_.SetCurrentState(TouchExplorationState::INVALID);
                return;
            }
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS);
            break;
        case TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG:
            server_.HoverEventRunner();
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG:
            if (server_.GetCurrentState() == TouchExplorationState::TWO_FINGERS_TAP ||
                server_.GetCurrentState() == TouchExplorationState::THREE_FINGERS_TAP ||
                server_.GetCurrentState() == TouchExplorationState::FOUR_FINGERS_TAP) {
                server_.Clear();
                server_.SetCurrentState(TouchExplorationState::INVALID);
            }
            break;
        default:
            server_.ProcessMultiFingerGesture(msg);
            break;
    }
}

TouchExploration::TouchExploration()
{
    InitOneFingerGestureFuncMap();
    InitTwoFingerGestureFuncMap();
    InitThreeFingerGestureFuncMap();
    InitFourFingerGestureFuncMap();

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (display == nullptr) {
        HILOG_ERROR("get display is nullptr");
        return;
    }
    moveThreshold_ = CalculateMoveThreshold(display->GetDpi());
    xMinPixels_ = display->GetWidth() * PIXEL_MULTIPLIER;
    yMinPixels_ = display->GetHeight() * PIXEL_MULTIPLIER;
    float density = display->GetVirtualPixelRatio();
    multiTapOffsetThresh_ = static_cast<int32_t>(density * MULTI_TAP_SLOP + MULTI_TAP_SLOP_DELTA);
    mMinPixelsBetweenSamplesX_ = display->GetWidth() * PIXEL_MULTIPLIER;
    mMinPixelsBetweenSamplesY_ = display->GetHeight() * PIXEL_MULTIPLIER;
#else
    HILOG_WARN("not support display manager");
    moveThreshold_ = 1;
    xMinPixels_ = 1;
    yMinPixels_ = 1;
    multiTapOffsetThresh_ = static_cast<int32_t>(1 * MULTI_TAP_SLOP + MULTI_TAP_SLOP_DELTA);
    mMinPixelsBetweenSamplesX_ = 1;
    mMinPixelsBetweenSamplesY_ = 1;
#endif
}

void TouchExploration::StartUp()
{
    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    handler_ = std::make_shared<TouchExplorationEventHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }

    gestureHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureRunner());
}

bool TouchExploration::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("PointerAction:%{public}d, PointerId:%{public}d, currentState:%{public}d.", event.GetPointerAction(),
        event.GetPointerId(), static_cast<int32_t>(GetCurrentState()));
    if (event.GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    MMI::PointerEvent::PointerItem pointerIterm;
    std::vector<int32_t> pIds = event.GetPointerIds();
    for (auto& pId : pIds) {
        if (!event.GetPointerItem(pId, pointerIterm)) {
            HILOG_ERROR("get pointerItem(%{public}d) failed", pId);
            return false;
        }
        if (pointerIterm.GetToolType() == MMI::PointerEvent::TOOL_TYPE_PEN) {
            EventTransmission::OnPointerEvent(event);
            return false;
        }
    }

    // Send touch event to AA to control volume adjustment.
    SendTouchEventToAA(event);

    HandlePointerEvent(event);
    return true;
}

void TouchExploration::HandlePointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (GetCurrentState() == TouchExplorationState::PASSING_THROUGH) {
        HandlePassingThroughState(event);
        return;
    }
    if (GetCurrentState() == TouchExplorationState::INVALID) {
        HandleInvalidState(event);
        return;
    }
    if (GetCurrentState() == TouchExplorationState::ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS) {
        HandleOneFingerDoubleTapAndLongPressState(event);
        return;
    }

    auto iter = handleEventFuncMap_.find(GetCurrentState());
    if (iter != handleEventFuncMap_.end()) {
        auto funcMap = iter->second.find(event.GetPointerAction());
        if (funcMap != iter->second.end()) {
            funcMap->second(event);
            return;
        }
    }

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);
    // If the event is not processed, GetCurrentState() is set to TOUCH_INIT when the last finger is lifted.
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1) ||
        !pointerIterm.IsPressed()) {
        Clear();
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::SendAccessibilityEventToAA(EventType eventType)
{
    HILOG_INFO("eventType is 0x%{public}x.", eventType);

    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(eventType);
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
    eventInfo.SetWindowId(windowsId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
}

void TouchExploration::SendTouchEventToAA(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_1) ||
        event.GetPointerId() >= SIMULATE_POINTER_ID) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);
    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
        SendAccessibilityEventToAA(EventType::TYPE_TOUCH_BEGIN);
    } else if (!pointerIterm.IsPressed()) {
        SendAccessibilityEventToAA(EventType::TYPE_TOUCH_END);
    }
}

void TouchExploration::SendGestureEventToAA(GestureType gestureId)
{
    HILOG_INFO("gestureId is %{public}d.", static_cast<int32_t>(gestureId));

    AccessibilityEventInfo eventInfo {};
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
    eventInfo.SetWindowId(windowsId);
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    eventInfo.SetGestureType(gestureId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
}

void TouchExploration::SendEventToMultimodal(MMI::PointerEvent event, ChangeAction action)
{
    HILOG_DEBUG("action:%{public}d, SourceType:%{public}d.", action, event.GetSourceType());

    switch (action) {
        case ChangeAction::HOVER_MOVE:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);
            break;
        case ChangeAction::POINTER_DOWN:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
            break;
        case ChangeAction::POINTER_UP:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
            break;
        case ChangeAction::HOVER_ENTER:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER);
            break;
        case ChangeAction::HOVER_EXIT:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
            break;
        case ChangeAction::HOVER_CANCEL:
            event.SetPointerAction(MMI::PointerEvent::POINTER_ACTION_HOVER_CANCEL);
            break;
        default:
            break;
    }
    EventTransmission::OnPointerEvent(event);
}

void TouchExploration::SendScreenWakeUpEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    // Send move event to wake up the screen and prevent the screen from turning off.
    MMI::PointerEvent::PointerItem pointerItem {};
    for (auto& pId : event.GetPointerIds()) {
        event.GetPointerItem(pId, pointerItem);
        pointerItem.SetPressed(false);
        event.RemovePointerItem(pId);
        event.AddPointerItem(pointerItem);
    }
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
}

void TouchExploration::HoverEventRunner()
{
    for (auto& event : receivedPointerEvents_) {
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
            SendEventToMultimodal(event, ChangeAction::HOVER_ENTER);
        } else if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
            SendEventToMultimodal(event, ChangeAction::HOVER_MOVE);
        } else if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
            SendEventToMultimodal(event, ChangeAction::HOVER_EXIT);
        }
    }
}

void TouchExploration::HandleInitStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        receivedPointerEvents_.push_back(event);
        SetCurrentState(TouchExplorationState::ONE_FINGER_DOWN);
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::SEND_HOVER_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::LONG_PRESS_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
        return;
    }

    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(TouchExplorationState::PASSING_THROUGH);
}

void TouchExploration::HandleInitStateUp(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(TouchExplorationState::PASSING_THROUGH);
    }
}

void TouchExploration::HandleInitStateMove(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(TouchExplorationState::PASSING_THROUGH);
}

void TouchExploration::HandlePassingThroughState(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleInvalidState(MMI::PointerEvent &event)
{
    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        SendScreenWakeUpEvent(event);
    }

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleCancelEvent(MMI::PointerEvent &event)
{
    if (GetCurrentState() == TouchExplorationState::TWO_FINGERS_DRAG && event.GetPointerId() == draggingPid_) {
        SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    }
    SendEventToMultimodal(event, ChangeAction::HOVER_CANCEL);

    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        Clear();
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleOneFingerDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
    CancelPostEvent(TouchExplorationMsg::LONG_PRESS_MSG);
    draggingPid_ = event.GetPointerId();
    if (!handler_->HasInnerEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
        return;
    }

    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);
}

void TouchExploration::HandleOneFingerDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::LONG_PRESS_MSG);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    receivedPointerEvents_.push_back(event);
    SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP);
}

void TouchExploration::HandleOneFingerDownStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent startPointerEvent = receivedPointerEvents_.front();
    MMI::PointerEvent::PointerItem startPointerIterm;
    startPointerEvent.GetPointerItem(startPointerEvent.GetPointerId(), startPointerIterm);

    float offsetX = startPointerIterm.GetDisplayX() - pointerIterm.GetDisplayX();
    float offsetY = startPointerIterm.GetDisplayY() - pointerIterm.GetDisplayY();
    double duration = hypot(offsetX, offsetY);
    if (duration > moveThreshold_) {
        CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(TouchExplorationMsg::LONG_PRESS_MSG);
        receivedPointerEvents_.clear();
        receivedPointerEvents_.push_back(event);
        oneFingerSwipePrePointer_ = startPointerIterm;
        Pointer mp;
        mp.px_ = static_cast<float>(startPointerIterm.GetDisplayX());
        mp.py_ = static_cast<float>(startPointerIterm.GetDisplayY());
        oneFingerSwipeRoute_.clear();
        oneFingerSwipeRoute_.push_back(mp);
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::SWIPE_COMPLETE_TIMEOUT));
        SetCurrentState(TouchExplorationState::ONE_FINGER_SWIPE);
        SendScreenWakeUpEvent(event);
    }
}

void TouchExploration::HandleOneFingerLongPressStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    draggingPid_ = event.GetPointerId();
    SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
}

void TouchExploration::HandleOneFingerLongPressStateUp(MMI::PointerEvent &event)
{
    Clear();
    SendEventToMultimodal(event, ChangeAction::HOVER_EXIT);
    SetCurrentState(TouchExplorationState::TOUCH_INIT);
}

void TouchExploration::HandleOneFingerLongPressStateMove(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::HOVER_MOVE);
}

void TouchExploration::HandleOneFingerSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::AddOneFingerSwipeEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("received pointer event is null!");
        return;
    }

    MMI::PointerEvent preMoveEvent = receivedPointerEvents_.back();
    MMI::PointerEvent::PointerItem preMovePointerIterm {};
    preMoveEvent.GetPointerItem(preMoveEvent.GetPointerId(), preMovePointerIterm);
    float offsetX = preMovePointerIterm.GetDisplayX() - pointerIterm.GetDisplayX();
    float offsetY = preMovePointerIterm.GetDisplayY() - pointerIterm.GetDisplayY();
    double duration = hypot(offsetX, offsetY);
    if (duration > moveThreshold_) {
        receivedPointerEvents_.push_back(event);
        CancelPostEvent(TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::SWIPE_COMPLETE_TIMEOUT));
    }

    if ((abs(pointerIterm.GetDisplayX() - oneFingerSwipePrePointer_.GetDisplayX())) >= xMinPixels_ ||
        (abs(pointerIterm.GetDisplayY() - oneFingerSwipePrePointer_.GetDisplayY())) >= yMinPixels_) {
        Pointer mp;
        oneFingerSwipePrePointer_ = pointerIterm;
        mp.px_ = pointerIterm.GetDisplayX();
        mp.py_ = pointerIterm.GetDisplayY();
        oneFingerSwipeRoute_.push_back(mp);
    }
}

std::vector<Pointer> TouchExploration::GetOneFingerSwipePath()
{
    HILOG_DEBUG();
    std::vector<Pointer> pointerPath;
    Pointer firstSeparation = oneFingerSwipeRoute_[0];
    Pointer nextPoint;
    Pointer newSeparation;
    float xUnitVector = 0;
    float yUnitVector = 0;
    float xVector = 0;
    float yVector = 0;
    float vectorLength = 0;
    int32_t numSinceFirstSep = 0;

    pointerPath.push_back(firstSeparation);
    for (size_t i = 1; i < oneFingerSwipeRoute_.size(); i++) {
        nextPoint = oneFingerSwipeRoute_[i];
        if (numSinceFirstSep > 0) {
            xVector = xUnitVector / numSinceFirstSep;
            yVector = yUnitVector / numSinceFirstSep;
            newSeparation.px_ = vectorLength * xVector + firstSeparation.px_;
            newSeparation.py_ = vectorLength * yVector + firstSeparation.py_;

            float xNextUnitVector = nextPoint.px_ - newSeparation.px_;
            float yNextUnitVector = nextPoint.py_ - newSeparation.py_;
            float nextVectorLength = hypot(xNextUnitVector, yNextUnitVector);
            if (nextVectorLength > EPSINON) {
                xNextUnitVector /= nextVectorLength;
                yNextUnitVector /= nextVectorLength;
            }

            if ((xVector * xNextUnitVector + yVector * yNextUnitVector) < DEGREES_THRESHOLD) {
                pointerPath.push_back(newSeparation);
                firstSeparation = newSeparation;
                xUnitVector = 0;
                yUnitVector = 0;
                numSinceFirstSep = 0;
            }
        }
        xVector = nextPoint.px_ - firstSeparation.px_;
        yVector = nextPoint.py_ - firstSeparation.py_;
        vectorLength = hypot(xVector, yVector);
        numSinceFirstSep += 1;
        if (vectorLength > EPSINON) {
            xUnitVector += xVector / vectorLength;
            yUnitVector += yVector / vectorLength;
        }
    }
    pointerPath.push_back(nextPoint);
    return pointerPath;
}

int32_t TouchExploration::GetSwipeDirection(const int32_t dx, const int32_t dy)
{
    if (abs(dx) > abs(dy)) {
        return dx > EPSINON ? SWIPE_RIGHT : SWIPE_LEFT;
    } else {
        return dy < EPSINON ? SWIPE_UP : SWIPE_DOWN;
    }
}

void TouchExploration::HandleOneFingerSwipeStateUp(MMI::PointerEvent &event)
{
    AddOneFingerSwipeEvent(event);
    CancelPostEvent(TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);

    if (oneFingerSwipeRoute_.size() < LIMIT_SIZE_TWO) {
        Clear();
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
        return;
    }

    std::vector<Pointer> pointerPath = GetOneFingerSwipePath();
    if (pointerPath.size() == LIMIT_SIZE_TWO) {
        int32_t swipeDirection = GetSwipeDirection(pointerPath[1].px_ - pointerPath[0].px_,
            pointerPath[1].py_ - pointerPath[0].py_);
        SendGestureEventToAA(GESTURE_DIRECTION[swipeDirection]);
    } else if (pointerPath.size() == LIMIT_SIZE_THREE) {
        int32_t swipeDirectionH = GetSwipeDirection(pointerPath[1].px_ - pointerPath[0].px_,
            pointerPath[1].py_ - pointerPath[0].py_);
        int32_t swipeDirectionHV = GetSwipeDirection(pointerPath[2].px_ - pointerPath[1].px_,
            pointerPath[2].py_ - pointerPath[1].py_);
        SendGestureEventToAA(GESTURE_DIRECTION_TO_ID[swipeDirectionH][swipeDirectionHV]);
    }

    Clear();
    SetCurrentState(TouchExplorationState::TOUCH_INIT);
}

void TouchExploration::HandleOneFingerSwipeStateMove(MMI::PointerEvent &event)
{
    AddOneFingerSwipeEvent(event);
    SendScreenWakeUpEvent(event);
}

bool TouchExploration::RecordFocusedLocation(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    AccessibilityElementInfo focusedElementInfo {};
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().FindFocusedElement(focusedElementInfo,
        FIND_FOCUS_TIMEOUT);
    if (!ret) {
        HILOG_ERROR("find focused element failed.");
        return false;
    }

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    offsetX_ = (focusedElementInfo.GetRectInScreen().GetLeftTopXScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomXScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayX();
    offsetY_ = (focusedElementInfo.GetRectInScreen().GetLeftTopYScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomYScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayY();
    return true;
}

void TouchExploration::HandleOneFingerSingleTapStateDown(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
    if (receivedPointerEvents_.empty()) {
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    MMI::PointerEvent::PointerItem curPointerItem;
    event.GetPointerItem(event.GetPointerId(), curPointerItem);
    MMI::PointerEvent preDownEvent = receivedPointerEvents_.front();
    MMI::PointerEvent::PointerItem preDownPointerItem;
    preDownEvent.GetPointerItem(preDownEvent.GetPointerId(), preDownPointerItem);
    int32_t durationX = preDownPointerItem.GetDisplayX() - curPointerItem.GetDisplayX();
    int32_t durationY = preDownPointerItem.GetDisplayY() - curPointerItem.GetDisplayY();
    if (durationX * durationX + durationY * durationY > multiTapOffsetThresh_ * multiTapOffsetThresh_) {
        HoverEventRunner();
        Clear();
        receivedPointerEvents_.push_back(event);
        SetCurrentState(TouchExplorationState::ONE_FINGER_DOWN);
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::SEND_HOVER_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::LONG_PRESS_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
        return;
    }

    Clear();
    receivedPointerEvents_.push_back(event);
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
    SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
}

void TouchExploration::HandleOneFingerSingleTapThenDownStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleOneFingerSingleTapThenDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    Clear();
    SetCurrentState(TouchExplorationState::TOUCH_INIT);

    if (!gestureHandler_) {
        HILOG_ERROR("gestureHandler is nullptr!");
        return;
    }

    gestureHandler_->PostTask([this]() {
        Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(
            ActionType::ACCESSIBILITY_ACTION_CLICK);
        }, "TASK_CLICK_ON_FOCUS");
}

void TouchExploration::HandleOneFingerSingleTapThenDownStateMove(MMI::PointerEvent &event)
{
    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent startPointerEvent = receivedPointerEvents_.front();
    MMI::PointerEvent::PointerItem startPointerIterm;
    startPointerEvent.GetPointerItem(startPointerEvent.GetPointerId(), startPointerIterm);

    float offsetX = startPointerIterm.GetDisplayX() - pointerIterm.GetDisplayX();
    float offsetY = startPointerIterm.GetDisplayY() - pointerIterm.GetDisplayY();
    double duration = hypot(offsetX, offsetY);
    if (duration > moveThreshold_) {
        CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::OffsetEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("received pointer event is null!");
        return;
    }

    MMI::PointerEvent startPointerEvent = receivedPointerEvents_.front();
    if (event.GetPointerId() != startPointerEvent.GetPointerId()) {
        return;
    }

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    pointer.SetDisplayX(offsetX_ + pointer.GetDisplayX());
    pointer.SetDisplayY(offsetY_ + pointer.GetDisplayY());
    event.RemovePointerItem(event.GetPointerId());
    event.AddPointerItem(pointer);
}

bool TouchExploration::SendDoubleTapAndLongPressDownEvent()
{
    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("receivedPointerEvents_ is empty!");
        return false;
    }
    if (!RecordFocusedLocation(receivedPointerEvents_.front())) {
        return false;
    }
    OffsetEvent(receivedPointerEvents_.front());
    SendEventToMultimodal(receivedPointerEvents_.front(), ChangeAction::NO_CHANGE);
    return true;
}

void TouchExploration::HandleOneFingerDoubleTapAndLongPressState(MMI::PointerEvent &event)
{
    OffsetEvent(event);
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointer.IsPressed())) {
        Clear();
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::Clear()
{
    receivedPointerEvents_.clear();
    draggingDownEvent_ = nullptr;
    offsetX_ = 0;
    offsetY_ = 0;
    oneFingerSwipeRoute_.clear();
    oneFingerSwipePrePointer_ = {};
    draggingPid_ = -1;
    multiTapNum_ = 0;
    multiFingerSwipeDirection_ = -1;
    multiFingerSwipeRoute_.clear();
    multiFingerSwipePrePoint_.clear();
}
} // namespace Accessibility
} // namespace OHOS