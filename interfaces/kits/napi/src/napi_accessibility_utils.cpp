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

#include "accessibility_utils.h"
#include "accessibility_def.h"

#include <charconv>
#include <cmath>
#include <iomanip>
#include <regex>
#include <sstream>
#include <vector>

#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace AccessibilityNapi {
namespace {
    const uint32_t COLOR_TRANSPARENT = 0x00000000;
    const uint32_t COLOR_WHITE = 0xffffffff;
    const uint32_t COLOR_BLACK = 0xff000000;
    const uint32_t COLOR_RED = 0xffff0000;
    const uint32_t COLOR_GREEN = 0xff00ff00;
    const uint32_t COLOR_BLUE = 0xff0000ff;
    const uint32_t COLOR_GRAY = 0xffc0c0c0;

    constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
    constexpr uint32_t COLOR_STRING_SIZE_4 = 4;
    constexpr uint32_t COLOR_STRING_SIZE_5 = 5;
    constexpr uint32_t COLOR_STRING_SIZE_7 = 7;
    constexpr uint32_t COLOR_STRING_SIZE_9 = 9;
    constexpr uint32_t COLOR_STRING_BASE = 16;
    constexpr uint32_t COLOR_ALPHA_MASK = 0xff000000;

    constexpr int32_t RGB_LENGTH = 6;
    constexpr int32_t ALPHA_LENGTH = 2;
    constexpr int32_t ALPHA_MOVE = 24;
    constexpr int32_t COLOR_MOVE = 8;
    const char UNICODE_BODY = '0';
    const std::string HALF_VALUE = "0";
    const std::string FULL_VALUE = "1";
    const std::string NUMBER_VALID_CHARS = "0123456789ABCDEFabcdef";
} // namespace
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityConfig;

uint32_t ParseResourceIdFromNAPI(napi_env env, napi_value value)
{
    uint32_t idValue = 0;
    bool hasProperty = false;
    napi_value propertyName = nullptr;
    napi_create_string_utf8(env, "id", NAPI_AUTO_LENGTH, &propertyName);
    napi_has_property(env, value, propertyName, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, value, propertyName, &itemValue);
        napi_get_value_uint32(env, itemValue, &idValue);
    }
    HILOG_DEBUG("get resource id is %{public}d", idValue);
    return idValue;
}
 
std::string ParseResourceBundleNameFromNAPI(napi_env env, napi_value value)
{
    std::string bundleNameValue;
    bool hasProperty = false;
    napi_value propertyName = nullptr;
    napi_create_string_utf8(env, "bundleName", NAPI_AUTO_LENGTH, &propertyName);
    napi_has_property(env, value, propertyName, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, value, propertyName, &itemValue);
        bundleNameValue = GetStringFromNAPI(env, itemValue);
    }
    HILOG_DEBUG("get resource bundleName is %{public}s", bundleNameValue.c_str());
    return bundleNameValue;
}
 
std::string ParseResourceModuleNameFromNAPI(napi_env env, napi_value value)
{
    std::string moduleNameValue;
    bool hasProperty = false;
    napi_value propertyName = nullptr;
    napi_create_string_utf8(env, "moduleName", NAPI_AUTO_LENGTH, &propertyName);
    napi_has_property(env, value, propertyName, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, value, propertyName, &itemValue);
        moduleNameValue = GetStringFromNAPI(env, itemValue);
    }
    HILOG_DEBUG("get resource moduleName is %{public}s", moduleNameValue.c_str());
    return moduleNameValue;
}

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
    napi_valuetype valuetype = napi_null;
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
    napi_valuetype valuetype = napi_null;
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
    HILOG_DEBUG("param=%{public}s.", param.c_str());
    return true;
}

bool ParseNumber(napi_env env, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype = napi_null;
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

bool ParseInt32(napi_env env, int32_t& param, napi_value args)
{
    if (!ParseNumber(env, args)) {
        return false;
    }

    napi_get_value_int32(env, args, &param);
    return true;
}

bool ParseInt64(napi_env env, int64_t& param, napi_value args)
{
    if (!ParseNumber(env, args)) {
        return false;
    }

    napi_get_value_int64(env, args, &param);
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

bool CheckJsFunction(napi_env env, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype = napi_null;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_function) {
        HILOG_DEBUG("Wrong argument type. function expected.");
        return false;
    }

    return true;
}

NAccessibilityErrMsg QueryRetMsg(OHOS::Accessibility::RetError errorCode)
{
    switch (errorCode) {
        case OHOS::Accessibility::RetError::RET_OK:
            return { NAccessibilityErrorCode::ACCESSIBILITY_OK, "" };
        case OHOS::Accessibility::RetError::RET_ERR_FAILED:
        case OHOS::Accessibility::RetError::RET_ERR_NULLPTR:
        case OHOS::Accessibility::RetError::RET_ERR_IPC_FAILED:
        case OHOS::Accessibility::RetError::RET_ERR_SAMGR:
        case OHOS::Accessibility::RetError::RET_ERR_TIME_OUT:
        case OHOS::Accessibility::RetError::RET_ERR_REGISTER_EXIST:
        case OHOS::Accessibility::RetError::RET_ERR_NO_REGISTER:
        case OHOS::Accessibility::RetError::RET_ERR_NO_CONNECTION:
        case OHOS::Accessibility::RetError::RET_ERR_NO_WINDOW_CONNECTION:
        case OHOS::Accessibility::RetError::RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE:
        case OHOS::Accessibility::RetError::RET_ERR_PERFORM_ACTION_FAILED_BY_ACE:
        case OHOS::Accessibility::RetError::RET_ERR_NO_INJECTOR:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
        case OHOS::Accessibility::RetError::RET_ERR_INVALID_PARAM:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM, ERROR_MESSAGE_PARAMETER_ERROR };
        case OHOS::Accessibility::RetError::RET_ERR_NO_PERMISSION:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_PERMISSION, ERROR_MESSAGE_NO_PERMISSION };
        case OHOS::Accessibility::RetError::RET_ERR_CONNECTION_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_TARGET_ABILITY_ALREADY_ENABLED,
                     ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED };
        case OHOS::Accessibility::RetError::RET_ERR_NO_CAPABILITY:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_RIGHT, ERROR_MESSAGE_NO_RIGHT };
        case OHOS::Accessibility::RetError::RET_ERR_NOT_INSTALLED:
        case OHOS::Accessibility::RetError::RET_ERR_NOT_ENABLED:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME,
                     ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME};
        case OHOS::Accessibility::RetError::RET_ERR_PROPERTY_NOT_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_PROPERTY_NOT_EXIST,
                     ERROR_MESSAGE_PROPERTY_NOT_EXIST };
        case OHOS::Accessibility::RetError::RET_ERR_ACTION_NOT_SUPPORT:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ACTION_NOT_SUPPORT,
                     ERROR_MESSAGE_ACTION_NOT_SUPPORT };
        case OHOS::Accessibility::RetError::RET_ERR_NOT_SYSTEM_APP:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NOT_SYSTEM_APP,
                     ERROR_MESSAGE_NOT_SYSTEM_APP };
        case OHOS::Accessibility::RetError::RET_ERR_ENABLE_MAGNIFICATION:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ENABLE_MAGNIFICATION,
                    ERROR_MESSAGE_ENABLE_MAGNIFICATION };
        case OHOS::Accessibility::RetError::RET_ERR_MAGNIFICATION_NOT_SUPPORT:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_CAPABILITY_NOT_SUPPORT,
                    ERROR_MESSAGE_CAPABILITY_NOT_SUPPORT };
        default:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
    }
}

