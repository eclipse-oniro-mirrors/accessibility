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
void AccessibleAbilityManagerServiceClientProxy::RegisterElementOperator(int windowId,
        const sptr <IAccessibilityElementOperator> &operation, const int userId)
{}
void AccessibleAbilityManagerServiceClientProxy::DeregisterElementOperator(const int windowId)
{}
sptr<IRemoteObject> AccessibleAbilityManagerServiceClientProxy::GetObject()
{
    return this->AsObject();
}
uint32_t AccessibleAbilityManagerServiceClientProxy::RegisterCaptionPropertyCallback(
        const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId)
{
    return 0;
}
CaptionProperty AccessibleAbilityManagerServiceClientProxy::GetCaptionProperty()
{
    CaptionProperty cap;
    return cap;
}
bool AccessibleAbilityManagerServiceClientProxy::SetCaptionProperty(const CaptionProperty& caption)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetCaptionState(const bool state)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetEnabled(const bool state)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::GetEnabledState()
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::GetCaptionState()
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::GetTouchGuideState()
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::GetGestureState()
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::GetKeyEventObserverState()
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetTouchGuideState(const bool state)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetGestureState(const bool state)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetKeyEventObserverState(const bool state)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    return true;
}
std::map<std::string, AppExecFwk::ElementName> AccessibleAbilityManagerServiceClientProxy::GetEnabledAbilities()
{
    std::map<std::string, AppExecFwk::ElementName> test;
    return test;
}
std::vector<AccessibilityAbilityInfo> AccessibleAbilityManagerServiceClientProxy::GetInstalledAbilities()
{
    std::vector<AccessibilityAbilityInfo> test;
    return test;
}
bool AccessibleAbilityManagerServiceClientProxy::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    return true;
}
bool AccessibleAbilityManagerServiceClientProxy::DeregisterUITestAbilityConnectionClient()
{
    return true;
}
int AccessibleAbilityManagerServiceClientProxy::GetActiveWindow()
{
    return 0;
}
} // namespace Accessibility
} // namespace OHOS
