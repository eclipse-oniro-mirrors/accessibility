/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "accessibility_base_utils.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    // The json value of capabilities
    const std::string CAPABILITIES_JSON_VALUE_RETRIEVE = "retrieve";
    const std::string CAPABILITIES_JSON_VALUE_TOUCH_GUIDE = "touchGuide";
    const std::string CAPABILITIES_JSON_VALUE_KEY_EVENT_OBSERVER = "keyEventObserver";
    const std::string CAPABILITIES_JSON_VALUE_ZOOM = "zoom";
    const std::string CAPABILITIES_JSON_VALUE_GESTURE = "gesture";
}

AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(const std::string &type)
{
    std::map<const std::string, AccessibilityAbilityTypes> accessibilityAbilityTypesTable = {
        {"spoken", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN},
        {"haptic", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC},
        {"audible", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE},
        {"visual", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL},
        {"generic", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC},
        {"all", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL},
    };

    if (accessibilityAbilityTypesTable.find(type) == accessibilityAbilityTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_INVALID;
    }

    return accessibilityAbilityTypesTable.at(type);
}

AbilityStateType ConvertStringToAbilityStateType(const std::string &type)
{
    std::map<const std::string, AbilityStateType> abilityStateTypeTable = {
        {"enable", AbilityStateType::ABILITY_STATE_ENABLE},
        {"disable", AbilityStateType::ABILITY_STATE_DISABLE},
        {"install", AbilityStateType::ABILITY_STATE_INSTALLED}};

    if (abilityStateTypeTable.find(type) == abilityStateTypeTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return ABILITY_STATE_INVALID;
    }

    return abilityStateTypeTable.at(type);
}

bool CheckStateType(const std::string& stateType)
{
    if (std::strcmp(stateType.c_str(), "enable") == 0 ||
        std::strcmp(stateType.c_str(), "disable") == 0 ||
        std::strcmp(stateType.c_str(), "install") == 0) {
        return true;
    }
    return false;
}

std::vector<std::string> ParseAbilityTypesToVec(uint32_t abilityTypesValue)
{
    std::vector<std::string> result;

    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN) {
        result.push_back("spoken");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC) {
        result.push_back("haptic");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE) {
        result.push_back("audible");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL) {
        result.push_back("visual");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC) {
        result.push_back("generic");
    }

    return result;
}

std::vector<std::string> ParseCapabilitiesToVec(uint32_t capabilitiesValue)
{
    std::vector<std::string> result;

    if (capabilitiesValue & Capability::CAPABILITY_RETRIEVE) {
        result.push_back("retrieve");
    }
    if (capabilitiesValue & Capability::CAPABILITY_TOUCH_GUIDE) {
        result.push_back("touchGuide");
    }
    if (capabilitiesValue & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
        result.push_back("keyEventObserver");
    }
    if (capabilitiesValue & Capability::CAPABILITY_ZOOM) {
        result.push_back("zoom");
    }
    if (capabilitiesValue & Capability::CAPABILITY_GESTURE) {
        result.push_back("gesture");
    }

    return result;
}

std::vector<std::string> ParseEventTypesToVec(uint32_t eventTypesValue)
{
    std::vector<std::string> result;
    static std::map<EventType, std::string> accessibilityEventTable = {
        {EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY, "requestFocusForAccessibility"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY, "announceForAccessibility"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT, "announceForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT,
            "requestFocusForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_SCROLLING_EVENT, "scrolling"}};

    for (std::map<EventType, std::string>::iterator itr = accessibilityEventTable.begin();
         itr != accessibilityEventTable.end(); ++itr) {
        if (eventTypesValue & itr->first) {
            result.push_back(itr->second);
        }
    }

    return result;
}

bool CheckAbilityType(const std::string& abilityType)
{
    if (std::strcmp(abilityType.c_str(), "audible") == 0 ||
        std::strcmp(abilityType.c_str(), "generic") == 0 ||
        std::strcmp(abilityType.c_str(), "haptic") == 0 ||
        std::strcmp(abilityType.c_str(), "spoken") == 0 ||
        std::strcmp(abilityType.c_str(), "visual") == 0 ||
        std::strcmp(abilityType.c_str(), "all") == 0) {
        return true;
    } else {
        return false;
    }
}

uint32_t ParseCapabilitiesFromVec(const std::vector<std::string> &capabilities)
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

OHOS::AccessibilityConfig::DALTONIZATION_TYPE ConvertStringToDaltonizationTypes(std::string& type)
{
    std::map<const std::string, OHOS::AccessibilityConfig::DALTONIZATION_TYPE> daltonizationTTypesTable = {
        {"Normal", OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Normal},
        {"Protanomaly", OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Protanomaly},
        {"Deuteranomaly", OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Deuteranomaly},
        {"Tritanomaly", OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Tritanomaly},
    };

    if (daltonizationTTypesTable.find(type) == daltonizationTTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Normal;
    }

    return daltonizationTTypesTable.at(type);
}

std::string ConvertDaltonizationTypeToString(OHOS::AccessibilityConfig::DALTONIZATION_TYPE type)
{
    static const std::map<OHOS::AccessibilityConfig::DALTONIZATION_TYPE, const std::string> typeTable = {
        {OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Normal, "Normal"},
        {OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Protanomaly, "Protanomaly"},
        {OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Deuteranomaly, "Deuteranomaly"},
        {OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Tritanomaly, "Tritanomaly"}};

    if (typeTable.find(type) == typeTable.end()) {
        return "";
    }

    return typeTable.at(type);
}

OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME ConvertStringToClickResponseTimeTypes(std::string& type)
{
    std::map<const std::string, OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME> clickResponseTimeTypesTable = {
        {"Short", OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayShort},
        {"Medium", OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayMedium},
        {"Long", OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayLong},
    };

    if (clickResponseTimeTypesTable.find(type) == clickResponseTimeTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayShort;
    }

    return clickResponseTimeTypesTable.at(type);
}

std::string ConvertClickResponseTimeTypeToString(OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME type)
{
    static const std::map<OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME, const std::string> typeTable = {
        {OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayShort, "Short"},
        {OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayMedium, "Medium"},
        {OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayLong, "Long"}};

    if (typeTable.find(type) == typeTable.end()) {
        return "";
    }

    return typeTable.at(type);
}

OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME ConvertStringToIgnoreRepeatClickTimeTypes(std::string& type)
{
    std::map<const std::string, OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME> mapTable = {
        {"Shortest", OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShortest},
        {"Short", OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShort},
        {"Medium", OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutMedium},
        {"Long", OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutLong},
        {"Longest", OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutLongest},
    };

    if (mapTable.find(type) == mapTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShortest;
    }

    return mapTable.at(type);
}

std::string ConvertIgnoreRepeatClickTimeTypeToString(OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME type)
{
    static const std::map<OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME, const std::string> typeTable = {
        {OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShortest, "Shortest"},
        {OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShort, "Short"},
        {OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutMedium, "Medium"},
        {OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutLong, "Long"},
        {OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutLongest, "Longest"}};

    if (typeTable.find(type) == typeTable.end()) {
        return "";
    }

    return typeTable.at(type);
}

} // namespace Accessibility
} // namespace OHOS
