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

#include "accessibility_input_interceptor.h"

#include <functional>

#include "accessibility_keyevent_filter.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_handler.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "power_mgr_client.h"
#include "input_manager.h"
#include "key_event.h"
#include "input_event.h"

namespace OHOS {
namespace Accessibility {
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::instance_ = nullptr;
sptr<AccessibilityInputInterceptor> AccessibilityInputInterceptor::GetInstance()
{
    HILOG_DEBUG();

    if (instance_ == nullptr) {
        instance_ = new AccessibilityInputInterceptor();
    }
    return instance_;
}

AccessibilityInputInterceptor::AccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    ams_ = nullptr;
    eventHandler_ = nullptr;
    pointerEventTransmitters_ = nullptr;
    keyEventTransmitters_ = nullptr;
    ams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (ams_ != nullptr) {
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(ams_->GetMainRunner());
    }
}

AccessibilityInputInterceptor::~AccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    RemoveAllInterceptors();
    DestroyTransmitters();
    ams_ = nullptr;
    eventHandler_ = nullptr;
    pointerEventTransmitters_ = nullptr;
    keyEventTransmitters_ = nullptr;
}

void AccessibilityInputInterceptor::InterceptKeyEventCallBack(std::shared_ptr<MMI::KeyEvent> keyEvent)
{
    HILOG_DEBUG();

    if ((nullptr == instance_) || (nullptr == instance_->eventHandler_)) {
        HILOG_DEBUG("eventHandler_ is empty.");
        return;
    }
    auto task = std::bind(&AccessibilityInputInterceptor::ProcessKeyEvent, instance_, keyEvent);
    instance_->eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
    return;
}

void AccessibilityInputInterceptor::InterceptPointerEventCallBack(std::shared_ptr<MMI::PointerEvent> pointerEvent)
{
    HILOG_DEBUG();

    if ((nullptr == instance_) || (nullptr == instance_->eventHandler_)) {
        HILOG_DEBUG("eventHandler_ is empty.");
        return;
    }
    auto task = std::bind(&AccessibilityInputInterceptor::ProcessPointerEvent, instance_, pointerEvent);
    instance_->eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
    return;
}

void AccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();
    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    MMI::InputManager::GetInstance()->SimulateInputEvent(keyEvent);
}

void AccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

void AccessibilityInputInterceptor::SetAvailableFunctions(uint32_t availableFunctions)
{
    HILOG_DEBUG("function[%d].", availableFunctions);

    if (availableFunctions_ == availableFunctions) {
        return;
    }
    availableFunctions_ = availableFunctions;
    DestroyTransmitters();
    CreateTransmitters();
}

void AccessibilityInputInterceptor::CreateTransmitters()
{
    HILOG_DEBUG("function[%d].", availableFunctions_);

    if (availableFunctions_ == 0) {
        RemoveAllInterceptors();
        return;
    }

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;
    bool isInterceptPointerEvent = false;
    bool isInterceptKeyEvent = false;

    if (availableFunctions_& FEATURE_INJECT_TOUCH_EVENTS) {
        sptr<TouchEventInjector> touchEventInjector = new TouchEventInjector();
        SetNextEventTransmitter(header, current, touchEventInjector);
        ams_->SetTouchEventInjector(touchEventInjector);
        isInterceptPointerEvent = true;
    }

    if (availableFunctions_& FEATURE_SCREEN_MAGNIFICATION) {
        sptr<AccessibilityZoomHandler> zoomHandler = new AccessibilityZoomHandler(0);
        SetNextEventTransmitter(header, current, zoomHandler);
        isInterceptPointerEvent = true;
    }

    if (availableFunctions_& FEATURE_TOUCH_EXPLORATION) {
        sptr<TouchGuider> touchGuider = new TouchGuider();
        touchGuider->StartUp();
        SetNextEventTransmitter(header, current, touchGuider);
        isInterceptPointerEvent = true;
    }

    SetNextEventTransmitter(header, current, instance_);
    pointerEventTransmitters_ = header;

    if (availableFunctions_& FEATURE_FILTER_KEY_EVENTS) {
        sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
        ams_->SetKeyEventFilter(keyEventFilter);
        keyEventFilter->SetNext(instance_);
        keyEventTransmitters_ = keyEventFilter;
        isInterceptKeyEvent = true;
    }

    CreateInterceptors(isInterceptPointerEvent, isInterceptKeyEvent);
}

