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
#include "accessible_ability_client.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

namespace {
    const std::vector<std::string> ELEMENT_INFO_ATTRIBUTE_NAMES = {"componentId", "pageId", "parentId", "inspectorKey",
        "bundleName", "componentType", "inputType", "text", "hintText", "description", "resourceName",
        "childNodeIds", "textLengthLimit", "rect", "checkable", "checked", "focusable", "isVisible",
        "accessibilityFocused", "selected", "clickable", "longClickable", "isEnable", "isPassword", "scrollable",
        "editable", "popupSupported", "pluralLineSupported", "deleteable", "isHint", "isEssential", "itemCount",
        "currentIndex", "startIndex", "endIndex", "rangeInfo", "grid", "gridItem", "activeRegion", "isContentInvalid",
        "error", "label", "beginSelected", "endSelected", "textMoveUnit", "parent", "children", "triggerAction",
        "contents", "lastContent", "windowId", "isFocused"};
    const std::vector<std::string> WINDOW_INFO_ATTRIBUTE_NAMES = {"isActive", "screenRect", "layer", "type", "anchor",
        "rootElement", "isFocused", "windowId"};
} // namespace

thread_local napi_ref NAccessibilityElement::consRef_ = nullptr;

void NAccessibilityElement::DefineJSAccessibilityElement(napi_env env)
{
    napi_property_descriptor descForAccessibilityElement[] = {
        DECLARE_NAPI_FUNCTION("attributeNames", NAccessibilityElement::AttributeNames),
        DECLARE_NAPI_FUNCTION("attributeValue", NAccessibilityElement::AttributeValue),
        DECLARE_NAPI_FUNCTION("actionNames", NAccessibilityElement::ActionNames),
        DECLARE_NAPI_FUNCTION("performAction", NAccessibilityElement::PerformAction),
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
    size_t index = 0;
    HILOG_DEBUG("elementInfo size(%{public}zu)", elementInfos.size());

    napi_value constructor = nullptr;
    napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);

    for (auto& elementInfo : elementInfos) {
        napi_value obj = nullptr;
        napi_status status = napi_new_instance(env, constructor, 0, nullptr, &obj);
        HILOG_INFO("status is %{public}d", status);
        ConvertElementInfoToJS(env, obj, elementInfo);
        napi_set_element(env, result, index, obj);
        index++;
    }
}

