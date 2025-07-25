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

void MockTouchExploration::InitOneFingerGestureFuncMap()
{
    handleEventFuncMap_ = {
        {MockTouchExplorationState::TOUCH_INIT, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleInitStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleInitStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleInitStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)},
            {MMI::PointerEvent::POINTER_ACTION_PULL_MOVE, BIND(HandleInitStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_PULL_UP, BIND(HandleInitStateUp)}}
        },
        {MockTouchExplorationState::ONE_FINGER_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::ONE_FINGER_LONG_PRESS, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerLongPressStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerLongPressStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerLongPressStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::ONE_FINGER_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::ONE_FINGER_SINGLE_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSingleTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleOneFingerSingleTapThenDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleOneFingerSingleTapThenDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleOneFingerSingleTapThenDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    };
}

MockTouchExplorationEventHandler::MockTouchExplorationEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, MockTouchExploration &server): AppExecFwk::EventHandler(runner),
    server_(server)
{
}

void MockTouchExplorationEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_INFO("TEhandler process event id = %{public}d, currentState is %{public}d", event->GetInnerEventId(),
        server_.GetCurrentState());

    MockTouchExplorationMsg msg = static_cast<MockTouchExplorationMsg>(event->GetInnerEventId());
    switch (msg) {
        case MockTouchExplorationMsg::SEND_HOVER_MSG:
            server_.HoverEventRunner();
            server_.Clear();
            server_.SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
            break;
        case MockTouchExplorationMsg::LONG_PRESS_MSG:
            server_.CancelPostEvent(MockTouchExplorationMsg::SEND_HOVER_MSG);
            server_.HoverEventRunner();
            server_.SetCurrentState(MockTouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case MockTouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG:
            if (!(server_.SendDoubleTapAndLongPressDownEvent())) {
                server_.SetCurrentState(MockTouchExplorationState::INVALID);
                return;
            }
            server_.SetCurrentState(MockTouchExplorationState::ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS);
            break;
        case MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG:
            server_.HoverEventRunner();
            server_.SetCurrentState(MockTouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG:
            if (server_.GetCurrentState() == MockTouchExplorationState::TWO_FINGERS_TAP ||
                server_.GetCurrentState() == MockTouchExplorationState::THREE_FINGERS_TAP ||
                server_.GetCurrentState() == MockTouchExplorationState::FOUR_FINGERS_TAP) {
                server_.Clear();
                server_.SetCurrentState(MockTouchExplorationState::INVALID);
            }
            break;
        default:
            server_.ProcessMultiFingerGesture(msg);
            break;
    }
}

MockTouchExploration::MockTouchExploration()
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

void MockTouchExploration::StartUp()
{
    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    handler_ = std::make_shared<MockTouchExplorationEventHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }

    gestureHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureRunner());
}

bool MockTouchExploration::OnPointerEvent(MMI::PointerEvent &event)
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

void MockTouchExploration::HandlePointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (GetCurrentState() == MockTouchExplorationState::PASSING_THROUGH) {
        HandlePassingThroughState(event);
        return;
    }
    if (GetCurrentState() == MockTouchExplorationState::INVALID) {
        HandleInvalidState(event);
        return;
    }
    if (GetCurrentState() == MockTouchExplorationState::ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS) {
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
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::SendAccessibilityEventToAA(EventType eventType)
{
    HILOG_INFO("eventType is 0x%{public}x.", eventType);

    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(eventType);
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
    eventInfo.SetWindowId(windowsId);
    AccessibilityEventInfoParcel eventInfoParcel(eventInfo);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfoParcel, 0);
}

void MockTouchExploration::SendTouchEventToAA(MMI::PointerEvent &event)
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

void MockTouchExploration::SendGestureEventToAA(GestureType gestureId)
{
    HILOG_INFO("gestureId is %{public}d.", static_cast<int32_t>(gestureId));

    AccessibilityEventInfo eventInfo {};
    int32_t windowsId = Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
    eventInfo.SetWindowId(windowsId);
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    eventInfo.SetGestureType(gestureId);
    AccessibilityEventInfoParcel eventInfoParcel(eventInfo);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfoParcel, 0);
}

void MockTouchExploration::SendEventToMultimodal(MMI::PointerEvent event, ChangeAction action)
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

void MockTouchExploration::SendScreenWakeUpEvent(MMI::PointerEvent &event)
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

void MockTouchExploration::HoverEventRunner()
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

void MockTouchExploration::HandleInitStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        receivedPointerEvents_.push_back(event);
        SetCurrentState(MockTouchExplorationState::ONE_FINGER_DOWN);
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::SEND_HOVER_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::LONG_PRESS_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
        return;
    }

    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(MockTouchExplorationState::PASSING_THROUGH);
}

void MockTouchExploration::HandleInitStateUp(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(MockTouchExplorationState::PASSING_THROUGH);
    }
}

