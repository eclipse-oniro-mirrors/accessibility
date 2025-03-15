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
#include "ani_accessibility_system_ability_client.h"
#include "hilog_wrapper.h"

constexpr int32_t INVALID_ANI_VERSION = 1;
constexpr int32_t CLASS_NOT_FOUND = 2;
constexpr int32_t NAMESPACE_NOT_FOUND = 3;
constexpr int32_t BIND_METHOD_FAILED = 4;

bool BindMethod(ani_env *env, ani_namespace cls, ani_class globalCls);

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        HILOG_ERROR("Unsupported ANI_VERSION_1");
        return (ani_status)INVALID_ANI_VERSION;
    }

    static const char *nsName = "L@ohos/accessibility/accessibility;";
    ani_namespace ns;
    if (env->FindNamespace(nsName, &ns) != ANI_OK) {
        HILOG_ERROR("namespace not found: %{public}s", nsName);
        return (ani_status)NAMESPACE_NOT_FOUND;
    }

    static const char *globalClassName = "L@ohos/accessibility/ETSGLOBAL;";
    ani_class globalCls;
    if (env->FindClass(globalClassName, &globalCls) != ANI_OK) {
        HILOG_ERROR("class not found: %{public}s", globalClassName);
        return (ani_status)CLASS_NOT_FOUND;
    }

    if (!BindMethod(env, ns, globalCls)) {
        HILOG_ERROR("bind method failed");
        return (ani_status)BIND_METHOD_FAILED;
    }

    ANIAccessibilityClient::accessibilityStateListeners_->SubscribeToFramework();
    ANIAccessibilityClient::touchGuideStateListeners_->SubscribeToFramework();

    *result = ANI_VERSION_1;
    return ANI_OK;
}

bool BindMethod(ani_env *env, ani_namespace ns, ani_class globalCls)
{
    std::array methods = {
        ani_native_function {"isOpenTouchGuideSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::IsOpenTouchGuideSync)},
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"isOpenAccessibilitySync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::IsOpenAccessibilitySync)},
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"on", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::SubscribeState)},
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"offObserver", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::UnsubscribeState)},
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"offAll", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeStateAll)},
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"sendAccessibilityEventSync", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::SendAccessibilityEvent)},
    };

    if (env->Class_BindNativeMethods(globalCls, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    return true;
}

ANI_EXPORT ani_status ANI_Destructor(ani_vm *vm)
{
    if (ANIAccessibilityClient::accessibilityStateListeners_) {
        ANIAccessibilityClient::accessibilityStateListeners_->UnsubscribeFromFramework();
    }
    if (ANIAccessibilityClient::touchGuideStateListeners_) {
        ANIAccessibilityClient::touchGuideStateListeners_->UnsubscribeFromFramework();
    }

    return ANI_OK;
}