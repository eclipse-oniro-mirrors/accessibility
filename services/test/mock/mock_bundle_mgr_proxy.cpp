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

#include "bundle_mgr_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
BundleMgrProxy::BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgr>(impl)
{
}

BundleMgrProxy::~BundleMgrProxy()
{
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    return true;
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string &appName, int32_t flags, int32_t userId, ApplicationInfo &appInfo)
{
    return true;
}

bool BundleMgrProxy::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos)
{
    return true;
}

bool BundleMgrProxy::GetApplicationInfos(
    int32_t flags, int32_t userId, std::vector<ApplicationInfo> &appInfos)
{
    return true;
}

bool BundleMgrProxy::GetBundleInfo(
    const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::GetBundleInfo(
    const std::string &bundleName, int32_t flags, BundleInfo &bundleInfo, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::GetBundleInfos(
    const BundleFlag flag, std::vector<BundleInfo> &bundleInfos, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::GetBundleInfos(
    int32_t flags, std::vector<BundleInfo> &bundleInfos, int32_t userId)
{
    return true;
}

int BundleMgrProxy::GetUidByBundleName(const std::string &bundleName, const int userId)
{
    return 0;
}

std::string BundleMgrProxy::GetAppIdByBundleName(const std::string &bundleName, const int userId)
{
    std::string appId = Constants::EMPTY_STRING;
    return appId;
}

bool BundleMgrProxy::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    return true;
}

bool BundleMgrProxy::GetBundlesForUid(const int uid, std::vector<std::string> &bundleNames)
{
    return true;
}

bool BundleMgrProxy::GetNameForUid(const int uid, std::string &name)
{
    return true;
}

bool BundleMgrProxy::GetBundleGids(const std::string &bundleName, std::vector<int> &gids)
{
    return true;
}

bool BundleMgrProxy::GetBundleGidsByUid(const std::string &bundleName, const int &uid, std::vector<int> &gids)
{
    return true;
}

std::string BundleMgrProxy::GetAppType(const std::string &bundleName)
{
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::CheckIsSystemAppByUid(const int uid)
{
    return true;
}

bool BundleMgrProxy::GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfo(const Want &want, int32_t flags, int32_t userId, AbilityInfo &abilityInfo)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfos(const Want &want, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfos(
    const Want &want, int32_t flags, int32_t userId, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAllAbilityInfos(const Want &want, int32_t userId, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfosForClone(const Want &want, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfosByUri(const std::string &abilityUri, std::vector<AbilityInfo> &abilityInfos)
{
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(
    const std::string &abilityUri, int32_t userId, AbilityInfo &abilityInfo)
{
    return true;
}

bool BundleMgrProxy::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos)
{
    return true;
}

std::string BundleMgrProxy::GetAbilityLabel(const std::string &bundleName, const std::string &className)
{
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::GetBundleArchiveInfo(const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo)
{
    return true;
}

bool BundleMgrProxy::GetBundleArchiveInfo(const std::string &hapFilePath, int32_t flags, BundleInfo &bundleInfo)
{
    return true;
}

bool BundleMgrProxy::GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo)
{
    return true;
}

bool BundleMgrProxy::GetLaunchWantForBundle(const std::string &bundleName, Want &want)
{
    return true;
}

int BundleMgrProxy::CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName)
{
    return 0;
}

int BundleMgrProxy::CheckPermission(const std::string &bundleName, const std::string &permission)
{
    return 0;
}

int BundleMgrProxy::CheckPermissionByUid(const std::string &bundleName, const std::string &permission, const int userId)
{
    return 0;
}

bool BundleMgrProxy::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    return true;
}

bool BundleMgrProxy::GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs)
{
    return true;
}

bool BundleMgrProxy::GetAppsGrantedPermissions(
    const std::vector<std::string> &permissions, std::vector<std::string> &appNames)
{
    return true;
}

bool BundleMgrProxy::HasSystemCapability(const std::string &capName)
{
    return true;
}

bool BundleMgrProxy::GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps)
{
    return true;
}

bool BundleMgrProxy::IsSafeMode()
{
    return true;
}

bool BundleMgrProxy::CleanBundleCacheFiles(
    const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::CleanBundleDataFiles(const std::string &bundleName, const int userId)
{
    return true;
}

bool BundleMgrProxy::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    return true;
}

bool BundleMgrProxy::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    return true;
}

bool BundleMgrProxy::UnregisterBundleStatusCallback()
{
    return true;
}

bool BundleMgrProxy::DumpInfos(
    const DumpFlag flag, const std::string &bundleName, int32_t userId, std::string &result)
{
    return true;
}

bool BundleMgrProxy::IsApplicationEnabled(const std::string &bundleName)
{
    return true;
}

bool BundleMgrProxy::SetApplicationEnabled(const std::string &bundleName, bool isEnable, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::IsAbilityEnabled(const AbilityInfo &abilityInfo)
{
    return true;
}

bool BundleMgrProxy::SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnabled, int32_t userId)
{
    return true;
}

std::string BundleMgrProxy::GetAbilityIcon(const std::string &bundleName, const std::string &className)
{
    return Constants::EMPTY_STRING;
}

sptr<IBundleInstaller> BundleMgrProxy::GetBundleInstaller()
{
    return nullptr;
}

sptr<IBundleUserMgr> BundleMgrProxy::GetBundleUserMgr()
{
    return nullptr;
}

bool BundleMgrProxy::CanRequestPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    return true;
}

bool BundleMgrProxy::RequestPermissionFromUser(
    const std::string &bundleName, const std::string &permission, const int userId)
{
    return true;
}

bool BundleMgrProxy::RegisterAllPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback)
{
    return true;
}

