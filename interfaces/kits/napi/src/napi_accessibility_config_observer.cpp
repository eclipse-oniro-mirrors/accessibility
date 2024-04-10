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

#include "accessibility_utils.h"
#include "accessibility_config_observer.h"

#include <uv.h>

#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityNapi;
using namespace OHOS::AccessibilityConfig;

namespace OHOS {
namespace Accessibility {
napi_handle_scope TmpOpenScope(napi_env env)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL(env, napi_open_handle_scope(env, &scope));
    return scope;
}
} // namespace Accessibility
} // namespace OHOS

void NAccessibilityConfigObserver::OnConfigChangedExtra(const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));
    if (configId_ == CONFIG_CONTENT_TIMEOUT) {
        NotifyIntChanged2JS(static_cast<int32_t>(value.contentTimeout));
    } else if (configId_ == CONFIG_BRIGHTNESS_DISCOUNT) {
        NotifyFloatChanged2JS(value.brightnessDiscount);
    } else if (configId_ == CONFIG_AUDIO_BALANCE) {
        NotifyFloatChanged2JS(value.audioBalance);
    } else if (configId_ ==  CONFIG_HIGH_CONTRAST_TEXT) {
        NotifyStateChanged2JS(value.highContrastText);
    } else if (configId_ ==  CONFIG_DALTONIZATION_STATE) {
        NotifyStateChanged2JS(value.daltonizationState);
    } else if (configId_ == CONFIG_INVERT_COLOR) {
        NotifyStateChanged2JS(value.invertColor);
    } else if (configId_ == CONFIG_ANIMATION_OFF) {
        NotifyStateChanged2JS(value.animationOff);
    } else if (configId_ == CONFIG_AUDIO_MONO) {
        NotifyStateChanged2JS(value.audioMono);
    } else if (configId_ == CONIFG_CLICK_RESPONSE_TIME) {
        NotifyStringChanged2JS(ConvertClickResponseTimeTypeToString(value.clickResponseTime));
    } else if (configId_ == CONFIG_IGNORE_REPEAT_CLICK_TIME) {
        NotifyStringChanged2JS(ConvertIgnoreRepeatClickTimeTypeToString(value.ignoreRepeatClickTime));
    } else if (configId_ == CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        NotifyStateChanged2JS(value.ignoreRepeatClickState);
    }
}

void NAccessibilityConfigObserver::OnConfigChanged(const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));
    if (configId_ == CONFIG_CAPTION_STATE) {
        NotifyStateChanged2JS(value.captionState);
    } else if (configId_ == CONFIG_CAPTION_STYLE) {
        NotifyPropertyChanged2JS(value.captionStyle);
    } else if (configId_ == CONFIG_SCREEN_MAGNIFICATION) {
        NotifyStateChanged2JS(value.screenMagnifier);
    } else if (configId_ == CONFIG_MOUSE_KEY) {
        NotifyStateChanged2JS(value.mouseKey);
    } else if (configId_ == CONFIG_SHORT_KEY) {
        NotifyStateChanged2JS(value.shortkey);
    } else if (configId_ == CONFIG_SHORT_KEY_TARGET) {
        NotifyStringChanged2JS(value.shortkey_target);
    } else if (configId_ == CONFIG_SHORT_KEY_MULTI_TARGET) {
        NotifyStringVectorChanged2JS(value.shortkeyMultiTarget);
    } else if (configId_ == CONFIG_MOUSE_AUTOCLICK) {
        NotifyIntChanged2JS(value.mouseAutoClick);
    } else if (configId_ == CONFIG_DALTONIZATION_COLOR_FILTER) {
        OnDaltonizationColorFilterConfigChanged();
    } else {
        OnConfigChangedExtra(value);
    }
}

void NAccessibilityConfigObserver::OnDaltonizationColorFilterConfigChanged()
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    DALTONIZATION_TYPE type = Normal;
    RetError ret = instance.GetDaltonizationColorFilter(type);
    if (ret == RET_OK) {
        NotifyStringChanged2JS(ConvertDaltonizationTypeToString(type));
    } else {
        NotifyStringChanged2JS(ConvertDaltonizationTypeToString(type));
        HILOG_ERROR("get DaltonizationColorFilter failed: %{public}d", ret);
    }
}

int NAccessibilityConfigObserver::NotifyStateChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyPropertyChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CaptionCallbackInfo *callbackInfo = static_cast<CaptionCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyStringChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyStringVectorChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
            napi_value jsEvent;
            napi_create_array(callbackInfo->env_, &jsEvent);
            ConvertStringVecToJS(callbackInfo->env_, jsEvent, callbackInfo->stringVector_);

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
            HILOG_DEBUG("NotifyStringVectorChanged napi_call_function result[%{public}zu]", result);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyIntChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyUintChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

int NAccessibilityConfigObserver::NotifyFloatChanged(uv_work_t *work)
{
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            StateCallbackInfo *callbackInfo = static_cast<StateCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
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
        HILOG_ERROR("NotifyStateChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyStateChanged(work);
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
        HILOG_ERROR("NotifyPropertyChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyPropertyChanged(work);
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
        HILOG_ERROR("NotifyStringChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyStringChanged(work);
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyStringChanged2JS work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityConfigObserver::NotifyStringVectorChanged2JS(std::vector<std::string> value)
{
    HILOG_DEBUG();

    StateCallbackInfo *callbackInfo = new(std::nothrow) StateCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->stringVector_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = handlerRef_;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("NotifyStringVectorChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyStringVectorChanged(work);
    if (ret != 0) {
        HILOG_ERROR("Failed to execute NotifyStringVectorChanged2JS work queue");
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
        HILOG_ERROR("NotifyIntChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyIntChanged(work);
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
        HILOG_ERROR("NotifyUintChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyUintChanged(work);
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
        HILOG_ERROR("NotifyFloatChanged2JS Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = NotifyFloatChanged(work);
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
            HILOG_DEBUG("SubscribeObserver Observer exist");
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
