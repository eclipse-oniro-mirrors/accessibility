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

#include "napi_accessibility_extension_context.h"

#include <uv.h>
#include "display_manager.h"
#include "js_extension_context.h"
#include "js_runtime_utils.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_element.h"
#include "accessibility_utils.h"

using namespace OHOS::AbilityRuntime;
using namespace OHOS::AccessibilityNapi;

namespace OHOS {
namespace Accessibility {
namespace {
static void ConvertAccessibilityWindowInfoToJS(
    napi_env env, napi_value result, const AccessibilityWindowInfo& accessibilityWindowInfo)
{
    // Bind js object to a Native object
    std::shared_ptr<AccessibilityWindowInfo> windowInfo =
        std::make_shared<AccessibilityWindowInfo>(accessibilityWindowInfo);
    AccessibilityElement* pAccessibilityElement = new(std::nothrow) AccessibilityElement(windowInfo);
    if (!pAccessibilityElement) {
        HILOG_ERROR("Failed to create work.");
        return;
    }

    napi_status sts = napi_wrap(
        env,
        result,
        pAccessibilityElement,
        [](napi_env env, void* data, void* hint) {
            AccessibilityElement* info = static_cast<AccessibilityElement*>(data);
            delete info;
            info = nullptr;
        },
        nullptr,
        nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
}

static void ConvertAccessibilityWindowInfosToJS(
    napi_env env, napi_value result, const std::vector<AccessibilityWindowInfo>& accessibilityWindowInfos)
{
    HILOG_DEBUG();
    size_t idx = 0;

    if (accessibilityWindowInfos.empty()) {
        return;
    }
    napi_value constructor = nullptr;
    napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);

    for (const auto& windowInfo : accessibilityWindowInfos) {
        napi_value obj = nullptr;
        napi_new_instance(env, constructor, 0, nullptr, &obj);
        ConvertAccessibilityWindowInfoToJS(env, obj, windowInfo);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

static bool IsNapiFunction(napi_env env, napi_value param)
{
    napi_valuetype valueType = napi_null;
    napi_status status = napi_typeof(env, param, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valueType != napi_function) {
        HILOG_ERROR("SubscribeState args[PARAM1] format is wrong");
        return false;
    }
    return true;
}

static bool IsNapiBool(napi_env env, napi_value param)
{
    napi_valuetype valuetype = napi_null;
    napi_status status = napi_typeof(env, param, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_boolean) {
        HILOG_ERROR("Wrong argument type. Boolean expected.");
        return false;
    }
    return true;
}

static bool IsNapiNumber(napi_env env, napi_value param)
{
    napi_valuetype valuetype;
    napi_status status = napi_typeof(env, param, &valuetype);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof error and status is %{public}d", status);
        return false;
    }

    if (valuetype != napi_number) {
        HILOG_ERROR("Wrong argument type. uint32 expected.");
        return false;
    }
    return true;
}

static void GetLastParamForTwo(napi_env env, NapiCallbackInfo& info, napi_value& lastParam,
    bool& isAccessibilityFocus)
{
    if (info.argv[PARAM0] != nullptr && info.argv[PARAM1] != nullptr &&
        IsNapiBool(env, info.argv[PARAM0]) && IsNapiFunction(env, info.argv[PARAM1])) {
        lastParam = ConvertFromJsValue(env, info.argv[PARAM0], isAccessibilityFocus) ?
            info.argv[PARAM1] : nullptr;
    } else if (info.argv[PARAM1] != nullptr && IsNapiFunction(env, info.argv[PARAM1])) {
        HILOG_INFO("argc is more than two, use callback: situation 1");
        lastParam = info.argv[PARAM1];
    } else if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
        HILOG_INFO("argc is more than two, use callback: situation 2");
        lastParam = info.argv[PARAM0];
    } else if (info.argv[PARAM0] != nullptr && IsNapiBool(env, info.argv[PARAM0])) {
        HILOG_INFO("argc is more than two, use promise: situation 3");
        lastParam = nullptr;
        ConvertFromJsValue(env, info.argv[PARAM0], isAccessibilityFocus);
    } else {
        lastParam = nullptr;
        HILOG_INFO("argc is more than two, use promise");
    }
}

class NAccessibilityExtensionContext final {
public:
    explicit NAccessibilityExtensionContext(
        const std::shared_ptr<AccessibilityExtensionContext>& context) : context_(context) {}
    ~NAccessibilityExtensionContext() = default;

