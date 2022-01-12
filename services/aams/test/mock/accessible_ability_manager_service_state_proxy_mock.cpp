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

#include "accessible_ability_manager_service_state_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
int testStateType = -1;
namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceStateProxy::AccessibleAbilityManagerServiceStateProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerServiceState>(impl)
{}

AccessibleAbilityManagerServiceStateProxy::~AccessibleAbilityManagerServiceStateProxy()
{}

bool AccessibleAbilityManagerServiceStateProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
} 

void AccessibleAbilityManagerServiceStateProxy::OnStateChanged(const uint32_t stateType)
{
    printf("OnStateChanged\n");
    testStateType = stateType;
    return;
}

} //namespace Accessibility
} //namespace OHOS