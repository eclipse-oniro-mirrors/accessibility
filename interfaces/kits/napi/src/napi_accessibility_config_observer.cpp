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
using namespace OHOS::AccessibilityNapi;
using namespace OHOS::AccessibilityConfig;

void NAccessibilityConfigObserver::OnConfigChanged(const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));
    switch (configId_) {
        case CONFIG_CAPTION_STATE:
            NotifyStateChanged2JS(value.captionState);
            break;
        case CONFIG_CAPTION_STYLE:
            NotifyPropertyChanged2JS(value.captionStyle);
            break;
        case CONFIG_SCREEN_MAGNIFICATION:
            NotifyStateChanged2JS(value.screenMagnifier);
            break;
        case CONFIG_MOUSE_KEY:
            NotifyStateChanged2JS(value.mouseKey);
            break;
        case CONFIG_SHORT_KEY:
            NotifyStateChanged2JS(value.shortkey);
            break;
        case CONFIG_SHORT_KEY_TARGET:
            NotifyStringChanged2JS(value.shortkey_target);
            break;
        case CONFIG_MOUSE_AUTOCLICK:
            NotifyIntChanged2JS(value.mouseAutoClick);
            break;
        case CONFIG_DALTONIZATION_COLOR_FILTER:
            NotifyStringChanged2JS(ConvertDaltonizationTypeToString(value.daltonizationColorFilter));
            break;
        case CONFIG_CONTENT_TIMEOUT:
            NotifyIntChanged2JS(static_cast<int32_t>(value.contentTimeout));
            break;
        case CONFIG_BRIGHTNESS_DISCOUNT:
            NotifyFloatChanged2JS(value.brightnessDiscount);
            break;
        case CONFIG_AUDIO_BALANCE:
            NotifyFloatChanged2JS(value.audioBalance);
            break;
        case CONFIG_HIGH_CONTRAST_TEXT:
            NotifyStateChanged2JS(value.highContrastText);
            break;
        case CONFIG_INVERT_COLOR:
            NotifyStateChanged2JS(value.invertColor);
            break;
        case CONFIG_AUDIO_MONO:
            NotifyStateChanged2JS(value.audioMono);
            break;
        case CONFIG_ANIMATION_OFF:
            NotifyStateChanged2JS(value.animationOff);
            break;
        default:
            break;
    }
}

void NAccessibilityConfigObserver::NotifyStateChanged2JS(bool enabled)
{
    HILOG_INFO("id = [%{public}d] enabled = [%{public}s]", static_cast<int32_t>(configId_), enabled ? "true" : "false");

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->state_ = enabled;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_object(callbackInfo->env_, &jsEvent);
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyStateChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyPropertyChanged2JS(const OHOS::AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));

    CaptionCallbackInfo *callbackInfo = new(std::nothrow) CaptionCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->caption_ = caption;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {},
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyPropertyChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyStringChanged2JS(const std::string& value)
{
    HILOG_INFO("value = [%{public}s]", value.c_str());

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->stringValue_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_value jsEvent;
            napi_create_string_utf8(callbackInfo->env_, callbackInfo->stringValue_.c_str(),
                callbackInfo->stringValue_.length(), &jsEvent);
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyStringChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyIntChanged2JS(int32_t value)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}d]", static_cast<int32_t>(configId_), value);

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->int32Value_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyIntChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyUintChanged2JS(uint32_t value)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}u]", static_cast<int32_t>(configId_), value);

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->uint32Value_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyUintChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyFloatChanged2JS(float value)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}f]", static_cast<int32_t>(configId_), value);

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->floatValue_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyFloatChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserverImpl::SubscribeToFramework()
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID_MAX); index ++) {
        instance.SubscribeConfigObserver(static_cast<CONFIG_ID>(index), shared_from_this(), false);
    }
}

void NAccessibilityConfigObserverImpl::OnConfigChanged(
    const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        if (observer && observer->configId_ == id) {
            observer->OnConfigChanged(value);
        }
    }
}

void NAccessibilityConfigObserverImpl::SubscribeObserver(napi_env env,
    OHOS::AccessibilityConfig::CONFIG_ID id, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->handlerRef_)) {
            HILOG_DEBUG("Observer exist");
            return;
        } else {
            iter++;
        }
    }

    napi_ref handler = nullptr;
    napi_create_reference(env, observer, 1, &handler);
    std::shared_ptr<NAccessibilityConfigObserver> observerPtr =
        std::make_shared<NAccessibilityConfigObserver>(env, handler, id);

    observers_.emplace_back(observerPtr);
}

void NAccessibilityConfigObserverImpl::UnsubscribeObserver(napi_env env,
    OHOS::AccessibilityConfig::CONFIG_ID id, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if ((*iter)->configId_ == id) {
            if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->handlerRef_)) {
                observers_.erase(iter);
                return;
            } else {
                iter++;
            }
        } else {
            iter++;
        }
    }
}

void NAccessibilityConfigObserverImpl::UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if ((*iter)->configId_ == id) {
            iter = observers_.erase(iter);
        } else {
            iter++;
        }
    }
}