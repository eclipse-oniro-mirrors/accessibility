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
#include "ani_accessibility_config.h"
#include "ani_utils.h"
#include "hilog_wrapper.h"
#include <ani_signature_builder.h>

using namespace arkts::ani_signature;

constexpr int32_t INVALID_ANI_VERSION = 1;
constexpr int32_t MODULE_NOT_FOUND = 2;
constexpr int32_t CLASS_NOT_FOUND = 3;
constexpr int32_t BIND_METHOD_FAILED = 4;
constexpr int32_t NAMESPACE_NOT_FOUND = 5;
static bool BindBooleanMethod(ani_env *env);
static bool BindIntMethod(ani_env *env);
static bool BindDoubleMethod(ani_env *env);
static bool BindStringMethod(ani_env *env);
static bool BindVectorStringMethod(ani_env *env);
static bool BindDaltonizationColorFilterMethod(ani_env *env);
static bool BindClickResponseTimeMethod(ani_env *env);
static bool BindRepeatClickIntervalMethod(ani_env *env);
static bool BindCaptionsStyleMethod(ani_env *env);
static bool BindAllMethod(ani_env *env);

static bool NamespaceBindMethod(ani_env *env, ani_namespace ns, ani_module mod)
{
    std::array methods = {
        ani_native_function{"on", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeState)},
        ani_native_function{"offObserver", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnsubscribeState)},
        ani_native_function{"offObservers", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnsubscribeStates)}
    };

    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        HILOG_ERROR(" Namespace_BindNativeFunctions error");
        return false;
    };

    std::array ModuleMethods = {
        ani_native_function{"enableAbilitySync", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityConfig::EnableAbilitySync)},
        ani_native_function{"disableAbilitySync", nullptr,
            reinterpret_cast<void *>(ANIAccessibilityConfig::DisableAbilitySync)},
    };

    if (env->Module_BindNativeFunctions(mod, ModuleMethods.data(), ModuleMethods.size()) != ANI_OK) {
        HILOG_ERROR(" Module_BindNativeFunctions error");
        return false;
    };
    return true;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        HILOG_ERROR("Unsupported ANI_VERSION_1");
        return (ani_status)INVALID_ANI_VERSION;
    }

    Namespace nsName = Builder::BuildNamespace("@ohos.accessibility.config.config");
    ani_namespace ns;
    if (env->FindNamespace(nsName.Descriptor().c_str(), &ns) != ANI_OK) {
        HILOG_ERROR("namespace config not found");
        return (ani_status)NAMESPACE_NOT_FOUND;
    }

    Module moduleName = Builder::BuildModule("@ohos.accessibility.config");
    ani_module mod;
    if (env->FindModule(moduleName.Descriptor().c_str(), &mod) != ANI_OK) {
        HILOG_ERROR("module config not found");
        return (ani_status)MODULE_NOT_FOUND;
    }

    if (!NamespaceBindMethod(env, ns, mod)) {
        HILOG_ERROR("bind method Namespace failed");
        return (ani_status)BIND_METHOD_FAILED;
    }

    if (!BindAllMethod(env)) {
        HILOG_ERROR(" bind method failed");
        return (ani_status)ANIUtils::BIND_METHOD_FAILED;
    }

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    (void)instance.InitializeContext();

    if (ANIAccessibilityConfig::configObservers_) {
        ANIAccessibilityConfig::configObservers_->SubscribeToFramework();
    }

    if (ANIAccessibilityConfig::enableAbilityListsObservers_) {
        ANIAccessibilityConfig::enableAbilityListsObservers_->SubscribeToFramework();
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}

static bool BindAllMethod(ani_env *env)
{
    if (!BindBooleanMethod(env)) {
        HILOG_ERROR(" bind method boolean failed");
        return false;
    }

    if (!BindIntMethod(env)) {
        HILOG_ERROR(" bind method int failed");
        return false;
    }

    if (!BindDoubleMethod(env)) {
        HILOG_ERROR(" bind method double failed");
        return false;
    }

    if (!BindStringMethod(env)) {
        HILOG_ERROR(" bind method String failed");
        return false;
    }

    if (!BindVectorStringMethod(env)) {
        HILOG_ERROR(" bind method VectorString failed");
        return false;
    }

    if (!BindDaltonizationColorFilterMethod(env)) {
        HILOG_ERROR(" bind method DaltonizationColorFilter failed");
        return false;
    }

    if (!BindClickResponseTimeMethod(env)) {
        HILOG_ERROR(" bind method ClickResponseTime failed");
        return false;
    }

    if (!BindRepeatClickIntervalMethod(env)) {
        HILOG_ERROR(" bind method epeatClickInterval failed");
        return false;
    }

    if (!BindCaptionsStyleMethod(env)) {
        HILOG_ERROR(" bind method failed");
        return false;
    }
    return true;
}

