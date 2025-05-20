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
            SetCurrentState(TouchExplorationState::TOUCH_INIT);
        }
    } else if (gestureType == GestureType::GESTURE_INVALID) {
        if (GetCurrentState() == TouchExplorationState::TWO_FINGERS_DOWN) {
            for (auto& event : receivedPointerEvents_) {
                SendEventToMultimodal(event, ChangeAction::NO_CHANGE);
            }
            SetCurrentState(TouchExplorationState::PASSING_THROUGH);
            return;
        }
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
    } else {
        // multi-finger multi-tap-and-hold gesture
        SendGestureEventToAA(gestureType);
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
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
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
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
    SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);
}

void TouchExploration::HandleTwoFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(TouchExplorationMsg::TWO_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        Clear();
        CancelPostEvent(TouchExplorationMsg::TWO_FINGER_SINGLE_TAP_MSG);
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
    return (firstXCos * secondXCos + firstYCos * secondYCos) >= MAX_DRAG_GESTURE_COSINE;
}

void TouchExploration::SendDragDownEventToMultimodal(MMI::PointerEvent event)
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

bool TouchExploration::IsRealMove(MMI::PointerEvent &event)
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
            SetCurrentState(TouchExplorationState::INVALID);
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
            SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
        } else {
            for (auto &receivedEvent : receivedPointerEvents_) {
                SendEventToMultimodal(receivedEvent, ChangeAction::NO_CHANGE);
            }
            Clear();
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

void TouchExploration::HandleTwoFingersDragStateDown(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() != static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        SendUpForDragDownEvent();
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleTwoFingersDragStateUp(MMI::PointerEvent &event)
{
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SendUpForDragDownEvent();
        Clear();
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

bool TouchExploration::GetPointerItemWithFingerNum(uint32_t fingerNum,
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

bool TouchExploration::IsMultiFingerMultiTap(MMI::PointerEvent &event, const uint32_t fingerNum)
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

bool TouchExploration::IsMultiFingerMultiTapGesture(MMI::PointerEvent &event, const uint32_t fingerNum)
{
    HILOG_DEBUG();

    if (fingerNum < static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
        return false;
    }

    if (IsMultiFingerMultiTap(event, fingerNum)) {
        multiTapNum_ = multiTapNum_ + 1;
        HILOG_DEBUG("%{public}d finger %{public}d tap is recognized", static_cast<int32_t>(fingerNum),
            multiTapNum_ + 1);
    } else {
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
        return false;
    }

    uint32_t fingerNumIndex = fingerNum - 2;
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_TAP_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(GESTURE_HOLD_MSG[multiTapNum_][fingerNumIndex]), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    return true;
}

void TouchExploration::HandleMultiFingersTapStateDown(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapEvent();
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);

    if (multiTapNum_ == TAP_COUNT_MAXIMUM - 1) {
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize < fingerNum) {
        handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG), 0,
            static_cast<int32_t>(TimeoutDuration::MULTI_FINGER_TAP_INTERVAL_TIMEOUT));
    } else if (pointerSize == fingerNum) {
        if (IsMultiFingerMultiTapGesture(event, fingerNum)) {
            if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
                SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);
            } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
                SetCurrentState(TouchExplorationState::THREE_FINGERS_CONTINUE_DOWN);
            } else {
                SetCurrentState(TouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN);
            }
        }
    } else {
        Clear();
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

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(TouchExplorationState::TOUCH_INIT);
    } else {
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleMultiFingersTapStateMove(MMI::PointerEvent &event, uint32_t fingerNum)
{
    if (event.GetPointerIds().size() >= fingerNum) {
        Clear();
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
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleMultiFingersContinueDownStateUp(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);
    CancelMultiFingerTapAndHoldEvent();

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_2)) {
            SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
        } else if (fingerNum == static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
            SetCurrentState(TouchExplorationState::THREE_FINGERS_TAP);
        } else {
            SetCurrentState(TouchExplorationState::FOUR_FINGERS_TAP);
        }
    } else if (pointerSize > fingerNum) {
        CancelMultiFingerTapEvent();
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
    }
}

