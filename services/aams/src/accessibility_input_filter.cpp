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

#include <functional>

#include "accessible_ability_manager_service.h"
#include "accessibility_input_filter.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_handler.h"
#include "hilog_wrapper.h"
#include "power_mgr_client.h"

namespace OHOS {
namespace Accessibility {

AccessibilityInputFilter::AccessibilityInputFilter()
{
    HILOG_DEBUG();

    ams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (ams_ != nullptr) {
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(ams_->GetMainRunner());
    } 
}

AccessibilityInputFilter::~AccessibilityInputFilter()
{
    HILOG_DEBUG();

    DestroyTransmitters();
}

void AccessibilityInputFilter::OnInputEvent(MMI::MultimodalEvent &event)
{
    HILOG_DEBUG();

    if (eventHandler_ == nullptr) {
        HILOG_ERROR("EventHandler_ is null.");
        InjectEventToMultimodal(event);
        return;
    }

    int sourceType = event.GetSourceDevice();
    HILOG_DEBUG("Source type[%d].", sourceType);
    switch (sourceType) {
        case MMI::SourceDevice::TOUCH_PANEL: {
            TouchEvent touchEvent = static_cast<TouchEvent&>(event);
            auto task = std::bind(&AccessibilityInputFilter::ProcessTouchEvent, this, touchEvent);
            eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
        }
            break;
        case MMI::SourceDevice::MOUSE: {
            MouseEvent mouseEvent = static_cast<MouseEvent&>(event);
            auto task = std::bind(&AccessibilityInputFilter::ProcessMouseEvent, this, mouseEvent);
            eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
        }
            break;
        case MMI::SourceDevice::KEYBOARD: {
            // TODO: fixme
            // MMI::KeyEvent keyEvent = static_cast<MMI::KeyEvent&>(event);
            // auto task = std::bind(&AccessibilityInputFilter::ProcessKeyEvent, this, keyEvent);
            // eventHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::LOW);
        }
            break;
        default:
            InjectEventToMultimodal(event);
            break;
    }
}

void AccessibilityInputFilter::OnTouchEvent(TouchEvent &event)
{
    HILOG_DEBUG();

    InjectEventToMultimodal(event);
}

void AccessibilityInputFilter::OnMouseEvent(MouseEvent &event)
{
    HILOG_DEBUG();

    InjectEventToMultimodal(event);
}

void AccessibilityInputFilter::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    // TODO: fixme
    // InjectEventToMultimodal(event);
}

void AccessibilityInputFilter::SetUser(uint32_t userId)
{
    HILOG_DEBUG("userId[%d].", userId);

    if (userId_ == userId) {
        return;
    }
    userId_ = userId;
    DestroyTransmitters();
    CreateTransmitters();
}

void AccessibilityInputFilter::SetAvailableFunctions(uint32_t availableFunctions)
{
    HILOG_DEBUG("function[%d].", availableFunctions);

    if (availableFunctions_ == availableFunctions) {
        return;
    }
    availableFunctions_ = availableFunctions;
    DestroyTransmitters();
    CreateTransmitters();
}

void AccessibilityInputFilter::CreateTransmitters()
{
    HILOG_DEBUG("function[%d].", availableFunctions_);

    if (availableFunctions_ == 0) {
        return;
    }

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_ & FEATURE_INJECT_TOUCH_EVENTS) {
        sptr<TouchEventInjector> touchEventInjector = new TouchEventInjector();
        SetNextEventTransmitter(header, current, touchEventInjector);
        ams_->SetTouchEventInjector(touchEventInjector);
    }

    if (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) {
        sptr<AccessibilityZoomHandler> zoomHandler = new AccessibilityZoomHandler(0);
        SetNextEventTransmitter(header, current, zoomHandler);
    }

    if (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) {
        sptr<TouchGuider> touchGuider = new TouchGuider();
        touchGuider->StartUp();
        SetNextEventTransmitter(header, current, touchGuider);
    }

    SetNextEventTransmitter(header, current, this);
    eventTransmitters_.insert(std::make_pair(EventType::OTHER, header));

    if (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) {
        sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
        ams_->SetKeyEventFilter(keyEventFilter);
        keyEventFilter->SetNext(this);
        eventTransmitters_.insert(std::make_pair(EventType::KEY, keyEventFilter));
    }
}

void AccessibilityInputFilter::DestroyTransmitters()
{
    HILOG_DEBUG();

    for (auto iter = eventTransmitters_.begin(); iter != eventTransmitters_.end(); iter++) {
        iter->second->DestroyEvents();
    }
    ams_->SetTouchEventInjector(nullptr);
    ams_->SetKeyEventFilter(nullptr);
    eventTransmitters_.clear();
}

void AccessibilityInputFilter::NotifyAccessibilityEvent(AccessibilityEventInfo &event) const
{
    HILOG_DEBUG();

    for (auto iter = eventTransmitters_.begin(); iter != eventTransmitters_.end(); iter++) {
        iter->second->OnAccessibilityEvent(event);
    }
}

void AccessibilityInputFilter::ProcessTouchEvent(TouchEvent &event)
{   
    HILOG_DEBUG();

    if (eventTransmitters_.empty() ||
        (eventTransmitters_.find(EventType::OTHER) == eventTransmitters_.end())) {
        HILOG_DEBUG("eventTransmitters_ is empty.");
        return;
    }
    PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
    eventTransmitters_.at(EventType::OTHER)->OnTouchEvent(event);
}

void AccessibilityInputFilter::ProcessMouseEvent(MouseEvent &event)
{
    HILOG_DEBUG();

    if (eventTransmitters_.empty() ||
        (eventTransmitters_.find(EventType::OTHER) == eventTransmitters_.end())) {
        HILOG_DEBUG("eventTransmitters_ is empty.");
        return;
    }
    PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
    eventTransmitters_.at(EventType::OTHER)->OnMouseEvent(event);
}

void AccessibilityInputFilter::ProcessKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    if (eventTransmitters_.empty() ||
        (eventTransmitters_.find(EventType::KEY) == eventTransmitters_.end())) {
        HILOG_DEBUG("eventTransmitters_ is empty.");
        return;
    }

    eventTransmitters_.at(EventType::KEY)->OnKeyEvent(event);
}

void AccessibilityInputFilter::SetNextEventTransmitter(sptr<EventTransmission> &header,
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
}  // namespace accessibility