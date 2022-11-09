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

#include "mock_power_mgr_client.h"
#include "power_errors.h"

namespace OHOS {
namespace PowerMgr {
PowerMgrClient::PowerMgrClient()
{}
PowerMgrClient::~PowerMgrClient()
{}

ErrCode PowerMgrClient::Connect()
{
    return ERR_OK;
}

void PowerMgrClient::ResetProxy(const wptr<IRemoteObject>& remote)
{}

void PowerMgrClient::PowerMgrDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{}

PowerErrors PowerMgrClient::RebootDevice(const std::string& reason)
{
    return PowerErrors::ERR_OK;
}

PowerErrors PowerMgrClient::ShutDownDevice(const std::string& reason)
{
    return PowerErrors::ERR_OK;
}

PowerErrors PowerMgrClient::SuspendDevice(SuspendDeviceType reason, bool suspendImmed)
{
    return PowerErrors::ERR_OK;
}

PowerErrors PowerMgrClient::WakeupDevice(WakeupDeviceType reason, const std::string& detail)
{
    return PowerErrors::ERR_OK;
}

bool PowerMgrClient::RefreshActivity(UserActivityType type)
{
    return true;
}

bool PowerMgrClient::IsRunningLockTypeSupported(uint32_t type)
{
    return true;
}

bool PowerMgrClient::ForceSuspendDevice()
{
    return true;
}

bool PowerMgrClient::IsScreenOn()
{
    return true;
}

PowerState PowerMgrClient::GetState()
{
    return PowerState::UNKNOWN;
}

std::shared_ptr<RunningLock> PowerMgrClient::CreateRunningLock(const std::string& name, RunningLockType type)
{
    return nullptr;
}

bool PowerMgrClient::RegisterPowerStateCallback(const sptr<IPowerStateCallback>& callback)
{
    return true;
}

bool PowerMgrClient::UnRegisterPowerStateCallback(const sptr<IPowerStateCallback>& callback)
{
    return true;
}

bool PowerMgrClient::RegisterShutdownCallback(
    const sptr<IShutdownCallback>& callback, IShutdownCallback::ShutdownPriority priority)
{
    return true;
}

bool PowerMgrClient::UnRegisterShutdownCallback(const sptr<IShutdownCallback>& callback)
{
    return true;
}

bool PowerMgrClient::RegisterPowerModeCallback(const sptr<IPowerModeCallback>& callback)
{
    return true;
}

bool PowerMgrClient::UnRegisterPowerModeCallback(const sptr<IPowerModeCallback>& callback)
{
    return true;
}

bool PowerMgrClient::SetDisplaySuspend(bool enable)
{
    return true;
}

PowerErrors PowerMgrClient::SetDeviceMode(const PowerMode mode)
{
    return PowerErrors::ERR_OK;
}

PowerMode PowerMgrClient::GetDeviceMode()
{
    return PowerMode::NORMAL_MODE;
}

std::string PowerMgrClient::Dump(const std::vector<std::string>& args)
{
    return "ERR_OK";
}
} // namespace PowerMgr
} // namespace OHOS