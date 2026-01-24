/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ani_accessibility_element.h"
#include "accessible_ability_client.h"
#include "ani_utils.h"
#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include <ani_signature_builder.h>
#include <variant>

namespace OHOS {
namespace Accessibility {

using namespace arkts::ani_signature;
using IntGetter = int32_t (AccessibilityElementInfo::*)() const;
using BoolGetter = bool (AccessibilityElementInfo::*)() const;
using DoubleGetter = double (AccessibilityElementInfo::*)() const;
using LongGetter = int64_t (AccessibilityElementInfo::*)() const;
using StringGetter = std::string &(AccessibilityElementInfo::*)() const;
using Getter = std::pair<const char*, std::variant<BoolGetter, IntGetter, DoubleGetter, LongGetter, StringGetter>>;

static ani_class g_accessibilityElementClass = nullptr;
static ani_class g_focusMoveResultClass = nullptr;

constexpr const char *ANI_ACCESSIBILITY_ELEMENT_CLS =
    "application.AccessibilityExtensionContext.AccessibilityElementImpl";
constexpr const char *ANI_ACCESSIBILITY_ELEMENT_CLEANER_CLS =
    "application.AccessibilityExtensionContext.AccessibilityElementCleaner";
constexpr const char *ANI_ACCESSIBILITY_FOCUS_MOVE_RESULT_CLS =
    "application.AccessibilityExtensionContext.FocusMoveResultImpl";
constexpr const char *ANI_ACCESSIBILITY_RECT_CLS =
    "application.AccessibilityExtensionContext.RectImpl";
constexpr const char *ANI_ACCESSIBILITY_GRID_CLS =
    "application.AccessibilityExtensionContext.AccessibilityGridImpl";
constexpr const char *ANI_ACCESSIBILITY_SPANS_CLS =
    "application.AccessibilityExtensionContext.AccessibilitySpanImpl";
constexpr const char *ANI_ACCESSIBILITY_ACTION_CLS =
    "@ohos.accessibility.AccessibilityAction";

static const std::vector<Getter> FIELD_MAP = {
    {"accessibilityFocused", BoolGetter(&AccessibilityElementInfo::HasAccessibilityFocus)},
    {"bundleName", StringGetter(&AccessibilityElementInfo::GetBundleName)},
    {"checkable", BoolGetter(&AccessibilityElementInfo::IsCheckable)},
    {"checked", BoolGetter(&AccessibilityElementInfo::IsChecked)},
    {"clickable", BoolGetter(&AccessibilityElementInfo::IsClickable)},
    {"componentId", LongGetter(&AccessibilityElementInfo::GetAccessibilityId)},
    {"componentType", StringGetter(&AccessibilityElementInfo::GetComponentType)},
    {"currentIndex", IntGetter(&AccessibilityElementInfo::GetCurrentIndex)},
    {"description", StringGetter(&AccessibilityElementInfo::GetDescriptionInfo)},
    {"editable", BoolGetter(&AccessibilityElementInfo::IsEditable)},
    {"endIndex", IntGetter(&AccessibilityElementInfo::GetEndIndex)},
    {"error", StringGetter(&AccessibilityElementInfo::GetError)},
    {"focusable", BoolGetter(&AccessibilityElementInfo::IsFocusable)},
    {"hintText", StringGetter(&AccessibilityElementInfo::GetHint)},
    {"inputType", IntGetter(&AccessibilityElementInfo::GetInputType)},
    {"inspectorKey", StringGetter(&AccessibilityElementInfo::GetInspectorKey)},
    {"isActive", BoolGetter(&AccessibilityElementInfo::GetIsActive)},
    {"isEnable", BoolGetter(&AccessibilityElementInfo::IsEnabled)},
    {"isHint", BoolGetter(&AccessibilityElementInfo::IsGivingHint)},
    {"isFocused", BoolGetter(&AccessibilityElementInfo::IsFocused)},
    {"isPassword", BoolGetter(&AccessibilityElementInfo::IsPassword)},
    {"isVisible", BoolGetter(&AccessibilityElementInfo::IsVisible)},
    {"itemCount", IntGetter(&AccessibilityElementInfo::GetItemCounts)},
    {"lastContent", StringGetter(&AccessibilityElementInfo::GetLatestContent)},
    {"longClickable", BoolGetter(&AccessibilityElementInfo::IsLongClickable)},
    {"pageId", IntGetter(&AccessibilityElementInfo::GetPageId)},
    {"pluralLineSupported", BoolGetter(&AccessibilityElementInfo::IsPluraLineSupported)},
    {"resourceName", StringGetter(&AccessibilityElementInfo::GetComponentResourceId)},
    {"scrollable", BoolGetter(&AccessibilityElementInfo::IsScrollable)},
    {"selected", BoolGetter(&AccessibilityElementInfo::IsSelected)},
    {"startIndex", IntGetter(&AccessibilityElementInfo::GetBeginIndex)},
    {"text", StringGetter(&AccessibilityElementInfo::GetContent)},
    {"textLengthLimit", IntGetter(&AccessibilityElementInfo::GetTextLengthLimit)},
    {"windowId", IntGetter(&AccessibilityElementInfo::GetWindowId)},
    {"offset", DoubleGetter(&AccessibilityElementInfo::GetOffset)},
    {"textType", StringGetter(&AccessibilityElementInfo::GetTextType)},
    {"accessibilityText", StringGetter(&AccessibilityElementInfo::GetAccessibilityText)},
    {"customComponentType", StringGetter(&AccessibilityElementInfo::GetCustomComponentType)},
    {"accessibilityNextFocusId", LongGetter(&AccessibilityElementInfo::GetAccessibilityNextFocusId)},
    {"accessibilityPreviousFocusId", LongGetter(&AccessibilityElementInfo::GetAccessibilityPreviousFocusId)},
    {"extraInfo", StringGetter(&AccessibilityElementInfo::GetAccessibilityText)},
    {"accessibilityScrollable", BoolGetter(&AccessibilityElementInfo::GetAccessibilityScrollable)},
    {"accessibilityGroup", BoolGetter(&AccessibilityElementInfo::GetAccessibilityGroup)},
    {"accessibilityLevel", StringGetter(&AccessibilityElementInfo::GetAccessibilityLevel)},
    {"navDestinationId", LongGetter(&AccessibilityElementInfo::GetNavDestinationId)},
    {"accessibilityVisible", BoolGetter(&AccessibilityElementInfo::GetAccessibilityVisible)},
    {"mainWindowId", IntGetter(&AccessibilityElementInfo::GetMainWindowId)},
    {"clip", BoolGetter(&AccessibilityElementInfo::GetClip)},
    {"accessibilityStateDescription", StringGetter(&AccessibilityElementInfo::GetAccessibilityStateDescription)},
};

bool InitializeAccessibilityElementClass(ani_env *env)
{
    if (g_accessibilityElementClass != nullptr && g_focusMoveResultClass != nullptr) {
        return true;
    }

    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_ELEMENT_CLS);
    if (ANI_OK != env->FindClass(className.Descriptor().c_str(), &g_accessibilityElementClass)) {
        HILOG_ERROR(" not found class AccessibilityElement");
        return false;
    }

