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

#include "napi_accessibility_system_ability_client.h"

#include <uv.h>
#include "accessibility_state_event.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

thread_local napi_ref NAccessibilityClient::aaConsRef_;
thread_local napi_ref NAccessibilityClient::aaStyleConsRef_;

std::map<std::string, std::vector<std::shared_ptr<StateListener>>> NAccessibilityClient::stateListeners_ = {};
std::vector<std::shared_ptr<CaptionListener>> NAccessibilityClient::captionListeners_ = {};

napi_value NAccessibilityClient::IsOpenAccessibility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    if (argc > 0) {
        HILOG_DEBUG("IsOpenAccessibility callback mode");
        napi_create_reference(env, argv, 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("IsOpenAccessibility promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "IsOpenAccessibility", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->enabled_ = instance->IsEnabled();
                HILOG_INFO("IsOpenAccessibility Executing enabled[%{public}d]", callbackInfo->enabled_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->enabled_, &result[PARAM1]));
            HILOG_INFO("IsOpenAccessibility completed enabled[%{public}d]", callbackInfo->enabled_);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo, &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::IsOpenTouchExploration(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    if (argc > 0) {
        HILOG_DEBUG("IsOpenTouchExploration callback mode");
        napi_create_reference(env, argv, 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("IsOpenTouchExploration promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "IsOpenTouchExploration", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->touchEnabled_ = instance->IsTouchExplorationEnabled();
                HILOG_INFO("IsOpenTouchExploration Executing touchEnabled[%{public}d]", callbackInfo->touchEnabled_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->touchEnabled_, &result[PARAM1]));
            HILOG_INFO("IsOpenTouchExploration completed touchEnabled_[%{public}d]", callbackInfo->touchEnabled_);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetAbilityList(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value parameters[3] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    std::string abilityTypes = GetStringFromNAPI(env, parameters[0]);
    std::string stateTypes = GetStringFromNAPI(env, parameters[1]);
    HILOG_INFO("abilityTypes[%{public}s] stateTypes[%{public}s]", abilityTypes.c_str(), stateTypes.c_str());

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    callbackInfo->abilityTypes_ = ConvertStringToAccessibilityAbilityTypes(abilityTypes);
    callbackInfo->stateTypes_ = ConvertStringToAbilityStateTypes(stateTypes);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_TWO) {
        napi_create_reference(env, parameters[ARGS_SIZE_TWO], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAbilityList", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->result_ = instance->GetAbilityList(callbackInfo->abilityTypes_,
                    callbackInfo->stateTypes_, callbackInfo->abilityList_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            ConvertAccessibleAbilityInfosToJS(env, result[PARAM1], callbackInfo->abilityList_);
            if (callbackInfo->callback_ && callbackInfo->result_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::SendEvent(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    callbackInfo->result_ = ConvertEventInfoJSToNAPI(env, parameters[0], callbackInfo->eventInfo_);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("SendEvent callback mode");
        napi_create_reference(env, parameters[1], 1, &callbackInfo->callback_);
    } else {
        HILOG_DEBUG("SendEvent promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SendEvent", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (callbackInfo->result_ && instance) {
                callbackInfo->result_ = instance->SendEvent(callbackInfo->eventInfo_);
            }
            HILOG_INFO("SendEvent result[%{public}d]", callbackInfo->result_);
        },
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value ret = 0;
            napi_get_undefined(env, &undefined);
            napi_get_undefined(env, &ret);
            if (callbackInfo->callback_) {
                if (callbackInfo->result_) {
                    result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                } else {
                    result[PARAM0] = GetErrorValue(env, CODE_FAILED);
                }
                result[PARAM1] = ret;
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->result_) {
                    napi_resolve_deferred(env, callbackInfo->deferred_, undefined);
                } else {
                    napi_reject_deferred(env, callbackInfo->deferred_, undefined);
                }
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

napi_value NAccessibilityClient::SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get event type");
        return nullptr;
    }

    std::string eventType = GetStringFromNAPI(env, args[0]);
    uint32_t type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    if (!std::strcmp(eventType.c_str(), "accessibilityStateChange")) {
        type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (!std::strcmp(eventType.c_str(), "touchGuideStateChange")) {
        type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        return nullptr;
    }

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>();
    stateListener->StartWork(env, 1, args);

    NAccessibilityClient::stateListeners_[eventType].push_back(stateListener);
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance) {
        instance->SubscribeStateObserver(stateListener, type);
    }
    return nullptr;
}

napi_value NAccessibilityClient::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    std::string eventType = GetStringFromNAPI(env, args[PARAM0]);
    uint32_t type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    if (!std::strcmp(eventType.c_str(), "accessibilityStateChange")) {
        type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (!std::strcmp(eventType.c_str(), "touchGuideStateChange")) {
        type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
    } else {
        HILOG_ERROR("UnsubscribeState eventType[%{public}s] is error", eventType.c_str());
        return nullptr;
    }
    HILOG_INFO("observer size%{public}zu", NAccessibilityClient::stateListeners_[eventType].size());
    bool result = true;
    for (auto it = NAccessibilityClient::stateListeners_[eventType].begin();
        it != NAccessibilityClient::stateListeners_[eventType].end();) {
        std::shared_ptr<StateListener> observer= *it;
        if (observer->GetEnv() == env) {
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                result = instance->UnsubscribeStateObserver(observer, type);
                it = NAccessibilityClient::stateListeners_[eventType].erase(it);
                HILOG_INFO("UnsubscribeState result%{public}d", result);
            }
        } else {
            it++;
        }
    }

    return nullptr;
}

StateListener::StateListener()
{}

AccessibilityStateEventType StateListener::GetStateType()
{
    HILOG_INFO("start");
    AccessibilityStateEventType type = EVENT_ACCESSIBILITY_STATE_CHANGED;
    if (!std::strcmp(eventType_.c_str(), "accessibility")) {
        type = EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (!std::strcmp(eventType_.c_str(), "touchExplorer")) {
        type = EVENT_TOUCH_GUIDE_STATE_CHANGED;
    } else {
        HILOG_ERROR("SubscribeState eventType[%s] is error", eventType_.c_str());
    }
    return type;
}

napi_value StateListener::StartWork(napi_env env, size_t functionIndex, napi_value (&args)[START_WORK_ARGS_SIZE])
{
    HILOG_INFO("start");
    eventType_ = GetStringFromNAPI(env, args[0]);
    napi_create_reference(env, args[functionIndex], 1, &handlerRef_);
    env_ = env;
    napi_value result = {0};
    return result;
}

void StateListener::NotifyJS(napi_env env, bool state, napi_ref handlerRef)
{
    HILOG_INFO("start");
    
    StateCallbackInfo *callbackInfo = new StateCallbackInfo();
    callbackInfo->state_ = state;
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
            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_value jsEvent = nullptr;
            napi_get_boolean(callbackInfo->env_, callbackInfo->state_, &jsEvent);

            napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
            delete callbackInfo;
            callbackInfo = nullptr;
            delete work;
            work = nullptr;
        });
}

void StateListener::OnStateChanged(const bool state)
{
    HILOG_INFO("start");
    for (auto &observer : NAccessibilityClient::stateListeners_[GetEventType()]) {
        if (observer.get() == this) {
            HILOG_INFO("observer found");
            observer->NotifyJS(GetEnv(), state, GetHandler());
            return;
        }
    }
    HILOG_WARN("observer not found");
}

napi_value NAccessibilityClient::GetCaptionProperty(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetCaptionProperty callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetCaptionProperty promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetCaptionProperty", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->captionProperty_ = instance->GetCaptionProperty();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_object(env, &result[PARAM1]);
            HILOG_INFO("GetAbilityList ENTER ConvertCaptionPropertyToJS");
            ConvertCaptionPropertyToJS(env, result[PARAM1], callbackInfo->captionProperty_);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::SetCaptionProperty(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    ConvertObjToCaptionProperty(env, parameters[PARAM0], &callbackInfo->captionProperty_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetCaptionProperty", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->setCaptionPropertyReturn_ = instance->SetCaptionPropertyTojson(
                    callbackInfo->captionProperty_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setCaptionPropertyReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("SetCaptionProperty complete function callback mode");
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

napi_value NAccessibilityClient::GetCaptionState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetCaptionState callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetCaptionState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetCaptionState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->captionState_ = instance->GetCaptionState();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->captionState_, &result[PARAM1]);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::SetCaptionState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_get_value_bool(env, parameters[PARAM0], &callbackInfo->captionState_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetCaptionState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->setCaptionStateReturn_ = instance->SetCaptionStateTojson(callbackInfo->captionState_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setCaptionStateReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_status retStatus =
                    napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("napi_call_function return: %{public}d", retStatus);
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

napi_value NAccessibilityClient::GetEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetEnabled callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetEnabled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetEnabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->enabledState_ = instance->GetEnabledState();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->enabledState_, &result[PARAM1]);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetTouchGuideState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetTouchGuideState callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetTouchGuideState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetTouchGuideState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->touchGuideState_ = instance->GetTouchGuideState();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->touchGuideState_, &result[PARAM1]);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetGestureState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetGestureState callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetGestureState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetGestureState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->gestureState_ = instance->GetGestureState();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->gestureState_, &result[PARAM1]);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetKeyEventObserverState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetKeyEventObserverState callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetKeyEventObserverState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetKeyEventObserverState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->keyEventObserverState_ = instance->GetKeyEventObserverState();
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            napi_get_boolean(env, callbackInfo->keyEventObserverState_, &result[PARAM1]);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetInstalled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetInstalled callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetInstalled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetInstalled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->result_ = instance->GetInstalledAbilities(callbackInfo->abilityList_);
            }
            HILOG_INFO("GetInstalled Executing GetInstalled[%{public}zu]", callbackInfo->abilityList_.size());
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            ConvertAccessibleAbilityInfosToJS(env, result[PARAM1], callbackInfo->abilityList_);
            if (callbackInfo->callback_ && callbackInfo->result_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

napi_value NAccessibilityClient::GetExtensionEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetExtensionEnabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto instance = AccessibilitySystemAbilityClient::GetInstance();
            if (instance) {
                callbackInfo->result_ = instance->GetEnabledAbilities(callbackInfo->enabledAbilities_);
            }
            HILOG_INFO("GetExtensionEnabled Executing GetExtensionEnabled[%{public}zu]",
                callbackInfo->enabledAbilities_.size());
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            HILOG_INFO("GetExtensionEnabled ENTER ConvertAccessibleAbilityInfosToJS");
            ConvertStringVecToJS(env, result[PARAM1], callbackInfo->enabledAbilities_);
            if (callbackInfo->callback_ && callbackInfo->result_) {
                napi_value callback = 0;
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
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

void NAccessibilityClient::DefineJSCaptionsManager(napi_env env)
{
    napi_property_descriptor captionsManagerDesc[] = {
        DECLARE_NAPI_GETTER_SETTER("enabled",
            NAccessibilityClient::GetCaptionStateEnabled, NAccessibilityClient::SetCaptionStateEnabled),
        DECLARE_NAPI_GETTER_SETTER("style",
            NAccessibilityClient::GetCaptionStyle, NAccessibilityClient::SetCaptionStyle),
        DECLARE_NAPI_FUNCTION("on", NAccessibilityClient::RegisterCaptionStateCallback),
        DECLARE_NAPI_FUNCTION("off", NAccessibilityClient::DeregisterCaptionStateCallback),
    };

    napi_value aaCons = nullptr;
    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "NAccessibilityClient",
            NAPI_AUTO_LENGTH,
            NAccessibilityClient::AccessibleAbilityConstructor,
            nullptr,
            sizeof(captionsManagerDesc) / sizeof(captionsManagerDesc[0]),
            captionsManagerDesc,
            &aaCons));

    napi_create_reference(env, aaCons, 1, &NAccessibilityClient::aaConsRef_);
}

napi_value NAccessibilityClient::AccessibleAbilityConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}


napi_value NAccessibilityClient::GetCaptionsManager(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_value result = 0;

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return result;
    }
    callbackInfo->captionState_ = instance->GetCaptionState();
    callbackInfo->captionProperty_ = instance->GetCaptionProperty();

    napi_value aaCons = nullptr;
    napi_get_reference_value(env, NAccessibilityClient::aaConsRef_, &aaCons);
    napi_new_instance(env, aaCons, 0, nullptr, &result);

    napi_value keyCode;
    napi_get_boolean(env, callbackInfo->captionState_, &keyCode);
    napi_set_named_property(env, result, "enabled", keyCode);

    napi_value aaStyleCons = nullptr;
    napi_get_reference_value(env, NAccessibilityClient::aaStyleConsRef_, &aaStyleCons);
    napi_status status = napi_new_instance(env, aaStyleCons, 0, nullptr, &keyCode);
    HILOG_INFO("napi_new_instance() = %{public}d ", (int32_t)status);
    ConvertCaptionPropertyToJS(env, keyCode, callbackInfo->captionProperty_);
    napi_set_named_property(env, result, "style", keyCode);

    delete callbackInfo;
    callbackInfo = nullptr;
    return result;
}

napi_value NAccessibilityClient::SetCaptionStateEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    bool captionState = false;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        napi_get_value_bool(env, parameters[PARAM0], &captionState);
        HILOG_INFO("captionState = %{public}s", captionState?"True":"False");

        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            bool returnVal = instance->SetCaptionStateTojson(captionState);
            HILOG_INFO("SetCaptionPropertyTojson() return = %{public}s", returnVal ? "True" : "False");
        }
    } else {
        HILOG_INFO("argc size Error");
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionStateEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value captionStateEnabled = nullptr;

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return captionStateEnabled;
    }
    bool captionState = instance->GetCaptionState();
    napi_get_boolean(env, captionState, &captionStateEnabled);

    HILOG_INFO("captionState = %{public}s", captionState ? "True" : "False");

    return captionStateEnabled;
}

