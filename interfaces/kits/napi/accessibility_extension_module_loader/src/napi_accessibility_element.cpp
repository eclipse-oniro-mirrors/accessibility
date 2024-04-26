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

#include "napi_accessibility_element.h"
#include <algorithm>
#include <map>
#include <vector>
#include <cinttypes>
#include "accessible_ability_client.h"
#include "hilog_wrapper.h"
#include "accessibility_utils.h"
#include "nlohmann/json.hpp"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityNapi;
namespace {
    const std::vector<std::string> ELEMENT_INFO_ATTRIBUTE_NAMES = {"componentId", "inspectorKey",
        "bundleName", "componentType", "inputType", "text", "hintText", "description", "triggerAction",
        "textMoveUnit", "contents", "lastContent", "itemCount", "currentIndex", "startIndex", "endIndex",
        "resourceName", "textLengthLimit", "rect", "checkable", "checked", "focusable", "isVisible",
        "selected", "clickable", "longClickable", "isEnable", "isPassword", "scrollable",
        "editable", "pluralLineSupported", "parent", "children", "isFocused", "accessibilityFocused",
        "error", "isHint", "pageId", "valueMax", "valueMin", "valueNow", "windowId", "accessibilityText",
        "textType", "offset", "allAttribute"};
    const std::vector<std::string> WINDOW_INFO_ATTRIBUTE_NAMES = {"isActive", "screenRect", "layer", "type",
        "rootElement", "isFocused", "windowId"};

    using AttributeNamesFunc = void (*)(NAccessibilityElementData *callbackInfo, napi_value &value);
    std::map<std::string, AttributeNamesFunc> elementInfoCompleteMap = {
        {"componentId", &NAccessibilityElement::GetElementInfoComponentId},
        {"inspectorKey", &NAccessibilityElement::GetElementInfoInspectorKey},
        {"bundleName", &NAccessibilityElement::GetElementInfoBundleName},
        {"componentType", &NAccessibilityElement::GetElementInfoComponentType},
        {"inputType", &NAccessibilityElement::GetElementInfoInputType},
        {"text", &NAccessibilityElement::GetElementInfoText},
        {"hintText", &NAccessibilityElement::GetElementInfoHintText},
        {"description", &NAccessibilityElement::GetElementInfoDescription},
        {"resourceName", &NAccessibilityElement::GetElementInfoResourceName},
        {"textLengthLimit", &NAccessibilityElement::GetElementInfoTextLengthLimit},
        {"rect", &NAccessibilityElement::GetElementInfoRect},
        {"checkable", &NAccessibilityElement::GetElementInfoCheckable},
        {"checked", &NAccessibilityElement::GetElementInfoChecked},
        {"focusable", &NAccessibilityElement::GetElementInfoFocusable},
        {"isVisible", &NAccessibilityElement::GetElementInfoIsVisible},
        {"selected", &NAccessibilityElement::GetElementInfoSelected},
        {"clickable", &NAccessibilityElement::GetElementInfoClickable},
        {"longClickable", &NAccessibilityElement::GetElementInfoLongClickable},
        {"isEnable", &NAccessibilityElement::GetElementInfoIsEnable},
        {"isPassword", &NAccessibilityElement::GetElementInfoIsPassword},
        {"scrollable", &NAccessibilityElement::GetElementInfoScrollable},
        {"editable", &NAccessibilityElement::GetElementInfoEditable},
        {"pluralLineSupported", &NAccessibilityElement::GetElementInfoPluralLineSupported},
        {"itemCount", &NAccessibilityElement::GetElementInfoItemCount},
        {"currentIndex", &NAccessibilityElement::GetElementInfoCurrentIndex},
        {"startIndex", &NAccessibilityElement::GetElementInfoStartIndex},
        {"endIndex", &NAccessibilityElement::GetElementInfoEndIndex},
        {"textMoveUnit", &NAccessibilityElement::GetElementInfoTextMoveUnit},
        {"parent", &NAccessibilityElement::GetElementInfoParent},
        {"children", &NAccessibilityElement::GetElementInfoChildren},
        {"triggerAction", &NAccessibilityElement::GetElementInfoTriggerAction},
        {"contents", &NAccessibilityElement::GetElementInfoContents},
        {"lastContent", &NAccessibilityElement::GetElementInfoLastContent},
        {"isFocused", &NAccessibilityElement::GetElementInfoIsFocused},
        {"accessibilityFocused", &NAccessibilityElement::GetElementInfoAccessibilityFocused},
        {"error", &NAccessibilityElement::GetElementInfoError},
        {"isHint", &NAccessibilityElement::GetElementInfoIsHint},
        {"pageId", &NAccessibilityElement::GetElementInfoPageId},
        {"valueMax", &NAccessibilityElement::GetElementInfoValueMax},
        {"valueMin", &NAccessibilityElement::GetElementInfoValueMin},
        {"valueNow", &NAccessibilityElement::GetElementInfoValueNow},
        {"windowId", &NAccessibilityElement::GetElementInfoWindowId},
        {"accessibilityText", &NAccessibilityElement::GetElementInfoAccessibilityText},
        {"textType", &NAccessibilityElement::GetElementInfoTextType},
        {"offset", &NAccessibilityElement::GetElementInfoOffset},
        {"allAttribute", &NAccessibilityElement::GetElementInfoAllAttribute},
    };
    std::map<std::string, AttributeNamesFunc> windowInfoCompleteMap = {
        {"isActive", &NAccessibilityElement::GetWindowInfoIsActive},
        {"screenRect", &NAccessibilityElement::GetWindowInfoScreenRect},
        {"layer", &NAccessibilityElement::GetWindowInfoLayer},
        {"type", &NAccessibilityElement::GetWindowInfoType},
        {"rootElement", &NAccessibilityElement::GetWindowInfoRootElement},
        {"isFocused", &NAccessibilityElement::GetWindowInfoIsFocused},
        {"windowId", &NAccessibilityElement::GetWindowInfoWindowId},
    };
} // namespace

thread_local napi_ref NAccessibilityElement::consRef_ = nullptr;