    std::array methods = {
        ani_native_function {"getCursorPositionNative", nullptr, reinterpret_cast<void *>(GetCursorPosition)},
        ani_native_function {"enableScreenCurtainNative", nullptr, reinterpret_cast<void *>(EnableScreenCurtain)},
        ani_native_function {"findElementNative", nullptr, reinterpret_cast<void *>(FindElement)},
        ani_native_function {"findElementsNative", nullptr, reinterpret_cast<void *>(FindElements)},
        ani_native_function {"findElementsByConditionNative", nullptr,
            reinterpret_cast<void *>(FindElementsByCondition)},
        ani_native_function {"getRootNative", nullptr, reinterpret_cast<void *>(GetRootElement)},
        ani_native_function {"getParentNative", nullptr, reinterpret_cast<void *>(GetParent)},
        ani_native_function {"getChildrenNative", nullptr, reinterpret_cast<void *>(GetChildren)},
        ani_native_function {"findElementByContentNative", nullptr, reinterpret_cast<void *>(FindElementByContent)},
        ani_native_function {"findElementByFocusDirectionNative", nullptr,
            reinterpret_cast<void *>(FindElementByFocusDirection)},
        ani_native_function {"findElementsByAccessibilityHintTextNative", nullptr,
            reinterpret_cast<void *>(FindElementsByAccessibilityHintText)},
        ani_native_function {"findElementByIdNative", nullptr, reinterpret_cast<void *>(FindElementById)},
        ani_native_function {"executeActionNative", nullptr, reinterpret_cast<void *>(ExecuteAction)}
    };

    if (ANI_OK != env->Class_BindNativeMethods(g_accessibilityElementClass, methods.data(), methods.size())) {
        HILOG_ERROR("Cannot bind native methods to element");
        g_accessibilityElementClass = nullptr;
        return false;
    }

    arkts::ani_signature::Type moveResultName = arkts::ani_signature::Builder::BuildClass(
        ANI_ACCESSIBILITY_FOCUS_MOVE_RESULT_CLS);
    if (ANI_OK != env->FindClass(moveResultName.Descriptor().c_str(), &g_focusMoveResultClass)) {
        HILOG_ERROR(" not found class focusMoveResult");
        return false;
    }

    ani_class cleanerCls;
    arkts::ani_signature::Type cleanerName = arkts::ani_signature::Builder::BuildClass(
        ANI_ACCESSIBILITY_ELEMENT_CLEANER_CLS);
    if (ANI_OK != env->FindClass(cleanerName.Descriptor().c_str(), &cleanerCls)) {
        HILOG_ERROR(" not found class AccessibilityElement");
        return false;
    }
    std::array cleanMethod = { ani_native_function {"cleanNative", ":", reinterpret_cast<void *>(Clean)} };
    if (ANI_OK != env->Class_BindNativeMethods(cleanerCls, cleanMethod.data(), cleanMethod.size())) {
        HILOG_ERROR("Cannot bind native methods to AccessibilityElementCleaner");
        return false;
    }
    return true;
}

ani_class GetAniAccessibilityElementClass()
{
    return g_accessibilityElementClass;
}

void Clean(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG("Clean begin");
    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    delete element;
    element = nullptr;
    HILOG_DEBUG("Clean end");
}

void ExecuteAction(ani_env *env, ani_object thisObj, ani_enum_item action, ani_object obj)
{
    HILOG_DEBUG();
    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    ani_size index;
    if (env->EnumItem_GetIndex(action, &index) != ANI_OK) {
        HILOG_ERROR("Failed to get enum index");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }

    std::map<std::string, std::string> actionArguments {};
    ActionType type = ANIUtils::ConvertStringToAccessibleOperationType(ACTION_NAMES[index]);
    if (type == ActionType::ACCESSIBILITY_ACTION_INVALID) {
        HILOG_ERROR("The action is not supported");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_ACTION_NOT_SUPPORT));
        return;
    }
    ANIUtils::ConvertActionArgsJSToANI(env, obj, actionArguments, type);
    actionArguments.insert(std::pair<std::string, std::string>("sysapi_check_perm", "1"));
    FindElementParams param = {
        .accessibilityElement_ = *element,
        .systemApi_ = true,
        .actionArguments_ = actionArguments,
        .actionName_ = ACTION_NAMES[index]
    };
    param.ret_ = AccessibleAbilityClient::GetInstance()->ExecuteAction(
        *param.accessibilityElement_.elementInfo_, type, param.actionArguments_);
    HILOG_DEBUG("ExecuteAction result [%{public}d]", param.ret_);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
    }
}

