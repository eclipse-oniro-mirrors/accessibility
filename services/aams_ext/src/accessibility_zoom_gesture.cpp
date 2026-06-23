/*
 * Copyright (C) 2022-2026 Huawei Device Co., Ltd.
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

#include "accessibility_zoom_gesture.h"
#include "hilog_wrapper.h"
#include "window_accessibility_controller.h"
#include "accessibility_window_manager.h"
#include "ext_utils.h"
#include "accessibility_input_interceptor.h"
#include "extend_service_manager.h"
#include "accessibility_def.h"
#include "magnification_window.h"
#include <set>
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "accessibility_extend_power_manager.h"
#endif
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr float TAP_MIN_DISTANCE = 8.0f;
    constexpr int32_t MULTI_FINGER_TAP_INTERVAL_TIMER = 100; // ms
    constexpr int32_t MULTI_TAP_TIMER = 250; // ms
    constexpr int32_t LONG_PRESS_TIMER = 300; // ms
    constexpr int32_t TOUCH_EXPLORATION_ZOOM_DELAY_TIMER = 250; // ms
    constexpr float DOUBLE_TAP_SLOP = 100.0f;
    constexpr uint32_t DOUBLE_TAP_COUNT = 2;
    constexpr uint32_t TRIPLE_TAP_COUNT = 3;
    constexpr float MIN_SCROLL_SPAN = 2.0f;
    constexpr float MIN_SCALE_SPAN = 2.0f;
    constexpr float MIN_SCALE = 0.1f;
    const std::set<int32_t> LEVITATE_ACTIONS = {
        MMI::PointerEvent::POINTER_ACTION_PROXIMITY_IN,
        MMI::PointerEvent::POINTER_ACTION_PROXIMITY_OUT,
        MMI::PointerEvent::POINTER_ACTION_LEVITATE_MOVE,
        MMI::PointerEvent::POINTER_ACTION_LEVITATE_IN_WINDOW,
        MMI::PointerEvent::POINTER_ACTION_LEVITATE_OUT_WINDOW,
        MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN,
        MMI::PointerEvent::POINTER_ACTION_BUTTON_UP
    };
} // namespace

AccessibilityZoomGesture::AccessibilityZoomGesture(
    std::shared_ptr<FullScreenMagnificationManager> fullScreenManager,
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager,
    std::shared_ptr<MagnificationMenuManager> menuManager)
    : fullScreenManager_(fullScreenManager),
    windowMagnificationManager_(windowMagnificationManager),
    menuManager_(menuManager)
{
    HILOG_DEBUG();
    zoomGestureEventHandler_ = std::make_shared<ZoomGestureEventHandler>(
        AccessibilityInputInterceptor::GetInstance()->GetInputManagerRunner(), *this);
    magnificationMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationModeCallback();
    gestureMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationTriggerMethodCallback();
    scale_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationScaleCallback();
    hasNotifyConflict_ = Singleton<ExtendServiceManager>::GetInstance().getNotifyZoomGestureConflictCallback();

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }

    float densityPixels = display->GetVirtualPixelRatio();
    tapDistance_ = TAP_MIN_DISTANCE;
    multiTapDistance_ = densityPixels * DOUBLE_TAP_SLOP + 0.5f;
#else
    HILOG_DEBUG("not support display manager");
    multiTapDistance_ = 1 * DOUBLE_TAP_SLOP + 0.5f;
#endif

    InitGestureFuncMap();
}


bool AccessibilityZoomGesture::IsTapOnInputMethod(MMI::PointerEvent &event)
{
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_1) {
        HILOG_DEBUG("not single finger.");
        return false;
    }

    std::vector<AccessibilityWindowInfo> windowInfos =
        Singleton<ExtendServiceManager>::GetInstance().getAccessibilityWindowsCallback(event.GetTargetDisplayId());
    for (auto &window : windowInfos) {
        if (window.GetWindowType() != INPUT_METHOD_WINDOW_TYPE) {
            continue;
        }
        for (auto &outRect : window.GetTouchHotAreas()) {
            int32_t leftTopX = outRect.GetLeftTopXScreenPostion();
            int32_t leftTopY = outRect.GetLeftTopYScreenPostion();
            int32_t rightBottomX = outRect.GetRightBottomXScreenPostion();
            int32_t rightBottomY = outRect.GetRightBottomYScreenPostion();
            MMI::PointerEvent::PointerItem item;
            event.GetPointerItem(event.GetPointerId(), item);
            int32_t itemX = item.GetDisplayX();
            int32_t itemY = item.GetDisplayY();
            if ((itemX >= leftTopX) && (itemX <= rightBottomX) &&
                (itemY >= leftTopY) && (itemY <= rightBottomY)) {
                HILOG_INFO("tap on input method window.");
                return true;
            }
        }
    }
    HILOG_DEBUG("have no input method window.");
    return false;
}

void AccessibilityZoomGesture::DisableGesture()
{
    OffZoom();
}
void AccessibilityZoomGesture::StartMagnificationInteract()
{
    zoomState_ = ZOOM;
}

bool AccessibilityZoomGesture::IsDownValid(std::shared_ptr<MMI::PointerEvent> curEvent,
    std::shared_ptr<MMI::PointerEvent> lastEvent)
{
    return CalcSeparationDistance(*curEvent, *lastEvent) <= multiTapDistance_;
}

bool AccessibilityZoomGesture::IsMoveValid(std::shared_ptr<MMI::PointerEvent> curEvent,
    std::shared_ptr<MMI::PointerEvent> lastEvent)
{
    return CalcSeparationDistance(*curEvent, *lastEvent) <= tapDistance_;
}

bool AccessibilityZoomGesture::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("zoomGestureState_ is %{public}d.", zoomGestureState_);

    if (shieldZoomGestureFlag_) {
        EventTransmission::OnPointerEvent(event);
        return true;
    }

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
        HILOG_INFO("PointerAction: %{public}d.", event.GetPointerAction());
    }

    int32_t sourceType = event.GetSourceType();
    if (zoomState_ == READY && sourceType != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    if (LEVITATE_ACTIONS.count(event.GetPointerAction()) > 0) {
        SendEventToMultimodal(event);
        return true;
    }

    OnPointerEventExecute(event);
    return true;
}

void AccessibilityZoomGesture::OnPointerEventExecute(MMI::PointerEvent &event)
{
    int32_t action = event.GetPointerAction();
    if (action < 1 || action >= POINTER_ACTION_MAX) {
        HILOG_ERROR("invalid action=%{public}d", action);
        EventTransmission::OnPointerEvent(event);
        return;
    }
    if (zoomGestureState_ == INVALID) {
        if (action == MMI::PointerEvent::POINTER_ACTION_UP && event.GetPointerIds().size() == 1) {
            TransferState(INIT);
        }
        return;
    }
    if (zoomGestureState_ == PASSING_THROUGH) {
        if (action == MMI::PointerEvent::POINTER_ACTION_UP && event.GetPointerIds().size() == 1) {
            TransferState(INIT);
        }
        SendEventToMultimodal(event);
        return;
    }
    auto func = handlerFuncMap_[gestureMode_][zoomState_][zoomGestureState_][action];
    if (func != nullptr) {
        func(event);
    } else {
        CacheEvents(event);
        SendCacheEventsToNext();
        HILOG_ERROR("no handler for gestureMode=%{public}d, magState=%{public}d, state=%{public}d, action=%{public}d",
                    gestureMode_, zoomState_, zoomGestureState_, action);
        if (action == MMI::PointerEvent::POINTER_ACTION_UP && event.GetPointerIds().size() == 1) {
            TransferState(INIT);
            return;
        }
        TransferState(PASSING_THROUGH);
    }
}

void AccessibilityZoomGesture::InitGestureFuncMap()
{
    HILOG_DEBUG();

    InitSingleFingerTripleTapFuncMap();
    InitThreeFingerDoubleTapFuncMap();
}

void AccessibilityZoomGesture::InitSingleFingerTripleTapFuncMap()
{
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][READY][INIT][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleSTReadyInitStateDown);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][READY][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleSTReadyOneFingerDownStateUp);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][READY][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleSTReadyOneFingerDownStateMove);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][READY][ONE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleSTReadyOneFingerTapStateDown);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleHoldStateDown);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleHoldStateUp);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleHoldStateMove);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][INIT][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleSTZoomInitStateDown);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleSTZoomOneFingerDownStateDown);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleSTZoomOneFingerDownStateUp);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleSTZoomOneFingerDownStateMove);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][TWO_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleSTZoomTwoFingerDownStateMove);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][ONE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleSTZoomOneFingerTapStateDown);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleZoomSlidingStateDown);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleZoomSlidingStateMove);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleZoomSlidingStateUp);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(DoNothingHandler);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleHotAreaSlidingStateMove);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleHotAreaSlidingStateUp);

    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(DoNothingHandler);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleMenuSlidingStateMove);
    handlerFuncMap_[SINGLE_FINGER_TRIPLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleMenuSlidingStateUp);
}

void AccessibilityZoomGesture::InitThreeFingerDoubleTapFuncMap()
{
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][INIT][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDReadyInitStateDown);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDReadyOneFingerDownStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDReadyOneFingerDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][TWO_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDReadyTwoFingersDownStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][TWO_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDReadyTwoFingersDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleTDReadyThreeFingersDownStateUp);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDReadyThreeFingersDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDReadyThreeFingersTapStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDReadyThreeFingersTapStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_TAP_THEN_DOWN]
        [MMI::PointerEvent::POINTER_ACTION_UP] = BIND(HandleTDReadyThreeFingersContinueDownStateUp);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][READY][THREE_FINGER_TAP_THEN_DOWN]
        [MMI::PointerEvent::POINTER_ACTION_MOVE] = BIND(HandleTDReadyThreeFingersContinueDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleHoldStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleHoldStateUp);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOLD][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleHoldStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][INIT][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDZoomInitStateDown);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDZoomOneFingerDownStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDZoomOneFingerDownStateMove);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][ONE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleTDZoomOneFingerDownStateUp);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][TWO_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDZoomTwoFingersDownStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][TWO_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDZoomTwoFingersDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleTDZoomThreeFingersDownStateUp);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_DOWN][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDZoomThreeFingersDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleTDZoomThreeFingersTapStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_TAP][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleTDZoomThreeFingersTapStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_TAP_THEN_DOWN]
        [MMI::PointerEvent::POINTER_ACTION_UP] = BIND(HandleTDZoomThreeFingersContinueDownStateUp);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][THREE_FINGER_TAP_THEN_DOWN]
        [MMI::PointerEvent::POINTER_ACTION_MOVE] = BIND(HandleTDZoomThreeFingersContinueDownStateMove);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(HandleZoomSlidingStateDown);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleZoomSlidingStateMove);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleZoomSlidingStateUp);
    
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(DoNothingHandler);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleHotAreaSlidingStateMove);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][HOT_AREA_SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleHotAreaSlidingStateUp);

    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_DOWN] =
        BIND(DoNothingHandler);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_MOVE] =
        BIND(HandleMenuSlidingStateMove);
    handlerFuncMap_[THREE_FINGER_DOUBLE_TAP_MODE][ZOOM][MENU_SLIDING][MMI::PointerEvent::POINTER_ACTION_UP] =
        BIND(HandleMenuSlidingStateUp);
}

void AccessibilityZoomGesture::SetMagnificationMode(uint32_t mode)
{
    magnificationMode_ = mode;
            }


void AccessibilityZoomGesture::SetGestureMode(int32_t mode)
{
    HILOG_INFO("Set gesture mode: %{public}d", mode);
    gestureMode_ = mode;
}

void AccessibilityZoomGesture::TransferState(int32_t state)
{
    HILOG_INFO("old state= %{public}d, new state= %{public}d", zoomGestureState_, state);
    zoomGestureState_ = state;
}

void AccessibilityZoomGesture::CacheEvents(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    cacheEvents_.emplace_back(pointerEvent);
}

void AccessibilityZoomGesture::SendCacheEventsToNext()
{
    HILOG_DEBUG();

    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    bool isMagnificationWindowShow = fullScreenManager_->IsMagnificationWindowShow();
    for (auto &pointerEvent : cacheEvents_) {
        SendEventToMultimodal(*pointerEvent);
    }
    ClearCacheEventsAndMsg();
}

void AccessibilityZoomGesture::SendEventToMultimodal(MMI::PointerEvent event)
{
    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    
    if (zoomState_ == ZOOM) {
        if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION && fullScreenManager_->IsMagnificationWindowShow()) {
            MMI::PointerEvent::PointerItem pointer {};
        event.GetPointerItem(event.GetPointerId(), pointer);

            PointerPos coordinates = fullScreenManager_->ConvertCoordinates(pointer.GetDisplayX(),
                pointer.GetDisplayY());
            if (gestureType_ != INVALID_GESTURE_TYPE) {
                coordinates = fullScreenManager_->ConvertGesture(gestureType_, coordinates);
            }
            pointer.SetDisplayX(coordinates.posX);
            pointer.SetDisplayY(coordinates.posY);
            pointer.SetTargetWindowId(-1);
        event.RemovePointerItem(event.GetPointerId());
        event.AddPointerItem(pointer);
        event.SetZOrder(10000); // magnification zlevel is 10000
        }
        if (magnificationMode_ != FULL_SCREEN_MAGNIFICATION &&
            windowMagnificationManager_->IsMagnificationWindowShow()) {
            MMI::PointerEvent::PointerItem pointer {};
            event.GetPointerItem(event.GetPointerId(), pointer);
            if (windowMagnificationManager_->IsTapOnMagnificationWindow(pointer.GetDisplayX(), pointer.GetDisplayY()) &&
                !windowMagnificationManager_->IsTapOnHotArea(pointer.GetDisplayX(), pointer.GetDisplayY())) {
                HILOG_ERROR("need convert pos");
                PointerPos coordinates = windowMagnificationManager_->ConvertCoordinates(pointer.GetDisplayX(),
                    pointer.GetDisplayY());
                pointer.SetDisplayX(coordinates.posX);
                pointer.SetDisplayY(coordinates.posY);
                pointer.SetTargetWindowId(-1);
                event.RemovePointerItem(event.GetPointerId());
                event.AddPointerItem(pointer);
                event.SetZOrder(10000); // magnification zlevel is 10000
        }
    }
}
    event.SetActionTime(ExtUtils::GetSystemTime() * US_TO_MS);
    EventTransmission::OnPointerEvent(event);
}

void AccessibilityZoomGesture::ClearCacheEventsAndMsg()
{
    HILOG_DEBUG();

    cacheEvents_.clear();
    lastDownEvent_ = nullptr;
    lastTripleTapEvents_[POINTER_ID_0] = nullptr;
    lastTripleTapEvents_[POINTER_ID_1] = nullptr;
    lastTripleTapEvents_[POINTER_ID_2] = nullptr;
    gestureType_ = INVALID_GESTURE_TYPE;
    bool isTapOnMenu_ = false;
    bool isTapOnWindowHotArea_ = false;
    bool isTapOnWindow_ = false;
    lastCenter = {0.0f, 0.0f};

    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    zoomGestureEventHandler_->RemoveEvent(LONG_PRESS_MSG);
    zoomGestureEventHandler_->RemoveEvent(HOLDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(TWO_FINGER_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(HOT_AREA_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(MENU_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(TOUCH_EXPLORATION_ZOOM_DELAY_MSG);
}


void AccessibilityZoomGesture::RecognizeScroll(ZOOM_FOCUS_COORDINATE &coordinate, bool reverse)
{
    HILOG_DEBUG();
    if (abs(lastCenter.centerX) < EPS && abs(lastCenter.centerY) < EPS) {
        lastCenter.centerX = coordinate.centerX;
        lastCenter.centerY = coordinate.centerY;
        return;
    }

    float offsetX = coordinate.centerX - lastCenter.centerX;
    float offsetY = coordinate.centerY - lastCenter.centerY;
    if ((abs(offsetX) > MIN_SCROLL_SPAN) || (abs(offsetY) > MIN_SCROLL_SPAN)) {
        lastCenter.centerX = coordinate.centerX;
        lastCenter.centerY = coordinate.centerY;
        if (reverse) {
            OnScroll(0.0f - offsetX, 0.0f - offsetY);
        } else {
            OnScroll(offsetX, offsetY);
        }
    }
}

bool AccessibilityZoomGesture::RecognizeScale(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    float curDistance = CalcSeparationDistance(event);
    if (abs(lastDistance_) < EPS) {
        HILOG_WARN("lastDistance_ is zero");
        lastDistance_ = curDistance;
        return false;
    }
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        if (abs(lastDistance_ - curDistance) >=  MIN_SCALE_SPAN) {
            if (fullScreenManager_ == nullptr) {
                HILOG_ERROR("fullScreenManager_ is nullptr.");
                    return false;
            }
            fullScreenManager_->SetScale(curDistance - lastDistance_);
            lastDistance_ = curDistance;
            return true;
        }
    } else {
        if (abs(baseDistance_ - curDistance) >  MIN_SCALE_DISTANCE) {
            hasScaled_ = true;
            if (windowMagnificationManager_ == nullptr) {
                HILOG_ERROR("fullScreenManager_ is nullptr.");
                return false;
            }
            windowMagnificationManager_->FixSourceCenter(true);
            windowMagnificationManager_->SetScale(curDistance - lastDistance_);
            lastDistance_ = curDistance;
            return true;
        } else {
            if (hasScaled_) {
                hasScaled_ = false;
                CalcFocusCoordinate(event, lastCenter);
            }
            windowMagnificationManager_->FixSourceCenter(false);
            return false;
        }
    }
    return false;
}

void AccessibilityZoomGesture::CalcFocusCoordinate(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE &coordinate)
{
    HILOG_DEBUG();

    float sumX = 0.0f;
    float sumY = 0.0f;
    int32_t upPointerId = -1;
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t count = pointerIdList.size();
    if (!count) {
        HILOG_DEBUG("The size of PointerIds is 0");
        return;
    }

    if (action == MMI::PointerEvent::POINTER_ACTION_UP) {
        upPointerId = event.GetPointerId();
        HILOG_DEBUG("The pointer id of up is %{public}d", upPointerId);
        count--;
            }

    if (!count) {
        HILOG_DEBUG("The size of PointerIds(down) is invalid");
        return;
                }

    for (int32_t pointerId : pointerIdList) {
        if (pointerId == upPointerId) {
            continue;
            }
        MMI::PointerEvent::PointerItem item;
        event.GetPointerItem(pointerId, item);
        sumX += static_cast<float>(item.GetRawDisplayX());
        sumY += static_cast<float>(item.GetRawDisplayY());
    }

    coordinate.centerX = sumX / count;
    coordinate.centerY = sumY / count;
    HILOG_DEBUG("centerX:%{public}f, centerY:%{public}f", coordinate.centerX, coordinate.centerY);
}

float AccessibilityZoomGesture::CalcScaleSpan(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE coordinate)
{
    HILOG_DEBUG();

    float span = 0.0f;
    float sumSpanX = 0.0f;
    float sumSpanY = 0.0f;
    int32_t upPointerId = -1;
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t count = pointerIdList.size();
    if (!count) {
        HILOG_DEBUG("The size of PointerIds is 0");
        return span;
    }

    for (int32_t pointerId : pointerIdList) {
        MMI::PointerEvent::PointerItem item;
        event.GetPointerItem(pointerId, item);
        sumSpanX += static_cast<float>(abs(item.GetRawDisplayX() - coordinate.centerX));
        sumSpanY += static_cast<float>(abs(item.GetRawDisplayY() - coordinate.centerY));
    }

    float spanX = sumSpanX / count;
    float spanY = sumSpanY / count;
    span = hypot(spanX, spanY) / HALF;
    HILOG_DEBUG("The span is %{public}f", span);
    return span;
}

bool AccessibilityZoomGesture::IsKnuckles(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    for (int32_t pointerId : pointerIdList) {
        MMI::PointerEvent::PointerItem item;
        event.GetPointerItem(pointerId, item);
        int32_t toolType = item.GetToolType();
        if (toolType == MMI::PointerEvent::TOOL_TYPE_KNUCKLE) {
            HILOG_INFO("is knuckle event.");
            return true;
            }
    }
    return false;
}

float AccessibilityZoomGesture::CalcSeparationDistance(MMI::PointerEvent &event)
{
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t count = pointerIdList.size();
    if (count != POINTER_COUNT_2) {
        HILOG_ERROR("only two fingers can cal distance");
        return 0.0f;
    }
    MMI::PointerEvent::PointerItem firstItem;
    MMI::PointerEvent::PointerItem secondItem;
    event.GetPointerItem(pointerIdList[POINTER_ID_0], firstItem);
    event.GetPointerItem(pointerIdList[POINTER_ID_1], secondItem);
    int32_t durationX = secondItem.GetDisplayX() - firstItem.GetDisplayX();
    int32_t durationY = secondItem.GetDisplayY() - firstItem.GetDisplayY();
    float distance = static_cast<float>(hypot(durationX, durationY));
    HILOG_DEBUG("distance:%{public}f", distance);
    return distance;
}

float AccessibilityZoomGesture::CalcSeparationDistance(MMI::PointerEvent &firstEvent,
    MMI::PointerEvent &secondEvent)
{
    HILOG_DEBUG();

    MMI::PointerEvent::PointerItem firstItem;
    MMI::PointerEvent::PointerItem secondItem;
    firstEvent.GetPointerItem(firstEvent.GetPointerId(), firstItem);
    secondEvent.GetPointerItem(secondEvent.GetPointerId(), secondItem);
    int32_t durationX = secondItem.GetDisplayX() - firstItem.GetDisplayX();
    int32_t durationY = secondItem.GetDisplayY() - firstItem.GetDisplayY();
    float distance = static_cast<float>(hypot(durationX, durationY));
    HILOG_DEBUG("distance:%{public}f", distance);
    return distance;
}


AccessibilityZoomGesture::ZoomGestureEventHandler::ZoomGestureEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityZoomGesture &zoomGesture): AppExecFwk::EventHandler(runner), zoomGesture_(zoomGesture)
{
    HILOG_DEBUG();
}

void AccessibilityZoomGesture::ZoomGestureEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    uint32_t eventId = event->GetInnerEventId();
    HILOG_DEBUG("process msg: %{public}d", eventId);
    switch (eventId) {
        case MULTI_TAP_MSG:
            HILOG_DEBUG("process multi tap msg.");
            zoomGesture_.TransferState(INIT);
            zoomGesture_.SendCacheEventsToNext();
            break;
        case HOLDING_MSG:
            {
            zoomGesture_.TransferState(HOLD);
            if (zoomGesture_.gestureMode_ == SINGLE_FINGER_TRIPLE_TAP_MODE) {
                int32_t pointerId = zoomGesture_.lastDownEvent_->GetPointerId();
                MMI::PointerEvent::PointerItem item;
                zoomGesture_.lastDownEvent_->GetPointerItem(pointerId, item);
                int32_t anchorX = item.GetDisplayX();
                int32_t anchorY = item.GetDisplayY();
                HILOG_DEBUG("anchorX:%{private}d, anchorY:%{private}d.", anchorX, anchorY);
                zoomGesture_.OnZoom(anchorX, anchorY, false);
            } else {
                int32_t anchorX = 0;
                int32_t anchorY = 0;
                for (int i = 0; i < POINTER_COUNT_3; i++) {
                    MMI::PointerEvent::PointerItem pointerItem;
                    zoomGesture_.lastTripleTapEvents_[i]->GetPointerItem(
                        zoomGesture_.lastTripleTapEvents_[i]->GetPointerId(), pointerItem);
                    anchorX += pointerItem.GetDisplayX();
                    anchorY += pointerItem.GetDisplayY();
                }
                zoomGesture_.OnZoom(anchorX / POINTER_COUNT_3, anchorY / POINTER_COUNT_3, false);
            }
            zoomGesture_.ClearCacheEventsAndMsg();
            break;
            }
        case WAIT_ANOTHER_FINGER_DOWN_MSG:
            zoomGesture_.TransferState(PASSING_THROUGH);
            zoomGesture_.SendCacheEventsToNext();
            break;
        case TWO_FINGER_SLIDING_MSG:
            zoomGesture_.TransferState(SLIDING);
            zoomGesture_.ClearCacheEventsAndMsg();
            break;
        case HOT_AREA_SLIDING_MSG:
            zoomGesture_.TransferState(HOT_AREA_SLIDING);
            zoomGesture_.ClearCacheEventsAndMsg();
            break;
        case MENU_SLIDING:
            zoomGesture_.TransferState(MENU_SLIDING);
            zoomGesture_.ClearCacheEventsAndMsg();
            break;
        case TOUCH_EXPLORATION_ZOOM_DELAY_MSG:
            {
                HILOG_DEBUG("process touch exploration zoom delay msg.");
                if (zoomGesture_.zoomState_ == READY) {
                    int32_t anchorX = 0;
                    int32_t anchorY = 0;
                    for (int i = 0; i < POINTER_COUNT_3; i++) {
                        MMI::PointerEvent::PointerItem pointerItem;
                        zoomGesture_.lastTripleTapEvents_[i]->GetPointerItem(
                            zoomGesture_.lastTripleTapEvents_[i]->GetPointerId(), pointerItem);
                        anchorX += pointerItem.GetDisplayX();
                        anchorY += pointerItem.GetDisplayY();
                    }
                    zoomGesture_.OnZoom(anchorX / POINTER_COUNT_3, anchorY / POINTER_COUNT_3, true);
                } else if (zoomGesture_.zoomState_ == ZOOM) {
                    zoomGesture_.OffZoom();
                }
                zoomGesture_.TransferState(INIT);
                zoomGesture_.ClearCacheEventsAndMsg();
                break;
            }
        default:
            break;
    }
}

void AccessibilityZoomGesture::GetWindowParam(bool needRefresh)
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    uint64_t currentScreen = displayMgr.GetDefaultDisplayId();
    OHOS::Rosen::DisplayOrientation currentOrientation = displayMgr.GetOrientation();
    if (needRefresh || (currentScreen != screenId_) || (currentOrientation != orientation_)) {
        HILOG_INFO("display id or orientation changed.");
        screenId_ = currentScreen;
        orientation_ = currentOrientation;
        sptr<Rosen::Display> display = displayMgr.GetDisplay(screenId_);
        if (!display) {
            HILOG_ERROR("Get display failed for screenId: %{public}" PRIu64, screenId_);
            return;
        }
        screenWidth_ = static_cast<uint32_t>(display->GetWidth());
        screenHeight_ = static_cast<uint32_t>(display->GetHeight());
        HILOG_INFO("screenWidth_ = %{public}d, screenHeight_ = %{public}d.", screenWidth_, screenHeight_);
    }
#else
    HILOG_INFO("not support zoom");
#endif
}

void AccessibilityZoomGesture::OnZoom(int32_t anchorX, int32_t anchorY, bool showMenu)
{
    HILOG_INFO();
    if (!hasNotifyConflict_ && gestureMode_ == THREE_FINGER_DOUBLE_TAP_MODE &&
            AccessibilityInputInterceptor::GetInstance()->IsTouchExplorationEnabled()) {
                Singleton<ExtendServiceManager>::GetInstance().notifyZoomGesutureConflictDialogCallback();
                hasNotifyConflict_ = true;
                return;
    }
    zoomState_ = ZOOM;
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
            }
    
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        if (fullScreenManager_ == nullptr) {
            HILOG_ERROR("fullScreenManager_ is nullptr.");
            return;
        }
        fullScreenManager_->EnableMagnification(anchorX, anchorY);
        if (showMenu) {
            menuManager_->ShowMenuWindow(FULL_SCREEN_MAGNIFICATION);
        }
    } else {
        if (windowMagnificationManager_ == nullptr) {
            HILOG_ERROR("windowMagnificationManager_ is nullptr.");
            return;
        }
        windowMagnificationManager_->EnableWindowMagnification(anchorX, anchorY);
        if (showMenu) {
            menuManager_->ShowMenuWindow(WINDOW_MAGNIFICATION);
        }
    }
    Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
    ExtUtils::RecordOnZoomGestureEvent("on", true);
}

void AccessibilityZoomGesture::OffZoom()
{
    HILOG_INFO();
    zoomState_ = READY;
    if (menuManager_ != nullptr) {
        HILOG_DEBUG("disable menu window");
        menuManager_->DisableMenuWindow();
    }
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        if (fullScreenManager_ != nullptr && fullScreenManager_->IsMagnificationWindowShow()) {
            HILOG_DEBUG("disable full screen magnification");
            fullScreenManager_->DisableMagnification(false);
            Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
                AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
            ExtUtils::RecordOnZoomGestureEvent("off", true);
            return;
        }
    } else {
        if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->IsMagnificationWindowShow()) {
            HILOG_DEBUG("disable window magnification");
            windowMagnificationManager_->DisableWindowMagnification();
            Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
                AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
            ExtUtils::RecordOnZoomGestureEvent("off", true);
            return;
        }
    }
}

void AccessibilityZoomGesture::OnScroll(float offsetX, float offsetY)
{
    HILOG_DEBUG("offsetX:%{public}f, offsetY:%{public}f.", offsetX, offsetY);
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        if (fullScreenManager_ == nullptr) {
            HILOG_ERROR("fullScreenManager_ is nullptr.");
            return;
        }
        fullScreenManager_->MoveMagnification(static_cast<int32_t>(offsetX), static_cast<int32_t>(offsetY));
    } else {
        if (windowMagnificationManager_ == nullptr) {
            HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
        windowMagnificationManager_->MoveMagnificationWindow(static_cast<int32_t>(offsetX),
            static_cast<int32_t>(offsetY));
    }
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    Singleton<AccessibilityExtendPowerManager>::GetInstance().RefreshActivity();
#endif
}

void AccessibilityZoomGesture::PersistScale()
{
    float scale = MagnificationWindow::GetInstance().GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    if (abs(scale_ - scale) > MIN_SCALE) {
        Singleton<ExtendServiceManager>::GetInstance().magnificationScaleCallback(scale);
        Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
            AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
        scale_ = scale;
    }
}

void AccessibilityZoomGesture::Clear()
{
    HILOG_DEBUG();
    SendCacheEventsToNext();
    TransferState(INIT);
}

void AccessibilityZoomGesture::DestroyEvents()
{
    HILOG_INFO();
    Clear();
    OffZoom();
    EventTransmission::DestroyEvents();
}

void AccessibilityZoomGesture::ShieldZoomGesture(bool state)
{
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
        }
    HILOG_INFO("ShieldZoomGesture state = %{public}d", state);
    if (shieldZoomGestureFlag_ == state) {
        return;
    }
    shieldZoomGestureFlag_ = state;
    if (state) {
        OffZoom();
    }
}

void AccessibilityZoomGesture::DoNothingHandler(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
}

void AccessibilityZoomGesture::HandleSTReadyInitStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1) {
        SendCacheEventsToNext();
        return;
    }

    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    singleFingerTapCount_ = 0;
    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
    TransferState(ONE_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleSTReadyOneFingerDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1 || IsTapOnInputMethod(event)) {
        SendCacheEventsToNext();
        TransferState(INIT);
        return;
    }

    singleFingerTapCount_ += 1;
    if (singleFingerTapCount_ == TRIPLE_TAP_COUNT) {
        ClearCacheEventsAndMsg();
        TransferState(INIT);
            int32_t pointerId = event.GetPointerId();
            MMI::PointerEvent::PointerItem item;
            event.GetPointerItem(pointerId, item);
            int32_t anchorX = item.GetDisplayX();
            int32_t anchorY = item.GetDisplayY();
        HILOG_DEBUG("anchorX:%{private}d, anchorY:%{private}d.", anchorX, anchorY);
        OnZoom(anchorX, anchorY, true);
    } else {
        TransferState(ONE_FINGER_TAP);
    }
}

void AccessibilityZoomGesture::HandleSTReadyOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1 || !IsMoveValid(lastDownEvent_, std::make_shared<MMI::PointerEvent>(event))) {
        SendCacheEventsToNext();
        TransferState(INIT);
        return;
    }
}

void AccessibilityZoomGesture::HandleSTReadyOneFingerTapStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1 || !IsDownValid(lastDownEvent_, std::make_shared<MMI::PointerEvent>(event))) {
        SendCacheEventsToNext();
        TransferState(INIT);
        return;
    }

    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    TransferState(ONE_FINGER_DOWN);
    if (singleFingerTapCount_ == DOUBLE_TAP_COUNT) {
        zoomGestureEventHandler_->SendEvent(HOLDING_MSG, 0, LONG_PRESS_TIMER);
    } else {
        zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
    }
}

void AccessibilityZoomGesture::HandleSTZoomInitStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        gestureType_ = fullScreenManager_->CheckTapOnHotArea(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    } else {
        isTapOnWindowHotArea_ = windowMagnificationManager_->IsTapOnHotArea(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
        isTapOnWindow_ = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
    }
    isTapOnMenu_ = menuManager_->IsTapOnMenu(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());

    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    lastTripleTapEvents_[POINTER_ID_0] = std::make_shared<MMI::PointerEvent>(event);
    singleFingerTapCount_ = 0;
    if (isTapOnWindowHotArea_) {
        zoomGestureEventHandler_->SendEvent(HOT_AREA_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else if (isTapOnMenu_) {
        zoomGestureEventHandler_->SendEvent(MENU_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else {
        zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
        }
    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
    TransferState(ONE_FINGER_DOWN);
    }

void AccessibilityZoomGesture::HandleSTZoomOneFingerDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    gestureType_ = INVALID_GESTURE_TYPE;

    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    zoomGestureEventHandler_->RemoveEvent(HOT_AREA_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(MENU_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);

    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);
    isTapOnWindow_ = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItem.GetDisplayX(),
        pointerItem.GetDisplayY());
    if (magnificationMode_ != FULL_SCREEN_MAGNIFICATION && !isTapOnWindow_) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    lastTripleTapEvents_[POINTER_ID_1] = std::make_shared<MMI::PointerEvent>(event);
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION || isTapOnWindow_) {
        zoomGestureEventHandler_->SendEvent(TWO_FINGER_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
        CalcFocusCoordinate(event, lastCenter);
        baseDistance_ = lastDistance_ = CalcSeparationDistance(event);
    }
    TransferState(TWO_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleSTZoomOneFingerDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(HOT_AREA_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(MENU_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    if (menuManager_ != nullptr) {
        MMI::PointerEvent::PointerItem pointerItem;
        event.GetPointerItem(event.GetPointerId(), pointerItem);
        if (isTapOnMenu_) {
            menuManager_->OnMenuTap();
            TransferState(INIT);
            ClearCacheEventsAndMsg();
            return;
        }
    }

    singleFingerTapCount_ += 1;
    if (singleFingerTapCount_ == POINTER_COUNT_3) {
        TransferState(INIT);
        ClearCacheEventsAndMsg();
        OffZoom();
    } else {
        TransferState(ONE_FINGER_TAP);
    }
}

void AccessibilityZoomGesture::HandleSTZoomOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1 || !IsMoveValid(lastDownEvent_, std::make_shared<MMI::PointerEvent>(event))) {
        if (isTapOnWindowHotArea_) {
            ClearCacheEventsAndMsg();
            TransferState(HOT_AREA_SLIDING);
            return;
        }
        if (isTapOnMenu_) {
            ClearCacheEventsAndMsg();
            TransferState(MENU_SLIDING);
            return;
        }
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
}

void AccessibilityZoomGesture::HandleSTZoomTwoFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    if (event.GetPointerIds().size() != POINTER_COUNT_2) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
        }
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_ID_2 || IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        return;
    }
    zoomGestureEventHandler_->RemoveEvent(TWO_FINGER_SLIDING_MSG);
    if (magnificationMode_ != FULL_SCREEN_MAGNIFICATION && !isTapOnWindow_) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    ClearCacheEventsAndMsg();
    TransferState(SLIDING);
}

void AccessibilityZoomGesture::HandleSTZoomOneFingerTapStateDown(MMI::PointerEvent &event)
{
    CacheEvents(event);
    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1 || !IsDownValid(lastDownEvent_, std::make_shared<MMI::PointerEvent>(event))) {
        SendCacheEventsToNext();
        TransferState(INIT);
        return;
    }

    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    TransferState(ONE_FINGER_DOWN);
    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
}

void AccessibilityZoomGesture::HandleZoomSlidingStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    size_t pointerCount = event.GetPointerIds().size();
    CalcFocusCoordinate(event, lastCenter);
    if (pointerCount == POINTER_COUNT_2) {
        baseDistance_ = lastDistance_ = CalcSeparationDistance(event);
    }
}

void AccessibilityZoomGesture::HandleZoomSlidingStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    size_t pointerCount = event.GetPointerIds().size();
    ZOOM_FOCUS_COORDINATE coordinate = {0.0f, 0.0f};
    CalcFocusCoordinate(event, coordinate);
    if (pointerCount != POINTER_COUNT_2) {
        RecognizeScroll(coordinate);
    } else {
        if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
            RecognizeScroll(coordinate);
            RecognizeScale(event);
        } else {
            if (!RecognizeScale(event)) {
                RecognizeScroll(coordinate);
            }
        }
    }
}

void AccessibilityZoomGesture::HandleZoomSlidingStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount == POINTER_COUNT_1) {
        TransferState(INIT);
        PersistScale();
    }
    if (pointerCount == POINTER_COUNT_3) {
        baseDistance_ = lastDistance_ = CalcSeparationDistance(event);
    }
    CalcFocusCoordinate(event, lastCenter);
}

void AccessibilityZoomGesture::HandleTDReadyInitStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }

    lastTripleTapEvents_[0] = std::make_shared<MMI::PointerEvent>(event);
    zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    TransferState(ONE_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleTDReadyOneFingerDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_2) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }

    lastTripleTapEvents_[POINTER_COUNT_1] = std::make_shared<MMI::PointerEvent>(event);
    zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    TransferState(TWO_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleTDReadyOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_COUNT_2 || !IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDReadyTwoFingersDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_3) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }

    lastTripleTapEvents_[POINTER_COUNT_2] = std::make_shared<MMI::PointerEvent>(event);
    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
    TransferState(THREE_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleTDReadyTwoFingersDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_COUNT_2 || !IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount == POINTER_COUNT_1) {
        TransferState(THREE_FINGER_TAP);
    } else if (pointerCount > POINTER_COUNT_3) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_COUNT_2 || !IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersTapStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    if (event.GetPointerId() < POINTER_COUNT_3) {
        lastTripleTapEvents_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
    }
    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize < POINTER_COUNT_3) {
        zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else if (pointerSize == POINTER_COUNT_3) {
        zoomGestureEventHandler_->SendEvent(HOLDING_MSG, 0, LONG_PRESS_TIMER);
        TransferState(THREE_FINGER_TAP_THEN_DOWN);
    } else {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersTapStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_COUNT_2 || !IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    zoomGestureEventHandler_->RemoveEvent(HOLDING_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == POINTER_COUNT_1) {
        if (AccessibilityInputInterceptor::GetInstance()->IsTouchExplorationEnabled()) {
            HILOG_INFO("Touch exploration enabled, set delay timer for zoom.");
            TransferState(THREE_FINGER_DOUBLE_TAP);
            zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
            zoomGestureEventHandler_->SendEvent(TOUCH_EXPLORATION_ZOOM_DELAY_MSG, 0,
                TOUCH_EXPLORATION_ZOOM_DELAY_TIMER);
        } else {
            int32_t anchorX = 0;
            int32_t anchorY = 0;
            for (int i = 0; i < POINTER_COUNT_3; i++) {
                MMI::PointerEvent::PointerItem pointerItem;
                lastTripleTapEvents_[i]->GetPointerItem(lastTripleTapEvents_[i]->GetPointerId(), pointerItem);
                anchorX += pointerItem.GetDisplayX();
                anchorY += pointerItem.GetDisplayY();
            }
            OnZoom(anchorX / POINTER_COUNT_3, anchorY / POINTER_COUNT_3, true);
            ClearCacheEventsAndMsg();
            TransferState(INIT);
        }
    }
}

void AccessibilityZoomGesture::HandleTDReadyThreeFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    bool isMoveValid = false;
    for (int i = 0; i < POINTER_COUNT_3; i++) {
        isMoveValid |= IsMoveValid(lastTripleTapEvents_[i], std::make_shared<MMI::PointerEvent>(event));
        }
    if (event.GetPointerIds().size() > POINTER_COUNT_3 || !isMoveValid) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleMenuSlidingStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() == 0) {
        if (!lastDownEvent_) {
            lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }
        MMI::PointerEvent::PointerItem lastDownItem;
        lastDownEvent_->GetPointerItem(lastDownEvent_->GetPointerId(), lastDownItem);
        MMI::PointerEvent::PointerItem currentItem;
        event.GetPointerItem(event.GetPointerId(), currentItem);
        int32_t deltaX = currentItem.GetDisplayX() - lastDownItem.GetDisplayX();
        int32_t deltaY = currentItem.GetDisplayY() - lastDownItem.GetDisplayY();
        menuManager_->MoveMenuWindow(deltaX, deltaY);

        lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }
}

void AccessibilityZoomGesture::HandleMenuSlidingStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() != 0) {
        return;
    }
    if (menuManager_ != nullptr) {
        menuManager_->AttachToEdge();
    }
    if (event.GetPointerIds().size() > POINTER_COUNT_1) {
        TransferState(INVALID);
    } else {
        TransferState(INIT);
    }
}

void AccessibilityZoomGesture::HandleHotAreaSlidingStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() == 0) {
        if (!lastDownEvent_) {
            lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }
        MMI::PointerEvent::PointerItem lastDownItem;
        lastDownEvent_->GetPointerItem(lastDownEvent_->GetPointerId(), lastDownItem);
        MMI::PointerEvent::PointerItem currentItem;
        event.GetPointerItem(event.GetPointerId(), currentItem);
        int32_t deltaX = currentItem.GetDisplayX() - lastDownItem.GetDisplayX();
        int32_t deltaY = currentItem.GetDisplayY() - lastDownItem.GetDisplayY();
        windowMagnificationManager_->MoveMagnificationWindow(deltaX, deltaY);

        lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }
}

void AccessibilityZoomGesture::HandleHotAreaSlidingStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() != 0) {
        return;
    }
    if (event.GetPointerIds().size() > POINTER_COUNT_1) {
        TransferState(INVALID);
    } else {
        TransferState(INIT);
    }
}

void AccessibilityZoomGesture::HandleHoldStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CalcFocusCoordinate(event, lastCenter);
}

void AccessibilityZoomGesture::HandleHoldStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerIds().size() == POINTER_COUNT_1) {
        OffZoom();
        TransferState(INIT);
    }
    CalcFocusCoordinate(event, lastCenter);
}

void AccessibilityZoomGesture::HandleHoldStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    ZOOM_FOCUS_COORDINATE coordinate = {0.0f, 0.0f};
    CalcFocusCoordinate(event, coordinate);
    RecognizeScroll(coordinate, true);
}

void AccessibilityZoomGesture::HandleTDZoomInitStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_1) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION) {
        gestureType_ = fullScreenManager_->CheckTapOnHotArea(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
            } else {
        isTapOnWindowHotArea_ = windowMagnificationManager_->IsTapOnHotArea(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
        isTapOnWindow_ = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
            }
    isTapOnMenu_ = menuManager_->IsTapOnMenu(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());

    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    lastTripleTapEvents_[0] = std::make_shared<MMI::PointerEvent>(event);
    if (isTapOnWindowHotArea_) {
        zoomGestureEventHandler_->SendEvent(HOT_AREA_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else if (isTapOnMenu_) {
        zoomGestureEventHandler_->SendEvent(MENU_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else {
        zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    }
    TransferState(ONE_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleTDZoomOneFingerDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    gestureType_ = INVALID_GESTURE_TYPE;

    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(HOT_AREA_SLIDING_MSG);
    zoomGestureEventHandler_->RemoveEvent(MENU_SLIDING_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_2) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
            return;
        }
    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);
    isTapOnWindow_ = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItem.GetDisplayX(),
        pointerItem.GetDisplayY());
    lastTripleTapEvents_[1] = std::make_shared<MMI::PointerEvent>(event);
    TransferState(TWO_FINGER_DOWN);
    if (magnificationMode_ != FULL_SCREEN_MAGNIFICATION && !isTapOnWindow_) {
        zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
        return;
    } else {
        zoomGestureEventHandler_->SendEvent(TWO_FINGER_SLIDING_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
        CalcFocusCoordinate(event, lastCenter);
        baseDistance_ = lastDistance_ = CalcSeparationDistance(event);
    }
}

void AccessibilityZoomGesture::HandleTDZoomOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_ID_2) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
        return;
    }
    if (!IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        if (isTapOnWindowHotArea_) {
            ClearCacheEventsAndMsg();
            TransferState(HOT_AREA_SLIDING);
            return;
        }
        if (isTapOnMenu_) {
            ClearCacheEventsAndMsg();
            TransferState(MENU_SLIDING);
            return;
        }
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDZoomOneFingerDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != 1) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    if (menuManager_ != nullptr) {
        MMI::PointerEvent::PointerItem pointerItem;
        event.GetPointerItem(event.GetPointerId(), pointerItem);
        if (menuManager_->IsTapOnMenu(pointerItem.GetDisplayX(), pointerItem.GetDisplayY())) {
            menuManager_->OnMenuTap();
            TransferState(INIT);
            ClearCacheEventsAndMsg();
        return;
        }
    }
    TransferState(PASSING_THROUGH);
    SendCacheEventsToNext();
}

void AccessibilityZoomGesture::HandleTDZoomTwoFingersDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(TWO_FINGER_SLIDING_MSG);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_3) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }

    lastTripleTapEvents_[POINTER_COUNT_2] = std::make_shared<MMI::PointerEvent>(event);
    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
    TransferState(THREE_FINGER_DOWN);
}

void AccessibilityZoomGesture::HandleTDZoomTwoFingersDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    if (event.GetPointerIds().size() != POINTER_COUNT_2) {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
        return;
    }
    int32_t pId = event.GetPointerId();
    if (pId > POINTER_ID_2 || IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        return;
    }
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(TWO_FINGER_SLIDING_MSG);
    if (magnificationMode_ == FULL_SCREEN_MAGNIFICATION || isTapOnWindow_) {
        ClearCacheEventsAndMsg();
        TransferState(SLIDING);
    } else {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
    }
}


void AccessibilityZoomGesture::HandleTDZoomThreeFingersDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount == POINTER_COUNT_1) {
        TransferState(THREE_FINGER_TAP);
    } else if (pointerCount > POINTER_COUNT_3) {
    SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
    }
}

void AccessibilityZoomGesture::HandleTDZoomThreeFingersDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    bool isMoveValid = false;
    if (event.GetPointerIds().size() > POINTER_COUNT_3) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
        return;
    }
    for (int i = 0; i < POINTER_COUNT_3; i++) {
        isMoveValid |= IsMoveValid(lastTripleTapEvents_[i], std::make_shared<MMI::PointerEvent>(event));
    }
    if (!isMoveValid) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDZoomThreeFingersTapStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    zoomGestureEventHandler_->RemoveEvent(WAIT_ANOTHER_FINGER_DOWN_MSG);
    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    if (event.GetPointerId() < POINTER_COUNT_3) {
        lastTripleTapEvents_[event.GetPointerId()] = std::make_shared<MMI::PointerEvent>(event);
    }
    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize < POINTER_COUNT_3) {
        zoomGestureEventHandler_->SendEvent(WAIT_ANOTHER_FINGER_DOWN_MSG, 0, MULTI_FINGER_TAP_INTERVAL_TIMER);
    } else if (pointerSize == POINTER_COUNT_3) {
        zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
        TransferState(THREE_FINGER_TAP_THEN_DOWN);
    } else {
        SendCacheEventsToNext();
        TransferState(PASSING_THROUGH);
    }
}

void AccessibilityZoomGesture::HandleTDZoomThreeFingersTapStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);

    int32_t pId = event.GetPointerId();
    if (pId > POINTER_ID_2) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
        return;
    }
    if (!IsMoveValid(lastTripleTapEvents_[pId], std::make_shared<MMI::PointerEvent>(event))) {
        uint32_t pointerSize = event.GetPointerIds().size();
        if (pointerSize == POINTER_COUNT_2 && (magnificationMode_ == FULL_SCREEN_MAGNIFICATION || isTapOnWindow_)) {
            ClearCacheEventsAndMsg();
            TransferState(SLIDING);
            return;
        }
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}

void AccessibilityZoomGesture::HandleTDZoomThreeFingersContinueDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    zoomGestureEventHandler_->RemoveEvent(LONG_PRESS_MSG);

    uint32_t pointerSize = event.GetPointerIds().size();
    if (pointerSize == POINTER_COUNT_1) {
        if (AccessibilityInputInterceptor::GetInstance()->IsTouchExplorationEnabled()) {
            HILOG_INFO("Touch exploration enabled in ZOOM state, set delay timer for off zoom.");
            TransferState(THREE_FINGER_DOUBLE_TAP);
            zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
            zoomGestureEventHandler_->SendEvent(TOUCH_EXPLORATION_ZOOM_DELAY_MSG, 0,
                TOUCH_EXPLORATION_ZOOM_DELAY_TIMER);
        } else {
            OffZoom();
            ClearCacheEventsAndMsg();
            TransferState(INIT);
        }
    }
}

void AccessibilityZoomGesture::HandleTDZoomThreeFingersContinueDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CacheEvents(event);
    bool isMoveValid = false;
    for (int i = 0; i < POINTER_COUNT_3; i++) {
        isMoveValid |= IsMoveValid(lastTripleTapEvents_[i], std::make_shared<MMI::PointerEvent>(event));
    }
    if (event.GetPointerIds().size() > POINTER_COUNT_3 || !isMoveValid) {
        TransferState(PASSING_THROUGH);
        SendCacheEventsToNext();
    }
}
} // namespace Accessibility
} // namespace OHOS