    static void Finalizer(napi_env env, void* data, void* hint)
    {
        HILOG_INFO();
        std::unique_ptr<NAccessibilityExtensionContext>(static_cast<NAccessibilityExtensionContext*>(data));
    }

    static napi_value SetTargetBundleName(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnSetTargetBundleName);
    }

    static napi_value GetFocusElement(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnGetFocusElement);
    }

    static napi_value GetWindowRootElement(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnGetWindowRootElement);
    }

    static napi_value GetWindows(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnGetWindows);
    }

    static napi_value InjectGesture(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnGestureInject);
    }

    static napi_value InjectGestureSync(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, NAccessibilityExtensionContext, OnGestureInjectSync);
    }

private:
    std::weak_ptr<AccessibilityExtensionContext> context_;

    napi_value OnSetTargetBundleName(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();
        NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
        if (info.argc < ARGS_SIZE_ONE) {
            HILOG_ERROR("Not enough params");
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }

        std::vector<std::string> targetBundleNames;
        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_OK) {
            if (ConvertJSToStringVec(env, info.argv[PARAM0], targetBundleNames)) {
                HILOG_INFO("targetBundleNames's size = %{public}zu", targetBundleNames.size());
            } else {
                errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
            }
        }

        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM) {
            HILOG_ERROR("invalid param");
            napi_throw(env, CreateJsError(env,
                static_cast<int32_t>(NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM),
                ERROR_MESSAGE_PARAMETER_ERROR));
            return CreateJsUndefined(env);
        }

        return SetTargetBundleNameCompleteTask(env, targetBundleNames, info);
    }

    napi_value SetTargetBundleNameCompleteTask(napi_env env, std::vector<std::string> targetBundleNames,
        NapiCallbackInfo& info)
        {
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, targetBundleNames, ret] () {
            HILOG_INFO("SetTargetBundleName begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }

            *ret = context->SetTargetBundleName(targetBundleNames);
        };
        NapiAsyncTask::CompleteCallback complete =
            [ret](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*ret == RET_OK) {
                task.Resolve(env, CreateJsUndefined(env));
            } else {
                HILOG_ERROR("set target bundle name failed. ret: %{public}d.", *ret);
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY),
                    ERROR_MESSAGE_SYSTEM_ABNORMALITY));
            }
        };

        napi_value lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr :
            ((info.argv[PARAM1] != nullptr && IsNapiFunction(env, info.argv[PARAM1])) ? info.argv[PARAM1] : nullptr);
        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::OnSetTargetBundleName",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value OnGetFocusElement(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();
        bool isAccessibilityFocus = false;
        napi_value lastParam = nullptr;
        if (info.argc >= ARGS_SIZE_TWO) {
            GetLastParamForTwo(env, info, lastParam, isAccessibilityFocus);
        } else if (info.argc == ARGS_SIZE_ONE) {
            if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
                lastParam = info.argv[PARAM0];
            } else {
                if (info.argv[PARAM0] != nullptr && IsNapiBool(env, info.argv[PARAM0])) {
                    ConvertFromJsValue(env, info.argv[PARAM0], isAccessibilityFocus);
                }
                lastParam = nullptr;
                HILOG_INFO("argc is one, use promise");
            }
        } else {
            lastParam = nullptr;
            HILOG_INFO("argc is others, use promise");
        }

        int32_t focus = isAccessibilityFocus ? FOCUS_TYPE_ACCESSIBILITY : FOCUS_TYPE_INPUT;
        HILOG_DEBUG("focus type is [%{public}d]", focus);
        return GetFoucusElementCompleteTask(env, focus, lastParam);
    }

    napi_value GetFoucusElementCompleteTask(napi_env env, int32_t focus, napi_value lastParam)
    {
        auto elementInfo = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, elementInfo, focus, ret] () {
            HILOG_INFO("GetFoucusElement begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }

            *ret = context->GetFocus(focus, *elementInfo);
        };
        NapiAsyncTask::CompleteCallback complete =
            [ret, elementInfo](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*ret == RET_OK) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_value napiElementInfo = nullptr;
                napi_new_instance(env, constructor, 0, nullptr, &napiElementInfo);
                NAccessibilityElement::ConvertElementInfoToJS(env, napiElementInfo, *elementInfo);
                task.Resolve(env, napiElementInfo);
            } else {
                HILOG_ERROR("Get focus elementInfo failed. ret: %{public}d", *ret);
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).errCode),
                    ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).message));
            }
        };

        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::OnGetFocusElement",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value OnGetWindowRootElement(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();
        int32_t windowId = INVALID_WINDOW_ID;
        bool isActiveWindow = true;
        napi_value lastParam = nullptr;
        if (info.argc >= ARGS_SIZE_TWO) {
            if (info.argv[PARAM1] != nullptr && IsNapiFunction(env, info.argv[PARAM1])) {
                HILOG_INFO("argc is more than two, use callback: situation 1");
                lastParam = info.argv[PARAM1];
            } else if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
                HILOG_INFO("argc is more than two, use callback: situation 2");
                lastParam = info.argv[PARAM0];
            } else {
                lastParam = nullptr;
                HILOG_INFO("argc is two, use promise");
            }
            if (info.argv[PARAM0] != nullptr && IsNapiNumber(env, info.argv[PARAM0])) {
                HILOG_INFO("argc is more than two, use promise: situation 3");
                isActiveWindow = !ConvertFromJsValue(env, info.argv[PARAM0], windowId);
            }
        } else if (info.argc == ARGS_SIZE_ONE) {
            if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
                lastParam = info.argv[PARAM0];
            } else if (info.argv[PARAM0] != nullptr && IsNapiNumber(env, info.argv[PARAM0])) {
                isActiveWindow = !ConvertFromJsValue(env, info.argv[PARAM0], windowId);
                lastParam = nullptr;
                HILOG_INFO("argc is one, use promise");
            }
        } else {
            lastParam = nullptr;
            HILOG_INFO("argc is others, use promise");
        }
        return GetWindowRootElementCompleteTask(env, windowId, isActiveWindow, lastParam);
    }

    napi_value GetWindowRootElementCompleteTask(
        napi_env env, int32_t windowId, bool isActiveWindow, napi_value lastParam)
    {
        auto elementInfo = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, isActiveWindow, windowId, elementInfo, ret] () {
            HILOG_INFO("GetWindowRootElement begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }
            if (isActiveWindow) {
                *ret = context->GetRoot(*elementInfo);
            } else {
                AccessibilityWindowInfo windowInfo;
                windowInfo.SetWindowId(windowId);
                *ret = context->GetRootByWindow(windowInfo, *elementInfo);
            }
        };

        NapiAsyncTask::CompleteCallback complete =
            [ret, elementInfo](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*ret == RET_OK) {
                napi_value constructor = nullptr;
                napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
                napi_value napiElementInfo = nullptr;
                napi_status result = napi_new_instance(env, constructor, 0, nullptr, &napiElementInfo);
                HILOG_DEBUG("napi_new_instance result is %{public}d", result);
                NAccessibilityElement::ConvertElementInfoToJS(env, napiElementInfo, *elementInfo);
                task.Resolve(env, napiElementInfo);
            } else {
                HILOG_ERROR("Get root elementInfo failed. ret : %{public}d", *ret);
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).errCode),
                    ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).message));
            }
        };

        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::OnGetWindowRootElement",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value OnGetWindows(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();

        int64_t displayId = 0;
        bool hasDisplayId = false;
        napi_value lastParam = nullptr;
        if (info.argc >= ARGS_SIZE_TWO) {
            if (info.argv[PARAM1] != nullptr && IsNapiFunction(env, info.argv[PARAM1])) {
                HILOG_INFO("argc is more than two, use callback: situation 1");
                lastParam = info.argv[PARAM1];
            } else if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
                HILOG_INFO("argc is more than two, use callback: situation 2");
                lastParam = info.argv[PARAM0];
            } else {
                lastParam = nullptr;
            }
            if (info.argv[PARAM0] != nullptr && IsNapiNumber(env, info.argv[PARAM0])) {
                hasDisplayId = ConvertFromJsValue(env, info.argv[PARAM0], displayId);
                HILOG_INFO("argc is more than two, use promise: situation 3");
            }
        } else if (info.argc == ARGS_SIZE_ONE) {
            if (info.argv[PARAM0] != nullptr && IsNapiFunction(env, info.argv[PARAM0])) {
                lastParam = info.argv[PARAM0];
            } else if (info.argv[PARAM0] != nullptr && IsNapiNumber(env, info.argv[PARAM0])) {
                hasDisplayId = ConvertFromJsValue(env, info.argv[PARAM0], displayId);
                lastParam = nullptr;
                HILOG_INFO("argc is one, use promise");
            }
        } else {
            lastParam = nullptr;
            HILOG_INFO("argc is others, use promise");
        }

        return hasDisplayId ? GetWindowsByDisplayIdAsync(env, lastParam, displayId) :
            GetWindowsAsync(env, lastParam);
    }

    napi_value GetWindowsAsync(napi_env env, napi_value lastParam)
    {
        HILOG_INFO();
        auto accessibilityWindows = std::make_shared<std::vector<OHOS::Accessibility::AccessibilityWindowInfo>>();
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, accessibilityWindows, ret] () {
            HILOG_INFO("Getwindows begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }
            *ret = context->GetWindows(*accessibilityWindows);
        };
        
        NapiAsyncTask::CompleteCallback complete =
            [ret, accessibilityWindows] (napi_env env, NapiAsyncTask& task, int32_t status) {
                if (*ret == RET_OK) {
                    napi_value napiWindowInfos = nullptr;
                    napi_create_array(env, &napiWindowInfos);
                    ConvertAccessibilityWindowInfosToJS(env, napiWindowInfos, *accessibilityWindows);
                    task.Resolve(env, napiWindowInfos);
                } else {
                    HILOG_ERROR("Get windowInfos failed.");
                    task.Reject(env, CreateJsError(env,
                        static_cast<int32_t>(ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).errCode),
                        ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).message));
                }
        };

        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::GetWindowsAsync",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value GetWindowsByDisplayIdAsync(napi_env env, napi_value lastParam, int64_t displayId)
    {
        HILOG_INFO();
        auto accessibilityWindows = std::make_shared<std::vector<OHOS::Accessibility::AccessibilityWindowInfo>>();
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, accessibilityWindows, ret, displayId] () {
            HILOG_INFO("GetwindowsByDisplayId begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }
            if (displayId < 0) {
                HILOG_ERROR("displayId is error: %{public}" PRId64 "", displayId);
                *ret = RET_ERR_INVALID_PARAM;
                return;
            }
            *ret = context->GetWindows(static_cast<uint64_t>(displayId), *accessibilityWindows);
        };
        
        NapiAsyncTask::CompleteCallback complete =
            [ret, accessibilityWindows] (napi_env env, NapiAsyncTask& task, int32_t status) {
                if (*ret == RET_OK) {
                    napi_value napiWindowInfos = nullptr;
                    napi_create_array(env, &napiWindowInfos);
                    ConvertAccessibilityWindowInfosToJS(env, napiWindowInfos, *accessibilityWindows);
                    task.Resolve(env, napiWindowInfos);
                } else {
                    HILOG_ERROR("Get windowInfosByDisplayId failed.");
                    task.Reject(env, CreateJsError(env,
                        static_cast<int32_t>(ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).errCode),
                        ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).message));
                }
        };

        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::GetWindowsByDisplayIdAsync",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value OnGestureInjectSync(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();
        NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
        if (info.argc != ARGS_SIZE_TWO) {
            HILOG_ERROR("invalid param");
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }

        napi_value nGesturePaths = reinterpret_cast<napi_value>(info.argv[PARAM0]);
        std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
            std::make_shared<AccessibilityGestureInjectPath>();
        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_OK) {
            if (!ConvertGesturePathJSToNAPI(env, nGesturePaths, gesturePath)) {
                errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
                HILOG_ERROR("invalid param");
            }
        }

        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM) {
            napi_throw(env, CreateJsError(env,
                static_cast<int32_t>(NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM),
                ERROR_MESSAGE_PARAMETER_ERROR));
            return CreateJsUndefined(env);
        }

        auto context = context_.lock();
        RetError ret = context->InjectGesture(gesturePath);
        if (ret != RET_OK) {
            HILOG_ERROR("result error, ret %{public}d", ret);
            napi_throw(env, CreateJsError(env,
                static_cast<int32_t>(NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM),
                ERROR_MESSAGE_PARAMETER_ERROR));
            return CreateJsUndefined(env);
        }

        HILOG_DEBUG("OnGestureInjectSync success");
        return CreateJsUndefined(env);
    }

    napi_value OnGestureInject(napi_env env, NapiCallbackInfo& info)
    {
        HILOG_INFO();
        NAccessibilityErrorCode errCode = NAccessibilityErrorCode::ACCESSIBILITY_OK;
        if (info.argc < ARGS_SIZE_ONE) {
            HILOG_ERROR("Not enough params");
            errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
        }

        napi_value nGesturePaths = info.argv[PARAM0];
        std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
            std::make_shared<AccessibilityGestureInjectPath>();
        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_OK) {
            if (!ConvertGesturePathJSToNAPI(env, nGesturePaths, gesturePath)) {
                errCode = NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM;
            }
        }

        if (errCode == NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM) {
            HILOG_ERROR("invalid param");
            napi_throw(env, CreateJsError(env,
                static_cast<int32_t>(NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM),
                ERROR_MESSAGE_PARAMETER_ERROR));
            return CreateJsUndefined(env);
        }
        return GestureInjectCompleteTask(env, info, gesturePath);
    }

    napi_value GestureInjectCompleteTask(
        napi_env env, NapiCallbackInfo& info, std::shared_ptr<AccessibilityGestureInjectPath> gesturePath)
    {
        auto ret = std::make_shared<RetError>(RET_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, gesturePath, ret] () {
            HILOG_INFO("GestureInject begin");
            auto context = weak.lock();
            if (!context) {
                HILOG_ERROR("context is released");
                *ret = RET_ERR_FAILED;
                return;
            }

            *ret = context->InjectGesture(gesturePath);
        };
        NapiAsyncTask::CompleteCallback complete =
            [ret, gesturePath](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*ret == RET_OK) {
                task.Resolve(env, CreateJsUndefined(env));
            } else {
                HILOG_ERROR("Gesture inject failed. ret: %{public}d.", *ret);
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).errCode),
                    ACCESSIBILITY_JS_TO_ERROR_CODE_MAP.at(*ret).message));
            }
        };

        napi_value lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr :
            ((info.argv[PARAM1] != nullptr && IsNapiFunction(env, info.argv[PARAM1])) ? info.argv[PARAM1] : nullptr);
        napi_value result = nullptr;
        NapiAsyncTask::Schedule("NAccessibilityExtensionContext::OnGestureInject",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }
};
} // namespace

