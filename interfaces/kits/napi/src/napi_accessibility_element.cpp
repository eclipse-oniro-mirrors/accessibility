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
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

static const std::vector<std::string> ELEMENT_INFO_ATTRIBUTE_NAMES = {"windowId", "pageId", "accessibilityId",
    "componentId", "bundleName", "componentType", "inputType", "text", "hintText", "description", "resourceName",
    "childNodeIds", "textLengthLimit", "rect", "checkable", "checked", "focusable", "focused", "isVisible",
    "accessibilityFocused", "selected", "clickable", "longClickable", "isEnable", "isPassword", "scrollable",
    "editable", "popupSupported", "pluralLineSupported", "deleteable", "isHint", "isEssential", "itemCount",
    "currentIndex", "startIndex", "endIndex", "rangeInfo", "grid", "gridItem", "activeRegion", "isContentInvalid",
    "error", "label", "beginSelected", "endSelected", "textMoveUnit", "parent"};
static const std::vector<std::string> WINDOW_INFO_ATTRIBUTE_NAMES = {"screenRect", "id", "layer", "title", "type",
    "childIds", "parentId", "isAccessibilityFocused", "isActive", "isFocused", "anchor", "rootElement",
    "parent", "childs"};

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

napi_value NAccessibilityElement::AttributeNames(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ZERO;
    napi_value argv, thisVar;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    AccessibilityElement* accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return nullptr;
    }
    NAccessibilityElementData *callbackInfo = new NAccessibilityElementData();
    callbackInfo->accessibilityElement_ = *accessibilityElement;

    napi_value promise = nullptr;
    if (argc == 0) {
        HILOG_DEBUG("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    } else {
        HILOG_ERROR("The size of args is err[%{public}zu]", argc);
        return nullptr;
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "AttributeNames", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) { // Execute the complete function
            HILOG_DEBUG("execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value result[ARGS_SIZE_ONE] = {0};

            // Promise mode
            napi_create_array(env, &result[PARAM0]);
            if (callbackInfo->accessibilityElement_.isElementInfo) {
                HILOG_DEBUG("covert element info to js");
                ConvertStringVecToJS(env, result[PARAM0], ELEMENT_INFO_ATTRIBUTE_NAMES);
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM0]);
            } else {
                HILOG_DEBUG("covert window info to js");
                ConvertStringVecToJS(env, result[PARAM0], WINDOW_INFO_ATTRIBUTE_NAMES);
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM0]);
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
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    if (argc != ARGS_SIZE_ONE) {
        HILOG_ERROR("argc is not one!");
        return nullptr;
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
        return nullptr;
    }

    NAccessibilityElementData *callbackInfo = new NAccessibilityElementData();
    callbackInfo->accessibilityElement_ = *accessibilityElement;
    callbackInfo->attribute_ = attribute;

    if (accessibilityElement->isElementInfo) {
        HILOG_DEBUG("It is element info");
        auto memFunc = g_elementInfoFuncMap.find(attribute);
        if (memFunc != g_elementInfoFuncMap.end()) {
            auto func = memFunc->second;
            if (func != nullptr) {
                return (*func)(env, callbackInfo);
            }
        }
        HILOG_ERROR("There is no the attribute[%{public}s] in element info", attribute.c_str());
    } else {
        HILOG_DEBUG("It is window info");
        auto memFunc = g_windowInfomemberFuncMap.find(attribute);
        if (memFunc != g_windowInfomemberFuncMap.end()) {
            auto func = memFunc->second;
            if (func != nullptr) {
                return (*func)(env, callbackInfo);
            }
        }
        HILOG_ERROR("There is no the attribute[%{public}s] in window info", attribute.c_str());
    }
    return nullptr;
}

napi_value NAccessibilityElement::ActionNames(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::PerformAction(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::FindElement(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ZERO;
    napi_value argv[ARGS_SIZE_TWO], thisVar;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    if (argc != ARGS_SIZE_TWO) {
        HILOG_ERROR("argc is not two!");
        return nullptr;
    }

    AccessibilityElement *accessibilityElement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&accessibilityElement);
    if (!accessibilityElement || status != napi_ok) {
        HILOG_ERROR("accessibilityElement is null or status[%{public}d] is wrong", status);
        return nullptr;
    }

    std::string conditionType;
    if (!ParseString(env, conditionType, argv[PARAM0])) {
        HILOG_ERROR("ParseString failed");
        return nullptr;
    }
    FindElementCondition condition = NAccessibilityElement::CovertStringToDirection(conditionType);
    if (condition == FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT) {
    }
    return nullptr;
}

FindElementCondition NAccessibilityElement::CovertStringToDirection(std::string str)
{
    static const std::map<std::string, FindElementCondition> findElementConditionTable = {
        {"content", FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT},
        {"focusType", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_TYPE},
        {"focusDirection", FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION},
        {"parent", FindElementCondition::FIND_ELEMENT_CONDITION_PARENT},
        {"child", FindElementCondition::FIND_ELEMENT_CONDITION_CHILD}
    };

    if (findElementConditionTable.find(str) == findElementConditionTable.end()) {
        return FindElementCondition::FIND_ELEMENT_CONDITION_INVALID;
    }

    return findElementConditionTable.at(str);
}

napi_value NAccessibilityElement::GetElementInfoWindowId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    napi_value promise = nullptr;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetElementInfoWindowId", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetElementInfoWindowId execute back");
            NAccessibilityElementData* callbackInfo = static_cast<NAccessibilityElementData*>(data);
            napi_value argv;
            int32_t windowId = callbackInfo->accessibilityElement_.elementInfo_->GetWindowId();
            HILOG_DEBUG("windowId: [%{public}d]", windowId);
            napi_get_boolean(env, windowId, &argv);
            napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetElementInfoPageId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoAccessibilityId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoComponentId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoBundleName(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoComponentType(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoInputType(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoText(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoHintText(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoDescription(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoResourceName(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoChildNodeIds(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoTextLengthLimit(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoRect(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoCheckable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoChecked(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoFocusable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoFocused(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsVisible(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoAccessibilityFocused(napi_env env,
    NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoSelected(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoClickable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoLongClickable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsEnable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsPassword(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoScrollable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoEditable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoPopupSupported(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoPluralLineSupported(napi_env env,
    NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoDeleteable(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsHint(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsEssential(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoItemCount(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoCurrentIndex(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoStartIndex(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoEndIndex(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoRangeInfo(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoGrid(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoGridItem(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoActiveRegion(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoIsContentInvalid(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoError(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoLabel(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoBeginSelected(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoEndSelected(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoTextMoveUnit(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetElementInfoParent(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoScreenRect(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    napi_value promise = nullptr;
    napi_create_promise(env, &callbackInfo->deferred_, &promise);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindowInfoScreenRect", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindowInfoScreenRect execute back");
            NAccessibilityElementData* callbackInfo = (NAccessibilityElementData*)data;
            napi_value argv;
            napi_create_object(env, &argv);
            Accessibility::Rect screenRect = callbackInfo->accessibilityElement_.windowInfo_->GetRectInScreen();
            ConvertRectToJS(env, argv, screenRect);
            napi_resolve_deferred(env, callbackInfo->deferred_, argv);
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityElement::GetWindowInfoId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoLayer(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoTitle(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoType(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoChildIds(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoParentId(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoIsAccessibilityFocused(napi_env env,
    NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoIsActive(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoIsFocused(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoAnchor(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoRootElement(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoChilds(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}

napi_value NAccessibilityElement::GetWindowInfoParent(napi_env env, NAccessibilityElementData *callbackInfo)
{
    HILOG_INFO("start");
    return nullptr;
}