bool CreateJsRect(ani_env *env, const Rect& rect, ani_object& rectObj)
{
    HILOG_DEBUG();
    if (env == nullptr) {
        HILOG_ERROR("CreateJsRect null env");
        return false;
    }

    ani_class cls;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_RECT_CLS);
    if (env->FindClass(className.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR(" not found class AccessibilityElement");
        return false;
    }
    rectObj = ANIUtils::CreateObject(env, cls);
    if (rectObj == nullptr) {
        HILOG_ERROR("Failed to create Rect object");
        return false;
    }
    ani_status status;
    if ((status = env->Object_SetPropertyByName_Int(rectObj, "left", rect.GetLeftTopXScreenPostion())) != ANI_OK) {
        HILOG_ERROR("Failed to set left %{public}d", status);
        return false;
    }
    if ((status = env->Object_SetPropertyByName_Int(rectObj, "top", rect.GetLeftTopYScreenPostion())) != ANI_OK) {
        HILOG_ERROR("Failed to set top %{public}d", status);
        return false;
    }
    if ((status = env->Object_SetPropertyByName_Int(rectObj, "width", rect.GetRightBottomXScreenPostion()))
        != ANI_OK) {
        HILOG_ERROR("Failed to set width %{public}d", status);
        return false;
    }
    if ((status = env->Object_SetPropertyByName_Int(rectObj, "height", rect.GetRightBottomYScreenPostion()))
        != ANI_OK) {
        HILOG_ERROR("Failed to set height %{public}d", status);
        return false;
    }
    return true;
}

bool SetElementRectField(ani_env *env, ani_object& elementObj, const Rect& rect, const std::string& name)
{
    HILOG_DEBUG();
    ani_class cls;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_RECT_CLS);
    if (env->FindClass(className.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("Class AccessibilityElement not found");
        return false;
    }

    ani_object rectObj = nullptr;
    if (!CreateJsRect(env, rect, rectObj)) {
        HILOG_ERROR("Failed to create Rect object");
        return false;
    }

    if (env->Object_SetPropertyByName_Ref(elementObj, name.c_str(), rectObj) != ANI_OK) {
        HILOG_ERROR("Failed to set rect field in AccessibilityElement");
        return false;
    }
    return true;
}

bool convertRectArrayToJs(ani_env *env, const std::vector<Rect>& rects, std::vector<ani_object>& jsArray)
{
    HILOG_DEBUG();
    if (env == nullptr) {
        HILOG_ERROR("convertRectArrayToJs null env");
        return false;
    }

    for (size_t i = 0; i < rects.size(); ++i) {
        ani_object rectObj = nullptr;
        if (!CreateJsRect(env, rects[i], rectObj)) {
            HILOG_ERROR("Failed to create Rect object");
            return false;
        }
        jsArray.push_back(rectObj);
    }
    return true;
}

bool SetElementRectArrayField(ani_env *env, ani_object& elementObj, const std::vector<Rect>& rects,
    const std::string& name)
{
    HILOG_DEBUG();
    if (env == nullptr) {
        HILOG_ERROR("setElementRectArrayField null env");
        return false;
    }

    std::vector<ani_object> jsArray;
    if (!convertRectArrayToJs(env, rects, jsArray)) {
        HILOG_ERROR("Failed to convert Rect array to JS");
        return false;
    }
    if (!ANIUtils::SetArrayField(env, elementObj, name, jsArray)) {
        HILOG_ERROR("Failed to set rect array field in AccessibilityElement");
        return false;
    }
    return true;
}

bool SetElementCurrentItemField(ani_env *env, ani_object& elementObj, const GridItemInfo& grid)
{
    HILOG_DEBUG();
    ani_class cls;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_GRID_CLS);
    if (env->FindClass(className.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR(" not found class AccessibilityElement");
        return false;
    }

    ani_object gridObj = ANIUtils::CreateObject(env, cls);
    if (gridObj == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityGrid object");
        return false;
    }

    ani_status status;
    if ((status = env->Object_SetPropertyByName_Int(gridObj, "rowIndex", grid.GetRowIndex())) != ANI_OK) {
        HILOG_ERROR("Failed to set rowIndex %{public}d", status);
    }
    if ((status = env->Object_SetPropertyByName_Int(gridObj, "columnIndex", grid.GetColumnIndex())) != ANI_OK) {
        HILOG_ERROR("Failed to set columnIndex %{public}d", status);
    }

    if (env->Object_SetPropertyByName_Ref(elementObj, "currentItem", gridObj) != ANI_OK) {
        HILOG_ERROR("Failed to set currentItem field in AccessibilityElement");
        return false;
    }
    return true;
}

bool PrepareSpan(ani_env *env, ani_object& spanObj, const SpanInfo& span)
{
    HILOG_DEBUG();
    ani_status status;
    if ((status = env->Object_SetPropertyByName_Double(spanObj, "spanId", span.GetSpanId())) != ANI_OK) {
        HILOG_ERROR("Failed to set spanId %{public}d", status);
        return false;
    }
    if (!ANIUtils::SetStringField(env, spanObj, "spanText", span.GetSpanText())) {
        HILOG_ERROR("Failed to set span start");
        return false;
    }
    if (!ANIUtils::SetStringField(env, spanObj, "accessibilityText", span.GetAccessibilityText())) {
        HILOG_ERROR("Failed to set span end");
        return false;
    }
    if (!ANIUtils::SetStringField(env, spanObj, "accessibilityDescription",
        span.GetAccessibilityDescription())) {
        HILOG_ERROR("Failed to set span end");
        return false;
    }
    if (!ANIUtils::SetStringField(env, spanObj, "accessibilityLevel", span.GetAccessibilityLevel())) {
        HILOG_ERROR("Failed to set span end");
        return false;
    }
    return true;
}