napi_value CreateBusinessError(napi_env env, OHOS::Accessibility::RetError errCode)
{
    napi_value result = nullptr;
    if (errCode == OHOS::Accessibility::RetError::RET_OK) {
        napi_get_undefined(env, &result);
    } else {
        NAccessibilityErrMsg errMsg = QueryRetMsg(errCode);
        napi_value eCode = nullptr;
        napi_create_int32(env, static_cast<int32_t>(errMsg.errCode), &eCode);
        napi_value eMsg = nullptr;
        napi_create_string_utf8(env, errMsg.message.c_str(), NAPI_AUTO_LENGTH, &eMsg);
        napi_create_error(env, nullptr, eMsg, &result);
        napi_set_named_property(env, result, "code", eCode);
    }
    return result;
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

bool CheckObserverEqual(napi_env env, napi_value observer, napi_env iterEnv, napi_ref iterRef)
{
    HILOG_DEBUG();
    if (env != iterEnv) {
        return false;
    }
    HILOG_DEBUG("Same env, begin check observer equal");
    napi_value item = nullptr;
    bool equalFlag = false;
    napi_get_reference_value(iterEnv, iterRef, &item);
    napi_status status = napi_strict_equals(iterEnv, item, observer, &equalFlag);
    if (status == napi_ok && equalFlag) {
        HILOG_DEBUG("Observer exist");
        return true;
    }
    return false;
}

/**********************************************************
 * Convert native object to js object
 *********************************************************/
void ConvertRectToJS(napi_env env, napi_value result, const Accessibility::Rect& rect)
{
    napi_value nLeftTopX = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopXScreenPostion(), &nLeftTopX));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "left", nLeftTopX));

    napi_value nLeftTopY = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopYScreenPostion(), &nLeftTopY));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "top", nLeftTopY));

    napi_value nWidth = nullptr;
    int32_t width = rect.GetRightBottomXScreenPostion() - rect.GetLeftTopXScreenPostion();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, width, &nWidth));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "width", nWidth));

    napi_value nHeight = nullptr;
    int32_t height = rect.GetRightBottomYScreenPostion() - rect.GetLeftTopYScreenPostion();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, height, &nHeight));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "height", nHeight));
}

void ConvertGridItemToJS(napi_env env, napi_value result, const Accessibility::GridItemInfo& gridItem)
{
    napi_value rowIndex = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetRowIndex(), &rowIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "rowIndex", rowIndex));
    napi_value columnIndex = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetColumnIndex(), &columnIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "columnIndex", columnIndex));
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

void ConvertAccessibleAbilityInfoToJS(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info)
{
    HILOG_DEBUG();
    ConvertAccessibleAbilityInfoToJSPart1(env, result, info);
    ConvertAccessibleAbilityInfoToJSPart2(env, result, info);
    ConvertAccessibleAbilityInfoToJSPart3(env, result, info);
}

void ConvertAccessibleAbilityInfoToJSPart1(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info)
{
    HILOG_DEBUG();
    napi_value nId = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetId().c_str(), NAPI_AUTO_LENGTH, &nId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "id", nId));

    napi_value nName = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetName().c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", nName));

    napi_value nBundleName = nullptr;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, info.GetPackageName().c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", nBundleName));

    napi_value nAbilityType = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityType));
    uint32_t abilityTypesValue = info.GetAccessibilityAbilityType();
    std::vector<std::string> abilityTypes = ParseAbilityTypesToVec(abilityTypesValue);
    for (size_t idxType = 0; idxType < abilityTypes.size(); idxType++) {
        napi_value nType = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityTypes[idxType].c_str(),
            NAPI_AUTO_LENGTH, &nType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityType, idxType, nType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "abilityTypes", nAbilityType));
}

