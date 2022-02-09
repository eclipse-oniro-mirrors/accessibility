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
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"
#include "accessibility_state_event.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

std::vector<std::shared_ptr<StateListener>> g_states;
napi_value IsOpenAccessibility(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start", __func__);
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    NAccessibilitySystemAbilityClient *callbackInfo = new NAccessibilitySystemAbilityClient();
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
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabled_ = AccessibilitySystemAbilityClient::GetInstance()->IsEnabled();
            HILOG_INFO("IsOpenAccessibility Executing enabled[%{public}d]", callbackInfo->enabled_);
        },
        [](napi_env env, napi_status status, void* data) {   //execute the complete function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->enabled_, &result[PARAM1]));
            HILOG_INFO("IsOpenAccessibility completed enabled[%{public}d]", callbackInfo->enabled_);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value return_val;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &return_val);
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

napi_value IsOpenTouchExploration(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start", __func__);
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    NAccessibilitySystemAbilityClient *callbackInfo = new NAccessibilitySystemAbilityClient();
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
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->enabled_ = AccessibilitySystemAbilityClient::GetInstance()->IsEnabled();
            HILOG_INFO("IsOpenTouchExploration Executing touchEnabled[%{public}d]", callbackInfo->touchEnabled_);
        },
        [](napi_env env, napi_status status, void* data) {   //execute the complete function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);

            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->enabled_, &result[PARAM1]));
            HILOG_INFO("IsOpenTouchExploration completed touchEnabled_[%{public}d]", callbackInfo->touchEnabled_);

            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value return_val;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &return_val);
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

napi_value GetAbilityList(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start", __func__);
    size_t argc = 3;
    napi_value parameters[3] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    std::string abilityTypes = GetStringFromNAPI(env,  parameters[0]);
    std::string stateTypes = GetStringFromNAPI(env,  parameters[1]);;

    HILOG_INFO("abilityTypes[%{public}s]", abilityTypes.c_str());
    HILOG_INFO("stateTypes[%{public}s]", stateTypes.c_str());

    NAccessibilitySystemAbilityClient *callbackInfo = new NAccessibilitySystemAbilityClient();
    callbackInfo->abilityTypes_ = ConvertStringToAccessibilityAbilityTypes(abilityTypes);
    callbackInfo->stateTypes_ = ConvertStringToAbilityStateTypes(stateTypes);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_TWO) {
        HILOG_DEBUG("GetAbilityList callback mode");
        napi_create_reference(env, parameters[2], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("GetAbilityList promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAbilityList", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {  // execute async to call c++ function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->abilityList_ = AccessibilitySystemAbilityClient::GetInstance()->GetAbilityList(
                int(callbackInfo->abilityTypes_),
                callbackInfo->stateTypes_);
            HILOG_INFO("GetAbilityList Executing GetAbilityList[%{public}d]", callbackInfo->abilityList_.size());
        },
        [](napi_env env, napi_status status, void* data) {   //execute the complete function
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
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
                napi_value return_val;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &return_val);
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

napi_value SendEvent(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start", __func__);
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilitySystemAbilityClient *callbackInfo = new NAccessibilitySystemAbilityClient();
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
        env, nullptr, resource,
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            callbackInfo->result_ = AccessibilitySystemAbilityClient::GetInstance()->SendEvent(
                callbackInfo->eventInfo_);
            HILOG_INFO("SendEvent result[%{public}d]", callbackInfo->result_);
        },
        [](napi_env env, napi_status status, void* data) {
            NAccessibilitySystemAbilityClient *callbackInfo = (NAccessibilitySystemAbilityClient*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(env, &undefined);
            NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->result_, &result[PARAM1]));
            if (callbackInfo->callback_) {
                result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_value return_val;
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &return_val);
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

napi_value SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start", __func__);
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "Failed to get event type");

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>();
    stateListener->StartWork(env, 1, args);
    g_states.push_back(stateListener);
    std::string eventType = GetStringFromNAPI(env, args[0]);
    AccessibilityStateEventType type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    if (std::strcmp(eventType.c_str(), "accessibility") == 0) {
        type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    } else if (std::strcmp(eventType.c_str(), "touchExplorer") == 0) {
        type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
    } else {
         HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
    }
    bool result = AccessibilitySystemAbilityClient::GetInstance()->SubscribeStateObserver(stateListener, type);
    napi_value ret = {0};
    napi_get_boolean(env, result, &ret);
    return ret;
}

