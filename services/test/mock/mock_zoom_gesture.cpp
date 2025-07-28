/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "window_accessibility_controller.h"
#include "accessibility_window_manager.h"
#include "utils.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "accessibility_power_manager.h"
#endif

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t POINTER_COUNT_1 = 1;
    constexpr size_t POINTER_COUNT_2 = 2;
    constexpr float TAP_MIN_DISTANCE = 8.0f;
    constexpr int32_t MULTI_TAP_TIMER = 250; // ms
    constexpr int32_t LONG_PRESS_TIMER = 300; // ms
    constexpr float DOUBLE_TAP_SLOP = 100.0f;
    constexpr uint32_t DOUBLE = 2;
    constexpr uint32_t TRIPLE_TAP_COUNT = 3;
    constexpr float NORMAL_SCALE = 1.0f;
    constexpr float MIN_SCROLL_SPAN = 2.0f;
    constexpr float MIN_SCALE_SPAN = 2.0f;
    constexpr float DEFAULT_ANCHOR = 0.5f;
    constexpr float MIN_SCALE = 0.1f;
} // namespace

MockZoomGesture::MockZoomGesture(
    std::shared_ptr<FullScreenMagnificationManager> fullScreenManager,
    std::shared_ptr<MagnificationMenuManager> menuManager)
    : fullScreenManager_(fullScreenManager), menuManager_(menuManager)
{
    HILOG_DEBUG();
    zoomGestureEventHandler_ = std::make_shared<ZoomGestureEventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner(), *this);

    tapDistance_ = TAP_MIN_DISTANCE;

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }

    float densityPixels = display->GetVirtualPixelRatio();
    multiTapDistance_ = densityPixels * DOUBLE_TAP_SLOP + 0.5f;
#else
    HILOG_DEBUG("not support display manager");
    multiTapDistance_ = 1 * DOUBLE_TAP_SLOP + 0.5f;
#endif
}

