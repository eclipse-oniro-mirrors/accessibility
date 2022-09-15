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
    int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
};

class BundleMgrService : public BundleMgrStub {
public:
    bool QueryAbilityInfo(const AAFwk::Want& want, AbilityInfo& abilityInfo) override;
    bool GetApplicationInfo(
        const std::string& appName, const ApplicationFlag flag, const int userId, ApplicationInfo& appInfo) override;

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