bool SetElementSpansField(ani_env *env, ani_object& elementObj, const std::vector<SpanInfo>& spans)
{
    HILOG_DEBUG();
    ani_ref undefinedRef = nullptr;
    if (env->GetUndefined(&undefinedRef) != ANI_OK) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array spansArray;
    env->Array_New(spans.size(), undefinedRef, &spansArray);
    if (spansArray == nullptr) {
        HILOG_ERROR("Failed to create spans array");
        return false;
    }
    ani_class cls;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_SPANS_CLS);
    if (env->FindClass(className.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("Class AccessibilitySpan not found");
        return false;
    }
    for (size_t i = 0; i < spans.size(); ++i) {
        ani_object spanObj = ANIUtils::CreateObject(env, cls);
        if (spanObj == nullptr) {
            HILOG_ERROR("Failed to create AccessibilitySpan object");
            return false;
        }
        if (!PrepareSpan(env, spanObj, spans[i])) {
            HILOG_ERROR("Failed to prepare span object");
            return false;
        }
        if (env->Array_Set(spansArray, static_cast<ani_size>(i), spanObj) != ANI_OK) {
            HILOG_ERROR("Failed to set span in array at index %zu", i);
            return false;
        }
    }

    if (env->Object_SetPropertyByName_Ref(elementObj, "spans", spansArray) != ANI_OK) {
        HILOG_ERROR("Failed to set spans field in AccessibilityElement");
        return false;
    }
    return true;
}

bool SetElementChildrenProperty(ani_env *env, ani_object& elementObj, const std::vector<int64_t>& children)
{
    HILOG_DEBUG();
    if (env == nullptr) {
        HILOG_ERROR("setElementChildrenProperty null env");
        return false;
    }
    ani_object value;
    std::vector<ani_object> childrenArray;
    for (const auto &childId : children) {
        if (ANIUtils::CreateAniLong(env, childId, value) != ANI_OK) {
            HILOG_ERROR("CreateAniLong failed for childId %{public}lld", childId);
            return false;
        }
        childrenArray.push_back(value);
        value = nullptr;  // Reset value for next iteration
    }
    ANIUtils::SetArrayField(env, elementObj, "childrenIds", childrenArray, true);
    return true;
}

std::string ConvertActionTypeToString(ActionType type)
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

void SetElementActionListProperty(ani_env *env, ani_object& elementObj, const std::vector<AccessibleAction>& actionList)
{
    HILOG_DEBUG();
    if (env == nullptr) {
        HILOG_ERROR("setElementActionListProperty null env");
        return;
    }
    std::vector<std::string> stringArray;
    for (const auto &action : actionList) {
        std::string actionString = ConvertActionTypeToString(action.GetActionType());
        stringArray.push_back(actionString);
    }
    ANIUtils::SetArrayStringField(env, elementObj, "supportedActionNames", stringArray);
}

std::string TextMoveUnitToString(OHOS::Accessibility::TextMoveUnit unit)
{
    static const std::map<OHOS::Accessibility::TextMoveUnit, const std::string> textMoveUnitTable = {
        {OHOS::Accessibility::TextMoveUnit::STEP_CHARACTER, "char"},
        {OHOS::Accessibility::TextMoveUnit::STEP_WORD, "word"},
        {OHOS::Accessibility::TextMoveUnit::STEP_LINE, "line"},
        {OHOS::Accessibility::TextMoveUnit::STEP_PAGE, "page"},
        {OHOS::Accessibility::TextMoveUnit::STEP_PARAGRAPH, "paragraph"}
    };

    if (textMoveUnitTable.find(unit) == textMoveUnitTable.end()) {
        return "char";
    }

    return textMoveUnitTable.at(unit);
}

int32_t ConvertOperationTypeToTarget(ActionType type)
{
    static const std::map<ActionType, AccessibilityAction> actionTable = {
        {ActionType::ACCESSIBILITY_ACTION_FOCUS, AccessibilityAction::FOCUS},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS, AccessibilityAction::CLEAR_FOCUS},
        {ActionType::ACCESSIBILITY_ACTION_SELECT, AccessibilityAction::SELECT},
        {ActionType::ACCESSIBILITY_ACTION_CLICK, AccessibilityAction::CLICK},
        {ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, AccessibilityAction::LONG_CLICK},
        {ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, AccessibilityAction::ACCESSIBILITY_FOCUS},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, AccessibilityAction::CLEAR_ACCESSIBILITY_FOCUS},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, AccessibilityAction::SCROLL_FORWARD},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, AccessibilityAction::SCROLL_BACKWARD},
        {ActionType::ACCESSIBILITY_ACTION_COPY, AccessibilityAction::COPY},
        {ActionType::ACCESSIBILITY_ACTION_PASTE, AccessibilityAction::PASTE},
        {ActionType::ACCESSIBILITY_ACTION_CUT, AccessibilityAction::CUT},
        {ActionType::ACCESSIBILITY_ACTION_SET_SELECTION, AccessibilityAction::SET_SELECTION},
        {ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION, AccessibilityAction::SET_CURSOR_POSITION},
        {ActionType::ACCESSIBILITY_ACTION_SET_TEXT, AccessibilityAction::SET_TEXT},
        {ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK, AccessibilityAction::SPAN_CLICK}
    };

    if (actionTable.find(type) == actionTable.end()) {
        return -1;
    }

    return static_cast<int32_t>(actionTable.at(type));
}

