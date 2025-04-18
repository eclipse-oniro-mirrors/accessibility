/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_POWER_MANAGER_H
#define ACCESSIBILITY_POWER_MANAGER_H

#include <set>
#include "singleton.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "power_mgr_client.h"
#endif
#include "ffrt.h"
#include "accessibility_account_data.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityPowerManager {
    DECLARE_SINGLETON(AccessibilityPowerManager)
public:
    bool DiscountBrightness(const float discount);
    bool RefreshActivity();
    bool InitRunningLock();
    bool HoldRunningLock(const std::string &bundleName);
    bool UnholdRunningLock(const std::string &bundleName = "");
    std::set<std::string> GetWakeLockAbilities();
private:
    std::shared_ptr<PowerMgr::RunningLock> wakeLock_ { nullptr };
    ffrt::mutex powerWakeLockMutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_POWER_MANAGER_H