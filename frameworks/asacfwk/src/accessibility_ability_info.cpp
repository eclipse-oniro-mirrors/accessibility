/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_ability_info.h"

#include "bundle_mgr_client.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"
#include "json_utils.h"

using namespace std;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace Accessibility {
AccessibilityAbilityInfo::AccessibilityAbilityInfo(AppExecFwk::ExtensionAbilityInfo abilityInfo)
{
    HILOG_DEBUG("start.");
    bundleName_ = abilityInfo.bundleName;
    moduleName_ = abilityInfo.moduleName;
    name_ = abilityInfo.name;
    HILOG_DEBUG("bundle name is [%{public}s].", bundleName_.c_str());
    HILOG_DEBUG("moudle name is [%{public}s].", moduleName_.c_str());
    HILOG_DEBUG("ability name is [%{public}s]].", name_.c_str());
    description_ = abilityInfo.description;
    std::vector<std::string> profileInfos;
    std::string metadataName = "ohos.accessibleability";
    BundleMgrClient bundleMgrClient;
    bundleMgrClient.GetResConfigFile(abilityInfo, metadataName, profileInfos);
    if (profileInfos.empty()) {
        HILOG_ERROR("profileInfos is empty.");
        return;
    }
    nlohmann::json sourceJson = nlohmann::json::parse(profileInfos[0]);
    if (!ParseAAConfig(sourceJson)) {
      HILOG_ERROR("Parse AccessibilityAbility config file failed.");
    }
}

bool AccessibilityAbilityInfo::ParseAAConfig(nlohmann::json sourceJson)
{
    HILOG_DEBUG("start.");
    //accessibilityCapabilities
    vector<string> capabilities;
    if (!JsonUtils::GetStringVecFromJson(
        sourceJson, AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES,
        capabilities)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }
    PraseVecUtils::ParseCapabilitiesFromVec(capabilities, capabilities_);

    //accessibilityCapabilityRationale
    if (!JsonUtils::GetStringFromJson(
        sourceJson,
        AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE,
        rationale_)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }

    // settingsAbility
    if (!JsonUtils::GetStringFromJson(
        sourceJson, AccessibleAbility_JSON_KEY_SETTINGS_ABILITY,
        settingsAbility_)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }
    return true;
}

uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
    HILOG_DEBUG("types(%{public}d)start.", abilityTypes_);
    return abilityTypes_;
}

uint32_t AccessibilityAbilityInfo::GetCapabilityValues()
{
    HILOG_DEBUG("start.");
    return capabilities_;
}

string AccessibilityAbilityInfo::GetDescription()
{
    HILOG_DEBUG("start.");
    return description_;
}

uint32_t AccessibilityAbilityInfo::GetEventTypes()
{
    HILOG_DEBUG("start.");
    return eventTypes_;
}

string AccessibilityAbilityInfo::GetId()
{
    HILOG_DEBUG("start.");
    return bundleName_ + "/" + name_;
}

string AccessibilityAbilityInfo::GetName()
{
    HILOG_DEBUG("start.");
    return name_;
}

string AccessibilityAbilityInfo::GetPackageName()
{
    HILOG_DEBUG("start.");
    return bundleName_;
}

void AccessibilityAbilityInfo::SetPackageName(string bundleName)
{
    HILOG_DEBUG("start.");
    bundleName_ = bundleName;
}

vector<string> AccessibilityAbilityInfo::GetFilterBundleNames()
{
    HILOG_DEBUG("start.");
    return targetBundleNames_;
}

std::string AccessibilityAbilityInfo::GetSettingsAbility()
{
    HILOG_DEBUG("start.");
    return settingsAbility_;
}


bool AccessibilityAbilityInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start.");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, moduleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, name_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, capabilities_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, rationale_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, abilityTypes_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, eventTypes_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, settingsAbility_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &targetBundleNames_);

    return true;
}

bool AccessibilityAbilityInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start.");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, moduleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, name_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, capabilities_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, rationale_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, abilityTypes_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, eventTypes_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, settingsAbility_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, targetBundleNames_);

    return true;
}

AccessibilityAbilityInfo *AccessibilityAbilityInfo::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG("start.");
    AccessibilityAbilityInfo *accessibilityAbilityInfo = new AccessibilityAbilityInfo();

    if (accessibilityAbilityInfo && !accessibilityAbilityInfo->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel AccessibilityAbilityInfo failed.");
        delete accessibilityAbilityInfo;
        accessibilityAbilityInfo = nullptr;
    }

    return accessibilityAbilityInfo;
}

} // namespace Accessibility
} // namespace OHOS