napi_value NAccessibilityClient::SetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        ConvertObjToCaptionProperty(env, parameters[PARAM0], &captionProperty);
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            bool returnVal = instance->SetCaptionPropertyTojson(captionProperty);
            HILOG_INFO("SetCaptionPropertyTojson() return = %{public}s", returnVal ? "True" : "False");
        }
    } else {
        HILOG_INFO("argc size Error");
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
    napi_value captionStyle = nullptr;

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return captionStyle;
    }
    captionProperty = instance->GetCaptionProperty();

    napi_value aaStyleCons = nullptr;
    napi_get_reference_value(env, NAccessibilityClient::aaStyleConsRef_, &aaStyleCons);
    napi_status status = napi_new_instance(env, aaStyleCons, 0, nullptr, &captionStyle);
    HILOG_INFO("napi_new_instance() = %{public}d ", (int32_t)status);
    ConvertCaptionPropertyToJS(env, captionStyle, captionProperty);

    HILOG_INFO("end");

    return captionStyle;
}

napi_value NAccessibilityClient::RegisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get event type");
        return nullptr;
    }

    std::string eventType = GetStringFromNAPI(env, args[0]);
    OHOS::AccessibilityConfig::CaptionObserverType type =
        OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    if (!std::strcmp(eventType.c_str(), "enableChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    } else if (!std::strcmp(eventType.c_str(), "styleChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY;
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        return nullptr;
    }

    std::shared_ptr<CaptionListener> captionListener = std::make_shared<CaptionListener>();
    captionListener->StartWork(env, 1, args);
    NAccessibilityClient::captionListeners_.push_back(captionListener);

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance) {
        instance->AddCaptionListener(captionListener, type);
    }

    return nullptr;
}

