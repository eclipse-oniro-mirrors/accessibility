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

#include <ani.h>
#include <array>
#include <iostream>
#include <vector>
#include "hilog_wrapper.h"
#include "ani_utils.h"

using namespace OHOS::Accessibility;

namespace {
    const std::string ERROR_MESSAGE_PARAMETER_ERROR = "Parameter error. Possible causes:"
        "1. Mandatory parameters are left unspecified; 2. Incorrect parameter types; 3. Parameter verification failed.";
    const std::string ERROR_MESSAGE_NO_PERMISSION = "Permission verification failed."
        "The application does not have the permission required to call the API.";
    const std::string ERROR_MESSAGE_NOT_SYSTEM_APP = "Permission verification failed."
        "A non-system application calls a system API.";
    const std::string ERROR_MESSAGE_NO_RIGHT = "No accessibility permission to perform the operation";
    const std::string ERROR_MESSAGE_SYSTEM_ABNORMALITY = "System abnormality";
    const std::string ERROR_MESSAGE_PROPERTY_NOT_EXIST = "This property does not exist";
    const std::string ERROR_MESSAGE_ACTION_NOT_SUPPORT = "This action is not supported";
    const std::string ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME = "Invalid bundle name or ability name";
    const std::string ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED = "Target ability already enabled";
} // namespace

std::string ANIUtils::ANIStringToStdString(ani_env *env, ani_string ani_str)
{
    ani_size strSize;
    env->String_GetUTF8Size(ani_str, &strSize);
   
    std::vector<char> buffer(strSize + 1); // +1 for null terminator
    char* utf8_buffer = buffer.data();

    ani_size bytes_written = 0;
    env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written);
    
    utf8_buffer[bytes_written] = '\0';
    std::string content = std::string(utf8_buffer);
    return content;
}

bool ANIUtils::GetStringField(ani_env *env, std::string fieldName, ani_object object, std::string &fieldValue)
{
    ani_ref ref;
    if (env->Object_GetFieldByName_Ref(object, fieldName.c_str(), &ref) != ANI_OK) {
        HILOG_ERROR("get field %{public}s failed", fieldName.c_str());
        return false;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(ref, &isUndefined) != ANI_OK) {
        HILOG_ERROR("get field %{public}s undefined failed", fieldName.c_str());
        return false;
    }
    if (!isUndefined) {
        fieldValue = ANIStringToStdString(env, static_cast<ani_string>(ref));
        return true;
    }
    return false;
}

bool ANIUtils::GetIntField(ani_env *env, std::string fieldName, ani_object object, int32_t &fieldValue)
{
    ani_ref ref;
    if (env->Object_GetFieldByName_Ref(object, fieldName.c_str(), &ref) != ANI_OK) {
        HILOG_ERROR("get field %{public}s failed", fieldName.c_str());
        return false;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(ref, &isUndefined) != ANI_OK) {
        HILOG_ERROR("get field %{public}s undefined failed", fieldName.c_str());
        return false;
    }
    if (!isUndefined) {
        if (env->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "intValue", nullptr, &fieldValue) ==
            ANI_OK) {
            return true;
        }
    }
    return false;
}

