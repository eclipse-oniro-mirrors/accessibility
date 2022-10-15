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
std::shared_ptr<AccessibilityInterceptorManager> AccessibilityInterceptorManager::instance_ = nullptr;
std::shared_ptr<AccessibilityInterceptorManager> AccessibilityInterceptorManager::GetInstance()
{
    HILOG_DEBUG();

    if (!instance_) {
        instance_ = std::make_shared<AccessibilityInterceptorManager>();
        if (!instance_) {
            HILOG_ERROR("instance_ is null");
            return nullptr;
        }
    }
    return instance_;
}

AccessibilityInterceptorManager::AccessibilityInterceptorManager()
{
    HILOG_DEBUG();

    inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
    inputInterceptor_ = std::make_shared<AccessibilityInputInterceptor>();
}

AccessibilityInterceptorManager::~AccessibilityInterceptorManager()
{
    HILOG_DEBUG();

    availableFunctions_ = 0;
    DestroyInterceptor();
    DestroyTransmitters();
    inputEventConsumer_ = nullptr;
    inputInterceptor_ = nullptr;
}

void AccessibilityInterceptorManager::SetAvailableFunctions(uint32_t availableFunctions)
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

void AccessibilityInterceptorManager::CreateTransmitters()
{
    HILOG_DEBUG("function[%{public}u].", availableFunctions_);

    if (!availableFunctions_) {
        return;
    }

    if ((availableFunctions_ & FEATURE_MOUSE_KEY) && (!mouseKey_)) {
        mouseKey_ = std::make_shared<AccessibilityMouseKey>();
        if (mouseKey_) {
            mouseKey_->SetNext(inputInterceptor_);
        }
    }

    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_INJECT_TOUCH_EVENTS) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION)) {
        CreatePointerEventTransmitters();
    }
    
    if ((availableFunctions_ & FEATURE_SHORT_KEY) ||
        (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS)) {
        CreateKeyEventTransmitters();
    }
}

void AccessibilityInterceptorManager::CreatePointerEventTransmitters()
{
    HILOG_DEBUG();

    std::shared_ptr<EventTransmission> header = nullptr;
    std::shared_ptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_MOUSE_AUTOCLICK) {
        std::shared_ptr<AccessibilityMouseAutoclick> mouseAutoclick = std::make_shared<AccessibilityMouseAutoclick>();
        if (!mouseAutoclick) {
            HILOG_ERROR("mouseAutoclick is null");
            return;
        }
        SetNextEventTransmitter(header, current, mouseAutoclick);
    }

    if (availableFunctions_& FEATURE_INJECT_TOUCH_EVENTS) {
        std::shared_ptr<TouchEventInjector> touchEventInjector = std::make_shared<TouchEventInjector>();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            return;
        }
        SetNextEventTransmitter(header, current, touchEventInjector);
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(touchEventInjector);
    }

    if (availableFunctions_& FEATURE_SCREEN_MAGNIFICATION) {
        std::shared_ptr<AccessibilityZoomGesture> zoomGesture = std::make_shared<AccessibilityZoomGesture>();
        if (!zoomGesture) {
            HILOG_ERROR("zoomGesture is null");
            return;
        }
        SetNextEventTransmitter(header, current, zoomGesture);
    }

    if (availableFunctions_& FEATURE_TOUCH_EXPLORATION) {
        std::shared_ptr<TouchGuider> touchGuider = std::make_shared<TouchGuider>();
        if (!touchGuider) {
            HILOG_ERROR("touchGuider is null");
            return;
        }
        touchGuider->StartUp();
        SetNextEventTransmitter(header, current, touchGuider);
    }
    SetNextEventTransmitter(header, current, inputInterceptor_);
    pointerEventTransmitters_ = header;
}

void AccessibilityInterceptorManager::CreateKeyEventTransmitters()
{
    HILOG_DEBUG();

    std::shared_ptr<EventTransmission> header = nullptr;
    std::shared_ptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_SHORT_KEY) {
        std::shared_ptr<AccessibilityShortKey> shortKey = std::make_shared<AccessibilityShortKey>();
        if (!shortKey) {
            HILOG_ERROR("shortKey is null");
            return;
        }
        SetNextEventTransmitter(header, current, shortKey);
    }

    if (availableFunctions_& FEATURE_FILTER_KEY_EVENTS) {
        std::shared_ptr<KeyEventFilter> keyEventFilter = std::make_shared<KeyEventFilter>();
        if (!keyEventFilter) {
            HILOG_ERROR("keyEventFilter is null");
            return;
        }
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(keyEventFilter);
        SetNextEventTransmitter(header, current, keyEventFilter);
    }
    SetNextEventTransmitter(header, current, inputInterceptor_);
    keyEventTransmitters_ = header;
}