bool MockZoomGesture::IsTapOnInputMethod(MMI::PointerEvent &event)
{
    size_t pointerCount = event.GetPointerIds().size();
    if (pointerCount != POINTER_COUNT_1) {
        HILOG_DEBUG("not single finger.");
        return false;
    }
    std::vector<AccessibilityWindowInfo> windowInfos =
        Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
    for (auto &window : windowInfos) {
        if (window.GetWindowType() == INPUT_METHOD_WINDOW_TYPE) {
            Rect inputRect = window.GetRectInScreen();
            int32_t leftTopX = inputRect.GetLeftTopXScreenPostion();
            int32_t leftTopY = inputRect.GetLeftTopYScreenPostion();
            int32_t rightBottomX = inputRect.GetRightBottomXScreenPostion();
            int32_t rightBottomY = inputRect.GetRightBottomYScreenPostion();
            
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

bool MockZoomGesture::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("state_ is %{public}d.", state_);

    if (shieldZoomGestureFlag_) {
        EventTransmission::OnPointerEvent(event);
        return true;
    }

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
        HILOG_INFO("PointerAction: %{public}d.", event.GetPointerAction());
    }

    int32_t sourceType = event.GetSourceType();
    if (state_ == READY_STATE && sourceType != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    if (event.GetPointerId() == SCROLL_SHOT_POINTER_ID) {
        HILOG_DEBUG("scrollshot injected.");
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    switch (state_) {
        case READY_STATE:
            CacheEvents(event);
            RecognizeInReadyState(event);
            break;
        case ZOOMIN_STATE:
            CacheEvents(event);
            RecognizeInZoomState(event);
            break;
        case SLIDING_STATE:
            RecognizeInSlidingState(event);
            break;
        case MENU_SLIDING_STATE:
            RecognizeInMenuSlidingState(event);
            break;
        case DRAGGING_STATE:
            RecognizeInDraggingState(event);
            break;
        default:
            break;
    }
    return true;
}

void MockZoomGesture::TransferState(ACCESSIBILITY_ZOOM_STATE state)
{
    HILOG_INFO("old state= %{public}d, new state= %{public}d", state_, state);
    state_ = state;
}

void MockZoomGesture::CacheEvents(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);

    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            if (pointerCount == POINTER_COUNT_1) {
                HILOG_DEBUG("Cache pointer down");
                preLastDownEvent_ = lastDownEvent_;
                lastDownEvent_ = pointerEvent;
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (pointerCount == POINTER_COUNT_1) {
                HILOG_DEBUG("Cache pointer up");
                preLastUpEvent_ = lastUpEvent_;
                lastUpEvent_ = pointerEvent;
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            if (pointerCount == POINTER_COUNT_1) {
                HILOG_DEBUG("Cache pointer move.");
                currentMoveEvent_ = pointerEvent;
            }
            break;
        default:
            HILOG_DEBUG("Action is %{public}d", action);
            break;
    }
    cacheEvents_.emplace_back(pointerEvent);
}

void MockZoomGesture::SendCacheEventsToNext()
{
    HILOG_DEBUG();

    bool isStartNewAction = false;
    int32_t action = MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    std::vector<std::shared_ptr<MMI::PointerEvent>> cacheEventsTmp;
    std::copy(cacheEvents_.begin(), cacheEvents_.end(), std::back_inserter(cacheEventsTmp));

    ClearCacheEventsAndMsg();

    size_t cacheEventsNum = 0;
    size_t cacheEventsTotalNum = cacheEventsTmp.size();

    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    
    for (auto &pointerEvent : cacheEventsTmp) {
        cacheEventsNum++;
        action = pointerEvent->GetPointerAction();
        if ((cacheEventsNum > 1) &&
            (cacheEventsNum == cacheEventsTotalNum) &&
            (action == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
            HILOG_DEBUG("The down event needs to be parsed again");
            isStartNewAction = true;
        }
        if (isStartNewAction) {
            OnPointerEvent(*pointerEvent);
            continue;
        }
        if (state_ != READY_STATE && fullScreenManager_->IsMagnificationWindowShow()) {
            MMI::PointerEvent::PointerItem pointer {};
            pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointer);
            PointerPos coordinates  = fullScreenManager_->ConvertCoordinates(pointer.GetDisplayX(),
                pointer.GetDisplayY());
            if (gestureType_ != INVALID_GESTURE_TYPE) {
                coordinates = fullScreenManager_->ConvertGesture(gestureType_, coordinates);
            }
            pointer.SetDisplayX(coordinates.posX);
            pointer.SetDisplayY(coordinates.posY);
            pointer.SetTargetWindowId(-1);
            pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
            pointerEvent->AddPointerItem(pointer);
            pointerEvent->SetZOrder(10000); // 10000 is magnification window zorder
        }
        pointerEvent->SetActionTime(Utils::GetSystemTime() * US_TO_MS);
        EventTransmission::OnPointerEvent(*pointerEvent);
    }
}

void MockZoomGesture::ClearCacheEventsAndMsg()
{
    HILOG_DEBUG();

    cacheEvents_.clear();
    preLastDownEvent_ = nullptr;
    lastDownEvent_ = nullptr;
    preLastUpEvent_ = nullptr;
    lastUpEvent_ = nullptr;
}

void MockZoomGesture::RecognizeInReadyState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    bool isTripleTaps = false;

    HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
            if ((pointerCount == POINTER_COUNT_1) && IsDownValid()) {
                zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
                IsTripleTaps();
            } else {
                SendCacheEventsToNext();
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if ((pointerCount == POINTER_COUNT_1) && IsUpValid() && !(IsTapOnInputMethod(event))) {
                isTripleTaps = IsTripleTaps();
            } else {
                SendCacheEventsToNext();
                HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            if ((pointerCount == POINTER_COUNT_1) && IsMoveValid()) {
                HILOG_DEBUG("move valid.");
            } else {
                SendCacheEventsToNext();
                HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            SendCacheEventsToNext();
            break;
        default:
            break;
    }

    if (isTripleTaps) {
        OnTripleTap(event);
        zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    }
}

void MockZoomGesture::RecognizeInZoomStateDownEvent(MMI::PointerEvent &event)
{
    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    gestureType_ = INVALID_GESTURE_TYPE;
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t pointerCount = pointerIdList.size();
    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
    if (pointerCount == POINTER_COUNT_1) {
        isLongPress_ = false;
        MMI::PointerEvent::PointerItem pointerItem;
        event.GetPointerItem(event.GetPointerId(), pointerItem);
        gestureType_ = fullScreenManager_->CheckTapOnHotArea(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        isTapOnMenu_ = menuManager_->IsTapOnMenu(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
        longPressDownEvent_ = pointerEvent;
        downPid_ = event.GetPointerId();
        if (IsDownValid()) {
            if (!isTapOnMenu_) {
                zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
            }
        } else {
            SendCacheEventsToNext();
        }
    } else if (pointerCount == POINTER_COUNT_2) {
        gestureType_ = INVALID_GESTURE_TYPE;
        if (isLongPress_ || IsKnuckles(event)) {
            HILOG_INFO("not transferState sliding.");
            SendCacheEventsToNext();
        } else {
            TransferState(SLIDING_STATE);
            scale_ = fullScreenManager_->GetScale();
            isScale_ = false;
            ClearCacheEventsAndMsg();
            ZOOM_FOCUS_COORDINATE focusXY = {0.0f, 0.0f};
            CalcFocusCoordinate(event, focusXY);
            lastScrollFocusX_ = focusXY.centerX;
            lastScrollFocusY_ = focusXY.centerY;
            float span = CalcScaleSpan(event, focusXY);
            preSpan_ = lastSpan_ = span;
        }
    } else {
        HILOG_INFO("invalid pointer count.");
    }
}

void MockZoomGesture::RecognizeInZoomStateMoveEvent(MMI::PointerEvent &event)
{
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t pointerCount = pointerIdList.size();
    HILOG_DEBUG();
    if ((pointerCount == POINTER_COUNT_1) && !IsLongPress() && IsMoveValid()) {
        HILOG_DEBUG("move valid.");
    } else if (isTapOnMenu_ && (!IsMoveValid() || IsLongPress())) {
        TransferState(MENU_SLIDING_STATE);
        ClearCacheEventsAndMsg();
    } else {
        SendCacheEventsToNext();
        HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
    }
}

void MockZoomGesture::RecognizeInZoomState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointerIdList = event.GetPointerIds();
    size_t pointerCount = pointerIdList.size();
    bool isTripleTaps = false;

    HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            RecognizeInZoomStateDownEvent(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (downPid_ == event.GetPointerId()) {
                isLongPress_ = false;
            }
            if ((pointerCount == POINTER_COUNT_1) && IsUpValid() && !(IsTapOnInputMethod(event))) {
                if (isTapOnMenu_ && menuManager_ != nullptr) {
                    menuManager_->OnMenuTap();
                    ClearCacheEventsAndMsg();
                } else {
                    isTripleTaps = IsTripleTaps();
                }
            } else {
                SendCacheEventsToNext();
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            RecognizeInZoomStateMoveEvent(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            SendCacheEventsToNext();
            HILOG_DEBUG("action:%{public}d", action);
            break;
        default:
            break;
    }

    if (isTripleTaps) {
        OnTripleTap(event);
    }
}

void MockZoomGesture::RecognizeInSlidingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    ZOOM_FOCUS_COORDINATE coordinate = {0.0f, 0.0f};
    CalcFocusCoordinate(event, coordinate);

    if (pointerCount == POINTER_COUNT_2) {
        RecognizeScale(event, coordinate);
        RecognizeScroll(event, coordinate);
    }

    HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);

    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }

    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (pointerCount == POINTER_COUNT_1) {
                TransferState(ZOOMIN_STATE);
                if (isScale_ && (abs(scale_ - fullScreenManager_->GetScale()) > MIN_SCALE)) {
                    fullScreenManager_->PersistScale();
                    isScale_ = false;
                }
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            TransferState(ZOOMIN_STATE);
            isScale_ = false;
            break;
        default:
            break;
    }
}

void MockZoomGesture::RecognizeInMenuSlidingState(MMI::PointerEvent &event)
{
    HILOG_INFO();
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    int32_t action = event.GetPointerAction();
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            if (isTapOnMenu_) {
                ClearCacheEventsAndMsg();
                if (lastSlidingEvent_ == nullptr) {
                    lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
                }
                MMI::PointerEvent::PointerItem lastSlidingItem;
                lastSlidingEvent_->GetPointerItem(lastSlidingEvent_->GetPointerId(), lastSlidingItem);

                MMI::PointerEvent::PointerItem currentItem;
                event.GetPointerItem(event.GetPointerId(), currentItem);

                int32_t deltaX = currentItem.GetDisplayX() - lastSlidingItem.GetDisplayX();
                int32_t deltaY = currentItem.GetDisplayY() - lastSlidingItem.GetDisplayY();
                menuManager_->MoveMenuWindow(deltaX, deltaY);

                lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
            } else {
                if (lastSlidingEvent_ != nullptr) {
                    lastSlidingEvent_ = nullptr;
                }
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            lastSlidingEvent_ = nullptr;
            isTapOnMenu_ = false;
            menuManager_->AttachToEdge();
            TransferState(ZOOMIN_STATE);
            ClearCacheEventsAndMsg();
            break;
        default:
            break;
    }
}

void MockZoomGesture::RecognizeScroll(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE &coordinate)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_UP:
            lastScrollFocusX_ = coordinate.centerX;
            lastScrollFocusY_ = coordinate.centerY;
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            float offsetX = coordinate.centerX - lastScrollFocusX_;
            float offsetY = coordinate.centerY - lastScrollFocusY_;
            if ((abs(offsetX) > MIN_SCROLL_SPAN) || (abs(offsetY) > MIN_SCROLL_SPAN)) {
                lastScrollFocusX_ = coordinate.centerX;
                lastScrollFocusY_ = coordinate.centerY;
                OnScroll(offsetX, offsetY);
            }
            break;
        }
        default:
            break;
    }
}

void MockZoomGesture::RecognizeScale(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE &coordinate)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    if (((action == MMI::PointerEvent::POINTER_ACTION_UP) && (pointerCount != POINTER_COUNT_2)) ||
        (action == MMI::PointerEvent::POINTER_ACTION_CANCEL)) {
        HILOG_DEBUG("Scaling is end");
        startScaling_ = false;
        preSpan_ = lastSpan_ = 0;
        return;
    }

    float span = CalcScaleSpan(event, coordinate);

    if (action == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        if (abs(preSpan_ - span) >= MIN_SCALE_SPAN) {
            startScaling_ = true;
            HILOG_DEBUG("start scaling.");
        }
    }
    if (!startScaling_) {
        // When the span is greater than or equal to MIN_SCALE_SPAN, start scaling.
        if (abs(preSpan_ - span) >= MIN_SCALE_SPAN) {
            startScaling_ = true;
            HILOG_DEBUG("start scaling.");
        }
    } else {
        // When the span is smaller than the MIN_SCALE_SPAN,
        // the scale recognition will be restarted.
        if (abs(lastSpan_ - span) < 1) {
            startScaling_ = false;
            preSpan_ = lastSpan_ = span;
        }
        if ((action == MMI::PointerEvent::POINTER_ACTION_UP) ||
            (action == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
            preSpan_ = lastSpan_ = span;
        }
    }

    if (!startScaling_) {
        HILOG_DEBUG("Current is not scaling");
        return;
    }

    if (action != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        HILOG_DEBUG("Action(%{public}d) is not move", action);
        return;
    }

    float scaleSpan = span - lastSpan_;
    if (abs(scaleSpan) > EPS) {
        OnScale(scaleSpan);
        lastSpan_ = span;
        isScale_ = true;
    }
}

void MockZoomGesture::RecognizeInDraggingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    int32_t action = event.GetPointerAction();
    if (event.GetPointerId() != DEFAULT_POINTER_ID) {
        return;
    }
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            int32_t pointerId = event.GetPointerId();
            MMI::PointerEvent::PointerItem item;
            event.GetPointerItem(pointerId, item);
            int32_t anchorX = item.GetDisplayX();
            int32_t anchorY = item.GetDisplayY();
            float offsetX = lastScrollFocusX_ - anchorX;
            float offsetY = lastScrollFocusY_ - anchorY;
            if ((abs(offsetX) > MIN_SCROLL_SPAN) || (abs(offsetY) > MIN_SCROLL_SPAN)) {
                lastScrollFocusX_ = anchorX;
                lastScrollFocusY_ = anchorY;
                OnScroll(offsetX, offsetY);
            }
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
            OffZoom();
            TransferState(READY_STATE);
            break;
        default:
            break;
    }
}

