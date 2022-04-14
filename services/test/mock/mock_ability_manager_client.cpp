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

#include "ability_manager_client.h"

namespace OHOS {
namespace AAFwk {
std::shared_ptr<AbilityManagerClient> AbilityManagerClient::instance_ = nullptr;
std::recursive_mutex AbilityManagerClient::mutex_;

AbilityManagerClient::AbilityManagerClient()
{}
AbilityManagerClient::~AbilityManagerClient()
{}

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::recursive_mutex> lock_l(mutex_);
        if (!instance_) {
            instance_ = std::make_shared<AbilityManagerClient>();
        }
    }
    return instance_;
}

ErrCode AbilityManagerClient::ConnectAbility(
    const Want& want, const sptr<IAbilityConnection>& connect, const sptr<IRemoteObject>& callerToken, int32_t userId)
{
    return NO_ERROR;
}

ErrCode AbilityManagerClient::DisconnectAbility(const sptr<IAbilityConnection>& connect)
{
    return NO_ERROR;
}

ErrCode AbilityManagerClient::SetAbilityController(
    const sptr<AppExecFwk::IAbilityController>& abilityController, bool imAStabilityTest)
{
    return NO_ERROR;
}
} // namespace AAFwk
} // namespace OHOS