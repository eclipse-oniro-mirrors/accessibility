/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "utils.h"

#include <hisysevent.h>

#include "bundle_mgr_client.h"
#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string KEY_ACCESSIBILITY_ABILITY_TYPES = "accessibilityAbilityTypes";
    const std::string KEY_ACCESSIBILITY_CAPABILITIES = "accessibilityCapabilities";
    const std::string KEY_SETTINGS_ABILITY = "settingsAbility";
    const std::string KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE = "accessibilityCapabilityRationale";
    const std::string KEY_IS_IMPORTANT = "isImportant";
    const std::string KEY_NEED_HIDE = "needHide";

    // The json value of accessibilityAbility type
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_SPOKEN = "spoken";
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_HAPIC = "haptic";
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_AUDIBLE = "audible";
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_VISUAL = "visual";
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_GENERIC = "generic";
    const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_ALL = "all";

    // The json value of capabilities
    const std::string CAPABILITIES_JSON_VALUE_RETRIEVE = "retrieve";
    const std::string CAPABILITIES_JSON_VALUE_TOUCH_GUIDE = "touchGuide";
    const std::string CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER = "keyEventObserver";
    const std::string CAPABILITIES_JSON_VALUE_ZOOM = "zoom";
    const std::string CAPABILITIES_JSON_VALUE_GESTURE = "gesture";
} // namespace

class JsonUtils {
public:
    static bool GetStringFromJson(const nlohmann::json &json, const std::string &key, std::string &value)
    {
        HILOG_DEBUG("start.");
        if (!json.is_object()) {
            HILOG_ERROR("json is not object.");
            return false;
        }
        if (json.find(key) != json.end() && json.at(key).is_string()) {
            HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
            value = json.at(key).get<std::string>();
        }
        return true;
    }

    static bool GetStringVecFromJson(const nlohmann::json &json, const std::string &key,
        std::vector<std::string> &value)
    {
        HILOG_DEBUG("start.");
        if (!json.is_object()) {
            HILOG_ERROR("json is not object.");
            return false;
        }
        if (json.find(key) != json.end() && json.at(key).is_array()) {
            HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
            value = json.at(key).get<std::vector<std::string>>();
        }
        return true;
    }

    static bool GetBoolFromJson(const nlohmann::json &json, const std::string &key, bool &value)
    {
        HILOG_DEBUG("start.");
        if (!json.is_object()) {
            HILOG_ERROR("json is not object.");
            return false;
        }
        if (json.find(key) != json.end() && json.at(key).is_boolean()) {
            HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
            value = json.at(key).get<bool>();
        }
        return true;
    }
};

class PraseVecUtils {
public:
    static uint32_t ParseAbilityTypesFromVec(const std::vector<std::string> &abilities)
    {
        HILOG_DEBUG("start.");
        uint32_t abilityTypes = 0;

        for (const auto &ability : abilities) {
            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_SPOKEN) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN;
            }

            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_HAPIC) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC;
            }

            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_AUDIBLE) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE;
            }

            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_VISUAL) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL;
            }

            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_GENERIC) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC;
            }

            if (ability == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_ALL) {
                abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL;
            }
        }
        return abilityTypes;
    }

    static uint32_t ParseCapabilitiesFromVec(const std::vector<std::string> &capabilities)
    {
        HILOG_DEBUG("start.");
        uint32_t capabilitiesValue = 0;

        for (const auto &capability : capabilities) {
            if (capability == CAPABILITIES_JSON_VALUE_RETRIEVE) {
                capabilitiesValue |= Capability::CAPABILITY_RETRIEVE;
            }

            if (capability == CAPABILITIES_JSON_VALUE_TOUCH_GUIDE) {
                capabilitiesValue |= Capability::CAPABILITY_TOUCH_GUIDE;
            }

            if (capability == CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER) {
                capabilitiesValue |= Capability::CAPABILITY_KEY_EVENT_OBSERVER;
            }

            if (capability == CAPABILITIES_JSON_VALUE_ZOOM) {
                capabilitiesValue |= Capability::CAPABILITY_ZOOM;
            }

            if (capability == CAPABILITIES_JSON_VALUE_GESTURE) {
                capabilitiesValue |= Capability::CAPABILITY_GESTURE;
            }
        }
        return capabilitiesValue;
    }
};

