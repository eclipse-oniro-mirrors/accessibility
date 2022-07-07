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

struct AccessibilityElement {
    AccessibilityElement() = default;
    AccessibilityElement(std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> elementInfo)
        : elementInfo_(elementInfo), isElementInfo_(true)
    {}
    AccessibilityElement(std::shared_ptr<OHOS::Accessibility::AccessibilityWindowInfo> windowInfo)
        : windowInfo_(windowInfo), isElementInfo_(false)
    {}

    std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> elementInfo_ = nullptr;
    std::shared_ptr<OHOS::Accessibility::AccessibilityWindowInfo> windowInfo_ = nullptr;
    bool isElementInfo_ = true;
};

struct NAccessibilityElementData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_env env_ {};
    AccessibilityElement accessibilityElement_ = {};
    std::map<std::string, std::string> actionArguments_;
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> nodeInfos_ {};
    OHOS::Accessibility::AccessibilityElementInfo nodeInfo_ {};
    std::string actionName_ = "";
    std::string condition_ = "";
    bool ret_ = false;
};

enum FindElementCondition {
    FIND_ELEMENT_CONDITION_CONTENT,
    FIND_ELEMENT_CONDITION_FOCUS_TYPE,
    FIND_ELEMENT_CONDITION_FOCUS_DIRECTION,
    FIND_ELEMENT_CONDITION_INVALID
};

class NAccessibilityElement {
public:
    static void DefineJSAccessibilityElement(napi_env env);
    static napi_value JSConstructor(napi_env env, napi_callback_info info);

    static void ConvertElementInfoToJS(napi_env env, napi_value result,
        const OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
    static void ConvertElementInfosToJS(napi_env env, napi_value result,
        const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);

    static napi_value AttributeNames(napi_env env, napi_callback_info info);
    static napi_value AttributeValue(napi_env env, napi_callback_info info);
    static napi_value ActionNames(napi_env env, napi_callback_info info);
    static napi_value PerformAction(napi_env env, napi_callback_info info);
    static napi_value FindElement(napi_env env, napi_callback_info info);
    static napi_value ErrorOperation(NAccessibilityElementData *callbackInfo);

    // Element info
    static napi_value GetElementInfoComponentId(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPageId(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoParentId(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoInspectorKey(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoBundleName(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoComponentType(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoInputType(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoText(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoHintText(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoDescription(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoResourceName(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoChildNodeIds(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoTextLengthLimit(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoRect(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoCheckable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoChecked(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoFocusable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsVisible(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoAccessibilityFocused(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoSelected(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoClickable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoLongClickable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsEnable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsPassword(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoScrollable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEditable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPopupSupported(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoPluralLineSupported(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoDeleteable(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsHint(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsEssential(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoItemCount(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoCurrentIndex(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoStartIndex(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEndIndex(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoRangeInfo(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoGrid(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoGridItem(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoActiveRegion(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsContentInvalid(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoError(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoLabel(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoBeginSelected(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoEndSelected(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoTextMoveUnit(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoParent(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoChildren(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoTriggerAction(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoContents(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoLastContent(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoWindowId(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoIsFocused(NAccessibilityElementData *callbackInfo);

    // Window info
    static napi_value GetWindowInfoIsActive(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoScreenRect(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoLayer(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoType(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoAnchor(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoRootElement(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoIsFocused(NAccessibilityElementData *callbackInfo);
    static napi_value GetWindowInfoWindowId(NAccessibilityElementData *callbackInfo);

    // Element info by condition
    static napi_value GetElementInfoByContent(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoByFocusType(NAccessibilityElementData *callbackInfo);
    static napi_value GetElementInfoByFocusDirection(NAccessibilityElementData *callbackInfo);

    static thread_local napi_ref consRef_;
private:
    static FindElementCondition ConvertStringToCondition(const std::string &str);
    static OHOS::Accessibility::FocusMoveDirection ConvertStringToDirection(const std::string &str);
    static int32_t ConvertStringToFocusType(const std::string &str);

    NAccessibilityElement() = default;
    ~NAccessibilityElement() = default;
};

typedef napi_value (*AttributeNamesFunc)(NAccessibilityElementData *callbackInfo);
static std::map<std::string, AttributeNamesFunc> g_elementInfoFuncMap = {
    {"componentId", &NAccessibilityElement::GetElementInfoComponentId},
    {"pageId", &NAccessibilityElement::GetElementInfoPageId},
    {"parentId", &NAccessibilityElement::GetElementInfoParentId},
    {"inspectorKey", &NAccessibilityElement::GetElementInfoInspectorKey},
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
    {"children", &NAccessibilityElement::GetElementInfoChildren},
    {"triggerAction", &NAccessibilityElement::GetElementInfoTriggerAction},
    {"contents", &NAccessibilityElement::GetElementInfoContents},
    {"lastContent", &NAccessibilityElement::GetElementInfoLastContent},
    {"windowId", &NAccessibilityElement::GetElementInfoWindowId},
    {"isFocused", &NAccessibilityElement::GetElementInfoIsFocused},
};
static std::map<std::string, AttributeNamesFunc> g_windowInfomemberFuncMap = {
    {"isActive", &NAccessibilityElement::GetWindowInfoIsActive},
    {"screenRect", &NAccessibilityElement::GetWindowInfoScreenRect},
    {"layer", &NAccessibilityElement::GetWindowInfoLayer},
    {"type", &NAccessibilityElement::GetWindowInfoType},
    {"anchor", &NAccessibilityElement::GetWindowInfoAnchor},
    {"rootElement", &NAccessibilityElement::GetWindowInfoRootElement},
    {"isFocused", &NAccessibilityElement::GetWindowInfoIsFocused},
    {"windowId", &NAccessibilityElement::GetWindowInfoWindowId},
};
#endif // NAPI_ACCESSIBILITY_ELEMENT_H