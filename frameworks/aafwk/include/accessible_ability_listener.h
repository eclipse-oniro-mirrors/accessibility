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

#ifndef ACCESSIBLE_ABILITY_LISTENER_H
#define ACCESSIBLE_ABILITY_LISTENER_H

#include <stdint.h>
#include "key_event.h"
#include "accessibility_event_info.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityListener {
public:
    AccessibleAbilityListener() = default;
    virtual ~AccessibleAbilityListener() = default;

    /**
     * @brief Called when your accessibility service is successfully connected to the OS.
     * @param
     * @return
     */
    virtual void OnAbilityConnected() {}

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The infomation of accessible event.
     * @return
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) = 0;

    /**
     * @brief Called when a user performs a specified gesture on the device that
     *        your accessibility application has requested to be in touch exploration mode.
     * @param gestureId The id of gesture.
     * @return
     */
    virtual void OnGesture(uint32_t gestureId) {}

    /**
     * @brief Called when the accessibility service is interrupted.
     * @param
     * @return
     */
    virtual void OnInterrupt() = 0;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @return Returns true if the event has been consumed; returns false otherwise.
     *         The event that has been consumed will not be sent to the application.
     */
    virtual bool OnKeyPressEvent(const MMI::KeyEvent& keyEvent) {return false;}
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBLE_ABILITY_LISTENER_H