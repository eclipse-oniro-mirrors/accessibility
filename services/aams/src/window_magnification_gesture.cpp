/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

// LCOV_EXCL_START
#include "window_magnification_gesture.h"
#include "utils.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"
#include "accessibility_window_manager.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "accessibility_power_manager.h"
#endif

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr float TAP_MIN_DISTANCE = 8.0f;
    constexpr float DOUBLE_TAP_SLOP = 100.0f;
    constexpr int32_t HANDLER = 0;
    constexpr int32_t SWITCH_MENU = -1;
    constexpr uint32_t TAP_COUNT_THREE = 3;
} // namespace

void WindowMagnificationGesture::InitGestureFuncMap()
{
    handleEventFuncMap_ = {
        {MagnificationGestureState::READY_STATE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleReadyStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleReadyStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleReadyStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleReadyStateCancel)},
            }
        },
        {MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleReadyStateOneFingerDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleReadyStateOneFingerDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleReadyStateOneFingerDownStateMove)},
            {MMI::PointerEvent::POINTER_ACTION_CANCEL, BIND(HandleReadyStateOneFingerDownStatCancel)},
            }
        },
        {MagnificationGestureState::READY_STATE_ONE_FINGER_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleReadyStateOneFingerTapDown)},
            }
        },
        {MagnificationGestureState::ZOOMIN_STATE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleZoomInStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleZoomInStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleZoomInStateMove)},
            }
        },
        {MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleZoomInStateOneFingerDownStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleZoomInStateOneFingerDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleZoomInStateOneFingerDownStateMove)},
            }
        },
        {MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_TAP, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleZoomInStateOneFingerTapDown)},
            }
        },
        {MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN, {
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleTwoFingersDownStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleTwoFingersDownStateMove)},
            }
        },
        {MagnificationGestureState::SLIDING_STATE, {
            {MMI::PointerEvent::POINTER_ACTION_DOWN, BIND(HandleSlidingStateDown)},
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleSlidingStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleSlidingStateMove)},
            }
        },
        {MagnificationGestureState::MENU_SLIDING_STATE, {
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleMenuSlidingStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleMenuSlidingStateMove)},
            }
        },
        {MagnificationGestureState::DRAGGING_STATE, {
            {MMI::PointerEvent::POINTER_ACTION_UP, BIND(HandleDraggingStateUp)},
            {MMI::PointerEvent::POINTER_ACTION_MOVE, BIND(HandleDraggingStateMove)},
            }
        }
    };
}

MagnificationGestureEventHandler::MagnificationGestureEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    WindowMagnificationGesture &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

void MagnificationGestureEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    MagnificationGestureMsg msg = static_cast<MagnificationGestureMsg>(event->GetInnerEventId());
    int64_t param = event->GetParam();
    HILOG_DEBUG("handler process event id = %{public}d, GestureState is %{public}d, param = %{public}d",
        event->GetInnerEventId(), server_.GetGestureState(), static_cast<int32_t>(param));
    switch (msg) {
        case MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG:
            if (server_.GetTripleDown()) {
                server_.OnDrag();
            } else {
                if (!server_.GetTapOnMenu()) {
                    server_.SendCacheEventsToNext();
                    server_.ResetTapCount();
                }
                // If this event is not canceled within 100ms, it is cleared.
                server_.SetGestureState(static_cast<MagnificationGestureState>(event->GetParam()), HANDLER);
            }
            break;
        case MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG:
            // If no next up comes in more than 250ms, clear it.
            server_.SendCacheEventsToNext();
            server_.ResetTapCount();
            server_.SetGestureState(static_cast<MagnificationGestureState>(event->GetParam()), HANDLER);
            break;
        case MagnificationGestureMsg::LONG_PRESS_MSG:
            // If it is not cleared within 200ms, it indicates that the current action is a long press.
            if (server_.GetTapOnMenu()) {
                server_.SetGestureState(MagnificationGestureState::MENU_SLIDING_STATE, HANDLER);
            } else {
                server_.SetGestureState(MagnificationGestureState::SLIDING_STATE, HANDLER);
            }
            break;
        default:
            break;
    }
}

WindowMagnificationGesture::WindowMagnificationGesture(
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager,
    std::shared_ptr<MagnificationMenuManager> menuManager)
    : windowMagnificationManager_(windowMagnificationManager), menuManager_(menuManager)
{
    HILOG_DEBUG();
    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    handler_ = std::make_shared<MagnificationGestureEventHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }

    InitGestureFuncMap();

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (display == nullptr) {
        HILOG_ERROR("get display is nullptr");
        return;
    }
    moveThreshold_ = CalculateMoveThreshold(display->GetDpi());
    float densityPixels = display->GetVirtualPixelRatio();
    multiTapDistance_ = densityPixels * DOUBLE_TAP_SLOP + 0.5f;
    tapDistance_ = TAP_MIN_DISTANCE;
#else
    HILOG_DEBUG("not support display manager");
    moveThreshold_ = 1;
    multiTapDistance_ = 1 * DOUBLE_TAP_SLOP + 0.5f;
#endif
}

bool WindowMagnificationGesture::OnPointerEvent(MMI::PointerEvent &event)
{
    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_UP) {
        HILOG_INFO("PointerAction: %{public}d.", event.GetPointerAction());
    }

    if (needBypassPointerEvent(event)) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }
    OnPointerEventExcute(event);
    return true;
}

bool WindowMagnificationGesture::needBypassPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("shieldZoomGestureFlag %{public}d.", shieldZoomGestureFlag_);
    if (shieldZoomGestureFlag_) {
        return true;
    }
    if (event.GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        return true;
    }
    if (event.GetPointerId() == SCROLL_SHOT_POINTER_ID) {
        HILOG_DEBUG("scrollshot injected.");
        return true;
    }
    return false;
}

void WindowMagnificationGesture::OnPointerEventExcute(MMI::PointerEvent &event)
{
    if (GetGestureState() == MagnificationGestureState::PASSING_THROUGH) {
        HandlePassingThroughState(event);
        return;
    }

    auto iter = handleEventFuncMap_.find(GetGestureState());
    if (iter != handleEventFuncMap_.end()) {
        auto funcMap = iter->second.find(event.GetPointerAction());
        if (funcMap != iter->second.end()) {
            funcMap->second(event);
            return;
        }
    }
    HILOG_ERROR("no way to handle this event.");
}

void WindowMagnificationGesture::DestroyEvents()
{
    HILOG_DEBUG();
    SendCacheEventsToNext();
    ResetTapCount();
    SetGestureState(MagnificationGestureState::READY_STATE, HANDLER);
    if (windowMagnificationManager_ != nullptr &&
        windowMagnificationManager_->IsMagnificationWindowShow()) {
        HILOG_DEBUG("window gesture disable.");
        windowMagnificationManager_->DisableWindowMagnification();
        if (menuManager_ != nullptr) {
            menuManager_->DisableMenuWindow();
        }
        isSingleTapOnWindow_ = false;
        isTapOnHotArea_ = false;
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
    }
}

void WindowMagnificationGesture::CancelPostEvent(MagnificationGestureMsg msg)
{
    if (handler_->HasInnerEvent(static_cast<uint32_t>(msg))) {
        handler_->RemoveEvent(static_cast<uint32_t>(msg));
        HILOG_DEBUG("innerEventID = %{public}u canceled.", static_cast<uint32_t>(msg));
    }
}

void WindowMagnificationGesture::SendCacheEventsToNext()
{
    HILOG_DEBUG();
    for (auto &receivedEvent : receivedPointerEvents_) {
        SendEventToMultimodal(receivedEvent, false, true);
    }
    Clear();
}

void WindowMagnificationGesture::HandlePassingThroughState(MMI::PointerEvent &event)
{
    SendEventToMultimodal(event, false, false);

    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);

    // the last finger is lifted
    if ((event.GetPointerCount() == static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) &&
        (!pointerItem.IsPressed())) {
        if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->IsMagnificationWindowShow()) {
            SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
        } else {
            SetGestureState(MagnificationGestureState::READY_STATE, event.GetPointerAction());
        }
    }
}

void WindowMagnificationGesture::HandleReadyStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (IsKnuckles(event)) {
        SendEventToMultimodal(event, true, false);
        return;
    }
    int32_t size = event.GetPointerCount();
    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    if (size == static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) {
        CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
        CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
        receivedPointerEvents_.push_back(event);
        SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, event.GetPointerAction());
        handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG),
            static_cast<int64_t>(MagnificationGestureState::READY_STATE),
            static_cast<int32_t>(MagnificationTimeout::TAP_TIMEOUT));
        return;
    }

    receivedPointerEvents_.push_back(event);
    SendCacheEventsToNext();
    SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleReadyStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    SendEventToMultimodal(event, false, false);
    if (event.GetPointerCount() != static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) {
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
    }
}

