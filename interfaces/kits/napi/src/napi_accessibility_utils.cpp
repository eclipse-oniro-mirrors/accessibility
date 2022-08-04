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

#include "napi_accessibility_utils.h"

#include <cmath>
#include <iomanip>
#include <regex>
#include <sstream>
#include <vector>

#include "accessible_ability_client.h"
#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessibility_event_info.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityConfig;

std::string GetStringFromNAPI(napi_env env, napi_value value)
{
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
        HILOG_ERROR("can not get string size");
        return "";
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
        HILOG_ERROR("can not get string value");
        return "";
    }
    return result;
}

bool ParseBool(napi_env env, bool& param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_boolean) {
        HILOG_ERROR("Wrong argument type. Boolean expected.");
        return false;
    }

    napi_get_value_bool(env, args, &param);
    return true;
}

bool ParseString(napi_env env, std::string& param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_string) {
        HILOG_ERROR("Wrong argument type. String expected.");
        return false;
    }

    param = GetStringFromNAPI(env, args);
    HILOG_INFO("param=%{public}s.", param.c_str());
    return true;
}

bool ParseNumber(napi_env env, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_number) {
        HILOG_ERROR("Wrong argument type. uint32 expected.");
        return false;
    }
    
    HILOG_DEBUG("The type of args is number.");
    return true;
}

bool ParseUint32(napi_env env, uint32_t& param, napi_value args)
{
    if (!ParseNumber(env, args)) {
        return false;
    }

    napi_get_value_uint32(env, args, &param);
    return true;
}

bool ParseInt32(napi_env env, int32_t& param, napi_value args)
{
    if (!ParseNumber(env, args)) {
        return false;
    }

    napi_get_value_int32(env, args, &param);
    return true;
}

bool ParseDouble(napi_env env, double& param, napi_value args)
{
    if (!ParseNumber(env, args)) {
        return false;
    }

    napi_get_value_double(env, args, &param);
    return true;
}

napi_value GetErrorValue(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

/**********************************************************
 * Convert native object to js object
 *********************************************************/
void ConvertRectToJS(napi_env env, napi_value result, const Accessibility::Rect& rect)
{
    napi_value nLeftTopX;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopXScreenPostion(), &nLeftTopX));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "left", nLeftTopX));

    napi_value nLeftTopY;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopYScreenPostion(), &nLeftTopY));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "top", nLeftTopY));

    napi_value nWidth;
    int32_t width = rect.GetRightBottomXScreenPostion() - rect.GetLeftTopXScreenPostion();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, width, &nWidth));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "width", nWidth));

    napi_value nHeight;
    int32_t height = rect.GetRightBottomYScreenPostion() - rect.GetLeftTopYScreenPostion();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, height, &nHeight));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "height", nHeight));
}

std::string ConvertWindowTypeToString(AccessibilityWindowType type)
{
    static const std::map<AccessibilityWindowType, const std::string> windowTypeTable = {
        {AccessibilityWindowType::TYPE_ACCESSIBILITY_OVERLAY, "accessibilityOverlay"},
        {AccessibilityWindowType::TYPE_APPLICATION, "application"},
        {AccessibilityWindowType::TYPE_INPUT_METHOD, "inputMethod"},
        {AccessibilityWindowType::TYPE_SPLIT_SCREEN_DIVIDER, "screenDivider"},
        {AccessibilityWindowType::TYPE_SYSTEM, "system"}};

    if (windowTypeTable.find(type) == windowTypeTable.end()) {
        return "";
    }

    return windowTypeTable.at(type);
}

static std::vector<std::string> ParseEventTypesToVec(uint32_t eventTypesValue)
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
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"}};

    for (std::map<EventType, std::string>::iterator itr = accessibilityEventTable.begin();
         itr != accessibilityEventTable.end(); ++itr) {
        if (eventTypesValue & itr->first) {
            result.push_back(itr->second);
        }
    }

    return result;
}

static std::vector<std::string> ParseAbilityTypesToVec(uint32_t abilityTypesValue)
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

static std::vector<std::string> ParseCapabilitiesToVec(uint32_t capabilitiesValue)
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

static void ConvertAccessibleAbilityInfoToJS(napi_env env, napi_value& result, AccessibilityAbilityInfo& info)
{
    napi_value nId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetId().c_str(), NAPI_AUTO_LENGTH, &nId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "id", nId));

    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetName().c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", nName));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, info.GetPackageName().c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", nBundleName));

    napi_value nAbilityType;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityType));
    uint32_t abilityTypesValue = info.GetAccessibilityAbilityType();
    std::vector<std::string> abilityTypes = ParseAbilityTypesToVec(abilityTypesValue);
    for (size_t idxType = 0; idxType < abilityTypes.size(); idxType++) {
        napi_value nType;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityTypes[idxType].c_str(),
            NAPI_AUTO_LENGTH, &nType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityType, idxType, nType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "abilityTypes", nAbilityType));

    napi_value nCapabilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nCapabilities));
    uint32_t capabilitiesValue = info.GetCapabilityValues();
    std::vector<std::string> capabilities = ParseCapabilitiesToVec(capabilitiesValue);
    for (size_t idxCap = 0; idxCap < capabilities.size(); idxCap++) {
        napi_value nCap;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, capabilities[idxCap].c_str(),
            NAPI_AUTO_LENGTH, &nCap));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nCapabilities, idxCap, nCap));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "capabilities", nCapabilities));

    napi_value description;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, info.GetDescription().c_str(), NAPI_AUTO_LENGTH, &description));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", description));

    napi_value nEventTypes;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nEventTypes));
    uint32_t eventTypesValue = info.GetEventTypes();
    std::vector<std::string> eventTypes = ParseEventTypesToVec(eventTypesValue);
    for (size_t idxEve = 0; idxEve < eventTypes.size(); idxEve++) {
        napi_value nEve;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, eventTypes[idxEve].c_str(), NAPI_AUTO_LENGTH, &nEve));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nEventTypes, idxEve, nEve));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "eventTypes", nEventTypes));

    napi_value filterBundleNames;
    size_t idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &filterBundleNames));
    std::vector<std::string> strFilterBundleNames = info.GetFilterBundleNames();
    for (auto &filterBundleName : strFilterBundleNames) {
        napi_value bundleName;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, filterBundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, filterBundleNames, idx, bundleName));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "targetBundleNames", filterBundleNames));
}

