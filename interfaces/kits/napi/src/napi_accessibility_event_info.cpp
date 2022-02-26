/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "napi_accessibility_event_info.h"
#include "accessibility_event_info.h"
#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessibility_info.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_window_info.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

napi_value NAccessibilityEventInfo::cons_ = nullptr;

void NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(napi_env env)
{
    napi_property_descriptor descForAccessibilityEventInfo[] = {
        DECLARE_NAPI_FUNCTION("getSource", NAccessibilityEventInfo::GetSource),
    };

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "EventInfo",
            NAPI_AUTO_LENGTH,
            NAccessibilityEventInfo::JSConstructor,
            nullptr,
            sizeof(descForAccessibilityEventInfo) / sizeof(descForAccessibilityEventInfo[0]),
            descForAccessibilityEventInfo,
            &NAccessibilityEventInfo::cons_));
}

napi_value NAccessibilityEventInfo::JSConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value NAccessibilityEventInfo::GetSource(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityEventInfo* eventInfo = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    napi_unwrap(env, thisVar, (void**)&eventInfo);
    NAccessibilityEventInfoData *callbackInfo = new NAccessibilityEventInfoData();
    callbackInfo->eventInfo_ = *eventInfo;
    napi_value promise = nullptr;
    if (argc > 0) {
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetSource", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityEventInfoData *callbackInfo = (NAccessibilityEventInfoData*)data;
            AccessibilityEventInfo eventInfo = callbackInfo->eventInfo_;
            callbackInfo->result_ = eventInfo.GetSource(callbackInfo->nodeInfo_);
        },
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityEventInfoData* callbackInfo = (NAccessibilityEventInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);
            argv[PARAM0] = GetErrorValue(env, callbackInfo->result_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->result_) {
                    napi_resolve_deferred(env, callbackInfo->deferred_, argv[PARAM1]);
                } else {
                    napi_reject_deferred(env, callbackInfo->deferred_, argv[PARAM0]);
                }
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}