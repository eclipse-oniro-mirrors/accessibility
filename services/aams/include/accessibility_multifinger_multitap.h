/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ACCESSIBILITY_MULTIFINGER_MULTITAP_H
#define ACCESSIBILITY_MULTIFINGER_MULTITAP_H

#include <cmath>
#include <vector>

#include "accessibility_gesture_recognizer.h"
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "accessibility_event_info.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_def.h"
#include "event_handler.h"
#include "event_runner.h"
#include "pointer_event.h"
#include "singleton.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t MAX_TAP_NUM = 3;
    constexpr uint32_t MAX_MULTI_FINGER_TYPE = 3;
} // namespace

enum MoveGirectionType : int32_t {
    SWIPE_LEFT = 0,
    SWIPE_RIGHT = 1,
    SWIPE_UP = 2,
    SWIPE_DOWN = 3
};

enum FingerTouchUpState : int32_t {
    NOT_ALL_FINGER_TOUCH_UP = 0,
    ALL_FINGER_TOUCH_UP = 1,
    TOUCH_DOWN_AFTER_ALL_FINGER_TOUCH_UP = 2,
};

enum MultiFingerGestureState : int32_t {
    GESTURE_NOT_START = 0,
    GESTURE_START = 1,
    GESTURE_CANCLE = 2,
    GESTURE_COMPLETE = 3,
    GESTURE_WAIT = 4,
};

class AccessibilityMultiTapGestureRecognizer;
class MultiFingerGestureHandler : public AppExecFwk::EventHandler {
public:
    MultiFingerGestureHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
        AccessibilityMultiTapGestureRecognizer &server);
    virtual ~MultiFingerGestureHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    /**
     * @brief Process the multi finger gesture by Gesture type.
     * @param gestureType Indicates the gesture type to be processed.
     */
    void ProcessMultiFingerGestureTypeEvent(const GestureType gestureType);

    /**
     * @brief Process the multi finger gesture event.
     * @param event Indicates the event to be processed.
     * @return true if the gesture event is processed success, else false.
     */
    bool ProcessMultiFingerGestureEvent(const AppExecFwk::InnerEvent::Pointer &event);

    /**
     * @brief check whether the gesture is a tap gesture.
     * @param gestureType Indicates the gesture to be processed.
     * @return true if the gesture is a tap gesture, else false.
     */
    bool IsTapGesture(const GestureType gestureType);

    AccessibilityMultiTapGestureRecognizer &server_;
};

class AccessibilityMultiTapGestureRecognizer : public AppExecFwk::EventHandler {
public:
    static constexpr uint32_t TWO_FINGER_SINGLE_TAP_MSG = 3;
    static constexpr uint32_t TWO_FINGER_LONG_PRESS_MSG = 4;
    static constexpr uint32_t TWO_FINGER_DOUBLE_TAP_MSG = 5;
    static constexpr uint32_t TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG = 6;
    static constexpr uint32_t TWO_FINGER_TRIPLE_TAP_MSG = 7;
    static constexpr uint32_t TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG = 8;
    static constexpr uint32_t THREE_FINGER_SINGLE_TAP_MSG = 9;
    static constexpr uint32_t THREE_FINGER_LONG_PRESS_MSG = 10;
    static constexpr uint32_t THREE_FINGER_DOUBLE_TAP_MSG = 11;
    static constexpr uint32_t THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG = 12;
    static constexpr uint32_t THREE_FINGER_TRIPLE_TAP_MSG = 13;
    static constexpr uint32_t THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG = 14;
    static constexpr uint32_t FOUR_FINGER_SINGLE_TAP_MSG = 15;
    static constexpr uint32_t FOUR_FINGER_LONG_PRESS_MSG = 16;
    static constexpr uint32_t FOUR_FINGER_DOUBLE_TAP_MSG = 17;
    static constexpr uint32_t FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG = 18;
    static constexpr uint32_t FOUR_FINGER_TRIPLE_TAP_MSG = 19;
    static constexpr uint32_t FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG = 20;
    static constexpr uint32_t WAIT_ANOTHER_FINGER_DOWN_MSG = 21;
    static constexpr uint32_t CANCEL_WAIT_FINGER_DOWN_MSG = 22;
    static constexpr uint32_t CANCEL_GESTURE = 23;
    static constexpr uint32_t COMPLETE_GESTURE = 24;

