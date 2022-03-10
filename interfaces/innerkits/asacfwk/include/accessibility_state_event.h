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

#ifndef ACCESSIBILITY_STATE_EVENT_H
#define ACCESSIBILITY_STATE_EVENT_H

#include <string>

namespace OHOS {
namespace Accessibility {
enum AccessibilityStateEventType : int {
    EVENT_ACCESSIBILITY_STATE_CHANGED = 0x00000001,
    EVENT_TOUCH_GUIDE_STATE_CHANGED = 0x00000002,
    EVENT_CAPTION_STATE_CHANGED = 0x00000004,
    EVENT_KEVEVENT_STATE_CHANGED = 0x00000008,
    EVENT_GESTURE_STATE_CHANGED = 0x00000010,
};

/*
* This class is used for ASAC as the listener of AAMS.
*/
class AccessibilityStateEvent {
public:
    AccessibilityStateEvent();

    /**
     * @brief Get the type of the state event.
     * @return EVENT_ACCESSIBILITY_STATE_CHANGED/EVENT_TOUCH_GUIDE_STATE_CHANGED
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityStateEventType GetEventType() const;

    /**
     * @brief Get the state of the ability.
     * @return  0:enabled;  otherwise is disabled;
     * @since 3
     * @sysCap Accessibility
     */
    int GetEventResult() const;

    /**
     * @brief Get the description of the ability.
     * @return The description of the ability
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetEventMsg() const;

    /**
     * @brief Set the type of the state event.
     * @param eventType EVENT_ACCESSIBILITY_STATE_CHANGED/EVENT_TOUCH_GUIDE_STATE_CHANGED
     * @since 3
     * @sysCap Accessibility
     */
    void SetEventType(const AccessibilityStateEventType eventType);

    /**
     * @brief Get the state of the ability.
     * @param enabled  0:enabled;  otherwise is disabled;
     * @since 3
     * @sysCap Accessibility
     */
    void SetEventResult(const int enabled);

    /**
     * @brief Set the description of the ability.
     * @param description The description of the ability
     * @since 3
     * @sysCap Accessibility
     */
    void SetEventMsg(std::string &description);
private:
    AccessibilityStateEventType eventType_ = EVENT_ACCESSIBILITY_STATE_CHANGED;
    int enabled_ = 0;
    std::string  describeEvent_ = "";
};

class AccessibilityStateObserver {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    virtual ~AccessibilityStateObserver() = default;

    /**
     * @brief Receives notifications on accessibility status changes.
     * @param stateEvent Indicates the status change event.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    virtual void OnStateChanged(const bool state) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif