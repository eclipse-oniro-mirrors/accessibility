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
#include "ipc_skeleton.h"
#include "mock_bundle_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "mock_accessible_ability_manager_service_stub.h"
#include "mock_service_registry.h"

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
    if (systemAbilityManager_ != nullptr) {
        return systemAbilityManager_;
    }

    systemAbilityManager_ = new SystemAbilityManagerProxy();
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

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId)
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

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    return CheckSystemAbility(systemAbilityId);
}

sptr<IRemoteObject> SystemAbilityManagerProxy::CheckSystemAbility(int32_t systemAbilityId, bool& isExist)
{
    return CheckSystemAbility(systemAbilityId);
}

int32_t SystemAbilityManagerProxy::LoadSystemAbility(int32_t systemAbilityId,
    const sptr<ISystemAbilityLoadCallback>& callback)
{
    return -1;
}
}