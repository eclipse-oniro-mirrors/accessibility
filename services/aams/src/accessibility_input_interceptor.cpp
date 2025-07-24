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

#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_mouse_autoclick.h"
#include "accessibility_short_key.h"
#include "accessibility_screen_touch.h"
#include "accessibility_touch_exploration.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_gesture.h"
#include "window_magnification_gesture.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "key_event.h"
#include "input_event.h"
#ifdef ACCESSIBILITY_WATCH_FEATURE
#include "res_type.h"
#include "res_sched_client.h"
#endif // ACCESSIBILITY_WATCH_FEATURE

namespace OHOS {
namespace Accessibility {
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::instance_ = nullptr;
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::GetInstance()
{
    HILOG_DEBUG();

    if (!instance_) {
        instance_ = new(std::nothrow) AccessibilityInputInterceptor();
        if (!instance_) {
            HILOG_ERROR("instance_ is null");
            return nullptr;
        }
    }
    return instance_;
}

AccessibilityInputInterceptor::AccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    inputManager_ = MMI::InputManager::GetInstance();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner());
}

AccessibilityInputInterceptor::~AccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    availableFunctions_ = 0;
    DestroyInterceptor();
    DestroyTransmitters();
    inputManager_ = nullptr;
    inputEventConsumer_ = nullptr;
}

bool AccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    if (inputManager_) {
        inputManager_->SimulateInputEvent(keyEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
    return true;
}

bool AccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent &event)
{
    if (event.GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE &&
        event.GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE) {
        HILOG_INFO("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
            event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());
    } else {
        HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
            event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());
    }

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    if (inputManager_) {
        inputManager_->SimulateInputEvent(pointerEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
    return true;
}

void AccessibilityInputInterceptor::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG("offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);
    if (inputManager_) {
        inputManager_->MoveMouse(offsetX, offsetY);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
}

void AccessibilityInputInterceptor::SetAvailableFunctions(uint32_t availableFunctions)
{
    HILOG_INFO("function[%{public}d].", availableFunctions);

    if (((availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) != FEATURE_SCREEN_MAGNIFICATION) &&
        (availableFunctions_ == availableFunctions) && ((availableFunctions & FEATURE_SCREEN_TOUCH) == 0)) {
        return;
    }
    availableFunctions_ = availableFunctions;

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty!");
        return;
    }
    eventHandler_->PostTask([this] {
        DestroyTransmitters();
        CreateTransmitters();
        UpdateInterceptor();
    });
}

void AccessibilityInputInterceptor::CreateTransmitters()
{
    HILOG_DEBUG("function[%{public}u].", availableFunctions_);

    if (!availableFunctions_) {
        return;
    }

    if ((availableFunctions_ & FEATURE_MOUSE_KEY) && (!mouseKey_)) {
        mouseKey_ = new(std::nothrow) AccessibilityMouseKey();
        if (mouseKey_) {
            mouseKey_->SetNext(instance_);
        }
    }

    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_INJECT_TOUCH_EVENTS) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) ||
        (availableFunctions_ & FEATURE_SCREEN_TOUCH)) {
        CreatePointerEventTransmitters();
    }
    
    if (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) {
        CreateKeyEventTransmitters();
    }
}

void AccessibilityInputInterceptor::CreatePointerEventTransmitters()
{
    HILOG_DEBUG();

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_MOUSE_AUTOCLICK) {
        sptr<AccessibilityMouseAutoclick> mouseAutoclick = new(std::nothrow) AccessibilityMouseAutoclick();
        if (!mouseAutoclick) {
            HILOG_ERROR("mouseAutoclick is null");
            return;
        }
        SetNextEventTransmitter(header, current, mouseAutoclick);
    }

    if (availableFunctions_& FEATURE_INJECT_TOUCH_EVENTS) {
        sptr<TouchEventInjector> touchEventInjector = new(std::nothrow) TouchEventInjector();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            return;
        }
        SetNextEventTransmitter(header, current, touchEventInjector);
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(touchEventInjector);
    }

    if (availableFunctions_& FEATURE_SCREEN_MAGNIFICATION) {
        CreateMagnificationGesture(header, current);
    } else {
        ClearMagnificationGesture();
    }

    if (availableFunctions_& FEATURE_TOUCH_EXPLORATION) {
        sptr<TouchExploration> touchExploration = new(std::nothrow) TouchExploration();
        if (!touchExploration) {
            HILOG_ERROR("touchExploration is null");
            return;
        }
        touchExploration->StartUp();
        SetNextEventTransmitter(header, current, touchExploration);
    }

    if ((availableFunctions_ & FEATURE_SCREEN_TOUCH) && ((availableFunctions_ & FEATURE_TOUCH_EXPLORATION) == 0)) {
        sptr<AccessibilityScreenTouch> screenTouch = new(std::nothrow) AccessibilityScreenTouch();
        if (!screenTouch) {
            HILOG_ERROR("screenTouch is null");
            return;
        }
        SetNextEventTransmitter(header, current, screenTouch);
    }

    SetNextEventTransmitter(header, current, instance_);
    pointerEventTransmitters_ = header;
}