void WindowMagnificationGesture::HandleReadyStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    SendEventToMultimodal(event, false, false);
    SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleReadyStateCancel(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    SendEventToMultimodal(event, false, false);
    if (windowMagnificationManager_ != nullptr &&
        windowMagnificationManager_->IsMagnificationWindowShow()) {
        SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
    } else {
        SetGestureState(MagnificationGestureState::READY_STATE, event.GetPointerAction());
    }
}

void WindowMagnificationGesture::HandleReadyStateOneFingerDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    int32_t size = event.GetPointerCount();
    if (size == static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) {
        HILOG_WARN("invalid state.");
    } else {
        receivedPointerEvents_.push_back(event);
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
    }
}

void WindowMagnificationGesture::HandleReadyStateOneFingerDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    receivedPointerEvents_.push_back(event);
    CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    if (IsMove(event, *lastDownEvent_, tapDistance_)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        return;
    }
    if (IsTapOnInputMethod(event)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::READY_STATE, event.GetPointerAction());
        return;
    }
    AddTapCount();
    if (GetTapCount() == TAP_COUNT_THREE) {
        isTripleDown_ = false;
        MMI::PointerEvent::PointerItem pointerItem;
        event.GetPointerItem(event.GetPointerId(), pointerItem);
        OnTripleTap(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        Clear();
        SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
    } else {
        handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG),
            static_cast<int64_t>(MagnificationGestureState::READY_STATE),
            static_cast<int32_t>(MagnificationTimeout::DOUBLE_TAP_TIMEOUT));
        SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_TAP, event.GetPointerAction());
    }
}

void WindowMagnificationGesture::HandleReadyStateOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    receivedPointerEvents_.push_back(event);

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    if (!isTripleDown_ && GetTapCount() == TAP_COUNT_TWO) {
        isTripleDown_ = true;
    } else {
        if (IsMove(event, *lastDownEvent_, tapDistance_)) {
            CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
            CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
            SendCacheEventsToNext();
            SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        }
    }
}

void WindowMagnificationGesture::HandleReadyStateOneFingerDownStatCancel(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    HandleReadyStateCancel(event);
}

void WindowMagnificationGesture::HandleReadyStateOneFingerTapDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
    receivedPointerEvents_.push_back(event);

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    std::shared_ptr<MMI::PointerEvent> preLastEvent = lastDownEvent_;
    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    if (IsMove(*preLastEvent, *lastDownEvent_, multiTapDistance_)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        return;
    }
    handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG),
        static_cast<int64_t>(MagnificationGestureState::READY_STATE),
        static_cast<int32_t>(MagnificationTimeout::TAP_TIMEOUT));
    SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleZoomInStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (IsKnuckles(event)) {
        SendEventToMultimodal(event, true, false);
        return;
    }
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        SendEventToMultimodal(event, true, true);
        return;
    }

    int32_t size = event.GetPointerCount();
    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);

    if (size != static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) {
        return;
    }
    if (menuManager_ != nullptr) {
        isTapOnMenu_ = menuManager_->IsTapOnMenu(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }
    if (isTapOnMenu_) {
        isTapOnHotArea_ = false;
        isSingleTapOnWindow_ = false;
    } else {
        isTapOnHotArea_ = windowMagnificationManager_->IsTapOnHotArea(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
        isSingleTapOnWindow_ = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItem.GetDisplayX(),
            pointerItem.GetDisplayY());
        if (isTapOnHotArea_) {
            handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::LONG_PRESS_MSG), 0,
                static_cast<int32_t>(MagnificationTimeout::LONG_PRESS_TIMEOUT));
        } else {
            receivedPointerEvents_.push_back(event);
        }
    }

    SetGestureState(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN, event.GetPointerAction());
    CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
    handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG),
        static_cast<int64_t>(MagnificationGestureState::ZOOMIN_STATE),
        static_cast<int32_t>(MagnificationTimeout::TAP_TIMEOUT));
}

void WindowMagnificationGesture::HandleZoomInStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    SendEventToMultimodal(event, true, true);
}

void WindowMagnificationGesture::HandleZoomInStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    SendEventToMultimodal(event, true, true);
}