    static constexpr uint32_t GESTURE_TAP_MSG[MAX_TAP_NUM][MAX_MULTI_FINGER_TYPE] = {
        {
            TWO_FINGER_SINGLE_TAP_MSG,
            THREE_FINGER_SINGLE_TAP_MSG,
            FOUR_FINGER_SINGLE_TAP_MSG,
        },
        {
            TWO_FINGER_DOUBLE_TAP_MSG,
            THREE_FINGER_DOUBLE_TAP_MSG,
            FOUR_FINGER_DOUBLE_TAP_MSG,
        },
        {
            TWO_FINGER_TRIPLE_TAP_MSG,
            THREE_FINGER_TRIPLE_TAP_MSG,
            FOUR_FINGER_TRIPLE_TAP_MSG,
        }
    };

    static constexpr uint32_t GESTURE_HOLD_MSG[MAX_TAP_NUM][MAX_MULTI_FINGER_TYPE] = {
        {
            TWO_FINGER_LONG_PRESS_MSG,
            THREE_FINGER_LONG_PRESS_MSG,
            FOUR_FINGER_LONG_PRESS_MSG,
        },
        {
            TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            THREE_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
            FOUR_FINGER_DOUBLE_TAP_AND_HOLD_MSG,
        },
        {
            TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            THREE_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
            FOUR_FINGER_TRIPLE_TAP_AND_HOLD_MSG,
        }
    };

    AccessibilityMultiTapGestureRecognizer();
    ~AccessibilityMultiTapGestureRecognizer() = default;

    /**
     * @brief Register GestureRecognizeListener.
     * @param listener the listener from touchguide.
     */
    void RegisterListener(AccessibilityGestureRecognizeListener &listener);

    /**
     * @brief Get the GestureRecognizeListener.
     * @return AccessibilityGestureRecognizeListener ptr.
     */
    AccessibilityGestureRecognizeListener *GetRecognizeListener() const
    {
        return listener_;
    }

    /**
     * @brief Handle a touch event. If an action is completed, the appropriate callback is called.
     *
     * @param event  the touch event to be handled.
     * @return true if the gesture be recognized, else false.
     */
    bool OnPointerEvent(MMI::PointerEvent &event);

    /**
     * @brief Cancle multi finger gesture rocognize state, buffer etc.
     */
    void Clear();

    /**
     * @brief Get the target fingers number touch down in first round.
     * @return the finger numbers touch down in first round.
     */
    int32_t GetTargetFingers() const
    {
        return targetFingers_;
    }

    /**
     * @brief Get finger touch up state.
     * @return the touch up state, indicates if fingers is still on the screen.
     */
    int32_t GetFingerTouchUpState() const
    {
        return fingerTouchUpState_;
    }

    /**
     * @brief Set the finger touch up state when touch up or finish touch down.
     * @param touchUpState the touchUpState to be set.
     */
    void SetFingerTouchUpState(const int32_t touchUpState)
    {
        fingerTouchUpState_ = touchUpState;
    }

    /**
     * @brief Set multi finger gesture state, when gesture recognize start, cancel, complete etc.
     * @param gestureState the multiFingerGestureState to be set.
     */
    void SetMultiFingerGestureState(const int32_t gestureState)
    {
        multiFingerGestureState_ = gestureState;
    }

    /**
     * @brief Determine whether multi finger gesture is started.
     * @return true if gesture recognize is started, else false.
     */
    bool IsMultiFingerGestureStarted() const
    {
        return multiFingerGestureState_ == MultiFingerGestureState::GESTURE_START;
    }

