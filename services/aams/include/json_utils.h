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

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <vector>
#include "accessibility_ability_info.h"
#include "extension_ability_info.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Accessibility {
extern const std::string AccessibleAbility_JSON_FILE_PATH;
extern const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_EVENT_TYPES;
extern const std::string AccessibleAbility_JSON_KEY_TARGET_BUNDLE_NAMES;
extern const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_ABILITY_TYPES;
extern const std::string AccessibleAbility_JSON_KEY_NOTIFICATION_TIMEOUT;
extern const std::string AccessibleAbility_JSON_KEY_UI_NONINTERACTIVE_TIMEOUT;
extern const std::string AccessibleAbility_JSON_KEY_UI_INTERACTIVE_TIMEOUT;
extern const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES;
extern const std::string AccessibleAbility_JSON_KEY_DESCRIPTION;
extern const std::string AccessibleAbility_JSON_KEY_SETTINGS_ABILITY;
extern const std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE;
extern const std::string AccessibleAbility_Config_JSON_FILE_PATH;

// The json value of event types
extern const std::string EVENT_TYPES_JSON_VALUE_CLICK;
extern const std::string EVENT_TYPES_JSON_VALUE_LONG_CLICK;
extern const std::string EVENT_TYPES_JSON_VALUE_SELECT;
extern const std::string EVENT_TYPES_JSON_VALUE_FOCUS;
extern const std::string EVENT_TYPES_JSON_VALUE_TEXT_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_PAGE_STATE_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_NOTIFICATION_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_HOVER_ENTER;
extern const std::string EVENT_TYPES_JSON_VALUE_HOVER_EXIT;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_BEGIN;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_END;
extern const std::string EVENT_TYPES_JSON_VALUE_PAGE_CONTENT_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_SCROLL;
extern const std::string EVENT_TYPES_JSON_VALUE_TEXT_SELECTION_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_PUBLIC_NOTICE;
extern const std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS;
extern const std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS_CLEAR;
extern const std::string EVENT_TYPES_JSON_VALUE_TEXT_MOVE_UNIT;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_BEGIN;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_END;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_BEGIN;
extern const std::string EVENT_TYPES_JSON_VALUE_TOUCH_END;
extern const std::string EVENT_TYPES_JSON_VALUE_WINDOW_UPDATE;
extern const std::string EVENT_TYPES_JSON_VALUE_INTERRUPT;
extern const std::string EVENT_TYPES_JSON_VALUE_GESTURE_EVENT;
extern const std::string EVENT_TYPES_JSON_VALUE_ALL;

// The json value of accessibilityAbility type
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_SPOKEN;
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_HAPIC;
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_AUDIBLE;
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_VISUAL;
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_GENERIC;
extern const std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_ALL;

// The json value of capabilities
extern const std::string CAPABILITIES_JSON_VALUE_RETRIEVE;
extern const std::string CAPABILITIES_JSON_VALUE_TOUCH_GUIDE;
extern const std::string CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER;
extern const std::string CAPABILITIES_JSON_VALUE_ZOOM;
extern const std::string CAPABILITIES_JSON_VALUE_GESTURE;

class AccessibilityConfigParse {
public:
    static void Parse(const AppExecFwk::ExtensionAbilityInfo &abilityInfo, AccessibilityAbilityInitParams &initParams);
};

class JsonUtils {
public:
    static bool GetJsonObjFromJson(nlohmann::json& jsonObj, const std::string& jsonPath);
    static bool GetStringFromJson(const nlohmann::json& json, const std::string& key, std::string& value);
    static bool GetInt32FromJson(const nlohmann::json& json, const std::string& key, int32_t& value);
    static bool GetStringVecFromJson(
        const nlohmann::json& json, const std::string& key, std::vector<std::string>& value);
    static bool ParseObjVecFromJson(
        const nlohmann::json& json, const std::string& key, std::vector<nlohmann::json>& value);
};

class PraseVecUtils {
public:
    static void ParseEventTypesFromVec(const std::vector<std::string>& events, uint32_t& eventTypes);
    static void ParseAbilityTypesFromVec(const std::vector<std::string>& abilities, uint32_t& abilityTypes);
    static void ParseCapabilitiesFromVec(const std::vector<std::string>& capabilities, uint32_t& capabilitiesValue);
};
} // namespace Accessibility
} // namespace OHOS
#endif  // JSON_UTILS_H