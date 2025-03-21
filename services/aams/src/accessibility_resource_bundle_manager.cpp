/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "accessibility_resource_bundle_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "bundle_info.h"
#include <cstdint>
#include "utils.h"
 
namespace OHOS {
namespace Accessibility {
AccessibilityResourceBundleManager::AccessibilityResourceBundleManager()
{
}
 
AccessibilityResourceBundleManager::~AccessibilityResourceBundleManager()
{
}
 
sptr<AppExecFwk::IBundleMgr> AccessibilityResourceBundleManager::GetBundleMgrProxy()
{
    HILOG_DEBUG();
    if (bundleManager_) {
        return bundleManager_;
    }
 
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        HILOG_ERROR("failed:fail to get system ability mgr.");
        return nullptr;
    }
 
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        HILOG_ERROR("failed:fail to get bundle manager proxy.");
        return nullptr;
    }
 
    bundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (!bundleManager_) {
        HILOG_ERROR("fail to new bundle manager.");
        return nullptr;
    }
 
    if (!bundleManagerDeathRecipient_) {
        bundleManagerDeathRecipient_ = new(std::nothrow) BundleManagerDeathRecipient();
        if (!bundleManagerDeathRecipient_) {
            HILOG_ERROR("bundleManagerDeathRecipient_ is null");
            return nullptr;
        }
    }

    if (!bundleManager_) {
        HILOG_ERROR("bundleManager_ is nullptr");
        return nullptr;
    }
    bundleManager_->AsObject()->AddDeathRecipient(bundleManagerDeathRecipient_);
    return bundleManager_;
}
 
ErrCode AccessibilityResourceBundleManager::GetBundleInfoV9(const std::string& bundleName, int32_t flags,
    AppExecFwk::BundleInfo& bundleInfo, int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        HILOG_ERROR("get bundleMgr failed");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = bundleMgr->GetBundleInfoV9(bundleName, flags, bundleInfo, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("get bundleInfo failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}
 
int AccessibilityResourceBundleManager::GetUidByBundleName(const std::string &bundleName,
    const std::string &abilityName, const int userId)
{
    int result = -1;
    bool ret = true;
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    do {
        if (bundleMgr == nullptr) {
            ret = false;
            break;
        }
        result = bundleMgr->GetUidByBundleName(bundleName, userId);
        if (result == -1) {
            ret = false;
            break;
        }
    } while (0);
    if (ret == false) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("GetUidByBundleName failed");
        return ERR_INVALID_VALUE;
    }
    return result;
}
 
bool AccessibilityResourceBundleManager::QueryExtensionAbilityInfos(
    const AppExecFwk::ExtensionAbilityType &extensionType, const int32_t &userId,
    std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos)
{
    bool result = true;
    bool ret = true;
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    do {
        if (bundleMgr == nullptr) {
            ret = false;
            break;
        }
        result = bundleMgr->QueryExtensionAbilityInfos(extensionType, userId, extensionInfos);
        if (result == false) {
            ret = false;
            break;
        }
    } while (0);
    if (ret == false) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT,
            A11yError::ERROR_QUERY_PACKAGE_INFO_FAILED);
        HILOG_ERROR("QueryExtensionAbilityInfos failed");
        return false;
    }
    return result;
}
 
void AccessibilityResourceBundleManager::BundleManagerDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibilityResourceBundleManager>::GetInstance().OnBundleManagerDied(remote);
}
 
void AccessibilityResourceBundleManager::OnBundleManagerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("OnBundleManagerDied ");
    if (bundleManager_->AsObject() == nullptr) {
        HILOG_ERROR("bundleManager_ is nullptr");
        return;
    }
    bundleManager_->AsObject()->RemoveDeathRecipient(bundleManagerDeathRecipient_);
    bundleManager_ = nullptr;
}
} // namespace Accessibility
} // namespace OHOS