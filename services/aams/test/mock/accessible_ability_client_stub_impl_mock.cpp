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

using namespace OHOS::Accessibility;



AccessibleAbilityClientStubImpl::AccessibleAbilityClientStubImpl
    (const std::shared_ptr<AccessibleAbilityEventHandler> &accessibleAbilityEventHandler)
    : handler_(accessibleAbilityEventHandler)
    {}

void AccessibleAbilityClientStubImpl::RegisterListenerImpl(const std::shared_ptr<AccessibleAbilityListener> &listener)
{}

void AccessibleAbilityClientStubImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{}

void AccessibleAbilityClientStubImpl::Disconnect(const int channelId)
{}

void AccessibleAbilityClientStubImpl::InitInner(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    if (!channel) {
        channelId_ = INVALID_CHANNEL_ID;
    } else {
        channelId_ = channelId;
    }
}

void AccessibleAbilityClientStubImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{}

void AccessibleAbilityClientStubImpl::OnAccessibilityEventInner(const AccessibilityEventInfo &eventInfo)
{
    if (channelId_ != INVALID_CHANNEL_ID) {
        if (!listener_) {
            return;
        }
    }
}

void AccessibleAbilityClientStubImpl::OnInterrupt()
{}

void AccessibleAbilityClientStubImpl::AccessibleAbilityClientStubImpl::OnInterruptInner()
{}

void AccessibleAbilityClientStubImpl::OnGesture(const int gestureId)
{}

void AccessibleAbilityClientStubImpl::OnGestureInner(const int gestureId)
{}

void AccessibleAbilityClientStubImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{}

void AccessibleAbilityClientStubImpl::OnKeyPressEventInner(const MMI::KeyEvent &keyEvent, const int sequence)
{}

void AccessibleAbilityClientStubImpl::OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale,
                                                             const float centerX, const float centerY)
{}

void AccessibleAbilityClientStubImpl::OnDisplayResizeChangedInner(const int displayId, const Rect &rect,
                                                                  const float scale, const float centerX, const float centerY)
{}

void AccessibleAbilityClientStubImpl::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{}

void AccessibleAbilityClientStubImpl::OnGestureSimulateResultInner(const int sequence, const bool completedSuccessfully)
{}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureValidityChanged(const bool validity)
{}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureValidityChangedInner(const bool validity)
{}

void AccessibleAbilityClientStubImpl::OnFingerprintGesture(const int gesture)
{}

void AccessibleAbilityClientStubImpl::OnFingerprintGestureInner(const int gesture)
{}