void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value& result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos)
{
    size_t index = 0;

    if (accessibleAbilityInfos.empty()) {
        return;
    }

    for (auto& abilityInfo : accessibleAbilityInfos) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertAccessibleAbilityInfoToJS(env, obj, abilityInfo);
        napi_set_element(env, result, index, obj);
        index++;
    }
}

const std::string ConvertAccessibilityEventTypeToString(EventType type)
{
    static const std::map<EventType, const std::string> a11yEvtTypeTable = {
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_PAGE_CONTENT_UPDATE, "pageContentUpdate"},
        {EventType::TYPE_PAGE_STATE_UPDATE, "pageStateUpdate"},
        {EventType::TYPE_TOUCH_BEGIN, "touchBegin"},
        {EventType::TYPE_TOUCH_END, "touchEnd"}};

    if (a11yEvtTypeTable.find(type) == a11yEvtTypeTable.end()) {
        return "";
    }

    return a11yEvtTypeTable.at(type);
}

std::string CoverGestureTypeToString(GestureType type)
{
    static const std::map<GestureType, const std::string> gestureTypeTable = {
        {GestureType::GESTURE_SWIPE_LEFT, "left"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT, "leftThenRight"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_UP, "leftThenUp"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN, "leftThenDown"},
        {GestureType::GESTURE_SWIPE_RIGHT, "right"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT, "rightThenLeft"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_UP, "rightThenUp"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN, "rightThenDown"},
        {GestureType::GESTURE_SWIPE_UP, "up"},
        {GestureType::GESTURE_SWIPE_UP_THEN_LEFT, "upThenLeft"},
        {GestureType::GESTURE_SWIPE_UP_THEN_RIGHT, "upThenRight"},
        {GestureType::GESTURE_SWIPE_UP_THEN_DOWN, "upThenDown"},
        {GestureType::GESTURE_SWIPE_DOWN, "down"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT, "downThenLeft"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT, "downThenRight"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_UP, "downThenUp"}
    };

    if (gestureTypeTable.find(type) == gestureTypeTable.end()) {
        return "";
    }

    return gestureTypeTable.at(type);
}

const std::string ConvertWindowUpdateTypeToString(WindowUpdateType type)
{
    static const std::map<WindowUpdateType, const std::string> windowUpdateTypeTable = {
        {WindowUpdateType::WINDOW_UPDATE_FOCUSED, "focus"},
        {WindowUpdateType::WINDOW_UPDATE_ACTIVE, "active"},
        {WindowUpdateType::WINDOW_UPDATE_ADDED, "add"},
        {WindowUpdateType::WINDOW_UPDATE_REMOVED, "remove"},
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, "bounds"}};

    if (windowUpdateTypeTable.find(type) == windowUpdateTypeTable.end()) {
        return "";
    }

    return windowUpdateTypeTable.at(type);
}

void ConvertEventTypeToString(const AccessibilityEventInfo &eventInfo, std::string &eventTypeString)
{
    EventType type = eventInfo.GetEventType();
    switch (type) {
        case TYPE_GESTURE_EVENT: {
            GestureType gestureType = eventInfo.GetGestureType();
            eventTypeString = CoverGestureTypeToString(gestureType);
            break;
        }
        case TYPE_WINDOW_UPDATE: {
            WindowUpdateType windowUpdateType = eventInfo.GetWindowChangeTypes();
            eventTypeString = ConvertWindowUpdateTypeToString(windowUpdateType);
            break;
        }
        default:
            eventTypeString = ConvertAccessibilityEventTypeToString(type);
            break;
    }
}

const std::string ConvertCategoryNotificationToString(NotificationCategory category)
{
    static const std::map<NotificationCategory, const std::string> categoryTable = {
        {NotificationCategory::CATEGORY_CALL, "call"},
        {NotificationCategory::CATEGORY_MSG, "msg"},
        {NotificationCategory::CATEGORY_EMAIL, "email"},
        {NotificationCategory::CATEGORY_EVENT, "event"},
        {NotificationCategory::CATEGORY_PROMO, "promo"},
        {NotificationCategory::CATEGORY_ALARM, "alarm"},
        {NotificationCategory::CATEGORY_PROGRESS, "progress"},
        {NotificationCategory::CATEGORY_SOCIAL, "social"},
        {NotificationCategory::CATEGORY_ERR, "err"},
        {NotificationCategory::CATEGORY_TRANSPORT, "transport"},
        {NotificationCategory::CATEGORY_SYS, "sys"},
        {NotificationCategory::CATEGORY_SERVICE, "service"},
        {NotificationCategory::CATEGORY_OTHERS, ""},
    };

    if (categoryTable.find(category) == categoryTable.end()) {
        return "";
    }

    return categoryTable.at(category);
}

