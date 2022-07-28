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

#ifndef ACCESSIBLE_ABILITY_LISTENER_H
#define ACCESSIBLE_ABILITY_LISTENER_H

#include "accessibility_event_info.h"

namespace OHOS {
namespace MMI {
class KeyEvent;
}
namespace Accessibility {
class AccessibleAbilityListener {
public:
    virtual ~AccessibleAbilityListener() = default;

    /**
     * @brief Called when an accessibility is connected.
     */
    virtual void OnAbilityConnected() = 0;

    /**
     * @brief Called when an accessibility is disconnected.
     */
    virtual void OnAbilityDisconnected() = 0;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) = 0;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @return Return true if the key event has been consumed, else return return false.
     */
    virtual bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_LISTENER_H