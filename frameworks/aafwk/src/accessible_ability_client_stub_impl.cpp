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

#include "accessible_ability_client_stub_impl.h"
#include "accessibility_extension_context.h"
#include "accessibility_ui_test_ability.h"
#include "display_resize_controller.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
void AccessibleAbilityClientStubImpl::RegisterListenerImpl(const std::shared_ptr<AccessibilityExtension> &listener)
{
    HILOG_DEBUG("Register AccessibleAbilityClientStubImpl listener.");
    if (listener_) {
        HILOG_DEBUG("listener already exists.");
        return;
    }

    listener_ = listener;
}

void AccessibleAbilityClientStubImpl::SetUITestEnabled()
{
    HILOG_DEBUG("start.");
    uiTestEnabled_ = true;
}

bool AccessibleAbilityClientStubImpl::RegisterUITestAbilityListener(
    const std::shared_ptr<IAccessibleUITestAbilityListener> &listener)
{
    HILOG_DEBUG("start.");
    if (uiTestListener_) {
        HILOG_DEBUG("listener already exists.");
        return false;
    }

    uiTestListener_ = listener;
    return true;
}

void AccessibleAbilityClientStubImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    HILOG_DEBUG("start.");
    if (!uiTestEnabled_) {
        if (!channel) {
            HILOG_DEBUG("channel is nullptr.");
            return;
        }
        if (!listener_ || !listener_->GetContext()) {
            HILOG_ERROR("listener_ is nullptr or there is no context in listener_.");
            return;
        }

        listener_->GetContext()->SetChannelId(channelId);
        AccessibilityOperator::AddChannel(channelId, channel);
        channelId_ = channelId;
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

        listener_->OnAbilityConnected();
    } else {
        if (!channel) {
            HILOG_DEBUG("channel is nullptr.");
            return;
        }
        if (!uiTestListener_) {
            HILOG_ERROR("listener_ is nullptr.");
            return;
        }

        auto instance = AccessibilityUITestAbility::GetInstance();
        if (!instance) {
            HILOG_ERROR("instance is nullptr");
            return;
        }
        instance->SetChannelId(channelId);
        AccessibilityOperator::AddChannel(channelId, channel);
        channelId_ = channelId;
        channel_ = channel;

        // Add death recipient
        if (!deathRecipient_) {
            deathRecipient_ = new AccessibleAbilityDeathRecipient(channelId_, channel_);
        }

        auto uiTestObject = channel_->AsObject();
        if (uiTestObject) {
            HILOG_DEBUG("Add death recipient");
            uiTestObject->AddDeathRecipient(deathRecipient_);
        }

        uiTestListener_->OnAbilityConnected();
    }
}

void AccessibleAbilityClientStubImpl::Disconnect(const int channelId)
{
    HILOG_DEBUG("start.");

    // Delete death recipient
    if (channel_ && channel_->AsObject()) {
        HILOG_ERROR("Remove death recipient");
        channel_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }

    // Remove channel
    AccessibilityOperator::RemoveChannel(channelId);
    channelId_ = INVALID_CHANNEL_ID;
    channel_ = nullptr;
    if (!uiTestEnabled_) {
        if (listener_ && listener_->GetContext()) {
            HILOG_DEBUG("Clear extensionContext channelId.");
            listener_->GetContext()->SetChannelId(channelId_);
            listener_ = nullptr;
        }
    } else {
        auto instance = AccessibilityUITestAbility::GetInstance();
        if (!instance) {
            HILOG_ERROR("instance is nullptr");
            return;
        }
        instance->SetChannelId(channelId_);
        if (uiTestListener_) {
            uiTestListener_->OnAbilityDisconnected();
            uiTestListener_ = nullptr;
        }
    }
}

void AccessibleAbilityClientStubImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG("start.");
    if (channelId_ != INVALID_CHANNEL_ID) {
        if (listener_) {
            listener_->OnAccessibilityEvent(eventInfo);
        }

        if (uiTestListener_) {
            uiTestListener_->OnAccessibilityEvent(eventInfo);
        }
    }
}

void AccessibleAbilityClientStubImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{
    HILOG_DEBUG("start.");
    if (listener_) {
        bool handled = listener_->OnKeyPressEvent(keyEvent);
        AccessibilityOperator::GetInstance().SetOnKeyPressEventResult(channelId_, handled, sequence);
    }

    if (uiTestListener_) {
        std::shared_ptr<MMI::KeyEvent> tmp = std::make_shared<MMI::KeyEvent>(keyEvent);
        bool uiTestHandled = uiTestListener_->OnKeyPressEvent(tmp, sequence);
        AccessibilityOperator::GetInstance().SetOnKeyPressEventResult(channelId_, uiTestHandled, sequence);
    }
}

void AccessibleAbilityClientStubImpl::OnDisplayResized(const int displayId, const Rect &rect, const float scale,
    const float centerX, const float centerY)
{
    HILOG_DEBUG("start.");
    if (channelId_ != INVALID_CHANNEL_ID) {
        if (listener_ && listener_->GetContext()) {
            HILOG_DEBUG("Get displayResize controller.");
            shared_ptr<DisplayResizeController> displayResizeController =
                listener_->GetContext()->GetDisplayResizeController(displayId);
            if (!displayResizeController) {
                HILOG_ERROR("There is no displayResizeController.");
                return;
            }

            displayResizeController->DispatchOnDisplayResized(rect, scale, centerX, centerY);
        }
    }
}

void AccessibleAbilityClientStubImpl::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{
    HILOG_DEBUG("start.");
    if (channelId_ != INVALID_CHANNEL_ID) {
        if (listener_ && listener_->GetContext()) {
            HILOG_DEBUG("Dispatch the result of simulation gesture.");
            listener_->GetContext()->DispatchOnSimulationGestureResult(sequence, completedSuccessfully);
        }

        if (uiTestEnabled_) {
            HILOG_DEBUG("Dispatch the result of simulation gesture.");
            auto instance = AccessibilityUITestAbility::GetInstance();
            if (!instance) {
                HILOG_ERROR("instance is nullptr");
                return;
            }
            instance->DispatchOnSimulationGestureResult(sequence, completedSuccessfully);
        }
    }
}

void AccessibleAbilityClientStubImpl::AccessibleAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start.");

    // Delete death recipient
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is nullptr.");
        return;
    }

    if (!recipientchannel_ || (recipientchannel_->AsObject() != remote)) {
        HILOG_ERROR("recipientchannel_ is nullptr or remote is wrong.");
        return;
    }
    remote->RemoveDeathRecipient(this);

    // Remove channel
    AccessibilityOperator::RemoveChannel(recipientChannelId_);
    recipientChannelId_ = INVALID_CHANNEL_ID;
    recipientchannel_ = nullptr;
}
} // namespace Accessibility
} // namespace OHOS