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

#include "accessibility_zoom_gesture.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "window_accessibility_controller.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t POINTER_COUNT_1 = 1;
    constexpr float TAP_MIN_DISTANCE = 8.0f;
    constexpr int32_t MULTI_TAP_TIMER = 300; // ms
    constexpr int32_t LONG_PRESS_TIMER = 500; // ms
    constexpr int64_t US_TO_MS = 1000;
    constexpr float MIN_SCALE_SPAN = 26.0f;
    constexpr float DOUBLE_TAP_SLOP = 100.0f;
    constexpr float HALF = 0.5f;
    constexpr uint32_t TRIPLE_TAP_COUNT = 3;
    constexpr float DEFAULT_SCALE = 2.0f;
} // namespace

AccessibilityZoomGesture::AccessibilityZoomGesture()
{
    HILOG_DEBUG();

    zoomGestureEventHandler_ = std::make_shared<ZoomGestureEventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner(), *this);

    tapDistance_ = TAP_MIN_DISTANCE;

    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }

    float densityPixels = display->GetVirtualPixelRatio();
    multiTapDistance_ = densityPixels * DOUBLE_TAP_SLOP + 0.5f;
}

bool AccessibilityZoomGesture::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("state_ is %{public}d.", state_);

    int32_t sourceType = event.GetSourceType();
    if (sourceType != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
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
        default:
            break;
    }
    return true;
}

void AccessibilityZoomGesture::TransferState(ACCESSIBILITY_ZOOM_STATE state)
{
    HILOG_DEBUG("state:%{public}d.", state);

    state_ = state;
}

void AccessibilityZoomGesture::CacheEvents(MMI::PointerEvent &event)
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
        default:
            HILOG_DEBUG("Action is %{public}d", action);
            break;
    }
    cacheEvents_.emplace_back(pointerEvent);
}

void AccessibilityZoomGesture::SendCacheEventsToNext()
{
    HILOG_DEBUG();

    bool isStartNewAction = false;
    int32_t action = MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    std::vector<std::shared_ptr<MMI::PointerEvent>> cacheEventsTmp;
    std::copy(cacheEvents_.begin(), cacheEvents_.end(), std::back_inserter(cacheEventsTmp));

    ClearCacheEventsAndMsg();

    size_t cacheEventsNum = 0;
    size_t cacheEventsTotalNum = cacheEventsTmp.size();
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
        } else {
            EventTransmission::OnPointerEvent(*pointerEvent);
        }
    }
}

void AccessibilityZoomGesture::ClearCacheEventsAndMsg()
{
    HILOG_DEBUG();

    cacheEvents_.clear();
    preLastDownEvent_ = nullptr;
    lastDownEvent_ = nullptr;
    preLastUpEvent_ = nullptr;
    lastUpEvent_ = nullptr;
    zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
}

void AccessibilityZoomGesture::RecognizeInReadyState(MMI::PointerEvent &event)
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
            } else {
                SendCacheEventsToNext();
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if ((pointerCount == POINTER_COUNT_1) && IsUpValid()) {
                isTripleTaps = IsTripleTaps();
            } else {
                SendCacheEventsToNext();
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            SendCacheEventsToNext();
            break;
        default:
            break;
    }

    if (isTripleTaps) {
        OnTripleTaps(event);
    }
}

void AccessibilityZoomGesture::RecognizeInZoomState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    bool isTripleTaps = false;

    HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            zoomGestureEventHandler_->RemoveEvent(MULTI_TAP_MSG);
            if (pointerCount == POINTER_COUNT_1) {
                if (IsDownValid()) {
                    zoomGestureEventHandler_->SendEvent(MULTI_TAP_MSG, 0, MULTI_TAP_TIMER);
                } else {
                    SendCacheEventsToNext();
                }
            } else if (pointerCount > POINTER_COUNT_1) {
                TransferState(SLIDING_STATE);
                ClearCacheEventsAndMsg();
                ZOOM_FOCUS_COORDINATE focusXY = {0.0f, 0.0f};
                CalcFocusCoordinate(event, focusXY);
                // Used for scroll algorithm.
                lastScrollFocusX_ = focusXY.centerX;
                lastScrollFocusY_ = focusXY.centerY;

                // Used for scale algorithm.
                float span = CalcScaleSpan(event, focusXY);
                if (span >= MIN_SCALE_SPAN) {
                    startScaling_ = true;
                    preSpan_ = lastSpan_ = span;
                }
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if ((pointerCount == POINTER_COUNT_1) && IsUpValid()) {
                isTripleTaps = IsTripleTaps();
            } else {
                SendCacheEventsToNext();
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            SendCacheEventsToNext();
            break;
        default:
            break;
    }

    if (isTripleTaps) {
        OnTripleTaps(event);
    }
}

void AccessibilityZoomGesture::RecognizeInSlidingState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();

    // Recognize scroll and zoom gestures.
    RecognizeScroll(event);
    RecognizeScale(event);

    HILOG_DEBUG("action:%{public}d, pointerCount:%{public}zu", action, pointerCount);
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_UP:
            if (pointerCount == POINTER_COUNT_1) {
                TransferState(ZOOMIN_STATE);
            }
            break;
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            TransferState(ZOOMIN_STATE);
            break;
        default:
            break;
    }
}