std::string ConvertOperationTypeToString(ActionType type)
{
    static const std::map<ActionType, const std::string> triggerActionTable = {
        {ActionType::ACCESSIBILITY_ACTION_FOCUS, "focus"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS, "clearFocus"},
        {ActionType::ACCESSIBILITY_ACTION_SELECT, "select"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION, "clearSelection"},
        {ActionType::ACCESSIBILITY_ACTION_CLICK, "click"},
        {ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, "longClick"},
        {ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, "accessibilityFocus"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, "clearAccessibilityFocus"},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, "scrollForward"},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, "scrollBackward"},
        {ActionType::ACCESSIBILITY_ACTION_COPY, "copy"},
        {ActionType::ACCESSIBILITY_ACTION_PASTE, "paste"},
        {ActionType::ACCESSIBILITY_ACTION_CUT, "cut"},
        {ActionType::ACCESSIBILITY_ACTION_SET_SELECTION, "setSelection"},
        {ActionType::ACCESSIBILITY_ACTION_SET_TEXT, "setText"},
        {ActionType::ACCESSIBILITY_ACTION_DELETED, "delete"},
    };

    if (triggerActionTable.find(type) == triggerActionTable.end()) {
        return "";
    }

    return triggerActionTable.at(type);
}

void ConvertOperationToJS(napi_env env, napi_value result, const AccessibleAction& operation)
{
    napi_value nType;
    ActionType type = operation.GetActionType();
    std::string strType = ConvertOperationTypeToString(type);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strType.c_str(), NAPI_AUTO_LENGTH, &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "type", nType));
    HILOG_DEBUG("operationType[%{public}s]", strType.c_str());

    napi_value nDescription;
    std::string strDescription = operation.GetDescriptionInfo();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strDescription.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", nDescription));
}

void ConvertGridInfoToJS(napi_env env, napi_value nGrid, const OHOS::Accessibility::GridInfo& grid)
{
    napi_value nRowCount;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetRowCount(), &nRowCount));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "rowCount", nRowCount));

    napi_value nColumnCount;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetColumnCount(), &nColumnCount));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "columnCount", nColumnCount));

    napi_value nSelectionMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetSelectionMode(), &nSelectionMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "selectionMode", nSelectionMode));
}

void ConvertGridItemToJS(napi_env env, napi_value nGridItem, const GridItemInfo& gridItem)
{
    napi_value nHeading;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, gridItem.IsHeading(), &nHeading));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "heading", nHeading));

    napi_value nColumnIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetColumnIndex(), &nColumnIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "columnIndex", nColumnIndex));

    napi_value nRowIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetRowIndex(), &nRowIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "rowIndex", nRowIndex));

    napi_value nColumnSpan;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetColumnSpan(), &nColumnSpan));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "columnSpan", nColumnSpan));

    napi_value nRowSpan;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetRowSpan(), &nRowSpan));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "rowSpan", nRowSpan));

    napi_value nSelected;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, gridItem.IsSelected(), &nSelected));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "selected", nSelected));
}

static WindowUpdateType ConvertStringToWindowUpdateTypes(std::string type)
{
    static const std::map<const std::string, WindowUpdateType> windowsUpdateTypesTable = {
        {"accessibilityFocus", WindowUpdateType::WINDOW_UPDATE_ACCESSIBILITY_FOCUSED},
        {"focus", WindowUpdateType::WINDOW_UPDATE_FOCUSED},
        {"active", WindowUpdateType::WINDOW_UPDATE_ACTIVE},
        {"add", WindowUpdateType::WINDOW_UPDATE_ADDED},
        {"remove", WindowUpdateType::WINDOW_UPDATE_REMOVED},
        {"bounds", WindowUpdateType::WINDOW_UPDATE_BOUNDS},
        {"title", WindowUpdateType::WINDOW_UPDATE_TITLE},
        {"layer", WindowUpdateType::WINDOW_UPDATE_LAYER},
        {"parent", WindowUpdateType::WINDOW_UPDATE_PARENT},
        {"children", WindowUpdateType::WINDOW_UPDATE_CHILDREN},
        {"pip", WindowUpdateType::WINDOW_UPDATE_PIP}};

    if (windowsUpdateTypesTable.find(type) == windowsUpdateTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return WINDOW_UPDATE_INVALID;
    }

    return windowsUpdateTypesTable.at(type);
}

static EventType ConvertStringToEventInfoTypes(std::string type)
{
    static const std::map<const std::string, EventType> eventInfoTypesTable = {
        {"click", EventType::TYPE_VIEW_CLICKED_EVENT},
        {"longClick", EventType::TYPE_VIEW_LONG_CLICKED_EVENT},
        {"select", EventType::TYPE_VIEW_SELECTED_EVENT},
        {"focus", EventType::TYPE_VIEW_FOCUSED_EVENT},
        {"textUpdate", EventType::TYPE_VIEW_TEXT_UPDATE_EVENT},
        {"hoverEnter", EventType::TYPE_VIEW_HOVER_ENTER_EVENT},
        {"hoverExit", EventType::TYPE_VIEW_HOVER_EXIT_EVENT},
        {"scroll", EventType::TYPE_VIEW_SCROLLED_EVENT},
        {"textSelectionUpdate", EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT},
        {"accessibilityFocus", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT},
        {"accessibilityFocusClear", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT}};

    if (eventInfoTypesTable.find(type) == eventInfoTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return TYPE_VIEW_INVALID;
    }

    return eventInfoTypesTable.at(type);
}