napi_value NAccessibilityElement::AttributeNames(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ZERO;
    napi_value argv, thisVar;
    void* data = nullptr;
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    napi_value promise = nullptr;
    if (argc == 0) {
        HILOG_DEBUG("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    } else {
        HILOG_ERROR("The size of args is err[%{public}zu]", argc);
        return ErrorOperation(callbackInfo);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "AttributeNames", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) { // Execute the complete function
            HILOG_DEBUG("execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            // Promise mode
            napi_create_array(env, &result);
            if (callbackInfo->accessibilityElement_.isElementInfo_) {
                HILOG_DEBUG("covert element info to js");
                ConvertStringVecToJS(env, result, ELEMENT_INFO_ATTRIBUTE_NAMES);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_DEBUG("covert window info to js");
                ConvertStringVecToJS(env, result, WINDOW_INFO_ATTRIBUTE_NAMES);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::AttributeValue(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    if (argc != ARGS_SIZE_ONE) {
        HILOG_ERROR("argc is not one!");
        return ErrorOperation(callbackInfo);
    }

    // Parse attribute name
    std::string attribute = "";
    ParseString(env, attribute, argv[PARAM0]);
    HILOG_INFO("attribute = %{public}s", attribute.c_str());

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    if (accessibilityElement->isElementInfo_) {
        HILOG_DEBUG("It is element info");
        auto memFunc = g_elementInfoFuncMap.find(attribute);
        if (memFunc != g_elementInfoFuncMap.end()) {
            auto func = memFunc->second;
            if (func != nullptr) {
                return (*func)(callbackInfo);
            }
        }
        HILOG_ERROR("There is no the attribute[%{public}s] in element info", attribute.c_str());
    } else {
        HILOG_DEBUG("It is window info");
        auto memFunc = g_windowInfomemberFuncMap.find(attribute);
        if (memFunc != g_windowInfomemberFuncMap.end()) {
            auto func = memFunc->second;
            if (func != nullptr) {
                return (*func)(callbackInfo);
            }
        }
        HILOG_ERROR("There is no the attribute[%{public}s] in window info", attribute.c_str());
    }
    return ErrorOperation(callbackInfo);
}

napi_value NAccessibilityElement::ActionNames(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ZERO;
    napi_value argv, thisVar;
    void* data = nullptr;
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}zu", argc);

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

    napi_value promise = nullptr;
    if (argc == 0) {
        HILOG_DEBUG("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    } else {
        HILOG_ERROR("The size of args is err[%{public}zu]", argc);
        return ErrorOperation(callbackInfo);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ActionNames", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                std::vector<AccessibleAction> operations =
                    callbackInfo->accessibilityElement_.elementInfo_->GetActionList();
                HILOG_DEBUG("action list size is %{public}zu", operations.size());
                for (auto &operation : operations) {
                    callbackInfo->actionNames_.push_back(ConvertOperationTypeToString(operation.GetActionType()));
                }
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        [](napi_env env, napi_status status, void* data) { // Execute the complete function
            HILOG_DEBUG("execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            // Promise mode
            if (callbackInfo->ret_) {
                napi_create_array(env, &result);
                ConvertStringVecToJS(env, result, callbackInfo->actionNames_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("get action names failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::PerformAction(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar;
    void* data = nullptr;
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}zu", argc);
    if (argc != ARGS_SIZE_ONE && argc != ARGS_SIZE_TWO) {
        HILOG_ERROR("argc's size[%{public}zu] is wrong!", argc);
        return ErrorOperation(callbackInfo);
    }

    // Parse action name
    std::string actionName;
    ParseString(env, actionName, argv[PARAM0]);
    HILOG_INFO("actionName = %{public}s", actionName.c_str());

    // Parse action args
    std::map<std::string, std::string> actionArguments;
    if (argc == ARGS_SIZE_TWO) {
        ConvertActionArgsJSToNAPI(env, argv[argc - 1], actionArguments,
            ConvertStringToAccessibleOperationType(actionName));
    }

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    if (!(accessibilityElement->isElementInfo_ && accessibilityElement->elementInfo_)) {
        HILOG_ERROR("accessibilityElement is wrong. isElementInfo_[%{public}d]", accessibilityElement->isElementInfo_);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;
    callbackInfo->stringData_ = actionName;
    callbackInfo->actionArguments_ = actionArguments;
    napi_value promise = nullptr;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "PerformAction", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            ActionType action = ConvertStringToAccessibleOperationType(callbackInfo->stringData_);
            sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
            if (aaClient) {
                callbackInfo->ret_ = aaClient->ExecuteAction(
                    *callbackInfo->accessibilityElement_.elementInfo_, action, callbackInfo->actionArguments_);
                HILOG_DEBUG("callbackInfo->ret_[%{public}d]", callbackInfo->ret_);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("ExecuteAction execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            napi_value ret = 0;
            napi_get_boolean(callbackInfo->env_, callbackInfo->ret_, &ret);
            napi_resolve_deferred(env, callbackInfo->deferred_, ret);
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::FindElement(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar;
    void* data = nullptr;
    NAccessibilityElementData *callbackInfo = new(std::nothrow) NAccessibilityElementData();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    callbackInfo->env_ = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    if (argc != ARGS_SIZE_TWO) {
        HILOG_ERROR("argc is not two!");
        return ErrorOperation(callbackInfo);
    }

    // Parse conditionType name
    std::string conditionType = "";
    ParseString(env, conditionType, argv[PARAM0]);
    HILOG_INFO("conditionType = %{public}s", conditionType.c_str());

    // Parse queryData name
    std::string queryData = "";
    ParseString(env, queryData, argv[PARAM1]);
    HILOG_INFO("queryData = %{public}s", queryData.c_str());

    // Unwrap AccessibilityElement
    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return ErrorOperation(callbackInfo);
    }
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    FindElementCondition condition = ConvertStringToCondition(conditionType);
    switch (condition) {
        case FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT:
            if (!ParseString(env, queryData, argv[PARAM1])) {
                HILOG_ERROR("ParseString for argv[1] failed");
                return ErrorOperation(callbackInfo);
            }
            if (!accessibilityElement->isElementInfo_) {
                HILOG_ERROR("Type of AccessibilityElement is not right");
                return ErrorOperation(callbackInfo);
            }
            callbackInfo->stringData_ = queryData;
            return GetElementInfoByContent(callbackInfo);
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE:
            if (!ParseString(env, queryData, argv[PARAM1])) {
                HILOG_ERROR("ParseString for argv[1] failed");
                return ErrorOperation(callbackInfo);
            }
            if (!accessibilityElement->isElementInfo_) {
                HILOG_ERROR("Type of AccessibilityElement is not right");
                return ErrorOperation(callbackInfo);
            }
            callbackInfo->stringData_ = queryData;
            return GetElementInfoByFocusType(callbackInfo);
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION:
            if (!ParseString(env, queryData, argv[PARAM1])) {
                HILOG_ERROR("ParseString for argv[1] failed");
                return ErrorOperation(callbackInfo);
            }
            if (!accessibilityElement->isElementInfo_) {
                HILOG_ERROR("Type of AccessibilityElement is not right");
                return ErrorOperation(callbackInfo);
            }
            callbackInfo->stringData_ = queryData;
            return GetElementInfoByFocusDirection(callbackInfo);
        default:
            HILOG_ERROR("condition is not right");
            break;
    }
    return ErrorOperation(callbackInfo);
}

napi_value NAccessibilityElement::ErrorOperation(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ErrorOperation", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            napi_value undefined = nullptr;
            napi_get_undefined(env, &undefined);
            napi_reject_deferred(env, callbackInfo->deferred_, undefined);
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoWindowId(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoWindowId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetWindowId();
                HILOG_DEBUG("windowId: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoWindowId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            napi_value argv;
            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get windowId failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoPageId(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoPageId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetPageId();
                HILOG_DEBUG("GetElementInfoPageId: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoPageId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("GetElementInfoPageId failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoParentId(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoParentId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetParentNodeId();
                HILOG_DEBUG("parentId: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoParentId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("GetElementInfoParentId failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoInspectorKey(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoInspectorKey", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetInspectorKey();
                HILOG_DEBUG("inspectorKey: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoInspectorKey execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get inspectorKey failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoBundleName(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoBundleName", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetBundleName();
                HILOG_DEBUG("bundle name: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoBundleName execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get bundle name failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoComponentType(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoComponentType", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetComponentType();
                HILOG_DEBUG("component type: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoComponentType execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get component type failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoInputType(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoInputType", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetInputType();
                HILOG_DEBUG("input type: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoInputType execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get input type failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoText(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoText", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetContent();
                HILOG_DEBUG("text: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoText execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get text failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoHintText(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoHintText", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetHint();
                HILOG_DEBUG("hint text: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoHintText execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get hint text failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoDescription(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoDescription", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetDescriptionInfo();
                HILOG_DEBUG("description: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoDescription execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get description failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoResourceName(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoResourceName", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetComponentResourceId();
                HILOG_DEBUG("resource name: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoResourceName execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get resource name failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoChildNodeIds(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoChildNodeIds", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->childIds_ = callbackInfo->accessibilityElement_.elementInfo_->GetChildIds();
                HILOG_DEBUG("childIds size: [%{public}zu]", callbackInfo->childIds_.size());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoChildNodeIds execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_array(env, &argv);
                size_t index = 0;
                for (auto& childId : callbackInfo->childIds_) {
                    napi_value id = nullptr;
                    napi_create_int32(callbackInfo->env_, childId, &id);
                    napi_set_element(env, argv, index, id);
                    index++;
                }
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get childIds failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoTextLengthLimit(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoTextLengthLimit", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetTextLengthLimit();
                HILOG_DEBUG("textLengthLimit: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoTextLengthLimit execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get textLengthLimit failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoRect(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoRect", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->screenRect_ = callbackInfo->accessibilityElement_.elementInfo_->GetRectInScreen();
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoRect execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_object(env, &argv);
                ConvertRectToJS(env, argv, callbackInfo->screenRect_);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("get rect failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoCheckable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoCheckable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsCheckable();
                HILOG_DEBUG("GetElementInfoCheckable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoCheckable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoCheckable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoChecked(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoChecked", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsChecked();
                HILOG_DEBUG("GetElementInfoChecked: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoChecked execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoChecked failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoFocusable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoFocusable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsFocusable();
                HILOG_DEBUG("GetElementInfoFocusable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoFocusable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoFocusable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsVisible(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsVisible", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsVisible();
                HILOG_DEBUG("GetElementInfoIsVisible: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsVisible execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsVisible failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoAccessibilityFocused(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoAccessibilityFocused", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->HasAccessibilityFocus();
                HILOG_DEBUG("GetElementInfoAccessibilityFocused: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoAccessibilityFocused execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoAccessibilityFocused failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoSelected(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoSelected", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsSelected();
                HILOG_DEBUG("GetElementInfoSelected: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoSelected execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoSelected failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoClickable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoClickable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsClickable();
                HILOG_DEBUG("GetElementInfoClickable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoClickable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoClickable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoLongClickable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoLongClickable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsLongClickable();
                HILOG_DEBUG("GetElementInfoLongClickable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoLongClickable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoLongClickable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsEnable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsEnable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsEnabled();
                HILOG_DEBUG("GetElementInfoIsEnable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsEnable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsEnable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsPassword(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsPassword", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsPassword();
                HILOG_DEBUG("GetElementInfoIsPassword: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsPassword execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsPassword failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoScrollable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoScrollable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsScrollable();
                HILOG_DEBUG("GetElementInfoScrollable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoScrollable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoScrollable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoEditable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoEditable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsEditable();
                HILOG_DEBUG("GetElementInfoEditable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoEditable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoEditable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoPopupSupported(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoPopupSupported", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsPopupSupported();
                HILOG_DEBUG("GetElementInfoPopupSupported: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoPopupSupported execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoPopupSupported failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoPluralLineSupported(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoPluralLineSupported", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsPluraLineSupported();
                HILOG_DEBUG("GetElementInfoPluralLineSupported: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoPluralLineSupported execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoPluralLineSupported failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoDeleteable(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoDeleteable", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsDeletable();
                HILOG_DEBUG("GetElementInfoDeleteable: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoDeleteable execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoDeleteable failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsHint(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsHint", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsGivingHint();
                HILOG_DEBUG("GetElementInfoIsHint: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsHint execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsHint failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsEssential(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsEssential", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsEssential();
                HILOG_DEBUG("GetElementInfoIsEssential: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsEssential execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsEssential failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoItemCount(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoItemCount", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetItemCounts();
                HILOG_DEBUG("GetElementInfoItemCount: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoItemCount execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoItemCount failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoCurrentIndex(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoCurrentIndex", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetCurrentIndex();
                HILOG_DEBUG("GetElementInfoCurrentIndex: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoCurrentIndex execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoCurrentIndex failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoStartIndex(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoStartIndex", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetBeginIndex();
                HILOG_DEBUG("GetElementInfoStartIndex: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoStartIndex execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoStartIndex failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoEndIndex(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoEndIndex", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetEndIndex();
                HILOG_DEBUG("GetElementInfoEndIndex: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoEndIndex execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoEndIndex failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoRangeInfo(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoRangeInfo", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->rangeInfo_ = callbackInfo->accessibilityElement_.elementInfo_->GetRange();
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoRangeInfo execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_object(callbackInfo->env_, &result);
                ConvertRangeInfoToJS(callbackInfo->env_, result, callbackInfo->rangeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoRangeInfo failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoGrid(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoGrid", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->gridInfo_ = callbackInfo->accessibilityElement_.elementInfo_->GetGrid();
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoGrid execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_object(callbackInfo->env_, &result);
                ConvertGridInfoToJS(callbackInfo->env_, result, callbackInfo->gridInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoGrid failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoGridItem(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoGridItem", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->gridItemInfo_ = callbackInfo->accessibilityElement_.elementInfo_->GetGridItem();
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoGridItem execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_object(callbackInfo->env_, &result);
                ConvertGridItemToJS(callbackInfo->env_, result, callbackInfo->gridItemInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoGridItem failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoActiveRegion(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoActiveRegion", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetLiveRegion();
                HILOG_DEBUG("GetElementInfoActiveRegion: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoActiveRegion execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoActiveRegion failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsContentInvalid(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsContentInvalid", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->GetContentInvalid();
                HILOG_DEBUG("GetElementInfoIsContentInvalid: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsContentInvalid execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsContentInvalid failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoError(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoError", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetContentInvalid();
                HILOG_DEBUG("GetElementInfoError: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoError execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(callbackInfo->env_, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH,
                    &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoError failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoLabel(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoLabel", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ =
                    callbackInfo->accessibilityElement_.elementInfo_->GetLabeledAccessibilityId();
                HILOG_DEBUG("GetElementInfoLabel: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoLabel execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoLabel failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoBeginSelected(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoBeginSelected", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetSelectedBegin();
                HILOG_DEBUG("GetElementInfoBeginSelected: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoBeginSelected execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoBeginSelected failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoEndSelected(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoEndSelected", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetSelectedEnd();
                HILOG_DEBUG("GetElementInfoEndSelected: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoEndSelected execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoEndSelected failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoTextMoveUnit(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoTextMoveUnit", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->stringData_ = ConvertTextMoveUnitToString(
                    callbackInfo->accessibilityElement_.elementInfo_->GetTextMovementStep());
                HILOG_DEBUG("GetElementInfoTextMoveUnit: [%{public}s]", callbackInfo->stringData_.c_str());
                if (callbackInfo->stringData_ == "") {
                    callbackInfo->ret_ = false;
                    HILOG_ERROR("no stringData_");
                }
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoTextMoveUnit execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(callbackInfo->env_, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH,
                    &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoTextMoveUnit failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoParent(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoParent", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
                if (!aaClient) {
                    HILOG_ERROR("aaClient is nullptr");
                    callbackInfo->ret_ = false;
                } else {
                    callbackInfo->ret_ = aaClient->GetParentElementInfo(
                        *callbackInfo->accessibilityElement_.elementInfo_, callbackInfo->nodeInfo_);
                }
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoParent execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &result);
                ConvertElementInfoToJS(env, result, callbackInfo->nodeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoParent failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoChildren(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoChildren", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
                if (!aaClient) {
                    HILOG_ERROR("aaClient is nullptr");
                    callbackInfo->ret_ = false;
                } else {
                    callbackInfo->ret_ = aaClient->GetChildren(
                        *callbackInfo->accessibilityElement_.elementInfo_, callbackInfo->nodeInfos_);
                }
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoChildren execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_array(env, &result);
                ConvertElementInfosToJS(env, result, callbackInfo->nodeInfos_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoChildren failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoTriggerAction(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoTriggerAction", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->stringData_ = ConvertOperationTypeToString(
                    callbackInfo->accessibilityElement_.elementInfo_->GetTriggerAction());
                HILOG_DEBUG("GetElementInfoTriggerAction: [%{public}s]", callbackInfo->stringData_.c_str());
                if (callbackInfo->stringData_ == "") {
                    callbackInfo->ret_ = false;
                    HILOG_ERROR("no stringData_");
                }
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoTriggerAction execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(callbackInfo->env_, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH,
                    &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoTriggerAction failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoContents(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoContents", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo && callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->accessibilityElement_.elementInfo_->GetContentList(callbackInfo->contents_);
                HILOG_DEBUG("contents size: [%{public}zu]", callbackInfo->contents_.size());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoContents execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_array(env, &argv);
                size_t index = 0;
                for (auto& content : callbackInfo->contents_) {
                    napi_value nContent = nullptr;
                    napi_create_string_utf8(env, content.c_str(), NAPI_AUTO_LENGTH, &nContent);
                    napi_set_element(env, argv, index, nContent);
                    index++;
                }
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("GetElementInfoContents failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoLastContent(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoLastContent", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_) {
                callbackInfo->stringData_ = callbackInfo->accessibilityElement_.elementInfo_->GetLatestContent();
                HILOG_DEBUG("last content: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoLastContent execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &argv);
                napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            } else {
                HILOG_ERROR("GetElementInfoLastContent failed!");
                napi_get_undefined(env, &argv);
                napi_reject_deferred(env, callbackInfo->deferred_, argv);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoIsFocused(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoIsFocused", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.elementInfo_->IsFocused();
                HILOG_DEBUG("GetElementInfoIsFocused: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoIsFocused execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoIsFocused failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoComponentId(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoComponentId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.elementInfo_->GetAccessibilityId();
                HILOG_DEBUG("GetElementInfoComponentId: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no elementInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoComponentId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetElementInfoComponentId failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoIsActive(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoIsActive", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.windowInfo_->IsActive();
                HILOG_DEBUG("GetWindowInfoIsActive: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoIsActive execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoIsActive failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoScreenRect(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoScreenRect", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->screenRect_ = callbackInfo->accessibilityElement_.windowInfo_->GetRectInScreen();
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoScreenRect execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_object(env, &result);
                ConvertRectToJS(env, result, callbackInfo->screenRect_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoScreenRect failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoLayer(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoLayer", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.windowInfo_->GetWindowLayer();
                HILOG_DEBUG("GetWindowInfoLayer: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoLayer execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoLayer failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoType(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoType", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->stringData_ =
                    ConvertWindowTypeToString(
                        callbackInfo->accessibilityElement_.windowInfo_->GetAccessibilityWindowType());
                HILOG_DEBUG("GetWindowInfoType: [%{public}s]", callbackInfo->stringData_.c_str());
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoType execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_string_utf8(env, callbackInfo->stringData_.c_str(), NAPI_AUTO_LENGTH, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoType failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoAnchor(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoAnchor", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
                if (!aaClient) {
                    HILOG_ERROR("aaClient is nullptr");
                    callbackInfo->ret_ = false;
                } else {
                    callbackInfo->ret_ = aaClient->GetAnchor(*callbackInfo->accessibilityElement_.windowInfo_,
                        callbackInfo->nodeInfo_);
                    if (callbackInfo->ret_ == false) {
                        HILOG_ERROR("GetAnchor failed");
                    }
                }
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoAnchor execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &result);
                ConvertElementInfoToJS(env, result, callbackInfo->nodeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoAnchor failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoRootElement(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoRootElement", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
                if (!aaClient) {
                    HILOG_ERROR("aaClient is nullptr");
                    callbackInfo->ret_ = false;
                } else {
                    callbackInfo->ret_ = aaClient->GetRootByWindow(*callbackInfo->accessibilityElement_.windowInfo_,
                        callbackInfo->nodeInfo_);
                    if (callbackInfo->ret_ == false) {
                        HILOG_ERROR("GetRootByWindow failed");
                    }
                }
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoRootElement execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &result);
                ConvertElementInfoToJS(env, result, callbackInfo->nodeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoRootElement failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoIsFocused(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoIsFocused", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->boolData_ = callbackInfo->accessibilityElement_.windowInfo_->IsFocused();
                HILOG_DEBUG("GetWindowInfoIsFocused: [%{public}d]", callbackInfo->boolData_);
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoIsFocused execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_get_boolean(callbackInfo->env_, callbackInfo->boolData_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoIsFocused failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoWindowId(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    napi_env env = callbackInfo->env_;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoWindowId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->accessibilityElement_.windowInfo_ != nullptr) {
                callbackInfo->int32Data_ = callbackInfo->accessibilityElement_.windowInfo_->GetWindowId();
                HILOG_DEBUG("GetWindowInfoWindowId: [%{public}d]", callbackInfo->int32Data_);
            } else {
                HILOG_ERROR("no windowInfo_");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoWindowId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_int32(callbackInfo->env_, callbackInfo->int32Data_, &result);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("GetWindowInfoWindowId failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoByContent(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    napi_create_promise(callbackInfo->env_, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(callbackInfo->env_, "GetElementInfoByContent", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(callbackInfo->env_, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
            if (aaClient != nullptr && callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->ret_ = aaClient->GetByContent(
                    *(callbackInfo->accessibilityElement_.elementInfo_), callbackInfo->stringData_,
                    callbackInfo->nodeInfos_);
            } else {
                HILOG_ERROR("no client or element info!");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoByContent execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_create_array(env, &result);
                ConvertElementInfosToJS(env, result, callbackInfo->nodeInfos_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("get element info failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(callbackInfo->env_, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoByFocusType(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    napi_create_promise(callbackInfo->env_, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(callbackInfo->env_, "GetElementInfoByFocusType", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(callbackInfo->env_, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            int32_t focusType = ConvertStringToFocusType(callbackInfo->stringData_);
            sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
            if (aaClient != nullptr && callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->ret_ = aaClient->GetFocusByElementInfo(
                    *(callbackInfo->accessibilityElement_.elementInfo_), focusType, callbackInfo->nodeInfo_);
            } else {
                HILOG_ERROR("no client or element info!");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoByFocusType execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &result);
                ConvertElementInfoToJS(env, result, callbackInfo->nodeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("get element info failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(callbackInfo->env_, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoByFocusDirection(NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO();
    napi_value promise = nullptr;
    napi_create_promise(callbackInfo->env_, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(callbackInfo->env_, "GetElementInfoByFocusDirection", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(callbackInfo->env_, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityElementData *callbackInfo = static_cast<NAccessibilityElementData*>(data);
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            FocusMoveDirection direction = ConvertStringToDirection(callbackInfo->stringData_);
            sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
            if (aaClient != nullptr && callbackInfo->accessibilityElement_.elementInfo_ != nullptr) {
                callbackInfo->ret_ = aaClient->GetNext(*(callbackInfo->accessibilityElement_.elementInfo_),
                    direction, callbackInfo->nodeInfo_);
            } else {
                HILOG_ERROR("no client or element info!");
                callbackInfo->ret_ = false;
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoByFocusDirection execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result = nullptr;
            if (!callbackInfo) {
                HILOG_ERROR("callbackInfo is nullptr");
                return;
            }

            if (callbackInfo->ret_) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_new_instance(env, constructor, 0, nullptr, &result);
                ConvertElementInfoToJS(env, result, callbackInfo->nodeInfo_);
                napi_resolve_deferred(env, callbackInfo->deferred_, result);
            } else {
                HILOG_ERROR("get element info failed!");
                napi_get_undefined(env, &result);
                napi_reject_deferred(env, callbackInfo->deferred_, result);
            }
            
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(callbackInfo->env_, callbackInfo->work_);
    return promise;
}

FindElementCondition NAccessibilityElement::ConvertStringToCondition(const std::string &str)
{
    static const std::map<std::string, FindElementCondition> findElementConditionTable = {
        {"content", FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT},
        {"focusType", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE},
        {"focusDirection", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION}
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