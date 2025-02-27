/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CJ_ACCESSIBILITY_UTILS_H
#define CJ_ACCESSIBILITY_UTILS_H

#include <cstdint>
#include "cj_accessibility_ffi.h"
#include "hilog_wrapper.h"
#include "accessibility_def.h"
#include "accessibility_event_info.h"
#include "accessibility_ability_info.h"
#include "native/ffi_remote_data.h"

namespace OHOS {
namespace Accessibility {
namespace Utils {
template <typename K, typename V> std::unordered_map<V, K> ReverseMap(const std::unordered_map<K, V> &map)
{
    std::unordered_map<V, K> rMap;
    for (auto pair : map) {
        rMap[pair.second] = pair.first;
    }
    return rMap;
}

template <typename T>
T GetValueFromStr(const std::unordered_map<std::string, T> &rMap, const std::string &key, RetError &errCode,
    const T &defaultVal)
{
    auto it = rMap.find(key);
    if (it != rMap.end()) {
        return it->second;
    } else {
        HILOG_DEBUG("GetValueFromStr failed(%{public}s).", it->first.c_str());
        return defaultVal;
    }
}

template <typename T>
std::string GetStrFromVal(const std::unordered_map<T, std::string> &map, const T &key, RetError &errCode,
    const std::string &defaultVal = "")
{
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        return defaultVal;
    }
}

static std::unordered_map<Capability, std::string> capabilityMap = { { Capability::CAPABILITY_RETRIEVE, "retrieve" },
    { Capability::CAPABILITY_TOUCH_GUIDE, "touchGuide" },
    { Capability::CAPABILITY_KEY_EVENT_OBSERVER, "keyEventObserver" },
    { Capability::CAPABILITY_ZOOM, "zoom" },
    { Capability::CAPABILITY_GESTURE, "gesture" } };
static auto rCapabilityMap = ReverseMap(capabilityMap);

static std::unordered_map<AccessibilityAbilityTypes, std::string> abilityTypeMap = {
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, "spoken" },
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC, "haptic" },
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE, "audible" },
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL, "visual" },
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC, "generic" },
    { AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL, "all" }
};
static auto rAbilityTypeMap = ReverseMap(abilityTypeMap);

static std::unordered_map<EventType, std::string> eventTypeMap = { { EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT,
    "accessibilityFocus" },
    { EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear" },
    { EventType::TYPE_VIEW_CLICKED_EVENT, "click" },
    { EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick" },
    { EventType::TYPE_VIEW_SELECTED_EVENT, "select" },
    { EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter" },
    { EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit" },
    { EventType::TYPE_VIEW_FOCUSED_EVENT, "focus" },
    { EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate" },
    { EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate" },
    { EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll" },
    { EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY, "requestFocusForAccessibility" },
    { EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY, "announceForAccessibility" },
    { EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT, "requestFocusForAccessibilityNotInterrupt" },
    { EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT, "announceForAccessibilityNotInterrupt" },
    { EventType::TYPE_VIEW_SCROLLING_EVENT, "scrolling" } };
static auto rEventTypeMap = ReverseMap(eventTypeMap);

static std::unordered_map<ActionType, std::string> actionMap = {
    { ActionType::ACCESSIBILITY_ACTION_CLICK, "click" },
    { ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, "longClick" },
    { ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, "scrollForward" },
    { ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, "scrollBackward" },
    { ActionType::ACCESSIBILITY_ACTION_FOCUS, "focus" },
    { ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS, "clearFocus" },
    { ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION, "clearSelection" },
    { ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, "accessibilityFocus" },
    { ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, "clearAccessibilityFocus" },
    { ActionType::ACCESSIBILITY_ACTION_CUT, "cut" },
    { ActionType::ACCESSIBILITY_ACTION_COPY, "copy" },
    { ActionType::ACCESSIBILITY_ACTION_PASTE, "paste" },
    { ActionType::ACCESSIBILITY_ACTION_SELECT, "select" },
    { ActionType::ACCESSIBILITY_ACTION_SET_TEXT, "setText" },
    { ActionType::ACCESSIBILITY_ACTION_DELETED, "delete" },
    { ActionType::ACCESSIBILITY_ACTION_SET_SELECTION, "setSelection" },
    { ActionType::ACCESSIBILITY_ACTION_COMMON, "common" },
    { ActionType::ACCESSIBILITY_ACTION_HOME, "home" },
    { ActionType::ACCESSIBILITY_ACTION_BACK, "back" },
    { ActionType::ACCESSIBILITY_ACTION_RECENTTASK, "recentTask" },
    { ActionType::ACCESSIBILITY_ACTION_NOTIFICATIONCENTER, "notificationCenter" },
    { ActionType::ACCESSIBILITY_ACTION_CONTROLCENTER, "controlCenter" },
    { ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION, "setCursorPosition" },
};
static auto rActionMap = ReverseMap(actionMap);

static std::unordered_map<WindowUpdateType, std::string> windowUpdateTypeMap = {
    { WindowUpdateType::WINDOW_UPDATE_FOCUSED, "focus" },
    { WindowUpdateType::WINDOW_UPDATE_ACTIVE, "active" },
    { WindowUpdateType::WINDOW_UPDATE_ADDED, "add" },
    { WindowUpdateType::WINDOW_UPDATE_REMOVED, "remove" },
    { WindowUpdateType::WINDOW_UPDATE_BOUNDS, "bounds" }
};
static auto rWindowUpdateTypeMap = ReverseMap(windowUpdateTypeMap);

static std::unordered_map<TextMoveUnit, std::string> textMoveUnitMap = { { TextMoveUnit::STEP_CHARACTER, "char" },
    { TextMoveUnit::STEP_WORD, "word" },
    { TextMoveUnit::STEP_LINE, "line" },
    { TextMoveUnit::STEP_PAGE, "page" },
    { TextMoveUnit::STEP_PARAGRAPH, "paragraph" } };
static auto rTextMoveUnitMap = ReverseMap(textMoveUnitMap);

// base
char *MallocCString(const std::string &origin, RetError &errCode);
CArrString VectorToCArrString(std::vector<std::string> &vec, RetError &errCode);
std::vector<std::string> CArrStringToVector(CArrString cArrStr);

// enum to string
CArrString GetAbilityTypesStr(uint32_t abilityTypes, RetError &errCode);
CArrString GetCapabilityStr(uint32_t capabilities, RetError &errCode);
CArrString GetEventTypeStr(uint32_t eventType, RetError &errCode);

bool CheckAbilityType(const std::string &abilityType);
bool CheckStateType(const std::string &stateType);

// func
CAccessibilityAbilityInfo ConvertAccAbilityInfo2C(AccessibilityAbilityInfo &abilityInfo, RetError &errCode);
CArrAccessibilityAbilityInfo ConvertArrAccAbilityInfo2CArr(std::vector<AccessibilityAbilityInfo> &abilityList,
    RetError &errCode);
CEventInfo ConvertEventInfo2C(const AccessibilityEventInfo &eventInfo, RetError &errCode);
AccessibilityEventInfo ConvertEventInfo(const CEventInfo &cEventInfo, RetError &errCode);
} // namespace Utils
} // namespace Accessibility
} // namespace OHOS
#endif // CJ_ACCESSIBILITY_UTILS_H