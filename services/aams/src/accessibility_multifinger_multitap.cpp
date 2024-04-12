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

#include <cfloat>
#include "accessibility_multifinger_multitap.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr float SLOP_DELTA = 0.5f;
    constexpr int32_t POINTER_COUNT_1 = 1;
    constexpr int32_t POINTER_COUNT_2 = 2;
    constexpr int32_t POINTER_COUNT_3 = 3;
    constexpr int32_t POINTER_COUNT_4 = 4;
    constexpr int32_t MULTI_FINGER_MAX_CONTINUE_TAP_NUM = 3;
    constexpr float TOUCH_SLOP = 8.0f;
    constexpr uint32_t MIN_MOVE_POINTER_NUM = 2;
} // namespace

MultiFingerGestureHandler::MultiFingerGestureHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityMultiTapGestureRecognizer &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

bool MultiFingerGestureHandler::IsTapGesture(const GestureType gestureType)
{
    if (gestureType == GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD ||
        gestureType == GestureType::GESTURE_INVALID) {
        return false;
    }

    return true;
}

void MultiFingerGestureHandler::ProcessMultiFingerGestureTypeEvent(const GestureType gestureType)
{
    HILOG_DEBUG("gesture id: %d", static_cast<int32_t>(gestureType));

    if (IsTapGesture(gestureType)) {
        if (server_.GetFingerTouchUpState() == FingerTouchUpState::ALL_FINGER_TOUCH_UP) {
            server_.GetRecognizeListener()->MultiFingerGestureOnCompleted(gestureType);
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
            server_.Clear();
        }
    } else {
        if (server_.GetFingerTouchUpState() != FingerTouchUpState::NOT_ALL_FINGER_TOUCH_UP) {
            if (gestureType != GestureType::GESTURE_INVALID) {
                server_.GetRecognizeListener()->MultiFingerGestureOnCompleted(gestureType);
            }
            server_.SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
        }
        server_.Clear();
    }
}

bool MultiFingerGestureHandler::ProcessMultiFingerGestureEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG("Inner Event Id id: %u", static_cast<uint32_t>(event->GetInnerEventId()));

    static std::map<uint32_t, GestureType> MULTI_GESTURE_TYPE = {
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_SINGLE_TAP_MSG,
            GestureType::GESTURE_TWO_FINGER_SINGLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_DOUBLE_TAP_MSG,
            GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD},
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_TRIPLE_TAP_MSG,
            GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_SINGLE_TAP_MSG,
            GestureType::GESTURE_THREE_FINGER_SINGLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_DOUBLE_TAP_MSG,
            GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_TRIPLE_TAP_MSG,
            GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_SINGLE_TAP_MSG,
            GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_LONG_PRESS_MSG, GestureType::GESTURE_INVALID},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_DOUBLE_TAP_MSG,
            GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_TRIPLE_TAP_MSG,
            GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP},
        {AccessibilityMultiTapGestureRecognizer::FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD}
    };

    uint32_t eventId = static_cast<uint32_t>(event->GetInnerEventId());
    if (MULTI_GESTURE_TYPE.find(eventId) == MULTI_GESTURE_TYPE.end()) {
        return false;
    }

    GestureType gestureType = MULTI_GESTURE_TYPE.at(eventId);
    ProcessMultiFingerGestureTypeEvent(gestureType);

    return true;
}

void MultiFingerGestureHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    if (!event) {
        HILOG_ERROR("event is null");
        return;
    }

    if (ProcessMultiFingerGestureEvent(event)) {
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
            server_.GetRecognizeListener()->MultiFingerGestureOnCancelled(true);
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

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }

    float density = display->GetVirtualPixelRatio();
    int32_t slop = static_cast<int32_t>(density * DOUBLE_TAP_SLOP + SLOP_DELTA);
    doubleTapOffsetThresh_ = slop;
    touchSlop_ = TOUCH_SLOP;
    mMinPixelsBetweenSamplesX_ = MIN_PIXELS(display->GetWidth());
    mMinPixelsBetweenSamplesY_ = MIN_PIXELS(display->GetHeight());
