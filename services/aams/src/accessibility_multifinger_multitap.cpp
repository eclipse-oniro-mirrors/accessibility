/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "accessibility_multifinger_multitap.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr float SLOP_DELTA = 0.5f;
    constexpr int32_t POINTER_COUNT_1 = 1;
    constexpr int32_t POINTER_COUNT_2 = 2;
    constexpr int32_t POINTER_COTINUE_TAP_ONE_TIME = 1;
    constexpr int32_t MULTI_FINGER_MAX_CONTINUE_TAP_NUM = 3;
    constexpr float TOUCH_SLOP = 8.0f;
} // namespace

MultiFingerGestureHandler::MultiFingerGestureHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityMultiTapGestureRecognizer &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

bool MultiFingerGestureHandler::ProcessTwoFingerGestureEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG("Inner Event Id id: %u", static_cast<uint32_t>(event->GetInnerEventId()));

    uint32_t eventId = static_cast<uint32_t>(event->GetInnerEventId());
    if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_SINGLE_TAP_MSG) {
        if (server_.GetFingerTouchUpState() == FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->TwoFingerGestureOnCompleted(
                GestureType::GESTURE_TWO_FINGER_SINGLE_TAP);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
            server_.Clear();
        }
    } else if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_LONG_PRESS_MSG) {
        if (server_.GetFingerTouchUpState() != FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
        }
        server_.Clear();
    } else if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_DOUBLE_TAP_MSG) {
        if (server_.GetFingerTouchUpState() == FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->TwoFingerGestureOnCompleted(
                GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
            server_.Clear();
        }
    } else if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG) {
        if (server_.GetFingerTouchUpState() != FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->TwoFingerGestureOnCompleted(
                GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
        }
        server_.Clear();
    } else if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_TRIPLE_TAP_MSG) {
        if (server_.GetFingerTouchUpState() == FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->TwoFingerGestureOnCompleted(
                GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
            server_.Clear();
        }
    } else if (eventId == AccessibilityMultiTapGestureRecognizer::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG) {
        if (server_.GetFingerTouchUpState() != FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->TwoFingerGestureOnCompleted(
                GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
        }
        server_.Clear();
    } else {
        return false;
    }

    return true;
}

void MultiFingerGestureHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    if (!event) {
        HILOG_ERROR("event is null");
        return;
    }

    if (ProcessTwoFingerGestureEvent(event)) {
        return;
    }

    switch (event->GetInnerEventId()) {
        case AccessibilityMultiTapGestureRecognizer::WAIT_ANOTHER_FINGER_DOWN_MSG:
            server_.SetFingerTouchUpState(FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP);
            break;
        case AccessibilityMultiTapGestureRecognizer::CANCEL_WAIT_FINGER_DOWN_MSG:
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_NOT_START);
            break;
        case AccessibilityMultiTapGestureRecognizer::CANCEL_GESTURE:
            if (server_.GetFingerTouchUpState() != FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
                server_.SetFingerTouchUpState(FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP);
            }
            server_.GetRecognizeListener()->TwoFingerGestureOnCancelled(true);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_CANCLE);
            server_.Clear();
            break;
        default:
            break;
    }
}

AccessibilityMultiTapGestureRecognizer::AccessibilityMultiTapGestureRecognizer()
{
    HILOG_DEBUG();

    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }

    float density = display->GetVirtualPixelRatio();
    int32_t slop = static_cast<int32_t>(density * DOUBLE_TAP_SLOP + SLOP_DELTA);
    doubleTapOffsetThresh_ = slop * slop;
    touchSlop_ = TOUCH_SLOP;

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    handler_ = std::make_shared<MultiFingerGestureHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

void AccessibilityMultiTapGestureRecognizer::RegisterListener(AccessibilityGestureRecognizeListener& listener)
{
    HILOG_DEBUG();

    listener_ = &listener;
}

