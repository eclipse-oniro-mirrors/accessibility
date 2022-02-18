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

#ifndef ACCESSIBLE_ABILITY_CLIENT_INTERFACE_H
#define ACCESSIBLE_ABILITY_CLIENT_INTERFACE_H

#include "iremote_broker.h"
#include "refbase.h"
#include "key_event.h"
#include "accessibility_element_info.h"
#include "accessible_ability_channel_interface.h"
#include "accessibility_event_info.h"

namespace OHOS {
namespace Accessibility {

class IAccessibleAbilityClient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityClient");

    /**
     * @brief Init accessible ability.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId) = 0;

    /**
     * @brief Disconnect accessible ability.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Disconnect(const int channelId) = 0;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The infomation of accessible event.
     * @return
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) = 0;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     * @return
     */
    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) = 0;

    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed.
     * @param displayId The id of display.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    virtual void OnDisplayResized(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY) = 0;

    /**
     * @brief Called when need to notify the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param completedSuccessfully The result of gesture completion.
     * @return
     */
    virtual void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) = 0;

    enum class Message {
        INIT = 0,
        DISCONNECT,
        ON_ACCESSIBILITY_EVENT,
        ON_KEY_PRESS_EVENT,
        ON_DISPALYRESIZE_CHANGED,
        ON_GESTURE_SIMULATE_RESULT,
    };
};

} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CLIENT_INTERFACE_H