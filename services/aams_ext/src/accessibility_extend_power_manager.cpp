/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "accessibility_extend_power_manager.h"
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "display_power_mgr_client.h"
#endif
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityExtendPowerManager::AccessibilityExtendPowerManager()
{
}

AccessibilityExtendPowerManager::~AccessibilityExtendPowerManager()
{
}

bool AccessibilityExtendPowerManager::DiscountBrightness(const float discount)
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    auto& displayPowerMgrClient = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!displayPowerMgrClient.DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
        return false;
    }
#else
    HILOG_DEBUG("not support display manager");
#endif
    return true;
}

bool AccessibilityExtendPowerManager::RefreshActivity()
{
    HILOG_DEBUG();
    return PowerMgr::PowerMgrClient::GetInstance().RefreshActivity(
        OHOS::PowerMgr::UserActivityType::USER_ACTIVITY_TYPE_TOUCH);
}
} // namespace Accessibility
} // namespace OHOS