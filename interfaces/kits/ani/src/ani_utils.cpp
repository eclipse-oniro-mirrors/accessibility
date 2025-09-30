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
#include <sstream>
#include <vector>
#include <charconv>
#include "hilog_wrapper.h"
#include "ani_utils.h"
#include <ani_signature_builder.h>

using namespace OHOS::Accessibility;
using namespace arkts::ani_signature;

constexpr const char* CLASSNAME_DOUBLE = "std.core.Double";
const std::string HALF_VALUE = "0";
const std::string FULL_VALUE = "1";

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

ani_string ANIUtils::StdStringToAniString(ani_env *env, std::string str)
{
    ani_string result_string{};
    env->String_NewUTF8(str.c_str(), str.size(), &result_string);
    return result_string;
}

bool ANIUtils::GetStringField(ani_env *env, std::string fieldName, ani_object object, std::string &fieldValue,
    bool isProperty)
{
    ani_ref ref;
    if (isProperty) {
        if (env->Object_GetPropertyByName_Ref(object, fieldName.c_str(), &ref) != ANI_OK) {
            HILOG_ERROR("get field %{public}s failed", fieldName.c_str());
            return false;
        }
        fieldValue = ANIStringToStdString(env, static_cast<ani_string>(ref));
        return true;
    }

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
        if (env->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "toInt", nullptr, &fieldValue) ==
            ANI_OK) {
            return true;
        }
    }
    return false;
}

bool ANIUtils::GetLongProperty(ani_env *env, std::string fieldName, ani_object object, int64_t &fieldValue)
{
    ani_long longValue;
    if (env->Object_GetPropertyByName_Long(object, fieldName.c_str(), &longValue) != ANI_OK) {
        HILOG_ERROR("get field %{public}s failed", fieldName.c_str());
        return false;
    }
    fieldValue = static_cast<int64_t>(longValue);
    return true;
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
    if (!ParseStringArray(env, static_cast<ani_object>(ref), fieldValue)) {
        HILOG_INFO(" ParseStringArray fail");
        return false;
    }
    return true;
}

bool ANIUtils::SetLongPropertyRef(ani_env *env, ani_object &object, const std::string &propName, int64_t propValue)
{
    if (env == nullptr) {
        HILOG_ERROR("SetLongField null env");
        return false;
    }
    ani_object value;
    if (CreateAniLong(env, propValue, value) != ANI_OK) {
        HILOG_ERROR("CreateAniLong failed");
        return false;
    }
    ani_status status;
    if ((status = env->Object_SetPropertyByName_Ref(object, propName.c_str(), value)) != ANI_OK) {
        HILOG_ERROR("set long field failed %{public}s %{public}d", propName.c_str(), status);
        return false;
    }
    return true;
}

bool ANIUtils::SetStringProperty(ani_env *env, ani_object &param, const std::string &name, const std::string &value)
{
    if (env == nullptr) {
        HILOG_ERROR("SetStringProperty null env");
        return false;
    }
    ani_string string = nullptr;
    ani_status status;
    if (value.empty()) {
        ani_ref nullRef = nullptr;
        if ((status = env->GetNull(&nullRef)) != ANI_OK) {
            HILOG_ERROR("get null status : %{public}d", status);
            return false;
        }
        if ((status = env->Object_SetPropertyByName_Ref(param, name.c_str(), nullRef)) != ANI_OK) {
            HILOG_ERROR("set null status : %{public}d", status);
            return false;
        }
        return true;
    }
    if ((status = env->String_NewUTF8(value.c_str(), value.size(), &string)) != ANI_OK) {
        HILOG_ERROR("change to ani string status : %{public}d", status);
        return false;
    }
    if ((status = env->Object_SetPropertyByName_Ref(param, name.c_str(), string)) != ANI_OK) {
        HILOG_ERROR("set ref status : %{public}d", status);
        return false;
    }
    return true;
}