void AccessibilityInterceptorManager::UpdateInterceptor()
{
    HILOG_DEBUG();

    MMI::InputManager *inputManager = MMI::InputManager::GetInstance();
    if (!inputManager) {
        HILOG_ERROR("inputManger is null.");
        return;
    }

    HILOG_INFO("interceptorId:%{public}d", interceptorId_);
    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) ||
        (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) ||
        (availableFunctions_ & FEATURE_MOUSE_KEY) ||
        (availableFunctions_ & FEATURE_SHORT_KEY)) {
        if (interceptorId_ < 0) {
            interceptorId_ = inputManager->AddInterceptor(inputEventConsumer_);
            HILOG_DEBUG("interceptorId:%{public}d.", interceptorId_);
        }
    } else {
        if (interceptorId_ >= 0) {
            inputManager->RemoveInterceptor(interceptorId_);
        }
        interceptorId_ = -1;
    }
}

void AccessibilityInterceptorManager::DestroyInterceptor()
{
    HILOG_DEBUG("interceptorId:%{public}d.", interceptorId_);

    MMI::InputManager *inputManager = MMI::InputManager::GetInstance();
    if (!inputManager) {
        HILOG_ERROR("inputManager is null.");
        return;
    }
    if (interceptorId_ >= 0) {
        inputManager->RemoveInterceptor(interceptorId_);
    }
    interceptorId_ = -1;
}

void AccessibilityInterceptorManager::DestroyTransmitters()
{
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
    }
    if (keyEventTransmitters_ != nullptr) {
        keyEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(nullptr);
        keyEventTransmitters_ = nullptr;
    }
}

void AccessibilityInterceptorManager::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event)
{
    HILOG_DEBUG();

    if (mouseKey_) {
        mouseKey_->OnPointerEvent(*event);
    }

    if (!pointerEventTransmitters_) {
        HILOG_DEBUG("pointerEventTransmitters_ is empty.");
        if (inputInterceptor_) {
            inputInterceptor_->OnPointerEvent(*event);
        }
        return;
    }

    pointerEventTransmitters_->OnPointerEvent(*event);
}

void AccessibilityInterceptorManager::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event)
{
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
        if (inputInterceptor_) {
            inputInterceptor_->OnKeyEvent(*event);
        }
        return;
    }

    keyEventTransmitters_->OnKeyEvent(*event);
}

void AccessibilityInterceptorManager::SetNextEventTransmitter(std::shared_ptr<EventTransmission> &header,
    std::shared_ptr<EventTransmission> &current, const std::shared_ptr<EventTransmission> &next)
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

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    HILOG_DEBUG();
    if (!keyEvent) {
        HILOG_WARN("keyEvent is null.");
        return;
    }

    auto interceptorManager = AccessibilityInterceptorManager::GetInstance();
    if (!interceptorManager) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }

    auto task = std::bind(&AccessibilityInterceptorManager::ProcessKeyEvent, interceptorManager, keyEvent);
    eventHandler_->PostTask(task, "InputKeyEvent");
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (!pointerEvent) {
        HILOG_WARN("pointerEvent is null.");
        return;
    }
    HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
        pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());

    auto interceptorManager = AccessibilityInterceptorManager::GetInstance();
    if (!interceptorManager) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }
    auto task = std::bind(&AccessibilityInterceptorManager::ProcessPointerEvent, interceptorManager, pointerEvent);
    eventHandler_->PostTask(task, "InputPointerEvent");
}

bool AccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    MMI::InputManager *inputManager = MMI::InputManager::GetInstance();
    if (inputManager) {
        inputManager->SimulateInputEvent(keyEvent);
    } else {
        HILOG_ERROR("inputManager is null.");
    }
    return true;
}

bool AccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
        event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    MMI::InputManager *inputManager = MMI::InputManager::GetInstance();
    if (inputManager) {
        inputManager->SimulateInputEvent(pointerEvent);
    } else {
        HILOG_ERROR("inputManager is null.");
    }
    return true;
}

void AccessibilityInputInterceptor::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG("offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);
    MMI::InputManager *inputManager = MMI::InputManager::GetInstance();
    if (inputManager) {
        inputManager->MoveMouse(offsetX, offsetY);
    } else {
        HILOG_ERROR("inputManager is null.");
    }
}
} // namespace Accessibility
} // namespace OHOS