void MockZoomGesture::CalcFocusCoordinate(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE &coordinate)
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

float MockZoomGesture::CalcScaleSpan(MMI::PointerEvent &event, ZOOM_FOCUS_COORDINATE coordinate)
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

    if (action == MMI::PointerEvent::POINTER_ACTION_UP) {
        upPointerId = event.GetPointerId();
        HILOG_DEBUG("The pointer id of up is %{public}d", upPointerId);
        count--;
    }

    if (!count) {
        HILOG_DEBUG("The size of PointerIds(down) is invalid");
        return span;
    }

    for (int32_t pointerId : pointerIdList) {
        if (pointerId == upPointerId) {
            continue;
        }
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

bool MockZoomGesture::IsDownValid()
{
    HILOG_DEBUG();

    if (!preLastDownEvent_) {
        HILOG_DEBUG("This is the first down event");
        return true;
    }

    if (CalcSeparationDistance(preLastDownEvent_, lastDownEvent_) >= multiTapDistance_) {
        HILOG_DEBUG("The down event is vailid");
        return false;
    }
    return true;
}

bool MockZoomGesture::IsUpValid()
{
    HILOG_DEBUG();

    if (!lastDownEvent_) {
        HILOG_DEBUG("The up event is invailid");
        return false;
    }

    if (CalcIntervalTime(lastDownEvent_, lastUpEvent_) >= LONG_PRESS_TIMER) {
        HILOG_DEBUG("The time has exceeded the long press time");
        return false;
    }

    if (CalcSeparationDistance(lastDownEvent_, lastUpEvent_) >= tapDistance_) {
        HILOG_DEBUG("The distance has exceeded the threshold");
        return false;
    }
    return true;
}

bool MockZoomGesture::IsMoveValid()
{
    HILOG_DEBUG();

    if (!lastDownEvent_) {
        HILOG_DEBUG("The move event is invailid");
        return false;
    }

    if (CalcIntervalTime(lastDownEvent_, currentMoveEvent_) >= LONG_PRESS_TIMER) {
        HILOG_DEBUG("The time has exceeded the long press time");
        return false;
    }

    if (CalcSeparationDistance(lastDownEvent_, currentMoveEvent_) >= tapDistance_) {
        HILOG_DEBUG("The distance has exceeded the threshold");
        return false;
    }
    return true;
}

bool MockZoomGesture::IsLongPress()
{
    HILOG_DEBUG();

    if (CalcIntervalTime(longPressDownEvent_, currentMoveEvent_) >= LONG_PRESS_TIMER) {
        HILOG_DEBUG("The time has exceeded the long press time");
        isLongPress_ = true;
        return true;
    }
    return false;
}

bool MockZoomGesture::IsKnuckles(MMI::PointerEvent &event)
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

bool MockZoomGesture::IsTripleTaps()
{
    HILOG_DEBUG();

    uint32_t upEventCount = 0;
    uint32_t downEventCount = 0;
    int32_t action = MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    for (auto &pointerEvent : cacheEvents_) {
        action = pointerEvent->GetPointerAction();
        if (action == MMI::PointerEvent::POINTER_ACTION_UP) {
            upEventCount++;
        }
        if (action == MMI::PointerEvent::POINTER_ACTION_DOWN) {
            downEventCount++;
        }
    }

    if (downEventCount == TRIPLE_TAP_COUNT && upEventCount == TAP_COUNT_TWO) {
        HILOG_INFO("Triple down detected");
        isTripleDown_ = true;
    }

    if (upEventCount >= TRIPLE_TAP_COUNT) {
        HILOG_DEBUG("Triple tap detected");
        isTripleDown_ = false;
        return true;
    }

    return false;
}

int64_t MockZoomGesture::CalcIntervalTime(std::shared_ptr<MMI::PointerEvent> firstEvent,
    std::shared_ptr<MMI::PointerEvent> secondEvent)
{
    HILOG_DEBUG();

    if (!firstEvent || !secondEvent) {
        HILOG_DEBUG("The event is null");
        return 0;
    }

    int64_t firstTime = firstEvent->GetActionTime();
    int64_t secondTime = secondEvent->GetActionTime();
    int64_t intervalTime = (secondTime - firstTime) / US_TO_MS;

    return intervalTime;
}

float MockZoomGesture::CalcSeparationDistance(std::shared_ptr<MMI::PointerEvent> firstEvent,
    std::shared_ptr<MMI::PointerEvent> secondEvent)
{
    HILOG_DEBUG();

    if (!firstEvent || !secondEvent) {
        HILOG_DEBUG("The event is null");
        return 0;
    }

    MMI::PointerEvent::PointerItem firstItem;
    MMI::PointerEvent::PointerItem secondItem;
    firstEvent->GetPointerItem(firstEvent->GetPointerId(), firstItem);
    secondEvent->GetPointerItem(secondEvent->GetPointerId(), secondItem);
    int32_t durationX = secondItem.GetDisplayX() - firstItem.GetDisplayX();
    int32_t durationY = secondItem.GetDisplayY() - firstItem.GetDisplayY();
    float distance = static_cast<float>(hypot(durationX, durationY));

    return distance;
}

void MockZoomGesture::OnTripleTap(MMI::PointerEvent &event)
{
    HILOG_INFO("state_ is %{public}d.", state_);

    switch (state_) {
        case READY_STATE: {
            TransferState(ZOOMIN_STATE);
            int32_t pointerId = event.GetPointerId();
            MMI::PointerEvent::PointerItem item;
            event.GetPointerItem(pointerId, item);
            int32_t anchorX = item.GetDisplayX();
            int32_t anchorY = item.GetDisplayY();
            HILOG_DEBUG("anchorX:%{private}d, anchorY:%{private}d.", anchorX, anchorY);
            OnZoom(anchorX, anchorY, true);
            break;
        }
        case ZOOMIN_STATE:
            TransferState(READY_STATE);
            OffZoom();
            break;
        default:
            break;
    }

    ClearCacheEventsAndMsg();
}

MockZoomGesture::ZoomGestureEventHandler::ZoomGestureEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    MockZoomGesture &zoomGesture): AppExecFwk::EventHandler(runner), zoomGesture_(zoomGesture)
{
    HILOG_DEBUG();
}

