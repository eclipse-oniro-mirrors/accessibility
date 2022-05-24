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

#include "json_utils.h"
#include <fstream>
#include <sstream>
#include "bundle_mgr_client.h"
#include "hilog_wrapper.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string AccessibleAbility_JSON_FILE_PATH = "/system/app/dummy_accessibility_ability_config.json";
    const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_ABILITY_TYPES = "accessibilityAbilityTypes";
    const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES = "accessibilityCapabilities";
    const std::string AccessibleAbility_JSON_KEY_SETTINGS_ABILITY = "settingsAbility";
    const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE =
        "accessibilityCapabilityRationale";
    const std::string AccessibleAbility_Config_JSON_FILE_PATH = "/system/app/accessibility_config.json";

    // The json value of event types
    const std::string EVENT_TYPES_JSON_VALUE_CLICK = "click";
    const std::string EVENT_TYPES_JSON_VALUE_LONG_CLICK = "longClick";
    const std::string EVENT_TYPES_JSON_VALUE_SELECT = "select";
    const std::string EVENT_TYPES_JSON_VALUE_FOCUS = "focus";
    const std::string EVENT_TYPES_JSON_VALUE_TEXT_UPDATE = "textUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_PAGE_STATE_UPDATE = "pageStateUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_NOTIFICATION_UPDATE = "notificationUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_HOVER_ENTER = "hoverEnter";
    const std::string EVENT_TYPES_JSON_VALUE_HOVER_EXIT = "hoverExit";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_BEGIN = "touchGuideBegin";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_END = "touchGuideEnd";
    const std::string EVENT_TYPES_JSON_VALUE_PAGE_CONTENT_UPDATE = "pageContentUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_SCROLL = "scroll";
    const std::string EVENT_TYPES_JSON_VALUE_TEXT_SELECTION_UPDATE = "textSelectionUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_PUBLIC_NOTICE = "publicNotice";
    const std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS = "accessibilityFocus";
    const std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS_CLEAR = "accessibilityFocusClear";
    const std::string EVENT_TYPES_JSON_VALUE_TEXT_MOVE_UNIT = "textMoveUnit";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_BEGIN = "touchGuideGestureBegin";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_END = "touchGuideGestureEnd";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_BEGIN = "touchBegin";
    const std::string EVENT_TYPES_JSON_VALUE_TOUCH_END = "touchEnd";
    const std::string EVENT_TYPES_JSON_VALUE_WINDOW_UPDATE = "windowUpdate";
    const std::string EVENT_TYPES_JSON_VALUE_INTERRUPT = "interrupt";
    const std::string EVENT_TYPES_JSON_VALUE_GESTURE_EVENT = "gesture";
    const std::string EVENT_TYPES_JSON_VALUE_ALL = "all";

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

void AccessibilityConfigParse::Parse(const AppExecFwk::ExtensionAbilityInfo &abilityInfo,
    AccessibilityAbilityInitParams &initParams)
{
    HILOG_DEBUG("start.");
    initParams.name = abilityInfo.name;
    initParams.bundleName = abilityInfo.bundleName;
    initParams.moduleName = abilityInfo.moduleName;
    initParams.description = abilityInfo.description;

    std::vector<std::string> profileInfos;
    std::string metadataName = "ohos.accessibleability";
    AppExecFwk::BundleMgrClient bundleMgrClient;
    bundleMgrClient.GetResConfigFile(abilityInfo, metadataName, profileInfos);
    if (profileInfos.empty()) {
        HILOG_ERROR("profileInfos is empty.");
        return;
    }

    nlohmann::json sourceJson = nlohmann::json::parse(profileInfos[0]);

    // accessibilityCapabilities
    vector<string> capabilities;
    if (!JsonUtils::GetStringVecFromJson(
        sourceJson, AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES,
        capabilities)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return;
    }
    PraseVecUtils::ParseCapabilitiesFromVec(capabilities, initParams.capabilities);

    // accessibilityAbilityTypes
    vector<string> abilityTypes;
    if (!JsonUtils::GetStringVecFromJson(
        sourceJson, AccessibleAbility_JSON_KEY_ACCESSIBILITY_ABILITY_TYPES,
        abilityTypes)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return;
    }
    PraseVecUtils::ParseAbilityTypesFromVec(abilityTypes, initParams.abilityTypes);

    // accessibilityCapabilityRationale
    if (!JsonUtils::GetStringFromJson(
        sourceJson,
        AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE,
        initParams.rationale)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return;
    }

    // settingsAbility
    if (!JsonUtils::GetStringFromJson(
        sourceJson, AccessibleAbility_JSON_KEY_SETTINGS_ABILITY,
        initParams.settingsAbility)) {
        HILOG_ERROR("Get stringVec from json failed.");
        return;
    }
}

