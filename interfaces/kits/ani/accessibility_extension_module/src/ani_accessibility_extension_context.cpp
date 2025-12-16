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
#include "ets_extension_context.h"

#include "ani_accessibility_extension_context.h"
#include "ani_common_want.h"
#include "ani_accessibility_element.h"
#include "ani_utils.h"
#include "hilog_wrapper.h"
#include "tokenid_kit.h"
#include "accesstoken_kit.h"
#include <ani_signature_builder.h>

namespace OHOS {
namespace Accessibility {

using namespace arkts::ani_signature;
using namespace Security::AccessToken;

constexpr const char *ANI_EXTENSION_CONTEXT_CLS =
    "application.AccessibilityExtensionContext.AccessibilityExtensionContext";
constexpr const char *WANT_CLS = "@ohos.app.ability.Want.Want";

static void StartAbility([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_object wantObj)
{
    HILOG_INFO("StartAbility begin");
    AccessibilityExtensionContext *extensionContext = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, object);
    if (extensionContext == nullptr) {
        HILOG_ERROR("extensionContext is nullptr");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return;
    }
    arkts::ani_signature::Type wantClass = Builder::BuildClass(WANT_CLS);
    ani_class cls;
    if (env->FindClass(wantClass.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("find class Want failed");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }
    ani_boolean isWant;
    if (ANI_OK != env->Object_InstanceOf(static_cast<ani_object>(object), static_cast<ani_type>(cls),
        &isWant)) {
        HILOG_ERROR("call object instance of Want failed");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }
    AAFwk::Want want;
    if (!AppExecFwk::UnwrapWant(env, wantObj, want)) {
        HILOG_ERROR("call UnWrapWant failed");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }
    RetError innerErrorCode = extensionContext->StartAbility(want);
    if (innerErrorCode != RET_OK) {
        HILOG_ERROR("StartAbility error");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(innerErrorCode));
        return;
    }
    HILOG_DEBUG("StartAbility end");
}

static ani_object GetDefaultFocusedElementIdsNative([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_int windowId)
{
    HILOG_DEBUG("getDefaultFocusedElementIdsNative begin");
    AccessibilityExtensionContext *extensionContext = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, object);
    if (extensionContext == nullptr) {
        HILOG_ERROR("extensionContext is nullptr");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return nullptr;
    }

    HILOG_DEBUG("windowId: %{public}d", windowId);
    auto accessibilityElements = std::make_shared<std::vector<OHOS::Accessibility::AccessibilityElementInfo>>();
    RetError ret = extensionContext->GetDefaultFocusedElementIds(static_cast<int32_t>(windowId),
        *accessibilityElements);
    if (ret != RET_OK) {
        HILOG_ERROR("GetDefaultFocusedElementIds error");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(accessibilityElements->size(), undefinedRef, &resultArray);
    if (resultArray == nullptr) {
        HILOG_ERROR("Failed to create result array");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }

    for (size_t i = 0; i < accessibilityElements->size(); i++) {
        int64_t elementId = (*accessibilityElements)[i].GetAccessibilityId();
        ani_object idObj;
        if (ANIUtils::CreateAniLong(env, elementId, idObj) != ANI_OK) {
            HILOG_ERROR("CreateAniLong failed");
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(resultArray, static_cast<ani_size>(i), idObj)) {
            HILOG_ERROR("Failed to set array element at index %{public}zu", i);
            ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
            return nullptr;
        }
    }

    HILOG_DEBUG("getDefaultFocusedElementIdsNative end, returned %{public}zu elements", accessibilityElements->size());
    return resultArray;
}

static ani_object GetElementsNative(ani_env *env, ani_object thisObj, ani_int windowId, ani_object elementId)
{
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    ani_boolean isUndefined;
    int64_t elementIdValue = -1;
    env->Reference_IsUndefined(elementId, &isUndefined);
    if (!isUndefined) {
        ani_long result;
        if (ANI_OK != env->Object_CallMethodByName_Long(elementId, "longValue", nullptr, &result)) {
            HILOG_ERROR("Failed to get elementId from ani_object");
            return nullptr;
        }
        elementIdValue = static_cast<int64_t>(result);
    }
    HILOG_DEBUG("GetElementsNative begin, windowId: %{public}d, elementId: %{public}lld", windowId, elementIdValue);
    std::vector<AccessibilityElementInfo> accessibilityElementsInfo;
    RetError ret = context->GetElements(static_cast<int32_t>(windowId), elementIdValue, accessibilityElementsInfo);
    if (ret != RET_OK) {
        HILOG_ERROR("GetElements error");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
        return nullptr;
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(accessibilityElementsInfo.size(), undefinedRef, &resultArray);
    if (resultArray == nullptr) {
        HILOG_ERROR("Failed to create result array");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    ani_status status;
    for (size_t i = 0; i < accessibilityElementsInfo.size(); i++) {
        ani_object elementObj = CreateAniAccessibilityElement(env, accessibilityElementsInfo[i]);
        if (elementObj == nullptr) {
            HILOG_ERROR("Failed to create AccessibilityElement at index %{public}zu", i);
            continue;
        }
        if (ANI_OK != (status = env->Array_Set(resultArray, static_cast<ani_size>(i), elementObj))) {
            HILOG_ERROR("Failed to set array element at index %{public}zu, status: %{public}d", i, status);
        }
    }
    return resultArray;
}

static ani_object GetRootInActiveWindow(ani_env *env, ani_object thisObj, ani_object windowId)
{
    HILOG_INFO("GetRootInActiveWindow begin");
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    auto elementInfo = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    RetError ret = RET_ERR_FAILED;
    ani_boolean isUndefined;
    env->Reference_IsUndefined(windowId, &isUndefined);
    if (isUndefined) {
        ret = context->GetRoot(*elementInfo, true);
    } else {
        ani_int id;
        if (ANI_OK !=env->Object_CallMethodByName_Int(windowId, "intValue", nullptr, &id)) {
            ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
            return nullptr;
        }
        AccessibilityWindowInfo windowInfo;
        windowInfo.SetWindowId(static_cast<int32_t>(id));
        windowInfo.SetMainWindowId(static_cast<int32_t>(id));
        ret = context->GetRootByWindow(windowInfo, *elementInfo, true);
    }
    HILOG_INFO("GetRootInActiveWindow end, ret: %{public}d", ret);
    HILOG_INFO("GetRoot end, mainWindowId: %{public}d, bundleName: %{public}s",
        elementInfo->GetMainWindowId(), elementInfo->GetBundleName().c_str());
    if (RET_OK != ret) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, *elementInfo);
}

static ani_object GetAccessibilityFocusedElement(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    auto elementInfo = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    RetError ret = context->GetFocus(FOCUS_TYPE_ACCESSIBILITY, *elementInfo, true);
    if (ret != RET_OK) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
        return nullptr;
    }
    return CreateAniAccessibilityElement(env, *elementInfo);
}

static ani_object GetAccessibilityWindowsSync(ani_env *env, ani_object thisObj, ani_object displayId)
{
    HILOG_DEBUG();
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    auto accessibilityWindows = std::make_shared<std::vector<OHOS::Accessibility::AccessibilityWindowInfo>>();
    RetError ret = RET_ERR_FAILED;
    ani_boolean isUndefined;
    env->Reference_IsUndefined(displayId, &isUndefined);
    if (isUndefined) {
        ret = context->GetWindows(*accessibilityWindows, true);
    } else {
        ani_long id;
        if (env->Object_CallMethodByName_Long(displayId, "longValue", nullptr, &id) != ANI_OK) {
            ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
            return nullptr;
        }
        if (id < 0) {
            HILOG_ERROR("displayId is error: %{public}" PRId64 "", id);
            ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
            return nullptr;
        }
        ret = context->GetWindows(static_cast<uint64_t>(id), *accessibilityWindows, true);
    }
    if (ret != RET_OK) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
        return nullptr;
    }
    return ConvertElementInfosToJs(env, *accessibilityWindows);
}

static bool CheckExtensionAbilityPermission()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == TOKEN_INVALID) {
        HILOG_ERROR("AccessToken type invalid!");
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(tokenId, OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_ERROR("AccessTokenID denied!");
        return false;
    }
    return true;
}

static void On(ani_env *env, ani_object thisObj, ani_string type, ani_fn_object callback)
{
    HILOG_DEBUG();
    if (!CheckExtensionAbilityPermission()) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NO_PERMISSION));
        return;
    }
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        return;
    }
    std::string inputType = ANIUtils::ANIStringToStdString(env, type);
    ani_ref fnRef;
    ani_status status = env->GlobalReference_Create(callback, &fnRef);
    if (inputType == "preDisconnect" && status == ANI_OK) {
        ani_vm *vm_;
        env->GetVM(&vm_);
        std::shared_ptr<AniDisconnectCallback> disCallback = std::make_shared<AniDisconnectCallback>(vm_, fnRef);
        std::shared_ptr<DisconnectCallback> baseCallback = std::static_pointer_cast<DisconnectCallback>(disCallback);
        context->RegisterDisconnectCallback(baseCallback);
    } else {
        HILOG_ERROR("Unsupported event type, Register callback failed");
    }
}