void MockZoomGesture::ZoomGestureEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    uint32_t eventId = event->GetInnerEventId();
    
    switch (eventId) {
        case MULTI_TAP_MSG:
            HILOG_DEBUG("process multi tap msg.");
            if (zoomGesture_.isTripleDown_) {
                zoomGesture_.OnDrag();
            } else {
                zoomGesture_.SendCacheEventsToNext();
            }
            break;
        default:
            break;
    }
}

void MockZoomGesture::GetWindowParam(bool needRefresh)
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
        screenWidth_ = static_cast<uint32_t>(display->GetWidth());
        screenHeight_ = static_cast<uint32_t>(display->GetHeight());
        HILOG_INFO("screenWidth_ = %{public}d, screenHeight_ = %{public}d.", screenWidth_, screenHeight_);
    }
    screenSpan_ = hypot(screenWidth_, screenHeight_);
#else
    HILOG_INFO("not support zoom");
#endif
}

void MockZoomGesture::StartMagnificationInteract()
{
    HILOG_INFO();
    TransferState(ZOOMIN_STATE);
}

void MockZoomGesture::DisableGesture()
{
    HILOG_INFO();
    TransferState(READY_STATE);
}

void MockZoomGesture::OnZoom(int32_t anchorX, int32_t anchorY, bool showMenu)
{
    HILOG_INFO();
    centerX_ = anchorX;
    centerY_ = anchorY;
    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    fullScreenManager_->EnableMagnification(anchorX, anchorY);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
    if (showMenu && menuManager_ != nullptr) {
        menuManager_->ShowMenuWindow(FULL_SCREEN_MAGNIFICATION);
    }
    Utils::RecordOnZoomGestureEvent("on", true);
}