void NAccessibilityElement::DefineJSAccessibilityElement(napi_env env)
{
    napi_property_descriptor descForAccessibilityElement[] = {
        DECLARE_NAPI_FUNCTION("attributeNames", NAccessibilityElement::AttributeNames),
        DECLARE_NAPI_FUNCTION("attributeValue", NAccessibilityElement::AttributeValue),
        DECLARE_NAPI_FUNCTION("actionNames", NAccessibilityElement::ActionNames),
        DECLARE_NAPI_FUNCTION("enableScreenCurtain", NAccessibilityElement::EnableScreenCurtain),
        DECLARE_NAPI_FUNCTION("performAction", NAccessibilityElement::PerformAction),
        DECLARE_NAPI_FUNCTION("getCursorPosition", NAccessibilityElement::GetCursorPosition),
        DECLARE_NAPI_FUNCTION("findElement", NAccessibilityElement::FindElement),
    };

    napi_value constructor = nullptr;

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "AccessibilityElement",
            NAPI_AUTO_LENGTH,
            NAccessibilityElement::JSConstructor,
            nullptr,
            sizeof(descForAccessibilityElement) / sizeof(descForAccessibilityElement[0]),
            descForAccessibilityElement,
            &constructor));
    napi_create_reference(env, constructor, 1, &NAccessibilityElement::consRef_);
}

napi_value NAccessibilityElement::JSConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

void NAccessibilityElement::ConvertElementInfoToJS(napi_env env, napi_value result,
    const AccessibilityElementInfo& elementInfo)
{
    // Bind js object to a Native object
    std::shared_ptr<AccessibilityElementInfo> elementInformation =
        std::make_shared<AccessibilityElementInfo>(elementInfo);
    AccessibilityElement* pAccessibilityElement = new(std::nothrow) AccessibilityElement(elementInformation);
    if (!pAccessibilityElement) {
        HILOG_ERROR("Failed to create elementInformation.");
        return;
    }
    napi_status sts = napi_wrap(
        env,
        result,
        pAccessibilityElement,
        [](napi_env env, void* data, void* hint) {
            AccessibilityElement* info = static_cast<AccessibilityElement*>(data);
            delete info;
            info = nullptr;
        },
        nullptr,
        nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
}

void NAccessibilityElement::ConvertElementInfosToJS(
    napi_env env, napi_value result, const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("elementInfo size(%{public}zu)", elementInfos.size());

    napi_value constructor = nullptr;
    napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);

    size_t index = 0;
    for (auto& elementInfo : elementInfos) {
        napi_value obj = nullptr;
        napi_status status = napi_new_instance(env, constructor, 0, nullptr, &obj);
        HILOG_DEBUG("status is %{public}d", status);
        ConvertElementInfoToJS(env, obj, elementInfo);
        napi_set_element(env, result, index, obj);
        index++;
    }
}

napi_value NAccessibilityElement::AttributeNames(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;

    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE - 1) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv, &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv, 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "AttributeNames", NAPI_AUTO_LENGTH, &resource);
    if (resource == nullptr) {
        HILOG_ERROR("resource is nullptr.");
        return nullptr;
    }
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        AttributeNamesComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityElement::AttributeNamesComplete(napi_env env, napi_status status, void* data)
{
    HILOG_DEBUG("AttributeNameComplete execute back");
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    napi_create_array(env, &result[1]);
    if (callbackInfo->accessibilityElement_.isElementInfo_) {
        HILOG_DEBUG("covert element info to js");
        ConvertStringVecToJS(env, result[1], ELEMENT_INFO_ATTRIBUTE_NAMES);
    } else {
        HILOG_DEBUG("covert window info to js");
        ConvertStringVecToJS(env, result[1], WINDOW_INFO_ATTRIBUTE_NAMES);
    }

    if (callbackInfo->callback_) {
        // Callback mode
        result[PARAM0] = CreateBusinessError(env, OHOS::Accessibility::RetError::RET_OK);
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        // Promise mode
        napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

napi_value NAccessibilityElement::AttributeValue(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    callbackInfo->env_ = env;
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    NAccessibilityErrorCode errCode = GetAttribute(env, argc, argv, callbackInfo);
    if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        delete accessibilityElement;
        accessibilityElement = nullptr;
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_INVALID_PARAM);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    return AttributeValueAsync(env, argc, argv, callbackInfo);
}

NAccessibilityErrorCode NAccessibilityElement::GetAttribute(
    napi_env env, size_t argc, napi_value* argv, NAccessibilityElementData* callbackInfo)
{
    NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
    }

    if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_OK) {
        // Parse attribute name
        std::string attribute = "";
        if (ParseString(env, attribute, argv[PARAM0])) {
            HILOG_DEBUG("attribute = %{public}s", attribute.c_str());
            callbackInfo->attribute_ = attribute;
        } else {
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }
    }
    return errCode;
}

napi_value NAccessibilityElement::AttributeValueAsync(
    napi_env env, size_t argc, napi_value* argv, NAccessibilityElementData* callbackInfo)
{
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_TWO - 1) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[PARAM1], &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "AttributeValue", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, NAccessibilityElement::AttributeValueExecute,
        NAccessibilityElement::AttributeValueComplete, reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityElement::AttributeValueExecute(napi_env env, void* data)
{
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    if (callbackInfo->attribute_ == "parent") {
        if (callbackInfo->accessibilityElement_.elementInfo_) {
            callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetParentElementInfo(
                *callbackInfo->accessibilityElement_.elementInfo_, callbackInfo->nodeInfo_);
        } else {
            HILOG_ERROR("elementInfo is nullptr");
        }
    } else if (callbackInfo->attribute_ == "children") {
        if (callbackInfo->accessibilityElement_.elementInfo_) {
            callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetChildren(
                *callbackInfo->accessibilityElement_.elementInfo_, callbackInfo->nodeInfos_);
        } else {
            HILOG_ERROR("elementInfo is nullptr");
        }
    } else if (callbackInfo->attribute_ == "rootElement") {
        if (callbackInfo->accessibilityElement_.windowInfo_) {
            callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetRootByWindow(
                *callbackInfo->accessibilityElement_.windowInfo_, callbackInfo->nodeInfo_);
        } else {
            HILOG_ERROR("windowInfo is nullptr");
        }
    } else {
        callbackInfo->ret_ = RET_OK;
    }
    HILOG_DEBUG("attribute[%{public}s], result[%{public}d]", callbackInfo->attribute_.c_str(), callbackInfo->ret_);
}

