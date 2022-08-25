/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "system_ability_manager_proxy.h"
#include "mock_accessible_ability_manager_service_stub.h"

namespace OHOS {
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
        case ACCESSIBILITY_MANAGER_SERVICE_ID:
            if (!g_MgrService) {
                g_MgrService = new OHOS::Accessibility::MockAccessibleAbilityManagerServiceStub();
            }
            remote = g_MgrService;
            break;
        default:
            GTEST_LOG_(INFO) << "This service is not dummy!!!!" << systemAbilityId;
            break;
    }
    return remote;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::GetSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    (void)deviceId;
    return GetSystemAbility(systemAbilityId);
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    (void)code;
    (void)data;
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, bool& isExist)
{
    (void)systemAbilityId;
    (void)isExist;
    return nullptr;
}

int32_t SystemAbilityManagerProxy::AddOnDemandSystemAbilityInfo(int32_t systemAbilityId,
    const std::u16string& localAbilityManagerName)
{
    (void)systemAbilityId;
    (void)localAbilityManagerName;
    return -1;
}

int32_t SystemAbilityManagerProxy::RemoveSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    (void)code;
    (void)data;
    return -1;
}

int32_t SystemAbilityManagerProxy::RemoveSystemAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return -1;
}

std::vector<std::u16string> SystemAbilityManagerProxy::ListSystemAbilities(unsigned int dumpFlags)
{
    (void)dumpFlags;
    std::vector<std::u16string> saNames;
    return saNames;
}

int32_t SystemAbilityManagerProxy::SubscribeSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityStatusChange>& listener)
{
    (void)systemAbilityId;
    (void)listener;
    return -1;
}

int32_t SystemAbilityManagerProxy::UnSubscribeSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityStatusChange>& listener)
{
    (void)systemAbilityId;
    (void)listener;
    return -1;
}

int32_t SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityLoadCallback>& callback)
{
    (void)systemAbilityId;
    (void)callback;
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemAbility(int32_t systemAbilityId, const sptr<IRemoteObject>& ability,
    const SAExtraProp& extraProp)
{
    (void)systemAbilityId;
    (void)ability;
    (void)extraProp;
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemAbilityWrapper(int32_t code, MessageParcel& data)
{
    (void)code;
    (void)data;
    return -1;
}

int32_t SystemAbilityManagerProxy::AddSystemProcess(
    const std::u16string& procName, const sptr<IRemoteObject>& procObject)
{
    (void)procName;
    (void)procObject;
    return -1;
}
}