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

#include "napi_accessibility_info.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

napi_value NElementInfo::cons_ = nullptr;
napi_ref NElementInfo::consRef_ = nullptr;

void NElementInfo::DefineJSElementInfo(napi_env env)
{
    napi_property_descriptor descForElementInfo[] = {
        DECLARE_NAPI_FUNCTION("executeAction", NElementInfo::ExecuteAction),
        DECLARE_NAPI_FUNCTION("getByContent", NElementInfo::GetByContent),
        DECLARE_NAPI_FUNCTION("get", NElementInfo::GetFocus),
        DECLARE_NAPI_FUNCTION("next", NElementInfo::GetNext),
        DECLARE_NAPI_FUNCTION("getChild", NElementInfo::GetChild),
        DECLARE_NAPI_FUNCTION("getParent", NElementInfo::GetParent),
    };

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "AccessibilityElementInfo",
            NAPI_AUTO_LENGTH,
            NElementInfo::JSConstructor,
            nullptr,
            sizeof(descForElementInfo) / sizeof(descForElementInfo[0]),
            descForElementInfo,
            &NElementInfo::cons_));
    napi_create_reference(env, NElementInfo::cons_, 1, &NElementInfo::consRef_);
}

napi_value NElementInfo::JSConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value NElementInfo::ExecuteAction(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    std::string action;
    ParseString(env, action, argv[PARAM0]);
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    HILOG_INFO("action = %{public}s", action.c_str());
    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }
    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    ConvertActionArgsJSToNAPI(env, argv[argc - 1], callbackInfo->actionArguments_,
        ConvertStringToAccessibleOperationType(action));
    callbackInfo->nativeNodeInfo_ = *nodeInfo;
    callbackInfo->content_ = action;
    napi_value promise = nullptr;

    // Parse function
    napi_valuetype valueType = napi_undefined;
    if (argc >= ARGS_SIZE_TWO) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    }
    if (valueType == napi_function) {
        HILOG_ERROR("ExecuteAction callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_ERROR("ExecuteAction promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ExecuteAction", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            ActionType action = ConvertStringToAccessibleOperationType(callbackInfo->content_);
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            callbackInfo->result_ = nodeInfo.ExecuteAction(action, callbackInfo->actionArguments_);
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("ExecuteAction execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->result_, &argv[PARAM1]);
            HILOG_DEBUG("callbackInfo->result_[%{public}d]", callbackInfo->result_);

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

napi_value NElementInfo::GetByContent(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    std::string content;
    ParseString(env, content, argv[PARAM0]);
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    HILOG_INFO("content[%{public}s]", content.c_str());

    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }

    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    callbackInfo->nativeNodeInfo_ = *nodeInfo;
    callbackInfo->content_ = content;
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetByContent", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            callbackInfo->ret_ = nodeInfo.GetByContent(callbackInfo->content_, callbackInfo->nodeInfos_);
        },
        [](napi_env env, napi_status status, void* data) {   // execute the complete function
            HILOG_DEBUG("execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_create_array(env, &argv[PARAM1]);
            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            ConvertElementInfosToJS(env, argv[PARAM1], callbackInfo->nodeInfos_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->ret_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                HILOG_DEBUG("callback mode");
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                HILOG_DEBUG("Promise mode");
                if (callbackInfo->ret_) {
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

napi_value NElementInfo::GetFocus(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    std::string focusType;
    ParseString(env, focusType, argv[PARAM0]);
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    HILOG_INFO("focusType[%{public}s]", focusType.c_str());

    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }

    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    callbackInfo->nativeNodeInfo_ = *nodeInfo;
    callbackInfo->content_ = focusType;
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetFocus", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            int focus = 0;
            if (!std::strcmp(callbackInfo->content_.c_str(), "accessibility")) {
                focus = FOCUS_TYPE_ACCESSIBILITY;
            } else if (!std::strcmp(callbackInfo->content_.c_str(), "normal")) {
                focus = FOCUS_TYPE_INPUT;
            } else {
                focus = FOCUS_TYPE_INVALID;
            }
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            callbackInfo->ret_ = nodeInfo.GetFocus(focus, callbackInfo->nodeInfo_);
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->ret_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                if (callbackInfo->ret_) {
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

static FocusMoveDirection CovertStringToDirection(std::string str)
{
    static const std::map<std::string, FocusMoveDirection> focusMoveDirectionTable = {
        {"up", FocusMoveDirection::UP},
        {"down", FocusMoveDirection::DOWN},
        {"left", FocusMoveDirection::LEFT},
        {"right", FocusMoveDirection::RIGHT},
        {"forward", FocusMoveDirection::FORWARD},
        {"backward", FocusMoveDirection::BACKWARD}
    };

    if (focusMoveDirectionTable.find(str) == focusMoveDirectionTable.end()) {
        return FocusMoveDirection::DIRECTION_INVALID;
    }

    return focusMoveDirectionTable.at(str);
}

napi_value NElementInfo::GetNext(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;
    bool ret = true;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    std::string direction;
    if (!ParseString(env, direction, argv[PARAM0]) || direction == "") {
        ret = false;
    }
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    HILOG_INFO("direction[%{public}s]", direction.c_str());

    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }

    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    callbackInfo->nativeNodeInfo_ = *nodeInfo;
    callbackInfo->content_ = direction;
    callbackInfo->ret_ = ret;
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetNext callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetNext promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetNext", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            if (callbackInfo->ret_) {
                callbackInfo->ret_ = nodeInfo.GetNext(CovertStringToDirection(callbackInfo->content_),
                    callbackInfo->nodeInfo_);
            }
        },
        [](napi_env env, napi_status status, void* data) {  // execute the complete function
            HILOG_DEBUG("GetNext execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->ret_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                if (callbackInfo->ret_) {
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

napi_value NElementInfo::GetChild(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);
    int childIndex;
    napi_get_value_int32(env, argv[PARAM0], &childIndex);
    HILOG_INFO("childIndex[%{public}d]", childIndex);

    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }

    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    callbackInfo->nativeNodeInfo_ = *nodeInfo;
    callbackInfo->childIndex_ = childIndex;
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetChild callback mode");
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetChild promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetChild", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            callbackInfo->ret_ = nodeInfo.GetChild(callbackInfo->childIndex_, callbackInfo->nodeInfo_);
        },
        [](napi_env env, napi_status status, void* data) {  // execute the complete function
            HILOG_DEBUG("GetChild execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->ret_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                if (callbackInfo->ret_) {
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

napi_value NElementInfo::GetParent(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_status status;
    napi_value thisVar;
    void* data = nullptr;
    AccessibilityElementInfo* nodeInfo = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOG_DEBUG("argc = %{public}d", (int)argc);

    status = napi_unwrap(env, thisVar, (void**)&nodeInfo);
    if (!nodeInfo) {
        HILOG_ERROR("nodeInfo is null!!");
    }

    NAccessibilityInfoData *callbackInfo = new NAccessibilityInfoData();
    callbackInfo->nativeNodeInfo_ = *nodeInfo;

    napi_value promise = nullptr;
    if (argc > 0) {
        HILOG_DEBUG("GetParent callback mode");
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetParent promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetParent", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilityInfoData *callbackInfo = (NAccessibilityInfoData*)data;
            AccessibilityElementInfo nodeInfo = callbackInfo->nativeNodeInfo_;
            callbackInfo->ret_ = nodeInfo.GetParent(callbackInfo->nodeInfo_);
        },
        [](napi_env env, napi_status status, void* data) {  // execute the complete function
            HILOG_DEBUG("GetParent execute back");
            NAccessibilityInfoData* callbackInfo = (NAccessibilityInfoData*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_reference_value(env, NElementInfo::consRef_, &NElementInfo::cons_);
            napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
            ConvertElementInfoToJS(env, argv[PARAM1], callbackInfo->nodeInfo_);

            argv[PARAM0] = GetErrorValue(env, callbackInfo->ret_ ? CODE_SUCCESS : CODE_FAILED);
            if (callbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                // Promise mode
                if (callbackInfo->ret_) {
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