void NAccessibilityElement::AttributeValueComplete(napi_env env, napi_status status, void* data)
{
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_TWO] = {0};

    if (callbackInfo->accessibilityElement_.isElementInfo_) {
        HILOG_DEBUG("It is element info");
        auto elementIter = elementInfoCompleteMap.find(callbackInfo->attribute_);
        if (elementIter == elementInfoCompleteMap.end()) {
            HILOG_ERROR("There is no the attribute[%{public}s] in element info", callbackInfo->attribute_.c_str());
            napi_get_undefined(callbackInfo->env_, &result[PARAM1]);
            callbackInfo->ret_ = RET_ERR_PROPERTY_NOT_EXIST;
        } else {
            (*elementIter->second)(callbackInfo, result[PARAM1]);
        }
    } else {
        HILOG_DEBUG("It is window info");
        auto windowIter = windowInfoCompleteMap.find(callbackInfo->attribute_);
        if (windowIter == windowInfoCompleteMap.end()) {
            HILOG_ERROR("There is no the attribute[%{public}s]", callbackInfo->attribute_.c_str());
            napi_get_undefined(callbackInfo->env_, &result[PARAM1]);
            callbackInfo->ret_ = RET_ERR_PROPERTY_NOT_EXIST;
        } else {
            (*windowIter->second)(callbackInfo, result[PARAM1]);
        }
    }

    HILOG_DEBUG("result is %{public}d", callbackInfo->ret_);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        napi_value callback;
        napi_value returnVal;
        napi_value undefined;
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_get_undefined(env, &undefined);
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        if (callbackInfo->ret_ == RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

bool NAccessibilityElement::CheckElementInfoParameter(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return false;
    }
    if (!callbackInfo->accessibilityElement_.elementInfo_) {
        HILOG_ERROR("element info is nullptr");
        napi_get_undefined(callbackInfo->env_, &value);
        callbackInfo->ret_ = RET_ERR_FAILED;
        return false;
    }
    return true;
}

void NAccessibilityElement::GetElementInfoComponentId(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int64(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetAccessibilityId(), &value));
}

void NAccessibilityElement::GetElementInfoPageId(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetPageId(), &value));
}

void NAccessibilityElement::GetElementInfoInspectorKey(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetInspectorKey().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoBundleName(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetBundleName().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoComponentType(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetComponentType().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoInputType(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetInputType(), &value));
}

void NAccessibilityElement::GetElementInfoText(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetContent().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoHintText(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetHint().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoDescription(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetDescriptionInfo().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoResourceName(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetComponentResourceId().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoTextLengthLimit(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetTextLengthLimit(), &value));
}

void NAccessibilityElement::GetElementInfoRect(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    OHOS::Accessibility::Rect screenRect = callbackInfo->accessibilityElement_.elementInfo_->GetRectInScreen();
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_object(callbackInfo->env_, &value));
    ConvertRectToJS(callbackInfo->env_, value, screenRect);
}

void NAccessibilityElement::GetElementInfoCheckable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsCheckable(), &value));
}

void NAccessibilityElement::GetElementInfoChecked(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsChecked(), &value));
}

void NAccessibilityElement::GetElementInfoFocusable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsFocusable(), &value));
}

void NAccessibilityElement::GetElementInfoIsVisible(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsVisible(), &value));
}

void NAccessibilityElement::GetElementInfoAccessibilityFocused(
    NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->HasAccessibilityFocus(), &value));
}

void NAccessibilityElement::GetElementInfoSelected(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsSelected(), &value));
}

void NAccessibilityElement::GetElementInfoClickable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsClickable(), &value));
}

void NAccessibilityElement::GetElementInfoLongClickable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsLongClickable(), &value));
}

void NAccessibilityElement::GetElementInfoIsEnable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsEnabled(), &value));
}

void NAccessibilityElement::GetElementInfoIsPassword(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsPassword(), &value));
}

void NAccessibilityElement::GetElementInfoScrollable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsScrollable(), &value));
}

void NAccessibilityElement::GetElementInfoEditable(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsEditable(), &value));
}

void NAccessibilityElement::GetElementInfoPluralLineSupported(
    NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsPluraLineSupported(), &value));
}

void NAccessibilityElement::GetElementInfoIsHint(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsGivingHint(), &value));
}

void NAccessibilityElement::GetElementInfoItemCount(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetItemCounts(), &value));
}

void NAccessibilityElement::GetElementInfoCurrentIndex(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetCurrentIndex(), &value));
}

void NAccessibilityElement::GetElementInfoStartIndex(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetBeginIndex(), &value));
}

void NAccessibilityElement::GetElementInfoEndIndex(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetEndIndex(), &value));
}

void NAccessibilityElement::GetElementInfoValueMax(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetRange().GetMax(), &value));
}

void NAccessibilityElement::GetElementInfoValueMin(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetRange().GetMin(), &value));
}

void NAccessibilityElement::GetElementInfoValueNow(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetRange().GetCurrent(), &value));
}

void NAccessibilityElement::GetElementInfoError(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetError().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoTextMoveUnit(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    std::string textMoveUnit = ConvertTextMoveUnitToString(
        callbackInfo->accessibilityElement_.elementInfo_->GetTextMovementStep());
    HILOG_DEBUG("ConvertTextMoveUnitToString: [%{public}s]", textMoveUnit.c_str());
    if (textMoveUnit == "") {
        callbackInfo->ret_ = RET_ERR_FAILED;
        napi_get_undefined(callbackInfo->env_, &value);
    } else {
        callbackInfo->ret_ = RET_OK;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
            textMoveUnit.c_str(), NAPI_AUTO_LENGTH, &value));
    }
}