void Utils::Parse(const AppExecFwk::ExtensionAbilityInfo &abilityInfo, AccessibilityAbilityInitParams &initParams)
{
    HILOG_DEBUG("start.");
    initParams.name = abilityInfo.name;
    initParams.bundleName = abilityInfo.bundleName;
    initParams.moduleName = abilityInfo.moduleName;
    initParams.description = abilityInfo.description;
    initParams.label = abilityInfo.label;

    std::vector<std::string> profileInfos;
    std::string metadataName = "ohos.accessibleability";
    AppExecFwk::BundleMgrClient bundleMgrClient;
    bundleMgrClient.GetResConfigFile(abilityInfo, metadataName, profileInfos);
    if (profileInfos.empty()) {
        HILOG_ERROR("profileInfos is empty.");
        return;
    }

    if (!nlohmann::json::accept(profileInfos[0])) {
        HILOG_ERROR("profileInfos is not json format.");
        return;
    }
    nlohmann::json sourceJson = nlohmann::json::parse(profileInfos[0]);

    // accessibilityCapabilities
    std::vector<std::string> capabilities;
    if (!JsonUtils::GetStringVecFromJson(sourceJson, KEY_ACCESSIBILITY_CAPABILITIES, capabilities)) {
        HILOG_ERROR("Get accessibilityCapabilities from json failed.");
        return;
    }
    initParams.staticCapabilities = PraseVecUtils::ParseCapabilitiesFromVec(capabilities);

    // accessibilityAbilityTypes
    std::vector<std::string> abilityTypes;
    if (!JsonUtils::GetStringVecFromJson(sourceJson, KEY_ACCESSIBILITY_ABILITY_TYPES, abilityTypes)) {
        HILOG_ERROR("Get accessibilityAbilityTypes from json failed.");
        return;
    }
    initParams.abilityTypes = PraseVecUtils::ParseAbilityTypesFromVec(abilityTypes);

    // accessibilityCapabilityRationale
    if (!JsonUtils::GetStringFromJson(sourceJson, KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE, initParams.rationale)) {
        HILOG_ERROR("Get accessibilityCapabilityRationale from json failed.");
        return;
    }

    // settingsAbility
    if (!JsonUtils::GetStringFromJson(sourceJson, KEY_SETTINGS_ABILITY, initParams.settingsAbility)) {
        HILOG_ERROR("Get settingsAbility from json failed.");
        return;
    }

    // isImportant
    if (!JsonUtils::GetBoolFromJson(sourceJson, KEY_IS_IMPORTANT, initParams.isImportant)) {
        HILOG_ERROR("Get isImportant from json failed.");
        return;
    }

    // needHide
    if (!JsonUtils::GetBoolFromJson(sourceJson, KEY_NEED_HIDE, initParams.needHide)) {
        HILOG_ERROR("Get needHide from json failed.");
        return;
    }
}

int64_t Utils::GetSystemTime()
{
    HILOG_DEBUG("start.");

    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    int64_t millisecond = static_cast<int64_t>(times.tv_sec * 1000 + times.tv_nsec / 1000000);

    return millisecond;
}