void SetElementBaseProperties(ani_env *env, ani_object& elementObj, const AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG();
    for (const auto& [name, getterVariant] : FIELD_MAP) {
        auto fieldName = name;
        std::visit([&, fieldName](auto getter) {
            using GetterType = decltype(getter);
            bool isSuccess = false;

            if constexpr (std::is_same_v<GetterType, BoolGetter>) {
                isSuccess = ANIUtils::SetBooleanField(env, elementObj, fieldName, (elementInfo.*getter)());
            } else if constexpr (std::is_same_v<GetterType, IntGetter>) {
                isSuccess = ANIUtils::SetIntField(env, elementObj, fieldName, (elementInfo.*getter)());
            } else if constexpr (std::is_same_v<GetterType, DoubleGetter>) {
                isSuccess = ANIUtils::SetDoubleField(env, elementObj, fieldName, (elementInfo.*getter)());
            } else if constexpr (std::is_same_v<GetterType, LongGetter>) {
                isSuccess = ANIUtils::SetLongField(env, elementObj, fieldName, (elementInfo.*getter)());
            } else if constexpr (std::is_same_v<GetterType, StringGetter>) {
                std::string value = (elementInfo.*getter)();
                isSuccess = ANIUtils::SetStringField(env, elementObj, fieldName, value);
            }

            if (!isSuccess) {
                HILOG_ERROR("Failed to set %{public}s", fieldName);
            }
        },
            getterVariant);
    }
}