void NAccessibilityElement::GetElementInfoParent(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    if (callbackInfo->ret_ == RET_OK) {
        napi_value constructor = nullptr;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_reference_value(callbackInfo->env_,
            NAccessibilityElement::consRef_, &constructor));
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_new_instance(callbackInfo->env_,
            constructor, 0, nullptr, &value));
        ConvertElementInfoToJS(callbackInfo->env_, value, callbackInfo->nodeInfo_);
    } else {
        HILOG_ERROR("GetElementInfoParent failed!");
        napi_get_undefined(callbackInfo->env_, &value);
    }
}

void NAccessibilityElement::GetElementInfoChildren(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    if (callbackInfo->ret_ == RET_OK) {
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_array(callbackInfo->env_, &value));
        ConvertElementInfosToJS(callbackInfo->env_, value, callbackInfo->nodeInfos_);
    } else {
        HILOG_ERROR("GetElementInfoChildren failed!");
        napi_get_undefined(callbackInfo->env_, &value);
    }
}

void NAccessibilityElement::GetElementInfoTriggerAction(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    std::string triggerAction = ConvertOperationTypeToString(
        callbackInfo->accessibilityElement_.elementInfo_->GetTriggerAction());
    HILOG_DEBUG("GetElementInfoTriggerAction: [%{public}s]", triggerAction.c_str());
    if (triggerAction == "") {
        callbackInfo->ret_ = RET_ERR_FAILED;
        napi_get_undefined(callbackInfo->env_, &value);
    } else {
        callbackInfo->ret_ = RET_OK;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
            triggerAction.c_str(), NAPI_AUTO_LENGTH, &value));
    }
}

void NAccessibilityElement::GetElementInfoContents(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    std::vector<std::string> contents {};
    callbackInfo->accessibilityElement_.elementInfo_->GetContentList(contents);
    HILOG_DEBUG("contents size: [%{public}zu]", contents.size());

    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_array(callbackInfo->env_, &value));
    size_t index = 0;
    for (auto& content : contents) {
        napi_value nContent = nullptr;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
            content.c_str(), NAPI_AUTO_LENGTH, &nContent));
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_set_element(callbackInfo->env_, value, index, nContent));
        index++;
    }
}

void NAccessibilityElement::GetElementInfoLastContent(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetLatestContent().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoWindowId(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetWindowId(), &value));
}

void NAccessibilityElement::GetElementInfoIsFocused(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->IsFocused(), &value));
}

void NAccessibilityElement::GetElementInfoAccessibilityText(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetAccessibilityText().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoTextType(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetTextType().c_str(), NAPI_AUTO_LENGTH, &value));
}

void NAccessibilityElement::GetElementInfoOffset(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckElementInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_double(callbackInfo->env_,
        callbackInfo->accessibilityElement_.elementInfo_->GetOffset(), &value));
}

void NAccessibilityElement::GetElementInfoAllAttribute(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_object(callbackInfo->env_, &value));
    if (CheckElementInfoParameter(callbackInfo, value)) {
        GetElementInfoAllAttribute1(callbackInfo, value);
        GetElementInfoAllAttribute2(callbackInfo, value);
        GetElementInfoAllAttribute3(callbackInfo, value);
        GetElementInfoAllAttribute4(callbackInfo, value);
    } else if (CheckWindowInfoParameter(callbackInfo, value)) {
        GetWindowInfoAllAttribute(callbackInfo, value);
    } else {
        return;
    }
}

void NAccessibilityElement::GetElementInfoAllAttribute1(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    napi_env env = callbackInfo->env_;
    napi_value componentId = nullptr;
    GetElementInfoComponentId(callbackInfo, componentId);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "componentId", componentId));

    napi_value inspectorKey = nullptr;
    GetElementInfoInspectorKey(callbackInfo, inspectorKey);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "inspectorKey", inspectorKey));

    napi_value bundleName = nullptr;
    GetElementInfoBundleName(callbackInfo, bundleName);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "bundleName", bundleName));

    napi_value componentType = nullptr;
    GetElementInfoComponentType(callbackInfo, componentType);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "componentType", componentType));

    napi_value inputType = nullptr;
    GetElementInfoInputType(callbackInfo, inputType);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "inputType", inputType));

    napi_value text = nullptr;
    GetElementInfoText(callbackInfo, text);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "text", text));

    napi_value hintText = nullptr;
    GetElementInfoHintText(callbackInfo, hintText);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "hintText", hintText));

    napi_value description = nullptr;
    GetElementInfoDescription(callbackInfo, description);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "description", description));

    napi_value resourceName = nullptr;
    GetElementInfoResourceName(callbackInfo, resourceName);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "resourceName", resourceName));
}

void NAccessibilityElement::GetElementInfoAllAttribute2(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    napi_env env = callbackInfo->env_;
    napi_value textLengthLimit = nullptr;
    GetElementInfoTextLengthLimit(callbackInfo, textLengthLimit);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "textLengthLimit", textLengthLimit));

    napi_value rect = nullptr;
    GetElementInfoRect(callbackInfo, rect);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "rect", rect));

    napi_value checkable = nullptr;
    GetElementInfoCheckable(callbackInfo, checkable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "checkable", checkable));

    napi_value checked = nullptr;
    GetElementInfoChecked(callbackInfo, checked);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "checked", checked));

    napi_value focusable = nullptr;
    GetElementInfoFocusable(callbackInfo, focusable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "focusable", focusable));

    napi_value isVisible = nullptr;
    GetElementInfoIsVisible(callbackInfo, isVisible);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isVisible", isVisible));

    napi_value selected = nullptr;
    GetElementInfoSelected(callbackInfo, selected);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "selected", selected));

    napi_value clickable = nullptr;
    GetElementInfoClickable(callbackInfo, clickable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "clickable", clickable));

    napi_value longClickable = nullptr;
    GetElementInfoLongClickable(callbackInfo, longClickable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "longClickable", longClickable));

    napi_value isEnable = nullptr;
    GetElementInfoIsEnable(callbackInfo, isEnable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isEnable", isEnable));

    napi_value isPassword = nullptr;
    GetElementInfoIsPassword(callbackInfo, isPassword);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isPassword", isPassword));
}

