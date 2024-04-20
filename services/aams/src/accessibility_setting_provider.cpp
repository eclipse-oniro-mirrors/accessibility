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

#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "rdb_errno.h"
#include "result_set.h"
#include "uri.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilitySettingProvider* AccessibilitySettingProvider::instance_;
std::mutex AccessibilitySettingProvider::mutex_;
sptr<IRemoteObject> AccessibilitySettingProvider::remoteObj_;
std::mutex AccessibilitySettingProvider::observerMutex_;
namespace {
const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
const std::string SETTING_COLUMN_VALUE = "VALUE";
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr int32_t PARAM_NUM_TEN = 10;
} // namespace

AccessibilitySettingProvider::~AccessibilitySettingProvider()
{
    instance_ = nullptr;
    remoteObj_ = nullptr;
}

AccessibilitySettingProvider& AccessibilitySettingProvider::GetInstance(int32_t systemAbilityId)
{
    HILOG_DEBUG("etInstance start, systemAbilityId = %{public}d.", systemAbilityId);
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new AccessibilitySettingProvider();
            Initialize(systemAbilityId);
        }
    }
    return *instance_;
}

void AccessibilitySettingProvider::DeleteInstance()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        HILOG_INFO("instance_ is nullptr");
        return;
    }
    delete instance_;
    instance_ = nullptr;
}

ErrCode AccessibilitySettingProvider::GetIntValue(const std::string& key, int32_t& value)
{
    int64_t valueLong;
    ErrCode ret = GetLongValue(key, valueLong);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int32_t>(valueLong);
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::GetLongValue(const std::string& key, int64_t& value)
{
    std::string valueStr;
    ErrCode ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int64_t>(strtoll(valueStr.c_str(), nullptr, PARAM_NUM_TEN));
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::GetFloatValue(const std::string& key, float& value)
{
    std::string valueStr;
    ErrCode ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = std::stof(valueStr);
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::GetBoolValue(const std::string& key, bool& value)
{
    std::string valueStr;
    ErrCode ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = (valueStr == "1" || valueStr == "true");
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

ErrCode AccessibilitySettingProvider::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

ErrCode AccessibilitySettingProvider::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    std::string valueStr = value ? "true" : "false";
    return PutStringValue(key, valueStr, needNotify);
}

bool AccessibilitySettingProvider::IsValidKey(const std::string& key)
{
    std::string value;
    ErrCode ret = GetStringValue(key, value);
    return (ret != ERR_NAME_NOT_FOUND) && (!value.empty());
}

sptr<AccessibilitySettingObserver> AccessibilitySettingProvider::CreateObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    sptr<AccessibilitySettingObserver> observer = new AccessibilitySettingObserver();
    observer->SetKey(key);
    observer->SetUpdateFunc(func);
    return observer;
}

ErrCode AccessibilitySettingProvider::RegisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    if (!observer) {
        HILOG_ERROR("observer is nullptr.");
        return ERR_INVALID_VALUE;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    helper->RegisterObserver(uri, observer);
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::RegisterObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    sptr<AccessibilitySettingObserver> observer = CreateObserver(key, func);
    if (observer == nullptr) {
        return ERR_NO_MEMORY;
    }
    if (RegisterObserver(observer) != ERR_OK) {
        return ERR_INVALID_OPERATION;
    }
    std::lock_guard<std::mutex> lock(observerMutex_);
    settingObserverMap_.insert(std::make_pair(key, observer));
    return RET_OK;
}

ErrCode AccessibilitySettingProvider::UnregisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    if (!observer) {
        HILOG_ERROR("observer is nullptr.");
        return ERR_INVALID_VALUE;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    helper->UnregisterObserver(uri, observer);
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to unregister observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::UnregisterObserver(const std::string& key)
{
    auto iter = settingObserverMap_.find(key);
    if (iter != settingObserverMap_.end() && iter->second != nullptr) {
        if (UnregisterObserver(iter->second) == ERR_OK) {
            std::lock_guard<std::mutex> lock(observerMutex_);
            settingObserverMap_.erase(iter);
            return RET_OK;
        } else {
            HILOG_WARN("succeed to unregister observer of key %{public}s", key.c_str());
            return ERR_INVALID_OPERATION;
        }
    }
    HILOG_WARN("failed to find the key %{public}s", key.c_str());
    return ERR_NAME_NOT_FOUND;
}

void AccessibilitySettingProvider::Initialize(int32_t systemAbilityId)
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        HILOG_ERROR("get sam return nullptr");
        return;
    }
    auto remoteObj = sam->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        HILOG_ERROR("Get remoteObj return nullptr, systemAbilityId=%{public}d", systemAbilityId);
        return;
    }
    remoteObj_ = remoteObj;
}

ErrCode AccessibilitySettingProvider::GetStringValue(const std::string& key, std::string& value)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    HILOG_DEBUG("key=%{public}s", key.c_str());
    Uri uri(AssembleUri(key));
    auto resultSet = helper->Query(uri, predicates, columns);
    ReleaseDataShareHelper(helper);
    if (resultSet == nullptr) {
        HILOG_ERROR("helper->Query return nullptr");
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_INVALID_OPERATION;
    }
    int32_t count;
    resultSet->GetRowCount(count);
    if (count == 0) {
        HILOG_WARN("not found value, key=%{public}s, count=%{public}d", key.c_str(), count);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NAME_NOT_FOUND;
    }
    const int32_t INDEX = 0;
    resultSet->GoToRow(INDEX);
    int32_t ret = resultSet->GetString(INDEX, value);
    if (ret != NativeRdb::E_OK) {
        HILOG_WARN("resultSet->GetString return not ok, ret=%{public}d", ret);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_INVALID_VALUE;
    }
    resultSet->Close();
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return ERR_OK;
}

ErrCode AccessibilitySettingProvider::PutStringValue
    (const std::string& key, const std::string& value, bool needNotify)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("helper is null.");
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    HILOG_DEBUG("key=%{public}s, value=%{public}s", key.c_str(), value.c_str());
    DataShare::DataShareValueObject keyObj(key);
    DataShare::DataShareValueObject valueObj(value);
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(SETTING_COLUMN_KEYWORD, keyObj);
    bucket.Put(SETTING_COLUMN_VALUE, valueObj);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(AssembleUri(key));
    if (helper->Update(uri, predicates, bucket) <= 0) {
        HILOG_DEBUG("no data exist, insert one row");
        auto ret = helper->Insert(uri, bucket);
        HILOG_DEBUG("helper insert ret(%{public}d).", static_cast<int>(ret));
    }
    if (needNotify) {
        helper->NotifyChange(AssembleUri(key));
    }
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return ERR_OK;
}

std::shared_ptr<DataShare::DataShareHelper> AccessibilitySettingProvider::CreateDataShareHelper()
{
    auto helper = DataShare::DataShareHelper::Creator(remoteObj_, SETTING_URI_PROXY);
    if (helper == nullptr) {
        HILOG_WARN("helper is nullptr, uri=%{public}s, remoteObj_=%{public}p", SETTING_URI_PROXY.c_str(),
            remoteObj_.GetRefPtr());
        return nullptr;
    }
    return helper;
}

bool AccessibilitySettingProvider::ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    if (!helper->Release()) {
        HILOG_WARN("release helper fail, remoteObj_=%{public}p", remoteObj_.GetRefPtr());
        return false;
    }
    return true;
}

Uri AccessibilitySettingProvider::AssembleUri(const std::string& key)
{
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    return uri;
}
} // namespace Accessibility
} // namespace OHOS