void MockZoomGesture::OffZoom()
{
    HILOG_INFO();
    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    if (fullScreenManager_->IsMagnificationWindowShow()) {
        HILOG_INFO("full magnification disable.");
        fullScreenManager_->DisableMagnification(false);
        menuManager_->DisableMenuWindow();
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
    }
    Utils::RecordOnZoomGestureEvent("off", true);
}

void MockZoomGesture::OnScroll(float offsetX, float offsetY)
{
    HILOG_DEBUG("offsetX:%{public}f, offsetY:%{public}f.", offsetX, offsetY);

    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    fullScreenManager_->MoveMagnification(static_cast<int32_t>(offsetX), static_cast<int32_t>(offsetY));

#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    AccessibilityPowerManager &powerMgr = Singleton<AccessibilityPowerManager>::GetInstance();
    powerMgr.RefreshActivity();
#endif
}

void MockZoomGesture::OnScale(float scaleSpan)
{
    HILOG_DEBUG();
    if (fullScreenManager_ == nullptr) {
        HILOG_ERROR("fullScreenManager_ is nullptr.");
        return;
    }
    fullScreenManager_->SetScale(scaleSpan);
}

void MockZoomGesture::Clear()
{
    HILOG_DEBUG();
    SendCacheEventsToNext();
    TransferState(READY_STATE);
}

void MockZoomGesture::DestroyEvents()
{
    HILOG_INFO();
    Clear();
    OffZoom();
    EventTransmission::DestroyEvents();
}

void MockZoomGesture::ShieldZoomGesture(bool state)
{
    shieldZoomGestureFlag_ = state;
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    HILOG_INFO("ShieldZoomGesture state = %{public}d", state);
    if (state) {
        Clear();
        menuManager_->DisableMenuWindow();
        if (fullScreenManager_ == nullptr) {
            HILOG_ERROR("fullScreenManager_ is nullptr.");
            return;
        }
        fullScreenManager_->DisableMagnification(true);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
    }
}

void MockZoomGesture::OnDrag()
{
    TransferState(DRAGGING_STATE);
    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    MMI::PointerEvent::PointerItem item;
    lastDownEvent_->GetPointerItem(lastDownEvent_->GetPointerId(), item);

    int32_t anchorX = item.GetDisplayX();
    int32_t anchorY = item.GetDisplayY();
    OnZoom(anchorX, anchorY, false);
    lastScrollFocusX_ = static_cast<float>(anchorX);
    lastScrollFocusY_ = static_cast<float>(anchorY);
    isTripleDown_ = false;
    ClearCacheEventsAndMsg();
}