void WindowMagnificationGesture::HandleZoomInStateOneFingerDownStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }

    if (event.GetPointerCount() == static_cast<int32_t>(PointerCountSize::POINTER_SIZE_2)) {
        CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
        CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
        CancelPostEvent(MagnificationGestureMsg::LONG_PRESS_MSG);
        isTapOnMenu_ = false;
        isSingleTapOnWindow_ = false;
        isTapOnHotArea_ = false;
        lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
        MMI::PointerEvent::PointerItem pointerItemOne;
        event.GetPointerItem(0, pointerItemOne);
        bool fingerOne = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItemOne.GetDisplayX(),
            pointerItemOne.GetDisplayY());
        
        MMI::PointerEvent::PointerItem pointerItemTwo;
        event.GetPointerItem(1, pointerItemTwo);
        bool fingerTwo = windowMagnificationManager_->IsTapOnMagnificationWindow(pointerItemTwo.GetDisplayX(),
            pointerItemTwo.GetDisplayY());
        isDoubleFingersValid_ = fingerOne || fingerTwo;

        if (isDoubleFingersValid_) {
            SetGestureState(MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN, event.GetPointerAction());
            Clear();
        } else {
            receivedPointerEvents_.push_back(event);
            SendCacheEventsToNext();
            SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
        }
    }
}

void WindowMagnificationGesture::HandleZoomInStateOneFingerDownStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (!isTapOnHotArea_) {
        receivedPointerEvents_.push_back(event);
    }
    CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::LONG_PRESS_MSG);

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    if (IsMove(event, *lastDownEvent_, tapDistance_)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        return;
    }
    if (IsTapOnInputMethod(event)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
        return;
    }
    AddTapCount();
    if (GetTapCount() == TAP_COUNT_THREE) {
        MMI::PointerEvent::PointerItem pointerItem;
        event.GetPointerItem(event.GetPointerId(), pointerItem);
        OnTripleTap(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        Clear();
        SetGestureState(MagnificationGestureState::READY_STATE, event.GetPointerAction());
    } else {
        if (isTapOnMenu_ && menuManager_ != nullptr) {
            menuManager_->OnMenuTap();
        } else {
            handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG),
                static_cast<int64_t>(MagnificationGestureState::ZOOMIN_STATE),
                static_cast<int32_t>(MagnificationTimeout::DOUBLE_TAP_TIMEOUT));
            SetGestureState(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_TAP, event.GetPointerAction());
        }
    }
}

void WindowMagnificationGesture::HandleZoomInStateOneFingerDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerCount() != 1) {
        receivedPointerEvents_.push_back(event);
        return;
    }

    if (isTapOnMenu_ || isTapOnHotArea_) {
        CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    } else {
        receivedPointerEvents_.push_back(event);
    }

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    if (IsMove(event, *lastDownEvent_, tapDistance_)) {
        CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
        CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
        CancelPostEvent(MagnificationGestureMsg::LONG_PRESS_MSG);
        SendCacheEventsToNext();
        if (isTapOnMenu_) {
            SetGestureState(MagnificationGestureState::MENU_SLIDING_STATE, event.GetPointerAction());
        } else if (isTapOnHotArea_) {
            SetGestureState(MagnificationGestureState::SLIDING_STATE, event.GetPointerAction());
        } else {
            SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        }
    }
}

void WindowMagnificationGesture::HandleZoomInStateOneFingerTapDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    CancelPostEvent(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG);
    CancelPostEvent(MagnificationGestureMsg::TRIPLE_TAP_FAIL_MSG);
    receivedPointerEvents_.push_back(event);

    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    std::shared_ptr<MMI::PointerEvent> preLastEvent = lastDownEvent_;
    lastDownEvent_ = std::make_shared<MMI::PointerEvent>(event);
    if (IsMove(*preLastEvent, *lastDownEvent_, multiTapDistance_)) {
        SendCacheEventsToNext();
        SetGestureState(MagnificationGestureState::PASSING_THROUGH, event.GetPointerAction());
        return;
    }
    handler_->SendEvent(static_cast<uint32_t>(MagnificationGestureMsg::SINGLE_TAP_FAIL_MSG),
        static_cast<int64_t>(MagnificationGestureState::ZOOMIN_STATE),
        static_cast<int32_t>(MagnificationTimeout::TAP_TIMEOUT));
    SetGestureState(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleTwoFingersDownStateUp(MMI::PointerEvent &event)
{
    if (windowMagnificationManager_ != nullptr) {
        if (startScale_) {
            windowMagnificationManager_->PersistScale();
        }
        startScale_ = false;
        windowMagnificationManager_->FixSourceCenter(startScale_);
    }
    lastSlidingEvent_ = nullptr;
    MMI::PointerEvent::PointerItem pointerItem;
    event.GetPointerItem(event.GetPointerId(), pointerItem);
    if ((event.GetPointerCount() == static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1)) &&
        (!pointerItem.IsPressed())) {
        SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
        isDoubleFingersValid_ = false;
    }
}