    /**
     * @brief Determine whether multi finger gesture is started or finished.
     * @return true if gesture recognize is started or finished, else false.
     */
    bool IsMultiFingerRecognize() const
    {
        return (multiFingerGestureState_ == MultiFingerGestureState::GESTURE_START ||
            multiFingerGestureState_ == MultiFingerGestureState::GESTURE_COMPLETE);
    }

    /**
     * @brief Get the two finger move threshold.
     * @return the two finger move threshold, indicates tap state to move state.
     */
    float GetTouchSlop() const
    {
        return touchSlop_;
    }
private:
    /**
     * @brief Cancle the pendding two finger gesture recognize event.
     */
    void CancelTwoFingerEvent();

    /**
     * @brief Cancle the pendding three finger gesture recognize event.
     */
    void CancelThreeFingerEvent();

    /**
     * @brief Cancle the pendding four finger gesture recognize event.
     */
    void CancelFourFingerEvent();

    /**
     * @brief Cancle the pendding finger gesture recognize event by finger num.
     * @param fingerNum which type gesture pendding event to be canceled.
     */
    void CancelTapAndHoldGestureEvent(const int32_t fingerNum);

    /**
     * @brief Cancle the pendding multi finger hold gesture recognize event.
     */
    void CancelHoldGestureEvent();

    /**
     * @brief Cancle All pendding inner event.
     */
    void CancelAllPenddingEvent();

    /**
     * @brief Cancel the multi gesture recognize process.
     * @param isNoDelayFlag if gesture cancel event is immediately processed.
     */
    void CancelGesture(const bool isNoDelayFlag);

    /**
     * @brief param check for two finger Double tap recognize gesture.
     * @param fingerNum the touch event from Multimodal.
     * @return true if the used param is ok, else false.
     */
    bool ParamCheck(const int32_t fingerNum);

    /**
     * @brief Get the last touch up time of the first finger.
     * @param fingerNum total touch down finger nums, means the lastUpPoint_ size.
     * @return the first finger's last touch up time.
     */
    int64_t GetLastFirstPointUpTime(const int32_t fingerNum);

    /**
     * @brief Get the current and pre PointerItems with fingerNum, store in curPoints and prePoints.
     * @param curPoints to store the cur touch event pointerItems rst, size is fingerNum.
     * @param prePoints to store the pre touch event pointerItems rst, size is fingerNum.
     * @param event current touch event, to get the curPoints.
     * @param prePointsEventInfo pre touch Event storage, to get the prePoints.
     * @return true if the get rst is ok, else false.
     */
    bool GetPointerItemWithFingerNum(int32_t fingerNum, std::vector<MMI::PointerEvent::PointerItem> &curPoints,
        std::vector<MMI::PointerEvent::PointerItem> &prePoints, MMI::PointerEvent &event,
        std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> &prePointsEventInfo);

    /**
     * @brief check whether the two taps offset is less than slop threshold.
     * @param fingerNum touch down finger nums to be processed.
     * @param curPoints current touch down pointer infos, size is fingerNum.
     * @param prePoints first round touch down pointer infos, size is fingfingerNumerNums.
     * @return true if the offset of two taps is less than slop threshold, else false.
     */
    bool IsDoubelTapSlopConditionMatch(const int32_t fingerNum,
        const std::vector<MMI::PointerEvent::PointerItem> &curPoints,
        const std::vector<MMI::PointerEvent::PointerItem> &prePoints);

    /**
     * @brief Determine whether it is a multi finger double tap gesture.
     * @param event the touch event from Multimodal.
     * @param fingerNum the target fingerNum to be processed.
     * @return true if the gesture is multi finger double tap, else false.
     */
    bool IsMultiFingerDoubleTap(MMI::PointerEvent &event, const int32_t fingerNum);

    /**
     * @brief save touch down point event info.
     */
    void storeBaseDownPoint();