static uint32_t ConvertStringToCapability(std::string type)
{
    HILOG_DEBUG();
    static const std::map<const std::string, uint32_t> capabilitiesTable = {
        {"retrieve", Capability::CAPABILITY_RETRIEVE},
        {"touchGuide", Capability::CAPABILITY_TOUCH_GUIDE},
        {"keyEventObserver", Capability::CAPABILITY_KEY_EVENT_OBSERVER},
        {"zoom", Capability::CAPABILITY_ZOOM},
        {"gesture", Capability::CAPABILITY_GESTURE}};

    if (capabilitiesTable.find(type) == capabilitiesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return 0;
    }

    return capabilitiesTable.at(type);
}

ActionType ConvertStringToAccessibleOperationType(const std::string &type)
{
    std::map<const std::string, ActionType> accessibleOperationTypeTable = {
        {"focus", ActionType::ACCESSIBILITY_ACTION_FOCUS},
        {"clearFocus", ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS},
        {"select", ActionType::ACCESSIBILITY_ACTION_SELECT},
        {"clearSelection", ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION},
        {"click", ActionType::ACCESSIBILITY_ACTION_CLICK},
        {"longClick", ActionType::ACCESSIBILITY_ACTION_LONG_CLICK},
        {"accessibilityFocus", ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS},
        {"clearAccessibilityFocus", ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS},
        {"scrollForward", ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD},
        {"scrollBackward", ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD},
        {"copy", ActionType::ACCESSIBILITY_ACTION_COPY},
        {"paste", ActionType::ACCESSIBILITY_ACTION_PASTE},
        {"cut", ActionType::ACCESSIBILITY_ACTION_CUT},
        {"setSelection", ActionType::ACCESSIBILITY_ACTION_SET_SELECTION},
        {"setText", ActionType::ACCESSIBILITY_ACTION_SET_TEXT},
        {"delete", ActionType::ACCESSIBILITY_ACTION_DELETED}};

    if (accessibleOperationTypeTable.find(type) == accessibleOperationTypeTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return ACCESSIBILITY_ACTION_INVALID;
    }

    return accessibleOperationTypeTable.at(type);
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

GlobalAction ConvertStringToGlobalAction(const std::string &type)
{
    std::map<const std::string, GlobalAction> globalActionTable = {
        {"back", GlobalAction::GLOBAL_ACTION_BACK},
        {"home", GlobalAction::GLOBAL_ACTION_HOME},
        {"recent", GlobalAction::GLOBAL_ACTION_RECENT},
        {"notification", GlobalAction::GLOBAL_ACTION_NOTIFICATION},
        {"lockScreen", GlobalAction::GLOBAL_ACTION_LOCK_SCREEN}};

    if (globalActionTable.find(type) == globalActionTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return GLOBAL_ACTION_INVALID;
    }

    return globalActionTable.at(type);
}

TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type)
{
    static const std::map<const std::string, TextMoveUnit> textMoveUnitTable = {{"char", TextMoveUnit::STEP_CHARACTER},
        {"word", TextMoveUnit::STEP_WORD},
        {"line", TextMoveUnit::STEP_LINE},
        {"page", TextMoveUnit::STEP_PAGE},
        {"paragraph", TextMoveUnit::STEP_PARAGRAPH}};

    if (textMoveUnitTable.find(type) == textMoveUnitTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return STEP_INVALID;
    }

    return textMoveUnitTable.at(type);
}

std::string ConvertTextMoveUnitToString(TextMoveUnit type)
{
    static const std::map<TextMoveUnit, const std::string> textMoveUnitTable = {{TextMoveUnit::STEP_CHARACTER, "char"},
        {TextMoveUnit::STEP_WORD, "word"},
        {TextMoveUnit::STEP_LINE, "line"},
        {TextMoveUnit::STEP_PAGE, "page"},
        {TextMoveUnit::STEP_PARAGRAPH, "paragraph"}};

    if (textMoveUnitTable.find(type) == textMoveUnitTable.end()) {
        HILOG_WARN("invalid key[0x%{public}x]", type);
        return "";
    }

    return textMoveUnitTable.at(type);
}

NotificationCategory ConvertStringToNotificationCategory(const std::string &type)
{
    static const std::map<const std::string, NotificationCategory> notificationCategoryTable = {
        {"call", NotificationCategory::CATEGORY_CALL},
        {"msg", NotificationCategory::CATEGORY_MSG},
        {"email", NotificationCategory::CATEGORY_EMAIL},
        {"event", NotificationCategory::CATEGORY_EVENT},
        {"promo", NotificationCategory::CATEGORY_PROMO},
        {"alarm", NotificationCategory::CATEGORY_ALARM},
        {"progress", NotificationCategory::CATEGORY_PROGRESS},
        {"social", NotificationCategory::CATEGORY_SOCIAL},
        {"err", NotificationCategory::CATEGORY_ERR},
        {"transport", NotificationCategory::CATEGORY_TRANSPORT},
        {"sys", NotificationCategory::CATEGORY_SYS},
        {"service", NotificationCategory::CATEGORY_SERVICE}};

    if (notificationCategoryTable.find(type) == notificationCategoryTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return CATEGORY_INVALID;
    }

    return notificationCategoryTable.at(type);
}

