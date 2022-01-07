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

#include <string>
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability.h"
#include "display_resize_controller.h"
#include "fingerprint_controller.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

const string TASK_INIT = "Init";
const string TASK_DISCONNECT = "Disconnect";
const string TASK_ON_ACCESSIBILITY_EVENT = "OnAccessibilityEvent";
const string TASK_ON_INTERRUPT = "OnInterrupt";
const string TASK_ON_GESTURE = "OnGesture";
const string TASK_ON_KEY_PRESS_EVENT = "OnKeyPressEvent";
const string TASK_ON_DISPLAYRESIZE_CHANGED = "OnDisplayResizeChanged";
const string TASK_ON_GESTURE_SIMULATE_RESULT = "OnGestureSimulateResult";
const string TASK_ON_FINGERPRINT_GESTURE_VALIDITY_CHANGED = "OnFingerprintGestureValidityChanged";
const string TASK_ON_FINGERPRINT_GESTURE = "OnFingerprintGesture";

AccessibleAbilityClientStubImpl::AccessibleAbilityClientStubImpl
    (const std::shared_ptr<AccessibleAbilityEventHandler> &accessibleAbilityEventHandler)
    : handler_(accessibleAbilityEventHandler)
    {}

void AccessibleAbilityClientStubImpl::RegisterListenerImpl(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG("Register AccessibleAbilityClientStubImpl listener.");
    if (listener_) {
        HILOG_DEBUG("listener already exists.");
        return;
    }

    listener_ = listener;
}

void AccessibleAbilityClientStubImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::function<void()> initFunc = std::bind(&AccessibleAbilityClientStubImpl::InitInner, this, channel, channelId);
    handler_->PostTask(initFunc, TASK_INIT);
}

void AccessibleAbilityClientStubImpl::Disconnect(const int channelId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::function<void()> disconnectFunc = std::bind(&AccessibleAbilityClientStubImpl::InitInner,
                                            this, nullptr, channelId);
    handler_->PostTask(disconnectFunc, TASK_DISCONNECT);
}

void AccessibleAbilityClientStubImpl::InitInner(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!channel) {
        HILOG_DEBUG("RemoveChannel start.");

        // Delete death recipient
        if (channel_ && channel_->AsObject()) {
            HILOG_ERROR("Remove death recipient");
            channel_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        }

        // Remove channel
        AccessibilityOperator::RemoveChannel(channelId);
        channelId_ = INVALID_CHANNEL_ID;
        AccessibleAbility::GetInstance().SetChannelId(channelId_);
        channel_ = channel;
    } else {
        HILOG_DEBUG("AddChannel start.");
        AccessibilityOperator::AddChannel(channelId, channel);
        channelId_ = channelId;
        AccessibleAbility::GetInstance().SetChannelId(channelId_);
        channel_ = channel;

        // Add death recipient
        if (!deathRecipient_) {
            deathRecipient_ = new AccessibleAbilityDeathRecipient(channelId_, channel_);
        }

        auto object = channel_->AsObject();
        if (object) {
            HILOG_DEBUG("Add death recipient");
            object->AddDeathRecipient(deathRecipient_);
        }

        // Add listener of ability
        if (!listener_) {
            HILOG_ERROR("listener is nullptr.");
            return;
        }
        listener_->OnAbilityConnected();
    }
}

void AccessibleAbilityClientStubImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onAccessibilityEventFunc = std::bind(
            &AccessibleAbilityClientStubImpl::OnAccessibilityEventInner, this, eventInfo);
        handler_->PostTask(onAccessibilityEventFunc, TASK_ON_ACCESSIBILITY_EVENT);
    }
}

void AccessibleAbilityClientStubImpl::OnAccessibilityEventInner(const AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener_) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }

    listener_->OnAccessibilityEvent(eventInfo);
}

void AccessibleAbilityClientStubImpl::OnInterrupt()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> interruptFunc = std::bind(&AccessibleAbilityClientStubImpl::OnInterruptInner, this);
        handler_->PostTask(interruptFunc, TASK_ON_INTERRUPT);
    }
}

void AccessibleAbilityClientStubImpl::AccessibleAbilityClientStubImpl::OnInterruptInner()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener_) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }
    listener_->OnInterrupt();
}

void AccessibleAbilityClientStubImpl::OnGesture(const int gestureId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onGestureFunc = std::bind(&AccessibleAbilityClientStubImpl::OnGestureInner,
                                                this, gestureId);
        handler_->PostTask(onGestureFunc, TASK_ON_GESTURE);
    }
}