bool ANIUtils::GetArrayStringField(ani_env *env, std::string fieldName, ani_object object,
    std::vector<std::string> &fieldValue)
{
    ani_ref ref;
    if (env->Object_GetFieldByName_Ref(object, fieldName.c_str(), &ref) != ANI_OK) {
        HILOG_ERROR("get field %{public}s failed", fieldName.c_str());
        return false;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(ref, &isUndefined) != ANI_OK) {
        HILOG_ERROR("get field %{public}s undefined failed", fieldName.c_str());
        return false;
    }
    if (isUndefined) {
        return false;
    }

    fieldValue.clear();
    ani_class arrayCls;
    const char *arrayClassName = "Lescompat/Array;";
    if (env->FindClass(arrayClassName, &arrayCls) != ANI_OK) {
        return false;
    }

    ani_method arrayLengthMethod;
    if (env->Class_FindMethod(arrayCls, "length", ":Lstd/core/Object;", &arrayLengthMethod) != ANI_OK) {
        return false;
    }

    ani_ref length;
    if (env->Object_CallMethod_Ref(static_cast<ani_object>(ref), arrayLengthMethod, &length) != ANI_OK) {
        return false;
    }

    int32_t lengthInt;
    if (env->Object_CallMethodByName_Int(static_cast<ani_object>(length), "intValue", nullptr, &lengthInt) != ANI_OK ||
        lengthInt <= 0) {
        return false;
    }

    ani_method arrayPopMethod;
    if (env->Class_FindMethod(arrayCls, "pop", ":Lstd/core/Object;", &arrayPopMethod) != ANI_OK) {
        return false;
    }

    ani_ref string;
    std::string result;
    for (int32_t i = 0; i < lengthInt; i++) {
        if (env->Object_CallMethod_Ref(static_cast<ani_object>(ref), arrayPopMethod, &string) != ANI_OK) {
            return false;
        }
        result = ANIStringToStdString(env, static_cast<ani_string>(string));
        fieldValue.insert(fieldValue.begin(), result);
    }

    return true;
}

bool ANIUtils::CheckObserverEqual(ani_env *env, ani_ref fnRef, ani_env *iterEnv, ani_ref iterFn)
{
    if (env != iterEnv) {
        HILOG_DEBUG("not the same env");
        return false;
    }
    ani_boolean isEquals = false;
    if (env->Reference_StrictEquals(fnRef, iterFn, &isEquals) != ANI_OK) {
        HILOG_ERROR("check observer equal failed!");
        return false;
    }
    return isEquals;
}

EventType ANIUtils::ConvertStringToEventInfoTypes(const std::string &type)
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
        {"announceForAccessibility", EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY}};

    if (eventInfoTypesTable.find(type) == eventInfoTypesTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return TYPE_VIEW_INVALID;
    }

    return eventInfoTypesTable.at(type);
}

ActionType ANIUtils::ConvertStringToAccessibleOperationType(const std::string &type)
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
        {"spanClick", ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK}};

    if (accessibleOperationTypeTable.find(type) == accessibleOperationTypeTable.end()) {
        HILOG_WARN("invalid key[%{public}s]", type.c_str());
        return ACCESSIBILITY_ACTION_INVALID;
    }

    return accessibleOperationTypeTable.at(type);
}

WindowUpdateType ANIUtils::ConvertStringToWindowUpdateTypes(const std::string &type)
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

TextMoveUnit ANIUtils::ConvertStringToTextMoveUnit(const std::string &type)
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

NAccessibilityErrMsg ANIUtils::QueryRetMsg(RetError errorCode)
{
    switch (errorCode) {
        case RetError::RET_OK:
            return { NAccessibilityErrorCode::ACCESSIBILITY_OK, "" };
        case RetError::RET_ERR_FAILED:
        case RetError::RET_ERR_NULLPTR:
        case RetError::RET_ERR_IPC_FAILED:
        case RetError::RET_ERR_SAMGR:
        case RetError::RET_ERR_TIME_OUT:
        case RetError::RET_ERR_REGISTER_EXIST:
        case RetError::RET_ERR_NO_REGISTER:
        case RetError::RET_ERR_NO_CONNECTION:
        case RetError::RET_ERR_NO_WINDOW_CONNECTION:
        case RetError::RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE:
        case RetError::RET_ERR_PERFORM_ACTION_FAILED_BY_ACE:
        case RetError::RET_ERR_NO_INJECTOR:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
        case RetError::RET_ERR_INVALID_PARAM:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM, ERROR_MESSAGE_PARAMETER_ERROR };
        case RetError::RET_ERR_NO_PERMISSION:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_PERMISSION, ERROR_MESSAGE_NO_PERMISSION };
        case RetError::RET_ERR_CONNECTION_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_TARGET_ABILITY_ALREADY_ENABLED,
                     ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED };
        case RetError::RET_ERR_NO_CAPABILITY:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_RIGHT, ERROR_MESSAGE_NO_RIGHT };
        case RetError::RET_ERR_NOT_INSTALLED:
        case RetError::RET_ERR_NOT_ENABLED:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME,
                     ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME};
        case RetError::RET_ERR_PROPERTY_NOT_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_PROPERTY_NOT_EXIST,
                     ERROR_MESSAGE_PROPERTY_NOT_EXIST };
        case RetError::RET_ERR_ACTION_NOT_SUPPORT:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ACTION_NOT_SUPPORT,
                     ERROR_MESSAGE_ACTION_NOT_SUPPORT };
        case RetError::RET_ERR_NOT_SYSTEM_APP:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NOT_SYSTEM_APP,
                     ERROR_MESSAGE_NOT_SYSTEM_APP };
        default:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
    }
}