GestureType ConvertStringToGestureType(const std::string &type)
{
    static const std::map<const std::string, GestureType> gestureTypesTable = {
        {"left", GestureType::GESTURE_SWIPE_LEFT},
        {"leftThenRight", GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT},
        {"leftThenUp", GestureType::GESTURE_SWIPE_LEFT_THEN_UP},
        {"leftThenDown", GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN},
        {"right", GestureType::GESTURE_SWIPE_RIGHT},
        {"rightThenLeft", GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT},
        {"rightThenUp", GestureType::GESTURE_SWIPE_RIGHT_THEN_UP},
        {"rightThenDown", GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN},
        {"up", GestureType::GESTURE_SWIPE_UP},
        {"upThenLeft", GestureType::GESTURE_SWIPE_UP_THEN_LEFT},
        {"upThenRight", GestureType::GESTURE_SWIPE_UP_THEN_RIGHT},
        {"upThenDown", GestureType::GESTURE_SWIPE_UP_THEN_DOWN},
        {"down", GestureType::GESTURE_SWIPE_DOWN},
        {"downThenLeft", GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT},
        {"downThenRight", GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT},
        {"downThenUp", GestureType::GESTURE_SWIPE_DOWN_THEN_UP}};

    if (gestureTypesTable.find(type) == gestureTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return GESTURE_INVALID;
    }
    return gestureTypesTable.at(type);
}

void ConvertActionArgsJSToNAPI(
    napi_env env, napi_value object, std::map<std::string, std::string>& args, OHOS::Accessibility::ActionType action)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    std::string str = "";
    switch (action) {
        case ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM:
        case ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM:
            napi_create_string_utf8(env, "htmlItem", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value htmlItemValue = nullptr;
                napi_get_property(env, object, propertyNameValue, &htmlItemValue);
                str = GetStringFromNAPI(env, htmlItemValue);
                args.insert(std::pair<std::string, std::string>("htmlItem", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT:
        case ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT:
            napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value textMoveUnitValue = nullptr;
                napi_get_property(env, object, propertyNameValue, &textMoveUnitValue);
                str = GetStringFromNAPI(env, textMoveUnitValue);
                args.insert(std::pair<std::string, std::string>("textMoveUnit", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_SELECTION:
            napi_create_string_utf8(env, "selectTextBegin", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value selectTextBeginValue = nullptr;
                napi_get_property(env, object, propertyNameValue, &selectTextBeginValue);
                str = GetStringFromNAPI(env, selectTextBeginValue);
                args.insert(std::pair<std::string, std::string>("selectTextBegin", str.c_str()));
            }
            napi_create_string_utf8(env, "selectTextEnd", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value selectTextEndValue = nullptr;
                napi_get_property(env, object, propertyNameValue, &selectTextEndValue);
                str = GetStringFromNAPI(env, selectTextEndValue);
                args.insert(std::pair<std::string, std::string>("selectTextEnd", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_TEXT:
            napi_create_string_utf8(env, "setText", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value setTextValue = nullptr;
                napi_get_property(env, object, propertyNameValue, &setTextValue);
                str = GetStringFromNAPI(env, setTextValue);
                args.insert(std::pair<std::string, std::string>("setText", str.c_str()));
            }
            break;
        default:
            break;
    }
}

bool ConvertEventInfoJSToNAPI(napi_env env, napi_value object, AccessibilityEventInfo& eventInfo)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    int32_t dataValue = 0;
    std::string str = "";
    EventType eventType = TYPE_VIEW_INVALID;
    napi_create_string_utf8(env, "type", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventType = ConvertStringToEventInfoTypes(str);
        eventInfo.SetEventType(eventType);
        if (eventType == TYPE_VIEW_INVALID) {
            return false;
        }
    } else {
        return false;
    }

    napi_create_string_utf8(env, "windowUpdateType", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value windowUpdateTypeValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &windowUpdateTypeValue);
        str = GetStringFromNAPI(env, windowUpdateTypeValue);
        eventInfo.SetEventType(TYPE_WINDOW_UPDATE);
        eventInfo.SetWindowChangeTypes(ConvertStringToWindowUpdateTypes(str));
    }

    napi_create_string_utf8(env, "bundleName", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value bundleNameValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &bundleNameValue);
        str = GetStringFromNAPI(env, bundleNameValue);
        if (str == "") {
            return false;
        }
        eventInfo.SetBundleName(str);
    } else {
        return false;
    }

    napi_create_string_utf8(env, "componentType", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value componentTypeValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &componentTypeValue);
        str = GetStringFromNAPI(env, componentTypeValue);
        eventInfo.SetComponentType(str);
    }

    napi_create_string_utf8(env, "pageId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value pageIdValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &pageIdValue);
        napi_get_value_int32(env, pageIdValue, &dataValue);
        eventInfo.SetPageId(dataValue);
    }

    napi_create_string_utf8(env, "description", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value descriptionValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &descriptionValue);
        str = GetStringFromNAPI(env, descriptionValue);
        eventInfo.SetDescription(str);
    }

    napi_create_string_utf8(env, "triggerAction", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value triggerActionValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &triggerActionValue);
        str = GetStringFromNAPI(env, triggerActionValue);
        eventInfo.SetTriggerAction(ConvertStringToAccessibleOperationType(str));
        if (eventInfo.GetTriggerAction() == ACCESSIBILITY_ACTION_INVALID) {
            return false;
        }
    } else {
        return false;
    }

    napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value textMoveUnitValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &textMoveUnitValue);
        str = GetStringFromNAPI(env, textMoveUnitValue);
        eventInfo.SetTextMovementStep(ConvertStringToTextMoveUnit(str));
    }

    napi_create_string_utf8(env, "contents", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value contentsValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &contentsValue);
        napi_value data = nullptr;
        uint32_t dataLen = 0;
        napi_get_array_length(env, contentsValue, &dataLen);
        for (uint32_t i = 0; i < dataLen; i++) {
            napi_get_element(env, contentsValue, i, &data);
            str = GetStringFromNAPI(env, data);
            eventInfo.AddContent(str);
        }
    }

    napi_create_string_utf8(env, "lastContent", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value lastContentValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &lastContentValue);
        str = GetStringFromNAPI(env, lastContentValue);
        eventInfo.SetLatestContent(str);
    }

    napi_create_string_utf8(env, "beginIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value beginIndexValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &beginIndexValue);
        napi_get_value_int32(env, beginIndexValue, &dataValue);
        eventInfo.SetBeginIndex(dataValue);
    }

    napi_create_string_utf8(env, "currentIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value currentIndexValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &currentIndexValue);
        napi_get_value_int32(env, currentIndexValue, &dataValue);
        eventInfo.SetCurrentIndex(dataValue);
    }

    napi_create_string_utf8(env, "endIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value endIndexValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &endIndexValue);
        napi_get_value_int32(env, endIndexValue, &dataValue);
        eventInfo.SetEndIndex(dataValue);
    }

    napi_create_string_utf8(env, "itemCount", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value itemCountValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &itemCountValue);
        napi_get_value_int32(env, itemCountValue, &dataValue);
        eventInfo.SetItemCounts(dataValue);
    }
    return true;
}

