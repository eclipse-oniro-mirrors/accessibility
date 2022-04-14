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

#ifndef ACCESSIBILITY_UI_TEST_ABILITY_LISTENER_H
#define ACCESSIBILITY_UI_TEST_ABILITY_LISTENER_H

#include <memory>
#include "accessibility_event_info.h"

namespace OHOS {
namespace MMI {
class KeyEvent;
}
namespace Accessibility {
// This API not work, should be deleted.
class IAccessibleUITestAbilityListener {
public:
    /**
     * @brief Called when an accessibility is connected.
     * @param
     * @return
     */
    virtual void OnAbilityConnected() = 0;

    /**
     * @brief Called when an accessibility is disconnected.
     * @param
     * @return
     */
    virtual void OnAbilityDisconnected() = 0;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     * @return
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) = 0;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     * @return
     */
    virtual bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const int32_t sequence) = 0;

    /**
     * @brief Called when need to notify the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param completedSuccessfully The result of gesture completion.
     * @return
     */
    virtual void OnGestureSimulateResult(const int32_t sequence, const bool completedSuccessfully) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_UI_TEST_ABILITY_LISTENER_H