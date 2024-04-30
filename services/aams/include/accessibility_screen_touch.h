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

#ifndef ACCESSIBILITY_SCREEN_TOUCH_H
#define ACCESSIBILITY_SCREEN_TOUCH_H

#include <atomic>
#include <string>
#include <thread>
#include "accessibility_event_transmission.h"
#include "accessibility_gesture_recognizer.h"

namespace OHOS {
namespace Accessibility {

enum ScreenTouchState : int32_t {
    DEFAULT_STATE,
    CLICK_RESPONSE_DELAY_STATE,
    IGNORE_REPEAT_CLICK_STATE,
    BOTH_RESPONSE_DELAY_IGNORE_REPEAT_CLICK
};

class AccessibilityScreenTouch : public EventTransmission {
public:
    /**
     * @brief A constructor used to create a screen touch instance.
     */
    AccessibilityScreenTouch();

    /**
     * @brief A destructor used to delete the screen touch instance.
     */
    ~AccessibilityScreenTouch();

    /**
     * @brief Handle pointer events from previous event stream node.
     *
     * @param event  the pointer event to be handled.
     * @return true: the event has been processed and does not need to be passed to the next node;
     *         false: the event is not processed.
     */
    bool OnPointerEvent(MMI::PointerEvent &event) override;

    uint32_t GetRealClickResponseTime();
    uint32_t GetRealIgnoreRepeatClickTime();
    bool GetRealIgnoreRepeatClickState();
private:
    void HandleResponseDelayStateInnerDown(MMI::PointerEvent &event);
    void HandleResponseDelayStateInnerMove(MMI::PointerEvent &event);
    void HandleResponseDelayStateInnerUp(MMI::PointerEvent &event);
    void HandleResponseDelayState(MMI::PointerEvent &event);

    void HandleIgnoreRepeatClickStateInnerDown(MMI::PointerEvent &event);
    void HandleIgnoreRepeatClickStateInnerMove(MMI::PointerEvent &event);
    void HandleIgnoreRepeatClickStateInnerUp(MMI::PointerEvent &event);
    void HandleIgnoreRepeatClickState(MMI::PointerEvent &event);

    void HandleBothStateInnerDown(MMI::PointerEvent &event);
    void HandleBothStateInnerMove(MMI::PointerEvent &event);
    void HandleBothStateInnerUp(MMI::PointerEvent &event);
    void HandleBothState(MMI::PointerEvent &event);

    void Clear();

    void DrawCircleProgress();

    bool isFirstDownEvent_ = false;
    bool isMoveBeyondThreshold_ = false;
    int64_t startTime_ = 0; // microsecond
    double threshold_ = 0.0;
    MMI::PointerEvent::PointerItem startPointer_ = {};

    int64_t lastUpTime_ = 0;
    int32_t lastDownPointerId_ = -1;
    bool isInterceptClick_ = false;
    
    ScreenTouchState currentState_;
    uint32_t clickResponseTime_;
    bool ignoreRepeatClickState_;
    uint32_t ignoreRepeatClickTime_;

    std::atomic<int32_t> circleCenterPhysicalX_;
    std::atomic<int32_t> circleCenterPhysicalY_;
    std::atomic<bool> isStopDrawCircle_;
    std::shared_ptr<std::thread> drawCircleThread_ = nullptr;

    static int64_t lastUpTime; // global last up time
    static int32_t lastDownPointerId; // global last down pointer id
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_TOUCH_GUIDER_H