void ConvertAccessibleAbilityInfoToJSPart2(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info)
{
    HILOG_DEBUG();
    napi_value nCapabilities = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nCapabilities));
    uint32_t capabilitiesValue = info.GetStaticCapabilityValues();
    std::vector<std::string> capabilities = ParseCapabilitiesToVec(capabilitiesValue);
    for (size_t idxCap = 0; idxCap < capabilities.size(); idxCap++) {
        napi_value nCap = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, capabilities[idxCap].c_str(),
            NAPI_AUTO_LENGTH, &nCap));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nCapabilities, idxCap, nCap));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "capabilities", nCapabilities));

    napi_value description = nullptr;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, info.GetDescription().c_str(), NAPI_AUTO_LENGTH, &description));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", description));

    napi_value nEventTypes = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nEventTypes));
    uint32_t eventTypesValue = info.GetEventTypes();
    std::vector<std::string> eventTypes = ParseEventTypesToVec(eventTypesValue);
    for (size_t idxEve = 0; idxEve < eventTypes.size(); idxEve++) {
        napi_value nEve = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, eventTypes[idxEve].c_str(), NAPI_AUTO_LENGTH, &nEve));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nEventTypes, idxEve, nEve));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "eventTypes", nEventTypes));

    napi_value filterBundleNames = nullptr;
    size_t idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &filterBundleNames));
    std::vector<std::string> strFilterBundleNames = info.GetFilterBundleNames();
    for (auto &filterBundleName : strFilterBundleNames) {
        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, filterBundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, filterBundleNames, idx, bundleName));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "targetBundleNames", filterBundleNames));
}

void ConvertAccessibleAbilityInfoToJSPart3(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info)
{
    HILOG_DEBUG();
    napi_value nNeedHide = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.NeedHide(), &nNeedHide));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "needHide", nNeedHide));
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
        {EventType::TYPE_TOUCH_END, "touchEnd"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY, "requestFocusForAccessibility"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY, "announceForAccessibility"},
        {EventType::TYPE_PAGE_OPEN, "pageOpen"},
        {EventType::TYPE_PAGE_CLOSE, "pageClose"},
        {EventType::TYPE_ELEMENT_INFO_CHANGE, "elementInfoChange"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT, "announceForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT,
            "requestFocusForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_SCROLLING_EVENT, "scrolling"}};

    if (a11yEvtTypeTable.find(type) == a11yEvtTypeTable.end()) {
        return "";
    }

    return a11yEvtTypeTable.at(type);
}

AccessibilityEventType CovertStringToAccessibilityEventType(const AccessibilityEventInfo &eventInfo,
    const std::string &eventTypeString)
{
    EventType type = eventInfo.GetEventType();
    AccessibilityEventType accessibilityEventType = CovertStringToAccessibilityEventType(eventTypeString);
    if (type == TYPE_WINDOW_UPDATE && accessibilityEventType == AccessibilityEventType::TYPE_FOCUS) {
        return AccessibilityEventType::TYPE_WINDOW_FOCUS;
    }
    return accessibilityEventType;
}

