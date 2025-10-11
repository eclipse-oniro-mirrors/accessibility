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

#include "accessibility_def.h"
#include "accessibility_event_info.h"
#include "accessibility_window_info.h"
#include "napi_utils.h"
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

enum FindElementCondition {
    FIND_ELEMENT_CONDITION_CONTENT,
    FIND_ELEMENT_CONDITION_FOCUS_TYPE,
    FIND_ELEMENT_CONDITION_TEXT_TYPE,
    FIND_ELEMENT_CONDITION_FOCUS_DIRECTION,
    FIND_ELEMENT_CONDITION_ELEMENT_ID,
    FIND_ELEMENT_CONDITION_INVALID
};

struct NAccessibilityElementData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_env env_ {};
    napi_ref callback_ {};
    AccessibilityElement accessibilityElement_ = {};
    std::map<std::string, std::string> actionArguments_;
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> nodeInfos_ {};
    OHOS::Accessibility::AccessibilityElementInfo nodeInfo_ {};
    FindElementCondition conditionId_ = FIND_ELEMENT_CONDITION_INVALID;
    std::string attribute_ = "";
    std::string actionName_ = "";
    std::string condition_ = "";
    std::string direction_ = "";
    int32_t cursorPosition_ = 0;
    bool systemApi = false;
    OHOS::Accessibility::RetError ret_ = OHOS::Accessibility::RetError::RET_ERR_FAILED;
};

using ParseConditionFunc = std::function<OHOS::Accessibility::RetError(OHOS::Accessibility::NAPICbInfo&,
    NAccessibilityElementData*)>;

using ParseElementFunc = std::function<OHOS::Accessibility::RetError(OHOS::Accessibility::NAPICbInfo&,
    NAccessibilityElementData*)>;

using ParseCallbackFunc = std::function<void(OHOS::Accessibility::NAPICbInfo&,
    NAccessibilityElementData*, napi_value&)>;

using TaskRunFunc = std::function<OHOS::Accessibility::RetError(OHOS::Accessibility::NAPICbInfo&,
    NAccessibilityElementData*, napi_value&)>;

struct ElementFunctionCallbackInfo {
    ParseElementFunc parseElement;
    ParseConditionFunc parseCondition;
    ParseCallbackFunc parseCallback;
    TaskRunFunc run;
};

class NAccessibilityElement {
public:
    static void DefineJSAccessibilityElement(napi_env env);
    static napi_value JSConstructor(napi_env env, napi_callback_info info);

