/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "accessibility_common_helper.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "system_ability_manager_proxy.h"
#include "mock_accessible_ability_manager_service_stub.h"

namespace OHOS {
static sptr<AppExecFwk::BundleMgrService> g_bundleMgrService = nullptr;
static sptr<OHOS::Accessibility::MockAccessibleAbilityManagerServiceStub> g_MgrService = nullptr;

SystemAbilityManagerClient& SystemAbilityManagerClient::GetInstance()
{
    static auto instance = new SystemAbilityManagerClient();
    return *instance;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    std::lock_guard<std::mutex> lock(systemAbilityManagerLock_);
    if (systemAbilityManager_ != nullptr) {
        return systemAbilityManager_;
    }

    systemAbilityManager_ = new SystemAbilityManagerProxy(nullptr);
    return systemAbilityManager_;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::GetSystemAbility(int32_t systemAbilityId)
{
    sptr<IRemoteObject> remote = nullptr;
    switch (systemAbilityId) {
        case BUNDLE_MGR_SERVICE_SYS_ABILITY_ID:
            if (!g_bundleMgrService) {
                g_bundleMgrService = new AppExecFwk::BundleMgrService();
            }
            remote = g_bundleMgrService;
            break;
        case ACCESSIBILITY_MANAGER_SERVICE_ID: {
            bool notNullFlag = Accessibility::AccessibilityCommonHelper::GetInstance().GetRemoteObjectNotNullFlag();
            if (notNullFlag) {
                if (!g_MgrService) {
                    g_MgrService = new OHOS::Accessibility::MockAccessibleAbilityManagerServiceStub();
                }
                remote = g_MgrService;
            }
            break;
        }
        default:
            GTEST_LOG_(INFO) << "This service is not dummy!!!!" << systemAbilityId;
            break;
    }
    return remote;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::GetSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    return GetSystemAbility(systemAbilityId);
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId)
{
    return nullptr;
}

int32_t GetOnDemandSystemAbilityIds(std::vector<int32_t>& systemAbilityIds)
{
    return -1;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, bool& isExist)
{
    return nullptr;
}

int32_t SystemAbilityManagerProxy::AddOnDemandSystemAbilityInfo(int32_t systemAbilityId,
    const std::u16string& localAbilityManagerName)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::RemoveSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::RemoveSystemAbility(int32_t systemAbilityId)
{
    return -1;
}

std::vector<std::u16string> SystemAbilityManagerProxy::ListSystemAbilities(unsigned int dumpFlags)
{
    std::vector<std::u16string> saNames;

    return saNames;
}

int32_t SystemAbilityManagerProxy::SubscribeSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityStatusChange>& listener)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::UnSubscribeSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityStatusChange>& listener)
{
    return -1;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId, int32_t timeout)
{
    return nullptr;
}

int32_t SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityLoadCallback>& callback)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId, const std::string& deviceId,
    const sptr<ISystemAbilityLoadCallback>& callback)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::UnloadSystemAbility(int32_t systemAbilityId)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::CancelUnloadSystemAbility(int32_t systemAbilityId)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemAbility(int32_t systemAbilityId, const sptr<IRemoteObject>& ability,
    const SAExtraProp& extraProp)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemProcess(
    const std::u16string& procName, const sptr<IRemoteObject>& procObject)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetSystemProcessInfo(
    int32_t systemAbilityId, SystemProcessInfo& systemProcessInfo)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetRunningSystemProcess(std::list<SystemProcessInfo>& systemProcessInfos)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::SubscribeSystemProcess(const sptr<ISystemProcessStatusChange>& listener)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::UnSubscribeSystemProcess(const sptr<ISystemProcessStatusChange>& listener)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetOnDemandReasonExtraData(int64_t extraDataId, MessageParcel& extraDataParcel)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
    std::vector<SystemAbilityOnDemandEvent>& abilityOnDemandEvents)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::UpdateOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
    const std::vector<SystemAbilityOnDemandEvent>& sabilityOnDemandEvents)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetOnDemandSystemAbilityIds(std::vector<int32_t>& systemAbilityIds)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::SendStrategy(int32_t type, std::vector<int32_t>& systemAbilityIds,
    int32_t level, std::string& action)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::UnloadAllIdleSystemAbility()
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetExtensionSaIds(const std::string& extension, std::vector<int32_t> &saIds)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetExtensionRunningSaList(const std::string& extension,
    std::vector<sptr<IRemoteObject>>& saList)
{
    return -1;
}

int32_t SystemAbilityManagerProxy::GetRunningSaExtensionInfoList(const std::string& extension,
    std::vector<SaExtensionInfo>& infoList)
{
    return -1;
};
sptr<IRemoteObject> SystemAbilityManagerProxy::Recompute(int32_t systemAbilityId, int32_t code)
{
    return nullptr;
}
}