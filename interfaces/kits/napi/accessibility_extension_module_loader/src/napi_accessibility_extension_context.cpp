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
#include "js_extension_context.h"
#include "js_runtime_utils.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_info.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_window_info.h"

using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace Accessibility {
namespace {
constexpr int32_t ERROR_CODE_ONE = 1;
class NAccessibilityExtensionContext final {
public:
    explicit NAccessibilityExtensionContext(
        const std::shared_ptr<AccessibilityExtensionContext>& context) : context_(context) {}
    ~NAccessibilityExtensionContext() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        HILOG_INFO("Finalizer is called");
        std::unique_ptr<NAccessibilityExtensionContext>(static_cast<NAccessibilityExtensionContext*>(data));
    }

    static NativeValue* GetFocusElementInfo(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetFocusElementInfo(*engine, *info) : nullptr;
    }

    static NativeValue* GetRootElementInfo(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetRootElementInfo(*engine, *info) : nullptr;
    }

    static NativeValue* GetWindows(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGetWindows(*engine, *info) : nullptr;
    }

    static NativeValue* ExecuteCommonAction(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnExecuteCommonAction(*engine, *info) : nullptr;
    }

    static NativeValue* GestureInject(NativeEngine* engine, NativeCallbackInfo* info)
    {
        NAccessibilityExtensionContext* me = CheckParamsAndGetThis<NAccessibilityExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnGestureInject(*engine, *info) : nullptr;
    }

private:
    std::weak_ptr<AccessibilityExtensionContext> context_;

    NativeValue* OnGetFocusElementInfo(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO("called.");
        // Only support one or two params
        if (info.argc != ARGS_SIZE_ONE && info.argc != ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        // Unwrap focusType
        std::string focusType;
        if (!ConvertFromJsValue(engine, info.argv[PARAM0], focusType)) {
            HILOG_ERROR("ConvertFromJsValue failed");
            return engine.CreateUndefined();
        }
        HILOG_INFO("GetFocusElementInfo focusType = %{public}s", focusType.c_str());

        int32_t focus = FOCUS_TYPE_INVALID;
        if (!std::strcmp(focusType.c_str(), "accessibility")) {
            focus = FOCUS_TYPE_ACCESSIBILITY;
        } else if (!std::strcmp(focusType.c_str(), "normal")) {
            focus = FOCUS_TYPE_INPUT;
        } else {
            focus = FOCUS_TYPE_INVALID;
        }

        AsyncTask::CompleteCallback complete =
            [weak = context_, focus](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetFocusElementInfo begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, ERROR_CODE_ONE, "Context is released"));
                    return;
                }

                OHOS::Accessibility::AccessibilityElementInfo elementInfo;
                bool ret = context->GetFocus(focus, elementInfo);
                if (ret) {
                    napi_value constructor = nullptr;
                    napi_get_reference_value(reinterpret_cast<napi_env>(&engine), NElementInfo::consRef_, &constructor);
                    napi_value napiElementInfo = nullptr;
                    napi_new_instance(reinterpret_cast<napi_env>(&engine), constructor, 0, nullptr, &napiElementInfo);
                    ConvertElementInfoToJS(reinterpret_cast<napi_env>(&engine), napiElementInfo, elementInfo);
                    NativeValue* nativeElementInfo = reinterpret_cast<NativeValue*>(napiElementInfo);
                    task.Resolve(engine, nativeElementInfo);
                } else {
                    HILOG_ERROR("Get focus elementInfo failed. ret: %{public}d", ret);
                    task.Reject(engine, CreateJsError(engine, false, "Get focus elementInfo failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr : info.argv[PARAM1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetRootElementInfo(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO("called.");
        // only support zero or one params
        if (info.argc != ARGS_SIZE_ZERO && info.argc != ARGS_SIZE_ONE) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [weak = context_](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetRootElementInfo begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, ERROR_CODE_ONE, "Context is released"));
                    return;
                }

                OHOS::Accessibility::AccessibilityElementInfo elementInfo;
                bool ret = context->GetRoot(elementInfo);
                if (ret) {
                    napi_value constructor = nullptr;

                    napi_get_reference_value(reinterpret_cast<napi_env>(&engine), NElementInfo::consRef_, &constructor);
                    napi_value napiElementInfo = nullptr;
                    napi_status result = napi_new_instance(reinterpret_cast<napi_env>(&engine), constructor, 0, nullptr,
                        &napiElementInfo);
                    HILOG_DEBUG("napi_new_instance result is %{public}d", result);
                    ConvertElementInfoToJS(reinterpret_cast<napi_env>(&engine), napiElementInfo, elementInfo);
                    NativeValue* nativeElementInfo = reinterpret_cast<NativeValue*>(napiElementInfo);
                    task.Resolve(engine, nativeElementInfo);
                } else {
                    HILOG_ERROR("Get root elementInfo failed. ret : %{public}d", ret);
                    task.Reject(engine, CreateJsError(engine, false, "Get root elementInfo failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ZERO) ? nullptr : info.argv[PARAM0];
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetWindows(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO("called.");
        // Only support zero or one params
        if (info.argc != ARGS_SIZE_ZERO && info.argc != ARGS_SIZE_ONE) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [weak = context_](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GetWindows begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, ERROR_CODE_ONE, "Context is released"));
                    return;
                }

                std::vector<OHOS::Accessibility::AccessibilityWindowInfo> accessibilityWindows;
                accessibilityWindows = context->GetWindows();
                if (!accessibilityWindows.empty()) {
                    napi_value napiWindowInfos = nullptr;
                    napi_create_array(reinterpret_cast<napi_env>(&engine), &napiWindowInfos);
                    ConvertAccessibilityWindowInfosToJS(
                        reinterpret_cast<napi_env>(&engine), napiWindowInfos, accessibilityWindows);
                    NativeValue* nativeWindowInfos = reinterpret_cast<NativeValue*>(napiWindowInfos);
                    task.Resolve(engine, nativeWindowInfos);
                } else {
                    HILOG_ERROR("Get windowInfos failed.");
                    task.Reject(engine, CreateJsError(engine, false, "Get windowInfos failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ZERO) ? nullptr : info.argv[PARAM0];
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnExecuteCommonAction(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO("called.");
        // Only support one or two params
        if (info.argc != ARGS_SIZE_ONE && info.argc != ARGS_SIZE_TWO) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        // Unwrap actionType
        std::string actionType;
        if (!ConvertFromJsValue(engine, info.argv[PARAM0], actionType)) {
            HILOG_ERROR("ConvertFromJsValue failed");
            return engine.CreateUndefined();
        }
        HILOG_INFO("ExecuteCommonAction actionType = %{public}s", actionType.c_str());

        GlobalAction action = ConvertStringToGlobalAction(actionType);
        HILOG_INFO("ExecuteCommonAction action = %{public}d", action);

        AsyncTask::CompleteCallback complete =
            [weak = context_, action](NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("ExecuteCommonAction begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, ERROR_CODE_ONE, "Context is released"));
                    return;
                }

                bool ret = context->ExecuteCommonAction(action);
                if (ret) {
                    task.Resolve(engine, engine.CreateBoolean(ret));
                } else {
                    HILOG_ERROR("Perform common action failed. ret: %{public}d.", ret);
                    task.Reject(engine, CreateJsError(engine, false, "Perform common action failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_ONE) ? nullptr : info.argv[PARAM1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGestureInject(NativeEngine& engine, NativeCallbackInfo& info)
    {
        HILOG_INFO("called.");
        // Only support two or three params
        if (info.argc != ARGS_SIZE_TWO && info.argc != ARGS_SIZE_THREE) {
            HILOG_ERROR("Not enough params");
            return engine.CreateUndefined();
        }

        // Unwrap gesturePaths
        napi_value nGesturePaths = reinterpret_cast<napi_value>(info.argv[PARAM0]);
        std::vector<AccessibilityGesturePath> gesturePaths;
        ConvertGesturePathsJSToNAPI(reinterpret_cast<napi_env>(&engine), nGesturePaths, gesturePaths);

        // Unwrap callback
        if (info.argv[PARAM1]->TypeOf() != NATIVE_FUNCTION) {
            HILOG_ERROR("The type of params is %{public}d.", info.argv[PARAM1]->TypeOf());
            return engine.CreateUndefined();
        }
        // Create callback info
        std::shared_ptr<NAccessibilityGestureResultListenerInfo> pCallbackInfo =
            std::make_shared<NAccessibilityGestureResultListenerInfo>();
        pCallbackInfo->env_ = reinterpret_cast<napi_env>(&engine);
        NAPI_CALL(reinterpret_cast<napi_env>(&engine), napi_create_reference(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(info.argv[PARAM1]), 1, &pCallbackInfo->callback_));
        pCallbackInfo->listener_ = std::make_shared<NAccessibilityGestureResultListener>();
        // Save callback info
        gestureInjectSequence ++;
        jsGestureResultListenerInfos[gestureInjectSequence] = pCallbackInfo;

        AsyncTask::CompleteCallback complete =
            [weak = context_, sequence = gestureInjectSequence, gesturePaths,
            listener = pCallbackInfo->listener_](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
                HILOG_INFO("GestureInject begin");
                auto context = weak.lock();
                if (!context) {
                    HILOG_ERROR("context is released");
                    task.Reject(engine, CreateJsError(engine, ERROR_CODE_ONE, "Context is released"));
                    return;
                }

                bool ret = context->GestureInject(sequence, gesturePaths, listener);
                if (ret) {
                    task.Resolve(engine, engine.CreateBoolean(ret));
                } else {
                    HILOG_ERROR("Gesture inject failed. ret: %{public}d.", ret);
                    task.Reject(engine, CreateJsError(engine, false, "Gesture inject failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGS_SIZE_TWO) ? nullptr : info.argv[PARAM2];
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }
};
} // namespace

NativeValue* CreateJsAccessibilityExtensionContext(
    NativeEngine& engine, std::shared_ptr<AccessibilityExtensionContext> context)
{
    HILOG_INFO("called.");
    NativeValue* objValue = CreateJsExtensionContext(engine, context);
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<NAccessibilityExtensionContext> jsContext =
        std::make_unique<NAccessibilityExtensionContext>(context);
    if (!object) {
        HILOG_ERROR("object is nullptr.");
        return nullptr;
    }
    object->SetNativePointer(jsContext.release(), NAccessibilityExtensionContext::Finalizer, nullptr);

    BindNativeFunction(engine, *object, "getFocusElementInfo", NAccessibilityExtensionContext::GetFocusElementInfo);
    BindNativeFunction(engine, *object, "getRootElementInfo", NAccessibilityExtensionContext::GetRootElementInfo);
    BindNativeFunction(engine, *object, "getWindows", NAccessibilityExtensionContext::GetWindows);
    BindNativeFunction(engine, *object, "executeCommonAction", NAccessibilityExtensionContext::ExecuteCommonAction);
    BindNativeFunction(engine, *object, "gestureInject", NAccessibilityExtensionContext::GestureInject);

    HILOG_INFO("called end.");
    return objValue;
}

void NAccessibilityGestureResultListener::OnGestureInjectResult(uint32_t sequence, bool result)
{
    HILOG_INFO("called.");

    if (jsGestureResultListenerInfos.empty()) {
        HILOG_ERROR("There is no information of jsGestureResultListenerInfos");
        return;
    }

    std::shared_ptr<NAccessibilityGestureResultListenerInfo> callbackInfo = nullptr;
    auto it = jsGestureResultListenerInfos.find(sequence);
    if (it != jsGestureResultListenerInfos.end()) {
        HILOG_DEBUG("callbackInfo has been found.");
        callbackInfo = jsGestureResultListenerInfos[sequence];
    }

    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    StateCallbackInfo *data = new StateCallbackInfo();
    data->env_ = callbackInfo->env_;
    data->state_ = result;
    data->ref_ = callbackInfo->callback_;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = data;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value napiResult = 0;
            napi_value callResult = 0;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_get_boolean(callbackInfo->env_, callbackInfo->state_, &napiResult);
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &callback);
            napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &napiResult, &callResult);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}
} // namespace Accessibility
} // namespace OHOS