void ANIUtils::ThrowBusinessError(ani_env *env, NAccessibilityErrMsg errMsg)
{
    static const char *errorClsName = "L@ohos/base/BusinessError;";
    ani_class cls {};
    if (env->FindClass(errorClsName, &cls) != ANI_OK) {
        HILOG_ERROR("find class BusinessError failed");
        return;
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", ":V", &ctor) != ANI_OK) {
        HILOG_ERROR("find method BusinessError.constructor failed");
        return;
    }
    ani_double errCode = static_cast<ani_double>(errMsg.errCode);
    ani_string errMsgStr;
    env->String_NewUTF8(errMsg.message.c_str(), errMsg.message.length(), &errMsgStr);
    ani_object errorObject;
    if (env->Object_New(cls, ctor, &errorObject) != ANI_OK) {
        HILOG_ERROR("create BusinessError object failed");
        return;
    }
    if (env->Object_SetPropertyByName_Double(errorObject, "code", errCode) != ANI_OK) {
        HILOG_ERROR("set property BusinessError.code failed!");
        return;
    }
    if (env->Object_SetPropertyByName_Ref(errorObject, "message", static_cast<ani_ref>(errMsgStr)) != ANI_OK) {
        HILOG_ERROR("set property BusinessError.message failed!");
        return;
    }
    if (env->ThrowError(static_cast<ani_error>(errorObject)) != ANI_OK) {
        HILOG_ERROR("throw BusinessError failed!");
        return;
    }
    HILOG_INFO("throw BusinessError success!");
    return;
}

ani_object ANIUtils::CreateBoolObject(ani_env *env, ani_boolean value)
{
    static const char *boolClsName = "Lstd/core/Boolean;";
    ani_class cls {};
    if (env->FindClass(boolClsName, &cls) != ANI_OK) {
        HILOG_ERROR("find class Boolean failed");
        return nullptr;
    }

    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", "Z:V", &ctor) != ANI_OK) {
        HILOG_ERROR("find method Boolean.constructor failed");
        return nullptr;
    }

    ani_object boolObject;
    if (env->Object_New(cls, ctor, &boolObject, value) != ANI_OK) {
        HILOG_ERROR("create Boolean object failed");
        return nullptr;
    }
    return boolObject;
}

bool ANIUtils::ConvertEventInfoMandatoryFields(ani_env *env, ani_object eventObject,
    AccessibilityEventInfo &eventInfo)
{
    std::string type;
    if (!GetStringField(env, "type", eventObject, type)) {
        HILOG_ERROR("get type Faild!");
        return false;
    }
    OHOS::Accessibility::EventType eventType = ConvertStringToEventInfoTypes(type);
    if (eventType == TYPE_VIEW_INVALID) {
        HILOG_ERROR("event type is invalid!");
        return false;
    }
    eventInfo.SetEventType(eventType);

    std::string bundleName;
    if (!GetStringField(env, "bundleName", eventObject, bundleName)) {
        HILOG_ERROR("get bundleName Faild!");
        return false;
    }
    if (bundleName == "") {
        HILOG_ERROR("bundle name is invalid!");
        return false;
    }
    eventInfo.SetBundleName(bundleName);

    std::string triggerAction;
    if (!GetStringField(env, "triggerAction", eventObject, triggerAction)) {
        HILOG_ERROR("get triggerAction Faild!");
        return false;
    }
    OHOS::Accessibility::ActionType action = ConvertStringToAccessibleOperationType(triggerAction);
    if (action == ACCESSIBILITY_ACTION_INVALID) {
        HILOG_ERROR("action is invalid!");
        return false;
    }
    eventInfo.SetTriggerAction(action);

    return true;
}

