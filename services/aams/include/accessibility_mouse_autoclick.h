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

#ifndef ACCESSIBILITY_MOUSE_AUTOCLICK_H
#define ACCESSIBILITY_MOUSE_AUTOCLICK_H

#include "accessibility_event_transmission.h"
#include "event_handler.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityMouseAutoclick : public EventTransmission {
public:
    /**
     * @brief A constructor used to create a AccessibilityMouseAutoclick instance.
     */
    AccessibilityMouseAutoclick();

    /**
     * @brief A destructor used to delete the AccessibilityMouseAutoclick instance.
     */
    virtual ~AccessibilityMouseAutoclick();

    /**
     * @brief Handle mouse events from previous event stream node.
     * @param event the pointer event from Multimodal
     * @return true: the event has been processed and does not need to be passed to the next node;
     *         false: the event is not processed.
     */
    bool OnPointerEvent(MMI::PointerEvent &event) override;

    void SendMouseClickEvent();
    void ResetAutoclickInfo();

    /**
     * @brief Destroy the events.
     */
    void DestroyEvents() override;

private:
    class MouseAutoclickEventHandler : public AppExecFwk::EventHandler {
    public:
        /**
         * @brief A constructor used to create a MouseAutoclickEventHandler instance.
         */
        MouseAutoclickEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
            AccessibilityMouseAutoclick &mouseAutoclick);
        virtual ~MouseAutoclickEventHandler() = default;

        /**
         * @brief Process the event of install system bundles.
         * @param event Indicates the event to be processed.
         */
        virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

    private:
        AccessibilityMouseAutoclick &mouseAutoclick_;
    };

    void CancelAutoclick();
    void RecognizeAutoclick(MMI::PointerEvent &event);
    bool IsMouseMovement(MMI::PointerEvent &event);
    int64_t GetSystemTime();
    int64_t GetDelayTime();

    std::shared_ptr<MouseAutoclickEventHandler> timeoutHandler_ = nullptr;
    std::shared_ptr<MMI::PointerEvent> lastMouseEvent_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_MOUSE_AUTOCLICK_H