void SetAccessibilityElementField(ani_env *env, ani_object& elementObj, const AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG();

    SetElementBaseProperties(env, elementObj, elementInfo);

    std::vector<std::string> contents {};
    elementInfo.GetContentList(contents);
    if (!ANIUtils::SetArrayStringField(env, elementObj, "contents", contents)) {
        HILOG_ERROR("Failed to set contents");
    }
    SetElementRectField(env, elementObj, elementInfo.GetRectInScreen(), "rect");
    if (!ANIUtils::SetStringField(env, elementObj, "textMoveUnit",
        TextMoveUnitToString(elementInfo.GetTextMovementStep()))) {
        HILOG_ERROR("Failed to set textMoveUnit");
    }
    if (!ANIUtils::SetEnumProperty(env, elementObj, ANI_ACCESSIBILITY_ACTION_CLS, "triggerAction",
        ConvertOperationTypeToTarget(elementInfo.GetTriggerAction()))) {
        HILOG_ERROR("Failed to set triggerAction");
    }
    if (!ANIUtils::SetDoubleField(env, elementObj, "valueMax", elementInfo.GetRange().GetMax())) {
        HILOG_ERROR("Failed to set valueMax");
    }
    if (!ANIUtils::SetDoubleField(env, elementObj, "valueMin", elementInfo.GetRange().GetMin())) {
        HILOG_ERROR("Failed to set valueMin");
    }
    if (!ANIUtils::SetDoubleField(env, elementObj, "valueNow", elementInfo.GetRange().GetCurrent())) {
        HILOG_ERROR("Failed to set valueNow");
    }
    SetElementActionListProperty(env, elementObj, elementInfo.GetActionList());
    SetElementCurrentItemField(env, elementObj, elementInfo.GetGridItem());
    SetElementSpansField(env, elementObj, elementInfo.GetSpanList());
    if (elementInfo.GetParentNodeId() >= 0) {
        if (!ANIUtils::SetLongField(env, elementObj, "parentId", elementInfo.GetParentNodeId())) {
            HILOG_ERROR("Failed to set parentId");
        }
    }
    if (!SetElementChildrenProperty(env, elementObj, elementInfo.GetChildIds())) {
        HILOG_ERROR("Failed to set childrenIds");
    }
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

void SetAccessibilityElementField(ani_env *env, ani_object& elementObj, const AccessibilityWindowInfo& windowInfo)
{
    if (!ANIUtils::SetBooleanField(env, elementObj, "isActive", windowInfo.IsActive())) {
        HILOG_ERROR("Failed to set isActive");
    }
    SetElementRectField(env, elementObj, windowInfo.GetRectInScreen(), "screenRect");
    if (!ANIUtils::SetIntField(env, elementObj, "layer", windowInfo.GetWindowLayer())) {
        HILOG_ERROR("Failed to set rowIndex");
    }
    if (!ANIUtils::SetStringField(env, elementObj, "type",
        ConvertWindowTypeToString(windowInfo.GetAccessibilityWindowType()))) {
        HILOG_ERROR("Failed to set type");
    }
    if (!ANIUtils::SetBooleanField(env, elementObj, "isFocused", windowInfo.IsFocused())) {
        HILOG_ERROR("Failed to set isFocused");
    }
    if (!ANIUtils::SetIntField(env, elementObj, "windowId", windowInfo.GetWindowId())) {
        HILOG_ERROR("Failed to set windowId");
    }
    if (!ANIUtils::SetIntField(env, elementObj, "mainWindowId", windowInfo.GetMainWindowId())) {
        HILOG_ERROR("Failed to set mainWindowId");
    }
    SetElementRectArrayField(env, elementObj, const_cast<AccessibilityWindowInfo&>(windowInfo).GetTouchHotAreas(),
        "hotArea");
}

void SetFocusMoveResultField(ani_env *env, ani_object& elementObj, ani_object resultArray, int32_t ret)
{
    ani_status status = env->Object_SetPropertyByName_Ref(elementObj, "target", resultArray);
    if (status != ANI_OK) {
        HILOG_ERROR("Failed to set target, status : %{public}d", status);
        return;
    }
    if (!ANIUtils::SetIntField(env, elementObj, "result", ret, true)) {
        HILOG_ERROR("Failed to set result code");
    }
    HILOG_INFO("SetFocusMoveResultField end, ret: %{public}d", ret);
}

ani_object CreateAniAccessibilityElement(ani_env *env, const AccessibilityWindowInfo& windowInfo)
{
    HILOG_DEBUG("CreateAniAccessibilityWindowElement begin");

    if (!InitializeAccessibilityElementClass(env)) {
        HILOG_ERROR("Failed to initialize AccessibilityElement class");
        return nullptr;
    }

    ani_object elementObj = ANIUtils::CreateObject(env, g_accessibilityElementClass);
    if (elementObj == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityElement object");
        return nullptr;
    }

    std::shared_ptr<AccessibilityWindowInfo> windowInfomation =
        std::make_shared<AccessibilityWindowInfo>(windowInfo);
    AccessibilityElement* element = new(std::nothrow) AccessibilityElement(windowInfomation);
    if (element == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityElement");
        return nullptr;
    }

    if (ANI_OK != ANIUtils::Wrap(env, elementObj, element)) {
        HILOG_ERROR("Cannot wrap AccessibilityElementInfo");
        delete element;
        element = nullptr;
        return nullptr;
    }
    SetAccessibilityElementField(env, elementObj, windowInfo);
    HILOG_DEBUG("CreateAniAccessibilityWindowElement end");
    return elementObj;
}

ani_object CreateAniAccessibilityElement(ani_env *env, const AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("CreateAniAccessibilityElement begin");

    if (!InitializeAccessibilityElementClass(env)) {
        HILOG_ERROR("Failed to initialize AccessibilityElement class");
        return nullptr;
    }

    ani_object elementObj = ANIUtils::CreateObject(env, g_accessibilityElementClass);
    if (elementObj == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityElement object");
        return nullptr;
    }

    std::shared_ptr<AccessibilityElementInfo> elementInfomation =
        std::make_shared<AccessibilityElementInfo>(elementInfo);
    HILOG_INFO("GetRoot end, mainWindowId: %{public}d, bundleName: %{public}s", elementInfomation->GetMainWindowId(),
        elementInfomation->GetBundleName().c_str());
    AccessibilityElement* element = new(std::nothrow) AccessibilityElement(elementInfomation);
    if (element == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityElement");
        return nullptr;
    }

    if (ANI_OK != ANIUtils::Wrap(env, elementObj, element)) {
        HILOG_ERROR("Cannot wrap AccessibilityElementInfo");
        delete element;
        element = nullptr;
        return nullptr;
    }

    SetAccessibilityElementField(env, elementObj, elementInfo);

    HILOG_INFO("CreateAniAccessibilityElement end");
    return elementObj;
}

ani_object CreateAniAccessibilityRuleResult(ani_env *env, std::vector<AccessibilityElementInfo> &infos, int32_t ret)
{
    HILOG_DEBUG("CreateAniAccessibilityRuleResult begin");
 
    if (!InitializeAccessibilityElementClass(env)) {
        HILOG_ERROR("Failed to initialize AccessibilityElement class");
        return nullptr;
    }
    ani_object elementObj = ANIUtils::CreateObject(env, g_focusMoveResultClass);
    ani_object resultArray = ConvertElementInfosToJs(env, infos);
    SetFocusMoveResultField(env, elementObj, resultArray, ret);
    HILOG_INFO("CreateAniAccessibilityElement end");
    return elementObj;
}

ani_object ConvertElementInfosToJs(ani_env *env, const std::vector<AccessibilityWindowInfo>& windowInfos)
{
    HILOG_DEBUG("ConvertElementInfosToAni elementInfo size(%{public}zu)", windowInfos.size());
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(windowInfos.size(), undefinedRef, &resultArray);
    if (windowInfos.empty()) {
        return resultArray;
    }
    if (resultArray == nullptr) {
        HILOG_ERROR("Failed to create result array");
        return nullptr;
    }

    for (size_t i = 0; i < windowInfos.size(); i++) {
        ani_object windowObj = CreateAniAccessibilityElement(env, windowInfos[i]);
        if (windowObj == nullptr) {
            HILOG_ERROR("Failed to create AccessibilityElement at index %{public}zu", i);
            continue;
        }

        if (ANI_OK != env->Array_Set(resultArray, static_cast<ani_size>(i), windowObj)) {
            HILOG_ERROR("Failed to set array element at index %{public}zu", i);
        }
    }

    HILOG_DEBUG("ConvertElementInfosToAni end, converted %{public}zu elements", windowInfos.size());
    return resultArray;
}

ani_int GetCursorPosition(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG("GetCursorPosition native method called");

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return static_cast<ani_int>(-1);
    }

    int32_t cursorPosition = 0;
    auto innerErrorCode = AccessibleAbilityClient::GetInstance()->GetCursorPosition(
        *element->elementInfo_, cursorPosition);
    if (innerErrorCode != RET_OK) {
        HILOG_ERROR("GetCursorPosition error");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(innerErrorCode));
        return static_cast<ani_int>(-1);
    }

    return static_cast<ani_int>(cursorPosition);
}

void EnableScreenCurtain(ani_env *env, ani_object thisObj, ani_boolean isEnable)
{
    HILOG_DEBUG("EnableScreenCurtain native method called, isEnable: %{public}d", isEnable);

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    HILOG_INFO("Screen curtain %{public}s", isEnable ? "enabled" : "disabled");
    auto ret = AccessibleAbilityClient::GetInstance()->EnableScreenCurtain(static_cast<bool>(isEnable));
    if (RET_OK != ret) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
}

ani_object FindElement(ani_env *env, ani_object thisObj, ani_string type, ani_long condition)
{
    HILOG_DEBUG("FindElement native method called");

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    std::string typeStr = ANIUtils::ANIStringToStdString(env, type);
    if (std::strcmp(typeStr.c_str(), "elementId") != 0) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    int64_t elementId = static_cast<int64_t>(condition);
    HILOG_DEBUG("Finding element by ID: %{public}lld", elementId);

    FindElementParams param = {FIND_ELEMENT_CONDITION_ELEMENT_ID, std::to_string(elementId), *element};

    FindElementExecute(&param);
    if (RET_OK != param.ret_) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, param.nodeInfo_);
}

