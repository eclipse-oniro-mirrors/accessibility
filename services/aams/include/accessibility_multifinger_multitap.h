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
#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessible_ability_manager_service.h"
#include "event_handler.h"
#include "event_runner.h"
#include "pointer_event.h"
#include "singleton.h"

namespace OHOS {
namespace Accessibility {

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
     * @brief Process the two finger gesture event.
     * @param event Indicates the event to be processed.
     */
    bool ProcessTwoFingerGestureEvent(const AppExecFwk::InnerEvent::Pointer &event);
    AccessibilityMultiTapGestureRecognizer &server_;
};

class AccessibilityMultiTapGestureRecognizer : public AppExecFwk::EventHandler {
public:
    static constexpr uint32_t TWO_FINGER_SINGLE_TAP_MSG = 3;
    static constexpr uint32_t TWO_FINGER_LONG_PRESS_MSG = 4;
    static constexpr uint32_t TWO_FINGER_DOUBLE_TAP_MSG = 5;
    static constexpr uint32_t TWO_FINGER_DOUBLE_TAP_AND_HOLD_MSG = 6;
    static constexpr uint32_t TWO_FINGER_TRIPLE_TAP_AND_HOLD_MSG = 7;
    static constexpr uint32_t TWO_FINGER_TRIPLE_TAP_MSG = 8;
    static constexpr uint32_t WAIT_ANOTHER_FINGER_DOWN_MSG = 9;
    static constexpr uint32_t CANCEL_WAIT_FINGER_DOWN_MSG = 10;
    static constexpr uint32_t CANCEL_GESTURE = 11;
    static constexpr uint32_t COMPLETE_GESTURE = 12;

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
     * @return true if the used param is ok, else false.
     */
    bool ParamCheck();

    /**
     * @brief Determine whether it is a two finger double tap gesture.
     * @param event the touch event from Multimodal.
     * @return true if the gesture is two finger double tap, else false.
     */
    bool IsTwoFingerDoubleTap(MMI::PointerEvent &event);

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
     * @brief Handle the two finger touch move event.
     * @param event the touch event to be handled.
     */
    void HandleTwoFingerMoveEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the touch up event, not a move gesture.
     * @param event the touch event to be handled.
     */
    void HandleMultiFingerTouchUpEvent(MMI::PointerEvent &event);

    float touchSlop_ = 0.0f;
    int32_t doubleTapOffsetThresh_ = 0;
    int32_t targetFingers_ = -1; // touch down finger numbers before first time the finger touch up
    int32_t addContinueTapNum_ = 0; // total number of touch down, except the first touch down
    int32_t multiFingerGestureState_ = 0; // recognize state, value is MultiFingerGestureState
    int32_t fingerTouchUpState_ = FingerTouchUpState::ALL_FINGER_TOUCH_UP;
    bool isFirstUp_ = 0; // whether the first time finger touch up

    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> firstDownPoint_; // first round touch down points
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> currentDownPoint_; // current round touch down points
    std::map<int32_t, std::shared_ptr<MMI::PointerEvent>> lastUpPoint_; // last time finger touch up points

    AccessibilityGestureRecognizeListener *listener_ = nullptr;
    std::shared_ptr<MultiFingerGestureHandler> handler_ = nullptr; // multi finger gesture recognize event handler
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_MULTIFINGER_MULTITAP_H