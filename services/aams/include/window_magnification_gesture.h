/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef MAGNIFICATION_GESTURE_H
#define MAGNIFICATION_GESTURE_H

#include "accessibility_event_transmission.h"
#include "window_magnification_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
class WindowMagnificationGesture;

#define BIND(func) [this](MMI::PointerEvent& event) { (func(event)); }

enum class MagnificationGestureState : uint32_t {
    PASSING_THROUGH,
    INVALID,
    READY_STATE,
    READY_STATE_ONE_FINGER_DOWN,
    READY_STATE_ONE_FINGER_TAP,
    ZOOMIN_STATE,
    ZOOMIN_STATE_ONE_FINGER_DOWN,
    ZOOMIN_STATE_ONE_FINGER_TAP,
    ZOOMIN_STATE_TWO_FINGERS_DOWN,
    SLIDING_STATE,
    TWO_FINGERS_SLIDING_STATE,
    MENU_SLIDING_STATE,
};

enum class MagnificationState : uint32_t {
    MAGNIFICATION_READY_STATE,
    MAGNIFICATION_ZOOMIN_STATE,
    MAGNIFICATION_SLIDING_STATE
};

enum class PointerCountSize : int32_t {
    POINTER_SIZE_1 = 1,
    POINTER_SIZE_2 = 2
};

enum class MagnificationGestureMsg : uint32_t {
    SINGLE_TAP_FAIL_MSG,
    TRIPLE_TAP_FAIL_MSG,
    LONG_PRESS_MSG,
    WAIT_ANOTHER_FINGER_DOWN_MSG,
};

// ms
enum class MagnificationTimeout : uint32_t {
    TAP_TIMEOUT = 100,
    DOUBLE_TAP_TIMEOUT = 250,
    LONG_PRESS_TIMEOUT = 200,
};

class MagnificationGestureEventHandler : public AppExecFwk::EventHandler {
public:
    MagnificationGestureEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
        WindowMagnificationGesture &server);
    virtual ~MagnificationGestureEventHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

private:
    WindowMagnificationGesture &server_;
};

class WindowMagnificationGesture : public EventTransmission {
public:
    WindowMagnificationGesture();
    explicit WindowMagnificationGesture(std::shared_ptr<WindowMagnificationManager> windowMagnificationManager);
    ~WindowMagnificationGesture() = default;

    bool OnPointerEvent(MMI::PointerEvent &event) override;
    void DestroyEvents() override;

    void CancelPostEvent(MagnificationGestureMsg msg);
    void SendCacheEventsToNext();
    void ShieldZoomGesture(bool state);
    void StartMagnificationInteract();
    void DisableGesture();

    /* Set current magnification gesture state */
    inline void SetGestureState(MagnificationGestureState state, int32_t action)
    {
        HILOG_INFO("gestureState changed: [%{public}d] -> [%{public}d], current action: %{public}d.",
            gestureState_, state, action);
        gestureState_ = state;
    }

    /* Get current magnification gesture state */
    inline MagnificationGestureState GetGestureState()
    {
        return gestureState_;
    }

    inline void AddTapCount()
    {
        tapCount_++;
        HILOG_INFO("add tap count, current count = [%{public}d].", tapCount_);
    }

    inline void ResetTapCount()
    {
        tapCount_ = 0;
        HILOG_INFO("reset tap count = 0");
    }

    inline uint32_t GetTapCount()
    {
        HILOG_INFO("get tap count = [%{public}d].", tapCount_);
        return tapCount_;
    }

    inline bool GetTapOnMenu()
    {
        return isTapOnMenu_;
    }
private:
    bool needBypassPointerEvent(MMI::PointerEvent &event);
    
    void OnPointerEventExcute(MMI::PointerEvent &event);

    void HandlePassingThroughState(MMI::PointerEvent &event);

    void HandleReadyStateDown(MMI::PointerEvent &event);
    void HandleReadyStateUp(MMI::PointerEvent &event);
    void HandleReadyStateMove(MMI::PointerEvent &event);
    void HandleReadyStateOneFingerDownStateDown(MMI::PointerEvent &event);
    void HandleReadyStateOneFingerDownStateUp(MMI::PointerEvent &event);
    void HandleReadyStateOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleReadyStateOneFingerTapDown(MMI::PointerEvent &event);

    void HandleZoomInStateDown(MMI::PointerEvent &event);
    void HandleZoomInStateUp(MMI::PointerEvent &event);
    void HandleZoomInStateMove(MMI::PointerEvent &event);
    void HandleZoomInStateOneFingerDownStateDown(MMI::PointerEvent &event);
    void HandleZoomInStateOneFingerDownStateUp(MMI::PointerEvent &event);
    void HandleZoomInStateOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleZoomInStateOneFingerTapDown(MMI::PointerEvent &event);

    void HandleTwoFingersDownStateUp(MMI::PointerEvent &event);
    void HandleTwoFingersDownStateMove(MMI::PointerEvent &event);

    void HandleSlidingStateDown(MMI::PointerEvent &event);
    void HandleSlidingStateUp(MMI::PointerEvent &event);
    void HandleSlidingStateMove(MMI::PointerEvent &event);

    void HandleMenuSlidingStateUp(MMI::PointerEvent &event);
    void HandleMenuSlidingStateMove(MMI::PointerEvent &event);

    void InitGestureFuncMap();

    bool IsMove(MMI::PointerEvent &startEvent, MMI::PointerEvent &endEvent, float slop);
    void Clear();
    void OnTripleTap(int32_t centerX, int32_t centerY);
    void SendEventToMultimodal(MMI::PointerEvent event, bool needTransfer, bool needResetTime);
    bool IsTapOnInputMethod(MMI::PointerEvent &event);
    inline float CalculateMoveThreshold(int dpi)
    {
        return dpi * COMPLEX_UNIT_MM_CONVERSION * MM_PER_CM;
    }

    std::shared_ptr<MagnificationGestureEventHandler> handler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager_ = nullptr;
    MagnificationState magnificationState_ = MagnificationState::MAGNIFICATION_READY_STATE;
    MagnificationGestureState gestureState_ = MagnificationGestureState::READY_STATE;

    using HandleEventFunc = std::function<void(MMI::PointerEvent &)>;
    std::map<MagnificationGestureState, std::map<int32_t, HandleEventFunc>> handleEventFuncMap_ {};
    std::list<MMI::PointerEvent> receivedPointerEvents_ {};
    std::shared_ptr<MMI::PointerEvent> lastDownEvent_ = nullptr;
    std::shared_ptr<MMI::PointerEvent> lastSlidingEvent_ = nullptr;
    bool isTapOnHotArea_ = false;
    bool isSingleTapOnWindow_ = false;
    bool isDoubleFingersValid_ = false;
    bool startScale_ = false;
    bool isTapOnMenu_ = false;

    float moveThreshold_ = 0.0f;
    float tapDistance_ = 0.0f;
    float multiTapDistance_ = 0.0f;

    uint32_t tapCount_ = 0;
    float distance_ = 0.0f;
    bool shieldZoomGestureFlag_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_GESTURE_H