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

#include "napi_accessibility_config.h"

#include <uv.h>
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityConfig;

std::vector<std::shared_ptr<ConfigListener>> NAccessibilityConfig::configListeners_ = {};
std::vector<std::shared_ptr<EnableAbilityListsObserver>> NAccessibilityConfig::enableAbilityListsObservers_ = {};

napi_value NAccessibilityConfig::EnableAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_THREE;
    napi_value parameters[ARGS_SIZE_THREE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilityConfigData* callbackInfo = new NAccessibilityConfigData();
    // parse name
    std::string ability = "";
    ParseString(env, ability, parameters[PARAM0]);
    HILOG_INFO("ability = %{public}s", ability.c_str());
    callbackInfo->abilityName_ = ability;

    // parse capability
    ConvertJSToCapabilities(env, parameters[PARAM1], callbackInfo->capabilities_);

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_THREE) {
        napi_create_reference(env, parameters[PARAM2], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "EnableAbility", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityConfigData* callbackInfo = (NAccessibilityConfigData*)data;
            auto instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            if (instance && callbackInfo->capabilities_ != 0) {
                callbackInfo->ret_ = instance->EnableAbilities(
                    callbackInfo->abilityName_, callbackInfo->capabilities_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityConfigData* callbackInfo = (NAccessibilityConfigData*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->ret_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityConfig::DisableAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilityConfigData* callbackInfo = new NAccessibilityConfigData();
    // parse name
    std::string ability = "";
    ParseString(env, ability, parameters[PARAM0]);
    HILOG_INFO("ability = %{public}s", ability.c_str());
    callbackInfo->abilityName_ = ability;

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_TWO) {
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "DisableAbility", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilityConfigData* callbackInfo = (NAccessibilityConfigData*)data;
            auto instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            if (instance) {
                callbackInfo->ret_ = instance->DisableAbilities(callbackInfo->abilityName_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityConfigData* callbackInfo = (NAccessibilityConfigData*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->ret_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                napi_value callback = 0;
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityConfig::SubscribeEnableAbilityListsObserver(napi_env env, napi_value (&args)[ARGS_SIZE_TWO])
{
    HILOG_INFO("start");
    std::shared_ptr<EnableAbilityListsObserver> observer = std::make_shared<EnableAbilityListsObserver>();
    napi_ref callback = nullptr;
    napi_create_reference(env, args[1], 1, &callback);
    observer->SetCallBack(callback);
    observer->SetEnv(env);
    enableAbilityListsObservers_.push_back(observer);
    HILOG_INFO("observer size%{public}zu", enableAbilityListsObservers_.size());

    auto instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (instance) {
        instance->SubscribeEnableAbilityListsObserver(observer);
    }
    return nullptr;
}

napi_value NAccessibilityConfig::UnsubscribeEnableAbilityListsObserver(napi_env env)
{
    HILOG_INFO("start and observer size%{public}zu", enableAbilityListsObservers_.size());
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end();) {
        if ((*iter)->GetEnv() == env) {
            auto instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            if (instance) {
                instance->UnsubscribeEnableAbilityListsObserver(*iter);
                enableAbilityListsObservers_.erase(iter);
            }
        } else {
            iter ++;
        }
    }
    return nullptr;
}

void EnableAbilityListsObserver::OnEnableAbilityListsStateChanged()
{
    HILOG_INFO("start");

    AccessibilityCallbackInfo *callbackInfo = new AccessibilityCallbackInfo();
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = callback_;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = (AccessibilityCallbackInfo *)callbackInfo;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AccessibilityCallbackInfo *callbackInfo = (AccessibilityCallbackInfo *)work->data;
            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_value jsEvent = nullptr;
            napi_value undefined = nullptr;
            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}
OHOS::AccessibilityConfig::CONFIG_ID ConfigListener::GetStateType()
{
    HILOG_INFO("start");
    OHOS::AccessibilityConfig::CONFIG_ID type = CONFIG_CAPTION_STATE;
    if (!std::strcmp(eventType_.c_str(), "enableChange")) {
        type = CONFIG_CAPTION_STATE;
    } else if (!std::strcmp(eventType_.c_str(), "styleChange")) {
        type = CONFIG_CAPTION_STYLE;
    } else {
        HILOG_ERROR("GetStateType eventType[%s] is error", eventType_.c_str());
    }
    return type;
}

napi_value ConfigListener::StartWork(napi_env env, size_t functionIndex, napi_value (&args)[CONFIG_START_WORK_ARGS_SIZE])
{
    HILOG_INFO("start");
    eventType_ = GetStringFromNAPI(env, args[0]);
    napi_create_reference(env, args[functionIndex], 1, &handlerRef_);
    env_ = env;
    napi_value result = {0};
    return result;
}

void ConfigListener::NotifyStateChangedJS(napi_env env, bool enabled, std::string eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "enableChange")) {
        StateCallbackInfo *callbackInfo = new StateCallbackInfo();
        callbackInfo->state_ = enabled;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (StateCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
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
            } 
        );
    } else {
        HILOG_ERROR("NotifyStateChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void ConfigListener::OnConfigChanged(const CONFIG_ID id, const ConfigValue &value)
{
    HILOG_INFO("start");
    switch (id)
    {
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

void ConfigListener::NotifyPropertyChangedJS(napi_env env,
    OHOS::AccessibilityConfig::CaptionProperty caption, std::string eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "styleChange")) {
        CaptionCallbackInfo *callbackInfo = new CaptionCallbackInfo();
        callbackInfo->caption_ = caption;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (CaptionCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                CaptionCallbackInfo *callbackInfo = (CaptionCallbackInfo *)work->data;
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
            } 
        );

    } else {
        HILOG_ERROR("NotifyPropertyChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void ConfigListener::NotifyStateChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, bool enabled)
{
    if (GetStateType() == id) {
        NotifyStateChangedJS(GetEnv(),
            enabled, GetEventType(), GetHandler());
    }
}
void ConfigListener::NotifyPropertyChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, OHOS::AccessibilityConfig::CaptionProperty caption)
{
    if (GetStateType() == id) {
        NotifyPropertyChangedJS(GetEnv(),
            caption, GetEventType(), GetHandler());
    }
}

void ConfigListener::NotifyStringChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, const std::string &value)
{
    if (GetStateType() == id) {
        NotifyStringChanged2JSInner(GetEnv(),
            value, GetEventType(), GetHandler());
    }
}
void ConfigListener::NotifyUintChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, uint32_t value)
{
    if (GetStateType() == id) {
        NotifyUintChanged2JSInner(GetEnv(),
            value, GetEventType(), GetHandler());
    }
}
void ConfigListener::NotifyIntChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, int32_t value)
{
    if (GetStateType() == id) {
        NotifyIntChanged2JSInner(GetEnv(),
            value, GetEventType(), GetHandler());
    }
}

void ConfigListener::NotifyFloatChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, float value)
{
    if (GetStateType() == id) {
        NotifyFloatChanged2JSInner(GetEnv(),
            value, GetEventType(), GetHandler());
    }
}

//todo
void ConfigListener::NotifyStringChanged2JSInner(napi_env env, const std::string &value, const std::string &eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "styleChange")) {
        StateCallbackInfo *callbackInfo = new StateCallbackInfo();
        callbackInfo->stringValue_ = value;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (StateCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
                napi_value jsEvent;
                napi_create_string_utf8(callbackInfo->env_, callbackInfo->stringValue_.c_str(), callbackInfo->stringValue_.length(), &jsEvent);

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
                HILOG_INFO("NotifyStringChanged2JSInner napi_call_function result[%{public}d]", result);
                delete callbackInfo;
                callbackInfo = nullptr;
                delete work;
                work = nullptr;
            } 
        );

    } else {
        HILOG_ERROR("NotifyStringChanged2JSInner eventType[%s] is error", eventType.c_str());
    }
}
void ConfigListener::NotifyIntChanged2JSInner(napi_env env, int32_t value, const std::string &eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "styleChange")) {
        StateCallbackInfo *callbackInfo = new StateCallbackInfo();
        callbackInfo->int32Value_ = value;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (StateCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
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
            } 
        );

    } else {
        HILOG_ERROR("NotifyIntChanged2JSInner eventType[%s] is error", eventType.c_str());
    }
}
void ConfigListener::NotifyUintChanged2JSInner(napi_env env, uint32_t value, const std::string &eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "styleChange")) {
        StateCallbackInfo *callbackInfo = new StateCallbackInfo();
        callbackInfo->uint32Value_ = value;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (StateCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
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
            } 
        );

    } else {
        HILOG_ERROR("NotifyUintChanged2JSInner eventType[%s] is error", eventType.c_str());
    }
}
void ConfigListener::NotifyFloatChanged2JSInner(napi_env env, float value, const std::string &eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");

    if (!std::strcmp(eventType.c_str(), "styleChange")) {
        StateCallbackInfo *callbackInfo = new StateCallbackInfo();
        callbackInfo->floatValue_ = value;
        callbackInfo->env_ = env;
        callbackInfo->ref_ = handlerRef;
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env, &loop);
        uv_work_t *work = new uv_work_t;
        work->data = (StateCallbackInfo *)callbackInfo;

        uv_queue_work(
            loop,
            work,
            [](uv_work_t *work) {},
            [](uv_work_t *work, int status) {
                StateCallbackInfo *callbackInfo = (StateCallbackInfo *)work->data;
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
            } 
        );

    } else {
        HILOG_ERROR("NotifyFloatChanged2JSInner eventType[%s] is error", eventType.c_str());
    }
}