bool ANIUtils::SetStringArrayProperty(ani_env *env, ani_object &object, const std::string &name,
    const std::vector<std::string> &values)
{
    ani_array arrayObj = CreateEmptyAniArray(env, values.size());
    if (arrayObj == nullptr) {
        HILOG_ERROR("SetStringArrayProperty array is null");
        return false;
    }
    ani_size index = 0;
    for (auto &str : values) {
        if (ANI_OK != env->Array_Set(arrayObj, index, StdStringToAniString(env, str))) {
            HILOG_ERROR("Object_CallMethodByName_Void failed name=%{public}s index=%{public}zu str=%{public}s",
                name.c_str(), index, str.c_str());
            return false;
        }
        index++;
    }
    ani_status status = env->Object_SetPropertyByName_Ref(object, name.c_str(), arrayObj);
    if (status != ANI_OK) {
        HILOG_ERROR("status : %{public}d", status);
        return false;
    }

    return true;
}

ani_array ANIUtils::CreateEmptyAniArray(ani_env *env, uint32_t size)
{
    if (env == nullptr) {
        HILOG_ERROR("create ani array env is null");
        return nullptr;
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(size, undefinedRef, &resultArray);
    return resultArray;
}

bool ANIUtils::GetEnumInt(ani_env *env, const char *enum_descriptor, ani_int enumIndex, int32_t& fieldValue)
{
    HILOG_INFO("processEnumInt enum=%{public}s, index=%{public}d", enum_descriptor, enumIndex);
    ani_enum enumType;
    if (ANI_OK != env->FindEnum(enum_descriptor, &enumType)) {
        HILOG_ERROR("Find Enum Faild");
        return false;
    }

    ani_enum_item enumItem;
    if (ANI_OK != env->Enum_GetEnumItemByIndex(enumType, enumIndex, &enumItem)) {
        HILOG_ERROR("Enum_GetEnumItemByIndex FAILD");
        return false;
    }

    if (ANI_OK != env->EnumItem_GetValue_Int(enumItem, &fieldValue)) {
        HILOG_ERROR("Enum_GetEnumItemByIndex FAILD");
        return false;
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
    Type errorClass = Builder::BuildClass("@ohos.base.BusinessError");
    ani_class cls {};
    if (env->FindClass(errorClass.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("find class BusinessError failed");
        return;
    }
    ani_method ctor;
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    if (env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor) != ANI_OK) {
        HILOG_ERROR("find method BusinessError.constructor failed");
        return;
    }
    ani_int errCode = static_cast<ani_int>(errMsg.errCode);
    ani_string errMsgStr;
    env->String_NewUTF8(errMsg.message.c_str(), errMsg.message.length(), &errMsgStr);
    ani_object errorObject;
    if (env->Object_New(cls, ctor, &errorObject) != ANI_OK) {
        HILOG_ERROR("create BusinessError object failed");
        return;
    }
    if (env->Object_SetPropertyByName_Int(errorObject, "code", errCode) != ANI_OK) {
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

ani_string ANIUtils::CreateAniString(ani_env *env, const std::string &str)
{
    ani_string aniString {};
    if (env != nullptr) {
        env->String_NewUTF8(str.c_str(), str.size(), &aniString);
    }
    return aniString;
}

ani_object ANIUtils::CreateBoolObject(ani_env *env, ani_boolean value)
{
    Type boolClass = Builder::BuildClass("std.core.Boolean");
    ani_class cls {};
    if (env->FindClass(boolClass.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("find class Boolean failed");
        return nullptr;
    }

    ani_method ctor;
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    sb.AddBoolean();
    if (env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor) != ANI_OK) {
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

    std::string triggerAction;
    if (GetStringField(env, "triggerAction", eventObject, triggerAction)) {
        eventInfo.SetTriggerAction(ConvertStringToAccessibleOperationType(triggerAction));
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

void ANIUtils::ConvertEventInfoRefFields(ani_env *env, ani_object eventObject,
    OHOS::Accessibility::AccessibilityEventInfo &eventInfo)
{
    ani_ref ref;
    if (env->Object_GetFieldByName_Ref(eventObject, "textResourceAnnouncedForAccessibility", &ref) != ANI_OK) {
        HILOG_ERROR("get field textResourceAnnouncedForAccessibility failed");
        return;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(ref, &isUndefined) != ANI_OK) {
        HILOG_ERROR("get field textResourceAnnouncedForAccessibility undefined failed");
        return;
    }
    if (isUndefined) {
        return;
    }
    int64_t id;
    if (!GetLongProperty(env, "id", static_cast<ani_object>(ref), id)) {
        return;
    }
    std::string bundleName;
    if (!GetStringField(env, "bundleName", static_cast<ani_object>(ref), bundleName, true)) {
        return;
    }
    std::string moduleName;
    if (!GetStringField(env, "moduleName", static_cast<ani_object>(ref), moduleName, true)) {
        return;
    }
    eventInfo.SetResourceId(id);
    eventInfo.SetResourceBundleName(bundleName);
    eventInfo.SetResourceModuleName(moduleName);
    HILOG_DEBUG("resourceId is %{public}lld, bundleName is %{public}s, moduleName is %{public}s",
        id, bundleName.c_str(), moduleName.c_str());
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

bool ANIUtils::ParseString(ani_env *env, ani_ref ref, std::string& outValue)
{
    ani_string str = static_cast<ani_string>(ref);
    ani_size strSize;
    auto status = env->String_GetUTF8Size(str, &strSize);
    if (status != ANI_OK) {
        HILOG_ERROR("AniUtils_ParseString String_GetUTF8Size status: %{public}d", status);
        return false;
    }
    std::vector<char> buffer(strSize + 1);
    char* utfBuffer = buffer.data();
    ani_size bytes_written = 0;
    env->String_GetUTF8(str, utfBuffer, strSize + 1, &bytes_written);
    utfBuffer[bytes_written] = '\0';
    outValue = std::string(utfBuffer);
    return true;
}

bool ANIUtils::ParseStringArray(ani_env *env, ani_object array, std::vector<std::string>& outValue)
{
    ani_size length = 0;
    if (env->Array_GetLength(static_cast<ani_array>(array), &length) != ANI_OK) {
        HILOG_ERROR("Failed to get array length");
        return false;
    }
    for (ani_size i = 0; i < length; i++) {
        ani_ref elementRef;
        if (env->Array_Get(static_cast<ani_array>(array), i, &elementRef) != ANI_OK) {
            HILOG_ERROR("Failed to get element at index %{public}zu", i);
            continue;
        }
        std::string strValue;
        if (!ANIUtils::ParseString(env, elementRef, strValue)) {
            HILOG_ERROR("Failed to parse string at index %{public}zu", i);
            continue;
        }
        outValue.push_back(strValue);
    }
    return true;
}

ani_object ANIUtils::CreateArray(ani_env *env, const std::vector<std::string> strs)
{
    ani_array array = ANIUtils::CreateEmptyAniArray(env, strs.size());
    if (array == nullptr) {
        HILOG_ERROR("Create array failed");
        return nullptr;
    }
    ani_size index = 0;
    for (auto &item : strs) {
        auto aniString = ANIUtils::CreateAniString(env, item);
        if (ANI_OK != env->Array_Set(array, index, aniString)) {
            HILOG_ERROR("Set array failed, index=%{public}zu", index);
            return nullptr;
        }
        index++;
    }
    return array;
}

std::string ANIUtils::AniStrToString(ani_env *env, ani_ref aniStr)
{
    ani_string ani_str = static_cast<ani_string>(aniStr);
    ani_size strSize;
    if (ANI_OK != env->String_GetUTF8Size(ani_str, &strSize)) {
        HILOG_ERROR(" Get utf8 size failed");
        return "";
    }
    std::vector<char> buffer(strSize + 1);
    ani_size bytes_written = 0;

    if (ANI_OK != env->String_GetUTF8(ani_str, buffer.data(), buffer.size(), &bytes_written)) {
        HILOG_ERROR(" Create string failed");
        return "";
    }
    return std::string(buffer.data(), bytes_written);
}

bool ANIUtils::GetStringMember(ani_env *env, ani_object options, const std::string &name, std::string &value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR(" Get property '%{public}s' failed", name.c_str());
        return false;
    }
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR(" Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        HILOG_ERROR(" GetStringMember isUndefined");
        return false;
    }
    value = ANIUtils::AniStrToString(env, ref);
    return true;
}

bool ANIUtils::GetNumberMember(ani_env *env, ani_object options, const std::string &name, uint32_t& value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR(" Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR(" Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_double valueDouble;
    if (ANI_OK != env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "toDouble", ":d", &valueDouble)) {
        HILOG_ERROR(" Unbox Double failed");
        return false;
    }
    HILOG_INFO(" valueDouble:%{public}f", valueDouble);
    value = static_cast<uint32_t>(valueDouble);
    return true;
}

bool ANIUtils::SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const ani_int value)
{
    static const char *className = "std.core.Int";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR(" Find class '%{public}s' failed", className);
        return false;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR(" Find method '<ctor>' failed");
        return false;
    }

    ani_object intObj;
    if (ANI_OK != env->Object_New(cls, ctor, &intObj, value)) {
        HILOG_ERROR(" New object '%{public}s' failed", className);
        return false;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), intObj)) {
        HILOG_ERROR(" Set property '%{public}s' failed", name.c_str());
        return false;
    }

    return true;
}

ani_object ANIUtils::CreateDouble(ani_env *env, int32_t value)
{
    if (env == nullptr) {
        HILOG_ERROR(" CreateDouble fail, env is nullptr");
        return nullptr;
    }
    ani_class doubleCls;
    ani_status status = ANI_ERROR;
    if ((status = env->FindClass(CLASSNAME_DOUBLE, &doubleCls)) != ANI_OK) {
        HILOG_ERROR(" status : %{public}d", status);
        return nullptr;
    }
    ani_method doubleCtor;
    if ((status = env->Class_FindMethod(doubleCls, "<ctor>", "d:", &doubleCtor)) != ANI_OK) {
        HILOG_ERROR(" status : %{public}d", status);
        return nullptr;
    }
    ani_object doubleObj;
    if ((status = env->Object_New(doubleCls, doubleCtor, &doubleObj, static_cast<ani_double>(value))) != ANI_OK) {
        HILOG_ERROR(" status : %{public}d", status);
        return nullptr;
    }
    return doubleObj;
}

ani_status ANIUtils::CreateAniBoolean(ani_env* env, bool value, ani_object& result)
{
    ani_status state;
    ani_class booleanClass;
    if ((state = env->FindClass("std.core.Boolean", &booleanClass)) != ANI_OK) {
        HILOG_ERROR("FindClass std/core/Boolean failed, %{public}d", state);
        return state;
    }
    ani_method booleanClassCtor;
    if ((state = env->Class_FindMethod(booleanClass, "<ctor>", "z:", &booleanClassCtor)) != ANI_OK) {
        HILOG_ERROR("Class_FindMethod Boolean ctor failed, %{public}d", state);
        return state;
    }
    ani_boolean aniValue = value;
    if ((state = env->Object_New(booleanClass, booleanClassCtor, &result, aniValue)) != ANI_OK) {
        HILOG_ERROR("New Boolean object failed, %{public}d", state);
        return state;
    }
    return state;
}

ani_status ANIUtils::CreateAniInt(ani_env* env, int32_t value, ani_object& result)
{
    ani_status state;
    ani_class intClass;
    if ((state = env->FindClass("std.core.Int", &intClass)) != ANI_OK) {
        HILOG_ERROR("FindClass std/core/Int failed, %{public}d", state);
        return state;
    }
    ani_method intClassCtor;
    if ((state = env->Class_FindMethod(intClass, "<ctor>", "i:", &intClassCtor)) != ANI_OK) {
        HILOG_ERROR("Class_FindMethod Int ctor failed, %{public}d", state);
        return state;
    }
    ani_int aniValue = value;
    if ((state = env->Object_New(intClass, intClassCtor, &result, aniValue)) != ANI_OK) {
        HILOG_ERROR("New Int object failed, %{public}d", state);
    }
    return state;
}

ani_status ANIUtils::CreateAniFloat(ani_env* env, float value, ani_object& result)
{
    ani_status state;
    ani_class floatClass;
    if ((state = env->FindClass("std.core.Float", &floatClass)) != ANI_OK) {
        HILOG_ERROR("FindClass std/core/Float failed, %{public}d", state);
        return state;
    }
    ani_method floatClassCtor;
    if ((state = env->Class_FindMethod(floatClass, "<ctor>", "f:", &floatClassCtor)) != ANI_OK) {
        HILOG_ERROR("Class_FindMethod Float ctor failed, %{public}d", state);
        return state;
    }
    ani_float aniValue = value;
    if ((state = env->Object_New(floatClass, floatClassCtor, &result, aniValue)) != ANI_OK) {
        HILOG_ERROR("New Float object failed, %{public}d", state);
    }
    return state;
}

ani_status ANIUtils::CreateAniLong(ani_env* env, int64_t value, ani_object& result)
{
    ani_status state;
    ani_class longClass;
    if ((state = env->FindClass("std.core.Long", &longClass)) != ANI_OK) {
        HILOG_ERROR("FindClass std/core/Long failed, %{public}d", state);
        return state;
    }
    ani_method longClassCtor;
    if ((state = env->Class_FindMethod(longClass, "<ctor>", "l:", &longClassCtor)) != ANI_OK) {
        HILOG_ERROR("Class_FindMethod Long ctor failed, %{public}d", state);
        return state;
    }
    ani_long aniValue = value;
    if ((state = env->Object_New(longClass, longClassCtor, &result, aniValue)) != ANI_OK) {
        HILOG_ERROR("New Long object failed, %{public}d", state);
    }
    return state;
}

bool ANIUtils::SetStringField(ani_env *env, ani_object &object, const std::string &fieldName,
    const std::string &value, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetStringField null env");
        return false;
    }
    ani_string string = nullptr;
    ani_status status;
    if (value.empty()) {
        ani_ref nullRef = nullptr;
        if ((status = env->GetNull(&nullRef)) != ANI_OK) {
            HILOG_ERROR("get null status : %{public}d", status);
            return false;
        }
        if (isProperty) {
            if ((status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), nullRef)) != ANI_OK) {
                HILOG_ERROR("set null property status : %{public}d", status);
                return false;
            }
            return true;
        } else {
            if ((status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), nullRef)) != ANI_OK) {
                HILOG_ERROR("set null status : %{public}d", status);
                return false;
            }
            return true;
        }
    }
    if ((status = env->String_NewUTF8(value.c_str(), value.size(), &string)) != ANI_OK) {
        HILOG_ERROR("change to ani string status : %{public}d", status);
        return false;
    }
    if (isProperty) {
        if ((status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), string)) != ANI_OK) {
            HILOG_ERROR("set property status : %{public}d", status);
            return false;
        }
        return true;
    } else {
        if ((status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), string)) != ANI_OK) {
            HILOG_ERROR("set ref status : %{public}d", status);
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetArrayStringField(ani_env *env, ani_object &object, const std::string &fieldName,
    const std::vector<std::string> &values, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetArrayStringField null env");
        return false;
    }
    ani_array arrayObj = ANIUtils::CreateEmptyAniArray(env, values.size());
    if (arrayObj == nullptr) {
        HILOG_ERROR("SetArrayStringField array is null");
        return false;
    }
    ani_size index = 0;
    for (auto &str : values) {
        if (ANI_OK != env->Array_Set(arrayObj, index, StdStringToAniString(env, str))) {
            HILOG_ERROR("Object_CallMethodByName_Void failed name=%{public}s index=%{public}zu str=%{public}s",
                fieldName.c_str(), index, str.c_str());
            return false;
        }
        index++;
    }
    if (isProperty) {
        ani_status status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), arrayObj);
        if (status != ANI_OK) {
            HILOG_ERROR("status : %{public}d", status);
            return false;
        }
        return true;
    } else {
        ani_status status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), arrayObj);
        if (status != ANI_OK) {
            HILOG_ERROR("status : %{public}d", status);
            return false;
        }

        return true;
    }
}

