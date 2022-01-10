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
#include "dummy.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"
#include "json_utils.h"

using namespace std;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace Accessibility {

AccessibilityAbilityInfo::AccessibilityAbilityInfo(AbilityInfo abilityInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    abilityInfo_ = abilityInfo;

#if 0
    // dummy data for test. It's temporary.
    AccessibilityAbilityInfoDummy accessibilityAbilityInfoDummydata;
    capabilities_ = accessibilityAbilityInfoDummydata.capabilities;
    abilityTypes_ = accessibilityAbilityInfoDummydata.accessibilityAbilityTypes;
    eventTypes_ = accessibilityAbilityInfoDummydata.accessibilityEventTypes;
    notificationTimeout_ = accessibilityAbilityInfoDummydata.notificationTimeout;
    uiInteractiveTimeout_ = accessibilityAbilityInfoDummydata.uiInteractiveTime;
    uiNoninteractiveTimeout_ = accessibilityAbilityInfoDummydata.uiNoninteractiveTime;
    targetBundleNames_ = accessibilityAbilityInfoDummydata.filterBundleNames;
    description_ = accessibilityAbilityInfoDummydata.description;
    settingsAbility_ = accessibilityAbilityInfoDummydata.settingsAbility;

#else

    if (!ParseAAConfig()) {
        HILOG_ERROR("Parse AccessibilityAbility config file failed.");
    }

#endif // 0
}

bool AccessibilityAbilityInfo::ParseAAConfig()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    nlohmann::json sourceJson;
    if (!JsonUtils::GetJsonObjFromJson(sourceJson, AccessibleAbility_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    vector<string> eventTypes;
    if (!JsonUtils::GetStringVecFromJson(sourceJson, AccessibleAbility_JSON_KEY_ACCESSIBILITY_EVENT_TYPES,
                                            eventTypes)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }
    PraseVecUtils::ParseEventTypesFromVec(eventTypes, eventTypes_);

    if (!JsonUtils::GetStringVecFromJson(sourceJson, AccessibleAbility_JSON_KEY_TARGET_BUNDLE_NAMES,
                                            targetBundleNames_)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }

    vector<string> abilityType;
    if (!JsonUtils::GetStringVecFromJson(sourceJson, AccessibleAbility_JSON_KEY_ACCESSIBILITY_ABILITY_TYPES,
                                            abilityType)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }
    PraseVecUtils::ParseAbilityTypesFromVec(abilityType, abilityTypes_);

    int notificationTimeout = -1;
    if (!JsonUtils::GetIntFromJson(sourceJson, AccessibleAbility_JSON_KEY_NOTIFICATION_TIMEOUT,
                                        notificationTimeout)) {
        HILOG_ERROR("Get int from json failed.");
        return false;
    }
    notificationTimeout_ = static_cast<uint32_t>(notificationTimeout);

    int uiNoninteractiveTimeout = -1;
    if (!JsonUtils::GetIntFromJson(sourceJson, AccessibleAbility_JSON_KEY_UI_NONINTERACTIVE_TIMEOUT,
                                        uiNoninteractiveTimeout)) {
        HILOG_ERROR("Get int from json failed.");
        return false;
    }
    uiNoninteractiveTimeout_ = static_cast<uint32_t>(uiNoninteractiveTimeout);

    int uiInteractiveTimeout = -1;
    if (!JsonUtils::GetIntFromJson(sourceJson, AccessibleAbility_JSON_KEY_UI_INTERACTIVE_TIMEOUT,
                                        uiInteractiveTimeout)) {
        HILOG_ERROR("Get int from json failed.");
        return false;
    }
    uiInteractiveTimeout_ = static_cast<uint32_t>(uiInteractiveTimeout);

    vector<string> capabilities;
    if (!JsonUtils::GetStringVecFromJson(sourceJson,
            AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES, capabilities)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return false;
    }
    PraseVecUtils::ParseCapabilitiesFromVec(capabilities, capabilities_);

    if (!JsonUtils::GetStringFromJson(sourceJson, AccessibleAbility_JSON_KEY_DESCRIPTION, description_)) {
        HILOG_ERROR("Get string from json failed.");
        return false;
    }

    if (!JsonUtils::GetStringFromJson(sourceJson, AccessibleAbility_JSON_KEY_SETTINGS_ABILITY, settingsAbility_)) {
        HILOG_ERROR("Get string from json failed.");
        return false;
    }

    return true;
}

const AbilityInfo &AccessibilityAbilityInfo::GetAbilityInfo()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return abilityInfo_;
}

uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return abilityTypes_;
}

uint32_t AccessibilityAbilityInfo::GetCapabilityValues()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return capabilities_;
}

string AccessibilityAbilityInfo::GetDescription()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return description_;
}

uint32_t AccessibilityAbilityInfo::GetEventTypes()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return eventTypes_;
}

string AccessibilityAbilityInfo::GetId()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return abilityInfo_.bundleName + "/" + abilityInfo_.name;
}

string AccessibilityAbilityInfo::GetName()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return abilityInfo_.name;
}

string AccessibilityAbilityInfo::GetPackageName()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return abilityInfo_.bundleName;
}

vector<string> AccessibilityAbilityInfo::GetFilterBundleNames()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return targetBundleNames_;
}

std::string AccessibilityAbilityInfo::GetSettingsAbility()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return settingsAbility_;
}

uint32_t AccessibilityAbilityInfo::GetNotificationTimeout()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return notificationTimeout_;
}

uint32_t AccessibilityAbilityInfo::GetUiInteractiveTime()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return uiInteractiveTimeout_;
}

uint32_t AccessibilityAbilityInfo::GetUiNoninteractiveTime()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return uiNoninteractiveTimeout_;
}

bool AccessibilityAbilityInfo::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, abilityTypes_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, capabilities_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, eventTypes_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, settingsAbility_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, notificationTimeout_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, uiInteractiveTimeout_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, uiNoninteractiveTimeout_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &targetBundleNames_);

    std::unique_ptr<AbilityInfo> abilityInfo(parcel.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        HILOG_ERROR("ReadParcelable AbilityInfo failed.");
        return false;
    }
    abilityInfo_ = *abilityInfo;

    return true;
}

bool AccessibilityAbilityInfo::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, abilityTypes_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, capabilities_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, eventTypes_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, settingsAbility_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, notificationTimeout_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, uiInteractiveTimeout_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, uiNoninteractiveTimeout_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, targetBundleNames_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &abilityInfo_);

    return true;
}

AccessibilityAbilityInfo *AccessibilityAbilityInfo::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);
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