void WindowMagnificationGesture::HandleTwoFingersDownStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }
    if (!isDoubleFingersValid_) {
        HILOG_ERROR("two fingers state is invalid.");
        return;
    }

    if (event.GetPointerCount() < static_cast<int32_t>(PointerCountSize::POINTER_SIZE_2)) {
        SetGestureState(MagnificationGestureState::SLIDING_STATE, event.GetPointerAction());
        return;
    }

    if (lastDownEvent_ == nullptr) {
        return;
    }

    MMI::PointerEvent::PointerItem baseFirstItem;
    lastDownEvent_->GetPointerItem(0, baseFirstItem);

    MMI::PointerEvent::PointerItem baseSecondItem;
    lastDownEvent_->GetPointerItem(1, baseSecondItem);

    float baseFocusX = (baseFirstItem.GetDisplayX() + baseSecondItem.GetDisplayX()) /
        static_cast<float>(DIVISOR_TWO);
    float baseFocusY = (baseFirstItem.GetDisplayY() + baseSecondItem.GetDisplayY()) /
        static_cast<float>(DIVISOR_TWO);
    int32_t baseOffsetX = abs(baseFirstItem.GetDisplayX() - baseSecondItem.GetDisplayX());
    int32_t baseOffsetY = abs(baseFirstItem.GetDisplayY() - baseSecondItem.GetDisplayY());
    float baseDistance = hypot(baseOffsetX, baseOffsetY);

    if (lastSlidingEvent_ == nullptr) {
        lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }

    MMI::PointerEvent::PointerItem firstItem;
    lastSlidingEvent_->GetPointerItem(0, firstItem);

    MMI::PointerEvent::PointerItem secondItem;
    lastSlidingEvent_->GetPointerItem(1, secondItem);

    float lastFocusX = (firstItem.GetDisplayX() + secondItem.GetDisplayX()) /
        static_cast<float>(DIVISOR_TWO);
    float lastFocusY = (firstItem.GetDisplayY() + secondItem.GetDisplayY()) /
        static_cast<float>(DIVISOR_TWO);
    int32_t offsetX = abs(firstItem.GetDisplayX() - secondItem.GetDisplayX());
    int32_t offsetY = abs(firstItem.GetDisplayY() - secondItem.GetDisplayY());
    float slidingDistance = hypot(offsetX, offsetY);

    MMI::PointerEvent::PointerItem firstCurrentItem;
    event.GetPointerItem(0, firstCurrentItem);

    MMI::PointerEvent::PointerItem secondCurrentItem;
    event.GetPointerItem(1, secondCurrentItem);

    float currentFocusX = (firstCurrentItem.GetDisplayX() + secondCurrentItem.GetDisplayX()) /
        static_cast<float>(DIVISOR_TWO);
    float currentFocusY = (firstCurrentItem.GetDisplayY() + secondCurrentItem.GetDisplayY()) /
        static_cast<float>(DIVISOR_TWO);
    int32_t currentOffsetX = abs(firstCurrentItem.GetDisplayX() - secondCurrentItem.GetDisplayX());
    int32_t currentOffsetY = abs(firstCurrentItem.GetDisplayY() - secondCurrentItem.GetDisplayY());
    float currentDistance = hypot(currentOffsetX, currentOffsetY);
    if (abs(baseDistance - currentDistance) > MIN_SCALE_DISTANCE) {
        startScale_ = true;
        windowMagnificationManager_->FixSourceCenter(startScale_);
    }

    if (startScale_) {
        float deltaSpan = currentDistance - slidingDistance;
        windowMagnificationManager_->SetScale(deltaSpan);
    } else {
        int32_t deltaX = static_cast<int32_t>(currentFocusX - lastFocusX);
        int32_t deltaY = static_cast<int32_t>(currentFocusY - lastFocusY);
        windowMagnificationManager_->MoveMagnificationWindow(deltaX, deltaY);
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
        AccessibilityPowerManager &powerMgr = Singleton<AccessibilityPowerManager>::GetInstance();
        powerMgr.RefreshActivity();
#endif
    }

    if (abs(windowMagnificationManager_->GetScale() - DEFAULT_SCALE) < EPS) {
        startScale_ = false;
        windowMagnificationManager_->FixSourceCenter(startScale_);
    }
    lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

