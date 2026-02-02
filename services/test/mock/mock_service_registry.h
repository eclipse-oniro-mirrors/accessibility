/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
    SystemAbilityManagerProxy() {}
    ~SystemAbilityManagerProxy() {}

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId);
    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    sptr<IRemoteObject> CheckSystemAbilityWrapper(int32_t code, MessageParcel& data);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, bool& isExist);
    sptr<IRemoteObject> LoadSystemAbility(int32_t systemAbilityId, int32_t timeout);
    int32_t LoadSystemAbility(int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>& callback);
    int32_t LoadSystemAbility(int32_t systemAbilityId, const std::string& deviceId,
        const sptr<ISystemAbilityLoadCallback>& callback);
};
} // namespace OHOS
#endif // OHOS_ACCESSIBILITY_MOCK_SYSTEM_ABILITY_MANAGER_H