void AccessibilityInputInterceptor::CreateInterceptors(bool isInterceptPointerEvent, bool isInterceptKeyEvent)
{
    HILOG_DEBUG();
    MMI::InputManager* inputManager = MMI::InputManager::GetInstance();
    int32_t id = 0;
    if (interceptorId_.empty()) {
        if (isInterceptPointerEvent) {
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_MOUSE,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::MOUSE, id));
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::TOUCHSCREEN, id));
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::TOUCHPAD, id));
        }
        if (isInterceptKeyEvent) {
            id = inputManager->AddInterceptor(InterceptKeyEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::KEY, id));
        }
        return;
    }
    if (isInterceptPointerEvent) {
        if (interceptorId_.find(InterceptSourceType::MOUSE) == interceptorId_.end()) {
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_MOUSE,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::MOUSE, id));
        }
        if (interceptorId_.find(InterceptSourceType::TOUCHSCREEN) == interceptorId_.end()) {
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::TOUCHSCREEN, id));
        }
        if (interceptorId_.find(InterceptSourceType::TOUCHPAD) == interceptorId_.end()) {
            id = inputManager->AddInterceptor(MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD,
                InterceptPointerEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::TOUCHPAD, id));
        }
    } else {
        if (interceptorId_.find(InterceptSourceType::MOUSE) != interceptorId_.end()) {
            id = interceptorId_.at(InterceptSourceType::MOUSE);
            inputManager->RemoveInterceptor(id);
            interceptorId_.erase(InterceptSourceType::MOUSE);
        }
        if (interceptorId_.find(InterceptSourceType::TOUCHSCREEN) != interceptorId_.end()) {
            id = interceptorId_.at(InterceptSourceType::TOUCHSCREEN);
            inputManager->RemoveInterceptor(id);
            interceptorId_.erase(InterceptSourceType::TOUCHSCREEN);
        }
        if (interceptorId_.find(InterceptSourceType::TOUCHPAD) != interceptorId_.end()) {
            id = interceptorId_.at(InterceptSourceType::TOUCHPAD);
            inputManager->RemoveInterceptor(id);
            interceptorId_.erase(InterceptSourceType::TOUCHPAD);
        }
    }
    if (isInterceptKeyEvent) {
        if (interceptorId_.find(InterceptSourceType::KEY) == interceptorId_.end()) {
            id = inputManager->AddInterceptor(InterceptKeyEventCallBack);
            interceptorId_.insert(std::make_pair(InterceptSourceType::KEY, id));
        }
    } else {
        if (interceptorId_.find(InterceptSourceType::KEY) != interceptorId_.end()) {
            id = interceptorId_.at(InterceptSourceType::KEY);
            inputManager->RemoveInterceptor(id);
            interceptorId_.erase(InterceptSourceType::KEY);
        }
    }
}

void AccessibilityInputInterceptor::RemoveAllInterceptors()
{
    HILOG_DEBUG();

    if (interceptorId_.empty()) {
        return;
    }

    for (auto iter = interceptorId_.begin(); iter != interceptorId_.end(); iter++) {
        MMI::InputManager::GetInstance()->RemoveInterceptor(iter->second);
    }
    interceptorId_.clear();
}

void AccessibilityInputInterceptor::DestroyTransmitters()
{
    HILOG_DEBUG();

    if (nullptr != pointerEventTransmitters_) {
        pointerEventTransmitters_->DestroyEvents();
        ams_->SetTouchEventInjector(nullptr);
        pointerEventTransmitters_= nullptr;
    }
    if (nullptr != keyEventTransmitters_) {
        keyEventTransmitters_->DestroyEvents();
        ams_->SetKeyEventFilter(nullptr);
        keyEventTransmitters_ = nullptr;
    }
}

void AccessibilityInputInterceptor::NotifyAccessibilityEvent(AccessibilityEventInfo &event) const
{
    HILOG_DEBUG();

    if (nullptr != pointerEventTransmitters_) {
        pointerEventTransmitters_->OnAccessibilityEvent(event);
    }
    if (nullptr != keyEventTransmitters_) {
        keyEventTransmitters_->OnAccessibilityEvent(event);
    }
}

void AccessibilityInputInterceptor::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event)
{
    HILOG_DEBUG();

    if (nullptr == pointerEventTransmitters_) {
        HILOG_DEBUG("pointerEventTransmitters_ is empty.");
        return;
    }
    PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
    pointerEventTransmitters_->OnPointerEvent(*event);
}

void AccessibilityInputInterceptor::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event)
{
    HILOG_DEBUG();

    if (nullptr == keyEventTransmitters_) {
        HILOG_DEBUG("keyEventTransmitters_ is empty.");
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
}
}  // namespace Accessibility