bool JsonUtils::GetJsonObjFromJson(nlohmann::json &jsonObj, const std::string &jsonPath)
{
    HILOG_DEBUG("start.");

    char realPath[PATH_MAX + 1] = { 0 };
    if (jsonPath.length() > PATH_MAX) {
        HILOG_ERROR("jsonPath is too long");
        return false;
    }
    if (!realpath(jsonPath.c_str(), realPath)) {
        HILOG_ERROR("Fail to get realpath of %{public}s", jsonPath.c_str());
        return false;
    }

    std::ifstream jsonFileStream;
    jsonFileStream.open(realPath, std::ios::in);
    if (!jsonFileStream.is_open()) {
        HILOG_ERROR("Open json file failed.");
        return false;
    }

    std::ostringstream buf;
    char ch;
    while (buf && jsonFileStream.get(ch)) {
        buf.put(ch);
    }
    jsonFileStream.close();

    jsonObj = nlohmann::json::parse(buf.str(), nullptr, false);
    if (!jsonObj.is_structured()) {
        HILOG_ERROR("Parse json file into jsonObj failed.");
        return false;
    }
    return true;
}

bool JsonUtils::GetStringFromJson(const nlohmann::json &json, const std::string &key, std::string &value)
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

bool JsonUtils::GetInt32FromJson(const nlohmann::json &json, const std::string &key, int32_t &value)
{
    HILOG_DEBUG("start.");
    if (!json.is_object()) {
        HILOG_ERROR("json is not object.");
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_number()) {
        HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
        value = json.at(key).get<int32_t>();
    }
    return true;
}