napi_value CreateJsAccessibilityExtensionContext(
    napi_env env, std::shared_ptr<AccessibilityExtensionContext> context)
{
    HILOG_INFO();
    napi_value object = CreateJsExtensionContext(env, context);
    std::unique_ptr<NAccessibilityExtensionContext> jsContext =
        std::make_unique<NAccessibilityExtensionContext>(context);
    if (!object) {
        HILOG_ERROR("object is nullptr.");
        return nullptr;
    }
    napi_wrap(env, object, jsContext.release(), NAccessibilityExtensionContext::Finalizer, nullptr, nullptr);
    const char *moduleName = "NAccessibilityExtensionContext";
    BindNativeFunction(env, object, "setTargetBundleName", moduleName,
        NAccessibilityExtensionContext::SetTargetBundleName);
    BindNativeFunction(env, object, "getFocusElement", moduleName,
        NAccessibilityExtensionContext::GetFocusElement);
    BindNativeFunction(env, object, "getWindowRootElement", moduleName,
        NAccessibilityExtensionContext::GetWindowRootElement);
    BindNativeFunction(env, object, "getWindows", moduleName, NAccessibilityExtensionContext::GetWindows);
    BindNativeFunction(env, object, "injectGesture", moduleName, NAccessibilityExtensionContext::InjectGesture);
    BindNativeFunction(env, object, "injectGestureSync", moduleName, NAccessibilityExtensionContext::InjectGestureSync);
    return object;
}
} // namespace Accessibility
} // namespace OHOS