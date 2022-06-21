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

#ifndef ACCESSIBILITY_INPUT_INTERCEPTOR_H
#define ACCESSIBILITY_INPUT_INTERCEPTOR_H

#include <map>
#include <memory>
#include <vector>

#include "accessibility_event_transmission.h"
#include "event_handler.h"
#include "i_input_event_consumer.h"
#include "input_manager.h"
#include "key_event.h"
#include "pointer_event.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerService;

class AccessibilityInputEventConsumer : public MMI::IInputEventConsumer {
public:
    AccessibilityInputEventConsumer();
    ~AccessibilityInputEventConsumer();
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override {};
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override {};
private:
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};

class AccessibilityInputInterceptor : public EventTransmission {
public:
    // Feature flag for screen magnification.
    static constexpr uint32_t FEATURE_SCREEN_MAGNIFICATION = 0x00000001;

    // Feature flag for touch exploration.
    static constexpr uint32_t FEATURE_TOUCH_EXPLORATION = 0x00000002;

    // Feature flag for filtering key events.
    static constexpr uint32_t FEATURE_FILTER_KEY_EVENTS = 0x00000004;

    // Feature flag for inject touch events.
    static constexpr uint32_t FEATURE_INJECT_TOUCH_EVENTS = 0x00000008;

    // Feature flag for mouse autoclick.
    static constexpr uint32_t FEATURE_MOUSE_AUTOCLICK = 0x00000010;

    // Feature flag for short key.
    static constexpr uint32_t FEATURE_SHORT_KEY = 0x00000020;

    static sptr<AccessibilityInputInterceptor> GetInstance();
    ~AccessibilityInputInterceptor();
    void ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event) const;
    void ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event) const;
    void OnKeyEvent(MMI::KeyEvent &event) override;
    void OnPointerEvent(MMI::PointerEvent &event) override;
    void SetAvailableFunctions(uint32_t availableFunctions);
    void NotifyAccessibilityEvent(AccessibilityEventInfo &event) const;
    static void InterceptKeyEventCallback(std::shared_ptr<MMI::KeyEvent> keyEvent);

private:
    AccessibilityInputInterceptor();
    static sptr<AccessibilityInputInterceptor> instance_;
    void CreateTransmitters();
    void DestroyTransmitters();
    void CreatePointerEventTransmitters();
    void CreateKeyEventTransmitters();
    void SetNextEventTransmitter(sptr<EventTransmission> &header, sptr<EventTransmission> &current,
        const sptr<EventTransmission> &next);
    void UpdateInterceptor();
    void DestroyInterceptor();

    sptr<EventTransmission> pointerEventTransmitters_ = nullptr;
    sptr<EventTransmission> keyEventTransmitters_ = nullptr;
    uint32_t availableFunctions_ = 0;
    int32_t interceptorId_ = -1;
    int32_t keyEventInterceptorId_ = -1;
    MMI::InputManager *inputManager_ = nullptr;
    std::shared_ptr<AccessibilityInputEventConsumer> inputEventConsumer_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_INPUT_INTERCEPTOR_H