AccessibilityEventType CovertStringToAccessibilityEventType(const std::string &eventType)
{
    static const std::map<const std::string, AccessibilityEventType> eventTypeTable = {
        {"accessibilityFocus", AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS},
        {"accessibilityFocusClear", AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS_CLEAR},
        {"click", AccessibilityEventType::TYPE_CLICK},
        {"longClick", AccessibilityEventType::TYPE_LONG_CLICK},
        {"select", AccessibilityEventType::TYPE_SELECT},
        {"hoverEnter", AccessibilityEventType::TYPE_HOVER_ENTER},
        {"hoverExit", AccessibilityEventType::TYPE_HOVER_EXIT},
        {"focus", AccessibilityEventType::TYPE_FOCUS},
        {"textUpdate", AccessibilityEventType::TYPE_TEXT_UPDATE},
        {"textSelectionUpdate", AccessibilityEventType::TYPE_TEXT_SELECTION_UPDATE},
        {"scroll", AccessibilityEventType::TYPE_SCROLL},
        {"requestFocusForAccessibility", AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY},
        {"announceForAccessibility", AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY},
        {"requestFocusForAccessibilityNotInterrupt",
            AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"announceForAccessibilityNotInterrupt",
            AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"elementInfoChange", AccessibilityEventType::TYPE_ELEMENT_INFO_CHANGE},
        {"scrolling", AccessibilityEventType::TYPE_SCROLLING},
        {"add", AccessibilityEventType::TYPE_WINDOW_ADD},
        {"remove", AccessibilityEventType::TYPE_WINDOW_REMOVE},
        {"bounds", AccessibilityEventType::TYPE_WINDOW_BOUNDS},
        {"active", AccessibilityEventType::TYPE_WINDOW_ACTIVE},
        {"focus", AccessibilityEventType::TYPE_WINDOW_FOCUS},
        {"property", AccessibilityEventType::TYPE_WINDOW_PROPERTY},
        {"layer", AccessibilityEventType::TYPE_WINDOW_LAYER},
        {"touchBegin", AccessibilityEventType::TYPE_TOUCH_BEGIN},
        {"touchEnd", AccessibilityEventType::TYPE_TOUCH_END},
        {"pageContentUpdate", AccessibilityEventType::TYPE_PAGE_CONTENT_UPDATE},
        {"pageStateUpdate", AccessibilityEventType::TYPE_PAGE_STATE_UPDATE},
        {"pageOpen", AccessibilityEventType::TYPE_PAGE_OPEN},
        {"pageClose", AccessibilityEventType::TYPE_PAGE_CLOSE},
        {"left", AccessibilityEventType::TYPE_SWIPE_LEFT},
        {"leftThenRight", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_RIGHT},
        {"leftThenUp", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_UP},
        {"leftThenDown", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_DOWN},
        {"right", AccessibilityEventType::TYPE_SWIPE_RIGHT},
        {"rightThenLeft", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_LEFT},
        {"rightThenUp", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_UP},
        {"rightThenDown", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_DOWN},
        {"up", AccessibilityEventType::TYPE_SWIPE_UP},
        {"upThenLeft", AccessibilityEventType::TYPE_SWIPE_UP_THEN_LEFT},
        {"upThenRight", AccessibilityEventType::TYPE_SWIPE_UP_THEN_RIGHT},
        {"upThenDown", AccessibilityEventType::TYPE_SWIPE_UP_THEN_DOWN},
        {"down", AccessibilityEventType::TYPE_SWIPE_DOWN},
        {"downThenLeft", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_LEFT},
        {"downThenRight", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_RIGHT},
        {"downThenUp", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_UP},
        {"twoFingerSingleTap", AccessibilityEventType::TYPE_TWO_FINGER_SINGLE_TAP},
        {"twoFingerDoubleTap", AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP},
        {"twoFingerDoubleTapAndHold", AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD},
        {"twoFingerTripleTap", AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP},
        {"twoFingerTripleTapAndHold", AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD},
        {"threeFingerSingleTap", AccessibilityEventType::TYPE_THREE_FINGER_SINGLE_TAP},
        {"threeFingerDoubleTap", AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP},
        {"threeFingerDoubleTapAndHold", AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD},
        {"threeFingerTripleTap", AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP},
        {"threeFingerTripleTapAndHold", AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD},
        {"fourFingerSingleTap", AccessibilityEventType::TYPE_FOUR_FINGER_SINGLE_TAP},
        {"fourFingerDoubleTap", AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP},
        {"fourFingerDoubleTapAndHold", AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD},
        {"fourFingerTripleTap", AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP},
        {"fourFingerTripleTapAndHold", AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD},
        {"threeFingerSwipeUp", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_UP},
        {"threeFingerSwipeDown", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_DOWN},
        {"threeFingerSwipeLeft", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_LEFT},
        {"threeFingerSwipeRight", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_RIGHT},
        {"fourFingerSwipeUp", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_UP},
        {"fourFingerSwipeDown", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_DOWN},
        {"fourFingerSwipeLeft", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_LEFT},
        {"fourFingerSwipeRight", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_RIGHT},
    };
    if (eventTypeTable.find(eventType) == eventTypeTable.end()) {
        return AccessibilityEventType::TYPE_ERROR;
    }
    return eventTypeTable.at(eventType);
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
        {GestureType::GESTURE_SWIPE_DOWN_THEN_UP, "downThenUp"},
        {GestureType::GESTURE_TWO_FINGER_SINGLE_TAP, "twoFingerSingleTap"},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP, "twoFingerDoubleTap"},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD, "twoFingerDoubleTapAndHold"},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP, "twoFingerTripleTap"},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD, "twoFingerTripleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_SINGLE_TAP, "threeFingerSingleTap"},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP, "threeFingerDoubleTap"},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD, "threeFingerDoubleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP, "threeFingerTripleTap"},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD, "threeFingerTripleTapAndHold"},
        {GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP, "fourFingerSingleTap"},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP, "fourFingerDoubleTap"},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD, "fourFingerDoubleTapAndHold"},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP, "fourFingerTripleTap"},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD, "fourFingerTripleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_UP, "threeFingerSwipeUp"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN, "threeFingerSwipeDown"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_LEFT, "threeFingerSwipeLeft"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_RIGHT, "threeFingerSwipeRight"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_UP, "fourFingerSwipeUp"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_DOWN, "fourFingerSwipeDown"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_LEFT, "fourFingerSwipeLeft"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_RIGHT, "fourFingerSwipeRight"}
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
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, "bounds"},
        {WindowUpdateType::WINDOW_UPDATE_PROPERTY, "property"},
        {WindowUpdateType::WINDOW_UPDATE_LAYER, "layer"}};

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
        {ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION, "setCursorPosition"},
        {ActionType::ACCESSIBILITY_ACTION_COMMON, "common"},
        {ActionType::ACCESSIBILITY_ACTION_SET_TEXT, "setText"},
        {ActionType::ACCESSIBILITY_ACTION_DELETED, "delete"},
        {ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK, "spanClick"},
        {ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM, "nextHtmlItem"},
        {ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM, "previousHtmlItem"}
    };

    if (triggerActionTable.find(type) == triggerActionTable.end()) {
        return "";
    }

    return triggerActionTable.at(type);
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
        {"pip", WindowUpdateType::WINDOW_UPDATE_PIP},
        {"property", WindowUpdateType::WINDOW_UPDATE_PROPERTY}};

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
        {"accessibilityFocusClear", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT},
        {"requestFocusForAccessibility", EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY},
        {"announceForAccessibility", EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY},
        {"announceForAccessibilityNotInterrupt", EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"requestFocusForAccessibilityNotInterrupt",
            EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"scrolling", EventType::TYPE_VIEW_SCROLLING_EVENT}};

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
        {"setCursorPosition", ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION},
        {"common", ActionType::ACCESSIBILITY_ACTION_COMMON},
        {"setText", ActionType::ACCESSIBILITY_ACTION_SET_TEXT},
        {"delete", ActionType::ACCESSIBILITY_ACTION_DELETED},
        {"home", ActionType::ACCESSIBILITY_ACTION_HOME},
        {"back", ActionType::ACCESSIBILITY_ACTION_BACK},
        {"recentTask", ActionType::ACCESSIBILITY_ACTION_RECENTTASK},
        {"notificationCenter", ActionType::ACCESSIBILITY_ACTION_NOTIFICATIONCENTER},
        {"controlCenter", ActionType::ACCESSIBILITY_ACTION_CONTROLCENTER},
        {"spanClick", ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK},
        {"nextHtmlItem", ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM},
        {"previousHtmlItem", ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM}};

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

TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type)
{
    static const std::map<const std::string, TextMoveUnit> textMoveUnitTable = {
        {"char", TextMoveUnit::STEP_CHARACTER},
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
    static const std::map<TextMoveUnit, const std::string> textMoveUnitTable = {
        {TextMoveUnit::STEP_CHARACTER, "char"},
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
            str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
            if (hasProperty) {
                args.insert(std::pair<std::string, std::string>("htmlItem", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT:
        case ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT:
            napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
            str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
            if (hasProperty) {
                args.insert(std::pair<std::string, std::string>("textMoveUnit", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_SELECTION:
            SetSelectionParam(env, object, args);
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION:
            napi_create_string_utf8(env, "offset", NAPI_AUTO_LENGTH, &propertyNameValue);
            str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
            CheckNumber(env, str);
            if (hasProperty) {
                args.insert(std::pair<std::string, std::string>("offset", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_TEXT:
            napi_create_string_utf8(env, "setText", NAPI_AUTO_LENGTH, &propertyNameValue);
            str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
            if (hasProperty) {
                args.insert(std::pair<std::string, std::string>("setText", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK:
            napi_create_string_utf8(env, "spanId", NAPI_AUTO_LENGTH, &propertyNameValue);
            str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
            CheckNumber(env, str);
            if (hasProperty) {
                args.insert(std::pair<std::string, std::string>("spanId", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD:
            SetScrollTypeParam(env, object, args);
            break;
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD:
            SetScrollTypeParam(env, object, args);
            break;
        default:
            break;
    }
}

void CheckNumber(napi_env env, std::string value)
{
    int num;
    std::stringstream streamStr;
    streamStr << value;
    if (!(streamStr >> num)) {
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
}

void SetSelectionParam(napi_env env, napi_value object, std::map<std::string, std::string>& args)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    std::string str = "";
    bool seleFlag = false;
    napi_create_string_utf8(env, "selectTextBegin", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    CheckNumber(env, str);
    if (hasProperty) {
        args.insert(std::pair<std::string, std::string>("selectTextBegin", str.c_str()));
    }
    napi_create_string_utf8(env, "selectTextEnd", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    CheckNumber(env, str);
    if (hasProperty) {
        args.insert(std::pair<std::string, std::string>("selectTextEnd", str.c_str()));
    }
    napi_create_string_utf8(env, "selectTextInForWard", NAPI_AUTO_LENGTH, &propertyNameValue);
    seleFlag = ConvertBoolJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        std::string value = seleFlag ? "forWard" : "backWard";
        args.insert(std::pair<std::string, std::string>("selectTextInForWard", value.c_str()));
    }
}

void SetScrollTypeParam(napi_env env, napi_value object, std::map<std::string, std::string>& args)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    std::string str = "";
    std::map<std::string, std::string> scrollValueMap = { {"halfScreen", HALF_VALUE}, {"fullScreen", FULL_VALUE} };
    std::string scrollValue = FULL_VALUE;

    napi_create_string_utf8(env, "scrolltype", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (!hasProperty) {
        napi_create_string_utf8(env, "scrollType", NAPI_AUTO_LENGTH, &propertyNameValue);
        str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    }
    if (hasProperty) {
        if (scrollValueMap.find(str) != scrollValueMap.end()) {
            scrollValue = scrollValueMap.find(str)->second;
            HILOG_DEBUG("ScrollValue %{public}s", scrollValue.c_str());
        } else {
            HILOG_DEBUG("Input is empty, throw error");
            napi_value err = CreateBusinessError(env, RetError::RET_ERR_INVALID_PARAM);
            napi_throw(env, err);
        }
        args.insert(std::pair<std::string, std::string>("scrolltype", scrollValue.c_str()));
    }
}

void SetPermCheckFlagForAction(bool checkPerm, std::map<std::string, std::string>& args)
{
    if (checkPerm) {
        args.insert(std::pair<std::string, std::string>("sysapi_check_perm", "1"));
    }
}

int32_t ConvertIntJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty)
{
    int32_t dataValue = 0;
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &itemValue);
        napi_get_value_int32(env, itemValue, &dataValue);
    }
    return dataValue;
}

bool ConvertBoolJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty)
{
    bool isBool = false;
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &itemValue);
        napi_get_value_bool(env, itemValue, &isBool);
    }
    return isBool;
}

std::string ConvertStringJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty)
{
    std::string str = "";
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &itemValue);
        str = GetStringFromNAPI(env, itemValue);
    }
    return str;
}

void ConvertStringArrayJSToNAPI(napi_env env, napi_value object,
    napi_value propertyNameValue, bool &hasProperty, std::vector<std::string> &stringArray)
{
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value contentsValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &contentsValue);
        napi_value data = nullptr;
        uint32_t dataLen = 0;
        napi_get_array_length(env, contentsValue, &dataLen);
        for (uint32_t i = 0; i < dataLen; i++) {
            napi_get_element(env, contentsValue, i, &data);
            std::string str = GetStringFromNAPI(env, data);
            stringArray.push_back(str);
        }
    }
}

void ConvertStringArrayJSToNAPICommon(napi_env env, napi_value object, std::vector<std::string> &stringArray)
{
    napi_value data = nullptr;
    uint32_t dataLen = 0;
    napi_get_array_length(env, object, &dataLen);
    for (uint32_t i = 0; i < dataLen; i++) {
        napi_get_element(env, object, i, &data);
        std::string str = GetStringFromNAPI(env, data);
        stringArray.push_back(str);
    }
}

void ConvertSpanToJS(napi_env env, napi_value result, const Accessibility::SpanInfo &span)
{
    napi_value spanId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, span.GetSpanId(), &spanId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "spanId", spanId));

    napi_value spanText;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, span.GetSpanText().c_str(), NAPI_AUTO_LENGTH, &spanText));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "spanText", spanText));

    napi_value accessibilityText;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, span.GetAccessibilityText().c_str(),
        NAPI_AUTO_LENGTH, &accessibilityText));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "accessibilityText", accessibilityText));

    napi_value accessibilityDescription;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, span.GetAccessibilityDescription().c_str(),
        NAPI_AUTO_LENGTH, &accessibilityDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "accessibilityDescription",
        accessibilityDescription));

    napi_value accessibilityLevel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, span.GetAccessibilityLevel().c_str(),
        NAPI_AUTO_LENGTH, &accessibilityLevel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "accessibilityLevel", accessibilityLevel));
}

bool ConvertEventInfoJSToNAPI(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo)
{
    HILOG_DEBUG();
    bool tmpResult = ConvertEventInfoJSToNAPIPart1(env, object, eventInfo);
    if (!tmpResult) {
        return false;
    }
    tmpResult = ConvertEventInfoJSToNAPIPart2(env, object, eventInfo);
    if (!tmpResult) {
        return false;
    }
    tmpResult = ConvertEventInfoJSToNAPIPart3(env, object, eventInfo);
    if (!tmpResult) {
        return false;
    }
    tmpResult = ConvertEventInfoJSToNAPIPart4(env, object, eventInfo);
    if (!tmpResult) {
        return false;
    }
    return true;
}