void AccessibilityMultiTapGestureRecognizer::CancelTwoFingerEvent()
{
    HILOG_DEBUG();

    if (!handler_) {
        HILOG_ERROR("handler_ is null ptr");
        return;
    }

    handler_->RemoveEvent(TWO_FINGER_SINGLE_TAP_MSG);
    handler_->RemoveEvent(TWO_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(TWO_FINGER_DOUBLE_TAP_MSG);
    handler_->RemoveEvent(TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(TWO_FINGER_TRIPLE_TAP_MSG);
    handler_->RemoveEvent(TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
}

void AccessibilityMultiTapGestureRecognizer::CancelAllPenddingEvent()
{
    HILOG_DEBUG();

    if (!handler_) {
        HILOG_ERROR("handler_ is null ptr");
        return;
    }

    handler_->RemoveEvent(CANCEL_GESTURE);
    handler_->RemoveEvent(CANCEL_WAIT_FINGER_DOWN_MSG);
    CancelTwoFingerEvent();
}

void AccessibilityMultiTapGestureRecognizer::Clear()
{
    HILOG_DEBUG();

    targetFingers_ = -1;
    addContinueTapNum_ = 0;
    firstDownPoint_.clear();
    lastUpPoint_.clear();
    currentDownPoint_.clear();
    CancelAllPenddingEvent();
}

void AccessibilityMultiTapGestureRecognizer::CancelGesture(bool isNoDelayFlag)
{
    HILOG_DEBUG();

    SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_CANCLE);

    if (fingerTouchUpState_ != FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
        fingerTouchUpState_ = FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP;
    }

    if (!listener_) {
        HILOG_ERROR("listener_ is null ptr");
        return;
    }

    listener_->TwoFingerGestureOnCancelled(isNoDelayFlag);
    Clear();
}

bool AccessibilityMultiTapGestureRecognizer::ParamCheck()
{
    if (!lastUpPoint_.count(0) || !lastUpPoint_.count(1) || !firstDownPoint_.count(0) || !firstDownPoint_.count(1)) {
        HILOG_ERROR("last_up point or first_down point container has wrong value");
        return false;
    }

    if (!lastUpPoint_[0] || !lastUpPoint_[1] || !firstDownPoint_[0] || !firstDownPoint_[1]) {
        HILOG_ERROR("last_up point or first_down point container has null ptr value");
        return false;
    }

    return true;
}

bool AccessibilityMultiTapGestureRecognizer::IsTwoFingerDoubleTap(MMI::PointerEvent &event)
{
    HILOG_DEBUG("doubleTapOffsetThresh: %d", doubleTapOffsetThresh_);

    if (!ParamCheck()) {
        return false;
    }

    // first pointer up time to second pointer down time
    int64_t firstUpTime = lastUpPoint_[0]->GetActionTime() > lastUpPoint_[1]->GetActionTime() ?
        lastUpPoint_[1]->GetActionTime() : lastUpPoint_[0]->GetActionTime();
    int64_t durationTime = event.GetActionTime() - firstUpTime;
    if (durationTime > DOUBLE_TAP_TIMEOUT || durationTime < MIN_DOUBLE_TAP_TIME) {
        HILOG_WARN("durationTime[%{public}" PRId64 "] is wrong", durationTime);
        return false;
    }

    std::vector<int32_t> pIds = event.GetPointerIds();
    if (pIds.size() != POINTER_COUNT_2) {
        return false;
    }

    MMI::PointerEvent::PointerItem pointerF = {};
    MMI::PointerEvent::PointerItem pointerS = {};
    if (!event.GetPointerItem(pIds[0], pointerF)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[0]);
        return false;
    }

    if (!event.GetPointerItem(pIds[1], pointerS)) {
        HILOG_ERROR("GetPointerItem(%d) failed", pIds[1]);
        return false;
    }

    MMI::PointerEvent::PointerItem firstPI;
    MMI::PointerEvent::PointerItem secondPI;
    firstDownPoint_[0]->GetPointerItem(firstDownPoint_[0]->GetPointerId(), firstPI);
    firstDownPoint_[1]->GetPointerItem(firstDownPoint_[1]->GetPointerId(), secondPI);
    HILOG_DEBUG("first finger preDown x: %d, y: %d. curDown x: %d, y: %d",
        firstPI.GetDisplayX(), firstPI.GetDisplayY(), pointerF.GetDisplayX(), pointerF.GetDisplayY());
    HILOG_DEBUG("second finger preDown x: %d, y: %d. curDown x: %d, y: %d",
        secondPI.GetDisplayX(), secondPI.GetDisplayY(), pointerS.GetDisplayX(), pointerS.GetDisplayY());

    int32_t durationXff = firstPI.GetDisplayX() - pointerF.GetDisplayX();
    int32_t durationYff = firstPI.GetDisplayY() - pointerF.GetDisplayY();
    int32_t durationXss = secondPI.GetDisplayX() - pointerS.GetDisplayX();
    int32_t durationYss = secondPI.GetDisplayY() - pointerS.GetDisplayY();

    int32_t durationXfs = firstPI.GetDisplayX() - pointerS.GetDisplayX();
    int32_t durationYfs = firstPI.GetDisplayY() - pointerS.GetDisplayY();
    int32_t durationXsf = secondPI.GetDisplayX() - pointerF.GetDisplayX();
    int32_t durationYsf = secondPI.GetDisplayY() - pointerF.GetDisplayY();

    return ((durationXff * durationXff + durationYff * durationYff < doubleTapOffsetThresh_ &&
        durationXss * durationXss + durationYss * durationYss < doubleTapOffsetThresh_) ||
        (durationXfs * durationXfs + durationYfs * durationYfs < doubleTapOffsetThresh_ &&
        durationXsf * durationXsf + durationYsf * durationYsf < doubleTapOffsetThresh_));
}

void AccessibilityMultiTapGestureRecognizer::HanleFirstTouchDownEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("gestureState is %d, touchUpState is %d", multiFingerGestureState_, fingerTouchUpState_);

    if (multiFingerGestureState_ == MultiFingerGestureState::GESTURE_WAIT) {
        if (event.GetPointerId() == 0) {
            fingerTouchUpState_ = FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP;
            Clear();
            return;
        } else if (!handler_->HasInnerEvent(WAIT_ANOTHER_FINGER_DOWN_MSG)) {
            HILOG_DEBUG("do not have WAIT_ANOTHER_FINGER_DOWN_MSG");
            SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_NOT_START);
        }
    }

    // NOT_ALL_FINGER_TOUCH_UP state can not revice touch down event
    if (fingerTouchUpState_ == FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
        Clear();
        return;
    }

    // start touch down, change fingerTouchUpState_ to TOUCH_DOWN_AFTER_ALL_FINGER_TOUCH_UP state
    fingerTouchUpState_ = FingerTouchUpState::TOUCH_DOWN_AFTER_ALL_FINGER_TOUCH_UP;
    firstDownPoint_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
    handler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, TAP_INTERVAL_TIMEOUT / US_TO_MS);
    if (event.GetPointerIds().size() == POINTER_COUNT_1) {
        SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_WAIT);
    } else if (event.GetPointerIds().size() == POINTER_COUNT_2) {
        listener_->TwoFingerGestureOnStarted();
        SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_START);
        handler_->SendEvent(TWO_FINGER_SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        handler_->SendEvent(TWO_FINGER_LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
    } else {
        CancelGesture(true);
    }
}

