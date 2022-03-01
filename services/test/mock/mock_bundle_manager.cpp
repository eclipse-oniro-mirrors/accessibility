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

#include "mock_bundle_manager.h"
#include <fstream>
#include <sstream>
#include <string>
#include "ability_config.h"
#include "ability_info.h"
#include "application_info.h"
#include "hilog_wrapper.h"
#include "bundle_mgr_client.h"

using namespace OHOS::AAFwk;
namespace OHOS {
const static std::string AccessibleAbility_JSON_FILE_PATH = "/system/app/dummy_accessibility_ability_config.json";
namespace AppExecFwk {
int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return 0;
}

bool BundleMgrService::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId)
{
    return true;
}

bool BundleMgrService::QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo)
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

bool BundleMgrService::QueryAbilityInfoByUri(const std::string &uri, AbilityInfo &abilityInfo)
{
    return false;
}

bool BundleMgrService::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    if (appName.empty()) {
        return false;
    }
    return true;
}

bool BundleMgrService::CheckWantEntity(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------1");
    auto entityVector = want.GetEntities();
    ElementName element = want.GetElement();
    if (entityVector.empty()) {
        return false;
    }

    auto find = false;
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------2");
    for (const auto &entity : entityVector) {
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
        HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------4");
        return true;
    }
    HILOG_DEBUG(" mock BundleMgrService QueryAbilityInfo CheckWantEntity ------------ start---------5");
    return false;
}

bool BundleMgrService::QueryAbilityInfos(const Want &want, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrService::QueryAbilityInfosForClone(const Want &want, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrService::GetAllFormsInfo(std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrService::GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrService::GetFormsInfoByModule(const std::string &bundleName, const std::string &moduleName,
    std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrService::GetShortcutInfos(const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfos)
{
    return true;
}

bool BundleMgrService::GetModuleUsageRecords(const int32_t number, std::vector<ModuleUsageRecord> &moduleUsageRecords)
{
    return true;
}

bool BundleMgrService::NotifyAbilityLifeStatus(const std::string &bundleName, const std::string &abilityName,
    const int64_t launchTime, const int uid)
{
    return true;
}

BundleMgrClient::BundleMgrClient()
{}

BundleMgrClient::~BundleMgrClient()
{}
bool BundleMgrClient::GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    std::ifstream jsonFileStream;
    jsonFileStream.open(AccessibleAbility_JSON_FILE_PATH, std::ios::in);
    if (!jsonFileStream.is_open()) {
        HILOG_ERROR("Open json file failed.");
        return false;
    }

    std::ostringstream buf;
    char ch;
    while (buf && jsonFileStream.get(ch)) {
        buf.put(ch);
    }
    jsonFileStream.close();

    profileInfos.push_back(buf.str());
    return true;
}
} // namespace AppExecFwk
} // namespace OHOS
