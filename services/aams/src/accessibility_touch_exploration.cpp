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

void TouchExploration::InitTwoFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {TouchExplorationState::TWO_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::TWO_FINGERS_DRAG, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersDragStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersDragStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersDragStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::TWO_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::TWO_FINGERS_UNKNOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleTwoFingersUnknownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersUnknownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersUnknownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
}

void TouchExploration::InitThreeFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {TouchExplorationState::THREE_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::THREE_FINGERS_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::THREE_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleThreeFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::THREE_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
}

void TouchExploration::InitFourFingerGestureFuncMap()
{
    handleEventFuncMap_.insert({
        {TouchExplorationState::THREE_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleThreeFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleThreeFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::FOUR_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::FOUR_FINGERS_SWIPE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersSwipeStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersSwipeStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersSwipeStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::FOUR_FINGERS_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleFourFingersTapStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMultiFingersTapStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersTapStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        },
        {TouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleMultiFingersContinueDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleFourFingersContinueDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleFourFingersContinueDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleCancelEvent)}}
        }
    });
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
            break;
        case TouchExplorationMsg::LONG_PRESS_MSG:
            HILOG_INFO("currentState is changed from ONE_FINGER_DOWN to ONE_FINGER_LONG_PRESS.");
            server_.CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
            server_.HoverEventRunner();
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG:
            HILOG_INFO("currentState is changed from ONE_FINGER_SINGLE_TAP_THEN_DOWN to\
                ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS.");
            server_.SendDoubleTapAndLongPressDownEvent();
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS);
            break;
        case TouchExplorationMsg::DOUBLE_TAP_MSG:
            server_.Clear();
            HILOG_INFO("currentState is changed from ONE_FINGER_SINGLE_TAP to TOUCH_INIT.");
            server_.SetCurrentState(TouchExplorationState::TOUCH_INIT);
            break;
        case TouchExplorationMsg::SWIPE_COMPLETE_TIMEOUT_MSG:
            server_.HoverEventRunner();
            HILOG_INFO("timeout, currentState is changed from ONE_FINGER_SWIPE to ONE_FINGER_LONG_PRESS.");
            server_.SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
            break;
        case TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG:
            if (server_.GetCurrentState() == TouchExplorationState::TWO_FINGERS_TAP ||
                server_.GetCurrentState() == TouchExplorationState::THREE_FINGERS_TAP ||
                server_.GetCurrentState() == TouchExplorationState::FOUR_FINGERS_TAP) {
                server_.Clear();
                HILOG_INFO("wait another finger timeout, currentState is changed to INVALID.");
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
    for (auto& pid : pIds) {
        if (!event.GetPointerItem(pid, pointerIterm)) {
            HILOG_ERROR("get pointerItem(%{public}d) failed", pid);
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
        HILOG_INFO("currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
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
    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerIterm = {};
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
    MMI::PointerEvent::PointerItem pointerItem = {};
    for (auto& pid : event.GetPointerIds()) {
        event.GetPointerItem(pid, pointerItem);
        pointerItem.SetPressed(false);
        event.RemovePointerItem(pid);
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

std::map<TouchExplorationMsg, GestureType> TouchExploration::GetMultiFingerMsgToGestureMap()
{
    static std::map<TouchExplorationMsg, GestureType> MULTI_GESTURE_TYPE = {
        {TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_SINGLE_TAP},
        {TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {TouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP},
        {TouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD},
        {TouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP},
        {TouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD},
        {TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_SINGLE_TAP},
        {TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {TouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP},
        {TouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD},
        {TouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP},
        {TouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD},
        {TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP},
        {TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {TouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP},
        {TouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD},
        {TouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_MSG, GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP},
        {TouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG, GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD}
    };
    return MULTI_GESTURE_TYPE;
}

void TouchExploration::ProcessMultiFingerGesture(TouchExplorationMsg msg)
{
    std::map<TouchExplorationMsg, GestureType> multiGestureMap = GetMultiFingerMsgToGestureMap();
    if (multiGestureMap.find(msg) == multiGestureMap.end()) {
        return;
    }

    static std::map<GestureType, TouchExplorationState> MULTI_FINGER_TAP_GESTURE_TO_STATE = {
        {GestureType::GESTURE_TWO_FINGER_SINGLE_TAP, TouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP, TouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP, TouchExplorationState::TWO_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_SINGLE_TAP, TouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP, TouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP, TouchExplorationState::THREE_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP, TouchExplorationState::FOUR_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP, TouchExplorationState::FOUR_FINGERS_TAP},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP, TouchExplorationState::FOUR_FINGERS_TAP}
    };
    GestureType gestureType = multiGestureMap.at(msg);
    if (MULTI_FINGER_TAP_GESTURE_TO_STATE.find(gestureType) != MULTI_FINGER_TAP_GESTURE_TO_STATE.end()) {
        // multi-finger multi-tap gesture
        if (GetCurrentState() == MULTI_FINGER_TAP_GESTURE_TO_STATE.at(gestureType)) {
            SendGestureEventToAA(gestureType);
            Clear();
            HILOG_INFO("currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
            SetCurrentState(TouchExplorationState::TOUCH_INIT);
        }
    } else if (gestureType == GestureType::GESTURE_INVALID) {
        if (GetCurrentState() == TouchExplorationState::TWO_FINGERS_DOWN) {
            HILOG_INFO("currentState is changed from TWO_FINGERS_DOWN to TWO_FINGERS_UNKNOWN.");
            SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
            return;
        }
        Clear();
        HILOG_ERROR("currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    } else {
        // multi-finger multi-tap-and-hold gesture
        SendGestureEventToAA(gestureType);
        Clear();
        HILOG_INFO("currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleInitStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        receivedPointerEvents_.push_back(event);
        HILOG_INFO("currentState is changed from TOUCH_INIT to ONE_FINGER_DOWN.");
        SetCurrentState(TouchExplorationState::ONE_FINGER_DOWN);
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::SEND_HOVER_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::LONG_PRESS_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::LONG_PRESS_TIMEOUT));
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
        return;
    }

    HILOG_INFO("currentState is changed from TOUCH_INIT to PASSING_THROUGH.");
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(TouchExplorationState::PASSING_THROUGH);
}

void TouchExploration::HandleInitStateUp(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        HILOG_INFO("currentState is changed from TOUCH_INIT to PASSING_THROUGH.");
        SetCurrentState(TouchExplorationState::PASSING_THROUGH);
    }
}

void TouchExploration::HandleInitStateMove(MMI::PointerEvent &event)
{
    HILOG_INFO("currentState is changed from TOUCH_INIT to PASSING_THROUGH.");
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    SetCurrentState(TouchExplorationState::PASSING_THROUGH);
}

void TouchExploration::HandlePassingThroughState(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointerIterm = {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        HILOG_INFO("currentState is changed from PASSING_THROUGH to TOUCH_INIT.");
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleInvalidState(MMI::PointerEvent &event)
{
    MMI::PointerEvent::PointerItem pointerIterm = {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        SendScreenWakeUpEvent(event);
    }

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        HILOG_INFO("currentState is changed from INVALID to TOUCH_INIT.");
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleCancelEvent(MMI::PointerEvent &event)
{
    if (GetCurrentState() == TouchExplorationState::TWO_FINGERS_DRAG && event.GetPointerId() == draggingPid_) {
        SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
    }
    SendEventToMultimodal(event, ChangeAction::HOVER_CANCEL);

    MMI::PointerEvent::PointerItem pointerIterm = {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    // the last finger is lifted
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointerIterm.IsPressed())) {
        Clear();
        HILOG_INFO("currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
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
        HILOG_INFO("currentState is changed from ONE_FINGER_DOWN to TWO_FINGERS_UNKNOWN.");
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
    HILOG_INFO("currentState is changed from ONE_FINGER_DOWN to TWO_FINGERS_DOWN.");
    SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);
}

void TouchExploration::HandleOneFingerDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::LONG_PRESS_MSG);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    receivedPointerEvents_.push_back(event);

    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::DOUBLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));

    HILOG_INFO("currentState is changed from ONE_FINGER_DOWN to ONE_FINGER_SINGLE_TAP.");
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
        HILOG_INFO("currentState is changed from ONE_FINGER_DOWN to ONE_FINGER_SWIPE.");
        SetCurrentState(TouchExplorationState::ONE_FINGER_SWIPE);
        SendScreenWakeUpEvent(event);
    }
}

void TouchExploration::HandleOneFingerLongPressStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    draggingPid_ = event.GetPointerId();
    HILOG_INFO("currentState is changed from ONE_FINGER_LONG_PRESS to TWO_FINGERS_UNKNOWN.");
    SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
}

void TouchExploration::HandleOneFingerLongPressStateUp(MMI::PointerEvent &event)
{
    Clear();
    SendEventToMultimodal(event, ChangeAction::HOVER_EXIT);

    HILOG_INFO("currentState is changed from ONE_FINGER_LONG_PRESS to TOUCH_INIT.");
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
    HILOG_ERROR("currentState is changed from ONE_FINGER_SWIPE to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::AddOneFingerSwipeEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    MMI::PointerEvent::PointerItem pointerIterm = {};
    event.GetPointerItem(event.GetPointerId(), pointerIterm);

    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("received pointer event is null!");
        return;
    }

    MMI::PointerEvent preMoveEvent = receivedPointerEvents_.back();
    MMI::PointerEvent::PointerItem preMovePointerIterm = {};
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
        HILOG_ERROR("fail to parse swipe direction, currentState is changed from ONE_FINGER_SWIPE to TOUCH_INIT.");
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
    HILOG_INFO("currentState is changed from ONE_FINGER_SWIPE to TOUCH_INIT.");
    SetCurrentState(TouchExplorationState::TOUCH_INIT);
}

void TouchExploration::HandleOneFingerSwipeStateMove(MMI::PointerEvent &event)
{
    AddOneFingerSwipeEvent(event);
    SendScreenWakeUpEvent(event);
}

void TouchExploration::RecordFocusedLocation(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    AccessibilityElementInfo focusedElementInfo = {};
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().FindFocusedElement(focusedElementInfo);
    if (!ret) {
        HILOG_ERROR("find focused element failed.");
        return;
    }

    MMI::PointerEvent::PointerItem pointer = {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    offsetX_ = (focusedElementInfo.GetRectInScreen().GetLeftTopXScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomXScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayX();
    offsetY_ = (focusedElementInfo.GetRectInScreen().GetLeftTopYScreenPostion() +
        focusedElementInfo.GetRectInScreen().GetRightBottomYScreenPostion()) / DIVIDE_NUM - pointer.GetDisplayY();
}

void TouchExploration::HandleOneFingerSingleTapStateDown(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::SEND_HOVER_MSG);
    CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_MSG);

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
        HILOG_INFO("currentState is changed from ONE_FINGER_SINGLE_TAP to ONE_FINGER_DOWN.");
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
    RecordFocusedLocation(event);
    HILOG_INFO("currentState is changed from ONE_FINGER_SINGLE_TAP to ONE_FINGER_SINGLE_TAP_THEN_DOWN.");
    SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
}

void TouchExploration::HandleOneFingerSingleTapThenDownStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    HILOG_ERROR("receive down event, currentState is changed from ONE_FINGER_SINGLE_TAP_THEN_DOWN to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleOneFingerSingleTapThenDownStateUp(MMI::PointerEvent &event)
{
    CancelPostEvent(TouchExplorationMsg::DOUBLE_TAP_AND_LONG_PRESS_MSG);
    Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(
        ActionType::ACCESSIBILITY_ACTION_CLICK);

    Clear();
    HILOG_INFO("currentState is changed from ONE_FINGER_SINGLE_TAP_THEN_DOWN to TOUCH_INIT.");
    SetCurrentState(TouchExplorationState::TOUCH_INIT);
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
        HILOG_ERROR("receive move event, currentState is changed from ONE_FINGER_SINGLE_TAP_THEN_DOWN to INVALID.");
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

    MMI::PointerEvent::PointerItem pointer = {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    pointer.SetDisplayX(offsetX_ + pointer.GetDisplayX());
    pointer.SetDisplayY(offsetY_ + pointer.GetDisplayY());
    event.RemovePointerItem(event.GetPointerId());
    event.AddPointerItem(pointer);
}

void TouchExploration::SendDoubleTapAndLongPressDownEvent()
{
    if (receivedPointerEvents_.empty()) {
        HILOG_ERROR("receivedPointerEvents_ is empty!");
        return;
    }
    OffsetEvent(receivedPointerEvents_.front());
    SendEventToMultimodal(receivedPointerEvents_.front(), ChangeAction::NO_CHANGE);
}

void TouchExploration::HandleOneFingerDoubleTapAndLongPressState(MMI::PointerEvent &event)
{
    OffsetEvent(event);
    SendEventToMultimodal(event, ChangeAction::NO_CHANGE);

    MMI::PointerEvent::PointerItem pointer = {};
    event.GetPointerItem(event.GetPointerId(), pointer);
    if ((event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) &&
        (!pointer.IsPressed())) {
        Clear();
        HILOG_INFO("currentState is changed from ONE_FINGER_DOUBLE_TAP_AND_LONG_PRESS to TOUCH_INIT.");
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleTwoFingersDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from TWO_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
        HILOG_ERROR("timeout, currentState is changed from TWO_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    HILOG_INFO("currentState is changed from TWO_FINGERS_DOWN to THREE_FINGERS_DOWN.");
    SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);
}

void TouchExploration::HandleTwoFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        HILOG_INFO("currentState is changed from TWO_FINGERS_DOWN to TWO_FINGERS_TAP.");
        SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        Clear();
        CancelPostEvent(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
        HILOG_ERROR("invalid pointer num, currentState is changed from TWO_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

bool TouchExploration::GetBasePointItem(MMI::PointerEvent::PointerItem &basePointerIterm, int32_t pId)
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

void TouchExploration::GetPointOffset(MMI::PointerEvent &event, std::vector<float> &firstPointOffset,
    std::vector<float> &secondPointOffset)
{
    HILOG_DEBUG();
    std::vector<int32_t> pIds = event.GetPointerIds();
    if (pIds.size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
    event.GetPointerItem(pIds[0], pointerF);
    event.GetPointerItem(pIds[1], pointerS);

    MMI::PointerEvent::PointerItem basePointerF = {};
    MMI::PointerEvent::PointerItem basePointerS = {};
    if (GetBasePointItem(basePointerF, pIds[0]) && GetBasePointItem(basePointerS, pIds[1])) {
        firstPointOffset.push_back(pointerF.GetDisplayX() - basePointerF.GetDisplayX());
        firstPointOffset.push_back(pointerF.GetDisplayY() - basePointerF.GetDisplayY());
        secondPointOffset.push_back(pointerS.GetDisplayX() - basePointerS.GetDisplayX());
        secondPointOffset.push_back(pointerS.GetDisplayY() - basePointerS.GetDisplayY());
    }
}

float TouchExploration::GetAngleCos(float offsetX, float offsetY, bool isGetX)
{
    HILOG_DEBUG();
    float ret = isGetX ? offsetX : offsetY;
    double duration = hypot(offsetX, offsetY);
    if (duration < - EPSINON || duration > EPSINON) {
        ret = ret / duration;
    }
    return ret;
}

bool TouchExploration::IsDragGestureAccept(MMI::PointerEvent &event)
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

void TouchExploration::SendDragDownEventToMultimodal(MMI::PointerEvent event)
{
    HILOG_DEBUG();
    std::vector<int32_t> pIds = event.GetPointerIds();
    int32_t xPointDown = 0;
    int32_t yPointDown = 0;
    int64_t actionTime = 0;
    MMI::PointerEvent::PointerItem basePointerIterm = {};

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

    MMI::PointerEvent::PointerItem pointer = {};
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

void TouchExploration::HandleTwoFingersDownStateMove(MMI::PointerEvent &event)
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
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
            Clear();
            SendScreenWakeUpEvent(event);
            HILOG_ERROR("invalid pointer num, currentState is changed from TWO_FINGERS_DOWN to INVALID.");
            SetCurrentState(TouchExplorationState::INVALID);
            return;
        }
        if (IsDragGestureAccept(event)) {
            draggingPid_ = event.GetPointerId();
            SendDragDownEventToMultimodal(event);
            SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
            HILOG_INFO("currentState is changed from TWO_FINGERS_DOWN to TWO_FINGERS_DRAG.");
            SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
        } else {
            for (auto &receivedEvent : receivedPointerEvents_) {
                SendEventToMultimodal(receivedEvent, ChangeAction::NO_CHANGE);
            }
            Clear();
            HILOG_INFO("currentState is changed from TWO_FINGERS_DOWN to PASSING_THROUGH.");
            SetCurrentState(TouchExplorationState::PASSING_THROUGH);
        }
    }
}

void TouchExploration::SendUpForDragDownEvent()
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
    MMI::PointerEvent::PointerItem pointerItem = {};
    draggingDownEvent_->SetActionTime(Utils::GetSystemTime() * US_TO_MS);
    draggingDownEvent_->GetPointerItem(draggingDownEvent_->GetPointerId(), pointerItem);
    pointerItem.SetPressed(false);
    pointerItem.SetDisplayX(x);
    pointerItem.SetDisplayY(y);
    draggingDownEvent_->RemovePointerItem(draggingDownEvent_->GetPointerId());
    draggingDownEvent_->AddPointerItem(pointerItem);
    SendEventToMultimodal(*draggingDownEvent_, ChangeAction::POINTER_UP);
}

void TouchExploration::HandleTwoFingersDragStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        SendUpForDragDownEvent();
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from TWO_FINGERS_DRAG to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleTwoFingersDragStateUp(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SendUpForDragDownEvent();
        Clear();
        HILOG_INFO("currentState is changed from TWO_FINGERS_DRAG to TOUCH_INIT.");
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleTwoFingersDragStateMove(MMI::PointerEvent &event)
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

    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
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
        MMI::PointerEvent::PointerItem pointer = {};
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

bool TouchExploration::GetPointerItemWithFingerNum(int32_t fingerNum,
    std::vector<MMI::PointerEvent::PointerItem> &curPoints,
    std::vector<MMI::PointerEvent::PointerItem> &prePoints, MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    bool findPrePointer = false;
    std::vector<int32_t> pIds = event.GetPointerIds();
    for (int32_t i = 0; i < fingerNum; i++) {
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

bool TouchExploration::IsMultiFingerMultiTap(MMI::PointerEvent &event, const int32_t fingerNum)
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

bool TouchExploration::IsMultiFingerMultiTapGesture(MMI::PointerEvent &event, const int32_t fingerNum)
{
    HILOG_DEBUG();
    if (IsMultiFingerMultiTap(event, fingerNum)) {
        multiTapNum_ = multiTapNum_ + 1;
        HILOG_DEBUG("%{public}d finger %{public}d tap is recognized", fingerNum, multiTapNum_ + 1);
    } else {
        Clear();
        HILOG_ERROR("invalid gesture, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
        return false;
    }

    uint32_t fingerNumIndex = static_cast<uint32_t>(fingerNum - 2);
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_TAP_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_HOLD_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    return true;
}

void TouchExploration::HandleMultiFingersTapStateDown(MMI::PointerEvent &event, int32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapEvent();
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);

    if (multiTapNum_ == TAP_COUNT_MAXIMUM - 1) {
        Clear();
        HILOG_ERROR("Exceeds max continuous tap count, currentState is changed from %{public}d to INVALID.",
            GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    int32_t pointerSize = static_cast<int32_t>(event.GetPointerIds().size());
    if (pointerSize < fingerNum) {
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    } else if (pointerSize == fingerNum) {
        if (IsMultiFingerMultiTapGesture(event, fingerNum)) {
            if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
                HILOG_INFO("currentState is changed from TWO_FINGERS_TAP to TWO_FINGERS_CONTINUE_DOWN.");
                SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);
            } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
                HILOG_INFO("currentState is changed from THREE_FINGERS_TAP to THREE_FINGERS_CONTINUE_DOWN.");
                SetCurrentState(TouchExplorationState::THREE_FINGERS_CONTINUE_DOWN);
            } else {
                HILOG_INFO("currentState is changed from FOUR_FINGERS_TAP to FOUR_FINGERS_CONTINUE_DOWN.");
                SetCurrentState(TouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN);
            }
        }
    } else {
        Clear();
        HILOG_INFO("invalid pointer num, currentState is changed from TWO_FINGERS_TAP to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleTwoFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void TouchExploration::HandleMultiFingersTapStateUp(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);

    int32_t pointerSize = static_cast<int32_t>(event.GetPointerIds().size());
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        HILOG_INFO("currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    } else {
        HILOG_ERROR("receive up event, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleMultiFingersTapStateMove(MMI::PointerEvent &event, int32_t fingerNum)
{
    if (static_cast<int32_t>(event.GetPointerIds().size()) >= fingerNum) {
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
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
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        HILOG_ERROR("receive move event, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleTwoFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void TouchExploration::HandleMultiFingersContinueDownStateDown(MMI::PointerEvent &event)
{
    CancelMultiFingerTapEvent();
    CancelMultiFingerTapAndHoldEvent();
    Clear();
    HILOG_ERROR("receive down event, currentState is changed from %{public}d to INVALID.", GetCurrentState());
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleMultiFingersContinueDownStateUp(MMI::PointerEvent &event, int32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapAndHoldEvent();

    int32_t pointerSize = static_cast<int32_t>(event.GetPointerIds().size());
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
            HILOG_INFO("currentState is changed from TWO_FINGERS_CONTINUE_DOWN to TWO_FINGERS_TAP.");
            SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
        } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
            HILOG_INFO("currentState is changed from THREE_FINGERS_CONTINUE_DOWN to THREE_FINGERS_TAP.");
            SetCurrentState(TouchExplorationState::THREE_FINGERS_TAP);
        } else {
            HILOG_INFO("currentState is changed from FOUR_FINGERS_CONTINUE_DOWN to FOUR_FINGERS_TAP.");
            SetCurrentState(TouchExplorationState::FOUR_FINGERS_TAP);
        }
    } else if (pointerSize > fingerNum) {
        CancelMultiFingerTapEvent();
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleTwoFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void TouchExploration::HandleMultiFingersContinueDownStateMove(MMI::PointerEvent &event, int32_t fingerNum)
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
        HILOG_ERROR("receive move event, currentState is changed from %{public}d to INVALID.", GetCurrentState());
        SetCurrentState(TouchExplorationState::INVALID);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleTwoFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void TouchExploration::HandleTwoFingersUnknownStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        receivedPointerEvents_.push_back(event);
        return;
    }

    Clear();
    HILOG_ERROR("receive down event, currentState is changed from TWO_FINGERS_UNKNOWN to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleTwoFingersUnknownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        Clear();
        HILOG_INFO("currentState is changed from TWO_FINGERS_UNKNOWN to TOUCH_INIT.");
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleTwoFingersUnknownStateMove(MMI::PointerEvent &event)
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
        HILOG_INFO("currentState is changed from TWO_FINGERS_UNKNOWN to TWO_FINGERS_DRAG.");
        SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
        return;
    }
}

void TouchExploration::HandleThreeFingersDownStateDown(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);

    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from THREE_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
        HILOG_ERROR("timeout, currentState is changed from THREE_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    HILOG_INFO("currentState is changed from THREE_FINGERS_DOWN to FOUR_FINGERS_DOWN.");
    SetCurrentState(TouchExplorationState::FOUR_FINGERS_DOWN);
}

void TouchExploration::HandleThreeFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        HILOG_INFO("currentState is changed from THREE_FINGERS_DOWN to THREE_FINGERS_TAP.");
        SetCurrentState(TouchExplorationState::THREE_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
        Clear();
        HILOG_ERROR("invalid pointer num, currentState is changed from THREE_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::StoreMultiFingerSwipeBaseDownPoint()
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

void TouchExploration::HandleThreeFingersDownStateMove(MMI::PointerEvent &event)
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
        CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
        CancelPostEvent(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
            Clear();
            SendScreenWakeUpEvent(event);
            HILOG_ERROR("invalid pointer num, currentState is changed from THREE_FINGERS_DOWN to INVALID.");
            SetCurrentState(TouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        HILOG_INFO("currentState is changed from THREE_FINGERS_DOWN to THREE_FINGERS_SWIPE.");
        SetCurrentState(TouchExplorationState::THREE_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleThreeFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    HILOG_ERROR("receive down event, currentState is changed from THREE_FINGERS_SWIPE to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

bool TouchExploration::GetMultiFingerSwipeBasePointerItem(MMI::PointerEvent::PointerItem &basePointerIterm, int32_t pId)
{
    HILOG_DEBUG();
    if (multiFingerSwipePrePoint_.count(pId) == 0 || !multiFingerSwipePrePoint_[pId]) {
        HILOG_ERROR("get base pointEvent(%{public}d) failed", pId);
        return false;
    }
    multiFingerSwipePrePoint_[pId]->GetPointerItem(pId, basePointerIterm);
    return true;
}

bool TouchExploration::SaveMultiFingerSwipeGesturePointerInfo(MMI::PointerEvent &event)
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
                HILOG_INFO("direction changed, currentState is changed from THREE_FINGERS_SWIPE to TOUCH_INIT.");
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
            } else {
                HILOG_INFO("direction changed, currentState is changed from THREE_FINGERS_SWIPE to INVALID.");
                SetCurrentState(TouchExplorationState::INVALID);
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

bool TouchExploration::RecognizeMultiFingerSwipePath(const std::vector<Pointer> &path)
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

GestureType TouchExploration::GetMultiFingerSwipeGestureId(int32_t fingerNum)
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

void TouchExploration::HandleMultiFingersSwipeStateUp(MMI::PointerEvent &event, int32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);

    if (!SaveMultiFingerSwipeGesturePointerInfo(event)) {
        return;
    }

    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        for (int32_t pIndex = 0; pIndex < fingerNum; pIndex++) {
            if (multiFingerSwipeRoute_.count(pIndex) == 0 ||
                multiFingerSwipeRoute_[pIndex].size() < MIN_MULTI_FINGER_SWIPE_POINTER_NUM) {
                Clear();
                HILOG_ERROR("invalid route, currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
                return;
            }
            if (!RecognizeMultiFingerSwipePath(multiFingerSwipeRoute_[pIndex])) {
                Clear();
                HILOG_ERROR("fail to parse swipe direction, currentState is changed from %{public}d to TOUCH_INIT.",
                    GetCurrentState());
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
                return;
            }
        }

        GestureType gestureId = GetMultiFingerSwipeGestureId(fingerNum);
        SendGestureEventToAA(gestureId);
        Clear();
        HILOG_INFO("currentState is changed from %{public}d to TOUCH_INIT.", GetCurrentState());
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    }
}

void TouchExploration::HandleThreeFingersSwipeStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersSwipeStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void TouchExploration::HandleThreeFingersSwipeStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    SaveMultiFingerSwipeGesturePointerInfo(event);
    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleThreeFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void TouchExploration::HandleThreeFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void TouchExploration::HandleThreeFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void TouchExploration::HandleThreeFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_3));
}

void TouchExploration::HandleFourFingersDownStateDown(MMI::PointerEvent &event)
{
    Clear();
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);
    HILOG_ERROR("receive down event, currentState is changed from FOUR_FINGERS_DOWN to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleFourFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        HILOG_INFO("currentState is changed from FOUR_FINGERS_DOWN to FOUR_FINGERS_TAP.");
        SetCurrentState(TouchExplorationState::FOUR_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        Clear();
        CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
        HILOG_ERROR("invalid pointer num, currentState is changed from FOUR_FINGERS_DOWN to INVALID.");
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleFourFingersDownStateMove(MMI::PointerEvent &event)
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
        CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
        CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);
        if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
            Clear();
            SendScreenWakeUpEvent(event);
            HILOG_ERROR("invalid pointer num, currentState is changed from FOUR_FINGERS_DOWN to INVALID.");
            SetCurrentState(TouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        HILOG_INFO("currentState is changed from FOUR_FINGERS_DOWN to FOUR_FINGERS_SWIPE.");
        SetCurrentState(TouchExplorationState::FOUR_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleFourFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
    HILOG_ERROR("receive down event, currentState is changed from FOUR_FINGERS_SWIPE to INVALID.");
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleFourFingersSwipeStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersSwipeStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void TouchExploration::HandleFourFingersSwipeStateMove(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    SaveMultiFingerSwipeGesturePointerInfo(event);
    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleFourFingersTapStateDown(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateDown(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void TouchExploration::HandleFourFingersTapStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersTapStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void TouchExploration::HandleFourFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void TouchExploration::HandleFourFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateMove(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_4));
}

void TouchExploration::CancelPostEvent(TouchExplorationMsg msg)
{
    HILOG_DEBUG("innerEventID = %{public}u", static_cast<uint32_t>(msg));
    if (handler_->HasInnerEvent(static_cast<uint32_t>(msg))) {
        handler_->RemoveEvent(static_cast<uint32_t>(msg));
    }
}

void TouchExploration::CancelMultiFingerTapEvent()
{
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_MSG);
}

void TouchExploration::CancelMultiFingerTapAndHoldEvent()
{
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
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