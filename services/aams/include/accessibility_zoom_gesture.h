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

#ifndef ACCESSIBILITY_ZOOM_GESTURE_H
#define ACCESSIBILITY_ZOOM_GESTURE_H

#include "accessibility_event_transmission.h"
#include "event_handler.h"
#include "pointer_event.h"
#include "dm_common.h"
#include "full_screen_magnification_manager.h"
#include "window_magnification_manager.h"
#include "magnification_menu_manager.h"
#include "magnification_def.h"
#include <unordered_map>
#include <functional>

namespace OHOS {
namespace Accessibility {

#define BIND(func) [this](MMI::PointerEvent& event) { (func(event)); }

enum ACCESSIBILITY_ZOOM_GESTURE_MSG : uint32_t {
    MULTI_TAP_MSG,
    LONG_PRESS_MSG,
    WAIT_ANOTHER_FINGER_DOWN_MSG,
    HOLDING_MSG,
    TWO_FINGER_SLIDING_MSG,
    HOT_AREA_SLIDING_MSG,
    MENU_SLIDING_MSG,
    TOUCH_EXPLORATION_ZOOM_DELAY_MSG,
};

struct ZOOM_FOCUS_COORDINATE {
    float centerX;
    float centerY;
};

class AccessibilityZoomGesture : public EventTransmission {
public:
    AccessibilityZoomGesture(std::shared_ptr<FullScreenMagnificationManager> fullScreenManager,
        std::shared_ptr<WindowMagnificationManager> windowMagnificationManager,
        std::shared_ptr<MagnificationMenuManager> menuManager);
    ~AccessibilityZoomGesture() = default;

    virtual bool OnPointerEvent(MMI::PointerEvent &event) override;
    void DestroyEvents() override;
    // flag = true shield zoom gesture | flag = false restore zoom gesture
    void ShieldZoomGesture(bool state);
    void GetWindowParam(bool needRefresh = false);
    void StartMagnificationInteract();
    void SetGestureMode(int32_t mode);
    void SetMagnificationMode(uint32_t mode);
    void DisableGesture();
    inline int32_t GetZoomState()
    {
        return zoomState_;
    }

private:
    class ZoomGestureEventHandler : public AppExecFwk::EventHandler {
    public:
        ZoomGestureEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
            AccessibilityZoomGesture &zoomGesture);
        virtual ~ZoomGestureEventHandler() = default;

        virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    private:
        AccessibilityZoomGesture &zoomGesture_;
    };

    void TransferState(int32_t state);
    void CacheEvents(MMI::PointerEvent &event);
    void SendCacheEventsToNext();
    void SendEventToMultimodal(MMI::PointerEvent event);
    void ClearCacheEventsAndMsg();

    void InitGestureFuncMap();
    void InitSingleFingerTripleTapFuncMap();
    void InitThreeFingerDoubleTapFuncMap();
    void OnPointerEventExecute(MMI::PointerEvent &event);

    void RecognizeInZoomStateDownEvent(MMI::PointerEvent &event);
    void RecognizeInZoomStateMoveEvent(MMI::PointerEvent &event);
    void RecognizeScroll(ZOOM_FOCUS_COORDINATE &coordinate);
    bool RecognizeScale(MMI::PointerEvent &event);
    void CalcFocusCoordinate(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE &coordinate);
    float CalcScaleSpan(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE coordinate);
    bool IsTapOnInputMethod(MMI::PointerEvent &event);
    bool IsDownValid(std::shared_ptr<MMI::PointerEvent> curEvent, std::shared_ptr<MMI::PointerEvent> lastEvent);
    bool IsMoveValid(std::shared_ptr<MMI::PointerEvent> curEvent, std::shared_ptr<MMI::PointerEvent> lastEvent);
    bool IsLongPress();
    bool IsKnuckles(MMI::PointerEvent &event);
    void OnTripleTap(MMI::PointerEvent &event);
    bool IsThreeFingerMultiTap(MMI::PointerEvent &event);
    float CalcSeparationDistance(MMI::PointerEvent &event);
    float CalcSeparationDistance(MMI::PointerEvent &firstEvent, MMI::PointerEvent &secondEvent);
    void OnZoom(int32_t centerX, int32_t centerY, bool showMenu);
    void OffZoom();
    void OnScroll(float offsetX, float offsetY);
    void PersistScale();
    void Clear();

    void DoNothingHandler(MMI::PointerEvent &event);
    void HandleSTReadyInitStateDown(MMI::PointerEvent &event);
    void HandleSTReadyOneFingerDownStateUp(MMI::PointerEvent &event);
    void HandleSTReadyOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleSTReadyOneFingerTapStateDown(MMI::PointerEvent &event);

    void HandleSTZoomInitStateDown(MMI::PointerEvent &event);
    void HandleSTZoomOneFingerDownStateDown(MMI::PointerEvent &event);
    void HandleSTZoomOneFingerDownStateUp(MMI::PointerEvent &event);
    void HandleSTZoomOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleSTZoomTwoFingerDownStateMove(MMI::PointerEvent &event);
    void HandleSTZoomOneFingerTapStateDown(MMI::PointerEvent &event);
    void HandleZoomSlidingStateDown(MMI::PointerEvent &event);
    void HandleZoomSlidingStateMove(MMI::PointerEvent &event);
    void HandleZoomSlidingStateUp(MMI::PointerEvent &event);