static bool ConvertGesturePointJSToNAPI(
    napi_env env, napi_value object, AccessibilityGesturePosition& gesturePathPosition)
{
    HILOG_DEBUG();
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    double position = 0;

    napi_create_string_utf8(env, "positionX", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value valueX = nullptr;
        napi_get_property(env, object, propertyNameValue, &valueX);
        napi_get_value_double(env, valueX, &position);
        gesturePathPosition.positionX_ = static_cast<float>(position);
    } else {
        return false;
    }

    napi_create_string_utf8(env, "positionY", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value valueY = nullptr;
        napi_get_property(env, object, propertyNameValue, &valueY);
        napi_get_value_double(env, valueY, &position);
        gesturePathPosition.positionY_ = static_cast<float>(position);
    } else {
        return false;
    }
    return true;
}

static void ConvertGesturePathJSToNAPI(napi_env env, napi_value object,
    std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;

    napi_create_string_utf8(env, "points", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value positionValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &positionValue);
        napi_value jsValue = nullptr;
        bool isArray = false;
        uint32_t dataLen = 0;
        if (napi_is_array(env, positionValue, &isArray) != napi_ok || isArray == false) {
            HILOG_ERROR("object is not an array.");
            return;
        }
        if (napi_get_array_length(env, positionValue, &dataLen) != napi_ok) {
            HILOG_ERROR("get array length failed.");
            return;
        }
        for (uint32_t i = 0; i < dataLen; i++) {
            jsValue = nullptr;
            AccessibilityGesturePosition path;
            if (napi_get_element(env, positionValue, i, &jsValue) != napi_ok) {
                HILOG_ERROR("get element of paths failed and i = %{public}d", i);
                return;
            }
            bool result = ConvertGesturePointJSToNAPI(env, jsValue, path);
            if (result) {
                gesturePath->AddPosition(path);
            }
        }
    }

    int64_t durationTime = 0;
    napi_create_string_utf8(env, "durationTime", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value timeValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &timeValue);
        napi_get_value_int64(env, timeValue, &durationTime);
        gesturePath->SetDurationTime(durationTime);
    }
}

void ConvertGesturePathsJSToNAPI(napi_env env, napi_value object,
    std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath,
    std::vector<std::shared_ptr<AccessibilityGestureInjectPath>>& gesturePathArray,
    bool &isParameterArray)
{
    HILOG_DEBUG();
    if (napi_is_array(env, object, &isParameterArray) != napi_ok) {
        HILOG_ERROR("judge array error.");
        return;
    }

    if (isParameterArray) {
        uint32_t dataLen = 0;
        if (napi_get_array_length(env, object, &dataLen) != napi_ok) {
            HILOG_ERROR("get array length failed.");
            return;
        }
        napi_value gesturePathJs = nullptr;
        for (uint32_t i = 0; i < dataLen; i++) {
            gesturePathJs = nullptr;
            if (napi_get_element(env, object, i, &gesturePathJs) != napi_ok) {
                HILOG_ERROR("get element of paths failed and i = %{public}d", i);
                return;
            }
            std::shared_ptr<AccessibilityGestureInjectPath> path = std::make_shared<AccessibilityGestureInjectPath>();
            ConvertGesturePathJSToNAPI(env, gesturePathJs, path);
            gesturePathArray.emplace_back(path);
        }
    } else {
        ConvertGesturePathJSToNAPI(env, object, gesturePath);
    }
}