void NAccessibilityElement::GetElementInfoAllAttribute3(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    napi_env env = callbackInfo->env_;
    napi_value scrollable = nullptr;
    GetElementInfoScrollable(callbackInfo, scrollable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "scrollable", scrollable));

    napi_value editable = nullptr;
    GetElementInfoEditable(callbackInfo, editable);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "editable", editable));

    napi_value pluralLineSupported = nullptr;
    GetElementInfoPluralLineSupported(callbackInfo, pluralLineSupported);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "pluralLineSupported", pluralLineSupported));

    napi_value itemCount = nullptr;
    GetElementInfoItemCount(callbackInfo, itemCount);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "itemCount", itemCount));

    napi_value currentIndex = nullptr;
    GetElementInfoCurrentIndex(callbackInfo, currentIndex);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "currentIndex", currentIndex));

    napi_value startIndex = nullptr;
    GetElementInfoStartIndex(callbackInfo, startIndex);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "startIndex", startIndex));

    napi_value endIndex = nullptr;
    GetElementInfoEndIndex(callbackInfo, endIndex);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "endIndex", endIndex));

    napi_value textMoveUnit = nullptr;
    GetElementInfoTextMoveUnit(callbackInfo, textMoveUnit);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "textMoveUnit", textMoveUnit));

    napi_value triggerAction = nullptr;
    std::string action = ConvertOperationTypeToString(
        callbackInfo->accessibilityElement_.elementInfo_->GetTriggerAction());
    if (action == "") {
        napi_get_undefined(callbackInfo->env_, &triggerAction);
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, action.c_str(), NAPI_AUTO_LENGTH, &triggerAction));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "triggerAction", triggerAction));

    napi_value contents = nullptr;
    GetElementInfoContents(callbackInfo, contents);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "contents", contents));
}

void NAccessibilityElement::GetElementInfoAllAttribute4(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    napi_env env = callbackInfo->env_;
    napi_value lastContent = nullptr;
    GetElementInfoLastContent(callbackInfo, lastContent);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "lastContent", lastContent));

    napi_value isFocused = nullptr;
    GetElementInfoIsFocused(callbackInfo, isFocused);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isFocused", isFocused));

    napi_value accessibilityFocused = nullptr;
    GetElementInfoAccessibilityFocused(callbackInfo, accessibilityFocused);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "accessibilityFocused", accessibilityFocused));

    napi_value error = nullptr;
    GetElementInfoError(callbackInfo, error);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "error", error));

    napi_value isHint = nullptr;
    GetElementInfoIsHint(callbackInfo, isHint);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isHint", isHint));

    napi_value pageId = nullptr;
    GetElementInfoPageId(callbackInfo, pageId);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "pageId", pageId));

    napi_value valueMax = nullptr;
    GetElementInfoValueMax(callbackInfo, valueMax);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "valueMax", valueMax));

    napi_value valueMin = nullptr;
    GetElementInfoValueMin(callbackInfo, valueMin);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "valueMin", valueMin));

    napi_value valueNow = nullptr;
    GetElementInfoValueNow(callbackInfo, valueNow);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "valueNow", valueNow));

    napi_value windowId = nullptr;
    GetElementInfoWindowId(callbackInfo, windowId);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "windowId", windowId));

    napi_value accessibilityText = nullptr;
    GetElementInfoAccessibilityText(callbackInfo, accessibilityText);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "accessibilityText", accessibilityText));

    napi_value textType = nullptr;
    GetElementInfoTextType(callbackInfo, textType);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "textType", textType));

    napi_value offset = nullptr;
    GetElementInfoOffset(callbackInfo, offset);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "offset", offset));
}

void NAccessibilityElement::GetWindowInfoAllAttribute(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    napi_env env = callbackInfo->env_;
    napi_value isActive = nullptr;
    GetWindowInfoIsActive(callbackInfo, isActive);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isActive", isActive));

    napi_value screenRect = nullptr;
    GetWindowInfoScreenRect(callbackInfo, screenRect);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "screenRect", screenRect));

    napi_value layer = nullptr;
    GetWindowInfoLayer(callbackInfo, layer);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "layer", layer));

    napi_value type = nullptr;
    GetWindowInfoType(callbackInfo, type);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "type", type));

    napi_value isFocused = nullptr;
    GetWindowInfoIsFocused(callbackInfo, isFocused);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isFocused", isFocused));

    napi_value windowId = nullptr;
    GetWindowInfoWindowId(callbackInfo, windowId);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "windowId", windowId));
}

bool NAccessibilityElement::CheckWindowInfoParameter(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return false;
    }
    if (!callbackInfo->accessibilityElement_.windowInfo_) {
        HILOG_ERROR("window info is nullptr");
        napi_get_undefined(callbackInfo->env_, &value);
        callbackInfo->ret_ = RET_ERR_FAILED;
        return false;
    }
    return true;
}

void NAccessibilityElement::GetWindowInfoIsActive(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.windowInfo_->IsActive(), &value));
}

void NAccessibilityElement::GetWindowInfoScreenRect(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    OHOS::Accessibility::Rect screenRect = callbackInfo->accessibilityElement_.windowInfo_->GetRectInScreen();
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_object(callbackInfo->env_, &value));
    ConvertRectToJS(callbackInfo->env_, value, screenRect);
}

void NAccessibilityElement::GetWindowInfoLayer(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.windowInfo_->GetWindowLayer(), &value));
}

void NAccessibilityElement::GetWindowInfoType(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    std::string accessibilityWindowType = ConvertWindowTypeToString(
        callbackInfo->accessibilityElement_.windowInfo_->GetAccessibilityWindowType());
    HILOG_DEBUG("GetWindowInfoType: [%{public}s]", accessibilityWindowType.c_str());
    if (accessibilityWindowType == "") {
        callbackInfo->ret_ = RET_ERR_FAILED;
        napi_get_undefined(callbackInfo->env_, &value);
    } else {
        callbackInfo->ret_ = RET_OK;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_string_utf8(callbackInfo->env_,
            accessibilityWindowType.c_str(), NAPI_AUTO_LENGTH, &value));
    }
}

