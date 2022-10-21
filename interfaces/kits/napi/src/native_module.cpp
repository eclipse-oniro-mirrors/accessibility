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
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_system_ability_client.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("isOpenAccessibility", NAccessibilityClient::IsOpenAccessibility),
        DECLARE_NAPI_FUNCTION("isOpenTouchGuide", NAccessibilityClient::IsOpenTouchExploration),
        DECLARE_NAPI_FUNCTION("getAbilityLists", NAccessibilityClient::GetAbilityList),
        DECLARE_NAPI_FUNCTION("getAccessibilityExtensionList", NAccessibilityClient::GetAccessibilityExtensionList),
        DECLARE_NAPI_FUNCTION("on", NAccessibilityClient::SubscribeState),
        DECLARE_NAPI_FUNCTION("off", NAccessibilityClient::UnsubscribeState),
        DECLARE_NAPI_FUNCTION("sendEvent", NAccessibilityClient::SendEvent),
        DECLARE_NAPI_FUNCTION("sendAccessibilityEvent", NAccessibilityClient::SendAccessibilityEvent),
        DECLARE_NAPI_FUNCTION("getCaptionsManager", NAccessibilityClient::GetCaptionsManager),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    (void)instance.InitializeContext();
    NAccessibilityClient::DefineJSCaptionsManager(env);
    NAccessibilityClient::DefineJSCaptionsStyle(env);
    NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(env, exports);
    NAccessibilityClient::accessibilityStateListeners_->SubscribeToFramework();
    NAccessibilityClient::touchGuideStateListeners_->SubscribeToFramework();
    NAccessibilityClient::captionListeners_->SubscribeToFramework();
    HILOG_INFO("-----Init end------");
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "accessibility",
    .nm_priv = ((void*)0),
    .reserved = {0},
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterNapiAccessibilityModule(void)
{
    napi_module_register(&_module);
}