RetError MockAbilityChannel::SearchElementInfoByAccessibilityId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter, bool systemApi)
{
    int32_t treeId = elementBasicInfo.treeId;
    int32_t windowId = elementBasicInfo.windowId;
    int64_t elementId = elementBasicInfo.elementId;
    HILOG_DEBUG("elementId:%{public}" PRId64 " winId: %{public}d treeId: %{public}d", elementId, windowId, treeId);
    if (systemApi && !Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        HILOG_WARN("SearchElementInfoByAccessibilityId permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, windowId, elementId, treeId, requestId,
        callback, mode, isFilter]() {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, windowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK || !CheckWinFromAwm(windowId)) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", windowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (windowId == SCENE_BOARD_WINDOW_ID && awm.IsInnerWindowRootElement(elementId)) {
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            HILOG_DEBUG("IsInnerWindowRootElement elementId: %{public}" PRId64 "", elementId);
        } else {
            int64_t realElementId = awm.GetSceneBoardElementId(windowId, elementId);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId,
                requestId, callback);
            RetError ret = elementOperator->SearchElementInfoByAccessibilityId(realElementId, requestId,
                callback, mode, isFilter);
            if (ret != RET_OK) {
                HILOG_ERROR("SearchElementInfoByAccessibilityId IPC Failed.");
                std::vector<AccessibilityElementInfo> infos = {};
                callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
                syncPromise->set_value(ret);
                return;
            }
            HILOG_DEBUG("MockAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfoByAccessibilityId");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfoByAccessibilityId result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError MockAbilityChannel::SearchDefaultFocusedByWindowId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter)
{
    int32_t windowId = elementBasicInfo.windowId;
    int64_t elementId = elementBasicInfo.elementId;
    int32_t treeId = elementBasicInfo.treeId;
    HILOG_DEBUG("elementId:%{public}" PRId64 " winId: %{public}d treeId: %{public}d", elementId, windowId, treeId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
 
    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }
 
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, windowId, elementId, treeId, requestId,
        callback, mode, isFilter]() {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, windowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK || !CheckWinFromAwm(windowId)) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", windowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchDefaultFocusByWindowIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }
 
        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (windowId == SCENE_BOARD_WINDOW_ID && awm.IsInnerWindowRootElement(elementId)) {
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchDefaultFocusByWindowIdResult(infos, requestId);
        } else {
            Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId,
                requestId, callback);
            elementOperator->SearchDefaultFocusedByWindowId(windowId, requestId, callback, mode, isFilter);
            HILOG_DEBUG("MockAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfoByAccessibilityId");
 
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfoByAccessibilityId result");
        return RET_ERR_TIME_OUT;
    }
    return RET_OK;
}

RetError MockAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi)
{
    HILOG_DEBUG("SearchElementInfosByText :channel SearchElementInfo elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    if (systemApi && !Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        return RET_ERR_NO_PERMISSION;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_ == nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("SearchElementInfosByText :channel SearchElementInfo treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId, text,
        requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }
        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->SearchElementInfosByText(realElementId, text, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfosByText");
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfosByText result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError MockAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi)
{
    HILOG_DEBUG("channel FindFocusedElementInfo elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    if (systemApi && !Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        return RET_ERR_NO_PERMISSION;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_ == nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("FindFocusedElementInfo :channel FindFocusedElementInfo treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId,
        focusType, requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, focusType,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }
        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->FindFocusedElementInfo(realElementId, focusType, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "FindFocusedElementInfo");
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait FindFocusedElementInfo result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError MockAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    bool systemApi)
{
    HILOG_DEBUG("FocusMoveSearch :channel FocusMoveSearch elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    if (systemApi && !Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        return RET_ERR_NO_PERMISSION;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_ == nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("FocusMoveSearch :channel FocusMoveSearch treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId,
        elementId, treeId, direction, requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }
        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->FocusMoveSearch(realElementId, direction, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "FocusMoveSearch");
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait FocusMoveSearch result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void MockAbilityChannel::SetKeyCodeToMmi(std::shared_ptr<MMI::KeyEvent>& keyEvent, const bool isPress,
    const int32_t keyCode)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (keyEvent == nullptr) {
        HILOG_ERROR("KeyEvent is nullptr");
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(isPress);
    item.SetKeyCode(keyCode);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(keyCode);
}

RetError MockAbilityChannel::TransmitActionToMmi(const int32_t action)
{
    HILOG_DEBUG("The action is %{public}d", action);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    std::shared_ptr<MMI::KeyEvent> keyEventUp = MMI::KeyEvent::Create();
    std::shared_ptr<MMI::KeyEvent> keyEventDown = MMI::KeyEvent::Create();
    if (keyEventUp == nullptr || keyEventDown == nullptr) {
        HILOG_ERROR("KeyEvent is nullptr");
        return RET_ERR_NULLPTR;
    }

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is nullptr");
        return RET_ERR_NULLPTR;
    }
    
    HILOG_INFO("Transmit keycode to MMI");

    if (accessibleKeyCodeTable.at(action).first) {
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventUp, false, accessibleKeyCodeTable.at(action).second.first);
    } else {
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventUp, false, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.second);
    }
    keyEventDown->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    keyEventUp->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    inputManager_->SimulateInputEvent(keyEventDown);
    inputManager_->SimulateInputEvent(keyEventUp);

    return RET_OK;
}

RetError MockAbilityChannel::EnableScreenCurtain(bool isEnable)
{
    HILOG_DEBUG();
    auto& aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.PostDelayUnloadTask();
    return aams.SetCurtainScreenUsingStatus(isEnable);
}

