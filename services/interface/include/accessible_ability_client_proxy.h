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

#ifndef ACCESSIBLE_ABILITY_CLIENT_PROXY_H
#define ACCESSIBLE_ABILITY_CLIENT_PROXY_H

#include "i_accessible_ability_client.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityClientProxy : public IRemoteProxy<IAccessibleAbilityClient> {
public:
    explicit AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object);
    virtual ~AccessibleAbilityClientProxy() = default;

    /**
     * @brief Init accessible ability through the proxy object.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId) override;

    /**
     * @brief Disconnect accessible ability through the proxy object.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Disconnect(const int32_t channelId) override;

    /**
     * @brief Called when an accessibility event occurs through the proxy object.
     * @param eventInfo The information of accessible event.
     * @return
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override;

    /**
     * @brief Called when a key event occurs through the proxy object.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     * @return
     */
    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence) override;

    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed
     *        through the proxy object.
     * @param displayId The id of display.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    virtual void OnDisplayResized(const int32_t displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY) override;

    /**
     * @brief Called when need to notify the result of simulation gesture through the proxy object.
     * @param sequence The sequence of gesture.
     * @param completedSuccessfully The result of gesture completion.
     * @return
     */
    virtual void OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully) override;

private:
    /**
     * @brief Send the command data from proxy to stub in IPC mechanism.
     * @param code The code matched the function called.
     * @param data Serializable data
     * @param reply The response of IPC
     * @param option The option parameter of IPC,such as: async,sync
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool SendTransactCmd(IAccessibleAbilityClient::Message code, MessageParcel &data,
        MessageParcel &reply,  MessageOption &option);

    bool WriteInterfaceToken(MessageParcel &data);

    static inline BrokerDelegator<AccessibleAbilityClientProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CLIENT_PROXY_H