void WindowMagnificationGesture::HandleSlidingStateDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG("no need process down in this sliding state.");
}

void WindowMagnificationGesture::HandleSlidingStateUp(MMI::PointerEvent &event)
{
    if (event.GetPointerCount() > static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1) && event.GetPointerId() > 0) {
        return;
    }
    lastSlidingEvent_ = nullptr;
    Clear();
    SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleSlidingStateMove(MMI::PointerEvent &event)
{
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }
    
    if (event.GetPointerCount() > static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1) && event.GetPointerId() > 0) {
        return;
    }

    if (isTapOnHotArea_) {
        if (lastSlidingEvent_ == nullptr) {
            lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
        }
        MMI::PointerEvent::PointerItem lastSlidingItem;
        lastSlidingEvent_->GetPointerItem(lastSlidingEvent_->GetPointerId(), lastSlidingItem);

        MMI::PointerEvent::PointerItem currentItem;
        event.GetPointerItem(event.GetPointerId(), currentItem);

        int32_t deltaX = currentItem.GetDisplayX() - lastSlidingItem.GetDisplayX();
        int32_t deltaY = currentItem.GetDisplayY() - lastSlidingItem.GetDisplayY();
        windowMagnificationManager_->MoveMagnificationWindow(deltaX, deltaY);

        lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
    } else {
        if (lastSlidingEvent_ != nullptr) {
            lastSlidingEvent_ = nullptr;
        }
    }
}

void WindowMagnificationGesture::HandleMenuSlidingStateMove(MMI::PointerEvent &event)
{
    if (event.GetPointerCount() > static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1) && event.GetPointerId() > 0) {
        return;
    }

    if (isTapOnMenu_ && menuManager_ != nullptr) {
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
}

void WindowMagnificationGesture::HandleMenuSlidingStateUp(MMI::PointerEvent &event)
{
    if (event.GetPointerCount() > static_cast<int32_t>(PointerCountSize::POINTER_SIZE_1) && event.GetPointerId() > 0) {
        return;
    }
    lastSlidingEvent_ = nullptr;
    isTapOnMenu_ = false;
    Clear();
    if (menuManager_ != nullptr) {
        menuManager_->AttachToEdge();
    }
    SetGestureState(MagnificationGestureState::ZOOMIN_STATE, event.GetPointerAction());
}

void WindowMagnificationGesture::HandleDraggingStateUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() != DEFAULT_POINTER_ID) {
        return;
    }
    SetGestureState(MagnificationGestureState::READY_STATE, HANDLER);
    lastSlidingEvent_ = nullptr;
    if (windowMagnificationManager_ != nullptr &&
        windowMagnificationManager_->IsMagnificationWindowShow()) {
        windowMagnificationManager_->DisableWindowMagnification();
        isSingleTapOnWindow_ = false;
        isTapOnHotArea_ = false;
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
    }
}

void WindowMagnificationGesture::HandleDraggingStateMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (event.GetPointerId() != DEFAULT_POINTER_ID) {
        return;
    }
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }

    if (lastSlidingEvent_ == nullptr) {
        lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
    }
    MMI::PointerEvent::PointerItem lastSlidingItem;
    lastSlidingEvent_->GetPointerItem(lastSlidingEvent_->GetPointerId(), lastSlidingItem);

    MMI::PointerEvent::PointerItem currentItem;
    event.GetPointerItem(event.GetPointerId(), currentItem);

    int32_t deltaX = currentItem.GetDisplayX() - lastSlidingItem.GetDisplayX();
    int32_t deltaY = currentItem.GetDisplayY() - lastSlidingItem.GetDisplayY();
    windowMagnificationManager_->MoveMagnificationWindow(deltaX, deltaY);

    lastSlidingEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

bool WindowMagnificationGesture::IsMove(MMI::PointerEvent &startEvent, MMI::PointerEvent &endEvent, float slop)
{
    MMI::PointerEvent::PointerItem endItem;
    endEvent.GetPointerItem(endEvent.GetPointerId(), endItem);

    MMI::PointerEvent::PointerItem startItem;
    startEvent.GetPointerItem(startEvent.GetPointerId(), startItem);
    float deltaX = startItem.GetDisplayX() - endItem.GetDisplayX();
    float deltaY = startItem.GetDisplayY() - endItem.GetDisplayY();
    double duration = hypot(deltaX, deltaY);
    return duration > slop;
}

