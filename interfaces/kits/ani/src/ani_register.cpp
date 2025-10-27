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
#include "ani_utils.h"
#include "accessibility_config.h"
#include "hilog_wrapper.h"
#include <ani_signature_builder.h>

using namespace arkts::ani_signature;

constexpr int32_t INVALID_ANI_VERSION = 1;
constexpr int32_t MODULE_NOT_FOUND = 2;
constexpr int32_t NAMESPACE_NOT_FOUND = 3;
constexpr int32_t BIND_METHOD_FAILED = 4;
constexpr int32_t CLASS_NOT_FOUND = 5;

static bool BindMethod(ani_env *env, ani_namespace cls, ani_module mod);
static bool BindCaptionMethod(ani_env *env, ani_class cls);

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        HILOG_ERROR("Unsupported ANI_VERSION_1");
        return (ani_status)INVALID_ANI_VERSION;
    }

    Namespace accessibilityNamespace = Builder::BuildNamespace("@ohos.accessibility.accessibility");
    ani_namespace ns;
    if (env->FindNamespace(accessibilityNamespace.Descriptor().c_str(), &ns) != ANI_OK) {
        HILOG_ERROR("namespace accessibility not found");
        return (ani_status)NAMESPACE_NOT_FOUND;
    }

    Module accessibilityModule = Builder::BuildModule("@ohos.accessibility");
    ani_module mod;
    if (env->FindModule(accessibilityModule.Descriptor().c_str(), &mod) != ANI_OK) {
        HILOG_ERROR("module accessibility not found");
        return (ani_status)MODULE_NOT_FOUND;
    }

    if (!BindMethod(env, ns, mod)) {
        HILOG_ERROR("bind method failed");
        return (ani_status)BIND_METHOD_FAILED;
    }

    ani_class cls = nullptr;
    if (env->FindClass(Builder::BuildClass("@ohos.accessibility.accessibility.CaptionsManagerImpl")
        .Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("class CaptionsManagerImpl not found");
        return (ani_status)CLASS_NOT_FOUND;
    }

    if (!BindCaptionMethod(env, cls)) {
        HILOG_ERROR("CaptionsManagerImpl bind method failed");
        return (ani_status)BIND_METHOD_FAILED;
    }

    ANIAccessibilityClient::accessibilityStateListeners_->SubscribeToFramework();
    ANIAccessibilityClient::touchGuideStateListeners_->SubscribeToFramework();
    ANIAccessibilityClient::screenReaderStateListeners_->SubscribeToFramework();
    ANIAccessibilityClient::captionListeners_->SubscribeToFramework();

    *result = ANI_VERSION_1;
    return ANI_OK;
}

static bool BindMethod(ani_env *env, ani_namespace ns, ani_module mod)
{
    std::array methods = {
        ani_native_function {"isOpenTouchGuideSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::IsOpenTouchGuideSync)},
        ani_native_function {"isOpenAccessibilitySync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::IsOpenAccessibilitySync)},
        ani_native_function {"onStateChange", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::SubscribeState)},
        ani_native_function {"offStateChange", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::UnsubscribeState)},
        ani_native_function {"offAll", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeStateAll)},
        ani_native_function {"isScreenReaderOpenSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityClient::IsScreenReaderOpenSync)},
        ani_native_function {"getAccessibilityExtensionListSync", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::GetAccessibilityExtensionListSync)},
        ani_native_function {"getCaptionsManager", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::GetCaptionsManager)},
   };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    std::array moduleMethods = {
        ani_native_function {"sendAccessibilityEventSync", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::SendAccessibilityEvent)},
    };
    if (env->Module_BindNativeFunctions(mod, moduleMethods.data(), moduleMethods.size()) != ANI_OK) {
        return false;
    };

    return true;
}

static bool BindCaptionMethod(ani_env *env, ani_class cls)
{
    std::array methods = {
        ani_native_function {"getEnabled", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::GetEnabled)},
        ani_native_function {"getStyle", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::GetStyle)},
        ani_native_function {"setEnabled", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::SetEnabled)},
        ani_native_function {"setStyle", nullptr, reinterpret_cast<void *>(ANIAccessibilityClient::SetStyle)},
        ani_native_function {"onEnableChange", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::SubscribeEnableChange)},
        ani_native_function {"onStyleChange", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::SubscribeStyleChange)},
        ani_native_function {"offEnableChangeWithCallback", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeEnableChangeWithCallback)},
        ani_native_function {"offEnableChangeAll", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeEnableChangeAll)},
        ani_native_function {"offStyleChangeWithCallback", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeStyleChangeWithCallback)},
        ani_native_function {"offStyleChangeAll", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityClient::UnsubscribeStyleChangeAll)},
   };

    if (env->Class_BindNativeMethods(cls, methods.data(), methods.size()) != ANI_OK) {
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
    if (ANIAccessibilityClient::screenReaderStateListeners_) {
        ANIAccessibilityClient::screenReaderStateListeners_->UnsubscribeFromFramework();
    }
    if (ANIAccessibilityClient::captionListeners_) {
        ANIAccessibilityClient::captionListeners_->UnsubscribeFromFramework();
    }

    return ANI_OK;
}