void AccessibilityZoomGesture::RecognizeScroll(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    ZOOM_FOCUS_COORDINATE coordinate = {0.0f, 0.0f};
    CalcFocusCoordinate(event, coordinate);

    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_UP:
            lastScrollFocusX_ = coordinate.centerX;
            lastScrollFocusY_ = coordinate.centerY;
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            float offsetX = coordinate.centerX - lastScrollFocusX_;
            float offsetY = coordinate.centerY - lastScrollFocusY_;
            if ((abs(offsetX) > 1) || (abs(offsetY) > 1)) {
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

void AccessibilityZoomGesture::RecognizeScale(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetPointerAction();
    size_t pointerCount = event.GetPointerIds().size();
    if (((action == MMI::PointerEvent::POINTER_ACTION_UP) && (pointerCount == POINTER_COUNT_1)) ||
        (action == MMI::PointerEvent::POINTER_ACTION_CANCEL)) {
        HILOG_DEBUG("Scaling is end");
        startScaling_ = false;
        preSpan_ = lastSpan_ = 0;
        return;
    }

    ZOOM_FOCUS_COORDINATE focusXY = {0.0f, 0.0f};
    CalcFocusCoordinate(event, focusXY);
    float span = CalcScaleSpan(event, focusXY);
    if (!startScaling_) {
        // When the span is greater than or equal to MIN_SCALE_SPAN, start scaling.
        if (span >= MIN_SCALE_SPAN) {
            startScaling_ = true;
            preSpan_ = lastSpan_ = span;
        }
    } else {
        // When the span is smaller than the MIN_SCALE_SPAN,
        // the scale recognition will be restarted.
        if (span < MIN_SCALE_SPAN) {
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
    
    lastSpan_ = span;
    float ratio = lastSpan_ / preSpan_;
    if (ratio != 1) {
        OnScale(ratio, focusXY.centerX, focusXY.centerY);
        preSpan_ = lastSpan_;
    }
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
        sumX += static_cast<float>(item.GetDisplayX());
        sumY += static_cast<float>(item.GetDisplayY());
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
        sumSpanX += static_cast<float>(abs(item.GetDisplayX() - coordinate.centerX));
        sumSpanY += static_cast<float>(abs(item.GetDisplayY() - coordinate.centerY));
    }

    float spanX = sumSpanX / count;
    float spanY = sumSpanY / count;
    span = hypot(spanX, spanY) / HALF;
    HILOG_DEBUG("The span is %{public}f", span);
    return span;
}

bool AccessibilityZoomGesture::IsDownValid()
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

bool AccessibilityZoomGesture::IsUpValid()
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

bool AccessibilityZoomGesture::IsTripleTaps()
{
    HILOG_DEBUG();

    uint32_t upEventCount = 0;
    int32_t action = MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    for (auto &pointerEvent : cacheEvents_) {
        action = pointerEvent->GetPointerAction();
        if (action == MMI::PointerEvent::POINTER_ACTION_UP) {
            upEventCount++;
        }
    }

    if (upEventCount >= TRIPLE_TAP_COUNT) {
        HILOG_DEBUG("Triple tap detected");
        return true;
    }

    return false;
}

int64_t AccessibilityZoomGesture::CalcIntervalTime(std::shared_ptr<MMI::PointerEvent> firstEvent,
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

float AccessibilityZoomGesture::CalcSeparationDistance(std::shared_ptr<MMI::PointerEvent> firstEvent,
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

void AccessibilityZoomGesture::OnTripleTaps(MMI::PointerEvent &event)
{
    HILOG_DEBUG("state_ is %{public}d.", state_);

    switch (state_) {
        case READY_STATE: {
            TransferState(ZOOMIN_STATE);
            int32_t pointerId = event.GetPointerId();
            MMI::PointerEvent::PointerItem item;
            event.GetPointerItem(pointerId, item);
            int32_t anchorX = item.GetDisplayX();
            int32_t anchorY = item.GetDisplayY();
            OnZoom(anchorX, anchorY);
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

AccessibilityZoomGesture::ZoomGestureEventHandler::ZoomGestureEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityZoomGesture &zoomGesture): AppExecFwk::EventHandler(runner), zoomGesture_(zoomGesture)
{
    HILOG_DEBUG();
}

void AccessibilityZoomGesture::ZoomGestureEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    uint32_t eventId = event->GetInnerEventId();
    
    switch (eventId) {
        case MULTI_TAP_MSG:
            zoomGesture_.SendCacheEventsToNext();
            break;
        default:
            break;
    }
}

void AccessibilityZoomGesture::OnZoom(int32_t anchorX, int32_t anchorY)
{
    HILOG_DEBUG("anchorX:%{public}d, anchorY:%{public}d.", anchorX, anchorY);

    OHOS::Rosen::WindowAccessibilityController::GetInstance().SetAnchorAndScale(anchorX, anchorY, DEFAULT_SCALE);
}

void AccessibilityZoomGesture::OffZoom()
{
    HILOG_DEBUG();

    OHOS::Rosen::WindowAccessibilityController::GetInstance().OffWindowZoom();
}

void AccessibilityZoomGesture::OnScroll(float offsetX, float offsetY)
{
    HILOG_DEBUG("offsetX:%{public}f, offsetY:%{public}f.", offsetX, offsetY);

    OHOS::Rosen::WindowAccessibilityController::GetInstance().SetAnchorOffset(
        static_cast<int32_t>(offsetX), static_cast<int32_t>(offsetY));
}

void AccessibilityZoomGesture::OnScale(float scaleRatio, float focusX, float focusY)
{
    HILOG_DEBUG("scaleRatio:%{public}f, focusX:%{public}f, focusY:%{public}f.", scaleRatio, focusX, focusY);

    OHOS::Rosen::WindowAccessibilityController::GetInstance().SetAnchorAndScale(
        static_cast<int32_t>(focusX), static_cast<int32_t>(focusY), scaleRatio);
}
} // namespace Accessibility
} // namespace OHOS
