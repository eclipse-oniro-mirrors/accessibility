/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_ACCESSIBILITY_MOCK_SYSTEM_ABILITY_MANAGER_H
#define OHOS_ACCESSIBILITY_MOCK_SYSTEM_ABILITY_MANAGER_H
 
#include "gmock/gmock.h"
#include "if_system_ability_manager.h"
 
namespace OHOS {
class SystemAbilityManagerProxy : public ISystemAbilityManager {
public:
    SystemAbilityManagerProxy() = default;
    ~SystemAbilityManagerProxy() = default;
    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId, const std::string &deviceId);
    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, const std::string &deviceId);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, bool &isExist);
    int32_t LoadSystemAbility(int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback> &callback);
    sptr<IRemoteObject> AsObject() override
    {
        return {};
    }
 
    MOCK_METHOD(std::vector<std::u16string>, ListSystemAbilities, (unsigned int dumpFlags));
    MOCK_METHOD(int32_t, RemoveSystemAbility, (int32_t systemAbilityId));
    MOCK_METHOD(
        int32_t, SubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange> &listener));
    MOCK_METHOD(
        int32_t, UnSubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange> &listener));
    MOCK_METHOD(int32_t, AddOnDemandSystemAbilityInfo,
        (int32_t systemAbilityId, const std::u16string &localAbilityManagerName));
    MOCK_METHOD(sptr<IRemoteObject>, LoadSystemAbility, (int32_t systemAbilityId, int32_t timeout));
    MOCK_METHOD(int32_t, LoadSystemAbility,
        (int32_t systemAbilityId, const std::string &deviceId, const sptr<ISystemAbilityLoadCallback> &callback));
    MOCK_METHOD(int32_t, UnloadSystemAbility, (int32_t systemAbilityId));
    MOCK_METHOD(int32_t, CancelUnloadSystemAbility, (int32_t systemAbilityId));
    MOCK_METHOD(int32_t, UnloadAllIdleSystemAbility, ());
    MOCK_METHOD(int32_t, SendStrategy,
        (int32_t type, std::vector<int32_t> &systemAbilityIds, int32_t level, std::string &action));
    MOCK_METHOD(int32_t, GetExtensionSaIds, (const std::string &extension, std::vector<int32_t> &saIds));
    MOCK_METHOD(
        int32_t, GetExtensionRunningSaList, (const std::string &extension, std::vector<sptr<IRemoteObject>> &saList));
    MOCK_METHOD(int32_t, GetCommonEventExtraDataIdlist,
        (int32_t saId, std::vector<int64_t> &extraDataIdList, const std::string &eventName));
    MOCK_METHOD(int32_t, GetOnDemandReasonExtraData, (int64_t extraDataId, MessageParcel &extraDataParcel));
    MOCK_METHOD(int32_t, GetOnDemandSystemAbilityIds, (std::vector<int32_t> & systemAbilityIds));
    MOCK_METHOD(int32_t, AddSystemProcess, (const std::u16string &procName, const sptr<IRemoteObject> &procObject));
    MOCK_METHOD(int32_t, AddSystemAbility, (int32_t systemAbilityId, const sptr<IRemoteObject>& ability,
        const ISystemAbilityManager::SAExtraProp& extraProp));
    MOCK_METHOD(int32_t, GetSystemProcessInfo, (int32_t systemAbilityId, SystemProcessInfo &systemProcessInfo));
    MOCK_METHOD(int32_t, GetRunningSystemProcess, (std::list<SystemProcessInfo> & systemProcessInfos));
    MOCK_METHOD(int32_t, SubscribeSystemProcess, (const sptr<ISystemProcessStatusChange> &listener));
    MOCK_METHOD(int32_t, UnSubscribeSystemProcess, (const sptr<ISystemProcessStatusChange> &listener));
    MOCK_METHOD(
        int32_t, GetRunningSaExtensionInfoList, (const std::string &extension, std::vector<SaExtensionInfo> &infoList));
    MOCK_METHOD(int32_t, GetOnDemandPolicy,
        (int32_t systemAbilityId, OnDemandPolicyType type,
            std::vector<SystemAbilityOnDemandEvent> &abilityOnDemandEvents));
    MOCK_METHOD(int32_t, UpdateOnDemandPolicy,
        (int32_t systemAbilityId, OnDemandPolicyType type,
            const std::vector<SystemAbilityOnDemandEvent> &abilityOnDemandEvents));
};
 
class SystemAbilityManagerClient {
public:
    static SystemAbilityManagerClient& GetInstance();
    sptr<ISystemAbilityManager> GetSystemAbilityManager();
 
private:
    sptr<ISystemAbilityManager> systemAbilityManager_;
    SystemAbilityManagerClient() = default;
    ~SystemAbilityManagerClient();
};
} // namespace OHOS
#endif // OHOS_ACCESSIBILITY_MOCK_SYSTEM_ABILITY_MANAGER_H