bool ConvertEventInfoJSToNAPIPart1(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo)
{
    bool hasProperty = false;
    std::string str = "";
    napi_value propertyNameValue = nullptr;
    napi_create_string_utf8(env, "type", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        EventType eventType = ConvertStringToEventInfoTypes(str);
        eventInfo.SetEventType(eventType);
        if (eventType == TYPE_VIEW_INVALID) {
            return false;
        }
    } else {
        return false;
    }

    napi_create_string_utf8(env, "windowUpdateType", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetEventType(TYPE_WINDOW_UPDATE);
        eventInfo.SetWindowChangeTypes(ConvertStringToWindowUpdateTypes(str));
    }

    napi_create_string_utf8(env, "bundleName", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        if (str != "") {
            eventInfo.SetBundleName(str);
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

bool ConvertEventInfoJSToNAPIPart2(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo)
{
    bool hasProperty = false;
    int32_t dataValue = 0;
    std::string str = "";
    napi_value propertyNameValue = nullptr;
    napi_create_string_utf8(env, "componentType", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetComponentType(str);
    }

    napi_create_string_utf8(env, "pageId", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetPageId(dataValue);
    }

    napi_create_string_utf8(env, "description", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetDescription(str);
    }

    napi_create_string_utf8(env, "triggerAction", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetTriggerAction(ConvertStringToAccessibleOperationType(str));
        if (eventInfo.GetTriggerAction() == ACCESSIBILITY_ACTION_INVALID) {
            return false;
        }
    } else {
        return false;
    }

    napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
    str = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetTextMovementStep(ConvertStringToTextMoveUnit(str));
    }

    napi_create_string_utf8(env, "elementId", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetRequestFocusElementId(dataValue);
    }
    return true;
}

bool ConvertEventInfoJSToNAPIPart3(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo)
{
    bool hasProperty = false;
    int32_t dataValue = 0;
    napi_value propertyNameValue = nullptr;
    napi_create_string_utf8(env, "contents", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::vector<std::string> stringArray {};
    ConvertStringArrayJSToNAPI(env, object, propertyNameValue, hasProperty, stringArray);
    if (hasProperty) {
        for (auto str : stringArray) {
            eventInfo.AddContent(str);
        }
    }

    napi_create_string_utf8(env, "lastContent", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::string strNapi = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetLatestContent(strNapi);
    }

    napi_create_string_utf8(env, "beginIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetBeginIndex(dataValue);
    }

    napi_create_string_utf8(env, "currentIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetCurrentIndex(dataValue);
    }

    napi_create_string_utf8(env, "endIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetEndIndex(dataValue);
    }

    napi_create_string_utf8(env, "itemCount", NAPI_AUTO_LENGTH, &propertyNameValue);
    dataValue = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetItemCounts(dataValue);
    }

    napi_create_string_utf8(env, "customId", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::string inspectorKey = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetInspectorKey(inspectorKey);
    }

    napi_create_string_utf8(env, "textAnnouncedForAccessibility", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::string announceText = ConvertStringJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        eventInfo.SetTextAnnouncedForAccessibility(announceText);
    }
    return true;
}

bool ConvertEventInfoJSToNAPIPart4(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo)
{
    bool hasProperty = false;
    napi_value propertyNameValue = nullptr;
    Accessibility::ResourceInfo resourceInfo;
    napi_create_string_utf8(env, "textResourceAnnouncedForAccessibility", NAPI_AUTO_LENGTH, &propertyNameValue);
    ConvertResourceJSToNAPI(env, object, propertyNameValue, hasProperty, resourceInfo);
    if (hasProperty) {
        eventInfo.SetResourceBundleName(resourceInfo.bundleName);
        eventInfo.SetResourceModuleName(resourceInfo.moduleName);
        eventInfo.SetResourceId(resourceInfo.resourceId);
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

bool ConvertGesturePathJSToNAPI(napi_env env, napi_value object,
    std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    if (!gesturePath) {
        HILOG_ERROR("gesturePath is null.");
        return false;
    }

    bool tmpResult = ConvertGesturePathJSToNAPIPart1(env, object, gesturePath);
    if (!tmpResult) {
        return false;
    }
    tmpResult = ConvertGesturePathJSToNAPIPart2(env, object, gesturePath);
    if (!tmpResult) {
        return false;
    }
    return true;
}

bool ConvertGesturePathJSToNAPIPart1(napi_env env, napi_value object,
    std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
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
            return false;
        }
        if (napi_get_array_length(env, positionValue, &dataLen) != napi_ok) {
            HILOG_ERROR("get array length failed.");
            return false;
        }
        for (uint32_t i = 0; i < dataLen; i++) {
            jsValue = nullptr;
            AccessibilityGesturePosition path;
            if (napi_get_element(env, positionValue, i, &jsValue) != napi_ok) {
                HILOG_ERROR("get element of paths failed and i = %{public}d", i);
                return false;
            }
            bool result = ConvertGesturePointJSToNAPI(env, jsValue, path);
            if (result) {
                gesturePath->AddPosition(path);
            } else {
                HILOG_ERROR("Parse gesture point error.");
                return false;
            }
        }
    } else {
        HILOG_ERROR("No points property.");
        return false;
    }
    return true;
}

bool ConvertGesturePathJSToNAPIPart2(napi_env env, napi_value object,
    std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;

    napi_create_string_utf8(env, "durationTime", NAPI_AUTO_LENGTH, &propertyNameValue);
    int64_t durationTime = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        gesturePath->SetDurationTime(durationTime);
        return true;
    }
    return false;
}

KeyAction TransformKeyActionValue(int32_t keyAction)
{
    HILOG_DEBUG("keyAction:%{public}d", keyAction);

    KeyAction action = KeyAction::UNKNOWN;
    if (keyAction == OHOS::MMI::KeyEvent::KEY_ACTION_DOWN) {
        action = KeyAction::DOWN;
    } else if (keyAction == OHOS::MMI::KeyEvent::KEY_ACTION_UP) {
        action = KeyAction::UP;
    } else if (keyAction == OHOS::MMI::KeyEvent::KEY_ACTION_CANCEL) {
        action = KeyAction::CANCEL;
    } else {
        HILOG_DEBUG("key action is invalid");
    }
    return action;
}

bool HasKeyCode(const std::vector<int32_t>& pressedKeys, int32_t keyCode)
{
    HILOG_DEBUG();

    return std::find(pressedKeys.begin(), pressedKeys.end(), keyCode) != pressedKeys.end();
}

void GetKeyValue(napi_env env, napi_value keyObject, std::optional<MMI::KeyEvent::KeyItem> keyItem)
{
    HILOG_DEBUG();

    if (!keyItem) {
        HILOG_WARN("keyItem is null.");
        return;
    }

    napi_value keyCodeValue = nullptr;
    int32_t keyCode = keyItem->GetKeyCode();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, keyCode, &keyCodeValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyObject, "code", keyCodeValue));

    napi_value timeValue = nullptr;
    int64_t pressedTime = keyItem->GetDownTime();
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, pressedTime, &timeValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyObject, "pressedTime", timeValue));

    napi_value deviceIdValue = nullptr;
    int32_t deviceId = keyItem->GetDeviceId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, deviceId, &deviceIdValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, keyObject, "deviceId", deviceIdValue));
}

