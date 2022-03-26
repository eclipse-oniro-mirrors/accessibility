/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITYGESTURERECOGNIZER_H
#define ACCESSIBILITYGESTURERECOGNIZER_H

#include <cmath>
#include <vector>

#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessibility_extension_context.h"
#include "accessible_ability_manager_service.h"
#include "event_handler.h"
#include "event_runner.h"
#include "hilog_wrapper.h"
#include "pointer_event.h"
#include "singleton.h"

namespace OHOS {
namespace Accessibility {
const int64_t GESTURE_STARTED_TIME_THRESHOLD = 300000; // microsecond
const int64_t GESTURE_NOT_STARTED_TIME_THRESHOLD = 150000; // microsecond
const float DOUBLE_TAP_SLOP = 100.0f;
const int64_t MIN_DOUBLE_TAP_TIME = 40000; // microsecond
const int64_t DOUBLE_TAP_TIMEOUT = 300000; // microsecond
const int64_t LONG_PRESS_TIMEOUT = 400000; // microsecond
const float DEGREES_THRESHOLD = 0.0f;
const int32_t DIRECTION_NUM = 4;
const int64_t US_TO_MS = 1000;
#define CALCULATION_DIMENSION(xdpi) ((xdpi) * (1.0f / 25.4f) * 10)
#define MIN_PIXELS(xyDpi) ((xyDpi) * 0.1f)

struct Pointer {
    float px_;
    float py_;
};

class AccessibilityGestureRecognizer;
class GestureHandler : public AppExecFwk::EventHandler {
public:
    GestureHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, AccessibilityGestureRecognizer &server);
    virtual ~GestureHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    AccessibilityGestureRecognizer &server_;
};

class AccessibilityGestureRecognizeListener {
public:
    /**
     * @brief A destructor used to delete the listener instance.
     * @param
     * @return
     */
    virtual ~AccessibilityGestureRecognizeListener() = default;

    /**
     * @brief The callback function when performed a double tap and then held down the second tap.
     * @param event  the touch event received.
     * @return
     */
    virtual void OnDoubleTapLongPress(MMI::PointerEvent &event);

    /**
     * @brief The callback function when lifted the finger on the second tap of a double tap.
     *
     * @param event  the touch event received.
     * @return true if the event is consumed, else false
     */
    virtual bool OnDoubleTap(MMI::PointerEvent &event);

    /**
     * @brief The callback function when recognized an event stream as a gesture.
     * @param
     * @return true if the event is consumed, else false
     */
    virtual bool OnStarted();

    /**
     * @brief The callback function when decided the event stream is a gesture.
     * @param gestureId  the recognized gesture ID.
     * @return true if the event is consumed, else false
     */
    virtual bool OnCompleted(GestureType gestureId);

    /**
     * @brief The callback function when decided an event stream doesn't match any known gesture.
     * @param event  the touch event received.
     * @return true if the event is consumed, else false
     */
    virtual bool OnCancelled(MMI::PointerEvent &event);
};

class AccessibilityGestureRecognizer : public AppExecFwk::EventHandler {
public:
    /**
     * @brief A constructor used to create a accessibilityGestureRecognizer instance.
     * @param
     * @return
     */
    AccessibilityGestureRecognizer();

    /**
     * @brief A destructor used to delete the accessibilityGestureRecognizer instance.
     * @param
     * @return
     */
    ~AccessibilityGestureRecognizer() {}

    /**
     * @brief Register GestureRecognizeListener.
     * @param listener the listener from touchguide
     * @return
     */
    void RegisterListener(AccessibilityGestureRecognizeListener& listener);

    /**
     * @brief Register GestureRecognizeListener.
     * @param listener the listener from touchguide
     * @return
     */
    void UnregisterListener();

    /**
     * @brief Determine whether a single tap has occurred.
     * @param
     * @return true if a single tap has occurred, else false.
     */
    bool IsfirstTap()
    {
        return isFirstTapUp_;
    }

    /**
     * @brief Handle a touch event. If an action is completed, the appropriate callback is called.
     *
     * @param event  the touch event to be handled.
     * @param rawEvent The raw touch event.
     * @return true if the gesture be recognized, else false
     */
    bool OnPointerEvent(MMI::PointerEvent &event);

    /**
     * @brief Clear state.
     * @param
     * @return
     */
    void Clear();

    /**
     * @brief If a double tap occurred, call OnDoubleTapLongPress callback.
     * @param event the touch event from touchguide
     * @return
     */
    void MaybeRecognizeLongPress(MMI::PointerEvent &event);

    /**
     * @brief If a single tap completed.
     * @param
     * @return
     */
    void SingleTapDetected();

    /**
     * @brief Set isLongpress_ flag;
     * @param value set isLongpress_ flag
     * @return
     */
    void SetIsLongpress (bool value)
    {
        isLongpress_ = value;
    }

    /**
     * @brief Get pCurDown_ ptr.
     * @param
     * @return
     */
    std::shared_ptr<MMI::PointerEvent> GetCurDown()
    {
        return pCurDown_;
    }

