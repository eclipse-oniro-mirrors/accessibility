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
#include "hilog_wrapper.h"

constexpr int32_t INVALID_ANI_VERSION = 1;
constexpr int32_t MODULE_NOT_FOUND = 2;
constexpr int32_t CLASS_NOT_FOUND = 3;
constexpr int32_t BIND_METHOD_FAILED = 4;

static bool BindMethod(ani_env *env, ani_class cls);

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        HILOG_ERROR("Unsupported ANI_VERSION_1");
        return (ani_status)INVALID_ANI_VERSION;
    }

    static const char *className = "L@ohos/accessibility/config/config/HighContrastTextConfig;";
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        HILOG_ERROR("class not found: %{public}s", className);
        return (ani_status)CLASS_NOT_FOUND;
    }

    if (!BindMethod(env, cls)) {
        HILOG_ERROR("bind method failed");
        return (ani_status)BIND_METHOD_FAILED;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}

static bool BindMethod(ani_env *env, ani_class cls)
{
    std::array methods = {
        ani_native_function {"setSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::SetHighContrastText)},
    };

    if (env->Class_BindNativeMethods(cls, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    methods = {
        ani_native_function {"getSync", nullptr, reinterpret_cast<void *>(
            ANIAccessibilityConfig::GetHighContrastText)},
    };

    if (env->Class_BindNativeMethods(cls, methods.data(), methods.size()) != ANI_OK) {
        return false;
    };

    return true;
}

ANI_EXPORT ani_status ANI_Destructor(ani_vm *vm)
{
    return ANI_OK;
}