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
#include "napi_accessibility_system_ability_client.h"

#include "accessibility_state_event.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

napi_value NAccessibilityClient::aaCons_;
napi_value NAccessibilityClient::aaStyleCons_;

std::vector<std::shared_ptr<StateListener>> NAccessibilityClient::listeners_ = {};
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabled_ = AccessibilitySystemAbilityClient::GetInstance()->IsEnabled();
            HILOG_INFO("IsOpenAccessibility Executing enabled[%{public}d]", callbackInfo->enabled_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabled_ = AccessibilitySystemAbilityClient::GetInstance()->IsEnabled();
            HILOG_INFO("IsOpenTouchExploration Executing touchEnabled[%{public}d]", callbackInfo->touchEnabled_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->enabled_, &result[PARAM1]));
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::GetAbilityList(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = 3;
    napi_value parameters[3] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    std::string abilityTypes = GetStringFromNAPI(env, parameters[0]);
    std::string stateTypes = GetStringFromNAPI(env, parameters[1]);

    HILOG_INFO("abilityTypes[%{public}s]", abilityTypes.c_str());
    HILOG_INFO("stateTypes[%{public}s]", stateTypes.c_str());

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    callbackInfo->abilityTypes_ = ConvertStringToAccessibilityAbilityTypes(abilityTypes);
    callbackInfo->stateTypes_ = ConvertStringToAbilityStateTypes(stateTypes);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_TWO) {
        HILOG_DEBUG("GetAbilityList callback mode");
        napi_create_reference(env, parameters[ARGS_SIZE_TWO], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetAbilityList promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAbilityList", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->abilityList_ = AccessibilitySystemAbilityClient::GetInstance()->GetAbilityList(
                int(callbackInfo->abilityTypes_), callbackInfo->stateTypes_);
            HILOG_INFO("GetAbilityList Executing GetAbilityList[%{public}d]", callbackInfo->abilityList_.size());
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            HILOG_INFO("GetAbilityList ENTER ConvertAccessibleAbilityInfosToJS");
            ConvertAccessibleAbilityInfosToJS(env, result[PARAM1], callbackInfo->abilityList_);
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
    ConvertEventInfoJSToNAPI(env, parameters[0], callbackInfo->eventInfo_);
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->result_ =
                AccessibilitySystemAbilityClient::GetInstance()->SendEvent(callbackInfo->eventInfo_);
            HILOG_INFO("SendEvent result[%{public}d]", callbackInfo->result_);
        },
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->result_, &result[PARAM1]));
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
    NAPI_ASSERT(env, status == napi_ok, "Failed to get event type");

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>();
    stateListener->StartWork(env, 1, args);
    NAccessibilityClient::listeners_.push_back(stateListener);
    std::string eventType = GetStringFromNAPI(env, args[0]);
    AccessibilityStateEventType type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    if (std::strcmp(eventType.c_str(), "accessibility") == 0) {
        type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (std::strcmp(eventType.c_str(), "touchExplorer") == 0) {
        type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
    }
    AccessibilitySystemAbilityClient::GetInstance()->SubscribeStateObserver(stateListener, type);
    return nullptr;
}

