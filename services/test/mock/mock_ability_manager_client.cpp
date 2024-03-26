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

#define private public
#define protected public
#include "ability_manager_client.h"
#undef private
#undef protected

namespace OHOS {
namespace AAFwk {
std::shared_ptr<AbilityManagerClient> AbilityManagerClient::instance_ = nullptr;
std::once_flag AbilityManagerClient::singletonFlag_;

AbilityManagerClient::AbilityManagerClient()
{}
AbilityManagerClient::~AbilityManagerClient()
{}

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    std::call_once(singletonFlag_, [] () {
        instance_ = std::make_shared<AbilityManagerClient>();
    });
    return instance_;
}

ErrCode AbilityManagerClient::ConnectAbility(
    const Want& want, sptr<IAbilityConnection> connect, sptr<IRemoteObject> callerToken, int32_t userId)
{
    return NO_ERROR;
}

ErrCode AbilityManagerClient::DisconnectAbility(sptr<IAbilityConnection> connect)
{
    return NO_ERROR;
}

ErrCode AbilityManagerClient::SetAbilityController(
    sptr<AppExecFwk::IAbilityController> abilityController, bool imAStabilityTest)
{
    return NO_ERROR;
}
} // namespace AAFwk
} // namespace OHOS