static void Off(ani_env *env, ani_object thisObj, ani_string type, ani_fn_object callback)
{
    HILOG_DEBUG();
    if (!CheckExtensionAbilityPermission()) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NO_PERMISSION));
        return;
    }
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        return;
    }
    ani_ref fnRef = nullptr;
    ani_boolean isUndefined;
    env->Reference_IsUndefined(callback, &isUndefined);
    if (!isUndefined) {
        ani_status status = env->GlobalReference_Create(callback, &fnRef);
        if (status != ANI_OK) {
            HILOG_ERROR("Failed to create global reference for callback");
            return;
        }
    }
    std::string inputType = ANIUtils::ANIStringToStdString(env, type);
    if (inputType == "preDisconnect") {
        ani_vm *vm_;
        env->GetVM(&vm_);
        std::shared_ptr<AniDisconnectCallback> disCallback = std::make_shared<AniDisconnectCallback>(vm_, fnRef);
        std::shared_ptr<DisconnectCallback> baseCallback = std::static_pointer_cast<DisconnectCallback>(disCallback);
        context->UnRegisterDisconnectCallback(baseCallback);
    } else {
        HILOG_ERROR("Unsupported event type, Unregister callback failed");
    }
}

static ani_object NotifyDisconnect(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();
    if (!CheckExtensionAbilityPermission()) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NO_PERMISSION));
        return nullptr;
    }
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return nullptr;
    }
    RetError ret = context->NotifyDisconnect();
    if (RET_OK != ret) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    return nullptr;
}