napi_value NAccessibilityClient::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start  observer size%{public}d", __func__, NAccessibilityClient::listeners_.size());
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    std::string eventType = GetStringFromNAPI(env, args[0]);

    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("UnsubscribeState callback mode %{public}d", NAccessibilityClient::listeners_.size());
        NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
        napi_create_reference(env, args[1], 1, &callbackInfo->callback_);
        napi_value resource = nullptr;
        napi_create_string_utf8(env, "off", NAPI_AUTO_LENGTH, &resource);
        callbackInfo->eventType_ = eventType;
        callbackInfo->stateListener_ = NAccessibilityClient::listeners_;
        // async to sync start
        AccessibilityStateEventType type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        std::string eventType = callbackInfo->eventType_;
        if (std::strcmp(eventType.c_str(), "accessibility") == 0) {
            type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        } else if (std::strcmp(eventType.c_str(), "touchExplorer") == 0) {
            type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
        } else {
            HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        }
        int i = 0;
        for (auto observer : NAccessibilityClient::listeners_) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                callbackInfo->result_ =
                    AccessibilitySystemAbilityClient::GetInstance()->UnsubscribeStateObserver(observer, type);
                NAccessibilityClient::listeners_.erase(NAccessibilityClient::listeners_.begin() + i);
                HILOG_INFO("%{public}s unregister result%{public}d", __func__, callbackInfo->result_);
                break;
            }
            i++;
        }
        napi_value result[ARGS_SIZE_TWO] = {0};
        napi_value callback = 0;
        napi_value undefined = 0;
        napi_get_undefined(env, &undefined);
        napi_get_boolean(env, callbackInfo->result_, &result[PARAM1]);
        if (callbackInfo->callback_) {
            result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
            napi_get_reference_value(env, callbackInfo->callback_, &callback);
            napi_value returnVal;
            napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
            napi_delete_reference(env, callbackInfo->callback_);
        } else {
            HILOG_ERROR("SubscribeState end eventType[%{public}s] is error", callbackInfo->eventType_.c_str());
        }
        // async to sync end
    } else {
        AccessibilityStateEventType type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        if (std::strcmp(eventType.c_str(), "accessibility") == 0) {
            type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        } else if (std::strcmp(eventType.c_str(), "touchExplorer") == 0) {
            type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
        } else {
            HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        }
        int i = 0;
        for (auto observer : NAccessibilityClient::listeners_) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                AccessibilitySystemAbilityClient::GetInstance()->UnsubscribeStateObserver(observer, type);
                NAccessibilityClient::listeners_.erase(NAccessibilityClient::listeners_.begin() + i);
                break;
            }
            i++;
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
    if (std::strcmp(eventType_.c_str(), "accessibility") == 0) {
        type = EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (std::strcmp(eventType_.c_str(), "touchExplorer") == 0) {
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

void StateListener::NotifyJS(napi_env env, bool enabled, std::string stateType, std::string desc, napi_ref handlerRef)
{
    HILOG_INFO("start");
    napi_value jsEvent;
    napi_create_object(env, &jsEvent);

    napi_value eventType;
    napi_create_string_utf8(env, stateType.c_str(), NAPI_AUTO_LENGTH, &eventType);
    napi_set_named_property(env, jsEvent, "eventType", eventType);

    napi_value state;
    napi_get_boolean(env, enabled, &state);
    napi_set_named_property(env, jsEvent, "state", state);

    napi_value description;
    napi_create_string_utf8(env, desc.c_str(), NAPI_AUTO_LENGTH, &description);
    napi_set_named_property(env, jsEvent, "description", description);

    napi_value handler = nullptr;
    napi_value callResult;
    napi_get_reference_value(env, handlerRef, &handler);
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_call_function(env, undefined, handler, 1, &jsEvent, &callResult);
    int32_t result;
    napi_get_value_int32(env, callResult, &result);
    HILOG_INFO("NotifyJS napi_call_function result[%{public}d]", result);
}

void StateListener::OnStateChanged(const OHOS::Accessibility::AccessibilityStateEvent& stateEvent)
{
    HILOG_INFO("start");
    for (auto observer : NAccessibilityClient::listeners_) {
        if (observer->GetStateType() == stateEvent.GetEventType()) {
            if (stateEvent.GetEventResult()) {
                observer->NotifyJS(observer->GetEnv(),
                    true,
                    observer->GetEventType(),
                    stateEvent.GetEventMsg(),
                    observer->GetHandler());
            } else {
                observer->NotifyJS(observer->GetEnv(),
                    false,
                    observer->GetEventType(),
                    stateEvent.GetEventMsg(),
                    observer->GetHandler());
            }
        }
    }
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->captionProperty_ = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        HILOG_DEBUG("SetCaptionProperty callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetCaptionProperty promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetCaptionProperty", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setCaptionPropertyReturn_ = AccessibilitySystemAbilityClient::GetInstance()
                ->SetCaptionPropertyTojson(callbackInfo->captionProperty_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->captionState_ = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionState();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        HILOG_DEBUG("SetCaptionState callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetCaptionState promise mode");
        napi_status retStatus = napi_create_promise(env, &callbackInfo->deferred_, &promise);

        HILOG_DEBUG("napi_create_promise return: %{public}d", retStatus);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetCaptionState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setCaptionStateReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->SetCaptionStateTojson(callbackInfo->captionState_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabledState_ = AccessibilitySystemAbilityClient::GetInstance()->GetEnabledState();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::SetEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_get_value_bool(env, parameters[PARAM0], &callbackInfo->enabledState_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("SetEnabled callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetEnabled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetEnabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setEnabledReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->SetEnabled(callbackInfo->enabledState_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setEnabledReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("SetEnabled complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->touchGuideState_ = AccessibilitySystemAbilityClient::GetInstance()->GetTouchGuideState();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::SetTouchGuideState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_get_value_bool(env, parameters[PARAM0], &callbackInfo->touchGuideState_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("SetTouchGuideState callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetTouchGuideState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetTouchGuideState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setTouchGuideStateReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->SetTouchGuideState(callbackInfo->touchGuideState_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setTouchGuideStateReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("SetTouchGuideState complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->gestureState_ = AccessibilitySystemAbilityClient::GetInstance()->GetGestureState();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::SetGestureState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_get_value_bool(env, parameters[PARAM0], &callbackInfo->gestureState_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("SetGestureState callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetGestureState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetGestureState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setGestureStateReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->SetGestureState(callbackInfo->gestureState_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setGestureStateReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("SetGestureState complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->keyEventObserverState_ =
                AccessibilitySystemAbilityClient::GetInstance()->GetKeyEventObserverState();
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::SetKeyEventObserverState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    napi_get_value_bool(env, parameters[PARAM0], &callbackInfo->keyEventObserverState_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("SetKeyEventObserverState callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SetKeyEventObserverState promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetKeyEventObserverState", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setKeyEvenReturn_ = AccessibilitySystemAbilityClient::GetInstance()->SetKeyEventObserverState(
                callbackInfo->keyEventObserverState_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setKeyEvenReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("SetKeyEventObserverState complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
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

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->abilityList_ = AccessibilitySystemAbilityClient::GetInstance()->GetInstalledAbilities();

            HILOG_INFO("GetInstalled Executing GetInstalled[%{public}d]", callbackInfo->abilityList_.size());
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            HILOG_INFO("GetInstalled ENTER ConvertAccessibleAbilityInfosToJS");
            ConvertAccessibleAbilityInfosToJS(env, result[PARAM1], callbackInfo->abilityList_);
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}


napi_value NAccessibilityClient::GetExtentionEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("GetExtentionEnabled callback mode");
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetExtentionEnabled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetExtentionEnabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabledAbilities_ = AccessibilitySystemAbilityClient::GetInstance()->GetEnabledAbilities();

            HILOG_INFO("GetExtentionEnabled Executing GetExtentionEnabled[%{public}d]",
                callbackInfo->enabledAbilities_.size());
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_create_array(env, &result[PARAM1]);
            HILOG_INFO("GetExtentionEnabled ENTER ConvertAccessibleAbilityInfosToJS");
            ConvertEnabledAbilitiesToJS(env, result[PARAM1], callbackInfo->enabledAbilities_);
            if (callbackInfo->callback_) {
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
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::ExtentionEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    ConvertJSToEnabledAbilities(env, parameters[PARAM0], callbackInfo->enabledAbilities_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("ExtentionEnabled callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("ExtentionEnabled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ExtentionEnabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setExtentionReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->SetEnabledObj(callbackInfo->enabledAbilities_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setExtentionReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("ExtentionEnabled complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
        },
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityClient::ExtentionDisabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
    ConvertJSToEnabledAbilities(env, parameters[PARAM0], callbackInfo->enabledAbilities_);

    napi_value promise = nullptr;

    if (argc >= ARGS_SIZE_TWO) {
        HILOG_DEBUG("ExtentionDisabled callback mode");
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("ExtentionDisabled promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "ExtentionDisabled", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        // execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->setExtentionReturn_ =
                AccessibilitySystemAbilityClient::GetInstance()->DisableAbilities(callbackInfo->enabledAbilities_);
        },
        // execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            napi_get_boolean(env, callbackInfo->setExtentionReturn_, &result[PARAM1]);
            if (callbackInfo->callback_) {
                napi_value callback = 0;
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value returnVal;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
                napi_delete_reference(env, callbackInfo->callback_);
                HILOG_DEBUG("ExtentionDisabled complete function callback mode");
            } else {
                napi_status retStatus = napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                HILOG_DEBUG("napi_resolve_deferred return: %{public}d", retStatus);
            }
            napi_delete_async_work(env, callbackInfo->work_);
            delete callbackInfo;
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

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "NAccessibilityClient",
            NAPI_AUTO_LENGTH,
            NAccessibilityClient::AccessibleAbilityConstructor,
            nullptr,
            sizeof(captionsManagerDesc) / sizeof(captionsManagerDesc[0]),
            captionsManagerDesc,
            &NAccessibilityClient::aaCons_));
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

    callbackInfo->captionState_ = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionState();
    callbackInfo->captionProperty_ = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_new_instance(env, NAccessibilityClient::aaCons_, 0, nullptr, &result);

    napi_value keyCode;
    napi_get_boolean(env, callbackInfo->captionState_, &keyCode);
    napi_set_named_property(env, result, "enabled", keyCode);

    napi_status status = napi_new_instance(env, NAccessibilityClient::aaStyleCons_, 0, nullptr, &keyCode);
    HILOG_INFO("%{public}s napi_new_instance() = %{public}d ", __func__, (int32_t)status);
    ConvertCaptionPropertyToJS(env, keyCode, callbackInfo->captionProperty_);
    napi_set_named_property(env, result, "style", keyCode);

    delete callbackInfo;
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
        HILOG_INFO("%{public}s captionState = %{public}s", __func__, captionState?"True":"False");

        bool returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionStateTojson(captionState);
        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionStateEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    napi_value captionStateEnabled = nullptr;

    bool captionState = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionState();
    napi_get_boolean(env, captionState, &captionStateEnabled);

    HILOG_INFO("%{public}s captionState = %{public}s", __func__, captionState ? "True" : "False");

    return captionStateEnabled;
}

napi_value NAccessibilityClient::SetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    OHOS::Accessibility::CaptionProperty captionProperty = {};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        ConvertObjToCaptionProperty(env, parameters[PARAM0], &captionProperty);
        bool returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);
        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
			__func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty {};
    napi_value captionStyle = nullptr;

    captionProperty = AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_status status = napi_new_instance(env, NAccessibilityClient::aaStyleCons_, 0, nullptr, &captionStyle);
    HILOG_INFO("%{public}s napi_new_instance() = %{public}d ", __func__, (int32_t)status);
    ConvertCaptionPropertyToJS(env, captionStyle, captionProperty);

    HILOG_INFO("%{public}s end", __func__);

    return captionStyle;
}

napi_value NAccessibilityClient::RegisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "Failed to get event type");

    std::shared_ptr<CaptionListener> captionListener = std::make_shared<CaptionListener>();
    captionListener->StartWork(env, 1, args);
    NAccessibilityClient::captionListeners_.push_back(captionListener);
    std::string eventType = GetStringFromNAPI(env, args[0]);
    CaptionObserverType type = CaptionObserverType::CAPTION_ENABLE;
    if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
        type = CaptionObserverType::CAPTION_ENABLE;
    } else if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
        type = CaptionObserverType::CAPTION_PROPERTY;
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
    }
    bool result = AccessibilitySystemAbilityClient::GetInstance()->AddCaptionListener(captionListener, type);
    napi_value ret = {0};
    napi_get_boolean(env, result, &ret);
    return ret;
}

napi_value NAccessibilityClient::DeregisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start observer size%{public}d", __func__, NAccessibilityClient::captionListeners_.size());
    bool retValue = true;
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    std::string eventType = GetStringFromNAPI(env, args[0]);

    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("DeregisterCaptionStateCallback callback mode %{public}d",
            NAccessibilityClient::captionListeners_.size());
        NAccessibilitySystemAbilityClient* callbackInfo = new NAccessibilitySystemAbilityClient();
        napi_create_reference(env, args[1], 1, &callbackInfo->callback_);
        napi_value resource = nullptr;
        napi_create_string_utf8(env, "DeregisterCaptionStateCallback", NAPI_AUTO_LENGTH, &resource);
        callbackInfo->eventType_ = eventType;
        callbackInfo->captionListener_ = NAccessibilityClient::captionListeners_;
        // async to sync start
        CaptionObserverType type = CaptionObserverType::CAPTION_ENABLE;
        std::string eventType = callbackInfo->eventType_;
        if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
            type = CaptionObserverType::CAPTION_ENABLE;
        } else if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
            type = CaptionObserverType::CAPTION_PROPERTY;
        } else {
            HILOG_ERROR("DeregisterCaptionStateCallback eventType[%{public}s] is error", eventType.c_str());
        }
        int i = 0;
        for (auto observer : NAccessibilityClient::captionListeners_) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                callbackInfo->result_ =
                    AccessibilitySystemAbilityClient::GetInstance()->DeleteCaptionListener(observer, type);
                NAccessibilityClient::captionListeners_.erase(NAccessibilityClient::captionListeners_.begin() + i);
                HILOG_INFO("%{public}s unregister result%{public}d", __func__, callbackInfo->result_);
                retValue = retValue & callbackInfo->result_;
                break;
            }
            i++;
        }
        napi_value result[ARGS_SIZE_TWO] = {0};
        napi_value callback = 0;
        napi_value undefined = 0;
        napi_get_undefined(env, &undefined);
        napi_get_boolean(env, callbackInfo->result_, &result[PARAM1]);
        if (callbackInfo->callback_) {
            napi_get_boolean(env, callbackInfo->result_, &result[PARAM0]); // maby rework

            napi_get_reference_value(env, callbackInfo->callback_, &callback);
            napi_value returnVal;
            napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
            napi_delete_reference(env, callbackInfo->callback_);
        } else {
            HILOG_ERROR("DeregisterCaptionStateCallback end eventType[%{public}s] is error",
                callbackInfo->eventType_.c_str());
        }
        // async to sync end
    } else {
        CaptionObserverType type = CaptionObserverType::CAPTION_ENABLE;
        if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
            type = CaptionObserverType::CAPTION_ENABLE;
        } else if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
            type = CaptionObserverType::CAPTION_PROPERTY;
        } else {
            HILOG_ERROR("DeregisterCaptionStateCallback eventType[%{public}s] is error", eventType.c_str());
        }
        int i = 0;
        for (auto observer : NAccessibilityClient::captionListeners_) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                retValue = retValue &
                    AccessibilitySystemAbilityClient::GetInstance()->DeleteCaptionListener(observer, type);
                NAccessibilityClient::captionListeners_.erase(NAccessibilityClient::captionListeners_.begin() + i);
                HILOG_INFO("%{public}s unregister result%{public}d", __func__, retValue);
                break;
            }
            i++;
        }
    }
    napi_value ret = 0;
    napi_get_boolean(env, retValue, &ret);
    return ret;
}

CaptionListener::CaptionListener()
{
}

CaptionObserverType CaptionListener::GetStateType()
{
    HILOG_INFO("start");
    CaptionObserverType type = CaptionObserverType::CAPTION_ENABLE;
    if (std::strcmp(eventType_.c_str(), "enableChange") == 0) {
        type = CaptionObserverType::CAPTION_ENABLE;
    } else if (std::strcmp(eventType_.c_str(), "styleChange") == 0) {
        type = CaptionObserverType::CAPTION_PROPERTY;
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

    if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
        napi_value jsEvent;
        napi_get_boolean(env, enabled, &jsEvent);

        napi_value handler = nullptr;
        napi_value callResult;
        napi_get_reference_value(env, handlerRef, &handler);
        napi_value undefined = 0;
        napi_get_undefined(env, &undefined);
        napi_call_function(env, undefined, handler, 1, &jsEvent, &callResult);
        int32_t result;
        napi_get_value_int32(env, callResult, &result);
        HILOG_INFO("NotifyJS napi_call_function result[%{public}d]", result);
    } else {
        HILOG_ERROR("NotifyStateChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void CaptionListener::OnCaptionStateChanged(const bool& enable)
{
    HILOG_INFO("start");
    for (auto observer : NAccessibilityClient::captionListeners_) {
        if (observer->GetStateType() == CaptionObserverType::CAPTION_ENABLE) {
            observer->NotifyStateChangedJS(observer->GetEnv(),
                enable, observer->GetEventType(), observer->GetHandler());
        }
    }
}

void CaptionListener::NotifyPropertyChangedJS(napi_env env,
    CaptionProperty caption, std::string eventType, napi_ref handlerRef)
{
    HILOG_INFO("start");
    napi_value jsEvent;
    napi_create_object(env, &jsEvent);

    if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
        ConvertCaptionPropertyToJS(env, jsEvent, caption);

        napi_value handler = nullptr;
        napi_value callResult;
        napi_get_reference_value(env, handlerRef, &handler);
        napi_value undefined = 0;
        napi_get_undefined(env, &undefined);
        napi_call_function(env, undefined, handler, 1, &jsEvent, &callResult);
        int32_t result;
        napi_get_value_int32(env, callResult, &result);
        HILOG_INFO("NotifyPropertyChangedJS napi_call_function result[%{public}d]", result);
    } else {
        HILOG_ERROR("NotifyPropertyChangedJS eventType[%s] is error", eventType.c_str());
    }
}

void CaptionListener::OnCaptionPropertyChanged(const CaptionProperty& caption)
{
    HILOG_INFO("start");
    for (auto observer : NAccessibilityClient::captionListeners_) {
        if (observer->GetStateType() == CaptionObserverType::CAPTION_PROPERTY) {
            observer->NotifyPropertyChangedJS(observer->GetEnv(),
                caption, observer->GetEventType(), observer->GetHandler());
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

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "NAccessibilityClientStyle",
            NAPI_AUTO_LENGTH,
            NAccessibilityClient::AccessibleAbilityConstructorStyle,
            nullptr,
            sizeof(captionsStyleDesc) / sizeof(captionsStyleDesc[0]),
            captionsStyleDesc,
            &NAccessibilityClient::aaStyleCons_));
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

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_string_utf8(env, captionProperty.GetFontFamily().c_str(), NAPI_AUTO_LENGTH, &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input FontFamily
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("%{public}s FontFamily = %{public}s", __func__, outBuffer);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetFontFamily(std::string(outBuffer));
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionsFontScale(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_int32(env, captionProperty.GetFontScale(), &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input FontScale
        int32_t num = 0;
        napi_get_value_int32(env, parameters[PARAM0], &num);
        HILOG_INFO("%{public}s FontScale = %{public}d", __func__, num);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetFontScale(num);
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionFrontColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_string_utf8(env, captionProperty.GetFontColor().c_str(), NAPI_AUTO_LENGTH, &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input frontColor
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("%{public}s frontColor = %{public}s", __func__, outBuffer);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetFontColor(std::string(outBuffer));
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionFontEdgeType(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_string_utf8(env, captionProperty.GetFontEdgeType().c_str(), NAPI_AUTO_LENGTH, &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input FontEdgeType
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("%{public}s FontEdgeType = %{public}s", __func__, outBuffer);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetFontEdgeType(std::string(outBuffer));
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionBackgroundColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_string_utf8(env, captionProperty.GetBackgroundColor().c_str(), NAPI_AUTO_LENGTH, &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input BackgroundColor
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("%{public}s BackgroundColor = %{public}s", __func__, outBuffer);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetBackgroundColor(std::string(outBuffer));
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}

napi_value NAccessibilityClient::GetCaptionWindowColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("start");

    OHOS::Accessibility::CaptionProperty captionProperty =
        AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

    napi_value returnValue = nullptr;
    napi_create_string_utf8(env, captionProperty.GetWindowColor().c_str(), NAPI_AUTO_LENGTH, &returnValue);

    HILOG_INFO("%{public}s end", __func__);

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

        // get input WindowColor
        char outBuffer[CHAE_BUFFER_MAX + 1] = {0};
        size_t outSize = 0;
        napi_get_value_string_utf8(env, parameters[PARAM0], outBuffer, CHAE_BUFFER_MAX, &outSize);
        HILOG_INFO("%{public}s WindowColor = %{public}s", __func__, outBuffer);

        // get CaptionProperty
        OHOS::Accessibility::CaptionProperty captionProperty =
            AccessibilitySystemAbilityClient::GetInstance()->GetCaptionProperty();

        // change the input info and then set the CaptionProperty
        captionProperty.SetWindowColor(std::string(outBuffer));
        returnVal = AccessibilitySystemAbilityClient::GetInstance()->SetCaptionPropertyTojson(captionProperty);

        HILOG_INFO("%{public}s SetCaptionPropertyTojson() return = %{public}s",
            __func__, returnVal ? "True" : "False");
    } else {
        HILOG_INFO("%{public}s argc size Error", __func__);
    }

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    return ret;
}