#else
    HILOG_DEBUG("not support display manager");
    doubleTapOffsetThresh_ = static_cast<int32_t>(1 * DOUBLE_TAP_SLOP + SLOP_DELTA);
    touchSlop_ = TOUCH_SLOP;
    mMinPixelsBetweenSamplesX_ = 1;
    mMinPixelsBetweenSamplesY_ = 1;
#endif

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

void AccessibilityMultiTapGestureRecognizer::CancelThreeFingerEvent()
{
    HILOG_DEBUG();

    if (!handler_) {
        HILOG_ERROR("handler_ is null ptr");
        return;
    }

    handler_->RemoveEvent(THREE_FINGER_SINGLE_TAP_MSG);
    handler_->RemoveEvent(THREE_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(THREE_FINGER_DOUBLE_TAP_MSG);
    handler_->RemoveEvent(THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(THREE_FINGER_TRIPLE_TAP_MSG);
    handler_->RemoveEvent(THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
}

void AccessibilityMultiTapGestureRecognizer::CancelFourFingerEvent()
{
    HILOG_DEBUG();

    if (!handler_) {
        HILOG_ERROR("handler_ is null ptr");
        return;
    }

    handler_->RemoveEvent(FOUR_FINGER_SINGLE_TAP_MSG);
    handler_->RemoveEvent(FOUR_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(FOUR_FINGER_DOUBLE_TAP_MSG);
    handler_->RemoveEvent(FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(FOUR_FINGER_TRIPLE_TAP_MSG);
    handler_->RemoveEvent(FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
}

void AccessibilityMultiTapGestureRecognizer::CancelTapAndHoldGestureEvent(const int32_t fingerNum)
{
    HILOG_DEBUG();

    switch (fingerNum) {
        case POINTER_COUNT_2:
            CancelTwoFingerEvent();
            break;
        case POINTER_COUNT_3:
            CancelThreeFingerEvent();
            break;
        case POINTER_COUNT_4:
            CancelFourFingerEvent();
            break;
        default:
            break;
    }
}

void AccessibilityMultiTapGestureRecognizer::CancelHoldGestureEvent()
{
    HILOG_DEBUG();

    if (!handler_) {
        HILOG_ERROR("handler_ is null ptr");
        return;
    }

    handler_->RemoveEvent(TWO_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(THREE_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(FOUR_FINGER_LONG_PRESS_MSG);
    handler_->RemoveEvent(FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG);
    handler_->RemoveEvent(FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG);
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
    CancelThreeFingerEvent();
    CancelFourFingerEvent();
}

void AccessibilityMultiTapGestureRecognizer::Clear()
{
    HILOG_DEBUG();

    targetFingers_ = -1;
    addContinueTapNum_ = 0;
    isMoveGestureRecognizing = 0;
    moveDirection = -1;
    firstDownPoint_.clear();
    lastUpPoint_.clear();
    currentDownPoint_.clear();
    preGesturePoint_.clear();
    pointerRoute_.clear();
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

    listener_->MultiFingerGestureOnCancelled(isNoDelayFlag);
    Clear();
}

bool AccessibilityMultiTapGestureRecognizer::ParamCheck(const int32_t fingerNum)
{
    if (static_cast<int32_t>(lastUpPoint_.size()) < fingerNum ||
        static_cast<int32_t>(firstDownPoint_.size()) < fingerNum) {
        HILOG_ERROR("last_up point or first_down point size is less than target fingerNum");
        return false;
    }

    for (int pId = 0; pId < fingerNum; pId++) {
        if (!lastUpPoint_.count(pId) || !lastUpPoint_[pId]) {
            HILOG_ERROR("last_up point or first_down point container has wrong value and pId is: %d", pId);
            return false;
        }
    }

    return true;
}

int64_t AccessibilityMultiTapGestureRecognizer::GetLastFirstPointUpTime(const int32_t fingerNum)
{
    HILOG_DEBUG();

    int64_t timeRst = lastUpPoint_[0]->GetActionTime();
    for (int32_t pId = 1; pId < fingerNum; pId++) {
        if (lastUpPoint_[pId]->GetActionTime() < timeRst) {
            timeRst = lastUpPoint_[pId]->GetActionTime();
        }
    }
    return timeRst;
}

bool AccessibilityMultiTapGestureRecognizer::IsDoubelTapSlopConditionMatch(const int32_t fingerNum,
    const std::vector<MMI::PointerEvent::PointerItem> &curPoints,
    const std::vector<MMI::PointerEvent::PointerItem> &prePoints)
{
    HILOG_DEBUG("doubleTapOffsetThresh_, %d", doubleTapOffsetThresh_);

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
        HILOG_DEBUG("moveDelta = %f, right = %d", moveDelta, doubleTapOffsetThresh_ * fingerNum);
        if (moveDelta < doubleTapOffsetThresh_ * fingerNum) {
            excludePid.push_back(nearestPid);
        } else {
            return false;
        }
    }

    return true;
}

bool AccessibilityMultiTapGestureRecognizer::GetPointerItemWithFingerNum(int32_t fingerNum,
    std::vector<MMI::PointerEvent::PointerItem> &curPoints,
    std::vector<MMI::PointerEvent::PointerItem> &prePoints, MMI::PointerEvent &event,
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> &prePointsEventInfo)
{
    HILOG_DEBUG();

    std::vector<int32_t> pIds = event.GetPointerIds();
    for (int32_t pId = 0; pId < fingerNum; pId++) {
        if (!event.GetPointerItem(pIds[pId], curPoints[pId])) {
            HILOG_ERROR("curPoint GetPointerItem(%d) failed", pIds[pId]);
            return false;
        }
        if (!prePointsEventInfo[pId]->GetPointerItem(prePointsEventInfo[pId]->GetPointerId(), prePoints[pId])) {
            HILOG_ERROR("prePoint GetPointerItem(%d) failed", prePointsEventInfo[pId]->GetPointerId());
            return false;
        }
    }
    return true;
}

bool AccessibilityMultiTapGestureRecognizer::IsMultiFingerDoubleTap(MMI::PointerEvent &event,
    const int32_t fingerNum)
{
    HILOG_DEBUG("fingerNum is %d", fingerNum);

    if (!ParamCheck(fingerNum)) {
        return false;
    }

    // first pointer up time to second pointer down time
    int64_t firstUpTime = GetLastFirstPointUpTime(fingerNum);
    int64_t durationTime = event.GetActionTime() - firstUpTime;
    if (durationTime > DOUBLE_TAP_TIMEOUT || durationTime < MIN_DOUBLE_TAP_TIME) {
        HILOG_WARN("durationTime[%{public}" PRId64 "] is wrong", durationTime);
        return false;
    }

    std::vector<int32_t> pIds = event.GetPointerIds();
    if (static_cast<int32_t>(pIds.size()) != fingerNum) {
        return false;
    }

    std::vector<MMI::PointerEvent::PointerItem> curPoints(fingerNum);
    std::vector<MMI::PointerEvent::PointerItem> prePoints(fingerNum);
    if (!GetPointerItemWithFingerNum(fingerNum, curPoints, prePoints, event, firstDownPoint_)) {
        return false;
    }

    return IsDoubelTapSlopConditionMatch(fingerNum, curPoints, prePoints);
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
        listener_->MultiFingerGestureOnStarted(true);
        SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_START);
        handler_->SendEvent(TWO_FINGER_SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        handler_->SendEvent(TWO_FINGER_LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
    } else if (event.GetPointerIds().size() == POINTER_COUNT_3) {
        listener_->MultiFingerGestureOnStarted(false);
        handler_->SendEvent(THREE_FINGER_SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        handler_->SendEvent(THREE_FINGER_LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
    } else if (event.GetPointerIds().size() == POINTER_COUNT_4) {
        handler_->SendEvent(FOUR_FINGER_SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
        handler_->SendEvent(FOUR_FINGER_LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT / US_TO_MS);
    } else {
        CancelGesture(true);
    }
}

void AccessibilityMultiTapGestureRecognizer::HandleMultiTapEvent(MMI::PointerEvent &event, const int32_t fingerNum)
{
    HILOG_DEBUG("fingerNum is %d", fingerNum);

    // check is double tap
    if (static_cast<int32_t>(firstDownPoint_.size()) == fingerNum &&
        static_cast<int32_t>(lastUpPoint_.size()) == fingerNum &&
        IsMultiFingerDoubleTap(event, fingerNum)) {
        HILOG_DEBUG("two finger Double tap is recognized, addContinueTapNum %d", addContinueTapNum_);
        addContinueTapNum_ = addContinueTapNum_ + 1;
    } else {
        addContinueTapNum_ = 0;
    }
    if (fingerNum < POINTER_COUNT_2 || fingerNum > POINTER_COUNT_4) {
        HILOG_ERROR("fingerNum: %d is wrong", fingerNum);
        return;
    }
    if (addContinueTapNum_ >= MULTI_FINGER_MAX_CONTINUE_TAP_NUM) {
        HILOG_ERROR("continue tap times: %u is wrong", addContinueTapNum_);
        CancelGesture(true);
        return;
    }
    uint32_t fingerNumIndex = static_cast<uint32_t>(fingerNum - 2);
    handler_->SendEvent(GESTURE_TAP_MSG[addContinueTapNum_][fingerNumIndex], 0, DOUBLE_TAP_TIMEOUT / US_TO_MS);
    handler_->SendEvent(GESTURE_HOLD_MSG[addContinueTapNum_][fingerNumIndex], 0, LONG_PRESS_TIMEOUT / US_TO_MS);
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
        HandleMultiTapEvent(event, targetFingers_);
    } else {
        HILOG_DEBUG("current fingers is more than last touch down finger nums");
        CancelGesture(true);
    }
}

void AccessibilityMultiTapGestureRecognizer::storeBaseDownPoint()
{
    HILOG_DEBUG();

    for (auto iter : currentDownPoint_) {
        Pointer mp;
        MMI::PointerEvent::PointerItem pointerIterm;
        std::vector<Pointer> mpVec;
        int32_t pId = iter.first;

        if (!iter.second->GetPointerItem(pId, pointerIterm)) {
            HILOG_ERROR("get GetPointerItem(%d) failed", pId);
            return;
        }

        mp.px_ = static_cast<float>(pointerIterm.GetDisplayX());
        mp.py_ = static_cast<float>(pointerIterm.GetDisplayY());
        mpVec.push_back(mp);
        pointerRoute_.insert(std::make_pair(pId, mpVec));
    }
}

int32_t AccessibilityMultiTapGestureRecognizer::GetSwipeDirection(const int32_t dx, const int32_t dy)
{
    HILOG_DEBUG();

    if (abs(dx) > abs(dy)) {
        return (dx < 0) ? MoveGirectionType::SWIPE_LEFT : MoveGirectionType::SWIPE_RIGHT;
    } else {
        return (dy < 0) ? MoveGirectionType::SWIPE_UP : MoveGirectionType::SWIPE_DOWN;
    }
}

void AccessibilityMultiTapGestureRecognizer::SaveMoveGesturePointerInfo(MMI::PointerEvent &event,
    const int32_t pId, const MMI::PointerEvent::PointerItem &pointerIterm, const int32_t dx, const int32_t dy)
{
    HILOG_DEBUG();

    int32_t currentDirection = GetSwipeDirection(dx, dy);
    if (!isMoveGestureRecognizing) {
        storeBaseDownPoint();
        moveDirection = currentDirection;
        isMoveGestureRecognizing = true;
        return;
    }

    if (moveDirection != currentDirection) {
        CancelGesture(true);
        return;
    }
    Pointer mp;
    mp.px_ = static_cast<float>(pointerIterm.GetDisplayX());
    mp.py_ = static_cast<float>(pointerIterm.GetDisplayY());
    pointerRoute_[pId].push_back(mp);
    //update preGesturePoint_
    preGesturePoint_[pId] = std::make_shared<MMI::PointerEvent>(event);
}

bool AccessibilityMultiTapGestureRecognizer::GetBasePointItem(MMI::PointerEvent::PointerItem &basePointerIterm,
    int32_t pId, std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> &pointInfo)
{
    HILOG_DEBUG();

    if (pointInfo.count(pId) == 0 || !pointInfo[pId]) {
        return false;
    }
    if (!pointInfo[pId]->GetPointerItem(pointInfo[pId]->GetPointerId(), basePointerIterm)) {
        HILOG_ERROR("base down point get GetPointerItem(%d) failed", pId);
        return false;
    }

    return true;
}

void AccessibilityMultiTapGestureRecognizer::HandleMultiFingerMoveEvent(MMI::PointerEvent &event)
{
    int32_t pIdSize = static_cast<int32_t>(event.GetPointerIds().size());
    int32_t downPointSize = static_cast<int32_t>(currentDownPoint_.size());
    int32_t pId = event.GetPointerId();
    HILOG_DEBUG("pointer num is %d, down pointer size is %d, pointId is %d", pIdSize, downPointSize, pId);

    MMI::PointerEvent::PointerItem pointerIterm;
    if (!event.GetPointerItem(pId, pointerIterm)) {
        HILOG_ERROR("get GetPointerItem(%d) failed", pId);
        return;
    }

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (isMoveGestureRecognizing) {
        if (!GetBasePointItem(basePointerIterm, pId, preGesturePoint_)) {
            return;
        }
    } else {
        if (!GetBasePointItem(basePointerIterm, pId, currentDownPoint_)) {
            return;
        }
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    HILOG_DEBUG("current point and first down point: pid %d, %d, %d, %d, %d", pId, pointerIterm.GetDisplayX(),
        pointerIterm.GetDisplayY(), basePointerIterm.GetDisplayX(), basePointerIterm.GetDisplayY());

    // two finger move will cancel gesture, but three or four finger move will enter move gesture recognize
    if (!isMoveGestureRecognizing && hypot(offsetX, offsetY) >= TOUCH_SLOP * downPointSize) {
        if (downPointSize == POINTER_COUNT_2) {
            HILOG_DEBUG("cancel gesture because finger move");
            CancelGesture(false);
        } else {
            CancelThreeFingerEvent();
            CancelFourFingerEvent();
            SaveMoveGesturePointerInfo(event, pId, pointerIterm, offsetX, offsetY);
        }
    } else if (isMoveGestureRecognizing && (abs(offsetX) >= mMinPixelsBetweenSamplesX_ ||
        abs(offsetY) >= mMinPixelsBetweenSamplesY_)) {
        SaveMoveGesturePointerInfo(event, pId, pointerIterm, offsetX, offsetY);
    }
}

void AccessibilityMultiTapGestureRecognizer::StoreUpPointInPointerRoute(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (!isMoveGestureRecognizing || multiFingerGestureState_ != MultiFingerGestureState::GESTURE_START) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerIterm;
    int32_t pId = event.GetPointerId();
    if (!event.GetPointerItem(pId, pointerIterm)) {
        HILOG_ERROR("get GetPointerItem(%d) failed", pId);
        return;
    }

    MMI::PointerEvent::PointerItem basePointerIterm;
    if (!GetBasePointItem(basePointerIterm, pId, preGesturePoint_)) {
        return;
    }

    int32_t offsetX = pointerIterm.GetDisplayX() - basePointerIterm.GetDisplayX();
    int32_t offsetY = pointerIterm.GetDisplayY() - basePointerIterm.GetDisplayY();
    if (abs(offsetX) > mMinPixelsBetweenSamplesX_ || abs(offsetY) > mMinPixelsBetweenSamplesY_) {
        SaveMoveGesturePointerInfo(event, pId, pointerIterm, offsetX, offsetY);
    }
}

bool AccessibilityMultiTapGestureRecognizer::recognizeGesturePath(const std::vector<Pointer> &path)
{
    HILOG_DEBUG();
    if (path.size() < MIN_MOVE_POINTER_NUM) {
        return false;
    }

    int pathSize = static_cast<int>(path.size() - 1);
    for (int routerIndex = 0; routerIndex < pathSize; routerIndex++) {
        int32_t dx = static_cast<int32_t>(path[routerIndex + 1].px_ - path[routerIndex].px_);
        int32_t dy = static_cast<int32_t>(path[routerIndex + 1].py_ - path[routerIndex].py_);
        if (GetSwipeDirection(dx, dy) != moveDirection) {
            return false;
        }
    }
    return true;
}

GestureType AccessibilityMultiTapGestureRecognizer::GetMoveGestureId()
{
    HILOG_DEBUG();

    int32_t downPointSize = static_cast<int32_t>(currentDownPoint_.size());
    if (downPointSize == POINTER_COUNT_3) {
        switch (moveDirection) {
            case MoveGirectionType::SWIPE_LEFT:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_LEFT;
            case MoveGirectionType::SWIPE_RIGHT:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_RIGHT;
            case MoveGirectionType::SWIPE_UP:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_UP;
            case MoveGirectionType::SWIPE_DOWN:
                return GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN;
            default:
                return GestureType::GESTURE_INVALID;
        }
    } else if (downPointSize == POINTER_COUNT_4) {
        switch (moveDirection) {
            case MoveGirectionType::SWIPE_LEFT:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_LEFT;
            case MoveGirectionType::SWIPE_RIGHT:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_RIGHT;
            case MoveGirectionType::SWIPE_UP:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_UP;
            case MoveGirectionType::SWIPE_DOWN:
                return GestureType::GESTURE_FOUR_FINGER_SWIPE_DOWN;
            default:
                return GestureType::GESTURE_INVALID;
        }
    }
    return GestureType::GESTURE_INVALID;
}

bool AccessibilityMultiTapGestureRecognizer::IsMoveGestureRecognize()
{
    HILOG_DEBUG();

    if (!isMoveGestureRecognizing || multiFingerGestureState_ != MultiFingerGestureState::GESTURE_START) {
        return false;
    }

    int32_t downPointSize = static_cast<int32_t>(currentDownPoint_.size());
    if (static_cast<int32_t>(pointerRoute_.size()) != downPointSize) {
        return false;
    }

    for (int32_t pIndex = 0; pIndex < downPointSize; pIndex++) {
        if (pointerRoute_.count(pIndex) == 0 || pointerRoute_[pIndex].size() < MIN_MOVE_POINTER_NUM) {
            return false;
        }
        if (!recognizeGesturePath(pointerRoute_[pIndex])) {
            return false;
        }
    }

    GestureType gestureId = GetMoveGestureId();
    listener_->MultiFingerGestureOnCompleted(gestureId);

    return true;
}

void AccessibilityMultiTapGestureRecognizer::HandleMultiFingerTouchUpEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("gestureState is %d, isFirstUp is %d, target finger num is %d",
        multiFingerGestureState_, isFirstUp_, targetFingers_);

    handler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    CancelHoldGestureEvent();

    if (multiFingerGestureState_ == MultiFingerGestureState::GESTURE_WAIT) {
        handler_->SendEvent(CANCEL_WAIT_FINGER_DOWN_MSG, event.GetPointerIds().size(), DOUBLE_TAP_TIMEOUT / US_TO_MS);
    }

    StoreUpPointInPointerRoute(event);
    if (event.GetPointerIds().size() == POINTER_COUNT_1) {
        if (IsMoveGestureRecognize()) {
            SetMultiFingerGestureState(MultiFingerGestureState::GESTURE_COMPLETE);
            fingerTouchUpState_ = FingerTouchUpState::ALL_FINGER_TOUCH_UP;
            Clear();
            return;
        }
        fingerTouchUpState_ = FingerTouchUpState::ALL_FINGER_TOUCH_UP;
        currentDownPoint_.clear();
        preGesturePoint_.clear();
        pointerRoute_.clear();
        moveDirection = -1;
        isMoveGestureRecognizing = false;
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
            preGesturePoint_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
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
            HandleMultiFingerMoveEvent(event);
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
