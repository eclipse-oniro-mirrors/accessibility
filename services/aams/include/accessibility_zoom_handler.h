/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_ZOOM_HANDLER_H
#define ACCESSIBILITY_ZOOM_HANDLER_H

#include <vector>
#include <mutex>
#include "accessibility_event_transmission.h"
#include "accessibility_zoom_proxy.h"
#include "accessibility_zoom_gesture.h"

namespace OHOS {
namespace Accessibility {
#define POINTER_COUNT_1 1

/**
 * @brief Zoom state machine.
 * When the magnifying function is turned on, it is in ReadyState.
 * After the triple event, it will enter the ZoomInState.
 * After another triple event, it will exit the magnifying state
 * and enter the ReadyState again.
 * In the amplification state, if the two fingers are pressed down,
 * it will enter the SlidingState, and any finger will be lifted
 * up to exit the SlidingState and enter the ZoomInState again.
 *
 *                         (ReadyState)
 *                           |       ^
 *                    Triple |       | (Triple|Action_cancel|screen_off)
 *                           V       |
 *                         (ZoomInState)
 *                           |       ^
 *          Two fingers down |       | Any finger up
 *                           V       |
 *                        (SlidingState)
 *
 */
enum ACCESSIBILITY_ZOOM_STATE {
    READY_STATE,
    ZOOMIN_STATE,
    SLIDING_STATE
};

class ZoomObserver {
public:
    ZoomObserver() {}
    virtual ~ZoomObserver() {}
    virtual void OnTransitionTo(const int state) {}
    virtual void OnBack(MMI::PointerEvent &event) {}
    virtual void OnZoomIn() {}
    virtual void OnZoomOut() {}
};

class ZoomState {
public:
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void OnPointerEvent(MMI::PointerEvent &event) {}
    std::vector<ZoomObserver> observer_ {};

public:
    void Register(ZoomObserver &observer)
    {
        observer_.push_back(observer);
    }
    void UnRegisterAll()
    {
        observer_.erase(observer_.begin(), observer_.end());
    }

private:
};

/**
 * @brief The event processing.
 *
 * @since 1.0
 * @version 1.0
 */
class AccessibilityZoomHandler : public ZoomObserver, public EventTransmission {
public:
    /**
     * @brief A constructor used to create a zoom handler.
     *
     * @param proxy  AccessibilityZoomProxy reference.
     * @param displayId  The Logical display id.
     * @since 1.0
     * @version 1.0
     */
    explicit AccessibilityZoomHandler(int displayId);

    /**
     * @brief A destructor used to delete the zoom handler.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~AccessibilityZoomHandler();

    /**
     * @brief Callback function when handle a motion event.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnPointerEvent(MMI::PointerEvent &event) override;

private:

    class ReadyState: public ZoomState {
    public:
        virtual void Enter() override;
        virtual void Exit() override;
        virtual void OnPointerEvent(MMI::PointerEvent &event) override;
    private:
        AccessibilityZoomGesture gesture_ {};
    };

    /**
     * Triple down.
     */
    class ZoomInState: public ZoomState {
    public:
        virtual void Enter() override;
        virtual void Exit() override;
        virtual void OnPointerEvent(MMI::PointerEvent &event) override;
    private:
        AccessibilityZoomGesture gesture_ {};
    };

    /**
     * the two fingers are pressed down.
     */
    class SlidingState: public ZoomState {
    public:
        virtual void Enter() override;
        virtual void Exit() override;
        virtual void OnPointerEvent(MMI::PointerEvent &event) override;
        virtual bool OnScroll();
        virtual bool OnScale();
    };

    ZoomState currentState_ {};
    ReadyState readyState_ {};
    ZoomInState zoomInState_ {};
    SlidingState slidingState_ {};
    std::recursive_mutex stateMutex_ {};
    int displayId_ = 0;
    void Initialize();
    void OnBack(MMI::PointerEvent &event) override;
    void OnTransitionTo(const int state) override;
    void OnZoomIn() override;
    void OnZoomOut() override;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_ZOOM_HANDLER_H