void AccessibleAbilityClientStubImpl::OnGestureInner(const int gestureId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener_) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }
    listener_->OnGesture(gestureId);
}

void AccessibleAbilityClientStubImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    // TODO: fixme
    // std::function<void()> onKeyPressEventFunc = std::bind(
    //     &AccessibleAbilityClientStubImpl::OnKeyPressEventInner, this, keyEvent, sequence);
    // handler_->PostTask(onKeyPressEventFunc, TASK_ON_KEY_PRESS_EVENT);
}

void AccessibleAbilityClientStubImpl::OnKeyPressEventInner(const MMI::KeyEvent &keyEvent, const int sequence)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener_) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }

    bool handled = listener_->OnKeyPressEvent(keyEvent);
    AccessibilityOperator::GetInstance().SetOnKeyPressEventResult(channelId_, handled, sequence);
}

void AccessibleAbilityClientStubImpl::OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale,
                                                             const float centerX, const float centerY)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onDisplayResizeChangedFunc = std::bind(
            &AccessibleAbilityClientStubImpl::OnDisplayResizeChangedInner, this,
            displayId, rect, scale, centerX, centerY);
        handler_->PostTask(onDisplayResizeChangedFunc, TASK_ON_DISPLAYRESIZE_CHANGED);
    }
}

void AccessibleAbilityClientStubImpl::OnDisplayResizeChangedInner(const int displayId, const Rect &rect,
    const float scale, const float centerX, const float centerY)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    shared_ptr<DisplayResizeController> displayResizeController =
        AccessibleAbility::GetInstance().GetDisplayResizeController(displayId);
    if (!displayResizeController) {
        HILOG_ERROR("There is no displayResizeController.");
        return;
    }

    displayResizeController->DispatchOnDisplayResizeChanged(rect, scale, centerX, centerY);
}

void AccessibleAbilityClientStubImpl::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onGestureSimulateResultFunc = std::bind(
            &AccessibleAbilityClientStubImpl::OnGestureSimulateResultInner, this, sequence, completedSuccessfully);
        handler_->PostTask(onGestureSimulateResultFunc, TASK_ON_GESTURE_SIMULATE_RESULT);
    }
}

void AccessibleAbilityClientStubImpl::OnGestureSimulateResultInner(const int sequence,
    const bool completedSuccessfully)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    AccessibleAbility::GetInstance().DispatchOnSimulationGestureResult(sequence, completedSuccessfully);
}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureValidityChanged(const bool validity)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onFingerprintGestureValidityChangedFunc = std::bind(
            &AccessibleAbilityClientStubImpl::OnFingerprintGestureValidityChangedInner, this, validity);
        handler_->PostTask(onFingerprintGestureValidityChangedFunc, TASK_ON_FINGERPRINT_GESTURE_VALIDITY_CHANGED);
    }
}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureValidityChangedInner(const bool validity)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    shared_ptr<FingerprintController> fingerprintController =
        AccessibleAbility::GetInstance().GetFingerprintController();
    if (!fingerprintController) {
        HILOG_ERROR("There is no fingerprintController.");
        return;
    }

    fingerprintController->DispatchOnFingerprintGestureStatusChanged(validity);
}

void AccessibleAbilityClientStubImpl::OnFingerprintGesture(const int gesture)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (channelId_ != INVALID_CHANNEL_ID) {
        std::function<void()> onFingerprintGestureFunc = std::bind(
            &AccessibleAbilityClientStubImpl::OnFingerprintGestureInner, this, gesture);
        handler_->PostTask(onFingerprintGestureFunc, TASK_ON_FINGERPRINT_GESTURE);
    }
}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureInner(const int gesture)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    shared_ptr<FingerprintController> fingerprintController =
        AccessibleAbility::GetInstance().GetFingerprintController();
    if (!fingerprintController) {
        HILOG_ERROR("There is no fingerprintController.");
        return;
    }

    fingerprintController->DispatchOnFingerprintGesture(gesture);
}

void AccessibleAbilityClientStubImpl::AccessibleAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    // Delete death recipient
    remote->RemoveDeathRecipient(this);

    // Remove channel
    AccessibilityOperator::RemoveChannel(recipientChannelId_);
    recipientChannelId_ = INVALID_CHANNEL_ID;
    AccessibleAbility::GetInstance().SetChannelId(recipientChannelId_);
    recipientchannel_ = nullptr;
}

} // namespace Accessibility
} // namespace OHOS