void ConvertKeyEventToJS(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent)
{
    napi_value keyCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, keyEvent->GetKeyCode(), &keyCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "keyCode", keyCode));

    napi_value keyAction;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, keyEvent->GetKeyAction(), &keyAction));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "keyAction", keyAction));

    napi_value keys;
    uint32_t idx = 0;
    std::vector<OHOS::MMI::KeyEvent::KeyItem> keyItems = keyEvent->GetKeyItems();
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &keys));
    for (auto &key : keyItems) {
        napi_value keyItem = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &keyItem));

        napi_value pressed;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, key.IsPressed(), &pressed));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyItem, "pressed", pressed));

        napi_value downTime;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, key.GetDownTime(), &downTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyItem, "downTime", downTime));

        napi_value deviceId;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, key.GetDeviceId(), &deviceId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyItem, "deviceId", deviceId));

        napi_value itemKeyCode;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, key.GetKeyCode(), &itemKeyCode));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyItem, "keyCode", itemKeyCode));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, keys, idx, keyItem));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "keys", keys));
}

void ConvertCaptionPropertyToJS(
    napi_env env, napi_value& result, OHOS::AccessibilityConfig::CaptionProperty captionProperty)
{
    HILOG_DEBUG();

    napi_value value;

    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, captionProperty.GetFontFamily().c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fontFamily", value));

    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, captionProperty.GetFontScale(), &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fontScale", value));

    uint32_t color = captionProperty.GetFontColor();
    std::string colorStr = ConvertColorToString(color);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fontColor", value));

    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, captionProperty.GetFontEdgeType().c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fontEdgeType", value));

    color = captionProperty.GetBackgroundColor();
    colorStr = ConvertColorToString(color);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "backgroundColor", value));

    color = captionProperty.GetWindowColor();
    colorStr = ConvertColorToString(color);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "windowColor", value));
}

const uint32_t COLOR_TRANSPARENT = 0x00000000;
const uint32_t COLOR_WHITE = 0xffffffff;
const uint32_t COLOR_BLACK = 0xff000000;
const uint32_t COLOR_RED = 0xffff0000;
const uint32_t COLOR_GREEN = 0xff00ff00;
const uint32_t COLOR_BLUE = 0xff0000ff;
const uint32_t COLOR_GRAY = 0xffc0c0c0;

constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
constexpr uint32_t COLOR_STRING_BASE = 16;
const std::regex COLOR_WITH_MAGIC("#[0-9A-Fa-f]{6,8}");
const std::regex COLOR_WITH_MAGIC_MINI("#[0-9A-Fa-f]{3,4}");
constexpr uint32_t COLOR_ALPHA_MASK = 0xff000000;

constexpr int32_t RGB_LENGTH = 6;
constexpr int32_t ALPHA_LENGTH = 2;
constexpr int32_t ALPHA_MOVE = 24;
constexpr int32_t COLOR_MOVE = 8;
const char UNICODE_BODY = '0';

uint32_t ConvertColorStringToNumer(std::string colorStr)
{
    HILOG_DEBUG("colorStr is %{public}s", colorStr.c_str());
    uint32_t color = COLOR_TRANSPARENT;
    if (colorStr.empty()) {
        // Empty string, return transparent
        return color;
    }
    // Remove all " ".
    colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());

    // Regex match for #909090 or #90909090.
    if (std::regex_match(colorStr, COLOR_WITH_MAGIC)) {
        colorStr.erase(0, 1);
        auto value = stoul(colorStr, nullptr, COLOR_STRING_BASE);
        if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
            // No alpha specified, set alpha to 0xff
            value |= COLOR_ALPHA_MASK;
        } else {
            auto alpha = value << ALPHA_MOVE;
            auto rgb = value >> COLOR_MOVE;
            value = alpha | rgb;
        }
        color = value;
        return color;
    }
    // Regex match for #rgb or #rgba.
    if (std::regex_match(colorStr, COLOR_WITH_MAGIC_MINI)) {
        colorStr.erase(0, 1);
        std::string newColorStr;
        // Translate #rgb or #rgba to #rrggbb or #rrggbbaa
        for (auto& c : colorStr) {
            newColorStr += c;
            newColorStr += c;
        }
        auto valueMini = stoul(newColorStr, nullptr, COLOR_STRING_BASE);
        if (newColorStr.length() < COLOR_STRING_SIZE_STANDARD) {
            // No alpha specified, set alpha to 0xff
            valueMini |= COLOR_ALPHA_MASK;
        } else {
            auto alphaMini = valueMini << ALPHA_MOVE;
            auto rgbMini = valueMini >> COLOR_MOVE;
            valueMini = alphaMini | rgbMini;
        }
        color = valueMini;
        return color;
    }

    // Match for special string
    static const std::map<std::string, uint32_t> colorTable {
        std::make_pair("black", COLOR_BLACK),
        std::make_pair("blue", COLOR_BLUE),
        std::make_pair("gray", COLOR_GRAY),
        std::make_pair("green", COLOR_GREEN),
        std::make_pair("red", COLOR_RED),
        std::make_pair("white", COLOR_WHITE),
    };
    auto it = colorTable.find(colorStr.c_str());
    if (it != colorTable.end()) {
        color = it->second;
    }
    return color;
}

std::string ConvertColorToString(uint32_t color)
{
    HILOG_DEBUG("color is 0X%{public}x", color);
    uint32_t rgb = color & (~COLOR_ALPHA_MASK);
    uint32_t alpha = (color) >> ALPHA_MOVE;
    std::stringstream rgbStream;
    rgbStream << std::hex << std::setw(RGB_LENGTH) << std::setfill(UNICODE_BODY) << rgb;
    std::stringstream alphaStream;
    alphaStream << std::hex << std::setw(ALPHA_LENGTH) << std::setfill(UNICODE_BODY) << alpha;
    std::string rgbStr(rgbStream.str());
    std::string alphaStr(alphaStream.str());
    std::string colorStr = "#" + rgbStr + alphaStr;
    HILOG_DEBUG("colorStr is %{public}s", colorStr.c_str());
    return colorStr;
}

