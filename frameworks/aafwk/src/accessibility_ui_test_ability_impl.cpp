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

#include "accessibility_ui_test_ability_impl.h"

#include <mutex>
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
static std::mutex g_Mutex;
static std::shared_ptr<AccessibilityUITestAbilityImpl> g_Instance = nullptr;

std::shared_ptr<AccessibilityUITestAbility> AccessibilityUITestAbility::GetInstance()
{
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (g_Instance == nullptr) {
        g_Instance = std::make_shared<AccessibilityUITestAbilityImpl>();
    }
    return g_Instance;
}

AccessibilityUITestAbilityImpl::AccessibilityUITestAbilityImpl()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return;
    }
    HILOG_DEBUG("ISystemAbilityManager obtained");

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerServiceClient object from samgr failed");
        return;
    }
    HILOG_DEBUG("Get remote object ok");

    serviceProxy_ = iface_cast<AccessibleAbilityManagerServiceClientProxy>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("Get aams proxy failed");
        return;
    }
    
    stub_ = new AccessibleAbilityClientStubImpl();
    stub_->SetUITestEnabled();
}

bool AccessibilityUITestAbilityImpl::RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::lock_guard<std::mutex> lock(g_Mutex);

    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return false;
    }

    if (!stub_) {
        HILOG_ERROR("AccessibleAbility::stub_ is nullptr");
        return false;
    }
    return stub_->RegisterUITestAbilityListener(listener);
}

bool AccessibilityUITestAbilityImpl::Connect()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::lock_guard<std::mutex> lock(g_Mutex);

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    if (!stub_) {
        HILOG_ERROR("stub_ is nullptr");
        return false;
    }
    return serviceProxy_->RegisterUITestAbilityConnectionClient(stub_);
}

bool AccessibilityUITestAbilityImpl::Disconnect()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::lock_guard<std::mutex> lock(g_Mutex);

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DeregisterUITestAbilityConnectionClient();
}
} // namespace Accessibility
} // namespace OHOS