napi_value NAccessibilityClient::DeregisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start observer size%{public}zu", NAccessibilityClient::captionListeners_.size());
    bool retValue = true;
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    std::string eventType = GetStringFromNAPI(env, args[0]);

    OHOS::AccessibilityConfig::CaptionObserverType type =
        OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    if (!std::strcmp(eventType.c_str(), "enableChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    } else if (!std::strcmp(eventType.c_str(), "styleChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY;
    } else {
        HILOG_ERROR("DeregisterCaptionStateCallback eventType[%{public}s] is error", eventType.c_str());
        return nullptr;
    }
    for (auto it = NAccessibilityClient::captionListeners_.begin();
        it != NAccessibilityClient::captionListeners_.end();) {
        std::shared_ptr<CaptionListener> observer= *it;
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance && observer->GetEnv() == env && !strcmp(observer->GetEventType().c_str(), eventType.c_str())) {
            retValue = retValue && instance->RemoveCaptionListener(observer, type);
            it = NAccessibilityClient::captionListeners_.erase(it);
            HILOG_INFO("unregister result%{public}d", retValue);
        } else {
            it++;
        }
    }

    return nullptr;
}

CaptionListener::CaptionListener()
{
}

OHOS::AccessibilityConfig::CaptionObserverType CaptionListener::GetStateType()
{
    HILOG_INFO("start");
    OHOS::AccessibilityConfig::CaptionObserverType type =
        OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    if (!std::strcmp(eventType_.c_str(), "enableChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE;
    } else if (!std::strcmp(eventType_.c_str(), "styleChange")) {
        type = OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY;
    } else {
        HILOG_ERROR("GetStateType eventType[%s] is error", eventType_.c_str());
    }
    return type;
}

napi_value CaptionListener::StartWork(napi_env env, size_t functionIndex, napi_value (&args)[START_WORK_ARGS_SIZE])
{
    HILOG_INFO("start");
    eventType_ = GetStringFromNAPI(env, args[0]);
    napi_create_reference(env, args[functionIndex], 1, &handlerRef_);
    env_ = env;
    napi_value result = {0};
    return result;
}

void CaptionListener::NotifyStateChangedJS(napi_env env, bool enabled, std::string eventType, napi_ref handlerRef)
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
    } else {
        HILOG_ERROR("NotifyStateChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void CaptionListener::OnStateChanged(const bool& enable)
{
    HILOG_INFO("start");
    for (auto &observer : NAccessibilityClient::captionListeners_) {
        if (observer->GetStateType() == OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE &&
            observer.get() == this) {
            observer->NotifyStateChangedJS(
                observer->GetEnv(), enable, observer->GetEventType(), observer->GetHandler());
        }
    }
}

void CaptionListener::NotifyPropertyChangedJS(napi_env env,
    const AccessibilityConfig::CaptionProperty &caption, const std::string &eventType, napi_ref handlerRef)
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
    } else {
        HILOG_ERROR("NotifyPropertyChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void CaptionListener::OnPropertyChanged(const OHOS::AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_INFO("start");
    for (auto &observer : NAccessibilityClient::captionListeners_) {
        if (observer->GetStateType() == OHOS::AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY &&
            observer.get() == this) {
            observer->NotifyPropertyChangedJS(
                observer->GetEnv(), caption, observer->GetEventType(), observer->GetHandler());
        }
    }
}

void NAccessibilityClient::DefineJSCaptionsStyle(napi_env env)
{
    napi_property_descriptor captionsStyleDesc[] = {
        DECLARE_NAPI_GETTER_SETTER("fontFamily",
            NAccessibilityClient::GetCaptionsFontFamily, NAccessibilityClient::SetCaptionsFontFamily),
        DECLARE_NAPI_GETTER_SETTER("fontScale",
            NAccessibilityClient::GetCaptionsFontScale, NAccessibilityClient::SetCaptionsFontScale),
        DECLARE_NAPI_GETTER_SETTER("fontColor",
            NAccessibilityClient::GetCaptionFrontColor, NAccessibilityClient::SetCaptionFrontColor),
        DECLARE_NAPI_GETTER_SETTER("fontEdgeType",
            NAccessibilityClient::GetCaptionFontEdgeType, NAccessibilityClient::SetCaptionFontEdgeType),
        DECLARE_NAPI_GETTER_SETTER("backgroundColor",
            NAccessibilityClient::GetCaptionBackgroundColor, NAccessibilityClient::SetCaptionBackgroundColor),
        DECLARE_NAPI_GETTER_SETTER("windowColor",
            NAccessibilityClient::GetCaptionWindowColor, NAccessibilityClient::SetCaptionWindowColor),
    };

    napi_value aaStyleCons = nullptr;

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "NAccessibilityClientStyle",
            NAPI_AUTO_LENGTH,
            NAccessibilityClient::AccessibleAbilityConstructorStyle,
            nullptr,
            sizeof(captionsStyleDesc) / sizeof(captionsStyleDesc[0]),
            captionsStyleDesc,
            &aaStyleCons));

    napi_create_reference(env, aaStyleCons, 1, &NAccessibilityClient::aaStyleConsRef_);
}