bool JsonUtils::GetStringVecFromJson(const nlohmann::json &json, const std::string &key,
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

bool JsonUtils::ParseObjVecFromJson(const nlohmann::json &json, const std::string &key,
                                    std::vector<nlohmann::json> &value)
{
    HILOG_DEBUG("start.");
    if (!json.is_object()) {
        HILOG_ERROR("json is not object.");
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_array()) {
        HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
        value = json.at(key).get<std::vector<nlohmann::json>>();
    }
    return true;
}

void PraseVecUtils::ParseEventTypesFromVec(const vector<string>& events, uint32_t& eventTypes)
{
    HILOG_DEBUG("start.");
    eventTypes = EventType::TYPE_VIEW_INVALID;
    static std::map<std::string, uint32_t> eventTypesTable = {
        {EVENT_TYPES_JSON_VALUE_CLICK, EventType::TYPE_VIEW_CLICKED_EVENT},
        {EVENT_TYPES_JSON_VALUE_LONG_CLICK, EventType::TYPE_VIEW_LONG_CLICKED_EVENT},
        {EVENT_TYPES_JSON_VALUE_SELECT, EventType::TYPE_VIEW_SELECTED_EVENT},
        {EVENT_TYPES_JSON_VALUE_FOCUS, EventType::TYPE_VIEW_FOCUSED_EVENT},
        {EVENT_TYPES_JSON_VALUE_TEXT_UPDATE, EventType::TYPE_VIEW_TEXT_UPDATE_EVENT},
        {EVENT_TYPES_JSON_VALUE_PAGE_STATE_UPDATE, EventType::TYPE_PAGE_STATE_UPDATE},
        {EVENT_TYPES_JSON_VALUE_NOTIFICATION_UPDATE, EventType::TYPE_NOTIFICATION_UPDATE_EVENT},
        {EVENT_TYPES_JSON_VALUE_HOVER_ENTER, EventType::TYPE_VIEW_HOVER_ENTER_EVENT},
        {EVENT_TYPES_JSON_VALUE_HOVER_EXIT, EventType::TYPE_VIEW_HOVER_EXIT_EVENT},
        {EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_BEGIN, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN},
        {EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_END, EventType::TYPE_TOUCH_GUIDE_GESTURE_END},
        {EVENT_TYPES_JSON_VALUE_PAGE_CONTENT_UPDATE, EventType::TYPE_PAGE_CONTENT_UPDATE},
        {EVENT_TYPES_JSON_VALUE_SCROLL, EventType::TYPE_VIEW_SCROLLED_EVENT},
        {EVENT_TYPES_JSON_VALUE_TEXT_SELECTION_UPDATE, EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT},
        {EVENT_TYPES_JSON_VALUE_PUBLIC_NOTICE, EventType::TYPE_PUBLIC_NOTICE_EVENT},
        {EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS, EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT},
        {EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS_CLEAR, EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT},
        {EVENT_TYPES_JSON_VALUE_TEXT_MOVE_UNIT, EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT},
        {EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_BEGIN, EventType::TYPE_TOUCH_GUIDE_BEGIN},
        {EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_END, EventType::TYPE_TOUCH_GUIDE_END},
        {EVENT_TYPES_JSON_VALUE_TOUCH_BEGIN, EventType::TYPE_TOUCH_BEGIN},
        {EVENT_TYPES_JSON_VALUE_TOUCH_END, EventType::TYPE_TOUCH_END},
        {EVENT_TYPES_JSON_VALUE_WINDOW_UPDATE, EventType::TYPE_WINDOW_UPDATE},
        {EVENT_TYPES_JSON_VALUE_INTERRUPT, EventType::TYPE_INTERRUPT_EVENT},
        {EVENT_TYPES_JSON_VALUE_GESTURE_EVENT, EventType::TYPE_GESTURE_EVENT},
        {EVENT_TYPES_JSON_VALUE_ALL, EventType::TYPES_ALL_MASK}
    };

    for (auto& str : events) {
        std::map<std::string, uint32_t>::iterator itr = eventTypesTable.find(str);
        if (itr != eventTypesTable.end()) {
            eventTypes |= itr->second;
        }
    }
}

void PraseVecUtils::ParseAbilityTypesFromVec(const vector<string>& abilities, uint32_t& abilityTypes)
{
    HILOG_DEBUG("start.");
    abilityTypes = 0;

    for (auto it = abilities.begin(); it != abilities.end(); it++) {
        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_SPOKEN) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN;
        }

        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_HAPIC) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC;
        }

        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_AUDIBLE) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE;
        }

        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_VISUAL) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL;
        }

        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_GENERIC) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC;
        }

        if (*it == ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_ALL) {
            abilityTypes |= AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL;
        }
    }
}

void PraseVecUtils::ParseCapabilitiesFromVec(const vector<string>& capabilities, uint32_t& capabilitiesValue)
{
    HILOG_DEBUG("start.");
    capabilitiesValue = 0;

    for (auto it = capabilities.begin(); it != capabilities.end(); it++) {
        if (*it == CAPABILITIES_JSON_VALUE_RETRIEVE) {
            capabilitiesValue |= Capability::CAPABILITY_RETRIEVE;
        }

        if (*it == CAPABILITIES_JSON_VALUE_TOUCH_GUIDE) {
            capabilitiesValue |= Capability::CAPABILITY_TOUCH_GUIDE;
        }

        if (*it == CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER) {
            capabilitiesValue |= Capability::CAPABILITY_KEY_EVENT_OBSERVER;
        }

        if (*it == CAPABILITIES_JSON_VALUE_ZOOM) {
            capabilitiesValue |= Capability::CAPABILITY_ZOOM;
        }

        if (*it == CAPABILITIES_JSON_VALUE_GESTURE) {
            capabilitiesValue |= Capability::CAPABILITY_GESTURE;
        }
    }
}
} // namespace Accessibility
} // namespace OHOS