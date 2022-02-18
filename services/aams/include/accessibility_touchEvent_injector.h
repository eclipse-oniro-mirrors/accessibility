/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "pointer_event.h"
#include "singleton.h"
#include "event_handler.h"
#include "event_runner.h"
#include "hilog_wrapper.h"
#include "gesture_simulation.h"
#include "accessibility_event_transmission.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_interface.h"

namespace OHOS {
namespace Accessibility {

#define DOUBLE_TAP_MIN_TIME 50
class AccessibleAbilityManagerService;

struct SendEventArgs {
    std::shared_ptr<MMI::PointerEvent> event_;
    bool isLastEvent_;
};

class TouchEventInjector;
class TouchInjectHandler : public AppExecFwk::EventHandler {
public:
    TouchInjectHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchEventInjector &server);
    virtual ~TouchInjectHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    TouchEventInjector &server_;
};

class TouchEventInjector : public EventTransmission, public AppExecFwk::EventHandler {
public:
    /**
     * @brief A constructor used to create a TouchEventInjector instance.
     * @param
     * @return
     */
    TouchEventInjector();

    /**
     * @brief A destructor used to delete the TouchEventInjector instance.
     * @param
     * @return
     */
    ~TouchEventInjector() {}

    /**
     * @brief Handle pointer events from previous event stream node.
     * @param event the pointer event from Multimodal
     * @return
     */
    void OnPointerEvent(MMI::PointerEvent &event) override;

    /**
     * @brief Clear event state from specific input source.
     * @param inputSource the input source
     * @return
     */
    void ClearEvents(uint32_t inputSource) override;

    /**
     * @brief Destroy event state.
     * @param
     * @return
     */
    void DestroyEvents() override;

    /**
     * @brief Inject simulated gestures.
     * @param gesturePath the vector of gesture path
     * @param service the corresponding AccessiblityAbility
     * @param sequence the sequence of gesture
     * @return
     */
    void InjectEvents(const std::vector<GesturePathDefine> &gesturePath,
        const sptr<IAccessibleAbilityClient> &service, int sequence );

    /**
     * @brief Send pointer event to next stream node.
     * @param event the touch event prepared to send
     * @return
     */
    void SendPointerEvent(MMI::PointerEvent &event);

    /**
     * @brief Parsing inject simulated gestures.
     * @param
     * @return
     */
    void InjectEventsInner();

    /**
     * @brief Get current gesture service.
     * @param
     * @return the corresponding AccessiblityAbility
     */
    sptr<IAccessibleAbilityClient> GetCurrentGestureService() {
        return currentGestureService_;
    }

    /**
     * @brief Get sequence of gesture.
     * @param
     * @return the sequence of gesture
     */
    int GetSequence() {
        return sequence_;
    }


    static constexpr uint32_t SEND_TOUCH_EVENT_MSG = 1;
    static constexpr uint32_t INJECT_EVENT_MSG = 2;

private:

    /**
     * @brief Cancel the gesture.
     * @param
     * @return
     */
    void CancelGesture();

    /**
     * @brief Cancel the injected events.
     * @param
     * @return
     */
    void CancelInjectedEvents();

    /**
     * @brief Get taps events.
     * @param
     * @return
     */
    void GetTapsEvents(long startTime);

    /**
     * @brief Get move events.
     * @param
     * @return
     */
    void GetMovesEvents(long startTime);

    /**
     * @brief Get touchevents from gesturepath.
     * @param
     * @return
     */
    void GetTouchEventsFromGesturePath(long startTime);

    /**
     * @brief create touchevent.
     * @param action the action of event
     * @param point the endpoint of event
     * @return the created touchevent
     */
    std::shared_ptr<MMI::PointerEvent> obtainTouchEvent(int action, MMI::PointerEvent::PointerItem point);

    /**
     * @brief Get the number of milliseconds elapsed since the system was booted.
     * @param
     * @return the number of milliseconds elapsed since the system was booted
     */
    long getSystemTime();



    int sequence_ = -1;
    bool isGestureUnderway_ = false;
    bool isDestroyEvent_ = false;
    std::vector<GesturePathDefine> gesturePath_;
    sptr<IAccessibleAbilityClient> currentGestureService_ = nullptr;
    std::shared_ptr<TouchInjectHandler> handler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::vector<std::shared_ptr<MMI::PointerEvent>> injectedEvents_;
};
}  // namespace Accessibility
}  // namespace OHOS

#endif  // ACCESSIBILITY_TOUCHEVENT_INJECTOR_H