napi_value NAccessibilityClient::AccessibleAbilityConstructorStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value NAccessibilityClient::GetCaptionsFontFamily(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    napi_create_string_utf8(env, captionProperty.GetFontFamily().c_str(), NAPI_AUTO_LENGTH, &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionsFontFamily(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        // Get input FontFamily
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("FontFamily = %{public}s", outBuffer);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontFamily(std::string(outBuffer));
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }
        HILOG_INFO("SetCaptionPropertyTojson() return = %{public}s", returnVal ? "True" : "False");
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionsFontScale(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    napi_create_int32(env, captionProperty.GetFontScale(), &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionsFontScale(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        // Get input FontScale
        int32_t num = 0;
        napi_get_value_int32(env, parameters[PARAM0], &num);
        HILOG_INFO("FontScale = %{public}d", num);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
        OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontScale(num);
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }

        HILOG_INFO("SetCaptionPropertyTojson() return = %{public}s", returnVal ? "True" : "False");
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionFrontColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    uint32_t color = captionProperty.GetFontColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionFrontColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontColor(color);
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionFontEdgeType(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    napi_create_string_utf8(env, captionProperty.GetFontEdgeType().c_str(), NAPI_AUTO_LENGTH, &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionFontEdgeType(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};

    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        // Get input FontEdgeType
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("FontEdgeType = %{public}s", outBuffer);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontEdgeType(std::string(outBuffer));
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }
        HILOG_INFO("SetCaptionPropertyTojson() return = %{public}s", returnVal ? "True" : "False");
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionBackgroundColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    uint32_t color = captionProperty.GetBackgroundColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionBackgroundColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetBackgroundColor(color);
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionWindowColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value returnValue = nullptr;
    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return returnValue;
    }
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
    uint32_t color = captionProperty.GetWindowColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    HILOG_INFO("end");
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionWindowColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool returnVal = false;
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        // Get CaptionProperty
        auto instance = AccessibilitySystemAbilityClient::GetInstance();
        if (instance) {
            OHOS::AccessibilityConfig::CaptionProperty captionProperty = instance->GetCaptionProperty();
            // Change the input info and then set the CaptionProperty
            captionProperty.SetWindowColor(color);
            returnVal = instance->SetCaptionPropertyTojson(captionProperty);
        }
    } else {
        HILOG_INFO("argc size Error");
    }
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}