static void HoldRunningLockSync(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    RetError ret = context->HoldRunningLock();
    if (RET_OK != ret) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
}

static void UnholdRunningLockSync(ani_env *env, ani_object thisObj)
{
    HILOG_DEBUG();
    AccessibilityExtensionContext* context = ANIUtils::Unwrap<AccessibilityExtensionContext>(env, thisObj);
    if (context == nullptr) {
        HILOG_ERROR("Failed to unwrap AccessibilityExtensionContext");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    RetError ret = context->UnholdRunningLock();
    if (RET_OK != ret) {
        HILOG_ERROR("Failed to get elementInfo");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
}

ani_object CreateAniAccessibilityExtensionContext(ani_env *env, std::shared_ptr<AccessibilityExtensionContext> context,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application)
{
    HILOG_DEBUG("CreateAniAccessibilityExtensionContext begin");
    if (!InitializeAccessibilityElementClass(env)) {
        HILOG_ERROR("Failed to initialize AccessibilityElement class");
        return nullptr;
    }
    ani_class cls;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_EXTENSION_CONTEXT_CLS);
    if (ANI_OK != env->FindClass(className.Descriptor().c_str(), &cls)) {
        HILOG_ERROR("Ani FindClass err: extention context");
        return nullptr;
    }
    std::array methods = {
        ani_native_function {"startAbilityNative", nullptr, reinterpret_cast<void *>(StartAbility)},
        ani_native_function {"getDefaultFocusedElementIdsNative", nullptr,
            reinterpret_cast<void *>(GetDefaultFocusedElementIdsNative)},
        ani_native_function {"getElementsNative", nullptr, reinterpret_cast<void *>(GetElementsNative)},
        ani_native_function {"getRootInActiveWindowNative", nullptr, reinterpret_cast<void *>(GetRootInActiveWindow)},
        ani_native_function {"getAccessibilityWindowsSync", nullptr,
            reinterpret_cast<void *>(GetAccessibilityWindowsSync)},
        ani_native_function {"getAccessibilityFocusedElementNative", nullptr,
            reinterpret_cast<void *>(GetAccessibilityFocusedElement)},
        ani_native_function {"notifyDisconnect", nullptr, reinterpret_cast<void *>(NotifyDisconnect)},
        ani_native_function {"onNative", nullptr, reinterpret_cast<void *>(On)},
        ani_native_function {"offNative", nullptr, reinterpret_cast<void *>(Off)},
        ani_native_function {"holdRunningLockSync", nullptr, reinterpret_cast<void *>(HoldRunningLockSync)},
        ani_native_function {"unholdRunningLockSync", nullptr, reinterpret_cast<void *>(UnholdRunningLockSync)},
    };
    HILOG_DEBUG("CreateAniAccessibilityExtensionContext bind context methods");
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR("Cannot bind native methods to extension context");
        return nullptr;
    }
    ani_object contextObj = ANIUtils::CreateObject(env, cls);
    if (ANI_OK != ANIUtils::Wrap(env, contextObj, context.get())) {
        HILOG_ERROR("Cannot wrap native object");
        return nullptr;
    }

    OHOS::AbilityRuntime::CreateEtsExtensionContext(env, cls, contextObj, context, context->GetAbilityInfo());
    HILOG_INFO("CreateAniAccessibilityExtensionContext end");
    return contextObj;
}
}
}
