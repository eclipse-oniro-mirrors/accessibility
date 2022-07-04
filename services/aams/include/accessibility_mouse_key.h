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

#ifndef ACCESSIBILITY_MOUSE_KEY_H_
#define ACCESSIBILITY_MOUSE_KEY_H_

#include "accessibility_event_transmission.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityMouseKey : public EventTransmission {
public:
    /**
     * @brief A constructor used to create a AccessibilityMouseKey instance.
     * @param
     * @return
     */
    AccessibilityMouseKey() = default;

    /**
     * @brief A destructor used to delete the AccessibilityMouseKey instance.
     * @param
     * @return
     */
    virtual ~AccessibilityMouseKey() = default;

    /**
     * @brief Handle mouse events from previous event stream node.
     * @param event the pointer event from Multimodal
     * @return true: the event has been processed and does not need to be passed to the next node;
     *         false: the event is not processed.
     */
    bool OnPointerEvent(MMI::PointerEvent &event) override;

    /**
     * @brief Handle key events from previous event stream node.
     * @param event the key event from Multimodal
     * @return true: the event has been processed and does not need to be passed to the next node;
     *         false: the event is not processed.
     */
    bool OnKeyEvent(MMI::KeyEvent &event) override;
private:
    enum SELECTED_KEY_TYPE : uint32_t {
        LEFT_KEY = 0,
        RIGHT_KEY,
        BOOTH_KEY,
    };

    enum CLICK_TYPE : uint32_t {
        SINGLE_CLICK = 1,
        DOUBLE_CLICK = 2,
    };

    void UpdateLastMouseEvent(MMI::PointerEvent &event);
    bool IsMouseKey(int32_t keyCode);
    void ExecuteMouseKey(int32_t keyCode);
    void MoveMousePointer(int32_t offsetX, int32_t offsetY);
    void SendMouseClickEvent(CLICK_TYPE clickType);
    int64_t GetSystemTime();

    std::shared_ptr<MMI::PointerEvent> lastMouseMoveEvent_ = nullptr;
    SELECTED_KEY_TYPE selectedKeyType_ = LEFT_KEY;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_MOUSE_KEY_H_