    /**
     * @brief Get continueDown_ flag.
     * @param
     * @return
     */
    bool GetContinueDown()
    {
        return continueDown_;
    }

    static constexpr uint32_t LONG_PRESS_MSG = 1;
    static constexpr uint32_t SINGLE_TAP_MSG = 2;

private:
    /**
     * @brief Recognize the standard gesture.
     * @param event the touch event from touchguide
     * @return true if the standard gesture be recognized, else false
     */
    bool StandardGestureRecognizer(MMI::PointerEvent &event);

    /**
     * @brief A double tap has occurred, call OnDoubleTap callback.
     * @param event the touch event from touchguide
     * @return true if the DoubleTap be recognized, else false
     */
    bool DoubleTapRecognized(MMI::PointerEvent &event);

    /**
     * @brief Recognize gestures based on the sequence of motions.
     * @param event the touch event from touchguide
     * @return true if the Direction be recognized, else false
     */
    bool recognizeDirectionGesture(MMI::PointerEvent &event);

    /**
     * @brief Handle the down event from touchguide.
     * @param event the touch event from touchguide
     * @return
     */
    void HandleTouchDownEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the move event from touchguide.
     * @param event the touch event from touchguide
     * @return
     */
    bool HandleTouchMoveEvent(MMI::PointerEvent &event);

    /**
     * @brief Handle the up event from touchguide.
     * @param event the touch event from touchguide
     * @return
     */
    bool HandleTouchUpEvent(MMI::PointerEvent &event);

    /**
     * @brief Check if it's double tap.
     * @param event the touch event from touchguide
     * @return true if it's double tap, else false
     */
    bool isDoubleTap(MMI::PointerEvent &event);

    /**
     * @brief Cancel the gesture.
     * @param
     * @return
     */
    void StandardGestureCancled();

    /**
     * @brief Get pointer path.
     * @param route all pointer route
     * @return the vector of PointerPath
     */
    std::vector<Pointer> GetPointerPath(std::vector<Pointer> &route);

    /**
     * @brief Get swipe direction.
     * @param firstP the startpoint
     * @param secondP the endpoint
     * @return the type of swipe direction
     */
    int GetSwipeDirection(Pointer firstP, Pointer secondP);

    static constexpr int SWIPE_UP = 0;
    static constexpr int SWIPE_DOWN = 1;
    static constexpr int SWIPE_LEFT = 2;
    static constexpr int SWIPE_RIGHT = 3;

    static constexpr GestureType GESTURE_DIRECTION[DIRECTION_NUM] = {
        GestureType::GESTURE_SWIPE_UP,
        GestureType::GESTURE_SWIPE_DOWN,
        GestureType::GESTURE_SWIPE_LEFT,
        GestureType::GESTURE_SWIPE_RIGHT
    };

    static constexpr GestureType GESTURE_DIRECTION_TO_ID[DIRECTION_NUM][DIRECTION_NUM] = {
        {
            GestureType::GESTURE_SWIPE_UP,
            GestureType::GESTURE_SWIPE_UP_THEN_DOWN,
            GestureType::GESTURE_SWIPE_UP_THEN_LEFT,
            GestureType::GESTURE_SWIPE_UP_THEN_RIGHT,
        },
        {
            GestureType::GESTURE_SWIPE_DOWN_THEN_UP,
            GestureType::GESTURE_SWIPE_DOWN,
            GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT,
            GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT,

        },
        {
            GestureType::GESTURE_SWIPE_LEFT_THEN_UP,
            GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN,
            GestureType::GESTURE_SWIPE_LEFT,
            GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT,

        },
        {
            GestureType::GESTURE_SWIPE_RIGHT_THEN_UP,
            GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN,
            GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT,
            GestureType::GESTURE_SWIPE_RIGHT
        }
    };

    bool continueDown_ = false;
    bool isLongpress_ = false;
    bool isDoubleTapdetecting_ = false;
    bool isTapDown_ = false;
    bool isFirstTapUp_ = false;
    bool isDoubleTap_ = false;
    bool isRecognizingGesture_ = false;
    bool isGestureStarted_ = false;
    int64_t startTime_ = 0; // microsecond
    float xMinPixels_ = 0;
    float yMinPixels_ = 0;
    float threshold_ = 0;
    int doubleTapScaledSlop_ = 0;
    MMI::PointerEvent::PointerItem prePointer_ = {};
    MMI::PointerEvent::PointerItem startPointer_ = {};
    std::vector<Pointer> pointerRoute_ {};
    AccessibilityGestureRecognizeListener *listener_ = nullptr;
    std::unique_ptr<MMI::PointerEvent> pPreUp_ = nullptr;
    std::shared_ptr<MMI::PointerEvent> pCurDown_ = nullptr;
    std::shared_ptr<GestureHandler> handler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS

#endif  // ACCESSIBILITYGESTURERECOGNIZER_H