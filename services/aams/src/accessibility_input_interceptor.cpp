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

#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_mouse_autoclick.h"
#include "accessibility_short_key.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_gesture.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "key_event.h"
#include "input_event.h"

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

    pointerEventTransmitters_ = nullptr;
    keyEventTransmitters_ = nullptr;
    inputManager_ = MMI::InputManager::GetInstance();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

AccessibilityInputInterceptor::~AccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    DestroyInterceptor();
    DestroyTransmitters();
    pointerEventTransmitters_ = nullptr;
    keyEventTransmitters_ = nullptr;
    interceptorId_ = -1;
    keyEventInterceptorId_ = -1;
    inputManager_ = nullptr;
    inputEventConsumer_ = nullptr;
}

void AccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    if (inputManager_ != nullptr) {
        inputManager_->SimulateInputEvent(keyEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
}

void AccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
        event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    if (inputManager_ != nullptr) {
        inputManager_->SimulateInputEvent(pointerEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
}

void AccessibilityInputInterceptor::SetAvailableFunctions(uint32_t availableFunctions)
{
    HILOG_INFO("function[%{public}d].", availableFunctions);

    if (availableFunctions_ == availableFunctions) {
        return;
    }
    availableFunctions_ = availableFunctions;
    DestroyTransmitters();
    CreateTransmitters();
    UpdateInterceptor();
}

void AccessibilityInputInterceptor::CreateTransmitters()
{
    HILOG_DEBUG("function[%{public}u].", availableFunctions_);

    if (!availableFunctions_) {
        return;
    }

    CreatePointerEventTransmitters();
    CreateKeyEventTransmitters();
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
        sptr<AccessibilityZoomGesture> zoomGesture = new(std::nothrow) AccessibilityZoomGesture(0);
        if (!zoomGesture) {
            HILOG_ERROR("zoomGesture is null");
            return;
        }
        SetNextEventTransmitter(header, current, zoomGesture);
    }

    if (availableFunctions_& FEATURE_TOUCH_EXPLORATION) {
        sptr<TouchGuider> touchGuider = new(std::nothrow) TouchGuider();
        if (!touchGuider) {
            HILOG_ERROR("touchGuider is null");
            return;
        }
        touchGuider->StartUp();
        SetNextEventTransmitter(header, current, touchGuider);
    }

    SetNextEventTransmitter(header, current, instance_);
    pointerEventTransmitters_ = header;
}

void AccessibilityInputInterceptor::CreateKeyEventTransmitters()
{
    HILOG_DEBUG();

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_SHORT_KEY) {
        sptr<AccessibilityShortKey> shortKey = new(std::nothrow) AccessibilityShortKey();
        if (!shortKey) {
            HILOG_ERROR("shortKey is null");
            return;
        }
        SetNextEventTransmitter(header, current, shortKey);
    }

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
    HILOG_DEBUG();
    if (!inputManager_) {
        HILOG_ERROR("inputManger is null.");
        return;
    }

    HILOG_INFO("interceptorId:%{public}d, keyEventInterceptorId:%{public}d", interceptorId_, keyEventInterceptorId_);
    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION)) {
        if (interceptorId_ < 0) {
            inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
            interceptorId_ = inputManager_->AddInterceptor(inputEventConsumer_);
            HILOG_DEBUG("interceptorId:%{public}d.", interceptorId_);
        }
    } else {
        if (interceptorId_ >= 0) {
            inputManager_->RemoveInterceptor(interceptorId_);
        }
        interceptorId_ = -1;
    }

    if ((availableFunctions_ & FEATURE_SHORT_KEY) ||
        (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS)) {
        if (keyEventInterceptorId_ < 0) {
            keyEventInterceptorId_ = inputManager_->AddInterceptor(InterceptKeyEventCallback);
            HILOG_DEBUG("keyEventInterceptorId:%{public}d.", keyEventInterceptorId_);
        }
    } else {
        if (keyEventInterceptorId_ >= 0) {
            inputManager_->RemoveInterceptor(keyEventInterceptorId_);
        }
        keyEventInterceptorId_ = -1;
    }
}

void AccessibilityInputInterceptor::InterceptKeyEventCallback(std::shared_ptr<MMI::KeyEvent> keyEvent)
{
    HILOG_DEBUG();

    if (!instance_ || !instance_->eventHandler_) {
        HILOG_ERROR("eventHandler is nullptr.");
        return;
    }
    auto task = std::bind(&AccessibilityInputInterceptor::ProcessKeyEvent, instance_, keyEvent);
    instance_->eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
}

void AccessibilityInputInterceptor::DestroyInterceptor()
{
    HILOG_DEBUG("interceptorId:%{public}d, keyEventInterceptorId:%{public}d", interceptorId_, keyEventInterceptorId_);

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is null.");
        return;
    }
    if (keyEventInterceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(keyEventInterceptorId_);
    }

    if (interceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(interceptorId_);
    }
    keyEventInterceptorId_ = -1;
    interceptorId_ = -1;
}

void AccessibilityInputInterceptor::DestroyTransmitters()
{
    HILOG_DEBUG();

    if (pointerEventTransmitters_ != nullptr) {
        pointerEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(nullptr);
        pointerEventTransmitters_= nullptr;
    }
    if (keyEventTransmitters_ != nullptr) {
        keyEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(nullptr);
        keyEventTransmitters_ = nullptr;
    }
}

void AccessibilityInputInterceptor::NotifyAccessibilityEvent(AccessibilityEventInfo &event) const
{
    HILOG_DEBUG();

    if (pointerEventTransmitters_ != nullptr) {
        pointerEventTransmitters_->OnAccessibilityEvent(event);
    }
    if (keyEventTransmitters_ != nullptr) {
        keyEventTransmitters_->OnAccessibilityEvent(event);
    }
}

void AccessibilityInputInterceptor::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event) const
{
    HILOG_DEBUG();

    if (!pointerEventTransmitters_) {
        HILOG_DEBUG("pointerEventTransmitters_ is empty.");
        const_cast<AccessibilityInputInterceptor*>(this)->OnPointerEvent(*event);
        return;
    }

    pointerEventTransmitters_->OnPointerEvent(*event);
}

void AccessibilityInputInterceptor::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event) const
{
    HILOG_DEBUG();

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

AccessibilityInputEventConsumer::AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

AccessibilityInputEventConsumer::~AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();

    eventHandler_ = nullptr;
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
        pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());

    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }
    auto task = std::bind(&AccessibilityInputInterceptor::ProcessPointerEvent, interceptor, pointerEvent);
    eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
}
} // namespace Accessibility
} // namespace OHOS