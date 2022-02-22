/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessibility_system_ability_client.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_window_info.h"
#include "napi_accessibility_info.h"

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
        DECLARE_NAPI_FUNCTION("on", NAccessibilityClient::SubscribeState),
        DECLARE_NAPI_FUNCTION("off", NAccessibilityClient::UnsubscribeState),
        DECLARE_NAPI_FUNCTION("sendEvent", NAccessibilityClient::SendEvent),
        DECLARE_NAPI_FUNCTION("getCaptionProperty", NAccessibilityClient::GetCaptionProperty),
        DECLARE_NAPI_FUNCTION("setCaptionProperty", NAccessibilityClient::SetCaptionProperty),
        DECLARE_NAPI_FUNCTION("getCaptionState", NAccessibilityClient::GetCaptionState),
        DECLARE_NAPI_FUNCTION("setCaptionState", NAccessibilityClient::SetCaptionState),
        DECLARE_NAPI_FUNCTION("getEnabled", NAccessibilityClient::GetEnabled),
        DECLARE_NAPI_FUNCTION("setEnabled", NAccessibilityClient::SetEnabled),
        DECLARE_NAPI_FUNCTION("getTouchGuideState", NAccessibilityClient::GetTouchGuideState),
        DECLARE_NAPI_FUNCTION("setTouchGuideState", NAccessibilityClient::SetTouchGuideState),
        DECLARE_NAPI_FUNCTION("getGestureState", NAccessibilityClient::GetGestureState),
        DECLARE_NAPI_FUNCTION("setGestureState", NAccessibilityClient::SetGestureState),
        DECLARE_NAPI_FUNCTION("getKeyEventObserverState", NAccessibilityClient::GetKeyEventObserverState),
        DECLARE_NAPI_FUNCTION("setKeyEventObserverState", NAccessibilityClient::SetKeyEventObserverState),
        DECLARE_NAPI_FUNCTION("extentionEnabled", NAccessibilityClient::ExtentionEnabled),
        DECLARE_NAPI_FUNCTION("extentionDisabled", NAccessibilityClient::ExtentionDisabled),
        DECLARE_NAPI_FUNCTION("getCaptionsManager", NAccessibilityClient::GetCaptionsManager),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    NAccessibilityClient::DefineJSCaptionsManager(env);
    NAccessibilityClient::DefineJSCaptionsStyle(env);
    NElementInfo::DefineJSElementInfo(env);
    NAccessibilityWindowInfo::DefineJSAccessibilityWindowInfo(env);
    NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(env);
    NAPI_CALL(env, napi_set_named_property(env, exports, "EventInfo", NAccessibilityEventInfo::cons_));

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
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