void NAccessibilityElement::GetWindowInfoRootElement(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    if (callbackInfo->ret_ == RET_OK) {
        napi_value constructor = nullptr;
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_reference_value(callbackInfo->env_,
            NAccessibilityElement::consRef_, &constructor));
        NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_new_instance(callbackInfo->env_,
            constructor, 0, nullptr, &value));
        ConvertElementInfoToJS(callbackInfo->env_, value, callbackInfo->nodeInfo_);
    } else {
        HILOG_ERROR("GetWindowInfoRootElement failed!");
        napi_get_undefined(callbackInfo->env_, &value);
    }
}

void NAccessibilityElement::GetWindowInfoIsFocused(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_get_boolean(callbackInfo->env_,
        callbackInfo->accessibilityElement_.windowInfo_->IsFocused(), &value));
}

void NAccessibilityElement::GetWindowInfoWindowId(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    if (!CheckWindowInfoParameter(callbackInfo, value)) {
        return;
    }
    NAPI_CALL_RETURN_VOID(callbackInfo->env_, napi_create_int32(callbackInfo->env_,
        callbackInfo->accessibilityElement_.windowInfo_->GetWindowId(), &value));
}

napi_value NAccessibilityElement::ActionNames(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}zu", argc);

    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;

    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE - 1) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv, &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv, 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;
    if (!callbackInfo->accessibilityElement_.isElementInfo_) {
        HILOG_ERROR("it is not element info");
        return ErrorOperation(callbackInfo);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ActionNames", NAPI_AUTO_LENGTH, &resource);
    if (resource == nullptr) {
        HILOG_ERROR("resource is nullptr.");
        return nullptr;
    }

    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        ActionNamesComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityElement::ActionNamesComplete(napi_env env, napi_status status, void* data)
{
    HILOG_DEBUG("ActionNamesComplete execute back");
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    if (callbackInfo->accessibilityElement_.elementInfo_) {
        std::vector<std::string> actionNames {};
        std::vector<AccessibleAction> operations =
            callbackInfo->accessibilityElement_.elementInfo_->GetActionList();
        HILOG_DEBUG("action list size is %{public}zu", operations.size());
        actionNames.resize(operations.size());
        std::transform(operations.begin(), operations.end(), actionNames.begin(),
            [](const AccessibleAction operation) {
                return ConvertOperationTypeToString(operation.GetActionType());
            });
        napi_create_array(env, &result[PARAM1]);
        ConvertStringVecToJS(env, result[PARAM1], actionNames);
        callbackInfo->ret_ = RET_OK;
    } else {
        HILOG_ERROR("no elementInfo_");
        callbackInfo->ret_ = RET_ERR_FAILED;
        napi_get_undefined(env, &result[PARAM1]);
    }

    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        // Callback mode
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        // Promise mode
        if (callbackInfo->accessibilityElement_.elementInfo_) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

napi_value NAccessibilityElement::EnableScreenCurtain(napi_env env, napi_callback_info info)
{
    HILOG_INFO("enter NAccessibilityElement::EnableScreenCurtain");

    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }

    //get value from ui
    bool isEnable = true;
    napi_get_value_bool(env, argv[0], &isEnable);
    HILOG_INFO("ui argv[0] isEnable = %{public}d", isEnable);

    //pass valule to server
    AccessibleAbilityClient::GetInstance()->EnableScreenCurtain(isEnable);
    return thisVar;
}

napi_value NAccessibilityElement::PerformAction(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    HILOG_DEBUG("argc = %{public}zu", argc);
    
    AccessibilityElement* accessibilityElement = UnrapAccessibilityElement(env, thisVar);
    if (!accessibilityElement) {
        return nullptr;
    }
    std::string actionName;
    NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
    if (argc < ARGS_SIZE_ONE || !ParseString(env, actionName, argv[PARAM0])) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        delete accessibilityElement;
        accessibilityElement = nullptr;
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_INVALID_PARAM);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }
    return PerformActionAsync(env, argc, argv, actionName, accessibilityElement);
}