void AccessibilityInputInterceptor::CreateMagnificationGesture(sptr<EventTransmission> &header,
    sptr<EventTransmission> &current)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().InitMagnification();
    uint32_t magnificationMode = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    if (magnificationMode == FULL_SCREEN_MAGNIFICATION) {
        HILOG_INFO("create zoomGesture");
        CreatZoomGesture();
        if (zoomGesture_ == nullptr) {
            HILOG_ERROR("zoomGesture create error.");
            return;
        }
        SetNextEventTransmitter(header, current, zoomGesture_);
    } else if (magnificationMode == WINDOW_MAGNIFICATION) {
        HILOG_INFO("create windowMagnificationGesture");
        CreatWindowMagnificationGesture();
        if (windowMagnificationGesture_ == nullptr) {
            HILOG_ERROR("windowMagnificationGesture create error.");
            return;
        }
        SetNextEventTransmitter(header, current, windowMagnificationGesture_);
    } else {
        HILOG_WARN("invalid magnificationMode");
        ClearMagnificationGesture();
    }
}

void AccessibilityInputInterceptor::CreatZoomGesture()
{
    std::shared_ptr<FullScreenMagnificationManager> fullScreenMagnificationManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetFullScreenMagnificationManager();
    std::shared_ptr<MagnificationMenuManager> menuManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMenuManager();
    if (fullScreenMagnificationManager == nullptr || menuManager == nullptr) {
        HILOG_ERROR("get windowMagnification or menu manager failed.");
        return;
    }

    if (zoomGesture_ == nullptr) {
        sptr<AccessibilityZoomGesture> zoomGesture =
            new(std::nothrow) AccessibilityZoomGesture(fullScreenMagnificationManager, menuManager);
        if (zoomGesture == nullptr) {
            HILOG_ERROR("zoomGesture create error.");
            return;
        }
        zoomGesture_ = zoomGesture;
    }

    if (needInteractMagnification_) {
        zoomGesture_->StartMagnificationInteract();
        needInteractMagnification_ = false;
    }
}

void AccessibilityInputInterceptor::CreatWindowMagnificationGesture()
{
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetWindowMagnificationManager();
    std::shared_ptr<MagnificationMenuManager> menuManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMenuManager();
    if (windowMagnificationManager == nullptr || menuManager == nullptr) {
        HILOG_ERROR("get windowMagnification or menu manager failed.");
        return;
    }

    if (windowMagnificationGesture_ == nullptr) {
        sptr<WindowMagnificationGesture> windowMagnificationGesture =
            new(std::nothrow) WindowMagnificationGesture(windowMagnificationManager, menuManager);
        if (windowMagnificationGesture == nullptr) {
            HILOG_ERROR("windowMagnificationGesture create error.");
            return;
        }
        windowMagnificationGesture_ = windowMagnificationGesture;
    }
    if (needInteractMagnification_) {
        windowMagnificationGesture_->StartMagnificationInteract();
        needInteractMagnification_ = false;
    }
}

void AccessibilityInputInterceptor::ClearMagnificationGesture()
{
    zoomGesture_ = nullptr;
    windowMagnificationGesture_ = nullptr;
}

void AccessibilityInputInterceptor::CreateKeyEventTransmitters()
{
    HILOG_DEBUG();

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_FILTER_KEY_EVENTS) {
        sptr<KeyEventFilter> keyEventFilter = new(std::nothrow) KeyEventFilter();
        if (!keyEventFilter) {
            HILOG_ERROR("keyEventFilter is null");
            return;
        }
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(keyEventFilter);
        SetNextEventTransmitter(header, current, keyEventFilter);
    }

    SetNextEventTransmitter(header, current, instance_);
    keyEventTransmitters_ = header;
}

void AccessibilityInputInterceptor::UpdateInterceptor()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();
    if (!inputManager_) {
        HILOG_ERROR("inputManger is null.");
        return;
    }

    if (interceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(interceptorId_);
        interceptorId_ = -1;
    }

    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) ||
        (availableFunctions_ & FEATURE_MOUSE_KEY) ||
        (availableFunctions_ & FEATURE_SCREEN_TOUCH)) {
            inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
            interceptorId_ = inputManager_->AddInterceptor(inputEventConsumer_);
    } else if (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) {
            inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
            interceptorId_ = inputManager_->AddInterceptor(inputEventConsumer_, PRIORITY_EVENT,
                MMI::CapabilityToTags(MMI::INPUT_DEV_CAP_KEYBOARD));
    }
    HILOG_INFO("interceptorId:%{public}d", interceptorId_);
}

void AccessibilityInputInterceptor::DestroyInterceptor()
{
    HILOG_DEBUG("interceptorId:%{public}d.", interceptorId_);

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is null.");
        return;
    }
    if (interceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(interceptorId_);
    }
    interceptorId_ = -1;
}

