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

#include "napi_accessibility_window_info.h"

#include "hilog_wrapper.h"
#include "napi_accessibility_info.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

napi_value NAccessibilityWindowInfo::cons_ = nullptr;
napi_ref NAccessibilityWindowInfo::consRef_ = nullptr;

void NAccessibilityWindowInfo::DefineJSAccessibilityWindowInfo(napi_env env)
{
    napi_property_descriptor descForAccessibilityWindowInfo[] = {
        DECLARE_NAPI_FUNCTION("getAnchorElementInfo", NAccessibilityWindowInfo::GetAnchorElementInfo),
        DECLARE_NAPI_FUNCTION("getRootElementInfo", NAccessibilityWindowInfo::GetRootElementInfo),
        DECLARE_NAPI_FUNCTION("getParent", NAccessibilityWindowInfo::GetParent),
        DECLARE_NAPI_FUNCTION("getChild", NAccessibilityWindowInfo::GetChild),
    };

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "AccessibilityWindowInfo",
            NAPI_AUTO_LENGTH,
            NAccessibilityWindowInfo::JSConstructor,
            nullptr,
            sizeof(descForAccessibilityWindowInfo) / sizeof(descForAccessibilityWindowInfo[0]),
            descForAccessibilityWindowInfo,
            &NAccessibilityWindowInfo::cons_));
    napi_create_reference(env, NAccessibilityWindowInfo::cons_, 1, &NAccessibilityWindowInfo::consRef_);
}

napi_value NAccessibilityWindowInfo::JSConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value NAccessibilityWindowInfo::GetAnchorElementInfo(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG("GetAnchorElementInfo called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityWindowInfo* windowInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    status = napi_unwrap(env, thisVar, (void**)&windowInfo);
    HILOG_DEBUG("napi_unwrap status: %{public}d", (int)status);
    if (!windowInfo) {
        HILOG_DEBUG("windowInfo is null!!");
    }

    NAccessibilityWindowInfoData *callbackInfo = new NAccessibilityWindowInfoData();
    callbackInfo->nativeWindowInfo_ = *windowInfo;

    napi_value promise = nullptr;
    if (argc > 0) {
        HILOG_ERROR("GetAnchorElementInfo callback mode");
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_ERROR("GetAnchorElementInfo promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAnchorElementInfo", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityWindowInfoData *callbackInfo = (NAccessibilityWindowInfoData*)data;
            AccessibilityWindowInfo windowInfo = callbackInfo->nativeWindowInfo_;
            callbackInfo->result_ = windowInfo.GetAnchor(callbackInfo->nodeInfo_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetAnchorElementInfo execute back");
            NAccessibilityWindowInfoData* callbackInfo = (NAccessibilityWindowInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->result_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
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

napi_value NAccessibilityWindowInfo::GetRootElementInfo(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG("GetRootElementInfo called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityWindowInfo* windowInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    status = napi_unwrap(env, thisVar, (void**)&windowInfo);
    HILOG_DEBUG("napi_unwrap status: %{public}d", (int)status);
    if (!windowInfo) {
        HILOG_DEBUG("windowInfo is null!!");
    }
    //todo
    NAccessibilityWindowInfoData *callbackInfo = new NAccessibilityWindowInfoData();
    callbackInfo->nativeWindowInfo_ = *windowInfo;

    napi_value promise = nullptr;
    if (argc > 0) {
        HILOG_ERROR("GetRootElementInfo callback mode");
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_ERROR("GetRootElementInfo promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetRootElementInfo", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityWindowInfoData *callbackInfo = (NAccessibilityWindowInfoData*)data;
            AccessibilityWindowInfo windowInfo = callbackInfo->nativeWindowInfo_;
            callbackInfo->result_ = windowInfo.GetRootAccessibilityInfo(callbackInfo->nodeInfo_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetRootElementInfo execute back");
            NAccessibilityWindowInfoData* callbackInfo = (NAccessibilityWindowInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->result_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
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

napi_value NAccessibilityWindowInfo::GetParent(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG("GetParent called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityWindowInfo* windowInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    status = napi_unwrap(env, thisVar, (void**)&windowInfo);
    HILOG_DEBUG("napi_unwrap status: %{public}d", (int)status);
    if (!windowInfo) {
        HILOG_DEBUG("windowInfo is null!!");
    }

    NAccessibilityWindowInfoData *callbackInfo = new NAccessibilityWindowInfoData();
    callbackInfo->nativeWindowInfo_ = *windowInfo;

    napi_value promise = nullptr;
    if (argc > 0) {
        HILOG_ERROR("GetParent callback mode");
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_ERROR("GetParent promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetParent", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityWindowInfoData *callbackInfo = (NAccessibilityWindowInfoData*)data;
            AccessibilityWindowInfo windowInfo = callbackInfo->nativeWindowInfo_;
            callbackInfo->window_ = windowInfo.GetParent();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetParent execute back");
            NAccessibilityWindowInfoData* callbackInfo = (NAccessibilityWindowInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NAccessibilityWindowInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertAccessibilityWindowInfoToJS(env, argv[PARAM1], callbackInfo->window_);

            if (callbackInfo->callback_) {
                // Callback mode
                argv[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, callbackInfo->deferred_, argv[PARAM1]);
            }

            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);

    return promise;
}

napi_value NAccessibilityWindowInfo::GetChild(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG("GetChild called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityWindowInfo* windowInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    status = napi_unwrap(env, thisVar, (void**)&windowInfo);
    HILOG_DEBUG("napi_unwrap status: %{public}d", (int)status);
    if (!windowInfo) {
        HILOG_DEBUG("windowInfo is null!!");
    }
    int childIndex;
    napi_get_value_int32(env, argv[PARAM0], &childIndex);
    HILOG_INFO("GetChild childIndex = %{public}d", childIndex);

    NAccessibilityWindowInfoData *callbackInfo = new NAccessibilityWindowInfoData();
    callbackInfo->nativeWindowInfo_ = *windowInfo;
    callbackInfo->childIndex_ = childIndex;

    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_TWO) {
        HILOG_ERROR("GetChild callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_ERROR("GetChild promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetChild", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityWindowInfoData *callbackInfo = (NAccessibilityWindowInfoData*)data;
            AccessibilityWindowInfo windowInfo = callbackInfo->nativeWindowInfo_;
            callbackInfo->window_ = windowInfo.GetChild(callbackInfo->childIndex_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetChild execute back");
            NAccessibilityWindowInfoData* callbackInfo = (NAccessibilityWindowInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NAccessibilityWindowInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertAccessibilityWindowInfoToJS(env, argv[PARAM1], callbackInfo->window_);

            if (callbackInfo->callback_) {
                // Callback mode
                argv[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, callbackInfo->deferred_, argv[PARAM1]);
            }

            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);

    return promise;
}