RetError MockAbilityChannel::HoldRunningLock()
{
    HILOG_DEBUG();
    if (!Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        HILOG_WARN("HoldRunningLock permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    std::string bundleName = "";
    if (!Utils::GetBundleNameByCallingUid(bundleName)) {
        return RET_ERR_FAILED;
    }
    if (!Singleton<AccessibilityPowerManager>::GetInstance().HoldRunningLock(bundleName)) {
        HILOG_ERROR("Failed to hold running lock.");
        return RET_ERR_FAILED;
    }
#endif
    return RET_OK;
}

RetError MockAbilityChannel::UnholdRunningLock()
{
    HILOG_DEBUG();
    if (!Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        HILOG_WARN("HoldRunningLock permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    std::string bundleName = "";
    if (!Utils::GetBundleNameByCallingUid(bundleName)) {
        return RET_ERR_FAILED;
    }
    auto &powerManager = Singleton<AccessibilityPowerManager>::GetInstance();
    if (!powerManager.UnholdRunningLock(bundleName)) {
        HILOG_ERROR("Failed to unhold running lock.");
        return RET_ERR_FAILED;
    }
#endif
    return RET_OK;
}

RetError MockAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("ExecuteAction elementId:%{public}" PRId64 " winId:%{public}d, action:%{public}d, requestId:%{public}d",
        elementId, accessibilityWindowId, action, requestId);
    if (actionArguments.find("sysapi_check_perm") != actionArguments.end()) {
        if (!Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
            OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
            HILOG_WARN("system api permission denied.");
            return RET_ERR_NO_PERMISSION;
        }
    }

    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }

    if (accessibleKeyCodeTable.find(action) != accessibleKeyCodeTable.end()) {
        RetError ret = TransmitActionToMmi(action);
        if (ret != RET_OK) {
            HILOG_ERROR("Transmit Action To Mmi failed!");
            callback->SetExecuteActionResult(false, requestId);
            return RET_ERR_FAILED;
        }
        callback->SetExecuteActionResult(true, requestId);
        return RET_OK;
    }
    return ExecuteActionAsync(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
}

RetError MockAbilityChannel::ExecuteActionAsync(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    SetFocusWindowIdAndElementId(accessibilityWindowId, elementId, action);
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId, action,
        actionArguments, requestId, callback]() {
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->ExecuteAction(realElementId, action, actionArguments, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "ExecuteAction");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_1000MS));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait ExecuteAction result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void MockAbilityChannel::SetFocusWindowIdAndElementId(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t action)
{
    auto& awm = Singleton<AccessibleAbilityManagerService>::GetInstance();
    if ((action == ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS)) {
        awm.SetFocusWindowId(accessibilityWindowId);
        awm.SetFocusElementId(elementId);
    } else if (action == ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS) {
        awm.SetFocusWindowId(WINDOW_ID_INVALID);
        awm.SetFocusElementId(ELEMENT_ID_INVALID);
    }
}

RetError MockAbilityChannel::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("windowId:%{public}d", windowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
 
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    std::shared_ptr<AccessibilityWindowInfo> tmpWindowInfo = std::make_shared<AccessibilityWindowInfo>(windowInfo);
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, windowId, tmpWindowInfo, syncPromise]() {
        HILOG_DEBUG("windowId:%{public}d", windowId);
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }
        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("MockAbilityChannel::GetWindow failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }

        if (Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindow(windowId, *tmpWindowInfo)) {
            syncPromise->set_value(RET_OK);
        } else {
            syncPromise->set_value(RET_ERR_NO_WINDOW_CONNECTION);
        }
        }, "GetWindow");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait GetWindow result");
        return RET_ERR_TIME_OUT;
    }

    windowInfo = *tmpWindowInfo;
    return syncFuture.get();
}

RetError MockAbilityChannel::GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    uint64_t displayId = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplayId();
    HILOG_DEBUG("default display id is %{public}" PRIu64 "", displayId);
    return GetWindows(displayId, windows, systemApi);
#else
    HILOG_DEBUG("not support display manager");
    return GetWindows(0, windows, systemApi);
#endif
}

RetError MockAbilityChannel::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows, bool systemApi)
{
    HILOG_DEBUG();
    return GetWindows(displayId, windows, systemApi);
}

RetError MockAbilityChannel::GetWindows(uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows,
    bool systemApi) const
{
    if (systemApi && !Singleton<AccessibleAbilityManagerService>::GetInstance().CheckPermission(
        OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY)) {
        return RET_ERR_NO_PERMISSION;
    }
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    auto tmpWindows = std::make_shared<std::vector<AccessibilityWindowInfo>>(windows);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, displayId, tmpWindows, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }
        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("GetWindows failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }
        std::vector<AccessibilityWindowInfo> windowInfos =
            Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
        for (auto &window : windowInfos) {
            if (window.GetDisplayId() == displayId) {
                tmpWindows->emplace_back(window);
            }
        }
#else
        for (auto &window : windowInfos) {
            tmpWindows->emplace_back(window);
        }
#endif
        syncPromise->set_value(RET_OK);
        }, "GetWindows");
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait GetWindows result");
        return RET_ERR_TIME_OUT;
    }
    windows = *tmpWindows;
    return syncFuture.get();
}

void MockAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, handled, sequence]() {
        sptr<KeyEventFilter> keyEventFilter =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
        if (!keyEventFilter) {
            return;
        }

        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            return;
        }
        keyEventFilter->SetServiceOnKeyEventResult(*clientConnection, handled, sequence);
        }, "SetOnKeyPressEventResult");
}

