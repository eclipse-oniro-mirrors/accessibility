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

#ifndef NAPI_ACCESSIBILITY_ELEMENT_H
#define NAPI_ACCESSIBILITY_ELEMENT_H

#include "accessibility_event_info.h"
#include "accessibility_window_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

const std::vector<std::string> ELEMENT_INFO_ATTRIBUTE_NAMES = {"windowId", "pageId", "accessibilityId", "componentId",
    "bundleName", "componentType", "inputType", "text", "hintText", "description", "resourceName", "childNodeIds",
    "textLengthLimit", "rect", "checkable", "checked", "focusable", "focused", "isVisible", "accessibilityFocused",
    "selected", "clickable", "longClickable", "isEnable", "isPassword", "scrollable", "editable", "popupSupported",
    "pluralLineSupported", "deleteable", "isHint", "isEssential", "itemCount", "currentIndex", "startIndex",
    "endIndex", "rangeInfo", "grid", "gridItem", "activeRegion", "isContentInvalid", "error", "label",
    "beginSelected", "endSelected", "textMoveUnit", "parent"};
const std::vector<std::string> WINDOW_INFO_ATTRIBUTE_NAMES = {"screenRect", "id", "layer", "title", "type", "childIds",
        "parentId", "isAccessibilityFocused", "isActive", "isFocused", "anchor", "rootElement", "parent", "childs"};

struct AccessibilityElement {
    ~AccessibilityElement() {
        if (elementInfo_) {
            delete elementInfo_;
            elementInfo_ = nullptr;
        }
        if (windowInfo_) {
            delete windowInfo_;
            windowInfo_ = nullptr;
        }
    }

    bool isElementInfo = true;
    OHOS::Accessibility::AccessibilityElementInfo *elementInfo_ = nullptr;
    OHOS::Accessibility::AccessibilityWindowInfo *windowInfo_ = nullptr;
};

struct NAccessibilityElementData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    AccessibilityElement accessibilityElement_ = {};
    std::string attribute_ = "";
};

enum FindElementCondition {
    FIND_ELEMENT_CONDITION_CONTENT,
    FIND_ELEMENT_CONDITION_FOCUS_TYPE,
    FIND_ELEMENT_CONDITION_FOCUS_DIRECTION,
    FIND_ELEMENT_CONDITION_PARENT,
    FIND_ELEMENT_CONDITION_CHILD,
    FIND_ELEMENT_CONDITION_INVALID
};

class NAccessibilityElement {
public:
    static void DefineJSAccessibilityElement(napi_env env);
    static napi_value JSConstructor(napi_env env, napi_callback_info info);

    static napi_value AttributeNames(napi_env env, napi_callback_info info);
    static napi_value AttributeValue(napi_env env, napi_callback_info info);
    static napi_value ActionNames(napi_env env, napi_callback_info info);
    static napi_value PerformAction(napi_env env, napi_callback_info info);
    static napi_value FindElement(napi_env env, napi_callback_info info);

