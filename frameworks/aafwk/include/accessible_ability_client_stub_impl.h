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

#ifndef ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H
#define ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H

#include <memory>
#include "refbase.h"
#include "accessible_ability_client_stub.h"
#include "accessible_ability_event_handler.h"
#include "hilog_wrapper.h"
#include "accessibility_operator.h"
#include "accessible_ability_listener.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityClientStubImpl : public AccessibleAbilityClientStub
{
public:
    AccessibleAbilityClientStubImpl(
        const std::shared_ptr<AccessibleAbilityEventHandler> &accessibleAbilityEventHandler);
    ~AccessibleAbilityClientStubImpl() = default;

    static const uint32_t INVALID_CHANNEL_ID = 0xFFFFFFFF;

    /**
     * @brief Register listener if you want to receive notification.
     * @param listener The listener used to receive notification.
     * @return
     */
    void RegisterListenerImpl(const std::shared_ptr<AccessibleAbilityListener> &listener);

    /**
     * @brief Init accessible ability.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     * @return
     */
    void Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId) override;

    /**
     * @brief Disconnect accessible ability.
     * @param channelId The id of channel.
     * @return
     */
    void Disconnect(const int channelId) override;

    /**
     * @brief Called when an accessibility event occurs. 
     * @param eventInfo The infomation of accessible event.
     * @return
     */
    void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override;

    /**
     * @brief Called when the accessibility service is interrupted.
     * @param
     * @return
     */
    void OnInterrupt() override;

    /**
     * @brief Called when a user performs a specified gesture on the device that
     *        your accessibility application has requested to be in touch exploration mode. 
     * @param gestureId The id of gesture.
     * @return
     */
    void OnGesture(const int gestureId) override;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     * @return
     */
    void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) override;

    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed.
     * @param displayId The id of display.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    void OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY) override;

    /**
     * @brief Called when need to notify the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param completedSuccessfully The result of gesture completion.
     * @return
     */
    void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) override;

    /**
     * @brief Called when the validity status of the fingerprint sensor's gesture detection changes.
     * @param validity The validity status of the fingerprint sensor's gesture detection.
     * @return
     */
    void OnFingerprintGestureValidityChanged(const bool validity) override;

    /**
     * @brief Called when the fingerprint sensor detects a gesture.
     * @param gesture The gesture which is detected by fingerprint sensor.
     * @return
     */ 
    void OnFingerprintGesture(const int gesture) override;

private:
    class AccessibleAbilityDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityDeathRecipient(uint32_t& channelId, sptr<IAccessibleAbilityChannel>& channel)
            : recipientChannelId_(channelId), recipientchannel_(channel) {};
        ~AccessibleAbilityDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote);

        uint32_t& recipientChannelId_;
        sptr<IAccessibleAbilityChannel>& recipientchannel_;
    };

    void InitInner(const sptr<IAccessibleAbilityChannel> &channel, const int channelId);

    void OnAccessibilityEventInner(const AccessibilityEventInfo &eventInfo);

    void OnInterruptInner();

    void OnGestureInner(const int gestureId);

    void OnKeyPressEventInner(const MMI::KeyEvent &keyEvent, const int sequence);

    void OnDisplayResizeChangedInner(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY);

    void OnGestureSimulateResultInner(const int sequence, const bool completedSuccessfully);

    void OnFingerprintGestureValidityChangedInner(const bool validity);

    void OnFingerprintGestureInner(const int gesture);

    sptr<IRemoteObject::DeathRecipient> deathRecipient_{};
    sptr<IAccessibleAbilityChannel> channel_ = nullptr;
    std::shared_ptr<AccessibleAbilityListener> listener_ = nullptr;
    std::shared_ptr<AccessibleAbilityEventHandler> handler_ = nullptr;
    uint32_t channelId_ = INVALID_CHANNEL_ID;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H