void MockTouchExploration::HandleInitStateMove(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(MockTouchExplorationState::PASSING_THROUGH);
}

void MockTouchExploration::HandlePassingThroughState(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleInvalidState(MMI::PointerEvent &event)
{
    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        SendScreenWakeUpEvent(event);
    }

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleCancelEvent(MMI::PointerEvent &event)
{
    if (GetCurrentState() == MockTouchExplorationState::TWO_FINGERS_DRAG && event.GetPointerId() == draggingPid_) {
        SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    }
    SendEventToMultimodal(event, ChangeAction::HOVER_CANCEL);

    MMI::PointerEvent::PointerItem pointerIterm {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleOneFingerDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::SEND_HOVER_MSG);
    CancelPostEvent(MockTouchExplorationMsg::LONG_PRESS_MSG);
    draggingPid_ = event.GetPointerId();
    if (!handler_->HasInnerEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        SetCurrentState(MockTouchExplorationState::TWO_FINGERS_UNKNOWN);
        return;
    }

    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    SetCurrentState(MockTouchExplorationState::TWO_FINGERS_DOWN);
}

void MockTouchExploration::HandleOneFingerDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(MockTouchExplorationMsg::LONG_PRESS_MSG);
    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    receivedPointerEvents_.push_back(event);
    SetCurrentState(MockTouchExplorationState::ONE_FINGER_SINGLE_TAP);
}

void MockTouchExploration::HandleOneFingerDownStateMove(MMI::PointerEvent &event)
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
        CancelPostEvent(MockTouchExplorationMsg::SEND_HOVER_MSG);
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(MockTouchExplorationMsg::LONG_PRESS_MSG);
        receivedPointerEvents_.clear();
        receivedPointerEvents_.push_back(event);
        oneFingerSwipePrePointer_ = startPointerIterm;
        Pointer mp;
        mp.px_ = static_cast<float>(startPointerIterm.GetDisplayX());
        mp.py_ = static_cast<float>(startPointerIterm.GetDisplayY());
        oneFingerSwipeRoute_.clear();
        oneFingerSwipeRoute_.push_back(mp);
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::SWIPE_COMPLETE_TIMEOUT));
        SetCurrentState(MockTouchExplorationState::ONE_FINGER_SWIPE);
        SendScreenWakeUpEvent(event);
    }
}

void MockTouchExploration::HandleOneFingerLongPressStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    draggingPid_ = event.GetPointerId();
    SetCurrentState(MockTouchExplorationState::TWO_FINGERS_UNKNOWN);
}

void MockTouchExploration::HandleOneFingerLongPressStateUp(MMI::PointerEvent &event)
{
    Clear();
    SendEventToMultimodal(event, ChangeAction::HOVER_EXIT);
    SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
}

void MockTouchExploration::HandleOneFingerLongPressStateMove(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::HOVER_MOVE);
}

void MockTouchExploration::HandleOneFingerSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::AddOneFingerSwipeEvent(MMI::PointerEvent &event)
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
        CancelPostEvent(MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG), 0,
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

std::vector<Pointer> MockTouchExploration::GetOneFingerSwipePath()
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

int32_t MockTouchExploration::GetSwipeDirection(const int32_t dx, const int32_t dy)
{
    if (abs(dx) > abs(dy)) {
        return dx > EPSINON ? SWIPE_RIGHT : SWIPE_LEFT;
    } else {
        return dy < EPSINON ? SWIPE_UP : SWIPE_DOWN;
    }
}

