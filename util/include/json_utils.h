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

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Accessibility {
const static std::string AccessibleAbility_JSON_FILE_PATH = "/system/app/dummy_accessibility_ability_config.json";
const static std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_EVENT_TYPES = "accessibilityEventTypes";
const static std::string AccessibleAbility_JSON_KEY_TARGET_BUNDLE_NAMES = "targetBundleNames";
const static std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_ABILITY_TYPES = "accessibilityAbilityTypes";
const static std::string AccessibleAbility_JSON_KEY_NOTIFICATION_TIMEOUT = "notificationTimeout";
const static std::string AccessibleAbility_JSON_KEY_UI_NONINTERACTIVE_TIMEOUT = "uiNoninteractiveTimeout";
const static std::string AccessibleAbility_JSON_KEY_UI_INTERACTIVE_TIMEOUT = "uiInteractiveTimeout";
const static std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES = "accessibilityCapabilities";
const static std::string AccessibleAbility_JSON_KEY_DESCRIPTION = "description";
const static std::string AccessibleAbility_JSON_KEY_SETTINGS_ABILITY = "settingsAbility";
const static std::string AccessibleAbility_JSON_KEY_ACCESSIBILITY_CAPABILITIES_RATIONALE =
    "accessibilityCapabilityRationale";
const static std::string AccessibleAbility_Config_JSON_FILE_PATH = "/system/app/accessibility_config.json";

// The json value of event types
const static std::string EVENT_TYPES_JSON_VALUE_CLICK = "click";
const static std::string EVENT_TYPES_JSON_VALUE_LONG_CLICK = "longClick";
const static std::string EVENT_TYPES_JSON_VALUE_SELECT = "select";
const static std::string EVENT_TYPES_JSON_VALUE_FOCUS = "focus";
const static std::string EVENT_TYPES_JSON_VALUE_TEXT_UPDATE = "textUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_PAGE_STATE_UPDATE = "pageStateUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_NOTIFICATION_UPDATE = "notificationUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_HOVER_ENTER = "hoverEnter";
const static std::string EVENT_TYPES_JSON_VALUE_HOVER_EXIT = "hoverExit";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_BEGIN = "touchGuideBegin";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_END = "touchGuideEnd";
const static std::string EVENT_TYPES_JSON_VALUE_PAGE_CONTENT_UPDATE = "pageContentUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_SCROLL = "scroll";
const static std::string EVENT_TYPES_JSON_VALUE_TEXT_SELECTION_UPDATE = "textSelectionUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_PUBLIC_NOTICE = "publicNotice";
const static std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS = "accessibilityFocus";
const static std::string EVENT_TYPES_JSON_VALUE_ACCESSIBILITY_FOCUS_CLEAR = "accessibilityFocusClear";
const static std::string EVENT_TYPES_JSON_VALUE_TEXT_MOVE_UNIT = "textMoveUnit";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_BEGIN = "touchGuideGestureBegin";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_GUIDE_GESTURE_END = "touchGuideGestureEnd";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_BEGIN = "touchBegin";
const static std::string EVENT_TYPES_JSON_VALUE_TOUCH_END = "touchEnd";
const static std::string EVENT_TYPES_JSON_VALUE_WINDOW_UPDATE = "windowUpdate";
const static std::string EVENT_TYPES_JSON_VALUE_INTERRUPT = "interrupt";
const static std::string EVENT_TYPES_JSON_VALUE_GESTURE_EVENT = "gesture";
const static std::string EVENT_TYPES_JSON_VALUE_ALL = "all";

// The json value of accessibilityAbility type
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_SPOKEN = "spoken";
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_HAPIC = "haptic";
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_AUDIBLE = "audible";
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_VISUAL = "visual";
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_GENERIC = "generic";
const static std::string ACCESSIBILITY_ABILITY_TYPES_JSON_VALUE_ALL = "all";

// The json value of capabilities
const static std::string CAPABILITIES_JSON_VALUE_RETRIEVE = "retrieve";
const static std::string CAPABILITIES_JSON_VALUE_TOUCH_GUIDE = "touchGuide";
const static std::string CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER = "keyEventObserver";
const static std::string CAPABILITIES_JSON_VALUE_ZOOM = "zoom";
const static std::string CAPABILITIES_JSON_VALUE_GESTURE = "gesture";

// The json object
const static std::string JSON_OBJECT_CAPABILITY = "Capability";
const static std::string JSON_OBJECT_ENABLED = "Enabled";
const static std::string JSON_OBJECT_CAPTION_STATE = "CaptionState";
const static std::string JSON_OBJECT_CAPTION_STYLE = "CaptionsStyle";

// The json value of Capability
const static std::string CAPABILITY_JSON_VALUE_ACCESSIBLE = "accessible";
const static std::string CAPABILITY_JSON_VALUE_TOUCH_GUIDE = "touchGuide";
const static std::string CAPABILITY_JSON_VALUE_GESTURE = "gesture";
const static std::string CAPABILITY_JSON_VALUE_KEY_EVENT_BOSERVER = "keyEventObserver";

// The json value of Caption
const static std::string CAPTION_JSON_VALUE_FONTFAMILY = "fontFamily";
const static std::string CAPTION_JSON_VALUE_FONTSCALE = "fontScale";
const static std::string CAPTION_JSON_VALUE_FONTCOLOR = "fontColor";
const static std::string CAPTION_JSON_VALUE_FONTEDGETYPE = "fontEdgeType";
const static std::string CAPTION_JSON_VALUE_BACKGROUNDCOLOR = "backgroundColor";
const static std::string CAPTION_JSON_VALUE_WINDOWCOLOR = "windowColor";

class JsonUtils {
public:
    static bool GetJsonObjFromJson(nlohmann::json& jsonObj, const std::string& jsonPath);
    static bool GetStringFromJson(const nlohmann::json& json, const std::string& key, std::string& value);
    static bool GetIntFromJson(const nlohmann::json& json, const std::string& key, int& value);
    static bool GetStringVecFromJson(
        const nlohmann::json& json, const std::string& key, std::vector<std::string>& value);
    static bool ParseObjVecFromJson(
        const nlohmann::json& json, const std::string& key, std::vector<nlohmann::json>& value);
    static bool SetStrValue(
        nlohmann::json& json, const std::string& key, const std::string& subkey, const std::string& value);
    static bool SetStrValue( nlohmann::json& json, const std::string& key, const std::string& value);
    static bool SetIntValue(nlohmann::json& json, const std::string& key, const std::string& subkey, int& value);
    static bool SetVecValue(nlohmann::json& json, const std::string& key, std::string& value);
    static bool ClearVecValue(nlohmann::json& json, const std::string& key);
    static bool RemoveVecValue(nlohmann::json& json, const std::string& key, const std::string& value);
    static bool CheckEnabledValue(nlohmann::json& json, const std::string& Enakey, const std::string& Inskey);
    static bool ToJson(const nlohmann::json& jsonObj, const std::string& jsonPath);
    static int GetIntValue(nlohmann::json& json, const std::string& key, const std::string& subkey);
    static std::string GetStrValue(nlohmann::json& json, const std::string& key, const std::string& subkey);
    static std::string GetStrValue(nlohmann::json& json, const std::string& key);
    static std::vector<std::string> GetVecValue(nlohmann::json& json, const std::string& key);
};

class PraseVecUtils {
public:
    static void ParseEventTypesFromVec(const std::vector<std::string>& events, uint32_t& eventTypes);
    static void ParseAbilityTypesFromVec(const std::vector<std::string>& abilities, uint32_t& abilityTypes);
    static void ParseCapabilitiesFromVec(const std::vector<std::string>& capabilities, uint32_t& capabilitiesValue);
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // JSON_UTILS_H