    void HandleTDReadyInitStateDown(MMI::PointerEvent &event);
    void HandleTDReadyOneFingerDownStateDown(MMI::PointerEvent &event);
    void HandleTDReadyOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleTDReadyTwoFingersDownStateDown(MMI::PointerEvent &event);
    void HandleTDReadyTwoFingersDownStateMove(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersDownStateUp(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersDownStateMove(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersTapStateDown(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersTapStateMove(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersContinueDownStateDown(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersContinueDownStateUp(MMI::PointerEvent &event);
    void HandleTDReadyThreeFingersContinueDownStateMove(MMI::PointerEvent &event);
    void HandleHoldStateDown(MMI::PointerEvent &event);
    void HandleHoldStateUp(MMI::PointerEvent &event);
    void HandleHoldStateMove(MMI::PointerEvent &event);
    void HandleMenuSlidingStateMove(MMI::PointerEvent &event);
    void HandleMenuSlidingStateUp(MMI::PointerEvent &event);
    void HandleHotAreaSlidingStateMove(MMI::PointerEvent &event);
    void HandleHotAreaSlidingStateUp(MMI::PointerEvent &event);

    void HandleTDZoomInitStateDown(MMI::PointerEvent &event);
    void HandleTDZoomOneFingerDownStateDown(MMI::PointerEvent &event);
    void HandleTDZoomOneFingerDownStateMove(MMI::PointerEvent &event);
    void HandleTDZoomOneFingerDownStateUp(MMI::PointerEvent &event);
    void HandleTDZoomTwoFingersDownStateDown(MMI::PointerEvent &event);
    void HandleTDZoomTwoFingersDownStateMove(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersDownStateUp(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersDownStateMove(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersTapStateDown(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersTapStateMove(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersContinueDownStateUp(MMI::PointerEvent &event);
    void HandleTDZoomThreeFingersContinueDownStateMove(MMI::PointerEvent &event);

    float tapDistance_ = 0.0f;
    float multiTapDistance_ = 0.0f;
    float baseDistance_ = 0.0f;
    float lastDistance_ = 0.0f;
    float scale_ = DEFAULT_SCALE;
    uint64_t screenId_ = -1;
    uint32_t screenWidth_ = 0;
    uint32_t screenHeight_ = 0;
    uint32_t gestureType_ = 0;
    int32_t zoomState_ = READY;
    int32_t zoomGestureState_ = INIT;
    int32_t gestureMode_ = THREE_FINGER_DOUBLE_TAP_MODE;
    uint32_t magnificationMode_ = FULL_SCREEN_MAGNIFICATION;
    bool isTapOnMenu_ = false;
    bool isTapOnWindowHotArea_ = false;
    bool isTapOnWindow_ = false;
    bool hasScaled_ = false;
    bool hasNotifyConflict_ = false;
    OHOS::Rosen::DisplayOrientation orientation_ =
        OHOS::Rosen::DisplayOrientation::UNKNOWN;
    ZOOM_FOCUS_COORDINATE lastCenter = {0.0f, 0.0f};
    std::shared_ptr<MMI::PointerEvent> lastDownEvent_ = nullptr;
    std::shared_ptr<MMI::PointerEvent> lastTripleTapEvents_[3] = {nullptr, nullptr, nullptr};
    std::shared_ptr<ZoomGestureEventHandler> zoomGestureEventHandler_ = nullptr;
    std::vector<std::shared_ptr<MMI::PointerEvent>> cacheEvents_;
    std::shared_ptr<FullScreenMagnificationManager> fullScreenManager_ = nullptr;
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager_ = nullptr;
    std::shared_ptr<MagnificationMenuManager> menuManager_ = nullptr;

    std::atomic<bool> shieldZoomGestureFlag_ = false;
    int32_t singleFingerTapCount_ = 0;

    static constexpr int32_t POINTER_ID_0 = 0;
    static constexpr int32_t POINTER_ID_1 = 1;
    static constexpr int32_t POINTER_ID_2 = 2;

    static constexpr int32_t POINTER_COUNT_1 = 1;
    static constexpr int32_t POINTER_COUNT_2 = 2;
    static constexpr int32_t POINTER_COUNT_3 = 3;

    static constexpr int32_t READY = 0;
    static constexpr int32_t ZOOM = 1;

    static constexpr int32_t PASSING_THROUGH = -2;
    static constexpr int32_t INVALID = -1;
    static constexpr int32_t INIT = 0;
    static constexpr int32_t ONE_FINGER_DOWN = 1;
    static constexpr int32_t ONE_FINGER_TAP = 2;
    static constexpr int32_t ONE_FINGER_TAP_THEN_DOWN = 3;
    static constexpr int32_t HOLD = 4;
    static constexpr int32_t TWO_FINGER_DOWN = 5;
    static constexpr int32_t THREE_FINGER_DOWN = 6;
    static constexpr int32_t THREE_FINGER_TAP = 7;
    static constexpr int32_t THREE_FINGER_TAP_THEN_DOWN = 8;
    static constexpr int32_t SLIDING = 9;
    static constexpr int32_t HOT_AREA_SLIDING = 10;
    static constexpr int32_t MENU_SLIDING = 11;
    static constexpr int32_t THREE_FINGER_DOUBLE_TAP = 12;

    static constexpr int32_t GESTURE_MODE_COUNT = 2;
    static constexpr int32_t MAGNIFICATION_STATE_COUNT = 2;
    static constexpr int32_t GESTURE_STATE_COUNT = 13;
    static constexpr int32_t POINTER_ACTION_MAX = 5;

    using GestureEventFunc = std::function<void(MMI::PointerEvent&)>;
    GestureEventFunc handlerFuncMap_
        [GESTURE_MODE_COUNT][MAGNIFICATION_STATE_COUNT][GESTURE_STATE_COUNT][POINTER_ACTION_MAX] = {};
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ZOOM_GESTURE_H