uint32_t GetColorValue(napi_env env, napi_value object, napi_value propertyNameValue)
{
    uint32_t color = COLOR_TRANSPARENT;
    char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
    size_t outSize = 0;
    napi_valuetype valueType = napi_undefined;
    napi_value value = nullptr;
    napi_get_property(env, object, propertyNameValue, &value);
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR("GetColorValue error! status is %{public}d", status);
        return color;
    }
    if (valueType == napi_number) {
        napi_get_value_uint32(env, value, &color);
        HILOG_DEBUG("valueType number, color is 0x%{public}x", color);
    }
    if (valueType == napi_string) {
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        color = ConvertColorStringToNumer(std::string(outBuffer));
    }
    HILOG_DEBUG("color is 0x%{public}x", color);
    return color;
}

uint32_t GetColorValue(napi_env env, napi_value value)
{
    uint32_t color = COLOR_TRANSPARENT;
    char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
    size_t outSize = 0;
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR("GetColorValue error! status is %{public}d", status);
        return color;
    }
    if (valueType == napi_number) {
        HILOG_DEBUG("color type is number");
        napi_get_value_uint32(env, value, &color);
    }
    if (valueType == napi_string) {
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        color = ConvertColorStringToNumer(std::string(outBuffer));
    }
    HILOG_DEBUG("color is 0x%{public}x", color);
    return color;
}

bool ConvertObjToCaptionProperty(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty)
{
    HILOG_DEBUG("start");
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    int32_t num = 0;

    napi_create_string_utf8(env, "fontFamily", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        ptrCaptionProperty->SetFontFamily(std::string(outBuffer));
    } else {
        return false;
    }

    napi_create_string_utf8(env, "fontScale", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &num);
        ptrCaptionProperty->SetFontScale(num);
    } else {
        return false;
    }

    napi_create_string_utf8(env, "fontColor", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        ptrCaptionProperty->SetFontColor(GetColorValue(env, object, propertyNameValue));
    } else {
        return false;
    }

    napi_create_string_utf8(env, "fontEdgeType", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        ptrCaptionProperty->SetFontEdgeType(std::string(outBuffer));
    } else {
        return false;
    }

    napi_create_string_utf8(env, "backgroundColor", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        ptrCaptionProperty->SetBackgroundColor(GetColorValue(env, object, propertyNameValue));
    } else {
        return false;
    }

    napi_create_string_utf8(env, "windowColor", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        ptrCaptionProperty->SetWindowColor(GetColorValue(env, object, propertyNameValue));
    } else {
        return false;
    }

    return true;
}

void ConvertJSToStringVec(napi_env env, napi_value arrayValue, std::vector<std::string>& values)
{
    HILOG_DEBUG();
    values.clear();

    bool hasElement = true;
    for (int32_t i = 0; hasElement; i++) {
        napi_has_element(env, arrayValue, i, &hasElement);
        if (hasElement) {
            napi_value value = nullptr;
            napi_get_element(env, arrayValue, i, &value);

            char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
            size_t outSize = 0;
            napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);

            values.push_back(std::string(outBuffer));
        }
    }
}

void ConvertJSToEventTypes(napi_env env, napi_value arrayValue, uint32_t &eventTypes)
{
    HILOG_DEBUG();
    eventTypes = TYPE_VIEW_INVALID;
    std::vector<std::string> values;
    ConvertJSToStringVec(env, arrayValue, values);
    for (auto &value : values) {
        HILOG_DEBUG("the event type is %{public}s", value.c_str());
        EventType eventType = ConvertStringToEventInfoTypes(value);
        if (eventType == TYPE_VIEW_INVALID) {
            HILOG_ERROR("the event type is invalid");
            eventTypes = TYPE_VIEW_INVALID;
            return;
        }
        eventTypes |= eventType;
    }
}

void ConvertJSToCapabilities(napi_env env, napi_value arrayValue, uint32_t &capabilities)
{
    HILOG_DEBUG();
    capabilities = 0;
    std::vector<std::string> values;
    ConvertJSToStringVec(env, arrayValue, values);
    for (auto &value : values) {
        HILOG_DEBUG("capability is %{public}s", value.c_str());
        uint32_t capability = ConvertStringToCapability(value);
        if (capability == 0) {
            HILOG_ERROR("the capability is invalid");
            capabilities = 0;
            return;
        }
        capabilities |= capability;
    }
}

void ConvertEnabledToJS(napi_env env, napi_value& captionsManager, bool value)
{
    HILOG_DEBUG();
    napi_value keyCode;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, value, &keyCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, captionsManager, "enabled", keyCode));
}

void ConvertStyleToJS(
    napi_env env, napi_value& captionsManager, OHOS::AccessibilityConfig::CaptionProperty captionProperty_)
{
    HILOG_DEBUG();
    napi_value keyCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &keyCode));

    ConvertCaptionPropertyToJS(env, keyCode, captionProperty_);

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, captionsManager, "style", keyCode));
}

void ConvertStringVecToJS(napi_env env, napi_value &result, std::vector<std::string> values)
{
    HILOG_DEBUG();
    size_t index = 0;
    for (auto& value : values) {
        napi_value str = nullptr;
        napi_create_string_utf8(env, value.c_str(), value.size(), &str);
        napi_set_element(env, result, index, str);
        index++;
    }
}
