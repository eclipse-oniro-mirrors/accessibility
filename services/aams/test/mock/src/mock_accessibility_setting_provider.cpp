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
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilitySettingProvider* AccessibilitySettingProvider::instance_;
std::mutex AccessibilitySettingProvider::mutex_;
sptr<IRemoteObject> AccessibilitySettingProvider::remoteObj_;
namespace {
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
} // namespace

AccessibilitySettingProvider::~AccessibilitySettingProvider()
{
    HILOG_DEBUG("start.");
    instance_ = nullptr;
    remoteObj_ = nullptr;
}

AccessibilitySettingProvider& AccessibilitySettingProvider::GetInstance(int32_t systemAbilityId)
{
    HILOG_DEBUG("start.");
    if (instance_ == nullptr) {
        instance_ = new AccessibilitySettingProvider();
    }
    return *instance_;
}

ErrCode AccessibilitySettingProvider::GetIntValue(const std::string& key, int32_t& value)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::GetLongValue(const std::string& key, int64_t& value)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::GetBoolValue(const std::string& key, bool& value)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    (void)needNotify;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    (void)needNotify;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    (void)needNotify;
    return ERR_OK;
}

bool AccessibilitySettingProvider::IsValidKey(const std::string& key)
{
    HILOG_DEBUG("start.");
    (void)key;
    return true;
}

sptr<AccessibilitySettingObserver> AccessibilitySettingProvider::CreateObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)func;
    return nullptr;
}

ErrCode AccessibilitySettingProvider::RegisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    HILOG_DEBUG("start.");
    (void)observer;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::UnregisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    HILOG_DEBUG("start.");
    (void)observer;
    return ERR_OK;
}

void AccessibilitySettingProvider::Initialize(int32_t systemAbilityId)
{
    HILOG_DEBUG("start.");
    (void)systemAbilityId;
}

ErrCode AccessibilitySettingProvider::GetStringValue(const std::string& key, std::string& value)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutStringValue
    (const std::string& key, const std::string& value, bool needNotify)
{
    HILOG_DEBUG("start.");
    (void)key;
    (void)value;
    (void)needNotify;
    return ERR_OK;
}

std::shared_ptr<DataShare::DataShareHelper> AccessibilitySettingProvider::CreateDataShareHelper()
{
    HILOG_DEBUG("start.");
    return nullptr;
}

bool AccessibilitySettingProvider::ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    HILOG_DEBUG("start.");
    (void)helper;
    return true;
}

Uri AccessibilitySettingProvider::AssembleUri(const std::string& key)
{
    HILOG_DEBUG("start.");
    (void)key;
    Uri uri(SETTING_URI_PROXY + "&key=" + "ok");
    return uri;
}
} // namespace Accessibility
} // namespace OHOS