void SetInputEventProperty(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent)
{
    HILOG_DEBUG();

    if (!keyEvent) {
        HILOG_ERROR("keyEvent is null.");
        return;
    }
    // set id
    napi_value idValue = nullptr;
    int32_t id = keyEvent->GetId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, id, &idValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "id", idValue));

    // set deviceId
    napi_value deviceIdValue = nullptr;
    int32_t deviceId = keyEvent->GetDeviceId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, deviceId, &deviceIdValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deviceId", deviceIdValue));

    // set actionTime
    napi_value actionTimeValue = nullptr;
    int64_t actionTime = keyEvent->GetActionTime();
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, actionTime, &actionTimeValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "actionTime", actionTimeValue));

    // set screenId
    napi_value screenIdValue = nullptr;
    int32_t screenId = keyEvent->GetTargetDisplayId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, screenId, &screenIdValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "screenId", screenIdValue));

    // set windowId
    napi_value windowIdValue = nullptr;
    int32_t windowId = keyEvent->GetTargetWindowId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, windowId, &windowIdValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "windowId", windowIdValue));
}

void ConvertKeyEventToJS(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent)
{
    HILOG_DEBUG();

    if (!keyEvent) {
        HILOG_ERROR("keyEvent is null.");
        return;
    }

    // set inputEvent
    SetInputEventProperty(env, result, keyEvent);

    // set action
    napi_value keyActionValue = nullptr;
    KeyAction keyAction = TransformKeyActionValue(keyEvent->GetKeyAction());
    if (keyAction != KeyAction::UNKNOWN) {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, keyAction, &keyActionValue));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "action", keyActionValue));
    }

    // set key
    napi_value keyObject = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &keyObject));
    std::optional<MMI::KeyEvent::KeyItem> keyItem = keyEvent->GetKeyItem();
    GetKeyValue(env, keyObject, keyItem);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "key", keyObject));

    // set unicodeChar
    napi_value unicodeCharValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 0, &unicodeCharValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "unicodeChar", unicodeCharValue));

    // set keys
    SetKeyPropertyPart1(env, result, keyEvent);
    SetKeyPropertyPart2(env, result, keyEvent);
}

void ConvertResourceJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty,
    Accessibility::ResourceInfo& resourceInfo)
{
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value itemValue = nullptr;
        napi_get_property(env, object, propertyNameValue, &itemValue);
        resourceInfo.resourceId = ParseResourceIdFromNAPI(env, itemValue);
        resourceInfo.bundleName = ParseResourceBundleNameFromNAPI(env, itemValue);
        resourceInfo.moduleName = ParseResourceModuleNameFromNAPI(env, itemValue);
    }
    HILOG_DEBUG("resourceId is %{public}d, bundleName is %{public}s, moduleName is %{public}s",
        resourceInfo.resourceId, resourceInfo.bundleName.c_str(), resourceInfo.moduleName.c_str());
}

void SetKeyPropertyPart1(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent)
{
    HILOG_DEBUG();
    if (!keyEvent) {
        HILOG_ERROR("keyEvent is nullptr.");
        return;
    }
    // set keys
    napi_value keysAarry = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &keysAarry));
    uint32_t index = 0;
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    for (const auto &pressedKeyCode : pressedKeys) {
        napi_value element = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &element));
        std::optional<MMI::KeyEvent::KeyItem> pressedKeyItem = keyEvent->GetKeyItem(pressedKeyCode);
        GetKeyValue(env, element, pressedKeyItem);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, keysAarry, index, element));
        ++index;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "keys", keysAarry));

    // set ctrlKey
    bool isPressed = HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT)
        || HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_CTRL_RIGHT);
    napi_value ctrlKeyValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, isPressed, &ctrlKeyValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "ctrlKey", ctrlKeyValue));

    // set altKey
    isPressed = HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT)
        || HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_ALT_RIGHT);
    napi_value altKeyValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, isPressed, &altKeyValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "altKey", altKeyValue));

    // set shiftKey
    isPressed = HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT)
        || HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    napi_value shiftKeyValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, isPressed, &shiftKeyValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "shiftKey", shiftKeyValue));

    // set logoKey
    isPressed = HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_META_LEFT)
        || HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_META_RIGHT);
    napi_value logoKeyValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, isPressed, &logoKeyValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "logoKey", logoKeyValue));

    // set fnKey
    isPressed = HasKeyCode(pressedKeys, OHOS::MMI::KeyEvent::KEYCODE_FN);
    napi_value fnKeyValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, isPressed, &fnKeyValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fnKey", fnKeyValue));
}

