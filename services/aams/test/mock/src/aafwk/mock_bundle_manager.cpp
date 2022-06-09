/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "mock_bundle_manager.h"
#include "ability_config.h"
#include "ability_info.h"
#include "application_info.h"
#include "hilog_wrapper.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace AppExecFwk {
BundleMgrService::BundleMgrService()
{}

BundleMgrService::~BundleMgrService()
{}

int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return 0;
}

bool BundleMgrService::GetBundleInfo(
    const std::string& bundleName, const BundleFlag flag, BundleInfo& bundleInfo, int32_t userId)
{
    (void)bundleName;
    (void)flag;
    (void)bundleInfo;
    (void)userId;
    return true;
}

bool BundleMgrService::QueryAbilityInfo(const AAFwk::Want& want, AbilityInfo& abilityInfo)
{
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo ------------ start");
    if (CheckWantEntity(want, abilityInfo)) {
        HILOG_DEBUG(" mock BundleMgrService CheckWantEntity true ------------ start");
        return true;
    }

    ElementName elementTemp = want.GetElement();
    std::string abilityNameTemp = elementTemp.GetAbilityName();
    std::string bundleNameTemp = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();

    if (bundleNameTemp.empty() || abilityNameTemp.empty()) {
        return false;
    }

    auto fun = abilityInfoMap_.find(bundleNameTemp);
    if (fun != abilityInfoMap_.end()) {
        auto call = fun->second;
        if (call) {
            call(bundleNameTemp, abilityInfo, elementTemp);
            return true;
        }
    }
    if (std::string::npos != elementTemp.GetBundleName().find("service")) {
        abilityInfo.type = AppExecFwk::AbilityType::SERVICE;
    }
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.name = "hello";
    if (want.HasEntity(Want::ENTITY_HOME) && want.GetAction() == Want::ACTION_HOME) {
        abilityInfo.applicationInfo.isLauncherApp = true;
    } else {
        abilityInfo.applicationInfo.isLauncherApp = false;
        abilityInfo.applicationInfo.iconPath = "icon path";
        abilityInfo.applicationInfo.label = "app label";
    }
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo ------------ end");
    return true;
}

bool BundleMgrService::QueryAbilityInfo(const Want& want, int32_t flags, int32_t userId, AbilityInfo& abilityInfo)
{
    (void)want;
    (void)flags;
    (void)userId;
    (void)abilityInfo;
    return true;
}

int BundleMgrService::GetUidByBundleName(const std::string& bundleName, const int userId)
{
    (void)bundleName;
    (void)userId;
    return 0;
}

bool BundleMgrService::CheckIsSystemAppByUid(const int uid)
{
    (void)uid;
    return true;
}

bool BundleMgrService::QueryAbilityInfoByUri(const std::string& uri, AbilityInfo& abilityInfo)
{
    (void)uri;
    (void)abilityInfo;
    return false;
}

bool BundleMgrService::GetApplicationInfo(
    const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo)
{
    (void)flag;
    (void)userId;
    (void)appInfo;
    if (appName.empty()) {
        return false;
    }
    return true;
}

bool BundleMgrService::CheckWantEntity(const AAFwk::Want& want, AbilityInfo& abilityInfo)
{
    (void)abilityInfo;
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------1");
    auto entityVector = want.GetEntities();
    ElementName element = want.GetElement();
    if (entityVector.empty()) {
        return false;
    }

    auto find = false;
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------2");
    for (const auto& entity : entityVector) {
        if (entity == Want::FLAG_HOME_INTENT_FROM_SYSTEM && element.GetAbilityName().empty() &&
            element.GetBundleName().empty()) {
            find = true;
            break;
        }
    }
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------3");
    auto bundleName = element.GetBundleName();
    auto abilityName = element.GetAbilityName();
    if (find || (bundleName == AbilityConfig::SYSTEM_UI_BUNDLE_NAME &&
                    (abilityName == AbilityConfig::SYSTEM_UI_STATUS_BAR ||
                        abilityName == AbilityConfig::SYSTEM_UI_NAVIGATION_BAR))) {
        GTEST_LOG_(INFO) << "QueryAbilityInfo ++> system luncher, find :" << find;
        HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start-----4");
        return true;
    }
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------5");
    return false;
}
} // namespace AppExecFwk
} // namespace OHOS