ani_object FindElementsByCondition(ani_env *env, ani_object thisObj, ani_string rule, ani_string condition)
{
    HILOG_DEBUG("FindElementsByCondition native method called");
    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        return nullptr;
    }
    std::string ruleStr = ANIUtils::ANIStringToStdString(env, rule);
    std::string conditionStr = ANIUtils::ANIStringToStdString(env, condition);
    FindElementParams param = {FIND_ELEMENT_BY_CONDITION, conditionStr, *element};
    param.rule_ = ruleStr;
    FindElementExecute(&param);
    if (RET_ERR_NOT_SYSTEM_APP == param.ret_ || RET_ERR_NO_PERMISSION == param.ret_) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    if (RET_OK != param.ret_) {
        param.moveSearchResult_ = -1;
    }
 
    return CreateAniAccessibilityRuleResult(env, param.nodeInfos_, param.moveSearchResult_);
}

FocusMoveDirection ConvertStringToDirection(const std::string &str)
{
    static const std::map<std::string, FocusMoveDirection> focusMoveDirectionTable = {
        {"up", FocusMoveDirection::UP},
        {"down", FocusMoveDirection::DOWN},
        {"left", FocusMoveDirection::LEFT},
        {"right", FocusMoveDirection::RIGHT},
        {"forward", FocusMoveDirection::FORWARD},
        {"backward", FocusMoveDirection::BACKWARD},
        {"findLast", FocusMoveDirection::FIND_LAST},
        {"getForwardScrollAncestor", FocusMoveDirection::GET_FORWARD_SCROLL_ANCESTOR},
        {"getBackwardScrollAncestor", FocusMoveDirection::GET_BACKWARD_SCROLL_ANCESTOR},
        {"getScrollableAncestor", FocusMoveDirection::GET_SCROLLABLE_ANCESTOR}
    };

    if (focusMoveDirectionTable.find(str) == focusMoveDirectionTable.end()) {
        return FocusMoveDirection::DIRECTION_INVALID;
    }

    return focusMoveDirectionTable.at(str);
}

DetailCondition ConvertStringToDetailCondition(const std::string &str)
{
    static const std::map<std::string, DetailCondition> detailConditionMap = {
        { "bypassSelf", DetailCondition::BYPASS_SELF },
        { "bypassSelfDescendants", DetailCondition::BYPASS_SELF_DESCENDANTS},
        { "checkSelf", DetailCondition::CHECK_SELF},
        { "checkSelfBypassDescendants", DetailCondition::CHECK_SELF_BYPASS_DESCENDANTS}
    };
    if (detailConditionMap.find(str) == detailConditionMap.end()) {
        return DetailCondition::BYPASS_SELF;
    }
 
    return detailConditionMap.at(str);
}

ani_object FindElementByContent(ani_env *env, ani_object thisObj, ani_string content)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    std::string contentStr = ANIUtils::ANIStringToStdString(env, content);
    FindElementParams param = {
        .conditionId_ = FIND_ELEMENT_CONDITION_CONTENT,
        .condition_ = contentStr,
        .accessibilityElement_ = *element
    };

    FindElementExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    ani_object resultArray = ConvertElementInfosToJs(env, param.nodeInfos_);
    return resultArray;
}

ani_object FindElementByFocusDirection(ani_env *env, ani_object thisObj, ani_string direction)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    std::string directionStr = ANIUtils::ANIStringToStdString(env, direction);
    FindElementParams param = {
        .conditionId_ = FIND_ELEMENT_CONDITION_FOCUS_DIRECTION,
        .condition_ = directionStr,
        .accessibilityElement_ = *element
    };

    FindElementExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, param.nodeInfo_);
}

ani_object FindElementsByAccessibilityHintText(ani_env *env, ani_object thisObj, ani_string hintText)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    std::string hintTextStr = ANIUtils::ANIStringToStdString(env, hintText);
    FindElementParams param = {
        .conditionId_ = FIND_ELEMENT_CONDITION_TEXT_TYPE,
        .condition_ = hintTextStr,
        .accessibilityElement_ = *element
    };

    FindElementExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    ani_object resultArray = ConvertElementInfosToJs(env, param.nodeInfos_);
    return resultArray;
}

ani_object FindElementById(ani_env *env, ani_object thisObj, ani_long elementId)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    int64_t id = static_cast<int64_t>(elementId);
    FindElementParams param = {
        .conditionId_ = FIND_ELEMENT_CONDITION_ELEMENT_ID,
        .condition_ = std::to_string(id),
        .accessibilityElement_ = *element
    };

    FindElementExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, param.nodeInfo_);
}

ani_object FindElements(ani_env *env, ani_object thisObj, ani_string type, ani_string condition)
{
    HILOG_DEBUG("FindElements native method called");

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    std::string typeStr = ANIUtils::ANIStringToStdString(env, type);
    std::string conditionStr = ANIUtils::ANIStringToStdString(env, condition);

    if (std::strcmp(typeStr.c_str(), "textType") != 0) {
        HILOG_ERROR("Unsupported find type: %{public}s", typeStr.c_str());
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return nullptr;
    }
    FindElementParams param = {FIND_ELEMENT_CONDITION_TEXT_TYPE, conditionStr, *element};
    FindElementExecute(&param);
    if (RET_OK != param.ret_) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    ani_object resultArray = ConvertElementInfosToJs(env, param.nodeInfos_);
    return resultArray;
}

void FindElementByText(FindElementParams *data)
{
    bool systemApi = data->systemApi_;
    nlohmann::json jsonStr;
    jsonStr["type"] = (data->conditionId_ ==
        FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT) ? "content" : "textType";
    jsonStr["value"] = data->condition_;
    data->ret_ = AccessibleAbilityClient::GetInstance()->GetByContent(
        *(data->accessibilityElement_.elementInfo_),
        jsonStr.dump().c_str(), data->nodeInfos_, systemApi);
}

