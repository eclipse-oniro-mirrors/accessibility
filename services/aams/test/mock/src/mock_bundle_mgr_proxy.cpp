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

#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
BundleMgrProxy::BundleMgrProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IBundleMgr>(impl)
{
    (void)impl;
}

BundleMgrProxy::~BundleMgrProxy()
{}

bool BundleMgrProxy::GetAbilityInfo(
    const std::string& bundleName, const std::string& abilityName, AbilityInfo& abilityInfo)
{
    (void)bundleName;
    (void)abilityName;
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo)
{
    (void)appName;
    (void)flag;
    (void)userId;
    (void)appInfo;
    return true;
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string& appName, int32_t flags, int32_t userId, ApplicationInfo& appInfo)
{
    (void)appName;
    (void)flags;
    (void)userId;
    (void)appInfo;
    return true;
}

bool BundleMgrProxy::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo>& appInfos)
{
    (void)flag;
    (void)userId;
    (void)appInfos;
    return true;
}

bool BundleMgrProxy::GetApplicationInfos(int32_t flags, int32_t userId, std::vector<ApplicationInfo>& appInfos)
{
    (void)flags;
    (void)userId;
    (void)appInfos;
    return true;
}

bool BundleMgrProxy::GetBundleInfo(
    const std::string& bundleName, const BundleFlag flag, BundleInfo& bundleInfo, int32_t userId)
{
    (void)bundleName;
    (void)flag;
    (void)bundleInfo;
    (void)userId;
    return true;
}

bool BundleMgrProxy::GetBundleInfo(const std::string& bundleName, int32_t flags, BundleInfo& bundleInfo, int32_t userId)
{
    (void)bundleName;
    (void)flags;
    (void)bundleInfo;
    (void)userId;
    return true;
}

bool BundleMgrProxy::GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo>& bundleInfos, int32_t userId)
{
    (void)flag;
    (void)bundleInfos;
    (void)userId;
    return true;
}

bool BundleMgrProxy::GetBundleInfos(int32_t flags, std::vector<BundleInfo>& bundleInfos, int32_t userId)
{
    (void)flags;
    (void)bundleInfos;
    (void)userId;
    return true;
}

int BundleMgrProxy::GetUidByBundleName(const std::string& bundleName, const int userId)
{
    (void)bundleName;
    (void)userId;
    return 0;
}

std::string BundleMgrProxy::GetAppIdByBundleName(const std::string& bundleName, const int userId)
{
    (void)bundleName;
    (void)userId;
    std::string appId = Constants::EMPTY_STRING;
    return appId;
}

bool BundleMgrProxy::GetBundleNameForUid(const int uid, std::string& bundleName)
{
    (void)uid;
    (void)bundleName;
    return true;
}

bool BundleMgrProxy::GetBundlesForUid(const int uid, std::vector<std::string>& bundleNames)
{
    (void)uid;
    (void)bundleNames;
    return true;
}

bool BundleMgrProxy::GetNameForUid(const int uid, std::string& name)
{
    (void)name;
    (void)uid;
    return true;
}

bool BundleMgrProxy::GetBundleGids(const std::string& bundleName, std::vector<int>& gids)
{
    (void)bundleName;
    (void)gids;
    return true;
}

bool BundleMgrProxy::GetBundleGidsByUid(const std::string& bundleName, const int& uid, std::vector<int>& gids)
{
    (void)bundleName;
    (void)uid;
    (void)gids;
    return true;
}