void ANIUtils::ConvertEventInfoStringFields(ani_env *env, ani_object eventObject,
    OHOS::Accessibility::AccessibilityEventInfo &eventInfo)
{
    std::string windowUpdateType;
    if (GetStringField(env, "windowUpdateType", eventObject, windowUpdateType)) {
        eventInfo.SetEventType(TYPE_WINDOW_UPDATE);
        eventInfo.SetWindowChangeTypes(ConvertStringToWindowUpdateTypes(windowUpdateType));
    }

    std::string componentType;
    if (GetStringField(env, "componentType", eventObject, componentType)) {
        eventInfo.SetComponentType(componentType);
    }

    std::string description;
    if (GetStringField(env, "description", eventObject, description)) {
        eventInfo.SetDescription(description);
    }

    std::string textMoveUnit;
    if (GetStringField(env, "textMoveUnit", eventObject, textMoveUnit)) {
        eventInfo.SetTextMovementStep(ConvertStringToTextMoveUnit(textMoveUnit));
    }

    std::vector<std::string> contents;
    if (GetArrayStringField(env, "contents", eventObject, contents)) {
        for (auto str : contents) {
            eventInfo.AddContent(str);
        }
    }

    std::string lastContent;
    if (GetStringField(env, "lastContent", eventObject, lastContent)) {
        eventInfo.SetLatestContent(lastContent);
    }

    std::string textAnnouncedForAccessibility;
    if (GetStringField(env, "textAnnouncedForAccessibility", eventObject, textAnnouncedForAccessibility)) {
        eventInfo.SetTextAnnouncedForAccessibility(textAnnouncedForAccessibility);
    }

    std::string customId;
    if (GetStringField(env, "customId", eventObject, customId)) {
        eventInfo.SetInspectorKey(customId);
    }
}

void ANIUtils::ConvertEventInfoIntFields(ani_env *env, ani_object eventObject,
    OHOS::Accessibility::AccessibilityEventInfo &eventInfo)
{
    int32_t pageId;
    if (GetIntField(env, "pageId", eventObject, pageId)) {
        eventInfo.SetPageId(pageId);
    }

    int32_t beginIndex;
    if (GetIntField(env, "beginIndex", eventObject, beginIndex)) {
        eventInfo.SetBeginIndex(beginIndex);
    }

    int32_t currentIndex;
    if (GetIntField(env, "currentIndex", eventObject, currentIndex)) {
        eventInfo.SetCurrentIndex(currentIndex);
    }

    int32_t endIndex;
    if (GetIntField(env, "endIndex", eventObject, endIndex)) {
        eventInfo.SetEndIndex(endIndex);
    }

    int32_t itemCount;
    if (GetIntField(env, "itemCount", eventObject, itemCount)) {
        eventInfo.SetItemCounts(itemCount);
    }

    int32_t elementId;
    if (GetIntField(env, "elementId", eventObject, elementId)) {
        eventInfo.SetRequestFocusElementId(elementId);
    }
}

bool ANIUtils::SendEventToMainThread(const std::function<void()> func)
{
    if (func == nullptr) {
        HILOG_ERROR("func is nullptr!");
        return false;
    }

    if (!mainHandler) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            HILOG_ERROR("get main event runner failed!");
            return false;
        }
        mainHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    mainHandler->PostTask(func, "", 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    return true;
}