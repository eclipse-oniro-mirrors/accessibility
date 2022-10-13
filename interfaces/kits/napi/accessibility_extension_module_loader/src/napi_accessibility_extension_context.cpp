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
#include "napi_accessibility_utils.h"

using namespace OHOS::AbilityRuntime;
using namespace OHOS::AccessibilityNapi;

namespace OHOS {
namespace Accessibility {
namespace {
constexpr int32_t CONTEXT_ERROR = 1;
constexpr int32_t PARAMETER_ERROR = 2;
constexpr int32_t RESULT_ERROR = 3;

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

    for (auto& windowInfo : accessibilityWindowInfos) {
        napi_value obj = nullptr;
        napi_new_instance(env, constructor, 0, nullptr, &obj);
        ConvertAccessibilityWindowInfoToJS(env, obj, windowInfo);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

class NAccessibilityExtensionContext final {
public:
    explicit NAccessibilityExtensionContext(
        const std::shared_ptr<AccessibilityExtensionContext>& context) : context_(context) {}
    ~NAccessibilityExtensionContext() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        HILOG_INFO();
        std::unique_ptr<NAccessibilityExtensionContext>(static_cast<NAccessibilityExtensionContext*>(data));
    }

    static NativeValue* SetTargetBundleName(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnSetTargetBundleName(*engine, *info) : nullptr;
    }

    static NativeValue* GetFocusElement(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetFocusElement(*engine, *info) : nullptr;
    }

    static NativeValue* GetWindowRootElement(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetWindowRootElement(*engine, *info) : nullptr;
    }

    static NativeValue* GetWindows(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetWindows(*engine, *info) : nullptr;
    }

    static NativeValue* InjectGesture(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGestureInject(*engine, *info) : nullptr;
    }

private:
    std::weak_ptr<AccessibilityExtensionContext> context_;

    NativeValue* OnSetTargetBundleName(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO();
        // Only support one or two params
        if (info.argc != ARGS_SIZE_ONE && info.argc != ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        // Unwrap target bundle names
        std::vector<std::string> targetBundleNames;
        ConvertJSToStringVec(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(info.argv[PARAM0]), targetBundleNames);
        HILOG_INFO("targetBundleNames's size = %{public}zu", targetBundleNames.size());

        AsyncTask::CompleteCallback complete =
            [weak = context_, targetBundleNames](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("SetTargetBundleName begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }

                RetError ret = context->SetTargetBundleName(targetBundleNames);
                if (ret == RET_OK) {
                    task.Resolve(engine, engine.CreateUndefined());
                } else {
                    HILOG_ERROR("set target bundle name failed. ret: %{public}d.", ret);
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "set target bundle name failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr : info.argv[PARAM1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::OnSetTargetBundleName",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetFocusElement(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO();
        // Support 0 ~ 2 params
        if (info.argc < ARGS_SIZE_ZERO || info.argc > ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        bool isAccessibilityFocus = false;
        NativeValue* lastParam = nullptr;
        if (info.argv[PARAM0]->TypeOf() == NATIVE_FUNCTION && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            lastParam = info.argv[PARAM0];
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_BOOLEAN && info.argv[PARAM1]->TypeOf() == NATIVE_FUNCTION) {
            if (!ConvertFromJsValue(engine, info.argv[PARAM0], isAccessibilityFocus)) {
                HILOG_ERROR("Convert isAccessibilityFocus from js value failed");
                return engine.CreateUndefined();
            }
            lastParam = info.argv[PARAM1];
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_BOOLEAN && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            if (!ConvertFromJsValue(engine, info.argv[PARAM0], isAccessibilityFocus)) {
                HILOG_ERROR("Convert isAccessibilityFocus from js value failed");
                return engine.CreateUndefined();
            }
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_UNDEFINED &&
                   info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            // Use default value.
        } else {
            HILOG_ERROR("Params is wrong");
            return engine.CreateUndefined();
        }

        int32_t focus = isAccessibilityFocus ? FOCUS_TYPE_ACCESSIBILITY : FOCUS_TYPE_INPUT;
        HILOG_DEBUG("focus type is [%{public}d]", focus);

        AsyncTask::CompleteCallback complete =
            [weak = context_, focus](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetFocusElement begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }

                OHOS::Accessibility::AccessibilityElementInfo elementInfo;
                RetError ret = context->GetFocus(focus, elementInfo);
                if (ret == RET_OK) {
                    napi_value constructor = nullptr;
                    napi_get_reference_value(reinterpret_cast<napi_env>(&engine), NAccessibilityElement::consRef_,
                        &constructor);
                    napi_value napiElementInfo = nullptr;
                    napi_new_instance(reinterpret_cast<napi_env>(&engine), constructor, 0, nullptr, &napiElementInfo);
                    NAccessibilityElement::ConvertElementInfoToJS(reinterpret_cast<napi_env>(&engine),
                        napiElementInfo, elementInfo);
                    NativeValue* nativeElementInfo = reinterpret_cast<NativeValue*>(napiElementInfo);
                    task.Resolve(engine, nativeElementInfo);
                } else {
                    HILOG_ERROR("Get focus elementInfo failed. ret: %{public}d", ret);
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "Get focus elementInfo failed."));
                }
            };

        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::OnGetFocusElement",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetWindowRootElement(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO();
        // Support 0 ~ 2 params
        if (info.argc < ARGS_SIZE_ZERO || info.argc > ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        int32_t windowId = INVALID_WINDOW_ID;
        bool isActiveWindow = true;
        NativeValue* lastParam = nullptr;
        if (info.argv[PARAM0]->TypeOf() == NATIVE_FUNCTION && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            lastParam = info.argv[PARAM0];
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_NUMBER && info.argv[PARAM1]->TypeOf() == NATIVE_FUNCTION) {
            if (!ConvertFromJsValue(engine, info.argv[PARAM0], windowId)) {
                HILOG_ERROR("Convert windowId failed");
                return engine.CreateUndefined();
            }
            lastParam = info.argv[PARAM1];
            isActiveWindow = false;
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_NUMBER && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            bool ret = ConvertFromJsValue(engine, info.argv[PARAM0], windowId);
            if (!ret) {
                HILOG_ERROR("Convert windowId failed. ret[%{public}d] windowId[%{public}d]", ret, windowId);
                return engine.CreateUndefined();
            }
            isActiveWindow = false;
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_UNDEFINED &&
                   info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            // Use default value.
        } else {
            HILOG_ERROR("Params is wrong");
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [weak = context_, windowId, isActiveWindow](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetWindowRootElement begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }

                HILOG_DEBUG("isActiveWindow[%{public}d] windowId[%{public}d]", isActiveWindow, windowId);
                OHOS::Accessibility::AccessibilityElementInfo elementInfo;
                RetError ret = RET_OK;
                if (isActiveWindow) {
                    ret = context->GetRoot(elementInfo);
                } else {
                    AccessibilityWindowInfo windowInfo;
                    windowInfo.SetWindowId(windowId);
                    ret = context->GetRootByWindow(windowInfo, elementInfo);
                }
                if (ret == RET_OK) {
                    napi_value constructor = nullptr;
                    napi_get_reference_value(reinterpret_cast<napi_env>(&engine), NAccessibilityElement::consRef_,
                        &constructor);
                    napi_value napiElementInfo = nullptr;
                    napi_status result = napi_new_instance(reinterpret_cast<napi_env>(&engine), constructor, 0, nullptr,
                        &napiElementInfo);
                    HILOG_DEBUG("napi_new_instance result is %{public}d", result);
                    NAccessibilityElement::ConvertElementInfoToJS(reinterpret_cast<napi_env>(&engine),
                        napiElementInfo, elementInfo);
                    NativeValue* nativeElementInfo = reinterpret_cast<NativeValue*>(napiElementInfo);
                    task.Resolve(engine, nativeElementInfo);
                } else {
                    HILOG_ERROR("Get root elementInfo failed. ret : %{public}d", ret);
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "Get root elementInfo failed."));
                }
            };

        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::OnGetWindowRootElement",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetWindows(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO();
        // Support 0 ~ 2 params
        if (info.argc < ARGS_SIZE_ZERO && info.argc > ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        int64_t displayId = 0;
        bool hasDisplayId = false;
        NativeValue* lastParam = nullptr;

        if (info.argv[PARAM0]->TypeOf() == NATIVE_FUNCTION && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            hasDisplayId = false;
            lastParam = info.argv[PARAM0];
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_NUMBER && info.argv[PARAM1]->TypeOf() == NATIVE_FUNCTION) {
            hasDisplayId = true;
            if (!ConvertFromJsValue(engine, info.argv[PARAM0], displayId)) {
                HILOG_ERROR("Convert displayId from js value failed");
                return engine.CreateUndefined();
            }
            lastParam = info.argv[PARAM1];
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_NUMBER && info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            hasDisplayId = true;
            if (!ConvertFromJsValue(engine, info.argv[PARAM0], displayId)) {
                HILOG_ERROR("Convert displayId from js value failed");
                return engine.CreateUndefined();
            }
        } else if (info.argv[PARAM0]->TypeOf() == NATIVE_UNDEFINED &&
                   info.argv[PARAM1]->TypeOf() == NATIVE_UNDEFINED) {
            hasDisplayId = false;
        } else {
            HILOG_ERROR("Params is wrong");
            return engine.CreateUndefined();
        }

        return hasDisplayId ? GetWindowsByDisplayIdAsync(engine, lastParam, displayId) :
            GetWindowsAsync(engine, lastParam);
    }

    NativeValue* GetWindowsAsync(NativeEngine& engine, NativeValue* lastParam)
    {
        HILOG_INFO();
        AsyncTask::CompleteCallback complete =
            [weak = context_](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetWindows begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }

                std::vector<OHOS::Accessibility::AccessibilityWindowInfo> accessibilityWindows;
                RetError ret = context->GetWindows(accessibilityWindows);
                if (ret == RET_OK) {
                    napi_value napiWindowInfos = nullptr;
                    napi_create_array(reinterpret_cast<napi_env>(&engine), &napiWindowInfos);
                    ConvertAccessibilityWindowInfosToJS(
                        reinterpret_cast<napi_env>(&engine), napiWindowInfos, accessibilityWindows);
                    NativeValue* nativeWindowInfos = reinterpret_cast<NativeValue*>(napiWindowInfos);
                    task.Resolve(engine, nativeWindowInfos);
                } else {
                    HILOG_ERROR("Get windowInfos failed.");
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "Get windowInfos failed."));
                }
            };

        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::GetWindowsAsync",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* GetWindowsByDisplayIdAsync(NativeEngine& engine, NativeValue* lastParam, int64_t displayId)
    {
        HILOG_INFO();
        AsyncTask::CompleteCallback complete =
            [weak = context_, displayId](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetWindows begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }

                if (displayId < 0) {
                    HILOG_ERROR("displayId is error: %{public}" PRId64 "", displayId);
                    task.Reject(engine, CreateJsError(engine, PARAMETER_ERROR, "displayId is error"));
                    return;
                }

                std::vector<OHOS::Accessibility::AccessibilityWindowInfo> accessibilityWindows;
                RetError ret = context->GetWindows(static_cast<uint64_t>(displayId), accessibilityWindows);
                if (ret == RET_OK) {
                    napi_value napiWindowInfos = nullptr;
                    napi_create_array(reinterpret_cast<napi_env>(&engine), &napiWindowInfos);
                    ConvertAccessibilityWindowInfosToJS(
                        reinterpret_cast<napi_env>(&engine), napiWindowInfos, accessibilityWindows);
                    NativeValue* nativeWindowInfos = reinterpret_cast<NativeValue*>(napiWindowInfos);
                    task.Resolve(engine, nativeWindowInfos);
                } else {
                    HILOG_ERROR("Get windowInfos failed.");
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "Get windowInfos failed."));
                }
            };

        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::GetWindowsByDisplayIdAsync",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGestureInject(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO();
        // Only support two or three params
        if (info.argc != ARGS_SIZE_ONE && info.argc != ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        // Unwrap gesturePaths
        bool isParameterArray = false;
        napi_value nGesturePaths = reinterpret_cast<napi_value>(info.argv[PARAM0]);
        std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
            std::make_shared<AccessibilityGestureInjectPath>();
        std::vector<std::shared_ptr<AccessibilityGestureInjectPath>> gesturePathArray;
        ConvertGesturePathsJSToNAPI(reinterpret_cast<napi_env>(&engine), nGesturePaths,
            gesturePath, gesturePathArray, isParameterArray);

        AsyncTask::CompleteCallback complete =
            [weak = context_, gesturePath, gesturePathArray, isParameterArray](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, CONTEXT_ERROR, "Context is released"));
                    return;
                }
                RetError ret = RET_OK;
                if (isParameterArray) {
                    ret = context->InjectGesture(gesturePathArray);
                } else {
                    ret = context->InjectGesture(gesturePath);
                }
                if (ret == RET_OK) {
                    task.Resolve(engine, engine.CreateUndefined());
                } else {
                    HILOG_ERROR("Gesture inject failed. ret: %{public}d.", ret);
                    task.Reject(engine, CreateJsError(engine, RESULT_ERROR, "Gesture inject failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr : info.argv[PARAM1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("NAccessibilityExtensionContext::OnGestureInject",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }
};
} // namespace

NativeValue* CreateJsAccessibilityExtensionContext(
    NativeEngine& engine, std::shared_ptr<AccessibilityExtensionContext> context)
{
    HILOG_INFO();
    NativeValue* objValue = CreateJsExtensionContext(engine, context);
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<NAccessibilityExtensionContext> jsContext =
        std::make_unique<NAccessibilityExtensionContext>(context);
    if (!object) {
        HILOG_ERROR("object is nullptr.");
        return nullptr;
    }
    object->SetNativePointer(jsContext.release(), NAccessibilityExtensionContext::Finalizer, nullptr);

    const char *moduleName = "NAccessibilityExtensionContext";
    BindNativeFunction(engine, *object, "setTargetBundleName", moduleName,
        NAccessibilityExtensionContext::SetTargetBundleName);
    BindNativeFunction(engine, *object, "getFocusElement", moduleName,
        NAccessibilityExtensionContext::GetFocusElement);
    BindNativeFunction(engine, *object, "getWindowRootElement", moduleName,
        NAccessibilityExtensionContext::GetWindowRootElement);
    BindNativeFunction(engine, *object, "getWindows", moduleName, NAccessibilityExtensionContext::GetWindows);
    BindNativeFunction(engine, *object, "injectGesture", moduleName, NAccessibilityExtensionContext::InjectGesture);

    return objValue;
}
} // namespace Accessibility
} // namespace OHOS