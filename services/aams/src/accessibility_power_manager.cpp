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

#include "accessibility_power_manager.h"
#include "running_lock.h"
#include "display_power_mgr_client.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string HOLD_LOCK_NAME = "AccessibilityWakeLock";
}
AccessibilityPowerManager::AccessibilityPowerManager()
{
}

AccessibilityPowerManager::~AccessibilityPowerManager()
{
}

bool AccessibilityPowerManager::DiscountBrightness(const float discount)
{
    auto& displayPowerMgrClient = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!displayPowerMgrClient.DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
        return false;
    }

    return true;
}

bool AccessibilityPowerManager::RefreshActivity()
{
    HILOG_DEBUG();
    return PowerMgr::PowerMgrClient::GetInstance().RefreshActivity(
        OHOS::PowerMgr::UserActivityType::USER_ACTIVITY_TYPE_TOUCH);
}

bool AccessibilityPowerManager::InitRunningLock()
{
    wakeLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(
        HOLD_LOCK_NAME, PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
    if (wakeLock_ == nullptr) {
        HILOG_INFO("CreateRunningLock InitRunningLock failed.");
        return false;
    }
    HILOG_INFO("CreateRunningLock InitRunningLock.");
    return true;
}

std::set<std::string> AccessibilityPowerManager::GetWakeLockAbilities()
{
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    return wakeLockAbilities_;
}

bool AccessibilityPowerManager::HoldRunningLock(std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    if (wakeLock_ == nullptr) {
        if (!InitRunningLock()) {
            return false;
        }
    }
    if (!wakeLockAbilities_.count(bundleName)) {
        wakeLockAbilities_.insert(bundleName);
    }
    wakeLock_->Lock();
    HILOG_DEBUG("wakeLock_ Lock success.");
    return true;
}

bool AccessibilityPowerManager::UnholdRunningLock(std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    if (wakeLock_ == nullptr) {
        if (!InitRunningLock()) {
            return false;
        }
    }

    if (bundleName == "") {
        wakeLockAbilities_.clear();
        wakeLock_->UnLock();
        wakeLock_ = nullptr;
        HILOG_DEBUG("wakeLock_ unLock success.");
    } else {
        if (!wakeLockAbilities_.count(bundleName)) {
            HILOG_DEBUG("bundleName: %{public}s not in holdRunningLock status.", bundleName.c_str());
            return true;
        }
        wakeLockAbilities_.erase(bundleName);
        if (wakeLockAbilities_.empty()) {
            wakeLock_->UnLock();
            wakeLock_ = nullptr;
            HILOG_DEBUG("bundleName: %{public}s erased, wakeLock_ unLock success.", bundleName.c_str());
        }
    }
    return true;
}
} // namespace Accessibility
} // namespace OHOS