void FindElementExecute(FindElementParams* data)
{
    if (data == nullptr) {
        HILOG_ERROR("FindElementParams is nullptr");
        return;
    }
    bool systemApi = data->systemApi_;
    HILOG_DEBUG("systemApi: %{public}d", systemApi);
    switch (data->conditionId_) {
        case FindElementCondition::FIND_ELEMENT_CONDITION_TEXT_TYPE:
            {
                FindElementByText(data);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_FOCUS_DIRECTION:
            {
                FocusMoveDirection direction = ConvertStringToDirection(data->condition_);
                HILOG_DEBUG("direction is %{public}d", direction);
                data->ret_ = AccessibleAbilityClient::GetInstance()->GetNext(
                    *(data->accessibilityElement_.elementInfo_), direction, data->nodeInfo_, systemApi);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_CONTENT:
            {
                FindElementByText(data);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_CONDITION_ELEMENT_ID:
            {
                int64_t elementId = 0;
                if (!ANIUtils::ConvertStringToInt64(data->condition_, elementId)) {
                    HILOG_ERROR("condition id convert failed");
                    return;
                }
                int32_t windowId = data->accessibilityElement_.elementInfo_->GetWindowId();
                if (windowId == 1) {
                    windowId = data->accessibilityElement_.elementInfo_->GetMainWindowId();
                }
                HILOG_DEBUG("elementId is %{public}lld windowId: %{public}d", elementId, windowId);
                data->ret_ = AccessibleAbilityClient::GetInstance()->GetByElementId(
                    elementId, windowId, data->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_BY_CONDITION:
            {
                HILOG_DEBUG("FIND_ELEMENT_BY_CONDITION");
                AccessibilityFocusMoveParam param;
                param.direction = ConvertStringToDirection(data->condition_);
                param.condition = ConvertStringToDetailCondition(data->rule_);
                data->ret_ = AccessibleAbilityClient::GetInstance()->FocusMoveSearchWithCondition(
                    *data->accessibilityElement_.elementInfo_, param, data->nodeInfos_, data->moveSearchResult_);
            }
            break;
        default:
            break;
    }
    HILOG_DEBUG("condition id[%{public}d], result[%{public}d]", data->conditionId_, data->ret_);
}

ani_object ConvertElementInfosToJs(ani_env *env, const std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("ConvertElementInfosToAni elementInfo size(%{public}zu)", elementInfos.size());

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(elementInfos.size(), undefinedRef, &resultArray);
    if (elementInfos.empty()) {
        return resultArray;
    }
    if (resultArray == nullptr) {
        HILOG_ERROR("Failed to create result array");
        return nullptr;
    }

    for (size_t i = 0; i < elementInfos.size(); i++) {
        ani_object elementObj = CreateAniAccessibilityElement(env, elementInfos[i]);
        if (elementObj == nullptr) {
            HILOG_ERROR("Failed to create AccessibilityElement at index %{public}zu", i);
            continue;
        }
        if (ANI_OK != env->Array_Set(resultArray, static_cast<ani_size>(i), elementObj)) {
            HILOG_ERROR("Failed to set array element at index %{public}zu", i);
        }
    }

    HILOG_DEBUG("ConvertElementInfosToAni end, converted %{public}zu elements", elementInfos.size());
    return resultArray;
}

ani_object GetChildren(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    FindElementParams param = {
        .accessibilityElement_ = *element,
        .systemApi_ = true,
        .attribute_ = "children"
    };

    AttributeValueExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    ani_object resultArray = ConvertElementInfosToJs(env, param.nodeInfos_);
    return resultArray;
}

ani_object GetParent(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    FindElementParams param = {
        .accessibilityElement_ = *element,
        .systemApi_ = true,
        .attribute_ = "parent"
    };

    AttributeValueExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, param.nodeInfo_);
}

ani_object GetRootElement(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();

    AccessibilityElement* element = ANIUtils::Unwrap<AccessibilityElement>(env, thisObj);
    if (element == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityElementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    FindElementParams param = {
        .accessibilityElement_ = *element,
        .systemApi_ = true,
        .attribute_ = "rootElement"
    };

    AttributeValueExecute(&param);
    if (param.ret_ != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, param.nodeInfo_);
}

void AttributeValueExecute(FindElementParams* data)
{
    if (data == nullptr) {
        HILOG_ERROR("FindElementParams is nullptr");
        return;
    }
    bool systemApi = data->systemApi_;
    if (data->attribute_ == "parent") {
        if (data->accessibilityElement_.elementInfo_) {
            data->ret_ = AccessibleAbilityClient::GetInstance()->GetParentElementInfo(
                *data->accessibilityElement_.elementInfo_, data->nodeInfo_, systemApi);
        } else {
            HILOG_ERROR("elementInfo is nullptr");
        }
    } else if (data->attribute_ == "children") {
        if (data->accessibilityElement_.elementInfo_) {
            data->ret_ = AccessibleAbilityClient::GetInstance()->GetChildren(
                *data->accessibilityElement_.elementInfo_, data->nodeInfos_, systemApi);
        } else {
            HILOG_ERROR("elementInfo is nullptr");
        }
    } else if (data->attribute_ == "rootElement") {
        if (data->accessibilityElement_.windowInfo_) {
            data->ret_ = AccessibleAbilityClient::GetInstance()->GetRootByWindow(
                *data->accessibilityElement_.windowInfo_, data->nodeInfo_, systemApi);
        } else {
            HILOG_ERROR("windowInfo is nullptr");
        }
    } else {
        data->ret_ = RET_OK;
    }
    HILOG_DEBUG("attribute[%{public}s], result[%{public}d]", data->attribute_.c_str(), data->ret_);
}
}
}