void AccessibilityInputInterceptor::DestroyTransmitters()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if ((availableFunctions_ & FEATURE_MOUSE_KEY) != FEATURE_MOUSE_KEY) {
        if (mouseKey_) {
            mouseKey_->DestroyEvents();
            mouseKey_ = nullptr;
        }
    }

    if (pointerEventTransmitters_ != nullptr) {
        pointerEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(nullptr);
        pointerEventTransmitters_= nullptr;
        zoomGesture_ = nullptr;
        windowMagnificationGesture_ = nullptr;
    }
    if (keyEventTransmitters_ != nullptr) {
        keyEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(nullptr);
        keyEventTransmitters_ = nullptr;
    }
}

void AccessibilityInputInterceptor::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if (mouseKey_) {
        mouseKey_->OnPointerEvent(*event);
    }

    if (!pointerEventTransmitters_) {
        HILOG_DEBUG("pointerEventTransmitters_ is empty.");
        const_cast<AccessibilityInputInterceptor*>(this)->OnPointerEvent(*event);
        return;
    }

    pointerEventTransmitters_->OnPointerEvent(*event);
}

void AccessibilityInputInterceptor::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if (mouseKey_) {
        bool result = mouseKey_->OnKeyEvent(*event);
        if (result) {
            HILOG_DEBUG("The event is mouse key event.");
            return;
        }
    }

    if (!keyEventTransmitters_) {
        HILOG_DEBUG("keyEventTransmitters_ is empty.");
        const_cast<AccessibilityInputInterceptor*>(this)->OnKeyEvent(*event);
        return;
    }

    keyEventTransmitters_->OnKeyEvent(*event);
}

void AccessibilityInputInterceptor::SetNextEventTransmitter(sptr<EventTransmission> &header,
    sptr<EventTransmission> &current, const sptr<EventTransmission> &next)
{
    HILOG_DEBUG();

    if (current != nullptr) {
        current->SetNext(next);
    } else {
        header = next;
    }
    current = next;
}

void AccessibilityInputInterceptor::ShieldZoomGesture(bool flag)
{
    HILOG_INFO("flag = %{public}d", flag);
    if (zoomGesture_) {
        zoomGesture_->ShieldZoomGesture(flag);
    }
    if (windowMagnificationGesture_) {
        windowMagnificationGesture_->ShieldZoomGesture(flag);
    }
}

void AccessibilityInputInterceptor::RefreshDisplayInfo()
{
    if (!zoomGesture_) {
        return;
    }
    zoomGesture_->GetWindowParam(true);
}

void AccessibilityInputInterceptor::StartMagnificationInteract(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    needInteractMagnification_ = true;
}

void AccessibilityInputInterceptor::DisableGesture(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    if (mode == FULL_SCREEN_MAGNIFICATION && zoomGesture_ != nullptr) {
        zoomGesture_->DisableGesture();
    } else if (mode == WINDOW_MAGNIFICATION && windowMagnificationGesture_ != nullptr) {
        windowMagnificationGesture_->DisableGesture();
    } else {
        HILOG_WARN("invalid mode.");
    }
}

void AccessibilityInputInterceptor::EnableGesture(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    if (mode == FULL_SCREEN_MAGNIFICATION && zoomGesture_ != nullptr) {
        zoomGesture_->StartMagnificationInteract();
    } else if (mode == WINDOW_MAGNIFICATION && windowMagnificationGesture_ != nullptr) {
        windowMagnificationGesture_->StartMagnificationInteract();
    } else {
        HILOG_WARN("invalid mode.");
    }
}

AccessibilityInputEventConsumer::AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner());
#ifdef ACCESSIBILITY_WATCH_FEATURE
    eventHandler_->PostTask([] {
        auto pid = getpid();
        auto tid = gettid();
        uint32_t qosLevel = 7;
        std::string strBundleName = "accessibility";
        std::string strPid = std::to_string(pid);
        std::string strTid = std::to_string(tid);
        std::string strQos = std::to_string(qosLevel);
        std::unordered_map<std::string, std::string> mapPayLoad;
        mapPayLoad["pid"] = strPid;
        mapPayLoad[strTid] = strQos;
        mapPayLoad["bundleName"] = strBundleName;
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, 0, mapPayLoad);
    });
#endif // ACCESSIBILITY_WATCH_FEATURE
}

AccessibilityInputEventConsumer::~AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();

    eventHandler_ = nullptr;
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    HILOG_DEBUG();
    if (!keyEvent) {
        HILOG_WARN("keyEvent is null.");
        return;
    }

    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }

    auto task = [keyEvent, interceptor] {interceptor->ProcessKeyEvent(keyEvent);};
    eventHandler_->PostTask(task, "InputKeyEvent");
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (!pointerEvent) {
        HILOG_WARN("pointerEvent is null.");
        return;
    }

    if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        HILOG_INFO("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
            pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());
    } else {
        HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
            pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());
    }

    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }
    auto task = [pointerEvent, interceptor] {interceptor->ProcessPointerEvent(pointerEvent);};
    eventHandler_->PostTask(task, "InputPointerEvent");
}
} // namespace Accessibility
} // namespace OHOS