void MockTouchExploration::HandleOneFingerSwipeStateUp(MMI::PointerEvent &event)
{
    AddOneFingerSwipeEvent(event);
    CancelPostEvent(MockTouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG);

    if (oneFingerSwipeRoute_.size() < LIMIT_SIZE_TWO) {
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
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
    SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
}

void MockTouchExploration::HandleOneFingerSwipeStateMove(MMI::PointerEvent &event)
{
    AddOneFingerSwipeEvent(event);
    SendScreenWakeUpEvent(event);
}

bool MockTouchExploration::RecordFocusedLocation(MMI::PointerEvent &event)
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

    focusedWindowId_ = focusedElementInfo.GetWindowId();

    offsetX_ = (focusedElementInfo.GetRectInScreen().GetLeftTopXScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomXScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayX();
    offsetY_ = (focusedElementInfo.GetRectInScreen().GetLeftTopYScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomYScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayY();
    return true;
}

void MockTouchExploration::HandleOneFingerSingleTapStateDown(MMI::PointerEvent &event)
{
    CancelPostEvent(MockTouchExplorationMsg::SEND_HOVER_MSG);
    if (receivedPointerEvents_.empty()) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
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
        SetCurrentState(MockTouchExplorationState::ONE_FINGER_DOWN);
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::SEND_HOVER_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::LONG_PRESS_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
        return;
    }

    Clear();
    receivedPointerEvents_.push_back(event);
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
    SetCurrentState(MockTouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
}

void MockTouchExploration::HandleOneFingerSingleTapThenDownStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(MockTouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::HandleOneFingerSingleTapThenDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(MockTouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    Clear();
    SetCurrentState(MockTouchExplorationState::TOUCH_INIT);

    if (!gestureHandler_) {
        HILOG_ERROR("gestureHandler is nullptr!");
        return;
    }

    gestureHandler_->PostTask([this]() {
        Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(
            ActionType::ACCESSIBILITY_ACTION_CLICK);
        }, "TASK_CLICK_ON_FOCUS");
}

void MockTouchExploration::HandleOneFingerSingleTapThenDownStateMove(MMI::PointerEvent &event)
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
        CancelPostEvent(MockTouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::OffsetEvent(MMI::PointerEvent &event)
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

    AccessibilityWindowInfo windowInfo = {};
    if (Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindow(focusedWindowId_, windowInfo)) {
        event.SetZOrder(static_cast<float>(windowInfo.GetWindowLayer() + 1));
    } else {
        HILOG_ERROR("get windowInfo failed");
    }

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    pointer.SetTargetWindowId(focusedWindowId_);
    int32_t x = offsetX_ + pointer.GetDisplayX();
    int32_t y = offsetY_ + pointer.GetDisplayY();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    int32_t displayWidth = displayMgr.GetWidth();
    int32_t displayHeight = displayMgr.GetHeight();
    x = x < 0 ? 0 : (x >= displayWidth ? displayWidth - 1 : x);
    y = y < 0 ? 0 : (y >= displayHeight ? displayHeight - 1 : y);
#endif
    pointer.SetDisplayX(x);
    pointer.SetDisplayY(y);
    event.RemovePointerItem(event.GetPointerId());
    event.AddPointerItem(pointer);
}

bool MockTouchExploration::SendDoubleTapAndLongPressDownEvent()
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

void MockTouchExploration::HandleOneFingerDoubleTapAndLongPressState(MMI::PointerEvent &event)
{
    OffsetEvent(event);
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointer.IsPressed())) {
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::Clear()
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

void MockTouchExploration::DestroyEvents()
{
    HILOG_INFO();
    Clear();
    handler_->RemoveAllEvents();
    SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    EventTransmission::DestroyEvents();
}

void MockTouchExploration::InitTwoFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {MockTouchExplorationState::TWO_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::TWO_FINGERS_DRAG, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersDragStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersDragStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersDragStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::TWO_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::TWO_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::TWO_FINGERS_UNKNOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersUnknownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersUnknownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersUnknownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
}

void MockTouchExploration::InitThreeFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {MockTouchExplorationState::THREE_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::THREE_FINGERS_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::THREE_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::THREE_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
}

void MockTouchExploration::InitFourFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {MockTouchExplorationState::THREE_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::FOUR_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::FOUR_FINGERS_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::FOUR_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {MockTouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
}

std::map<MockTouchExplorationMsg, GestureType> MockTouchExploration::GetMultiFingerMsgToGestureMap()
{
    static std::map<MockTouchExplorationMsg, GestureType> MULTI_GESTURE_TYPE = {
        {MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_SINGLE_TAP},
        {MockTouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {MockTouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP},
        {MockTouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD},
        {MockTouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP},
        {MockTouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD},
        {MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_SINGLE_TAP},
        {MockTouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {MockTouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP},
        {MockTouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD},
        {MockTouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP},
        {MockTouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD},
        {MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP},
        {MockTouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {MockTouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP},
        {MockTouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD},
        {MockTouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP},
        {MockTouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD}
    };
    return MULTI_GESTURE_TYPE;
}

void MockTouchExploration::ProcessMultiFingerGesture(MockTouchExplorationMsg msg)
{
    std::map<MockTouchExplorationMsg, GestureType> multiGestureMap = GetMultiFingerMsgToGestureMap();
    if (multiGestureMap.find(msg) == multiGestureMap.end()) {
        return;
    }

    static std::map<GestureType, MockTouchExplorationState> MULTI_FINGER_TAP_GESTURE_TO_STATE = {
        {GestureType::GESTURE_TWO_FINGER_SINGLE_TAP, MockTouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP, MockTouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP, MockTouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_SINGLE_TAP, MockTouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP, MockTouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP, MockTouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP, MockTouchExplorationState::FOUR_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP, MockTouchExplorationState::FOUR_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP, MockTouchExplorationState::FOUR_FINGERS_TAP}
    };
    GestureType gestureType = multiGestureMap.at(msg);
    if (MULTI_FINGER_TAP_GESTURE_TO_STATE.find(gestureType) != MULTI_FINGER_TAP_GESTURE_TO_STATE.end()) {
        // multi-finger multi-tap gesture
        if (GetCurrentState() == MULTI_FINGER_TAP_GESTURE_TO_STATE.at(gestureType)) {
            SendGestureEventToAA(gestureType);
            Clear();
            SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
        }
    } else if (gestureType == GestureType::GESTURE_INVALID) {
        if (GetCurrentState() == MockTouchExplorationState::TWO_FINGERS_DOWN) {
            for (auto& event : receivedPointerEvents_) {
                SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
            }
            Clear();
            SetCurrentState(MockTouchExplorationState::PASSING_THROUGH);
            return;
        }
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    } else {
        // multi-finger multi-tap-and-hold gesture
        SendGestureEventToAA(gestureType);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleTwoFingersDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    SetCurrentState(MockTouchExplorationState::THREE_FINGERS_DOWN);
}

void MockTouchExploration::HandleTwoFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(MockTouchExplorationState::TWO_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        Clear();
        CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

bool MockTouchExploration::GetBasePointItem(MMI::PointerEvent::PointerItem &basePointerIterm, int32_t pId)
{
    HILOG_DEBUG();
    for (auto rIter = receivedPointerEvents_.rbegin(); rIter != receivedPointerEvents_.rend(); ++rIter) {
        if ((rIter->GetPointerId() == pId) && (rIter->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
            rIter->GetPointerItem(pId, basePointerIterm);
            return true;
        }
    }
    return false;
}

void MockTouchExploration::GetPointOffset(MMI::PointerEvent &event, std::vector<float> &firstPointOffset,
    std::vector<float> &secondPointOffset)
{
    HILOG_DEBUG();
    std::vector<int32_t> pIds = event.GetPointerIds();
    if (pIds.size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerF {};
    MMI::PointerEvent::PointerItem pointerS {};
    event.GetPointerItem(pIds[0], pointerF);
    event.GetPointerItem(pIds[1], pointerS);

    MMI::PointerEvent::PointerItem basePointerF {};
    MMI::PointerEvent::PointerItem basePointerS {};
    if (GetBasePointItem(basePointerF, pIds[0]) && GetBasePointItem(basePointerS, pIds[1])) {
        firstPointOffset.push_back(pointerF.GetDisplayX() - basePointerF.GetDisplayX());
        firstPointOffset.push_back(pointerF.GetDisplayY() - basePointerF.GetDisplayY());
        secondPointOffset.push_back(pointerS.GetDisplayX() - basePointerS.GetDisplayX());
        secondPointOffset.push_back(pointerS.GetDisplayY() - basePointerS.GetDisplayY());
    }
}

float MockTouchExploration::GetAngleCos(float offsetX, float offsetY, bool isGetX)
{
    HILOG_DEBUG();
    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration < - EPSINON || duration > EPSINON) {
        ret = ret / duration;
    }
    return ret;
}

bool MockTouchExploration::IsDragGestureAccept(MMI::PointerEvent &event)
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
    return (firstXCos * secondXCos + firstYCos * secondYCos) >= MAX_DRAG_GESTURE_COSINE;
}

void MockTouchExploration::SendDragDownEventToMultimodal(MMI::PointerEvent event)
{
    HILOG_DEBUG();
    std::vector<int32_t> pIds = event.GetPointerIds();
    int32_t xPointDown = 0;
    int32_t yPointDown = 0;
    int64_t actionTime = 0;
    MMI::PointerEvent::PointerItem basePointerIterm {};

    for (auto &baseEvent : receivedPointerEvents_) {
        if ((baseEvent.GetPointerId() == event.GetPointerId()) &&
            (baseEvent.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
            baseEvent.GetPointerItem(event.GetPointerId(), basePointerIterm);
            xPointDown = basePointerIterm.GetDisplayX();
            yPointDown = basePointerIterm.GetDisplayY();
            actionTime = baseEvent.GetActionTime();
            break;
        }
    }

    MMI::PointerEvent::PointerItem pointer {};
    event.GetPointerItem(draggingPid_, pointer);
    pointer.SetDisplayX(xPointDown);
    pointer.SetDisplayY(yPointDown);
    event.RemovePointerItem(draggingPid_);
    event.AddPointerItem(pointer);
    event.SetActionTime(actionTime);
    int32_t removePid = draggingPid_ == pIds[0] ? pIds[1] : pIds[0];
    event.RemovePointerItem(removePid);
    SendEventToMultimodal(event, ChangeAction::POINTER_DOWN);
    draggingDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

bool MockTouchExploration::IsRealMove(MMI::PointerEvent &event)
{
    std::vector<float> firstPointOffset;
    std::vector<float> secondPointOffset;
    GetPointOffset(event, firstPointOffset, secondPointOffset);
    if (firstPointOffset.size() != SCREEN_AXIS_NUM || secondPointOffset.size() != SCREEN_AXIS_NUM) {
        return false;
    }

    return (hypot(firstPointOffset[0], firstPointOffset[1]) >= TOUCH_SLOP &&
        hypot(secondPointOffset[0], secondPointOffset[1]) >= TOUCH_SLOP);
}

void MockTouchExploration::HandleTwoFingersDownStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, event.GetPointerId())) {
        HILOG_ERROR("get base point item failed, pid = %{public}d.", event.GetPointerId());
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (hypot(offsetX, offsetY) > TOUCH_SLOP * static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        HILOG_DEBUG("cancel two-finger tap gesture because finger move");
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
            Clear();
            SendScreenWakeUpEvent(event);
            SetCurrentState(MockTouchExplorationState::INVALID);
            return;
        }
        if (!IsRealMove(event)) {
            HILOG_DEBUG("not a move");
            return;
        }
        if (IsDragGestureAccept(event)) {
            draggingPid_ = event.GetPointerId();
            SendDragDownEventToMultimodal(event);
            SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
            SetCurrentState(MockTouchExplorationState::TWO_FINGERS_DRAG);
        } else {
            for (auto &receivedEvent : receivedPointerEvents_) {
                SendEventToMultimodal(receivedEvent, ChangeAction::NO_CHANGE);
            }
            Clear();
            SetCurrentState(MockTouchExplorationState::PASSING_THROUGH);
        }
    }
}

void MockTouchExploration::SendUpForDragDownEvent()
{
    HILOG_DEBUG();
    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("received pointer event is null!");
        return;
    }

    MMI::PointerEvent lastMoveEvent = receivedPointerEvents_.back();
    MMI::PointerEvent::PointerItem movePointerItem;
    lastMoveEvent.GetPointerItem(lastMoveEvent.GetPointerId(), movePointerItem);
    int32_t x = movePointerItem.GetDisplayX();
    int32_t y = movePointerItem.GetDisplayY();

    if (draggingDownEvent_ == nullptr) {
        HILOG_ERROR("dragging down event is null!");
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem {};
    draggingDownEvent_->SetActionTime(Utils::GetSystemTime() * US_TO_MS);
    draggingDownEvent_->GetPointerItem(draggingDownEvent_->GetPointerId(), pointerItem);
    pointerItem.SetPressed(false);
    pointerItem.SetDisplayX(x);
    pointerItem.SetDisplayY(y);
    draggingDownEvent_->RemovePointerItem(draggingDownEvent_->GetPointerId());
    draggingDownEvent_->AddPointerItem(pointerItem);
    SendEventToMultimodal(*draggingDownEvent_, ChangeAction::POINTER_UP);
}

void MockTouchExploration::HandleTwoFingersDragStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        SendUpForDragDownEvent();
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleTwoFingersDragStateUp(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SendUpForDragDownEvent();
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleTwoFingersDragStateMove(MMI::PointerEvent &event)
{
    std::vector<int32_t> pIds = event.GetPointerIds();
    if (pIds.size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2) || event.GetPointerId() != draggingPid_) {
        return;
    }

    receivedPointerEvents_.push_back(event);

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

    MMI::PointerEvent::PointerItem pointerF {};
    MMI::PointerEvent::PointerItem pointerS {};
    event.GetPointerItem(pIds[0], pointerF);
    event.GetPointerItem(pIds[1], pointerS);
    float xPointF = pointerF.GetDisplayX();
    float xPointS = pointerS.GetDisplayX();
    float yPointF = pointerF.GetDisplayY();
    float yPointS = pointerS.GetDisplayY();
    float offsetX = abs(xPointF - xPointS);
    float offsetY = abs(yPointF - yPointS);
    double duration = hypot(offsetX, offsetY);
    if (duration > miniZoomPointerDistance) {
        // Adjust this event's location.
        MMI::PointerEvent::PointerItem pointer {};
        event.GetPointerItem(event.GetPointerId(), pointer);
        pointer.SetDisplayX(pointer.GetDisplayX() + offsetX / DIVIDE_NUM);
        pointer.SetDisplayY(pointer.GetDisplayY() + offsetY / DIVIDE_NUM);
        event.RemovePointerItem(event.GetPointerId());
        event.AddPointerItem(pointer);
    }
    int32_t removePid = draggingPid_ == pIds[0]? pIds[1] : pIds[0];
    event.RemovePointerItem(removePid);
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
}

bool MockTouchExploration::GetPointerItemWithFingerNum(uint32_t fingerNum,
    std::vector<MMI::PointerEvent::PointerItem> &curPoints,
    std::vector<MMI::PointerEvent::PointerItem> &prePoints, MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (fingerNum > static_cast<uint32_t>(INT32_MAX)) {
        HILOG_ERROR("fingerNum is invalid!");
        return false;
    }

    bool findPrePointer = false;
    std::vector<int32_t> pIds = event.GetPointerIds();
    for (int32_t i = 0; i < static_cast<int32_t>(fingerNum); i++) {
        findPrePointer = false;
        event.GetPointerItem(pIds[i], curPoints[i]);
        for (auto &preEvent : receivedPointerEvents_) {
            if ((preEvent.GetPointerId() == i) &&
                (preEvent.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
                findPrePointer = true;
                preEvent.GetPointerItem(i, prePoints[i]);
                break;
            }
        }
        if (!findPrePointer) {
            HILOG_ERROR("get prePointerItem(%{public}d) failed", i);
            return false;
        }
    }
    return true;
}

bool MockTouchExploration::IsMultiFingerMultiTap(MMI::PointerEvent &event, const uint32_t fingerNum)
{
    HILOG_DEBUG();
    std::vector<MMI::PointerEvent::PointerItem> curPoints(fingerNum);
    std::vector<MMI::PointerEvent::PointerItem> prePoints(fingerNum);
    if (!GetPointerItemWithFingerNum(fingerNum, curPoints, prePoints, event)) {
        return false;
    }

    std::vector<int32_t> excludePid(fingerNum, -1);
    for (auto curPoint : curPoints) {
        float moveDelta = FLT_MAX;
        int32_t nearestPid = -1;
        int32_t curX = curPoint.GetDisplayX();
        int32_t curY = curPoint.GetDisplayY();
        for (auto prePoint : prePoints) {
            int32_t pId = prePoint.GetPointerId();
            if (std::find(excludePid.begin(), excludePid.end(), pId) != excludePid.end()) {
                continue;
            }
            int32_t preX = prePoint.GetDisplayX();
            int32_t preY = prePoint.GetDisplayY();
            int32_t offsetX = curX - preX;
            int32_t offsetY = curY - preY;
            if (offsetX == 0 && offsetY == 0) {
                nearestPid = pId;
                moveDelta = 0;
                break;
            }

            float delta = hypot(offsetX, offsetY);
            if (delta < moveDelta) {
                moveDelta = delta;
                nearestPid = pId;
            }
        }
        if (moveDelta < multiTapOffsetThresh_ * fingerNum) {
            excludePid.push_back(nearestPid);
        } else {
            return false;
        }
    }

    return true;
}

bool MockTouchExploration::IsMultiFingerMultiTapGesture(MMI::PointerEvent &event, const uint32_t fingerNum)
{
    HILOG_DEBUG();

    if (fingerNum < static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return false;
    }

    if (IsMultiFingerMultiTap(event, fingerNum)) {
        multiTapNum_ = multiTapNum_ + 1;
        HILOG_DEBUG("%{public}d finger %{public}d tap is recognized", static_cast<int32_t>(fingerNum),
            multiTapNum_ + 1);
    } else {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return false;
    }

    uint32_t fingerNumIndex = fingerNum - 2;
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_TAP_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_HOLD_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    return true;
}

void MockTouchExploration::HandleMultiFingersTapStateDown(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapEvent();
    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);

    if (multiTapNum_ == TAP_COUNT_MAXIMUM - 1) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize < fingerNum) {
        handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    } else if (pointerSize == fingerNum) {
        if (IsMultiFingerMultiTapGesture(event, fingerNum)) {
            if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
                SetCurrentState(MockTouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);
            } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
                SetCurrentState(MockTouchExplorationState::THREE_FINGERS_CONTINUE_DOWN);
            } else {
                SetCurrentState(MockTouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN);
            }
        }
    } else {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleTwoFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void MockTouchExploration::HandleMultiFingersTapStateUp(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    } else {
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleMultiFingersTapStateMove(MMI::PointerEvent &event, uint32_t fingerNum)
{
    if (event.GetPointerIds().size() >= fingerNum) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, event.GetPointerId())) {
        HILOG_ERROR("get base point item failed, pid = %{public}d.", event.GetPointerId());
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (hypot(offsetX, offsetY) > TOUCH_SLOP * (fingerNum - 1)) {
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }

    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleTwoFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void MockTouchExploration::HandleMultiFingersContinueDownStateDown(MMI::PointerEvent &event)
{
    CancelMultiFingerTapEvent();
    CancelMultiFingerTapAndHoldEvent();
    Clear();
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::HandleMultiFingersContinueDownStateUp(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapAndHoldEvent();

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
            SetCurrentState(MockTouchExplorationState::TWO_FINGERS_TAP);
        } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
            SetCurrentState(MockTouchExplorationState::THREE_FINGERS_TAP);
        } else {
            SetCurrentState(MockTouchExplorationState::FOUR_FINGERS_TAP);
        }
    } else if (pointerSize > fingerNum) {
        CancelMultiFingerTapEvent();
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleTwoFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void MockTouchExploration::HandleMultiFingersContinueDownStateMove(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, event.GetPointerId())) {
        HILOG_ERROR("get base point item failed, pid = %{public}d.", event.GetPointerId());
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (hypot(offsetX, offsetY) > TOUCH_SLOP * fingerNum) {
        Clear();
        CancelMultiFingerTapEvent();
        CancelMultiFingerTapAndHoldEvent();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }

    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleTwoFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void MockTouchExploration::HandleTwoFingersUnknownStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        receivedPointerEvents_.push_back(event);
        return;
    }

    Clear();
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::HandleTwoFingersUnknownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleTwoFingersUnknownStateMove(MMI::PointerEvent &event)
{
    if ((event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) ||
        (event.GetPointerId() != draggingPid_)) {
        return;
    }

    receivedPointerEvents_.push_back(event);

    std::vector<int32_t> pIds = event.GetPointerIds();
    if (IsDragGestureAccept(event)) {
        int32_t removePid = draggingPid_ == pIds[0] ? pIds[1] : pIds[0];
        event.RemovePointerItem(removePid);
        SendEventToMultimodal(event, ChangeAction::POINTER_DOWN);
        draggingDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
        SetCurrentState(MockTouchExplorationState::TWO_FINGERS_DRAG);
        return;
    }
}

void MockTouchExploration::HandleThreeFingersDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
        return;
    }

    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(MockTouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    SetCurrentState(MockTouchExplorationState::FOUR_FINGERS_DOWN);
}

void MockTouchExploration::HandleThreeFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(MockTouchExplorationState::THREE_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
        Clear();
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::StoreMultiFingerSwipeBaseDownPoint()
{
    HILOG_DEBUG();
    for (auto& event : receivedPointerEvents_) {
        if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN) {
            Pointer mp;
            MMI::PointerEvent::PointerItem pointerIterm;
            std::vector<Pointer> mpVec;
            int32_t pId = event.GetPointerId();
            event.GetPointerItem(pId, pointerIterm);
            mp.px_ = static_cast<float>(pointerIterm.GetDisplayX());
            mp.py_ = static_cast<float>(pointerIterm.GetDisplayY());
            mpVec.push_back(mp);
            multiFingerSwipeRoute_.insert(std::make_pair(pId, mpVec));
            multiFingerSwipePrePoint_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
        }
    }
}

void MockTouchExploration::HandleThreeFingersDownStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, event.GetPointerId())) {
        HILOG_ERROR("get base point item failed, pid = %{public}d.", event.GetPointerId());
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (hypot(offsetX, offsetY) > TOUCH_SLOP * static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(MockTouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
            Clear();
            SendScreenWakeUpEvent(event);
            SetCurrentState(MockTouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        SetCurrentState(MockTouchExplorationState::THREE_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleThreeFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    SetCurrentState(MockTouchExplorationState::INVALID);
}

bool MockTouchExploration::GetMultiFingerSwipeBasePointerItem(MMI::PointerEvent::PointerItem &basePointerIterm, int32_t pId)
{
    HILOG_DEBUG();
    if (multiFingerSwipePrePoint_.count(pId) == 0 || !multiFingerSwipePrePoint_[pId]) {
        HILOG_ERROR("get base pointEvent(%{public}d) failed", pId);
        return false;
    }
    multiFingerSwipePrePoint_[pId]->GetPointerItem(pId, basePointerIterm);
    return true;
}

bool MockTouchExploration::SaveMultiFingerSwipeGesturePointerInfo(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    MMI::PointerEvent::PointerItem pointerIterm;
    int32_t pId = event.GetPointerId();
    event.GetPointerItem(pId, pointerIterm);

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetMultiFingerSwipeBasePointerItem(basePointerIterm, pId)) {
        HILOG_WARN("get base point item failed, pid = %{public}d.", pId);
        return true;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (abs(offsetX) > mMinPixelsBetweenSamplesX_ || abs(offsetY) > mMinPixelsBetweenSamplesY_) {
        if (multiFingerSwipeDirection_ != GetSwipeDirection(offsetX, offsetY)) {
            Clear();
            if ((event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) &&
                (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1))) {
                SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
            } else {
                SetCurrentState(MockTouchExplorationState::INVALID);
            }
            return false;
        }

        Pointer mp;
        mp.px_ = static_cast<float>(pointerIterm.GetDisplayX());
        mp.py_ = static_cast<float>(pointerIterm.GetDisplayY());
        multiFingerSwipeRoute_[pId].push_back(mp);
        multiFingerSwipePrePoint_[pId] = std::make_shared<MMI::PointerEvent>(event);
    }

    return true;
}

bool MockTouchExploration::RecognizeMultiFingerSwipePath(const std::vector<Pointer> &path)
{
    HILOG_DEBUG();
    if (path.size() < MIN_MULTI_FINGER_SWIPE_POINTER_NUM) {
        return false;
    }

    int pathSize = static_cast<int>(path.size() - 1);
    for (int routerIndex = 0; routerIndex < pathSize; routerIndex++) {
        int32_t dx = static_cast<int32_t>(path[routerIndex + 1].px_ - path[routerIndex].px_);
        int32_t dy = static_cast<int32_t>(path[routerIndex + 1].py_ - path[routerIndex].py_);
        if (GetSwipeDirection(dx, dy) != multiFingerSwipeDirection_) {
            return false;
        }
    }
    return true;
}

GestureType MockTouchExploration::GetMultiFingerSwipeGestureId(uint32_t fingerNum)
{
    HILOG_DEBUG();
    if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        switch (multiFingerSwipeDirection_) {
            case SWIPE_LEFT:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_LEFT;
            case SWIPE_RIGHT:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_RIGHT;
            case SWIPE_UP:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_UP;
            case SWIPE_DOWN:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN;
            default:
                return GestureType::GESTURE_INVALID;
        }
    } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        switch (multiFingerSwipeDirection_) {
            case SWIPE_LEFT:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_LEFT;
            case SWIPE_RIGHT:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_RIGHT;
            case SWIPE_UP:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_UP;
            case SWIPE_DOWN:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_DOWN;
            default:
                return GestureType::GESTURE_INVALID;
        }
    }
    return GestureType::GESTURE_INVALID;
}

void MockTouchExploration::HandleMultiFingersSwipeStateUp(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);

    if (!SaveMultiFingerSwipeGesturePointerInfo(event)) {
        return;
    }

    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        if (fingerNum > static_cast<uint32_t>(INT32_MAX)) {
            SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
            return;
        }
        for (int32_t pIndex = 0; pIndex < static_cast<int32_t>(fingerNum); pIndex++) {
            if (multiFingerSwipeRoute_.count(pIndex) == 0 ||
                multiFingerSwipeRoute_[pIndex].size() < MIN_MULTI_FINGER_SWIPE_POINTER_NUM) {
                Clear();
                SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
                return;
            }
            if (!RecognizeMultiFingerSwipePath(multiFingerSwipeRoute_[pIndex])) {
                Clear();
                SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
                return;
            }
        }

        GestureType gestureId = GetMultiFingerSwipeGestureId(fingerNum);
        SendGestureEventToAA(gestureId);
        Clear();
        SetCurrentState(MockTouchExplorationState::TOUCH_INIT);
    }
}

void MockTouchExploration::HandleThreeFingersSwipeStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersSwipeStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void MockTouchExploration::HandleThreeFingersSwipeStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    SaveMultiFingerSwipeGesturePointerInfo(event);
    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleThreeFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void MockTouchExploration::HandleThreeFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void MockTouchExploration::HandleThreeFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void MockTouchExploration::HandleThreeFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void MockTouchExploration::HandleFourFingersDownStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::HandleFourFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(MockTouchExplorationState::FOUR_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        Clear();
        CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
        SetCurrentState(MockTouchExplorationState::INVALID);
    }
}

void MockTouchExploration::HandleFourFingersDownStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);

    MMI::PointerEvent::PointerItem pointerIterm;
    event.GetPointerItem(event.GetPointerId(), pointerIterm);
    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, event.GetPointerId())) {
        HILOG_ERROR("get base point item failed, pid = %{public}d.", event.GetPointerId());
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (hypot(offsetX, offsetY) > TOUCH_SLOP * static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
            Clear();
            SendScreenWakeUpEvent(event);
            SetCurrentState(MockTouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        SetCurrentState(MockTouchExplorationState::FOUR_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleFourFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    SetCurrentState(MockTouchExplorationState::INVALID);
}

void MockTouchExploration::HandleFourFingersSwipeStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersSwipeStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void MockTouchExploration::HandleFourFingersSwipeStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    SaveMultiFingerSwipeGesturePointerInfo(event);
    SendScreenWakeUpEvent(event);
}

void MockTouchExploration::HandleFourFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void MockTouchExploration::HandleFourFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void MockTouchExploration::HandleFourFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void MockTouchExploration::HandleFourFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void MockTouchExploration::CancelPostEvent(MockTouchExplorationMsg msg)
{
    HILOG_DEBUG("innerEventID = %{public}u", static_cast<uint32_t>(msg));
    if (handler_->HasInnerEvent(static_cast<uint32_t>(msg))) {
        handler_->RemoveEvent(static_cast<uint32_t>(msg));
    }
}

void MockTouchExploration::CancelMultiFingerTapEvent()
{
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_MSG);
}

void MockTouchExploration::CancelMultiFingerTapAndHoldEvent()
{
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(MockTouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(MockTouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(MockTouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
}
} // namespace Accessibility
} // namespace OHOS