void WindowMagnificationGesture::Clear()
{
    HILOG_DEBUG();
    receivedPointerEvents_.clear();
}

void WindowMagnificationGesture::OnTripleTap(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("OnTripleTap");
    ResetTapCount();

    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }
    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    if (windowMagnificationManager_->IsMagnificationWindowShow()) {
        windowMagnificationManager_->DisableWindowMagnification();
        menuManager_->DisableMenuWindow();
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
        isSingleTapOnWindow_ = false;
        isTapOnHotArea_ = false;
    } else {
        windowMagnificationManager_->EnableWindowMagnification(centerX, centerY);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
            AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
        menuManager_->ShowMenuWindow(WINDOW_MAGNIFICATION);
    }
}

void WindowMagnificationGesture::SendEventToMultimodal(MMI::PointerEvent event, bool needTransfer, bool needResetTime)
{
    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }

    if (isTapOnMenu_) {
        HILOG_DEBUG("Tap on menu.");
        return;
    }

    if (isSingleTapOnWindow_ && !isTapOnHotArea_) {
        MMI::PointerEvent::PointerItem pointer {};
        event.GetPointerItem(event.GetPointerId(), pointer);

        PointerPos coordinates  = windowMagnificationManager_->ConvertCoordinates(pointer.GetDisplayX(),
            pointer.GetDisplayY());

        pointer.SetDisplayX(coordinates.posX);
        pointer.SetDisplayY(coordinates.posY);
        pointer.SetTargetWindowId(-1);
        event.RemovePointerItem(event.GetPointerId());
        event.AddPointerItem(pointer);
        event.SetZOrder(10000); // magnification zlevel is 10000
    }

    if (needResetTime) {
        event.SetActionTime(Utils::GetSystemTime() * US_TO_MS);
    }
    EventTransmission::OnPointerEvent(event);
}

void WindowMagnificationGesture::ShieldZoomGesture(bool state)
{
    HILOG_INFO("ShieldZoomGesture state = %{public}d", state);
    shieldZoomGestureFlag_ = state;
    if (state) {
        Clear();
        if (windowMagnificationManager_ != nullptr) {
            windowMagnificationManager_->DisableWindowMagnification(true);
            SetGestureState(MagnificationGestureState::READY_STATE, HANDLER);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
                AnnounceType::ANNOUNCE_MAGNIFICATION_DISABLE);
        }
        if (menuManager_ != nullptr) {
            menuManager_->DisableMenuWindow();
        }
        isSingleTapOnWindow_ = false;
        isTapOnHotArea_ = false;
    }
}

void WindowMagnificationGesture::StartMagnificationInteract()
{
    HILOG_DEBUG();
    SetGestureState(MagnificationGestureState::ZOOMIN_STATE, SWITCH_MENU);
}

void WindowMagnificationGesture::DisableGesture()
{
    HILOG_DEBUG();
    SetGestureState(MagnificationGestureState::READY_STATE, SWITCH_MENU);
}

bool WindowMagnificationGesture::IsTapOnInputMethod(MMI::PointerEvent &event)
{
    int32_t pointerCount = event.GetPointerCount();
    if (pointerCount != 1) {
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

bool WindowMagnificationGesture::IsKnuckles(MMI::PointerEvent &event)
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

void WindowMagnificationGesture::OnDrag()
{
    HILOG_INFO();
    SetGestureState(MagnificationGestureState::DRAGGING_STATE, HANDLER);
    ResetTapCount();
    isTripleDown_ = false;
    if (lastDownEvent_ == nullptr) {
        HILOG_ERROR("lastDownEvent_ is nullptr");
        return;
    }

    MMI::PointerEvent::PointerItem item;
    lastDownEvent_->GetPointerItem(lastDownEvent_->GetPointerId(), item);

    int32_t anchorX = item.GetDisplayX();
    int32_t anchorY = item.GetDisplayY();

    if (windowMagnificationManager_ == nullptr) {
        HILOG_ERROR("windowMagnificationManager_ is nullptr.");
        return;
    }
    windowMagnificationManager_->EnableWindowMagnification(anchorX, anchorY);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
    Clear();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP