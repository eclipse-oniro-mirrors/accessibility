/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <vector>
#include <gmock/gmock.h>
#include "ability_info.h"
#include "application_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "form_info.h"
#include "shortcut_info.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string COM_IX_HIWORLD = "com.ix.hiworld";
const std::string COM_IX_HIMUSIC = "com.ix.hiMusic";
const std::string COM_IX_HIRADIO = "com.ix.hiRadio";
const std::string COM_IX_HISERVICE = "com.ix.hiService";
const std::string COM_IX_MUSICSERVICE = "com.ix.musicService";
const std::string COM_IX_HIDATA = "com.ix.hiData";
const std::string COM_IX_HIEXTENSION = "com.ix.hiExtension";
const std::string COM_IX_HIACCOUNT = "com.ix.hiAccount";
const std::string COM_IX_HIBACKGROUNDMUSIC = "com.ix.hiBackgroundMusic";
const std::string COM_IX_HIBACKGROUNDDATA = "com.ix.hiBackgroundData";
const std::string COM_IX_HISINGLEMUSIC = "com.ix.hiSingleMusicInfo";
const std::string COM_IX_ACCOUNTSERVICE = "com.ix.accountService";
const std::string COM_OHOS_TEST = "com.ohos.test";
constexpr int32_t MAX_SYS_UID = 2899;
constexpr int32_t ROOT_UID = 0;
const int32_t BASE_USER_RANGE = 200000;
const int32_t APPLICATIONINFO_UID = 20000000;
} // namespace

class BundleMgrProxy : public IRemoteProxy<IBundleMgr> {
public:
    explicit BundleMgrProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IBundleMgr>(impl)
    {}
    virtual ~BundleMgrProxy()
    {}
    int QueryWantAbility(const AAFwk::Want& want, std::vector<AbilityInfo>& abilityInfos);

    bool QueryAbilityInfo(const AAFwk::Want& want, AbilityInfo& abilityInfo) override;

    bool QueryAbilityInfoByUri(const std::string& uri, AbilityInfo& abilityInfo) override;

    bool GetApplicationInfo(
        const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo) override;

    bool GetBundleInfo(
        const std::string& bundleName, const BundleFlag flag, BundleInfo& bundleInfo, int32_t userId) override;

    virtual bool CheckIsSystemAppByUid(const int uid) override;
};

class BundleMgrStub : public IRemoteStub<IBundleMgr> {
public:
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
};

class BundleMgrService : public BundleMgrStub {
public:
    BundleMgrService();
    ~BundleMgrService();

    bool QueryAbilityInfo(const AAFwk::Want& want, AbilityInfo& abilityInfo) override;
    bool QueryAbilityInfo(const Want& want, int32_t flags, int32_t userId, AbilityInfo& abilityInfo) override;
    bool QueryAbilityInfoByUri(const std::string& uri, AbilityInfo& abilityInfo) override;
    bool GetApplicationInfo(
        const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo) override;
    bool GetBundleInfo(
        const std::string& bundleName, const BundleFlag flag, BundleInfo& bundleInfo, int32_t userId) override;
    int GetUidByBundleName(const std::string& bundleName, const int userId) override;
    virtual bool CheckIsSystemAppByUid(const int uid) override;
    bool CheckWantEntity(const AAFwk::Want&, AbilityInfo&);

public:
    using QueryAbilityInfoFunType =
        std::function<bool(std::string bundleName, AbilityInfo& abilityInfo, ElementName& elementTemp)>;
    std::map<std::string, QueryAbilityInfoFunType> abilityInfoMap_;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // MOCK_BUNDLE_MANAGER_H
