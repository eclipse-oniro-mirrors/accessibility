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

#include "napi_accessibility_utils.h"
#include "napi_accessibility_config_observer.h"

#include <uv.h>

#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityConfig;

napi_value NAccessibilityConfigObserver::StartWork(
    napi_env env, size_t functionIndex, napi_value (&args)[CONFIG_START_WORK_ARGS_SIZE])
{
    HILOG_INFO();
    int32_t id = 0;
    ParseInt32(env, id, args[0]);
    configId_ = static_cast<OHOS::AccessibilityConfig::CONFIG_ID>(id);
    napi_create_reference(env, args[functionIndex], 1, &handlerRef_);
    env_ = env;
    napi_value result = {0};
    return result;
}

void NAccessibilityConfigObserver::NotifyStateChangedJS(napi_env env, bool enabled,
    const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d] enabled = [%{public}s]", static_cast<int32_t>(id), enabled ? "true" : "false");

    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->state_ = enabled;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<StateCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_get_boolean(callbackInfo->env_, callbackInfo->state_, &jsEvent);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            int32_t result;
            napi_get_value_int32(callbackInfo->env_, callResult, &result);
            HILOG_INFO("NotifyStateChangedJS napi_call_function result[%{public}d]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void NAccessibilityConfigObserver::OnConfigChanged(const CONFIG_ID id, const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(id));
    switch (id) {
        case CONFIG_CAPTION_STATE:
            NotifyStateChanged2JS(id, value.captionState);
            break;
        case CONFIG_CAPTION_STYLE:
            NotifyPropertyChanged2JS(id, value.captionStyle);
            break;
        case CONFIG_SCREEN_MAGNIFICATION:
            NotifyStateChanged2JS(id, value.screenMagnifier);
            break;
        case CONFIG_MOUSE_KEY:
            NotifyStateChanged2JS(id, value.mouseKey);
            break;
        case CONFIG_SHORT_KEY:
            NotifyStateChanged2JS(id, value.shortkey);
            break;
        case CONFIG_SHORT_KEY_TARGET:
            NotifyStringChanged2JS(id, value.shortkey_target);
            break;
        case CONFIG_MOUSE_AUTOCLICK:
            NotifyIntChanged2JS(id, value.mouseAutoClick);
            break;
        case CONFIG_DALTONIZATION_COLOR_FILTER:
            NotifyUintChanged2JS(id, value.daltonizationColorFilter);
            break;
        case CONFIG_CONTENT_TIMEOUT:
            NotifyUintChanged2JS(id, value.contentTimeout);
            break;
        case CONFIG_BRIGHTNESS_DISCOUNT:
            NotifyFloatChanged2JS(id, value.brightnessDiscount);
            break;
        case CONFIG_AUDIO_BALANCE:
            NotifyFloatChanged2JS(id, value.audioBalance);
            break;
        default:
            break;
    }
}

void NAccessibilityConfigObserver::NotifyPropertyChangedJS(napi_env env,
    OHOS::AccessibilityConfig::CaptionProperty caption,
    const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(id));

    CaptionCallbackInfo *callbackInfo = new CaptionCallbackInfo();
    callbackInfo->caption_ = caption;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<CaptionCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CaptionCallbackInfo *callbackInfo = static_cast<CaptionCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_object(callbackInfo->env_, &jsEvent);
            ConvertCaptionPropertyToJS(callbackInfo->env_, jsEvent, callbackInfo->caption_);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            int32_t result;
            napi_get_value_int32(callbackInfo->env_, callResult, &result);
            HILOG_INFO("NotifyPropertyChangedJS napi_call_function result[%{public}d]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void NAccessibilityConfigObserver::NotifyStateChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, bool enabled)
{
    if (GetConfigId() == id) {
        NotifyStateChangedJS(GetEnv(), enabled, id, GetHandler());
    }
}

void NAccessibilityConfigObserver::NotifyPropertyChanged2JS(
    const OHOS::AccessibilityConfig::CONFIG_ID id, OHOS::AccessibilityConfig::CaptionProperty caption)
{
    if (GetConfigId() == id) {
        NotifyPropertyChangedJS(GetEnv(), caption, id, GetHandler());
    }
}

void NAccessibilityConfigObserver::NotifyStringChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id,
                                                          const std::string &value)
{
    if (GetConfigId() == id) {
        NotifyStringChanged2JSInner(GetEnv(), value, id, GetHandler());
    }
}
void NAccessibilityConfigObserver::NotifyUintChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, uint32_t value)
{
    if (GetConfigId() == id) {
        NotifyUintChanged2JSInner(GetEnv(), value, id, GetHandler());
    }
}
void NAccessibilityConfigObserver::NotifyIntChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, int32_t value)
{
    if (GetConfigId() == id) {
        NotifyIntChanged2JSInner(GetEnv(), value, id, GetHandler());
    }
}

void NAccessibilityConfigObserver::NotifyFloatChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, float value)
{
    if (GetConfigId() == id) {
        NotifyFloatChanged2JSInner(GetEnv(), value, id, GetHandler());
    }
}

void NAccessibilityConfigObserver::NotifyStringChanged2JSInner(
    napi_env env, const std::string& value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}s]", static_cast<int32_t>(id), value.c_str());

    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->stringValue_ = value;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<StateCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_string_utf8(callbackInfo->env_,
                callbackInfo->stringValue_.c_str(),
                callbackInfo->stringValue_.length(),
                &jsEvent);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            size_t result;
            const uint32_t BUF_SIZE = 1024;
            char buf[BUF_SIZE] = {0};
            napi_get_value_string_utf8(callbackInfo->env_, callResult, buf, BUF_SIZE, &result);
            HILOG_INFO("NotifyStringChanged2JSInner napi_call_function result[%{public}zu]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void NAccessibilityConfigObserver::NotifyIntChanged2JSInner(
    napi_env env, int32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}d]", static_cast<int32_t>(id), value);

    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->int32Value_ = value;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<StateCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_int32(callbackInfo->env_, callbackInfo->int32Value_, &jsEvent);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            int32_t result;
            napi_get_value_int32(callbackInfo->env_, callResult, &result);
            HILOG_INFO("NotifyIntChanged2JSInner napi_call_function result[%{public}d]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void NAccessibilityConfigObserver::NotifyUintChanged2JSInner(
    napi_env env, uint32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}u]", static_cast<int32_t>(id), value);

    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->uint32Value_ = value;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<StateCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_uint32(callbackInfo->env_, callbackInfo->uint32Value_, &jsEvent);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            uint32_t result;
            napi_get_value_uint32(callbackInfo->env_, callResult, &result);
            HILOG_INFO("NotifyUintChanged2JSInner napi_call_function result[%{public}d]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void NAccessibilityConfigObserver::NotifyFloatChanged2JSInner(
    napi_env env, float value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}f]", static_cast<int32_t>(id), value);

    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->floatValue_ = value;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<StateCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_double(callbackInfo->env_, double(callbackInfo->floatValue_), &jsEvent);

            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            int32_t result;
            napi_get_value_int32(callbackInfo->env_, callResult, &result);
            HILOG_INFO("NotifyFloatChanged2JSInner napi_call_function result[%{public}d]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}