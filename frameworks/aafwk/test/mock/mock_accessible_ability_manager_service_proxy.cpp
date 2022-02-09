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

#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_errorcode.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceClientProxy::AccessibleAbilityManagerServiceClientProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerServiceClient>(impl)
{}

AccessibleAbilityManagerServiceClientProxy::~AccessibleAbilityManagerServiceClientProxy()
{}

bool AccessibleAbilityManagerServiceClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibleAbilityManagerServiceClientProxy::SendEvent(const AccessibilityEventInfo &uiEvent, const int userId)
{}

uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterStateCallback(
    const sptr<IAccessibleAbilityManagerServiceState> &client, const int userId)
{
    return 0;
}

std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetAbilityList(const int abilityTypes,
    const int stateType)
{
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    return abilityInfos;
}

void AccessibleAbilityManagerServiceClientProxy::RegisterInteractionOperation(int windowId,
        const sptr <IAccessibilityInteractionOperation> &operation, const int userId)
{}

void AccessibleAbilityManagerServiceClientProxy::DeregisterInteractionOperation(const int windowId)
{}

void AccessibleAbilityManagerServiceClientProxy::Interrupt(const int userId)
{}

uint64_t AccessibleAbilityManagerServiceClientProxy::GetSuggestedInterval()
{
    return 0;
}

void AccessibleAbilityManagerServiceClientProxy::RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj)
{}

sptr<IRemoteObject> AccessibleAbilityManagerServiceClientProxy::GetObject()
{
    return this->AsObject();
}

} //namespace Accessibility
} //namespace OHOS
