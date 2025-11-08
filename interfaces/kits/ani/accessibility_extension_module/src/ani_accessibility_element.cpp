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

static ani_class g_accessibilityElementClass = nullptr;

constexpr const char *ANI_ACCESSIBILITY_ELEMENT_CLS =
    "application.AccessibilityExtensionContext.AccessibilityElementImpl";
constexpr const char *ANI_ACCESSIBILITY_ELEMENT_CLEANER_CLS =
    "application.AccessibilityExtensionContext.AccessibilityElementCleaner";

bool InitializeAccessibilityElementClass(ani_env *env)
{
    if (g_accessibilityElementClass != nullptr) {
        return true;
    }

    HILOG_DEBUG("InitializeAccessibilityElementClass begin");

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
        ani_native_function {"findElementsByConditionNative", nullptr, reinterpret_cast<void *>(FindElementsByCondition)},
    };

    if (ANI_OK != env->Class_BindNativeMethods(g_accessibilityElementClass, methods.data(), methods.size())) {
        HILOG_ERROR("Cannot bind native methods to element");
        g_accessibilityElementClass = nullptr;
        return false;
    }

    ani_class cleanerCls;
    arkts::ani_signature::Type cleanerName = arkts::ani_signature::Builder::BuildClass(
        ANI_ACCESSIBILITY_ELEMENT_CLEANER_CLS);
    if (ANI_OK != env->FindClass(cleanerName.Descriptor().c_str(), &cleanerCls)) {
        HILOG_ERROR(" not found class AccessibilityElement");
        return false;
    }
    std::array cleanMethod = {
        ani_native_function {"cleanNative", ":", reinterpret_cast<void *>(Clean)},
    };
    if (ANI_OK != env->Class_BindNativeMethods(cleanerCls, cleanMethod.data(), cleanMethod.size())) {
        HILOG_ERROR("Cannot bind native methods to AccessibilityElementCleaner");
        return false;
    }

    HILOG_DEBUG("InitializeAccessibilityElementClass end - methods bound successfully");
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

void SetAccessibilityElementField(ani_env *env, ani_object& elementObj, const AccessibilityElementInfo& elementInfo)
{
    HILOG_DEBUG("SetAccessibilityElementField begin");
    // mainWindowId  int
    if (!ANIUtils::SetIntField(env, elementObj, "mainWindowId", elementInfo.GetMainWindowId())) {
        HILOG_ERROR("Failed to set mainWindowId");
    }
    HILOG_INFO("setAccessibilityElementField end");
}

void SetAccessibilityElementField(ani_env *env, ani_object& elementObj, const AccessibilityWindowInfo& windowInfo)
{
    // mainWindowId  int
    if (!ANIUtils::SetIntField(env, elementObj, "mainWindowId", windowInfo.GetMainWindowId())) {
        HILOG_ERROR("Failed to set mainWindowId");
    }
    HILOG_INFO("setAccessibilityElementField end");
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

ani_object FindElement(ani_env *env, ani_object thisObj, ani_string type, ani_double condition)
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
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    std::string ruleStr = ANIUtils::ANIStringToStdString(env, rule);
    std::string conditionStr = ANIUtils::ANIStringToStdString(env, condition);

    FindElementByConditionParams param = {FIND_ELEMENT_BY_CONDITION, conditionStr, *element};
    param.rule_ = ruleStr;

    FindElementExecute(&param);
    if (RET_OK != param.ret_) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(param.ret_));
        return nullptr;
    }
    return nullptr;
}

FocusMoveDirection ConvertStringToDirection(const std::string &str)
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
                HILOG_DEBUG("elementId is %{public}lld windowId: %{public}d", elementId, windowId);
                data->ret_ = AccessibleAbilityClient::GetInstance()->GetByElementId(
                    elementId, windowId, data->nodeInfo_);
            }
            break;
        case FindElementCondition::FIND_ELEMENT_BY_CONDITION:
            {
                HILOG_DEBUG("FIND_ELEMENT_BY_CONDITION");
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
}
}
