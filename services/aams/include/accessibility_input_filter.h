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
#ifndef ACCESSIBILITY_INPUT_FILTER_H
#define ACCESSIBILITY_INPUT_FILTER_H

#include <map>
#include <memory>
#include <vector>

#include "dummy.h"
#include "multimoduleDummy.h"
#include "event_handler.h"

#include "accessibility_event_transmission.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityManagerService;

// Extended from input module
class AccessibilityInputFilter : public EventTransmission, public MultimodalInputFilter {
public:
    // Feature flag for screen magnification.
    const static uint32_t FEATURE_SCREEN_MAGNIFICATION = 0x00000001;

    // Feature flag for touch exploration.
    const static uint32_t FEATURE_TOUCH_EXPLORATION = 0x00000002;

    // Feature flag for filtering key events.
    const static uint32_t FEATURE_FILTER_KEY_EVENTS = 0x00000004;

    // Feature flag for inject touch events.
    const static uint32_t FEATURE_INJECT_TOUCH_EVENTS = 0x00000008;

    AccessibilityInputFilter();
    ~AccessibilityInputFilter();

    void OnInputEvent(MultimodalEvent &event) override;
    void OnTouchEvent(TouchEvent &event) override;
    void OnMouseEvent(MouseEvent &event) override;
    void OnKeyEvent(MMI::KeyEvent &event) override;
    // void OnAccessibilityEvent(AccessibilityEventInfo &event) override {};  // TBD
    void SetUser(uint32_t userId);
    void SetAvailableFunctions(uint32_t availableFunctions);
    void NotifyAccessibilityEvent(AccessibilityEventInfo &event) const;

private:
    enum class EventType {
        KEY = 0,
        OTHER = 1,
    };

    void ProcessTouchEvent(TouchEvent &event);
    void ProcessMouseEvent(MouseEvent &event);
    void ProcessKeyEvent(MMI::KeyEvent &event);
    void CreateTransmitters();
    void DestroyTransmitters();
    void SetNextEventTransmitter(sptr<EventTransmission> &header, sptr<EventTransmission> &current,
        const sptr<EventTransmission> &next);

    std::shared_ptr<AccessibleAbilityManagerService> ams_ = nullptr;
    std::map<EventType, sptr<EventTransmission>> eventTransmitters_ {};
    uint32_t userId_ = 0;
    uint32_t availableFunctions_ = 0;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};

}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_INPUT_FILTER_H