bool BundleMgrProxy::RegisterPermissionsChanged(
    const std::vector<int> &uids, const sptr<OnPermissionChangedCallback> &callback)
{
    return true;
}

bool BundleMgrProxy::UnregisterPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback)
{
    return true;
}

bool BundleMgrProxy::GetAllFormsInfo(std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrProxy::GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrProxy::GetFormsInfoByModule(
    const std::string &bundleName, const std::string &moduleName, std::vector<FormInfo> &formInfos)
{
    return true;
}

bool BundleMgrProxy::GetShortcutInfos(const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfos)
{
    return true;
}

bool BundleMgrProxy::GetAllCommonEventInfo(const std::string &eventKey, std::vector<CommonEventInfo> &commonEventInfos)
{
    return true;
}

bool BundleMgrProxy::GetModuleUsageRecords(const int32_t number, std::vector<ModuleUsageRecord> &moduleUsageRecords)
{
    return true;
}

bool BundleMgrProxy::NotifyAbilityLifeStatus(
    const std::string &bundleName, const std::string &abilityName, const int64_t launchTime, const int uid)
{
    return true;
}

bool BundleMgrProxy::CheckBundleNameInAllowList(const std::string &bundleName)
{
    return true;
}

bool BundleMgrProxy::BundleClone(const std::string &bundleName)
{
    return true;
}

bool BundleMgrProxy::RemoveClonedBundle(const std::string &bundleName, const int32_t uid)
{
    return true;
}

bool BundleMgrProxy::GetDistributedBundleInfo(
    const std::string &networkId, int32_t userId, const std::string &bundleName,
    DistributedBundleInfo &distributedBundleInfo)
{
    return true;
}

std::string BundleMgrProxy::GetAppPrivilegeLevel(const std::string &bundleName, int32_t userId)
{
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(const Want &want, const int32_t &flag, const int32_t &userId,
    std::vector<ExtensionAbilityInfo> &extensionInfos)
{
    return true;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(const Want &want, const ExtensionAbilityType &extensionType,
    const int32_t &flag, const int32_t &userId, std::vector<ExtensionAbilityInfo> &extensionInfos)
{
    return true;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(const ExtensionAbilityType &extensionType, const int32_t &userId,
    std::vector<ExtensionAbilityInfo> &extensionInfos)
{
    return true;
}

std::shared_ptr<Media::PixelMap> BundleMgrProxy::GetAbilityPixelMapIcon(const std::string &bundleName,
    const std::string &abilityName)
{
    return nullptr;
}

template<typename T>
bool BundleMgrProxy::GetParcelableInfo(IBundleMgr::Message code, MessageParcel &data, T &parcelableInfo)
{
    return true;
}

template<typename T>
bool BundleMgrProxy::GetParcelableInfos(IBundleMgr::Message code, MessageParcel &data, std::vector<T> &parcelableInfos)
{
    return true;
}

bool BundleMgrProxy::SendTransactCmd(IBundleMgr::Message code, MessageParcel &data, MessageParcel &reply)
{
    return true;
}

bool BundleMgrProxy::VerifyCallingPermission(const std::string &permission)
{
    return true;
}

std::vector<std::string> BundleMgrProxy::GetAccessibleAppCodePaths(int32_t userId)
{
    std::vector<std::string> vec;
    return vec;
}

bool BundleMgrProxy::QueryExtensionAbilityInfoByUri(const std::string &uri, int32_t userId,
    ExtensionAbilityInfo &extensionAbilityInfo)
{
    return true;
}
} // namespace AppExecFwk
} // namespace OHOS