RetError MockAbilityChannel::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("GetCursorPosition :channel GetCursorPosition elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("GetCursorPosition :channel GetCursorPosition treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId,
        requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->GetCursorPosition(realElementId, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "GetCursorPosition");
    
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait GetCursorPosition result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError MockAbilityChannel::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_INFO();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, gesturePath, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
            HILOG_ERROR("MockAbilityChannel::SendSimulateGesture failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }

        sptr<TouchEventInjector> touchEventInjector =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            syncPromise->set_value(RET_ERR_NO_INJECTOR);
            return;
        }
        touchEventInjector->InjectEvents(gesturePath);
        syncPromise->set_value(RET_OK);
        }, "SendSimulateGesture");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait SendSimulateGesture result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError MockAbilityChannel::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, targetBundleNames, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        clientConnection->SetAbilityInfoTargetBundleName(targetBundleNames);
        syncPromise->set_value(RET_OK);
        }, "SetTargetBundleName");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait SetTargetBundleName result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

sptr<AccessibleAbilityConnection> MockAbilityChannel::GetConnection(
    int32_t accountId, const std::string &clientName)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }

    HILOG_DEBUG("accountId[%{public}d] clientName[%{public}s]", accountId, clientName.c_str());
    return accountData->GetAccessibleAbilityConnection(clientName);
}

RetError MockAbilityChannel::GetElementOperator(
    int32_t accountId, int32_t windowId, int32_t focusType, const std::string &clientName,
    sptr<IAccessibilityElementOperator> &elementOperator, const int32_t treeId)
{
    HILOG_DEBUG();
    elementOperator = nullptr;
    sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
    if (!clientConnection) {
        HILOG_ERROR("There is no client connection");
        return RET_ERR_NO_CONNECTION;
    }
    if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("the client has no retieve capability");
        return RET_ERR_NO_CAPABILITY;
    }

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);
    sptr<AccessibilityWindowConnection> connection =  nullptr;
    connection = accountData->GetAccessibilityWindowConnection(realId);
    if (connection == nullptr) {
        HILOG_ERROR("windowId[%{public}d] has no connection", realId);
        return RET_ERR_NO_WINDOW_CONNECTION;
    }
    if (treeId <= 0) {
        elementOperator = connection->GetProxy();
    } else {
        elementOperator = connection->GetCardProxy(treeId);
    }
    if (!elementOperator) {
        HILOG_ERROR("The proxy of window connection is nullptr");
        return RET_ERR_NULLPTR;
    }
    return RET_OK;
}

bool MockAbilityChannel::CheckWinFromAwm(const int32_t windowId)
{
    std::vector<AccessibilityWindowInfo> windowsFromAwm =
        Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
    if (!windowsFromAwm.empty()) {
        for (const auto& window: windowsFromAwm) {
            if (windowId == window.GetWindowId()) {
                return true;
            }
        }
    }
    return false;
}

RetError MockAbilityChannel::SetIsRegisterDisconnectCallback(bool isRegister)
{
    HILOG_INFO();
    sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId_, clientName_);
    if (clientConnection == nullptr) {
        HILOG_ERROR("GetConnection failed, clientName: %{public}s", clientName_.c_str());
        return RET_ERR_NULLPTR;
    }
    clientConnection->SetIsRegisterDisconnectCallback(isRegister);
    return RET_OK;
}

RetError MockAbilityChannel::NotifyDisconnect()
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    sptr<AccessibleAbilityConnection> clientConnection = accountData->GetWaitDisConnectAbility(clientName_);
    if (clientConnection == nullptr) {
        HILOG_ERROR("GetConnection failed, clientName: %{public}s", clientName_.c_str());
        return RET_ERR_NULLPTR;
    }
    accountData->RemoveWaitDisconnectAbility(clientName_);
    clientConnection->NotifyDisconnect();
    return RET_OK;
}

RetError MockAbilityChannel::ConfigureEvents(const std::vector<uint32_t> needEvents)
{
    HILOG_INFO();
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateUITestConfigureEvents(needEvents);
    if (ret != RET_OK) {
        HILOG_ERROR("Configure Events failed!");
        return RET_ERR_FAILED;
    }
    return RET_OK;
}

void MockAbilityChannel::SearchElementInfoBySpecificProperty(const ElementBasicInfo elementBasicInfo,
    const SpecificPropertyParam& param, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    int32_t treeId = elementBasicInfo.treeId;
    int32_t windowId = elementBasicInfo.windowId;
    int64_t elementId = elementBasicInfo.elementId;
    HILOG_DEBUG("elementId:%{public}" PRId64 " winId: %{public}d treeId: %{public}d "
        "propertyTarget:%{public}s propertyType:%{public}u",
        elementId, windowId, treeId, param.propertyTarget.c_str(), static_cast<uint32_t>(param.propertyType));
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, windowId, elementId, treeId, requestId,
        callback, param]() {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, windowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK || !CheckWinFromAwm(windowId)) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", windowId);
            std::list<AccessibilityElementInfo> infos = {};
            std::list<AccessibilityElementInfo> treeInfos = {};
            callback->SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (windowId == SCENE_BOARD_WINDOW_ID && awm.IsInnerWindowRootElement(elementId)) {
            std::list<AccessibilityElementInfo> infos = {};
            std::list<AccessibilityElementInfo> treeInfos = {};
            callback->SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
            HILOG_DEBUG("IsInnerWindowRootElement elementId: %{public}" PRId64 "", elementId);
        } else {
            int64_t realElementId = awm.GetSceneBoardElementId(windowId, elementId);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId,
                requestId, callback);
            elementOperator->SearchElementInfoBySpecificProperty(realElementId, param, requestId, callback);
            HILOG_DEBUG("MockAbilityChannel::SearchElementInfosBySpecificProperty successfully");
        }
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfosBySpecificProperty");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfosBySpecificProperty result");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS
