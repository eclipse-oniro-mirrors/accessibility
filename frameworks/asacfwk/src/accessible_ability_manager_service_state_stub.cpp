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

#include "accessible_ability_manager_service_state_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "accessibility_system_ability_client.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceStateStub::AccessibleAbilityManagerServiceStateStub()
{
    HILOG_DEBUG("start");
}

int AccessibleAbilityManagerServiceStateStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityManagerServiceStateStub::OnRemoteRequest, cmd = %{public}u, flags= %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerServiceStateStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    if (code == static_cast<uint32_t>(IAccessibleAbilityManagerServiceState::Message::ON_STATE_CHANGED)) {
        return HandleOnStateChanged(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerServiceStateStub::HandleOnStateChanged(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");
    uint32_t stateType = data.ReadUint32();
    OnStateChanged(stateType);

    return NO_ERROR;
}

void AccessibleAbilityManagerServiceStateStub::OnStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType[%{public}d}", stateType);
    std::shared_ptr<AccessibilitySystemAbilityClient>  instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        HILOG_DEBUG("Can't get asac instance");
        return;
    }
    if (stateType & AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED) {
        instance->SetEnabled(true);
    } else {
        instance->SetEnabled(false);
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED) {
        instance->UpdateTouchExplorationEnabled(true);
    } else {
        instance->UpdateTouchExplorationEnabled(false);
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_CAPTION_ENABLED) {
        instance->SetCaptionEnabled(true);
    } else {
        instance->SetCaptionEnabled(false);
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_KEYEVENT_ENABLED) {
        instance->SetKeyEventObserverState(true);
    } else {
        instance->SetKeyEventObserverState(false);
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_GESTURE_ENABLED) {
        instance->SetGestureState(true);
    } else {
        instance->SetGestureState(false);
    }
}
} // namespace Accessibility
} // namespace OHOS