    /**
     * @brief Get move direction by move distance.
     * @param dx the x axis distance between base point and current point.
     * @param dy the y axis distance between base point and current point.
     * @return the move direction, value range is MoveGirectionType.
     */
    int32_t GetSwipeDirection(const int32_t dx, const int32_t dy);

    /**
     * @brief Get the base point Item info by point Id.
     * @param basePointerIterm to save th base pointItem info.
     * @param pId the point Id to get the pointItem.
     * @param pointInfo the touch down event point info storage.
     * @return true if get base pointItem success, else false.
     */
    bool GetBasePointItem(MMI::PointerEvent::PointerItem &basePointerIterm,
        int32_t pId, std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> &pointInfo);

    /**
     * @brief Save move gesture path info.
     * @param event the touch event to be handled.
     * @param pId the point Id to be handled.
     * @param pointerIterm the point Item info to be saved.
     * @param dx the x axis distance between base point and current point.
     * @param dy the y axis distance between base point and current point.
     */
    void SaveMoveGesturePointerInfo(MMI::PointerEvent &event,
        const int32_t pId, const MMI::PointerEvent::PointerItem &pointerIterm, const int32_t dx, const int32_t dy);

    /**
     * @brief recognize the move path is correct and match a move gesture.
     * @param path move path pointer info storage.
     * @return true if the move path is correct and match a move gesture, else false.
     */
    bool recognizeGesturePath(const std::vector<Pointer> &path);

    /**
     * @brief Get the matched move gesture by moveDirection and fingerNum.
     * @return the matched gesture Id.
     */
    GestureType GetMoveGestureId();

    /**
     * @brief whether the multi finger move event match a move gesture.
     * @return true if multi finger move event match a move gesture, else false.
     */
    bool IsMoveGestureRecognize();

    /**
     * @brief Save move gesture path info when finger up.
     * @param event the touch event to be handled.
     */
    void StoreUpPointInPointerRoute(MMI::PointerEvent &event);

    /**
     * @brief Handle the first touch down event.
     * @param event the touch event to be handled.
     */
    void HanleFirstTouchDownEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the continue touch down event.
     * @param event the touch event to be handled.
     */
    void HandleContinueTouchDownEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the multi finger touch move event.
     * @param event the touch event to be handled.
     */
    void HandleMultiFingerMoveEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the touch up event, not a move gesture.
     * @param event the touch event to be handled.
     */
    void HandleMultiFingerTouchUpEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the continue touch down event, decide whether it is a multi tap event.
     * @param event the touch event to be handled.
     * @param fingerNum the touch down fingerNum to be handled.
     */
    void HandleMultiTapEvent(MMI::PointerEvent &event, const int32_t fingerNum);

    float touchSlop_ = 0.0f;
    int32_t doubleTapOffsetThresh_ = 0;
    int32_t targetFingers_ = -1; // touch down finger numbers before first time the finger touch up
    uint32_t addContinueTapNum_ = 0; // total number of touch down, except the first touch down
    int32_t multiFingerGestureState_ = 0; // recognize state, value is MultiFingerGestureState
    int32_t fingerTouchUpState_ = FingerTouchUpState::ALL_FINGER_TOUCH_UP;
    bool isFirstUp_ = 0; // whether the first time finger touch up
    bool isMoveGestureRecognizing = false; // in move gesture recognize process or not
    int32_t moveDirection = -1;
    float mMinPixelsBetweenSamplesX_ = 0;
    float mMinPixelsBetweenSamplesY_ = 0;

    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> firstDownPoint_; // first round touch down points
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> currentDownPoint_; // current round touch down points
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> preGesturePoint_; // pre move event points
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> lastUpPoint_; // last time finger touch up points
    std::map<int32_t, std::vector<Pointer>> pointerRoute_;

    AccessibilityGestureRecognizeListener *listener_ = nullptr;
    std::shared_ptr<MultiFingerGestureHandler> handler_ = nullptr; // multi finger gesture recognize event handler
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_MULTIFINGER_MULTITAP_H