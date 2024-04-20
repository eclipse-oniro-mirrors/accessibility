/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include "accessibility_datashare_helper.h"

#include "datashare_errno.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "uri.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {

namespace {
    constexpr int32_t INDEX = 0;
    const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
    const std::string SETTING_COLUMN_VALUE = "VALUE";
    constexpr int32_t DECIMAL_NOTATION = 10;
    const std::string SETTING_GLOBAL_URI = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA";
    const std::string SETTING_SYSTEM_URI = "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_";
    const std::string SETTING_SECURE_URI =
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_";
}

AccessibilityDatashareHelper::AccessibilityDatashareHelper(DATASHARE_TYPE type, int32_t accountId)
    :type_(type), accountId_(accountId)
{
    HILOG_DEBUG();
}

std::string AccessibilityDatashareHelper::GetStringValue(const std::string& key, const std::string& defaultValue)
{
    std::string resultStr = defaultValue;
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    std::shared_ptr<DataShare::DataShareResultSet> resultSet = nullptr;
    auto helper = CreateDatashareHelper();
    do {
        std::vector<std::string> columns = { SETTING_COLUMN_VALUE };
        DataShare::DataSharePredicates predicates;
        Uri uri(AssembleUri(key));
        int32_t count = 0;
        predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
        if (helper == nullptr) {
            break;
        }
        resultSet = helper->Query(uri, predicates, columns);
        if (resultSet == nullptr) {
            break;
        }
        resultSet->GetRowCount(count);
        if (count == 0) {
            break;
        }
        resultSet->GoToRow(INDEX);
        int32_t rtn = resultSet->GetString(INDEX, resultStr);
        if (rtn  != DataShare::E_OK) {
            break;
        }
    } while (0);
    if (helper != nullptr) {
        DestoryDatashareHelper(helper);
        helper = nullptr;
    }
    if (resultSet != nullptr) {
        resultSet->Close();
        resultSet = nullptr;
    }
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return resultStr;
}

int64_t AccessibilityDatashareHelper::GetLongValue(const std::string& key, const int64_t& defaultValue)
{
    int64_t result = defaultValue;
    std::string valueStr = GetStringValue(key, "");
    if (valueStr != "") {
        result = static_cast<int64_t>(std::strtoll(valueStr.c_str(), nullptr, DECIMAL_NOTATION));
    }
    return result;
}

int32_t AccessibilityDatashareHelper::GetIntValue(const std::string& key, const int32_t& defaultValue)
{
    int64_t valueLong = GetLongValue(key, defaultValue);
    return static_cast<int32_t>(valueLong);
}

bool AccessibilityDatashareHelper::GetBoolValue(const std::string& key, const bool& defaultValue)
{
    bool result = defaultValue;
    std::string valueStr = GetStringValue(key, "");
    if (valueStr != "") {
        result = (valueStr == "1" || valueStr == "true");
    }
    return result;
}

float AccessibilityDatashareHelper::GetFloatValue(const std::string& key, const float& defaultValue)
{
    float result = defaultValue;
    std::string valueStr = GetStringValue(key, "");
    if (valueStr != "") {
        result = std::stof(valueStr);
    }
    return result;
}

RetError AccessibilityDatashareHelper::PutStringValue(const std::string& key, const std::string& value, bool needNotify)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDatashareHelper();
    RetError rtn = RET_OK;
    do {
        if (helper == nullptr) {
            rtn = RET_ERR_NULLPTR;
            break;
        }
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
    } while (0);
    if (helper != nullptr) {
        DestoryDatashareHelper(helper);
        helper = nullptr;
    }
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return rtn;
}

RetError AccessibilityDatashareHelper::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError AccessibilityDatashareHelper::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError AccessibilityDatashareHelper::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    std::string valueStr = value ? "true" : "false";
    return PutStringValue(key, valueStr, needNotify);
}

RetError AccessibilityDatashareHelper::PutFloatValue(const std::string& key, float value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

void AccessibilityDatashareHelper::Initialize(int32_t systemAbilityId)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        HILOG_ERROR("get sam return nullptr");
        return;
    }
    auto remoteObj = systemAbilityManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        HILOG_ERROR("Get remoteObj return nullptr, systemAbilityId=%{public}d", systemAbilityId);
        return;
    }
    remoteObj_ = remoteObj;
    switch (type_) {
        case DATASHARE_TYPE::GLOBAL:
            uriProxyStr_ = SETTING_GLOBAL_URI + "?Proxy=true";
            break;
        case DATASHARE_TYPE::SYSTEM:
            uriProxyStr_ = SETTING_SYSTEM_URI + std::to_string(accountId_) + "?Proxy=true";
            break;
        case DATASHARE_TYPE::SECURE:
            uriProxyStr_ = SETTING_SECURE_URI + std::to_string(accountId_) + "?Proxy=true";
            break;
        default:
            uriProxyStr_ = SETTING_GLOBAL_URI + "?Proxy=true";
            HILOG_WARN("undefined DATASHARE_TYPE, use global table");
            break;
    }
}

sptr<AccessibilitySettingObserver> AccessibilityDatashareHelper::CreateObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    sptr<AccessibilitySettingObserver> observer = new AccessibilitySettingObserver();
    observer->SetKey(key);
    observer->SetUpdateFunc(func);
    return observer;
}

RetError AccessibilityDatashareHelper::RegisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDatashareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return RET_ERR_NULLPTR;
    }
    helper->RegisterObserver(uri, observer);
    DestoryDatashareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return RET_OK;
}

RetError AccessibilityDatashareHelper::UnregisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDatashareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return RET_ERR_NULLPTR;
    }
    helper->UnregisterObserver(uri, observer);
    DestoryDatashareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to unregister observer of uri=%{public}s", uri.ToString().c_str());
    return RET_OK;
}

std::shared_ptr<DataShare::DataShareHelper> AccessibilityDatashareHelper::CreateDatashareHelper()
{
    if (remoteObj_ == nullptr) {
        return nullptr;
    }
    auto helper = DataShare::DataShareHelper::Creator(remoteObj_, uriProxyStr_);
    if (helper == nullptr) {
        HILOG_WARN("helper is nullptr, uri=%{public}s, remoteObj_=%{public}p", uriProxyStr_.c_str(),
            remoteObj_.GetRefPtr());
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::READ_EVENT, A11yError::ERROR_READ_FAILED);
        return nullptr;
    }
    return helper;
}

bool AccessibilityDatashareHelper::DestoryDatashareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    if (helper && !helper->Release()) {
        HILOG_WARN("release helper fail, remoteObj_=%{public}p", remoteObj_.GetRefPtr());
        return false;
    }
    return true;
}

Uri AccessibilityDatashareHelper::AssembleUri(const std::string& key)
{
    Uri uri(uriProxyStr_ + "&key=" + key);
    return uri;
}

} // namespace Accessibility
} // namespace OHOS

