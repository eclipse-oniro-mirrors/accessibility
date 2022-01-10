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

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "accessible_ability.h"
#include "accessibility_element_info.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_info.h"
#include "napi_accessible_ability.h"
#include "napi_accessible_ability_observer.h"
#include "napi_accessibility_def.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_window_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

napi_value g_accessibleAbility;
std::map<std::string, std::shared_ptr<AccessibleAbilityCallbackInfo>> g_JsAACallbacks = {};
std::shared_ptr<AccessibleAbilityListener> g_AAObserver = std::make_shared<AccessibleAbilityObserver>();

void CreateGlobalAccessibleAbilityJSObject(napi_env env) {
    napi_property_descriptor accessibleAbilityDesc[] = {
        DECLARE_NAPI_FUNCTION("on", RegisterCallback),
        DECLARE_NAPI_FUNCTION("connect", ConnectToAAMS),
        DECLARE_NAPI_FUNCTION("disableAbility", DisableAbility),
        DECLARE_NAPI_FUNCTION("getWindows", GetWindows),
        DECLARE_NAPI_FUNCTION("getRootElementInfo", GetRootElementInfo),
        DECLARE_NAPI_FUNCTION("getFocusElementInfo", GetFocusElementInfo),
        DECLARE_NAPI_FUNCTION("performCommonAction", PerformCommonAction),
    };

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "AccessibleAbility",
            NAPI_AUTO_LENGTH,
            AccessibleAbilityConstructor,
            nullptr,
            sizeof(accessibleAbilityDesc) / sizeof(accessibleAbilityDesc[0]),
            accessibleAbilityDesc,
            &g_accessibleAbility));
}