std::string BundleMgrProxy::GetAppType(const std::string& bundleName)
{
    (void)bundleName;
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::CheckIsSystemAppByUid(const int uid)
{
    (void)uid;
    return true;
}

bool BundleMgrProxy::GetBundleInfosByMetaData(const std::string& metaData, std::vector<BundleInfo>& bundleInfos)
{
    (void)metaData;
    (void)bundleInfos;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfo(const Want& want, AbilityInfo& abilityInfo)
{
    (void)want;
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfo(const Want& want, int32_t flags, int32_t userId, AbilityInfo& abilityInfo)
{
    (void)want;
    (void)flags;
    (void)userId;
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfos(const Want& want, std::vector<AbilityInfo>& abilityInfos)
{
    (void)want;
    (void)abilityInfos;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfos(
    const Want& want, int32_t flags, int32_t userId, std::vector<AbilityInfo>& abilityInfos)
{
    (void)want;
    (void)flags;
    (void)userId;
    (void)abilityInfos;
    return true;
}

bool BundleMgrProxy::QueryAllAbilityInfos(const Want& want, int32_t userId, std::vector<AbilityInfo>& abilityInfos)
{
    (void)want;
    (void)userId;
    (void)abilityInfos;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfosForClone(const Want& want, std::vector<AbilityInfo>& abilityInfos)
{
    (void)want;
    (void)abilityInfos;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(const std::string& abilityUri, AbilityInfo& abilityInfo)
{
    (void)abilityUri;
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfosByUri(const std::string& abilityUri, std::vector<AbilityInfo>& abilityInfos)
{
    (void)abilityUri;
    (void)abilityInfos;
    return true;
}

bool BundleMgrProxy::QueryAbilityInfoByUri(const std::string& abilityUri, int32_t userId, AbilityInfo& abilityInfo)
{
    (void)abilityUri;
    (void)userId;
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::QueryKeepAliveBundleInfos(std::vector<BundleInfo>& bundleInfos)
{
    (void)bundleInfos;
    return true;
}

std::string BundleMgrProxy::GetAbilityLabel(const std::string& bundleName, const std::string& className)
{
    (void)bundleName;
    (void)className;
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::GetBundleArchiveInfo(const std::string& hapFilePath, const BundleFlag flag, BundleInfo& bundleInfo)
{
    (void)hapFilePath;
    (void)flag;
    (void)bundleInfo;
    return true;
}

bool BundleMgrProxy::GetBundleArchiveInfo(const std::string& hapFilePath, int32_t flags, BundleInfo& bundleInfo)
{
    (void)hapFilePath;
    (void)flags;
    (void)bundleInfo;
    return true;
}

bool BundleMgrProxy::GetHapModuleInfo(const AbilityInfo& abilityInfo, HapModuleInfo& hapModuleInfo)
{
    (void)abilityInfo;
    (void)hapModuleInfo;
    return true;
}

bool BundleMgrProxy::GetLaunchWantForBundle(const std::string& bundleName, Want& want)
{
    (void)bundleName;
    (void)want;
    return true;
}

int BundleMgrProxy::CheckPublicKeys(const std::string& firstBundleName, const std::string& secondBundleName)
{
    (void)firstBundleName;
    (void)secondBundleName;
    return 0;
}

int BundleMgrProxy::CheckPermission(const std::string& bundleName, const std::string& permission)
{
    (void)bundleName;
    (void)permission;
    return 0;
}

int BundleMgrProxy::CheckPermissionByUid(const std::string& bundleName, const std::string& permission, const int userId)
{
    (void)bundleName;
    (void)permission;
    (void)userId;
    return 0;
}

bool BundleMgrProxy::GetPermissionDef(const std::string& permissionName, PermissionDef& permissionDef)
{
    (void)permissionName;
    (void)permissionDef;
    return true;
}

bool BundleMgrProxy::GetAllPermissionGroupDefs(std::vector<PermissionDef>& permissionDefs)
{
    (void)permissionDefs;
    return true;
}

bool BundleMgrProxy::GetAppsGrantedPermissions(
    const std::vector<std::string>& permissions, std::vector<std::string>& appNames)
{
    (void)permissions;
    (void)appNames;
    return true;
}

bool BundleMgrProxy::HasSystemCapability(const std::string& capName)
{
    (void)capName;
    return true;
}

bool BundleMgrProxy::GetSystemAvailableCapabilities(std::vector<std::string>& systemCaps)
{
    (void)systemCaps;
    return true;
}

bool BundleMgrProxy::IsSafeMode()
{
    return true;
}

bool BundleMgrProxy::CleanBundleCacheFiles(
    const std::string& bundleName, const sptr<ICleanCacheCallback>& cleanCacheCallback, int32_t userId)
{
    (void)bundleName;
    (void)cleanCacheCallback;
    (void)userId;
    return true;
}

bool BundleMgrProxy::CleanBundleDataFiles(const std::string& bundleName, const int userId)
{
    (void)bundleName;
    (void)userId;
    return true;
}

bool BundleMgrProxy::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback>& bundleStatusCallback)
{
    (void)bundleStatusCallback;
    return true;
}

bool BundleMgrProxy::ClearBundleStatusCallback(const sptr<IBundleStatusCallback>& bundleStatusCallback)
{
    (void)bundleStatusCallback;
    return true;
}

bool BundleMgrProxy::UnregisterBundleStatusCallback()
{
    return true;
}

bool BundleMgrProxy::DumpInfos(const DumpFlag flag, const std::string& bundleName, int32_t userId, std::string& result)
{
    (void)flag;
    (void)bundleName;
    (void)userId;
    (void)result;
    return true;
}

bool BundleMgrProxy::IsApplicationEnabled(const std::string& bundleName)
{
    (void)bundleName;
    return true;
}

bool BundleMgrProxy::SetApplicationEnabled(const std::string& bundleName, bool isEnable, int32_t userId)
{
    (void)bundleName;
    (void)isEnable;
    (void)userId;
    return true;
}

bool BundleMgrProxy::IsAbilityEnabled(const AbilityInfo& abilityInfo)
{
    (void)abilityInfo;
    return true;
}

bool BundleMgrProxy::SetAbilityEnabled(const AbilityInfo& abilityInfo, bool isEnabled, int32_t userId)
{
    (void)abilityInfo;
    (void)isEnabled;
    (void)userId;
    return true;
}

std::string BundleMgrProxy::GetAbilityIcon(const std::string& bundleName, const std::string& className)
{
    (void)bundleName;
    (void)className;
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
    const std::string& bundleName, const std::string& permissionName, const int userId)
{
    (void)bundleName;
    (void)permissionName;
    (void)userId;
    return true;
}

bool BundleMgrProxy::RequestPermissionFromUser(
    const std::string& bundleName, const std::string& permission, const int userId)
{
    (void)bundleName;
    (void)permission;
    (void)userId;
    return true;
}

bool BundleMgrProxy::RegisterAllPermissionsChanged(const sptr<OnPermissionChangedCallback>& callback)
{
    (void)callback;
    return true;
}

bool BundleMgrProxy::RegisterPermissionsChanged(
    const std::vector<int>& uids, const sptr<OnPermissionChangedCallback>& callback)
{
    (void)callback;
    return true;
}

bool BundleMgrProxy::UnregisterPermissionsChanged(const sptr<OnPermissionChangedCallback>& callback)
{
    (void)callback;
    return true;
}

bool BundleMgrProxy::GetAllFormsInfo(std::vector<FormInfo>& formInfos)
{
    (void)formInfos;
    return true;
}

bool BundleMgrProxy::GetFormsInfoByApp(const std::string& bundleName, std::vector<FormInfo>& formInfos)
{
    (void)bundleName;
    (void)formInfos;
    return true;
}

bool BundleMgrProxy::GetFormsInfoByModule(
    const std::string& bundleName, const std::string& moduleName, std::vector<FormInfo>& formInfos)
{
    (void)bundleName;
    (void)moduleName;
    (void)formInfos;
    return true;
}

bool BundleMgrProxy::GetShortcutInfos(const std::string& bundleName, std::vector<ShortcutInfo>& shortcutInfos)
{
    (void)bundleName;
    (void)shortcutInfos;
    return true;
}

bool BundleMgrProxy::GetAllCommonEventInfo(const std::string& eventKey, std::vector<CommonEventInfo>& commonEventInfos)
{
    (void)eventKey;
    (void)commonEventInfos;
    return true;
}

bool BundleMgrProxy::GetModuleUsageRecords(const int32_t number, std::vector<ModuleUsageRecord>& moduleUsageRecords)
{
    (void)number;
    (void)moduleUsageRecords;
    return true;
}

bool BundleMgrProxy::NotifyAbilityLifeStatus(
    const std::string& bundleName, const std::string& abilityName, const int64_t launchTime, const int uid)
{
    (void)bundleName;
    (void)abilityName;
    (void)launchTime;
    (void)uid;
    return true;
}

bool BundleMgrProxy::CheckBundleNameInAllowList(const std::string& bundleName)
{
    (void)bundleName;
    return true;
}

bool BundleMgrProxy::BundleClone(const std::string& bundleName)
{
    (void)bundleName;
    return true;
}

bool BundleMgrProxy::RemoveClonedBundle(const std::string& bundleName, const int32_t uid)
{
    (void)bundleName;
    (void)uid;
    return true;
}

bool BundleMgrProxy::GetDistributedBundleInfo(const std::string& networkId, int32_t userId,
    const std::string& bundleName, DistributedBundleInfo& distributedBundleInfo)
{
    (void)networkId;
    (void)userId;
    (void)bundleName;
    (void)distributedBundleInfo;
    return true;
}

std::string BundleMgrProxy::GetAppPrivilegeLevel(const std::string& bundleName, int32_t userId)
{
    (void)bundleName;
    (void)userId;
    return Constants::EMPTY_STRING;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(
    const Want& want, const int32_t& flag, const int32_t& userId, std::vector<ExtensionAbilityInfo>& extensionInfos)
{
    (void)want;
    (void)flag;
    (void)userId;
    (void)extensionInfos;
    return true;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(const Want& want, const ExtensionAbilityType& extensionType,
    const int32_t& flag, const int32_t& userId, std::vector<ExtensionAbilityInfo>& extensionInfos)
{
    (void)want;
    (void)extensionType;
    (void)flag;
    (void)userId;
    (void)extensionInfos;
    return true;
}

bool BundleMgrProxy::QueryExtensionAbilityInfos(
    const ExtensionAbilityType& extensionType, const int32_t& userId, std::vector<ExtensionAbilityInfo>& extensionInfos)
{
    (void)extensionType;
    (void)userId;
    (void)extensionInfos;
    return true;
}

std::shared_ptr<Media::PixelMap> BundleMgrProxy::GetAbilityPixelMapIcon(
    const std::string& bundleName, const std::string& abilityName)
{
    (void)bundleName;
    (void)abilityName;
    return nullptr;
}

template <typename T>
bool BundleMgrProxy::GetParcelableInfo(IBundleMgr::Message code, MessageParcel& data, T& parcelableInfo)
{
    (void)code;
    (void)data;
    (void)parcelableInfo;
    return true;
}

template <typename T>
bool BundleMgrProxy::GetParcelableInfos(IBundleMgr::Message code, MessageParcel& data, std::vector<T>& parcelableInfos)
{
    (void)code;
    (void)data;
    (void)parcelableInfos;
    return true;
}

bool BundleMgrProxy::SendTransactCmd(IBundleMgr::Message code, MessageParcel& data, MessageParcel& reply)
{
    (void)code;
    (void)data;
    (void)reply;
    return true;
}

bool BundleMgrProxy::VerifyCallingPermission(const std::string& permission)
{
    (void)permission;
    return true;
}

std::vector<std::string> BundleMgrProxy::GetAccessibleAppCodePaths(int32_t userId)
{
    (void)userId;
    std::vector<std::string> vec;
    return vec;
}

bool BundleMgrProxy::QueryExtensionAbilityInfoByUri(
    const std::string& uri, int32_t userId, ExtensionAbilityInfo& extensionAbilityInfo)
{
    (void)uri;
    (void)userId;
    (void)extensionAbilityInfo;
    return true;
}
} // namespace AppExecFwk
} // namespace OHOS