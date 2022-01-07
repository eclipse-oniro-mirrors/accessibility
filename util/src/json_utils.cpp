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

#include <fstream>
#include <sstream>
#include "json_utils.h"
#include "hilog_wrapper.h"
#include "accessibility_event_info.h"
#include "accessibility_ability_info.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

bool JsonUtils::GetJsonObjFromJson(nlohmann::json &jsonObj, const std::string &jsonPath)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::ifstream jsonFileStream;
    jsonFileStream.open(jsonPath.c_str(), std::ios::in);
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
    HILOG_DEBUG("%{public}s start.", __func__);
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

bool JsonUtils::GetIntFromJson(const nlohmann::json &json, const std::string &key, int &value)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!json.is_object()) {
        HILOG_ERROR("json is not object.");
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_number()) {
        HILOG_DEBUG("Find key[%{public}s] successful.", key.c_str());
        value = json.at(key).get<int>();
    }
    return true;
}

bool JsonUtils::GetStringVecFromJson(const nlohmann::json &json, const std::string &key,
                                     std::vector<std::string> &value)
{
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
    eventTypes = EventType::TYPE_VIEW_INVALID;

    for (auto it = events.begin(); it != events.end(); it++) {
        if (*it == EVENT_TYPES_JSON_VALUE_CLICK) {
            eventTypes |= EventType::TYPE_VIEW_CLICKED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_LONG_CLICK) {
            eventTypes |= EventType::TYPE_VIEW_LONG_CLICKED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_SELECT) {
            eventTypes |= EventType::TYPE_VIEW_SELECTED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_FOCUS) {
            eventTypes |= EventType::TYPE_VIEW_FOCUSED_EVENT;
        }
        
        if (*it == EVENT_TYPES_JSON_VALUE_TEXT_UPDATE) {
            eventTypes |= EventType::TYPE_VIEW_TEXT_UPDATE_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_PAGE_STATE_UPDATE) {
            eventTypes |= EventType::TYPE_PAGE_STATE_UPDATE;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_NOTIFICATION_UPDATE) {
            eventTypes |= EventType::TYPE_NOTIFICATION_UPDATE_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_HOVER_ENTER) {
            eventTypes |= EventType::TYPE_VIEW_HOVER_ENTER_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_HOVER_EXIT) {
            eventTypes |= EventType::TYPE_VIEW_HOVER_EXIT_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_BEGIN) {
            eventTypes |= EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_END) {
            eventTypes |= EventType::TYPE_TOUCH_GUIDE_GESTURE_END;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_PAGE_CONTENT_UPDATE) {
            eventTypes |= EventType::TYPE_PAGE_CONTENT_UPDATE;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_SCROLL) {
            eventTypes |= EventType::TYPE_VIEW_SCROLLED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TEXT_SELECTION_UPDATE) {
            eventTypes |= EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_PUBLIC_NOTICE) {
            eventTypes |= EventType::TYPE_PUBLIC_NOTICE_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS) {
            eventTypes |= EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS_CLEAR) {
            eventTypes |= EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TEXT_MOVE_UNIT) {
            eventTypes |= EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_BEGIN) {
            eventTypes |= EventType::TYPE_TOUCH_GUIDE_BEGIN;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_END) {
            eventTypes |= EventType::TYPE_TOUCH_GUIDE_END;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_BEGIN) {
            eventTypes |= EventType::TYPE_TOUCH_BEGIN;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_TOUCH_END) {
            eventTypes |= EventType::TYPE_TOUCH_END;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_WINDOW_UPDATE) {
            eventTypes |= EventType::TYPE_WINDOW_UPDATE;
        }

        if (*it == EVENT_TYPES_JSON_VALUE_ALL) {
            eventTypes |= EventType::TYPES_ALL_MASK;
        }
    }
}

void PraseVecUtils::ParseAbilityTypesFromVec(const vector<string>& abilities, uint32_t& abilityTypes)
{
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
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