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

#ifndef INTERFACE_ACCESSIBLE_ABILITY_CLIENT_H
#define INTERFACE_ACCESSIBLE_ABILITY_CLIENT_H

#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "i_accessible_ability_channel.h"
#include "iremote_broker.h"
#include "key_event.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityClient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityClient");

    /**
     * @brief Init accessible ability.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     */
    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId) = 0;

    /**
     * @brief Disconnect accessible ability.
     * @param channelId The id of channel.
     */
    virtual void Disconnect(const int32_t channelId) = 0;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) = 0;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     */
    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence) = 0;

    enum class Message {
        INIT = 0,
        DISCONNECT,
        ON_ACCESSIBILITY_EVENT,
        ON_KEY_PRESS_EVENT,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBLE_ABILITY_CLIENT_H