void SetKeyPropertyPart2(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent)
{
    HILOG_DEBUG();
    // set capsLock
    napi_value capsLockValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, false, &capsLockValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "capsLock", capsLockValue));

    // set numLock
    napi_value numLockValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, false, &numLockValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "numLock", numLockValue));

    // set scrollLock
    napi_value scrollLockValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, false, &scrollLockValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "scrollLock", scrollLockValue));
}

void ConvertCaptionPropertyToJS(
    napi_env env, napi_value& result, OHOS::AccessibilityConfig::CaptionProperty captionProperty)
{
    HILOG_DEBUG();

    napi_value value = nullptr;

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

    if (ColorRegexMatch(colorStr, color)) {
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

bool IsColorWithMagic(const std::string& colorStr)
{
    if (colorStr.size() < 1 || colorStr.substr(0, 1) != "#") {
        return false;
    }

    for (int i = 1; i < colorStr.size(); i++) {
        if (NUMBER_VALID_CHARS.find(colorStr[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool ColorRegexMatch(std::string colorStr, uint32_t &color)
{
    // for example #909090 or #90909090. avoid use regex match #[0-9A-Fa-f]{6,8}.
    if (IsColorWithMagic(colorStr) &&
        (colorStr.size() == COLOR_STRING_SIZE_7 || colorStr.size() == COLOR_STRING_SIZE_9)) {
        colorStr.erase(0, 1);
        auto colorValue = stoul(colorStr, nullptr, COLOR_STRING_BASE);
        if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
            // No alpha specified, set alpha to 0xff
            colorValue |= COLOR_ALPHA_MASK;
        } else {
            auto alpha = colorValue << ALPHA_MOVE;
            auto rgb = colorValue >> COLOR_MOVE;
            colorValue = alpha | rgb;
        }
        color = colorValue;
        return true;
    }
    // for #rgb or #rgba. avoid use regex match #[0-9A-Fa-f]{3,4}.
    if (IsColorWithMagic(colorStr) &&
        (colorStr.size() == COLOR_STRING_SIZE_4 || colorStr.size() == COLOR_STRING_SIZE_5)) {
        colorStr.erase(0, 1);
        std::string newColorStr;
        // Translate #rgb or #rgba to #rrggbb or #rrggbbaa
        for (const auto& c : colorStr) {
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
        return true;
    }
    return false;
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
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        color = ConvertColorStringToNumer(std::string(outBuffer));
    }
    HILOG_DEBUG("color is 0x%{public}x", color);
    return color;
}

uint32_t GetColorValue(napi_env env, napi_value value)
{
    uint32_t color = COLOR_TRANSPARENT;
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
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
        color = ConvertColorStringToNumer(std::string(outBuffer));
    }
    HILOG_DEBUG("color is 0x%{public}x", color);
    return color;
}

bool ConvertObjToCaptionProperty(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty)
{
    if (!ptrCaptionProperty) {
        HILOG_ERROR("ptrCaptionProperty is null.");
        return false;
    }

    bool tmpResult = ConvertObjToCaptionPropertyPart1(env, object, ptrCaptionProperty);
    if (!tmpResult) {
        return false;
    }
    tmpResult = ConvertObjToCaptionPropertyPart2(env, object, ptrCaptionProperty);
    if (!tmpResult) {
        return false;
    }
    return true;
}

bool ConvertObjToCaptionPropertyPart1(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    int32_t num = 100;

    napi_create_string_utf8(env, "fontFamily", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::string fontFamily = ConvertCaptionPropertyJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        ptrCaptionProperty->SetFontFamily(fontFamily);
    } else {
        return false;
    }

    napi_create_string_utf8(env, "fontScale", NAPI_AUTO_LENGTH, &propertyNameValue);
    num = ConvertIntJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
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
    return true;
}

bool ConvertObjToCaptionPropertyPart2(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;

    napi_create_string_utf8(env, "fontEdgeType", NAPI_AUTO_LENGTH, &propertyNameValue);
    std::string fontEdgeType = ConvertCaptionPropertyJSToNAPI(env, object, propertyNameValue, hasProperty);
    if (hasProperty) {
        ptrCaptionProperty->SetFontEdgeType(fontEdgeType);
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

std::string ConvertCaptionPropertyJSToNAPI(napi_env env, napi_value object,
    napi_value propertyNameValue, bool &hasProperty)
{
    char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        size_t outSize = 0;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
    }
    return std::string(outBuffer);
}

bool ConvertJSToStringVec(napi_env env, napi_value arrayValue, std::vector<std::string>& values)
{
    HILOG_DEBUG();
    values.clear();

    bool hasElement = true;
    for (int32_t i = 0; hasElement; i++) {
        napi_has_element(env, arrayValue, i, &hasElement);
        if (hasElement) {
            napi_value value = nullptr;
            napi_status status = napi_get_element(env, arrayValue, i, &value);
            if (status != napi_ok) {
                return false;
            }

            char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
            size_t outSize = 0;
            status = napi_get_value_string_utf8(env, value, outBuffer, CHAE_BUFFER_MAX, &outSize);
            if (status != napi_ok) {
                return false;
            }

            values.push_back(std::string(outBuffer));
        }
    }
    return true;
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

bool ConvertJSToCapabilities(napi_env env, napi_value arrayValue, uint32_t &capabilities)
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
            return false;
        }
        capabilities |= capability;
    }
    return true;
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

void ConvertInt64VecToJS(napi_env env, napi_value &result, std::vector<std::int64_t> values)
{
    HILOG_DEBUG();
    size_t index = 0;
    for (auto& value : values) {
        napi_value id = nullptr;
        napi_create_int64(env, value, &id);
        napi_set_element(env, result, index, id);
        index++;
    }
}

bool ConvertStringToInt64(std::string &str, int64_t &value)
{
    auto [ptr, errCode] = std::from_chars(str.data(), str.data() + str.size(), value);
    return errCode == std::errc{} && ptr == str.data() + str.size();
}
} // namespace AccessibilityNapi
} // namespace OHOS
