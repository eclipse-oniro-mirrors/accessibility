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

#include "accessibility_ui_test_ability.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
std::shared_ptr<AccessibilityUITestAbility> AccessibilityUITestAbility::instance_ = nullptr;
AccessibilityUITestAbility& AccessibilityUITestAbility::GetInstance()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (instance_ == nullptr) {
        std::shared_ptr<AccessibilityUITestAbility> temp(new AccessibilityUITestAbility);
        instance_ = temp;
    }
    return *instance_;
}

AccessibilityUITestAbility::AccessibilityUITestAbility()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    stub_ = new AccessibleAbilityClientStubImpl();
    stub_->SetUITestEnabled();
    GetService();
}

bool AccessibilityUITestAbility::RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);
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

bool AccessibilityUITestAbility::Connect()
{
    HILOG_DEBUG("%{public}s start.", __func__);

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

bool AccessibilityUITestAbility::Disconnect()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DeregisterUITestAbilityConnectionClient();
}

void AccessibilityUITestAbility::GetService()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (serviceProxy_ != nullptr) {
        return;
    }

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
    }
}
} // namespace Accessibility
} // namespace OHOS