static bool BindBooleanMethod(ani_env *env)
{
    Type classNameBoolean = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplBoolean");
    ani_class clsBoolean;
    if (env->FindClass(classNameBoolean.Descriptor().c_str(), &clsBoolean) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplBoolean not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncboolean)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncboolean)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsBoolean, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindDoubleMethod(ani_env *env)
{
    Type classNameDouble = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplDouble");
    ani_class clsDouble;
    if (env->FindClass(classNameDouble.Descriptor().c_str(), &clsDouble) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplDouble not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncDouble)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncDouble)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsDouble, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindIntMethod(ani_env *env)
{
    Type classNameInt = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplInt");
    ani_class clsInt;
    if (env->FindClass(classNameInt.Descriptor().c_str(), &clsInt) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplInt not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncInt)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncInt)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsInt, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindStringMethod(ani_env *env)
{
    Type classNameString = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplString");
    ani_class clsString;
    if (env->FindClass(classNameString.Descriptor().c_str(), &clsString) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplString not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncString)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncString)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsString, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindVectorStringMethod(ani_env *env)
{
    Type classNameArrString = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplArrString");
    ani_class clsArrString;
    if (env->FindClass(classNameArrString.Descriptor().c_str(), &clsArrString) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplArrString not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncVectorString)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncVectorString)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsArrString, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindDaltonizationColorFilterMethod(ani_env *env)
{
    Type classNameDalColFilte = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplDalColFilter");
    ani_class clsDalColFilte;
    if (env->FindClass(classNameDalColFilte.Descriptor().c_str(), &clsDalColFilte) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplDalColFilter not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncDaltonizationColorFilter)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncDaltonizationColorFilter)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsDalColFilte, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindClickResponseTimeMethod(ani_env *env)
{
    Type classNameCliResTime = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplCliResTime");
    ani_class clsCliResTime;
    if (env->FindClass(classNameCliResTime.Descriptor().c_str(), &clsCliResTime) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplCliResTime not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncClickResponseTime)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncClickResponseTime)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsCliResTime, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindRepeatClickIntervalMethod(ani_env *env)
{
    Type classNameRepeateClickInterval =
        Builder::BuildClass("@ohos.accessibility.config.config.ConfigRepeateClickInterval");
    ani_class clsRepeateClickInterval;
    if (env->FindClass(classNameRepeateClickInterval.Descriptor().c_str(), &clsRepeateClickInterval) != ANI_OK) {
        HILOG_ERROR(" config class ConfigRepeateClickInterval not found");
        return false;
    }

    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncRepeatClickInterval)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncRepeatClickInterval)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsRepeateClickInterval, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

static bool BindCaptionsStyleMethod(ani_env *env)
{
    Type classNameCaptionsStyle = Builder::BuildClass("@ohos.accessibility.config.config.ConfigImplCaptionsStyle");
    ani_class clsCaptionsStyle;
    if (env->FindClass(classNameCaptionsStyle.Descriptor().c_str(), &clsCaptionsStyle) != ANI_OK) {
        HILOG_ERROR(" config class ConfigImplCaptionsStyle not found");
        return false;
    }
    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetSyncCaptionsStyle)},
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetSyncCaptionsStyle)},
        ani_native_function {"onOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SubscribeConfigObserver)},
        ani_native_function {"offOnly", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObservers)},
        ani_native_function {"offWithCallback", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::UnSubscribeConfigObserver)},
    };

    if (env->Class_BindNativeMethods(clsCaptionsStyle, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };
    return true;
}

ANI_EXPORT ani_status ANI_Destructor(ani_vm *vm)
{
    HILOG_INFO("accessibiliyConfig cleanup");
    if (ANIAccessibilityConfig::configObservers_) {
        ANIAccessibilityConfig::configObservers_->UnsubscribeFromFramework();
    }
    if (ANIAccessibilityConfig::enableAbilityListsObservers_) {
        ANIAccessibilityConfig::enableAbilityListsObservers_->UnsubscribeFromFramework();
    }
    return ANI_OK;
}