void TouchExploration::HandleTwoFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HandleMultiFingersContinueDownStateUp(event, static_cast<uint32_t>(PointerCount::POINTER_COUNT_2));
}

void TouchExploration::HandleMultiFingersContinueDownStateMove(MMI::PointerEvent &event, uint32_t fingerNum)
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
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleTwoFingersUnknownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        Clear();
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
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    if (!handler_->HasInnerEvent(static_cast<uint32_t>(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG))) {
        Clear();
        SetCurrentState(TouchExplorationState::INVALID);
        return;
    }

    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    handler_->SendEvent(static_cast<uint32_t>(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG), 0,
        static_cast<int32_t>(TimeoutDuration::DOUBLE_TAP_TIMEOUT));
    SetCurrentState(TouchExplorationState::FOUR_FINGERS_DOWN);
}

void TouchExploration::HandleThreeFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelPostEvent(TouchExplorationMsg::THREE_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(TouchExplorationState::THREE_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_3)) {
        CancelPostEvent(TouchExplorationMsg::THREE_FINGER_SINGLE_TAP_MSG);
        Clear();
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
            SetCurrentState(TouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        SetCurrentState(TouchExplorationState::THREE_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleThreeFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
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
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
            } else {
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

GestureType TouchExploration::GetMultiFingerSwipeGestureId(uint32_t fingerNum)
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

void TouchExploration::HandleMultiFingersSwipeStateUp(MMI::PointerEvent &event, uint32_t fingerNum)
{
    receivedPointerEvents_.push_back(event);

    if (!SaveMultiFingerSwipeGesturePointerInfo(event)) {
        return;
    }

    if (event.GetPointerIds().size() == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        if (fingerNum > static_cast<uint32_t>(INT32_MAX)) {
            SetCurrentState(TouchExplorationState::TOUCH_INIT);
            return;
        }
        for (int32_t pIndex = 0; pIndex < static_cast<int32_t>(fingerNum); pIndex++) {
            if (multiFingerSwipeRoute_.count(pIndex) == 0 ||
                multiFingerSwipeRoute_[pIndex].size() < MIN_MULTI_FINGER_SWIPE_POINTER_NUM) {
                Clear();
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
                return;
            }
            if (!RecognizeMultiFingerSwipePath(multiFingerSwipeRoute_[pIndex])) {
                Clear();
                SetCurrentState(TouchExplorationState::TOUCH_INIT);
                return;
            }
        }

        GestureType gestureId = GetMultiFingerSwipeGestureId(fingerNum);
        SendGestureEventToAA(gestureId);
        Clear();
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
    SetCurrentState(TouchExplorationState::INVALID);
}

void TouchExploration::HandleFourFingersDownStateUp(MMI::PointerEvent &event)
{
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == static_cast<uint32_t>(PointerCount::POINTER_COUNT_1)) {
        SetCurrentState(TouchExplorationState::FOUR_FINGERS_TAP);
    } else if (pointerSize > static_cast<uint32_t>(PointerCount::POINTER_COUNT_4)) {
        Clear();
        CancelPostEvent(TouchExplorationMsg::FOUR_FINGER_SINGLE_TAP_MSG);
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
            SetCurrentState(TouchExplorationState::INVALID);
            return;
        }
        StoreMultiFingerSwipeBaseDownPoint();
        multiFingerSwipeDirection_ = GetSwipeDirection(offsetX, offsetY);
        SetCurrentState(TouchExplorationState::FOUR_FINGERS_SWIPE);
    }

    SendScreenWakeUpEvent(event);
}

void TouchExploration::HandleFourFingersSwipeStateDown(MMI::PointerEvent &event)
{
    Clear();
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
} // namespace Accessibility
} // namespace OHOS