std::string Utils::GetUri(const OHOS::AppExecFwk::ElementName &elementName)
{
    HILOG_DEBUG("bundle name(%{public}s) ability name(%{public}s)",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    return elementName.GetBundleName() + "/" + elementName.GetAbilityName();
}

std::string Utils::GetUri(const std::string &bundleName, const std::string &abilityName)
{
    HILOG_DEBUG("bundle name(%{public}s) ability name(%{public}s)", bundleName.c_str(), abilityName.c_str());
    return bundleName + "/" + abilityName;
}

std::string Utils::GetAbilityAutoStartStateKey(const std::string &bundleName, const std::string &abilityName,
    int32_t accountId)
{
    HILOG_DEBUG("bundle name(%{public}s) ability name(%{public}s) accountId(%{public}d)",
        bundleName.c_str(), abilityName.c_str(), accountId);
    return bundleName + "/" + abilityName + "/" + std::to_string(accountId);
}

void Utils::SelectUsefulFromVecWithSameBundle(std::vector<std::string> &selectVec, std::vector<std::string> &cmpVec,
    bool &hasDif, const std::string &bundleName)
{
    HILOG_DEBUG();
    for (auto iter = selectVec.begin(); iter != selectVec.end();) {
        if (iter->substr(0, iter->find("/")) != bundleName) {
            ++iter;
            continue;
        }
        auto it = cmpVec.begin();
        for (; it != cmpVec.end(); ++it) {
            if ((*it) == (*iter)) {
                break;
            }
        }
        if (it == cmpVec.end()) {
            selectVec.erase(iter);
            hasDif = true;
        } else {
            ++iter;
        }
    }
}

void Utils::RecordUnavailableEvent(A11yUnavailableEvent event, A11yError errCode,
    const std::string &bundleName, const std::string &abilityName)
{
    if (!(errCode > A11yError::ERROR_NEED_REPORT_BASE && errCode < A11yError::ERROR_NEED_REPORT_END)) {
        return;
    }
    std::ostringstream oss;
    oss << "accessibility function is unavailable: " << "event: " << TransferUnavailableEventToString(event)
        << ", errCode: " << static_cast<int32_t>(errCode)
        << ", bundleName: " << bundleName << ", abilityName: " << abilityName << ";";
    std::string info = oss.str();
    HILOG_DEBUG("accessibility function is unavailable: %{public}s", info.c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "UNAVAILABLE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "MSG", info);
    if (ret != 0) {
        HILOG_ERROR("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

std::string Utils::TransferUnavailableEventToString(A11yUnavailableEvent type)
{
    std::string event;
    switch (type) {
        case A11yUnavailableEvent::READ_EVENT:
            event = "READ";
            break;
        case A11yUnavailableEvent::CONNECT_EVENT:
            event = "CONNECT";
            break;
        case A11yUnavailableEvent::QUERY_EVENT:
            event = "QUERY";
            break;
        default:
            event = "UNDEFINE";
            break;
    }
    return event;
}

void Utils::RecordStartingA11yEvent(uint32_t flag)
{
    std::ostringstream oss;
    oss << "starting accessibility: " << "event: " << "system" << ", id: " << flag << ";";
    HILOG_DEBUG("starting accessibility: %{public}s", oss.str().c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "STARTING_FUNCTION",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "MSG", oss.str());
    if (ret != 0) {
        HILOG_ERROR("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void Utils::RecordStartingA11yEvent(const std::string &name)
{
    std::ostringstream oss;
    oss << "starting accessibility: " << "event: " << "extension" << ", name: " << name << ";";
    HILOG_DEBUG("starting accessibility: %{public}s", oss.str().c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "STARTING_FUNCTION",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "MSG", oss.str());
    if (ret != 0) {
        HILOG_ERROR("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void Utils::VectorToString(const std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG();
    int32_t i = 0;
    for (auto& var : vectorVal) {
        if (i > 0) {
            stringOut = stringOut + ',';
        }
        stringOut = stringOut + var.c_str();
        i++;
    }
    HILOG_DEBUG("stringOUT = %{public}s .", stringOut.c_str());
}

void Utils::StringToVector(const std::string &stringIn, std::vector<std::string> &vectorResult)
{
    HILOG_DEBUG();
    int32_t strLength = static_cast<int32_t>(stringIn.size());
    std::vector<int32_t> position;

    if (strLength == 0) {
        return;
    }

    for (int32_t j = 0; j < strLength; j++) {
        if (stringIn[j] == ',') {
            position.push_back(j);
        }
    }

    int32_t wrodCount = static_cast<int32_t>(position.size());
    if ((wrodCount == 0) && (strLength > 0)) {
        vectorResult.push_back(stringIn);
    } else {
        int32_t startWrod = 0;
        int32_t length = 0;
        for (int32_t i = 0; i <= wrodCount; i++) {
            if (i == 0) {
                length = position[i];
                vectorResult.push_back(stringIn.substr(startWrod, length)); // First string
            } else if (i < wrodCount) {
                startWrod = position[i - 1] + 1;
                length = position[i] - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Second string to last-1 string
            } else {
                startWrod = position[i - 1] + 1;
                length = strLength - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Last string
            }
        }
    }
    HILOG_DEBUG("strLength = %{public}d, wrodCount = %{public}d, stringIn : %{public}s",
        strLength, wrodCount, stringIn.c_str());
    for (auto& var : vectorResult) {
        HILOG_DEBUG("vectorResult = %{public}s ", var.c_str());
    }
}
} // namespace Accessibility
} // namespace OHOS