void AccessibilityMultiTapGestureRecognizer::HandleContinueTouchDownEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("fingerNum is %d, gestureState is %d, touchUpstate is %d",
        targetFingers_, multiFingerGestureState_, fingerTouchUpState_);

    if (targetFingers_ == POINTER_COUNT_1) {
        return;
    }

    if (multiFingerGestureState_ != MultiFingerGestureState::GESTURE_START) {
        CancelGesture(true);
        return;
    }

    if (fingerTouchUpState_ == FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
        CancelGesture(true);
        return;
    }

    fingerTouchUpState_ = FingerTouchUpState::TOUCH_DOWN_AFTER_ALL_FINGER_TOUCH_UP;
    int32_t pointerSize = static_cast<int32_t>(event.GetPointerIds().size());
    if (pointerSize < targetFingers_) {
        handler_->SendEvent(CANCEL_GESTURE, 0, TAP_INTERVAL_TIMEOUT / US_TO_MS);
    } else if (pointerSize == targetFingers_) {
        // check is double tap
        if (firstDownPoint_.size() == POINTER_COUNT_2 && lastUpPoint_.size() == POINTER_COUNT_2 &&
            IsTwoFingerDoubleTap(event)) {
            HILOG_DEBUG("two finger Double tap is recognized, addContinueTapNum %d", addContinueTapNum_);
            addContinueTapNum_ = (addContinueTapNum_ + 1) % MULTI_FINGER_MAX_CONTINUE_TAP_NUM;
        } else {
            addContinueTapNum_ = 0;
        }

        if (addContinueTapNum_ == 0) {
            handler_->SendEvent(TWO_FINGER_SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
            handler_->SendEvent(TWO_FINGER_LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
        } else if (addContinueTapNum_ == POINTER_COTINUE_TAP_ONE_TIME) {
            handler_->SendEvent(TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
            handler_->SendEvent(TWO_FINGER_DOUBLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        } else {
            handler_->SendEvent(TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
            handler_->SendEvent(TWO_FINGER_TRIPLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        }
    } else {
        HILOG_DEBUG("current fingers is more than last touch down finger nums");
        CancelGesture(true);
    }
}

void AccessibilityMultiTapGestureRecognizer::HandleTwoFingerMoveEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("finger num is %d, pId is %d", static_cast<int32_t>(currentDownPoint_.size()), event.GetPointerId());

    int32_t pId = event.GetPointerId();
    MMI::PointerEvent::PointerItem pointerIterm;
    if (!event.GetPointerItem(pId, pointerIterm)) {
        HILOG_ERROR("get GetPointerItem(%d) failed", pId);
        return;
    }

    if (currentDownPoint_.size() != POINTER_COUNT_2 || currentDownPoint_.count(pId) == 0 ||
        !currentDownPoint_[pId]) {
        CancelGesture(false);
        return;
    }

    MMI::PointerEvent::PointerItem firstDownPointerIterm;
    if (!currentDownPoint_[pId]->GetPointerItem(currentDownPoint_[pId]->GetPointerId(), firstDownPointerIterm)) {
        HILOG_ERROR("first down point get GetPointerItem(%d) failed", pId);
        return;
    }

    int32_t offsetX = firstDownPointerIterm.GetDisplayX() - pointerIterm.GetDisplayX();
    int32_t offsetY = firstDownPointerIterm.GetDisplayY() - pointerIterm.GetDisplayY();
    HILOG_DEBUG("current point and first down point: %d, %d, %d, %d", pointerIterm.GetDisplayX(),
        pointerIterm.GetDisplayY(), firstDownPointerIterm.GetDisplayX(), firstDownPointerIterm.GetDisplayY());

    // two finger move not recognize as a gesture
    if (hypot(offsetX, offsetY) >= TOUCH_SLOP * event.GetPointerIds().size()) {
        HILOG_DEBUG("cancel gesture because finger move");
        CancelGesture(false);
    }
}

void AccessibilityMultiTapGestureRecognizer::HandleMultiFingerTouchUpEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("gestureState is %d, isFirstUp is %d, target finger num is %d",
        multiFingerGestureState_, isFirstUp_, targetFingers_);

    handler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    handler_->RemoveEvent(TWO_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG);

    if (multiFingerGestureState_ == MultiFingerGestureState::GESTURE_WAIT) {
        handler_->SendEvent(CANCEL_WAIT_FINGER_DOWN_MSG, event.GetPointerIds().size(), DOUBLE_TAP_TIMEOUT / US_TO_MS);
    }

    if (event.GetPointerIds().size() == POINTER_COUNT_1) {
        fingerTouchUpState_ = FingerTouchUpState::ALL_FINGER_TOUCH_UP;
        currentDownPoint_.clear();
    } else {
        fingerTouchUpState_ = FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP;
    }

    if (isFirstUp_) {
        isFirstUp_ = false;
        if (targetFingers_ != -1 && static_cast<int32_t>(event.GetPointerIds().size()) != targetFingers_) {
            CancelGesture(true);
            return;
        }
    }

    lastUpPoint_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
    if (targetFingers_ == -1 && multiFingerGestureState_ == MultiFingerGestureState::GESTURE_START) {
        targetFingers_ = static_cast<int32_t>(event.GetPointerIds().size());
    }
}

bool AccessibilityMultiTapGestureRecognizer::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("gestureState is %d", multiFingerGestureState_);

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            // cancel last cancel event when recevie a new down event
            CancelAllPenddingEvent();
            isFirstUp_ = true;
            currentDownPoint_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
            if (targetFingers_ == -1) {
                HanleFirstTouchDownEvent(event);
            } else {
                HandleContinueTouchDownEvent(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            if (multiFingerGestureState_ != MultiFingerGestureState::GESTURE_START) {
                return false;
            }

            if (event.GetPointerIds().size() == POINTER_COUNT_2) {
                HandleTwoFingerMoveEvent(event);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            HandleMultiFingerTouchUpEvent(event);
            break;
        default:
            break;
    }
    return false;
}
} // namespace Accessibility
} // namespace OHOS
