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

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "napi_accessibility_config.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value InitConfigModule(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", NAccessibilityConfig::SubscribeState),
        DECLARE_NAPI_FUNCTION("off", NAccessibilityConfig::UnsubscribeState),
        DECLARE_NAPI_FUNCTION("enableAbility", NAccessibilityConfig::EnableAbility),
        DECLARE_NAPI_FUNCTION("disableAbility", NAccessibilityConfig::DisableAbility),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    HILOG_INFO("-----Init config module end------");
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _config_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitConfigModule,
    .nm_modname = "accessibility.config",
    .nm_priv = ((void*)0),
    .reserved = {0},
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_config_module);
}