bool ANIUtils::SetIntField(ani_env *env, ani_object &object, const std::string &fieldName,
    int32_t fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetIntField null env");
        return false;
    }
    ani_object value;
    if (CreateAniInt(env, fieldValue, value) != ANI_OK) {
        HILOG_ERROR("CreateAniInt failed");
        return false;
    }
    if (isProperty) {
        if (env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), value) != ANI_OK) {
            HILOG_ERROR("set int property failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    } else {
        if (env->Object_SetFieldByName_Ref(object, fieldName.c_str(), value) != ANI_OK) {
            HILOG_ERROR("set int field failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetDoubleField(ani_env *env, ani_object &object, const std::string &fieldName,
    double fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetDoubleField null env");
        return false;
    }
    ani_status status;
    ani_object value = CreateDouble(env, static_cast<int32_t>(fieldValue));
    if (isProperty) {
        if ((status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set double property failed %{public}s %{public}d", fieldName.c_str(), status);
            return false;
        }
        return true;
    } else {
        if ((status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set double field failed %{public}s %{public}d", fieldName.c_str(), status);
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetBooleanField(ani_env *env, ani_object &object, const std::string &fieldName,
    bool fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetBoolField null env");
        return false;
    }
    ani_object value;
    if (CreateAniBoolean(env, fieldValue, value) != ANI_OK) {
        HILOG_ERROR("CreateAniBoolean failed");
        return false;
    }
    if (isProperty) {
        if ((env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set bool property failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    } else {
        if ((env->Object_SetFieldByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set bool field failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetFloatField(ani_env *env, ani_object &object, const std::string &fieldName,
    float fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetFloatField null env");
        return false;
    }
    ani_object value;
    if (CreateAniFloat(env, fieldValue, value) != ANI_OK) {
        HILOG_ERROR("CreateAniFloat failed");
        return false;
    }
    if (isProperty) {
        if ((env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set float property failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    } else {
        if ((env->Object_SetFieldByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set float field failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetLongField(ani_env *env, ani_object &object, const std::string &fieldName,
    int64_t fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetLongField null env");
        return false;
    }
    ani_object value;
    if (CreateAniLong(env, fieldValue, value) != ANI_OK) {
        HILOG_ERROR("CreateAniLong failed");
        return false;
    }
    ani_status status;
    if (isProperty) {
        if ((status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set long property failed %{public}s %{public}d", fieldName.c_str(), status);
            return false;
        }
        return true;
    } else {
        if ((status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), value)) != ANI_OK) {
            HILOG_ERROR("set long field failed %{public}s %{public}d", fieldName.c_str(), status);
            return false;
        }
        return true;
    }
}

bool ANIUtils::SetArrayField(ani_env *env, ani_object &object, const std::string &fieldName,
    const std::vector<ani_object> &fieldValue, bool isProperty)
{
    if (env == nullptr) {
        HILOG_ERROR("SetArrayField null env");
        return false;
    }
    ani_array array = CreateEmptyAniArray(env, fieldValue.size());
    if (array == nullptr) {
        HILOG_ERROR("CreateArray failed");
        return false;
    }
    ani_size index = 0;
    for (auto &item : fieldValue) {
        if (ANI_OK != env->Array_Set(array, index, item)) {
            HILOG_ERROR("Object_CallMethodByName_Void failed name=%{public}s index=%{public}zu",
                fieldName.c_str(), index);
            return false;
        }
        index++;
    }
    if (isProperty) {
        if (env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), array) != ANI_OK) {
            HILOG_ERROR("set array property failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    } else {
        if (env->Object_SetFieldByName_Ref(object, fieldName.c_str(), array) != ANI_OK) {
            HILOG_ERROR("set array field failed %{public}s", fieldName.c_str());
            return false;
        }
        return true;
    }
}

ani_object ANIUtils::CreateObject(ani_env *env, ani_class cls, ...)
{
    ani_object nullobj {};

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR("[ANI] Not found <ctor> for class");
        return nullobj;
    }

    ani_object obj;
    va_list args;
    va_start(args, cls);
    ani_status status = env->Object_New_V(cls, ctor, &obj, args);
    va_end(args);
    if (ANI_OK != status) {
        HILOG_ERROR("[ANI] Failed to Object_New for class 1");
        return nullobj;
    }
    return obj;
}

bool ANIUtils::ConvertStringToInt64(std::string &str, int64_t &value)
{
    auto [ptr, errCode] = std::from_chars(str.data(), str.data() + str.size(), value);
    return errCode == std::errc{} && ptr == str.data() + str.size();
}

void ANIUtils::CheckNumber(ani_env *env, std::string value)
{
    int num;
    std::stringstream streamStr;
    streamStr << value;
    if (!(streamStr >> num)) {
        ThrowBusinessError(env, QueryRetMsg(RetError::RET_ERR_INVALID_PARAM));
    }
}

void ANIUtils::SetScrollTypeParam(ani_env *env, ani_object obj, std::map<std::string, std::string>& args)
{
    ani_ref fiedNameValue;
    std::string str = "";
    std::map<std::string, std::string> scrollValueMap = { {"halfScreen", HALF_VALUE}, {"fullScreen", FULL_VALUE} };
    std::string scrollValue = FULL_VALUE;

    if (ANI_OK == env->Object_GetFieldByName_Ref(obj, "scrollType", &fiedNameValue)) {
        str = ANIStringToStdString(env, static_cast<ani_string>(fiedNameValue));
        if (scrollValueMap.find(str) != scrollValueMap.end()) {
            scrollValue = scrollValueMap.find(str)->second;
            HILOG_DEBUG("ScrollValue %{public}s", scrollValue.c_str());
        } else {
            HILOG_DEBUG("Input is empty, output fullScreen, value is 1");
            HILOG_DEBUG("Input is empty, throw error");
            ThrowBusinessError(env, QueryRetMsg(RetError::RET_ERR_INVALID_PARAM));
        }
        args.insert(std::pair<std::string, std::string>("scrolltype", scrollValue.c_str()));
    }
}

void ANIUtils::SetSelectionParam(ani_env *env, ani_object obj, std::map<std::string, std::string>& args)
{
    ani_ref fiedNameValue;
    ani_boolean forWard;
    std::string str = "";
    if (ANI_OK == env->Object_GetFieldByName_Ref(obj, "selectTextBegin", &fiedNameValue)) {
        str = ANIStringToStdString(env, static_cast<ani_string>(fiedNameValue));
        CheckNumber(env, str);
        args.insert(std::pair<std::string, std::string>("selectTextBegin", str.c_str()));
    }
    if (ANI_OK == env->Object_GetFieldByName_Ref(obj, "selectTextEnd", &fiedNameValue)) {
        str = ANIStringToStdString(env, static_cast<ani_string>(fiedNameValue));
        CheckNumber(env, str);
        args.insert(std::pair<std::string, std::string>("selectTextEnd", str.c_str()));
    }
    if (ANI_OK == env->Object_GetFieldByName_Boolean(obj, "selectTextInForWard", &forWard)) {
        std::string value = forWard ? "forWard" : "backWard";
        args.insert(std::pair<std::string, std::string>("selectTextInForWard", value.c_str()));
    }
}