napi_value AccessibleAbilityConstructor(napi_env env, napi_callback_info info) {
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value GetAccessibleAbility(napi_env env, napi_callback_info info) {
    HILOG_INFO("GetAccessibleAbility called");
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv = 0;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= expectedArgsCount, "Requires up to 1 argument.");

    AsyncGetAccessibleAbilityCallbackInfo* pCallbackInfo = new AsyncGetAccessibleAbilityCallbackInfo();
    pCallbackInfo->env_ = env;

    napi_value promise = nullptr;

    if (argc > (expectedArgsCount - CALLBACK_SIZE)) {
        // Callback mode
        HILOG_INFO("GetAccessibleAbility callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv, &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv, 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("GetAccessibleAbility promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAccessibleAbility", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            AsyncGetAccessibleAbilityCallbackInfo* pCallbackInfo = (AsyncGetAccessibleAbilityCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_new_instance(env, g_accessibleAbility, 0, nullptr, &result[PARAM1]);

            if (pCallbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, pCallbackInfo->deferred_, result[PARAM1]);
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);
    napi_queue_async_work(env, pCallbackInfo->asyncWork_);

    return promise;
}

napi_value RegisterCallback(napi_env env, napi_callback_info info) {
    HILOG_INFO("RegisterCallback called");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 2 arguments.");
    std::string type;
    ParseString(env, type, argv[PARAM0]);
    std::shared_ptr<AccessibleAbilityCallbackInfo> pCallbackInfo = std::make_shared<AccessibleAbilityCallbackInfo>();
    pCallbackInfo->env_ = env;

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType);
    NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
    NAPI_CALL(env, napi_create_reference(env, argv[PARAM1], 1, &pCallbackInfo->callback_));
    g_JsAACallbacks[type] = pCallbackInfo;
    
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value DisableAbility(napi_env env, napi_callback_info info) {
    HILOG_INFO("DisableAbility called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= ARGS_SIZE_ONE, "Requires up to 1 argument.");

    AsyncDisableAbilityCallbackInfo* pCallbackInfo = new AsyncDisableAbilityCallbackInfo();
    pCallbackInfo->env_ = env;

    napi_value promise = nullptr;

    if (argc == ARGS_SIZE_ONE) {
        // Callback mode
        HILOG_INFO("DisableAbility callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv, &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv, 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("DisableAbility promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "DisableAbility", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOG_DEBUG("DisableAbility execute");
            AccessibleAbility::GetInstance().DisableAbility();
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("DisableAbility execute back");
            AsyncDisableAbilityCallbackInfo* pCallbackInfo = (AsyncDisableAbilityCallbackInfo*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_undefined(env, &argv[PARAM1]);

            if (pCallbackInfo->callback_) {
                // Callback mode
                argv[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, pCallbackInfo->deferred_, argv[PARAM1]);
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);

    napi_queue_async_work(env, pCallbackInfo->asyncWork_);
    return promise;
}

napi_value GetWindows(napi_env env, napi_callback_info info) {
    HILOG_INFO("GetWindows called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= ARGS_SIZE_ONE, "Requires up to 1 argument.");

    AsyncGetWindowsCallbackInfo* pCallbackInfo = new AsyncGetWindowsCallbackInfo();
    pCallbackInfo->env_ = env;

    napi_value promise = nullptr;

    if (argc == ARGS_SIZE_ONE) {
        // Callback mode
        HILOG_INFO("GetWindows callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv, &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv, 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("GetWindows promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetWindows", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOG_DEBUG("GetWindows execute");
            AsyncGetWindowsCallbackInfo* pCallbackInfo = (AsyncGetWindowsCallbackInfo*)data;
            pCallbackInfo->accessibilityWindows_ = Accessibility::AccessibleAbility::GetInstance().GetWindows();
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetWindows execute back");
            AsyncGetWindowsCallbackInfo* pCallbackInfo = (AsyncGetWindowsCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            if (!pCallbackInfo->accessibilityWindows_.empty()) {
                HILOG_DEBUG("GetWindows accessibilityWindows is not null");
                napi_create_array(env, &result[PARAM1]);
                ConvertAccessibilityWindowInfosToJS(env, result[PARAM1], pCallbackInfo->accessibilityWindows_);
            } else {
                HILOG_DEBUG("GetWindows accessibilityWindows is null");
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (pCallbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, pCallbackInfo->deferred_, result[PARAM1]);
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);

    napi_queue_async_work(env, pCallbackInfo->asyncWork_);
    return promise;
}

napi_value GetRootElementInfo(napi_env env, napi_callback_info info) {
    HILOG_INFO("GetRootElementInfo called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= ARGS_SIZE_ONE, "Requires up to 1 argument.");

    AsyncGetRootElementInfoCallbackInfo* pCallbackInfo = new AsyncGetRootElementInfoCallbackInfo();
    pCallbackInfo->env_ = env;

    napi_value promise = nullptr;

    if (argc == ARGS_SIZE_ONE) {
        // Callback mode
        HILOG_INFO("GetRootElementInfo callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv, &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv, 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("GetRootElementInfo promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetRootElementInfo", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOG_DEBUG("GetRootElementInfo execute");
            AsyncGetRootElementInfoCallbackInfo* pCallbackInfo = (AsyncGetRootElementInfoCallbackInfo*)data;
            pCallbackInfo->result_ = AccessibleAbility::GetInstance().GetRootElementInfo(pCallbackInfo->elementInfo_);
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetRootElementInfo execute back");
            AsyncGetRootElementInfoCallbackInfo* pCallbackInfo = (AsyncGetRootElementInfoCallbackInfo*)data;
            napi_value jsReturnValue = 0;
            napi_value argv[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            if (pCallbackInfo->elementInfo_.has_value()) {
                HILOG_DEBUG("GetRootElementInfo elementInfo is not null");
                napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &argv[PARAM1]);
                ConvertElementInfoToJS(env, argv[PARAM1], *(pCallbackInfo->elementInfo_));
            } else {
                HILOG_DEBUG("GetRootElementInfo elementInfo is null");
                napi_get_undefined(env, &argv[PARAM1]);
            }

            argv[PARAM0] = GetErrorValue(env, pCallbackInfo->result_ ? CODE_SUCCESS : CODE_FAILED);
            if (pCallbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, argv, &jsReturnValue);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                if (pCallbackInfo->result_) {
                    napi_resolve_deferred(env, pCallbackInfo->deferred_, argv[PARAM1]);
                } else {
                    napi_reject_deferred(env, pCallbackInfo->deferred_, argv[PARAM0]);
                }
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);

    napi_queue_async_work(env, pCallbackInfo->asyncWork_);
    return promise;
}

napi_value ConnectToAAMS(napi_env env, napi_callback_info info) {
    HILOG_INFO("ConnectToAAMS called");

    AccessibleAbility& aa = AccessibleAbility::GetInstance();
    aa.RegisterListener(g_AAObserver);
    aa.ConnectToAAMS();

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value GetFocusElementInfo(napi_env env, napi_callback_info info) {
    HILOG_INFO("GetFocusElementInfo called");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= expectedArgsCount, "Requires at most 2 arguments.");
    std::string focusType;
    ParseString(env, focusType, argv[PARAM0]);
    HILOG_INFO("GetFocusElementInfo focusType = %{public}s", focusType.c_str());

    AsyncGetFocusElementInfoCallbackInfo* pCallbackInfo = new AsyncGetFocusElementInfoCallbackInfo();
    pCallbackInfo->env_ = env;
    pCallbackInfo->focusType_ = focusType;

    napi_value promise = nullptr;

    if (argc > (expectedArgsCount - CALLBACK_SIZE)) {
        // Callback mode
        HILOG_INFO("GetFocusElementInfo callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("GetFocusElementInfo promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetFocusElementInfo", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOG_DEBUG("GetFocusElementInfo execute");
            AsyncGetFocusElementInfoCallbackInfo* pCallbackInfo = (AsyncGetFocusElementInfoCallbackInfo*)data;
            uint32_t focus = 0;
            if (std::strcmp(pCallbackInfo->focusType_.c_str(), "accessibility") == 0) {
                focus = FOCUS_TYPE_ACCESSIBILITY;
            } else if (std::strcmp(pCallbackInfo->focusType_.c_str(), "normal") == 0) {
                focus = FOCUS_TYPE_INPUT;
            } else {
                focus = FOCUS_TYPE_INVALID;
            }
            pCallbackInfo->result_ = AccessibleAbility::GetInstance().GetFocusElementInfo(
                                        focus, pCallbackInfo->elementInfo_);
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("GetFocusElementInfo execute back");
            AsyncGetFocusElementInfoCallbackInfo* pCallbackInfo = (AsyncGetFocusElementInfoCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            if (pCallbackInfo->elementInfo_.has_value()) {
                HILOG_DEBUG("GetFocusElementInfo elementInfo is not null");
                napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &result[PARAM1]);
                ConvertElementInfoToJS(env, result[PARAM1], *(pCallbackInfo->elementInfo_));
            } else {
                HILOG_DEBUG("GetFocusElementInfo elementInfo is null");
                napi_get_undefined(env, &result[PARAM1]);
            }

            result[PARAM0] = GetErrorValue(env, pCallbackInfo->result_ ? CODE_SUCCESS : CODE_FAILED);
            if (pCallbackInfo->callback_) {
                // Callback mode
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                if (pCallbackInfo->result_) {
                    napi_resolve_deferred(env, pCallbackInfo->deferred_, result[PARAM1]);
                } else {
                    napi_reject_deferred(env, pCallbackInfo->deferred_, result[PARAM0]);
                }
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);

    napi_queue_async_work(env, pCallbackInfo->asyncWork_);
    return promise;
}

napi_value PerformCommonAction(napi_env env, napi_callback_info info) {
    HILOG_INFO("PerformCommonAction called");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc <= expectedArgsCount, "Requires at most 2 arguments.");
    std::string action;
    ParseString(env, action, argv[PARAM0]);
    HILOG_INFO("PerformCommonAction action = %{public}s", action.c_str());

    AsyncPerformCommonActionCallbackInfo* pCallbackInfo = new AsyncPerformCommonActionCallbackInfo();
    pCallbackInfo->env_ = env;
    pCallbackInfo->action_ = ConvertStringToGlobalAction(action);

    napi_value promise = nullptr;

    if (argc > (expectedArgsCount - CALLBACK_SIZE)) {
        // Callback mode
        HILOG_INFO("PerformCommonAction callback mode");
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
        NAPI_ASSERT(env, valueType == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &pCallbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOG_INFO("PerformCommonAction promise mode");
        napi_create_promise(env, &pCallbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "PerformCommonAction", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOG_DEBUG("PerformCommonAction execute");
            AsyncPerformCommonActionCallbackInfo* pCallbackInfo = (AsyncPerformCommonActionCallbackInfo*)data;
            pCallbackInfo->performActionResult_ = AccessibleAbility::GetInstance().PerformCommonAction(pCallbackInfo->action_);
            HILOG_DEBUG("PerformCommonAction result is %{public}d!", pCallbackInfo->performActionResult_);
        },
        [](napi_env env, napi_status status, void* data) {
            HILOG_DEBUG("PerformCommonAction execute back");
            AsyncPerformCommonActionCallbackInfo* pCallbackInfo = (AsyncPerformCommonActionCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, pCallbackInfo->performActionResult_, &result[PARAM1]);
            HILOG_DEBUG("PerformCommonAction result is %{public}d!", pCallbackInfo->performActionResult_);

            if (pCallbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, pCallbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, pCallbackInfo->callback_);
            } else {
                // Promise mode
                napi_resolve_deferred(env, pCallbackInfo->deferred_, result[PARAM1]);
            }

            napi_delete_async_work(env, pCallbackInfo->asyncWork_);
            delete pCallbackInfo;
        },
        (void*)pCallbackInfo,
        &pCallbackInfo->asyncWork_);

    napi_queue_async_work(env, pCallbackInfo->asyncWork_);
    return promise;
}