    // Element info
    static napi_value GetElementInfoWindowId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPageId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoAccessibilityId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoComponentId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoBundleName(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoComponentType(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoInputType(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoText(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoHintText(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoDescription(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoResourceName(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoChildNodeIds(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoTextLengthLimit(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoRect(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoCheckable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoChecked(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoFocusable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoFocused(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsVisible(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoAccessibilityFocused(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoSelected(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoClickable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoLongClickable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsEnable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsPassword(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoScrollable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEditable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPopupSupported(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPluralLineSupported(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoDeleteable(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsHint(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsEssential(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoItemCount(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoCurrentIndex(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoStartIndex(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEndIndex(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoRangeInfo(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoGrid(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoGridItem(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoActiveRegion(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsContentInvalid(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoError(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoLabel(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoBeginSelected(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEndSelected(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoTextMoveUnit(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoParent(napi_env env, NAccessibilityElementData *callbackInfo);

    // Window info
    static napi_value GetWindowInfoScreenRect(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoLayer(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoTitle(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoType(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoChildIds(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoParentId(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoIsAccessibilityFocused(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoIsActive(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoIsFocused(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoAnchor(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoRootElement(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoChilds(napi_env env, NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoParent(napi_env env, NAccessibilityElementData *callbackInfo);

    static thread_local napi_ref consRef_;
private:
    static FindElementCondition CovertStringToDirection(std::string str);

    NAccessibilityElement() = default;
    ~NAccessibilityElement() = default;
};

typedef napi_value (*AttributeNamesFunc)(napi_env env, NAccessibilityElementData *callbackInfo);
static std::map<std::string, AttributeNamesFunc> g_elementInfoFuncMap = {
    {"windowId", &NAccessibilityElement::GetElementInfoWindowId},
    {"pageId", &NAccessibilityElement::GetElementInfoPageId},
    {"accessibilityId", &NAccessibilityElement::GetElementInfoAccessibilityId},
    {"componentId", &NAccessibilityElement::GetElementInfoComponentId},
    {"bundleName", &NAccessibilityElement::GetElementInfoBundleName},
    {"componentType", &NAccessibilityElement::GetElementInfoComponentType},
    {"inputType", &NAccessibilityElement::GetElementInfoInputType},
    {"text", &NAccessibilityElement::GetElementInfoText},
    {"hintText", &NAccessibilityElement::GetElementInfoHintText},
    {"description", &NAccessibilityElement::GetElementInfoDescription},
    {"resourceName", &NAccessibilityElement::GetElementInfoResourceName},
    {"childNodeIds", &NAccessibilityElement::GetElementInfoChildNodeIds},
    {"textLengthLimit", &NAccessibilityElement::GetElementInfoTextLengthLimit},
    {"rect", &NAccessibilityElement::GetElementInfoRect},
    {"checkable", &NAccessibilityElement::GetElementInfoCheckable},
    {"checked", &NAccessibilityElement::GetElementInfoChecked},
    {"focusable", &NAccessibilityElement::GetElementInfoFocusable},
    {"focused", &NAccessibilityElement::GetElementInfoFocused},
    {"isVisible", &NAccessibilityElement::GetElementInfoIsVisible},
    {"accessibilityFocused", &NAccessibilityElement::GetElementInfoAccessibilityFocused},
    {"selected", &NAccessibilityElement::GetElementInfoSelected},
    {"clickable", &NAccessibilityElement::GetElementInfoClickable},
    {"longClickable", &NAccessibilityElement::GetElementInfoLongClickable},
    {"isEnable", &NAccessibilityElement::GetElementInfoIsEnable},
    {"isPassword", &NAccessibilityElement::GetElementInfoIsPassword},
    {"scrollable", &NAccessibilityElement::GetElementInfoScrollable},
    {"editable", &NAccessibilityElement::GetElementInfoEditable},
    {"popupSupported", &NAccessibilityElement::GetElementInfoPopupSupported},
    {"pluralLineSupported", &NAccessibilityElement::GetElementInfoPluralLineSupported},
    {"deleteable", &NAccessibilityElement::GetElementInfoDeleteable},
    {"isHint", &NAccessibilityElement::GetElementInfoIsHint},
    {"isEssential", &NAccessibilityElement::GetElementInfoIsEssential},
    {"itemCount", &NAccessibilityElement::GetElementInfoItemCount},
    {"currentIndex", &NAccessibilityElement::GetElementInfoCurrentIndex},
    {"startIndex", &NAccessibilityElement::GetElementInfoStartIndex},
    {"endIndex", &NAccessibilityElement::GetElementInfoEndIndex},
    {"rangeInfo", &NAccessibilityElement::GetElementInfoRangeInfo},
    {"grid", &NAccessibilityElement::GetElementInfoGrid},
    {"gridItem", &NAccessibilityElement::GetElementInfoGridItem},
    {"activeRegion", &NAccessibilityElement::GetElementInfoActiveRegion},
    {"isContentInvalid", &NAccessibilityElement::GetElementInfoIsContentInvalid},
    {"error", &NAccessibilityElement::GetElementInfoError},
    {"label", &NAccessibilityElement::GetElementInfoLabel},
    {"beginSelected", &NAccessibilityElement::GetElementInfoBeginSelected},
    {"endSelected", &NAccessibilityElement::GetElementInfoEndSelected},
    {"textMoveUnit", &NAccessibilityElement::GetElementInfoTextMoveUnit},
    {"parent", &NAccessibilityElement::GetElementInfoParent},
};
static std::map<std::string, AttributeNamesFunc> g_windowInfomemberFuncMap = {
    {"screenRect", &NAccessibilityElement::GetWindowInfoScreenRect},
    {"id", &NAccessibilityElement::GetWindowInfoId},
    {"layer", &NAccessibilityElement::GetWindowInfoLayer},
    {"title", &NAccessibilityElement::GetWindowInfoTitle},
    {"type", &NAccessibilityElement::GetWindowInfoType},
    {"childIds", &NAccessibilityElement::GetWindowInfoChildIds},
    {"parentId", &NAccessibilityElement::GetWindowInfoParentId},
    {"isAccessibilityFocused", &NAccessibilityElement::GetWindowInfoIsAccessibilityFocused},
    {"isActive", &NAccessibilityElement::GetWindowInfoIsActive},
    {"isFocused", &NAccessibilityElement::GetWindowInfoIsFocused},
    {"anchor", &NAccessibilityElement::GetWindowInfoAnchor},
    {"rootElement", &NAccessibilityElement::GetWindowInfoRootElement},
    {"childs", &NAccessibilityElement::GetWindowInfoChilds},
    {"parent", &NAccessibilityElement::GetWindowInfoParent},
};
#endif // NAPI_ACCESSIBILITY_ELEMENT_H