napi_value UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s start  observer size%{public}d", __func__, g_states.size());
    bool result = false;
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    //napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    //NAPI_ASSERT(env, status == napi_ok, "Failed to get event type");
    std::string eventType = GetStringFromNAPI(env, args[0]);

    if (argc > ARGS_SIZE_ONE) {
        HILOG_DEBUG("UnsubscribeState callback mode %{public}d", g_states.size());
        NAccessibilitySystemAbilityClient *callbackInfo = new NAccessibilitySystemAbilityClient();
        napi_create_reference(env, args[1], 1, &callbackInfo->callback_);
        napi_value resource = nullptr;
        napi_create_string_utf8(env, "off", NAPI_AUTO_LENGTH, &resource);
        callbackInfo->eventType_ = eventType;
        callbackInfo->stateListener_ = g_states;
        //async to sync start
        AccessibilityStateEventType type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        std::string eventType = callbackInfo->eventType_ ;
        if (std::strcmp(eventType.c_str(), "accessibility") == 0) {
            type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
        } else if (std::strcmp(eventType.c_str(), "touchExplorer") == 0) {
            type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
        } else {
            HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        }
        int i = 0;
        HILOG_INFO("%{public}s *******observer size%{public}d*******", __func__, g_states.size());
        for (auto observer : g_states) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                callbackInfo->result_ = AccessibilitySystemAbilityClient::GetInstance()->UnsubscribeStateObserver(observer, type);
                HILOG_INFO("%{public}s ***********i%{public}d*********", __func__, i);
                g_states.erase(g_states.begin() + i);
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
        // NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->result_, &result[PARAM1]));
        if (callbackInfo->callback_) {
            result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
            napi_get_reference_value(env, callbackInfo->callback_, &callback);
            napi_value return_val;
            napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &return_val);
            napi_delete_reference(env, callbackInfo->callback_);
        } else {
            HILOG_ERROR("SubscribeState end eventType[%{public}s] is error", callbackInfo->eventType_.c_str());
        }
        //async to sync end
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
        for (auto observer : g_states) {
            if (observer->GetEnv() == env && strcmp(observer->GetEventType().c_str(), eventType.c_str()) == 0) {
                result = AccessibilitySystemAbilityClient::GetInstance()->UnsubscribeStateObserver(observer, type);
                g_states.erase(g_states.begin() + i);
                break;
            }
            i++;
        }
    }
    napi_value ret = 0;
    napi_get_boolean(env, result, &ret);
    return ret;
}

StateListener::StateListener()
{

}

AccessibilityStateEventType StateListener::GetStateType()
{
    HILOG_INFO("%{public}s" , __func__);
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

napi_value StateListener::StartWork(napi_env env, size_t functionIndex, napi_value *args)
{
    HILOG_INFO("%{public}s" , __func__);
    eventType_ = GetStringFromNAPI(env, args[0]);
    napi_create_reference(env, args[functionIndex], 1, &handlerRef_);
    env_ = env;
    napi_value result = {0};
    return result;
}

void StateListener::NotifyJS(napi_env env, bool enabled, std::string stateType, std::string desc,  napi_ref handlerRef)
{
    HILOG_INFO("%{public}s" , __func__);
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
    HILOG_INFO("%{public}s" , __func__);
    for (auto observer : g_states) {
        if (observer->GetStateType() == stateEvent.GetEventType()) {
            if (stateEvent.GetEventResult()) {
                observer->NotifyJS(observer->GetEnv(), true, observer->GetEventType(),
                    stateEvent.GetEventMsg(), observer->GetHandler());
            } else {
                observer->NotifyJS(observer->GetEnv(), false, observer->GetEventType(),
                    stateEvent.GetEventMsg(), observer->GetHandler());
            }
        }
    }
}