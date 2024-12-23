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

#include "accessibility_setting_provider.h"

#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "result_set.h"
#include "uri.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
std::shared_ptr<AccessibilitySettingProvider> AccessibilitySettingProvider::instance_ = nullptr;
ffrt::mutex AccessibilitySettingProvider::mutex_;
namespace {
    constexpr int32_t DEFAULT_ACCOUNT_ID = 100;
} // namespace

AccessibilitySettingProvider::AccessibilitySettingProvider()
    : AccessibilityDatashareHelper(DATASHARE_TYPE::GLOBAL, DEFAULT_ACCOUNT_ID)
{
}

AccessibilitySettingProvider::~AccessibilitySettingProvider()
{
    instance_ = nullptr;
}

std::shared_ptr<AccessibilitySettingProvider> AccessibilitySettingProvider::GetInstance(int32_t systemAbilityId)
{
    HILOG_DEBUG("etInstance start, systemAbilityId = %{public}d.", systemAbilityId);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = std::make_shared<AccessibilitySettingProvider>();
        instance_->Initialize(systemAbilityId);
    }
    return instance_;
}

void AccessibilitySettingProvider::DeleteInstance()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        HILOG_INFO("instance_ is nullptr");
        return;
    }
    instance_ = nullptr;
}

RetError AccessibilitySettingProvider::GetIntValue(const std::string& key, int32_t& value)
{
    value = AccessibilityDatashareHelper::GetIntValue(key, 0);
    return RET_OK;
}

RetError AccessibilitySettingProvider::GetLongValue(const std::string& key, int64_t& value)
{
    value = AccessibilityDatashareHelper::GetLongValue(key, 0);
    return RET_OK;
}

RetError AccessibilitySettingProvider::GetFloatValue(const std::string& key, float& value)
{
    value = AccessibilityDatashareHelper::GetFloatValue(key, 0);
    return RET_OK;
}

RetError AccessibilitySettingProvider::GetBoolValue(const std::string& key, bool& value)
{
    value = AccessibilityDatashareHelper::GetBoolValue(key, 0);
    return RET_OK;
}

RetError AccessibilitySettingProvider::GetStringValue(const std::string& key, std::string& value)
{
    value = AccessibilityDatashareHelper::GetStringValue(key, "");
    return RET_OK;
}

RetError AccessibilitySettingProvider::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutIntValue(key, value, needNotify);
}

RetError AccessibilitySettingProvider::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutLongValue(key, value, needNotify);
}

RetError AccessibilitySettingProvider::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutBoolValue(key, value, needNotify);
}

RetError AccessibilitySettingProvider::PutStringValue(const std::string& key, const std::string& value, bool needNotify)
{
    return AccessibilityDatashareHelper::PutStringValue(key, value, needNotify);
}

sptr<AccessibilitySettingObserver> AccessibilitySettingProvider::CreateObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    return AccessibilityDatashareHelper::CreateObserver(key, func);
}

RetError AccessibilitySettingProvider::RegisterObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    return AccessibilityDatashareHelper::RegisterObserver(key, func);
}

RetError AccessibilitySettingProvider::UnregisterObserver(const std::string& key)
{
    return AccessibilityDatashareHelper::UnregisterObserver(key);
}
} // namespace Accessibility
} // namespace OHOS