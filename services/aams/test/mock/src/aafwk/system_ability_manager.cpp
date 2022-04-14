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

#include "system_ability_manager.h"
#include "ability_death_recipient.h"
#include "datetime_ex.h"
#include "errors.h"
#include "hilog_wrapper.h"
#include "sam_log.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
std::mutex SystemAbilityManager::instanceLock;
sptr<SystemAbilityManager> SystemAbilityManager::instance;

SystemAbilityManager::SystemAbilityManager()
{}

SystemAbilityManager::~SystemAbilityManager()
{}

void SystemAbilityManager::Init()
{}

const sptr<DBinderService> SystemAbilityManager::GetDBinder() const
{
    return nullptr;
}

sptr<SystemAbilityManager> SystemAbilityManager::GetInstance()
{
    HILOG_ERROR("SystemAbilityManager::GetInstance");
    std::lock_guard<std::mutex> autoLock(instanceLock);
    if (!instance) {
        instance = new SystemAbilityManager;
    }
    return instance;
}

sptr<IRemoteObject> SystemAbilityManager::GetSystemAbility(int32_t systemAbilityId)
{
    HILOG_ERROR("SystemAbilityManager::GetSystemAbility");
    auto iter = abilityMap_.find(systemAbilityId);
    if (iter != abilityMap_.end()) {
        return iter->second.remoteObj;
    }
    HILOG_ERROR("SystemAbilityManager::GetSystemAbility nullptr");
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManager::GetSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    return nullptr;
}

sptr<IRemoteObject> SystemAbilityManager::CheckSystemAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return nullptr;
}

bool SystemAbilityManager::CheckDistributedPermission()
{
    return true;
}

sptr<IRemoteObject> SystemAbilityManager::CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    return nullptr;
}

int32_t SystemAbilityManager::FindSystemAbilityNotify(int32_t systemAbilityId, int32_t code)
{
    (void)systemAbilityId;
    (void)code;
    return 0;
}

int32_t SystemAbilityManager::FindSystemAbilityNotify(
    int32_t systemAbilityId, const std::string& deviceId, int32_t code)
{
    (void)systemAbilityId;
    (void)deviceId;
    (void)code;
    return ERR_OK;
}

bool SystemAbilityManager::IsNameInValid(const std::u16string& name)
{
    (void)name;
    return false;
}

int32_t SystemAbilityManager::AddOnDemandSystemAbilityInfo(
    int32_t systemAbilityId, const std::u16string& localAbilityManagerName)
{
    (void)systemAbilityId;
    (void)localAbilityManagerName;
    return ERR_OK;
}

int32_t SystemAbilityManager::StartOnDemandAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return ERR_INVALID_VALUE;
}

sptr<IRemoteObject> SystemAbilityManager::CheckSystemAbility(int32_t systemAbilityId, bool& isExist)
{
    (void)systemAbilityId;
    (void)isExist;
    return nullptr;
}

int32_t SystemAbilityManager::LoadSystemAbility(
    int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>& callback)
{
    (void)systemAbilityId;
    (void)callback;
    return ERR_OK;
}

void SystemAbilityManager::DoInsertSaData(
    const std::u16string& strName, const sptr<IRemoteObject>& ability, const SAExtraProp& extraProp)
{
    (void)strName;
    (void)ability;
    (void)extraProp;
}

int32_t SystemAbilityManager::RemoveSystemAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    abilityMap_.clear();
    return ERR_OK;
}

int32_t SystemAbilityManager::RemoveSystemAbility(const sptr<IRemoteObject>& ability)
{
    (void)ability;
    return ERR_OK;
}

std::vector<std::u16string> SystemAbilityManager::ListSystemAbilities(unsigned int dumpFlags)
{
    (void)dumpFlags;
    std::vector<std::u16string> list;
    return list;
}

int32_t SystemAbilityManager::SubscribeSystemAbility(
    int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener)
{
    (void)systemAbilityId;
    (void)listener;
    return ERR_OK;
}

int32_t SystemAbilityManager::UnSubscribeSystemAbility(
    int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener)
{
    (void)systemAbilityId;
    (void)listener;
    return ERR_OK;
}

void SystemAbilityManager::SetDeviceName(const std::u16string& name)
{
    (void)name;
}

const std::u16string& SystemAbilityManager::GetDeviceName() const
{
    return deviceName_;
}

void SystemAbilityManager::NotifyRemoteSaDied(const std::u16string& name)
{
    (void)name;
}

void SystemAbilityManager::NotifyRemoteDeviceOffline(const std::string& deviceId)
{
    (void)deviceId;
}

void SystemAbilityManager::ParseRemoteSaName(const std::u16string& name, std::string& deviceId, std::u16string& saName)
{
    (void)name;
    (void)deviceId;
    (void)saName;
}

int32_t SystemAbilityManager::AddSystemAbility(
    int32_t systemAbilityId, const sptr<IRemoteObject>& ability, const SAExtraProp& extraProp)
{
    SAInfo saInfo;
    saInfo.remoteObj = ability;
    saInfo.isDistributed = extraProp.isDistributed;
    saInfo.capability = extraProp.capability;
    saInfo.permission = Str16ToStr8(extraProp.permission);
    abilityMap_[systemAbilityId] = std::move(saInfo);
    return 0;
}

bool SystemAbilityManager::IsLocalDeviceId(const std::string& deviceId)
{
    (void)deviceId;
    return false;
}

int32_t SystemAbilityManager::AddSystemProcess(const std::u16string& procName, const sptr<IRemoteObject>& procObject)
{
    (void)procName;
    (void)procObject;
    return 0;
}
} // namespace OHOS