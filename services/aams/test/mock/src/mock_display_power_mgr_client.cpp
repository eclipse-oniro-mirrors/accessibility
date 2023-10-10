/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "display_power_mgr_client.h"

namespace OHOS {
namespace DisplayPowerMgr {
DisplayPowerMgrClient::DisplayPowerMgrClient() = default;
DisplayPowerMgrClient::~DisplayPowerMgrClient() = default;

sptr<IDisplayPowerMgr> DisplayPowerMgrClient::GetProxy()
{
    return nullptr;
}

void DisplayPowerMgrClient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    (void)remote;
}

bool DisplayPowerMgrClient::SetDisplayState(DisplayState state,
    PowerMgr::StateChangeReason reason, uint32_t id)
{
    (void)state;
    (void)reason;
    (void)id;
    return true;
}

DisplayState DisplayPowerMgrClient::GetDisplayState(uint32_t id)
{
    (void)id;
    return DisplayState::DISPLAY_UNKNOWN;
}

std::vector<uint32_t> DisplayPowerMgrClient::GetDisplayIds()
{
    return {};
}

int32_t DisplayPowerMgrClient::GetMainDisplayId()
{
    return INVALID_DISPLAY_ID;
}

bool DisplayPowerMgrClient::SetBrightness(uint32_t value, uint32_t displayId, bool continuous)
{
    (void)value;
    (void)displayId;
    (void)continuous;
    return true;
}

bool DisplayPowerMgrClient::DiscountBrightness(double discount, uint32_t displayId)
{
    (void)discount;
    (void)displayId;
    return true;
}

bool DisplayPowerMgrClient::OverrideBrightness(uint32_t value, uint32_t displayId)
{
    (void)value;
    (void)displayId;
    return true;
}

bool DisplayPowerMgrClient::RestoreBrightness(uint32_t displayId)
{
    (void)displayId;
    return true;
}

uint32_t DisplayPowerMgrClient::GetBrightness(uint32_t displayId)
{
    (void)displayId;
    return BRIGHTNESS_OFF;
}

uint32_t DisplayPowerMgrClient::GetDefaultBrightness()
{
    return BRIGHTNESS_DEFAULT;
}

uint32_t DisplayPowerMgrClient::GetMaxBrightness()
{
    return BRIGHTNESS_MAX;
}

uint32_t DisplayPowerMgrClient::GetMinBrightness()
{
    return BRIGHTNESS_MIN;
}

bool DisplayPowerMgrClient::AdjustBrightness(uint32_t value, uint32_t duration, uint32_t id)
{
    (void)value;
    (void)duration;
    (void)id;
    return true;
}

bool DisplayPowerMgrClient::AutoAdjustBrightness(bool enable)
{
    (void)enable;
    return true;
}

bool DisplayPowerMgrClient::IsAutoAdjustBrightness()
{
    return true;
}

bool DisplayPowerMgrClient::RegisterCallback(sptr<IDisplayPowerCallback> callback)
{
    (void)callback;
    return true;
}

bool DisplayPowerMgrClient::BoostBrightness(int32_t timeoutMs, uint32_t displayId)
{
    (void)timeoutMs;
    (void)displayId;
    return true;
}

bool DisplayPowerMgrClient::CancelBoostBrightness(uint32_t displayId)
{
    (void)displayId;
    return true;
}

uint32_t DisplayPowerMgrClient::GetDeviceBrightness(uint32_t displayId)
{
    (void)displayId;
    return 0;
}

DisplayErrors DisplayPowerMgrClient::GetError()
{
    return DisplayErrors::ERR_OK;
}
}  // namespace DisplayPowerMgr
}  // namespace OHOS
