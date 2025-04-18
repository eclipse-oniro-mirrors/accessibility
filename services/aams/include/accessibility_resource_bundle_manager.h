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
#ifndef ACCESSIBILITY_RESOURCE_BUNDLE_MANAGER_H
#define ACCESSIBILITY_RESOURCE_BUNDLE_MANAGER_H

#include "singleton.h"
#include "resource_manager.h"
#include "res_config.h"
#include "bundle_info.h"
#include "locale_config.h"
#include "locale_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "iaccessible_ability_manager_service.h"
#include "iremote_object.h"
#include "ffrt.h"
  
namespace OHOS {
namespace Accessibility {

class AccessibilityResourceBundleManager {
public:
    AccessibilityResourceBundleManager();
    ~AccessibilityResourceBundleManager();

    ErrCode GetBundleInfoV9(const std::string& bundleName, int32_t flags,
        AppExecFwk::BundleInfo& bundleInfo, int32_t userId);

    int GetUidByBundleName(const std::string &bundleName, const std::string &abilityName, const int userId);

    bool QueryExtensionAbilityInfos(const AppExecFwk::ExtensionAbilityType &extensionType, const int32_t &userId,
        std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos);

    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();

public:
    ffrt::mutex bundleMutex_;

private:
    class BundleManagerDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        BundleManagerDeathRecipient() = default;
        ~BundleManagerDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(BundleManagerDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    void OnBundleManagerDied(const wptr<IRemoteObject> &remote);

    sptr<AppExecFwk::IBundleMgr> bundleManager_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> bundleManagerDeathRecipient_ = nullptr;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBILITY_RESOURCE_BUNDLE_MANAGER_H