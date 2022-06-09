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

#ifndef MOCK_BUNDLE_MANAGER_H
#define MOCK_BUNDLE_MANAGER_H

#include <gmock/gmock.h>
#include <vector>
#include "ability_info.h"
#include "application_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
const std::string COM_IX_HIWORLD = "com.ix.hiworld";
const std::string COM_IX_HIMUSIC = "com.ix.hiMusic";
const std::string COM_IX_HIRADIO = "com.ix.hiRadio";
const std::string COM_IX_HISERVICE = "com.ix.hiService";
const std::string COM_IX_MUSICSERVICE = "com.ix.musicService";
const std::string COM_IX_HIDATA = "com.ix.hiData";

class BundleMgrStub : public IRemoteStub<IBundleMgr> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IBundleMgr");
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
};

class BundleMgrService : public BundleMgrStub {
public:
    bool QueryAbilityInfo(const AAFwk::Want& want, AbilityInfo& abilityInfo) override;
    bool QueryAbilityInfoByUri(const std::string& uri, AbilityInfo& abilityInfo) override;
    bool GetApplicationInfo(
        const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo) override;

    bool QueryAbilityInfos(const Want& want, std::vector<AbilityInfo>& abilityInfos) override;
    bool GetAllFormsInfo(std::vector<FormInfo>& formInfos) override;
    bool GetFormsInfoByApp(const std::string& bundleName, std::vector<FormInfo>& formInfos) override;
    bool GetFormsInfoByModule(
        const std::string& bundleName, const std::string& moduleName, std::vector<FormInfo>& formInfos) override;
    bool GetShortcutInfos(const std::string& bundleName, std::vector<ShortcutInfo>& shortcutInfos) override;