AccessibilityElement* NAccessibilityElement::UnrapAccessibilityElement(napi_env env, napi_value thisVar)
{
    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    if (!(accessibilityElement->isElementInfo_ && accessibilityElement->elementInfo_)) {
        HILOG_ERROR("accessibilityElement is wrong. isElementInfo_[%{public}d]", accessibilityElement->isElementInfo_);
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    return accessibilityElement;
}

napi_value NAccessibilityElement::PerformActionAsync(napi_env env, size_t argc, napi_value* argv,
    std::string actionName, AccessibilityElement* accessibilityElement)
{
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    callbackInfo->env_ = env;
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    return PerformActionConstructPromise(env, argc, argv, callbackInfo, actionName);
}

napi_value NAccessibilityElement::PerformActionConstructPromise(napi_env env, size_t argc, napi_value* argv,
    NAccessibilityElementData* callbackInfo, std::string actionName)
{
    napi_value promise = nullptr;
    std::map<std::string, std::string> actionArguments {};
    if (argc >= ARGS_SIZE_THREE) {
        napi_valuetype secondParamType = napi_null;
        napi_typeof(env, argv[PARAM1], &secondParamType);
        napi_valuetype thirdParamType = napi_null;
        napi_typeof(env, argv[PARAM2], &thirdParamType);
        if (thirdParamType == napi_function) {
            if (secondParamType == napi_object) {
                ConvertActionArgsJSToNAPI(env, argv[PARAM1], actionArguments,
                    ConvertStringToAccessibleOperationType(actionName));
            }
            napi_create_reference(env, argv[PARAM2], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else if (secondParamType == napi_function) {
            napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            HILOG_DEBUG("argc is three, use promise");
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else if (argc == ARGS_SIZE_TWO) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[PARAM1], &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            if (valueType == napi_object) {
                ConvertActionArgsJSToNAPI(env, argv[PARAM1], actionArguments,
                    ConvertStringToAccessibleOperationType(actionName));
            }
            HILOG_DEBUG("argc is two, use promise");
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        HILOG_DEBUG("argc is others, use promise");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    callbackInfo->actionName_ = actionName;
    callbackInfo->actionArguments_ = actionArguments;
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "PerformAction", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, PerformActionExecute, PerformActionComplete,
        reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityElement::PerformActionExecute(napi_env env, void* data)
{
    HILOG_DEBUG("PerformActionExecute");
    NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    ActionType action = ConvertStringToAccessibleOperationType(callbackInfo->actionName_);
    if (action == ActionType::ACCESSIBILITY_ACTION_INVALID) {
        HILOG_ERROR("The action is not supported");
        callbackInfo->ret_ = OHOS::Accessibility::RetError::RET_ERR_ACTION_NOT_SUPPORT;
        return;
    }
    callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->ExecuteAction(
        *callbackInfo->accessibilityElement_.elementInfo_, action, callbackInfo->actionArguments_);
    HILOG_DEBUG("callbackInfo->ret_[%{public}d]", callbackInfo->ret_);
}

void NAccessibilityElement::PerformActionComplete(napi_env env, napi_status status, void* data)
{
    HILOG_DEBUG("PerformActionComplete execute back");
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_get_undefined(env, &result[PARAM1]);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        // Callback mode
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        // Promise mode
        if (callbackInfo->ret_ == RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }

    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

napi_value NAccessibilityElement::FindElement(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    if (!accessibilityElement->isElementInfo_) {
        HILOG_ERROR("Type of AccessibilityElement is not right");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }

    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    callbackInfo->env_ = env;
    FindElementConstructCallbackInfo(env, argc, argv, callbackInfo, accessibilityElement);
    if (!callbackInfo) {
        return nullptr;
    }
    return FindElementAsync(env, argc, argv, callbackInfo, accessibilityElement);
}

napi_value NAccessibilityElement::GetCursorPositionAsync(napi_env env, size_t argc, napi_value* argv,
    NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement)
{
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE - 1) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[PARAM2], &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv[PARAM2], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    callbackInfo->accessibilityElement_ = *accessibilityElement;

    napi_value resource = nullptr;
    napi_create_string_utf8(callbackInfo->env_, "GetCursorPosition", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(callbackInfo->env_, nullptr, resource, GetCursorPositionExecute,
        GetCursorPositionComplete, reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work_with_qos(callbackInfo->env_, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityElement::FindElementAsync(napi_env env, size_t argc, napi_value* argv,
    NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement)
{
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_THREE - 1) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[PARAM2], &valueType);
        if (valueType == napi_function) {
            napi_create_reference(env, argv[PARAM2], 1, &callbackInfo->callback_);
            napi_get_undefined(env, &promise);
        } else {
            napi_create_promise(env, &callbackInfo->deferred_, &promise);
        }
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    callbackInfo->accessibilityElement_ = *accessibilityElement;

    napi_value resource = nullptr;
    napi_create_string_utf8(callbackInfo->env_, "FindElement", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(callbackInfo->env_, nullptr, resource, FindElementExecute,
        FindElementComplete, reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work_with_qos(callbackInfo->env_, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityElement::FindElementConstructCallbackInfo(napi_env env, size_t argc, napi_value* argv,
    NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement)
{
    NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
    if (argc < ARGS_SIZE_THREE - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
    }

    if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_OK) {
        // Parse conditionType name
        std::string conditionType = "";
        if (ParseString(env, conditionType, argv[PARAM0])) {
            HILOG_DEBUG("conditionType = %{public}s", conditionType.c_str());
            if (std::strcmp(conditionType.c_str(), "content") != 0 &&
                std::strcmp(conditionType.c_str(), "focusType") != 0 &&
                std::strcmp(conditionType.c_str(), "focusDirection") != 0 &&
                std::strcmp(conditionType.c_str(), "textType") != 0 &&
                std::strcmp(conditionType.c_str(), "elementId") != 0) {
                HILOG_ERROR("argv[PARAM0] is wrong[%{public}s", conditionType.c_str());
                errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
            } else {
                callbackInfo->conditionId_ = ConvertStringToCondition(conditionType);
            }
        } else {
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }

        // Parse queryData name
        std::string queryData = "";
        int64_t elementId;
        if (ParseString(env, queryData, argv[PARAM1])) {
            HILOG_DEBUG("queryData = %{public}s", queryData.c_str());
            callbackInfo->condition_ = queryData;
        } else if (ParseInt64(env, elementId, argv[PARAM1])) {
            queryData = std::to_string(elementId);
            HILOG_DEBUG("queryData = %{public}s", queryData.c_str());
            callbackInfo->condition_ = queryData;
        } else {
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }
    }

    if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        delete accessibilityElement;
        accessibilityElement = nullptr;
        napi_value err = CreateBusinessError(env, RetError::RET_ERR_INVALID_PARAM);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
    }
}

void NAccessibilityElement::FindElementByText(NAccessibilityElementData *callbackInfo)
{
    nlohmann::json jsonStr = nullptr;
    jsonStr["type"] = (callbackInfo->conditionId_ ==
        FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT) ? "content" : "textType";
    jsonStr["value"] = callbackInfo->condition_;
    callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetByContent(
        *(callbackInfo->accessibilityElement_.elementInfo_),
        jsonStr.dump().c_str(), callbackInfo->nodeInfos_);
    return;
}

void NAccessibilityElement::GetCursorPositionExecute(napi_env env, void* data)
{
    HILOG_DEBUG("GetCursorPositionExecute");
    NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetCursorPosition(
        *callbackInfo->accessibilityElement_.elementInfo_, callbackInfo->cursorPosition_);
}

void NAccessibilityElement::FindElementExecute(napi_env env, void* data)
{
    NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    switch (callbackInfo->conditionId_) {
        case FindElementCondition::FIND_ELEMENT_CONDITION_INVALID:
            HILOG_ERROR("condition id is invalid");
            callbackInfo->ret_ = RET_ERR_INVALID_PARAM;
            return;
        case FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT:
        case FindElementCondition::FIND_ELEMENT_CONDITION_TEXT_TYPE:
            FindElementByText(callbackInfo);
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE:
            {
                int32_t focusType = ConvertStringToFocusType(callbackInfo->condition_);
                HILOG_DEBUG("focusType is %{public}d", focusType);
                callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetFocusByElementInfo(
                    *(callbackInfo->accessibilityElement_.elementInfo_),
                    focusType, callbackInfo->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION:
            {
                FocusMoveDirection direction = ConvertStringToDirection(callbackInfo->condition_);
                HILOG_DEBUG("direction is %{public}d", direction);
                callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetNext(
                    *(callbackInfo->accessibilityElement_.elementInfo_), direction,
                    callbackInfo->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_ELEMENT_ID:
            {
                int64_t elementId = std::stoll(callbackInfo->condition_);
                HILOG_DEBUG("elementId is %{public}" PRId64 "", elementId);
                callbackInfo->ret_ = AccessibleAbilityClient::GetInstance()->GetByElementId(
                    elementId, callbackInfo->nodeInfo_);
            }
            break;
        default:
            break;
    }
    HILOG_DEBUG("condition id[%{public}d], result[%{public}d]", callbackInfo->conditionId_, callbackInfo->ret_);
}


void NAccessibilityElement::GetCursorPositionComplete(napi_env env, napi_status status, void* data)
{
    HILOG_DEBUG("GetCursorPositionComplete execute back");
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_create_int32(env, callbackInfo->cursorPosition_, &result[PARAM1]);
    HILOG_INFO("Response [requestId:%{public}d]", callbackInfo->cursorPosition_);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        // Callback mode
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        HILOG_INFO("NAccessibilityElement::GetCursorPositionComplete   in CallbackMode");
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        // Promise mode
        if (callbackInfo->ret_ == RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }

    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

void NAccessibilityElement::FindElementComplete(napi_env env, napi_status status, void* data)
{
    NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    napi_value result[ARGS_SIZE_TWO] = {0};
    GetElement(callbackInfo, result[PARAM1]);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        HILOG_DEBUG("callback mode. result is %{public}d", callbackInfo->ret_);
        napi_value callback = 0;
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_value undefined = 0;
        napi_get_undefined(env, &undefined);
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        HILOG_DEBUG("promise mode. result is %{public}d", callbackInfo->ret_);
        if (callbackInfo->ret_ == RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

void NAccessibilityElement::GetElement(NAccessibilityElementData *callbackInfo, napi_value &value)
{
    HILOG_DEBUG("condition id[%{public}d]", callbackInfo->conditionId_);
    napi_env env = callbackInfo->env_;
    if (callbackInfo->ret_ != RET_OK) {
        HILOG_ERROR("GetElementInfo failed!");
        napi_get_undefined(env, &value);
        return;
    }

    switch (callbackInfo->conditionId_) {
        case FindElementCondition::FIND_ELEMENT_CONDITION_INVALID:
            HILOG_ERROR("condition id is invalid");
            callbackInfo->ret_ = RET_ERR_INVALID_PARAM;
            return;
        case FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT:
            napi_create_array(env, &value);
            ConvertElementInfosToJS(env, value, callbackInfo->nodeInfos_);
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_TEXT_TYPE:
            napi_create_array(env, &value);
            ConvertElementInfosToJS(env, value, callbackInfo->nodeInfos_);
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE:
            {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &value);
                ConvertElementInfoToJS(env, value, callbackInfo->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION:
            {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &value);
                ConvertElementInfoToJS(env, value, callbackInfo->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_ELEMENT_ID:
            {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &value);
                ConvertElementInfoToJS(env, value, callbackInfo->nodeInfo_);
            }
            break;
        default:
            break;
    }
}

napi_value NAccessibilityElement::ErrorOperation(NAccessibilityElementData *callbackInfo)
{
    HILOG_DEBUG();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    if (callbackInfo->callback_) {
        HILOG_DEBUG("callback mode");
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ErrorOperation", NAPI_AUTO_LENGTH, &resource);
    if (resource == nullptr) {
        HILOG_ERROR("resource is nullptr.");
        return nullptr;
    }
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
            if (callbackInfo->callback_) {
                // Callback mode
                result[PARAM1] = undefined;
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityElement::GetCursorPosition(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        return nullptr;
    }
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return nullptr;
    }
    if (!accessibilityElement->isElementInfo_) {
        HILOG_ERROR("Type of AccessibilityElement is not right");
        return nullptr;
    }

    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    return GetCursorPositionAsync(env, argc, argv, callbackInfo, accessibilityElement);
}

FindElementCondition NAccessibilityElement::ConvertStringToCondition(const std::string &str)
{
    static const std::map<std::string, FindElementCondition> findElementConditionTable = {
        {"content", FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT},
        {"focusType", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE},
        {"focusDirection", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION},
        {"elementId", FindElementCondition::FIND_ELEMENT_CONDITION_ELEMENT_ID},
        {"textType", FindElementCondition::FIND_ELEMENT_CONDITION_TEXT_TYPE}
    };

    if (findElementConditionTable.find(str) == findElementConditionTable.end()) {
        return FindElementCondition::FIND_ELEMENT_CONDITION_INVALID;
    }

    return findElementConditionTable.at(str);
}

FocusMoveDirection NAccessibilityElement::ConvertStringToDirection(const std::string &str)
{
    static const std::map<std::string, FocusMoveDirection> focusMoveDirectionTable = {
        {"up", FocusMoveDirection::UP},
        {"down", FocusMoveDirection::DOWN},
        {"left", FocusMoveDirection::LEFT},
        {"right", FocusMoveDirection::RIGHT},
        {"forward", FocusMoveDirection::FORWARD},
        {"backward", FocusMoveDirection::BACKWARD}
    };

    if (focusMoveDirectionTable.find(str) == focusMoveDirectionTable.end()) {
        return FocusMoveDirection::DIRECTION_INVALID;
    }

    return focusMoveDirectionTable.at(str);
}

int32_t NAccessibilityElement::ConvertStringToFocusType(const std::string &str)
{
    static const std::map<std::string, int32_t> focusTypeTable = {
        {"normal", FOCUS_TYPE_INPUT},
        {"accessibility", FOCUS_TYPE_ACCESSIBILITY}
    };

    if (focusTypeTable.find(str) == focusTypeTable.end()) {
        return FOCUS_TYPE_INVALID;
    }
    return focusTypeTable.at(str);
}