    static void ConvertElementInfoToJS(napi_env env, napi_value result,
        const OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
    static void ConvertElementInfosToJS(napi_env env, napi_value result,
        const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);
    static void ConvertElementIdVecToJS(napi_env env, napi_value result,
        const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);

    static napi_value AttributeNames(napi_env env, napi_callback_info info);
    static napi_value AttributeValue(napi_env env, napi_callback_info info);
    static napi_value ActionNames(napi_env env, napi_callback_info info);
    static napi_value PerformAction(napi_env env, napi_callback_info info);
    static napi_value EnableScreenCurtain(napi_env env, napi_callback_info info);
    static napi_value FindElement(napi_env env, napi_callback_info info);
    static napi_value ErrorOperation(NAccessibilityElementData *callbackInfo);
    static napi_value GetCursorPosition(napi_env env, napi_callback_info info);
    static napi_value ExecuteAction(napi_env env, napi_callback_info info);
    static napi_value FocusMoveSearchWithCondition(napi_env env, napi_callback_info info);

    // Element info
    static void GetElementInfoComponentId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoPageId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoInspectorKey(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoBundleName(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoComponentType(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoInputType(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoText(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoHintText(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoDescription(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoResourceName(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoTextLengthLimit(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoRect(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoCheckable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoChecked(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoFocusable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsVisible(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityFocused(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoSelected(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoClickable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoLongClickable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsEnable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsPassword(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoScrollable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoEditable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoPluralLineSupported(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsHint(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoItemCount(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoCurrentIndex(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoStartIndex(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoEndIndex(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoError(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoTextMoveUnit(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoParent(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoChildren(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoTriggerAction(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoContents(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoLastContent(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoWindowId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsFocused(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoValueMax(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoValueMin(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoValueNow(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityText(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoTextType(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoOffset(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoGridItem(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoCheckboxGroup(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoRow(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoColumn(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoSideBarContainer(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoListItemIndex(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityGroup(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityLevel(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoNavDestinationId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoSpan(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsActive(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityVisible(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoClip(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoMainWindowId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoCustomComponentType(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoExtraInfo(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityNextFocusId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityPreviousFocusId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityParentId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityChildrenIds(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAccessibilityScrollable(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoSupportedActionNames(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoIsEssential(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoChildrenTreeId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoBelongTreeId(NAccessibilityElementData *callbackInfo, napi_value &value);

    // Window info
    static void GetWindowInfoIsActive(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoScreenRect(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoLayer(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoType(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoRootElement(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoIsFocused(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoWindowId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoMainWindowId(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoHotArea(NAccessibilityElementData *callbackInfo, napi_value &value);

    static OHOS::Accessibility::RetError ParseConditionInt64(OHOS::Accessibility::NAPICbInfo& cbInfo,
        NAccessibilityElementData* elementData, size_t paramIndex, FindElementCondition conditionId);

    static OHOS::Accessibility::RetError ParseConditionString(OHOS::Accessibility::NAPICbInfo& cbInfo,
        NAccessibilityElementData* elementData, size_t paramIndex, FindElementCondition conditionId);

    static napi_value QueryCommon(napi_env env, napi_callback_info info,
        const ElementFunctionCallbackInfo& callbackInfo);

    static OHOS::Accessibility::RetError RunFindElementAsync(OHOS::Accessibility::NAPICbInfo& cbInfo,
        NAccessibilityElementData* elementData, napi_value& result);

    static void ParseCallback(OHOS::Accessibility::NAPICbInfo& cbInfo,
        NAccessibilityElementData* elementData, size_t paramIndex, napi_value& result);

    static OHOS::Accessibility::RetError ParseFocusMoveSearchWithCondition(
        OHOS::Accessibility::NAPICbInfo &cbInfo, NAccessibilityElementData *elementData);

    static napi_value GetProperty(napi_env env, napi_callback_info info, std::string key);
    static napi_value GetParent(napi_env env, napi_callback_info info);
    static napi_value GetChildren(napi_env env, napi_callback_info info);
    static napi_value GetRootElement(napi_env env, napi_callback_info info);
    static napi_value FindElementByContent(napi_env env, napi_callback_info info);
    static napi_value FindElementByFocusDirection(napi_env env, napi_callback_info info);
    static napi_value FindElementsByAccessibilityHintText(napi_env env, napi_callback_info info);
    static napi_value FindElementById(napi_env env, napi_callback_info info);
    static OHOS::Accessibility::DetailCondition ConvertStringToDetailCondition(const std::string &str);

    static napi_ref consRef_;
private:
    static void AttributeNamesComplete(napi_env env, napi_status status, void* data);
    static void AttributeValueExecute(napi_env env, void* data);
    static void AttributeValueComplete(napi_env env, napi_status status, void* data);
    static void ActionNamesComplete(napi_env env, napi_status status, void* data);
    static void PerformActionExecute(napi_env env, void* data);
    static void PerformActionComplete(napi_env env, napi_status status, void* data);
    static void FindElementConstructCallbackInfo(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement);
    static void FindElementExecute(napi_env env, void* data);
    static void FindElementComplete(napi_env env, napi_status status, void* data);
    static void FindElementByText(NAccessibilityElementData *callbackInfo);
    static void GetElement(NAccessibilityElementData *callbackInfo, napi_value &value);
    static napi_value PerformActionAsync(napi_env env, size_t argc, napi_value* argv,
        std::string actionName, AccessibilityElement* accessibilityElement, bool checkPerm = false);
    static napi_value PerformActionConstructPromise(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo, std::string actionName, bool checkPerm = false);
    static napi_value FindElementAsync(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement);
    static NAccessibilityErrorCode GetAttribute(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo);
    static napi_value AttributeValueAsync(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo);
    static FindElementCondition ConvertStringToCondition(const std::string &str);
    static OHOS::Accessibility::FocusMoveDirection ConvertStringToDirection(const std::string &str);
    static int32_t ConvertStringToFocusType(const std::string &str);
    static napi_value GetCursorPositionAsync(napi_env env, size_t argc, napi_value* argv,
        NAccessibilityElementData* callbackInfo, AccessibilityElement* accessibilityElement);
    static void GetCursorPositionExecute(napi_env env, void* data);
    static void GetCursorPositionComplete(napi_env env, napi_status status, void* data);
    static AccessibilityElement* UnrapAccessibilityElement(napi_env env, napi_value thisVar);
    static bool CheckElementInfoParameter(NAccessibilityElementData *callbackInfo, napi_value &value);
    static bool CheckWindowInfoParameter(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute1(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute2(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute3(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute4(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute5(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetElementInfoAllAttribute6(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetWindowInfoAllAttribute(NAccessibilityElementData *callbackInfo, napi_value &value);
    static void GetExtraElementInfo(NAccessibilityElementData *callbackInfo, napi_value &value, std::string keyStr);
    static void FocusMoveSearchWithConditionExecute(napi_env env, void* data);
    static void FocusMoveSearchWithConditionComplete(napi_env env, napi_status status, void* data);

    static OHOS::Accessibility::RetError ParseAccessibilityElement(
        OHOS::Accessibility::NAPICbInfo& cbInfo, NAccessibilityElementData* elementData);

    static OHOS::Accessibility::RetError RunAttributeValueAsync(
        OHOS::Accessibility::NAPICbInfo& cbInfo, NAccessibilityElementData* elementData, napi_value& result);

    static OHOS::Accessibility::RetError RunAttributeValue(
        OHOS::Accessibility::NAPICbInfo& cbInfo, NAccessibilityElementData* elementData, napi_value& result);

    NAccessibilityElement() = default;
    ~NAccessibilityElement() = default;
};

template<const char* Key>
struct ElementProperty {
    static constexpr const char* value() { return Key; }
};

template<typename T>
static napi_value GetElementProperty(napi_env env, napi_callback_info info)
{
    return NAccessibilityElement::GetProperty(env, info, T::value());
}
#endif // NAPI_ACCESSIBILITY_ELEMENT_H