    MOCK_METHOD2(QueryWantAbility, int(const AAFwk::Want& want, std::vector<AbilityInfo>& abilityInfos));
    MOCK_METHOD3(GetApplicationInfos,
        bool(const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo>& appInfos));
    MOCK_METHOD3(GetBundleInfos, bool(const BundleFlag flag, std::vector<BundleInfo>& bundleInfos, int32_t userId));
    MOCK_METHOD3(GetBundleInfos, bool(int32_t flags, std::vector<BundleInfo>& bundleInfos, int32_t userId));
    MOCK_METHOD2(GetUidByBundleName, int(const std::string& bundleName, const int userId));
    MOCK_METHOD2(GetBundleNameForUid, bool(const int uid, std::string& bundleName));
    MOCK_METHOD2(GetBundleGids, bool(const std::string& bundleName, std::vector<int>& gids));
    MOCK_METHOD3(GetBundleGidsByUid, bool(const std::string& bundleName, const int& uid, std::vector<int>& gids));
    MOCK_METHOD2(QueryAbilityInfosByUri, bool(const std::string& abilityUri, std::vector<AbilityInfo>& abilityInfos));
    MOCK_METHOD3(CheckPermissionByUid,
        int(const std::string& bundleName, const std::string& permission, const int userId));
    MOCK_METHOD2(GetAllCommonEventInfo,
        bool(const std::string& eventKey, std::vector<CommonEventInfo>& commonEventInfos));
    MOCK_METHOD0(GetBundleUserMgr, sptr<IBundleUserMgr>());
    MOCK_METHOD3(GetDistributedBundleInfo, bool(const std::string& networkId, const std::string& bundleName,
        DistributedBundleInfo& distributedBundleInfo));
    MOCK_METHOD1(GetAppType, std::string(const std::string& bundleName));
    MOCK_METHOD1(CheckIsSystemAppByUid, bool(const int uid));
    MOCK_METHOD2(GetBundleInfosByMetaData, bool(const std::string& metaData, std::vector<BundleInfo>& bundleInfos));
    MOCK_METHOD1(QueryKeepAliveBundleInfos, bool(std::vector<BundleInfo>& bundleInfos));
    MOCK_METHOD2(GetAbilityLabel, std::string(const std::string& bundleName, const std::string& className));
    MOCK_METHOD3(GetBundleArchiveInfo,
        bool(const std::string& hapFilePath, const BundleFlag flag, BundleInfo& bundleInfo));
    MOCK_METHOD2(GetHapModuleInfo, bool(const AbilityInfo& abilityInfo, HapModuleInfo& hapModuleInfo));
    MOCK_METHOD2(GetLaunchWantForBundle, bool(const std::string& bundleName, Want& want));
    MOCK_METHOD2(CheckPublicKeys, int(const std::string& firstBundleName, const std::string& secondBundleName));
    MOCK_METHOD2(CheckPermission, int(const std::string& bundleName, const std::string& permission));
    MOCK_METHOD2(GetPermissionDef, bool(const std::string& permissionName, PermissionDef& permissionDef));
    MOCK_METHOD1(GetAllPermissionGroupDefs, bool(std::vector<PermissionDef>& permissionDefs));
    MOCK_METHOD2(GetAppsGrantedPermissions,
        bool(const std::vector<std::string>& permissions, std::vector<std::string>& appNames));
    MOCK_METHOD1(HasSystemCapability, bool(const std::string& capName));
    MOCK_METHOD1(GetSystemAvailableCapabilities, bool(std::vector<std::string>& systemCaps));
    MOCK_METHOD0(IsSafeMode, bool());
    MOCK_METHOD3(CleanBundleCacheFiles,
        bool(const std::string& bundleName, const sptr<ICleanCacheCallback>& cleanCacheCallback, int32_t userId));
    MOCK_METHOD2(CleanBundleDataFiles, bool(const std::string& bundleName, const int userId));
    MOCK_METHOD1(RegisterBundleStatusCallback, bool(const sptr<IBundleStatusCallback>& bundleStatusCallback));
    MOCK_METHOD1(ClearBundleStatusCallback, bool(const sptr<IBundleStatusCallback>& bundleStatusCallback));
    MOCK_METHOD0(UnregisterBundleStatusCallback, bool());
    MOCK_METHOD4(
        DumpInfos, bool(const DumpFlag flag, const std::string& bundleName, int32_t userId, std::string& result));
    MOCK_METHOD1(IsApplicationEnabled, bool(const std::string& bundleName));
    MOCK_METHOD3(SetApplicationEnabled, bool(const std::string& bundleName, bool isEnable, int32_t userId));
    MOCK_METHOD0(GetBundleInstaller, sptr<IBundleInstaller>());
    MOCK_METHOD2(GetAppIdByBundleName, std::string(const std::string& bundleName, const int userId));
    MOCK_METHOD2(GetBundlesForUid, bool(const int uid, std::vector<std::string>& bundleNames));
    MOCK_METHOD2(GetNameForUid, bool(const int uid, std::string& name));
    MOCK_METHOD1(IsAbilityEnabled, bool(const AbilityInfo& abilityInfo));
    MOCK_METHOD3(SetAbilityEnabled, bool(const AbilityInfo& abilityInfo, bool isEnabled, int32_t userId));
    MOCK_METHOD2(GetAbilityIcon, std::string(const std::string& bundleName, const std::string& className));
    MOCK_METHOD3(
        CanRequestPermission, bool(const std::string& bundleName, const std::string& permissionName, const int userId));
    MOCK_METHOD3(RequestPermissionFromUser,
        bool(const std::string& bundleName, const std::string& permission, const int userId));

    bool GetBundleInfo(const std::string& bundleName, const BundleFlag flag, BundleInfo& bundleInfo,
        int32_t userId = Constants::UNSPECIFIED_USERID) override;

    BundleMgrService();

    virtual ~BundleMgrService()
    {}

    bool CheckWantEntity(const AAFwk::Want&, AbilityInfo&);

public:
    using QueryAbilityInfoFunType =
        std::function<bool(std::string bundleName, AbilityInfo& abilityInfo, ElementName& elementTemp)>;
    std::map<std::string, QueryAbilityInfoFunType> abilityInfoMap_;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // MOCK_BUNDLE_MANAGER_H
