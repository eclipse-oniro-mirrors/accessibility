/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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


#ifndef ACCESSIBILITY_TOUCHEVENT_INJECTOR_H
#define ACCESSIBILITY_TOUCHEVENT_INJECTOR_H

#include <time.h>
#include "accessibility_event_transmission.h"
#include "event_handler.h"
#include "event_runner.h"
#include "accessibility_gesture_inject_path.h"
#include "pointer_event.h"
#include "singleton.h"

namespace OHOS {
namespace Accessibility {
const int64_t DOUBLE_TAP_MIN_TIME = 50000; // microsecond

struct SendEventArgs {
    std::shared_ptr<MMI::PointerEvent> event_;
};

class TouchEventInjector;
class TouchInjectHandler : public AppExecFwk::EventHandler {
public:
    TouchInjectHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchEventInjector &server);
    virtual ~TouchInjectHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    TouchEventInjector &server_;
};

class TouchEventInjector : public EventTransmission, public AppExecFwk::EventHandler {
public:
    static constexpr uint32_t SEND_TOUCH_EVENT_MSG = 1;

    /**
     * @brief A constructor used to create a TouchEventInjector instance.
     */
    TouchEventInjector();

    /**
     * @brief A destructor used to delete the TouchEventInjector instance.
     */
    ~TouchEventInjector() {}

    /**
     * @brief Handle pointer events from previous event stream node.
     * @param event the pointer event from Multimodal
     * @return true: the event has been processed and does not need to be passed to the next node;
     *         false: the event is not processed.
     */
    bool OnPointerEvent(MMI::PointerEvent &event) override;

    /**
     * @brief Destroy event state.
     */
    void DestroyEvents() override;

    /**
     * @brief Send pointer event to next stream node.
     * @param event the touch event prepared to send
     */
    void SendPointerEvent(MMI::PointerEvent &event);

    /**
     * @brief Inject simulated gestures.
     * @param gesturePath the gesture path
     */
    void InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);

private:
    /**
     * @brief Cancel the gesture.
     */
    void CancelGesture();

    /**
     * @brief Cancel the injected events.
     */
    void CancelInjectedEvents();

    /**
     * @brief create touchevent.
     * @param action the action of event
     * @param point the endpoint of event
     * @return the created touchevent
     */
    std::shared_ptr<MMI::PointerEvent> obtainTouchEvent(int32_t action,
        MMI::PointerEvent::PointerItem point, int64_t actionTime);

    /**
     * @brief Get the number of microseconds elapsed since the system was booted.
     * @return the number of microseconds elapsed since the system was booted
     */
    int64_t GetSystemTime();

    /**
     * @brief Parse taps events.
     * @param startTime the start time of gesture injection
     * @param gesturePath the gesture path
     */
    void ParseTapsEvents(int64_t startTime, const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);

    /**
     * @brief Parse move events.
     * @param startTime the start time of gesture injection
     * @param gesturePath the gesture path
     */
    void ParseMovesEvents(int64_t startTime, const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);

    /**
     * @brief Parse touchevents from gesturepath.
     * @param startTime the start time of gesture injection
     * @param gesturePath the gesture path
     */
    void ParseTouchEventsFromGesturePath(int64_t startTime,
        const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);

    bool isGestureUnderway_ = false;
    bool isDestroyEvent_ = false;
    std::shared_ptr<TouchInjectHandler> handler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::vector<std::shared_ptr<MMI::PointerEvent>> injectedEvents_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_TOUCHEVENT_INJECTOR_H