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

std::shared_ptr<NAccessibilityConfigObserverImpl> NAccessibilityConfig::configObservers_ =
    std::make_shared<NAccessibilityConfigObserverImpl>();
std::shared_ptr<EnableAbilityListsObserverImpl> NAccessibilityConfig::enableAbilityListsObservers_ =
    std::make_shared<EnableAbilityListsObserverImpl>();

napi_value NAccessibilityConfig::EnableAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_THREE;
    napi_value parameters[ARGS_SIZE_THREE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    // parse name
    std::string ability = "";
    ParseString(env, ability, parameters[PARAM0]);
    HILOG_INFO("ability = %{private}s", ability.c_str());
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
            NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
            auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
            if (callbackInfo->capabilities_ != 0) {
                callbackInfo->ret_ = instance.EnableAbility(
                    callbackInfo->abilityName_, callbackInfo->capabilities_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
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
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    // parse name
    std::string ability = "";
    ParseString(env, ability, parameters[PARAM0]);
    HILOG_INFO("ability = %{private}s", ability.c_str());
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
            NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
            auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
            if (callbackInfo) {
                callbackInfo->ret_ = instance.DisableAbility(callbackInfo->abilityName_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
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

napi_value NAccessibilityConfig::SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get callback info");
        return nullptr;
    }

    std::string observerType = "";
    ParseString(env, observerType, args[PARAM0]);
    if (std::strcmp(observerType.c_str(), "enableAbilityListsStateChanged")) {
        HILOG_ERROR("args[PARAM0] is wrong[%{public}s]", observerType.c_str());
        return nullptr;
    }

    napi_ref callback = nullptr;
    napi_create_reference(env, args[PARAM1], 1, &callback);
    std::shared_ptr<EnableAbilityListsObserver> observer = std::make_shared<EnableAbilityListsObserver>(env, callback);

    enableAbilityListsObservers_->SubscribeObserver(observer);
    return nullptr;
}

napi_value NAccessibilityConfig::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get callback info");
        return nullptr;
    }

    std::string observerType = "";
    ParseString(env, observerType, args[PARAM0]);
    if (std::strcmp(observerType.c_str(), "enableAbilityListsStateChanged")) {
        HILOG_ERROR("args[PARAM0] is wrong[%{public}s]", observerType.c_str());
        return nullptr;
    }

    enableAbilityListsObservers_->UnsubscribeObserver();
    return nullptr;
}

void NAccessibilityConfig::SetConfigComplete(napi_env env, napi_status status, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value ret = 0;
    napi_get_undefined(env, &undefined);
    napi_get_undefined(env, &ret);
    if (callbackInfo->callback_) {
        result[PARAM0] = GetErrorValue(env, CODE_SUCCESS);
        result[PARAM1] = ret;
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
        HILOG_DEBUG("complete function callback mode");
    } else {
        napi_resolve_deferred(env, callbackInfo->deferred_, undefined);
        HILOG_DEBUG("complete function promise mode");
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

void NAccessibilityConfig::SetConfigExecute(napi_env env, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRASTE_TEXT:
            instance.SetHighContrastTextState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
            instance.SetInvertColorState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
            instance.SetAnimationOffState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
            instance.SetScreenMagnificationState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
            instance.SetAudioMonoState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
            instance.SetMouseKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
            instance.SetShortKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            instance.SetCaptionState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            instance.SetContentTimeout(callbackInfo->uint32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            instance.SetMouseAutoClick(callbackInfo->int32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
            instance.SetAudioBalance(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            instance.SetBrightnessDiscount(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
            {
                auto filter = ConvertStringToDaltonizationTypes(callbackInfo->stringConfig_);
                instance.SetDaltonizationColorFilter(filter);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            instance.SetShortkeyTarget(callbackInfo->stringConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            instance.SetCaptionProperty(callbackInfo->captionProperty_);
            break;
        default:
            break;
    }
}

void NAccessibilityConfig::GetConfigComplete(napi_env env, napi_status status, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    HILOG_INFO("callbackInfo->id_ = %{public}d", callbackInfo->id_);
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRASTE_TEXT:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            napi_get_boolean(env, callbackInfo->boolConfig_, &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            napi_create_uint32(env, callbackInfo->uint32Config_, &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            napi_create_int32(env, callbackInfo->int32Config_, &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            napi_create_double(env, static_cast<double>(callbackInfo->floatConfig_), &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            napi_create_string_utf8(env, callbackInfo->stringConfig_.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            napi_create_object(env, &result[PARAM1]);
            ConvertCaptionPropertyToJS(env, result[PARAM1], callbackInfo->captionProperty_);
            break;
        default:
            break;
    }
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
}

void NAccessibilityConfig::GetConfigExecute(napi_env env, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRASTE_TEXT:
            instance.GetHighContrastTextState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
            instance.GetInvertColorState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
            instance.GetAnimationOffState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
            instance.GetScreenMagnificationState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
            instance.GetAudioMonoState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
            instance.GetMouseKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
            instance.GetShortKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            instance.GetCaptionState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            instance.GetContentTimeout(callbackInfo->uint32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            instance.GetMouseAutoClick(callbackInfo->int32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
            instance.GetAudioBalance(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            instance.GetBrightnessDiscount(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
            {
                OHOS::AccessibilityConfig::DALTONIZATION_TYPE type;
                instance.GetDaltonizationColorFilter(type);
                callbackInfo->stringConfig_ = ConvertDaltonizationTypeToString(type);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            instance.GetShortkeyTarget(callbackInfo->stringConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            instance.GetCaptionProperty(callbackInfo->captionProperty_);
            break;
        default:
            break;
    }
}

napi_value NAccessibilityConfig::SetConfig(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_value jsthis;
    napi_get_cb_info(env, info, &argc, parameters, &jsthis, nullptr);

    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    NAccessibilityConfigClass* obj;
    NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        delete callbackInfo;
        callbackInfo = nullptr;
        return nullptr;
    }
    HILOG_INFO("ConfigID = %{public}d", obj->GetConfigId());

    switch (obj->GetConfigId()) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRASTE_TEXT:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            {
                bool state = false;
                ParseBool(env, state, parameters[PARAM0]);
                callbackInfo->boolConfig_ = state;
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            {
                uint32_t timeout = 0;
                ParseUint32(env, timeout, parameters[PARAM0]);
                callbackInfo->uint32Config_ = timeout;
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            {
                int32_t time = 0;
                ParseInt32(env, time, parameters[PARAM0]);
                callbackInfo->int32Config_ = time;
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            {
                double doubleTemp = 0;
                napi_get_value_double(env, parameters[PARAM0], &doubleTemp);
                callbackInfo->floatConfig_ = static_cast<float>(doubleTemp);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            {
                std::string target = "";
                ParseString(env, target, parameters[PARAM0]);
                callbackInfo->stringConfig_ = target;
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            ConvertObjToCaptionProperty(env, parameters[PARAM0], &callbackInfo->captionProperty_);
            break;
        default:
            break;
    }
    callbackInfo->id_ = obj->GetConfigId();

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_TWO) {
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SetConfig", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        NAccessibilityConfig::SetConfigExecute,
        // Execute the complete function
        NAccessibilityConfig::SetConfigComplete,
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}


napi_value NAccessibilityConfig::GetConfig(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_value jsthis;
    napi_get_cb_info(env, info, &argc, parameters, &jsthis, nullptr);

    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return nullptr;
    }
    NAccessibilityConfigClass* obj;
    NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        delete callbackInfo;
        callbackInfo = nullptr;
        return nullptr;
    }
    HILOG_INFO("ConfigID = %{public}d", obj->GetConfigId());
    callbackInfo->id_ = obj->GetConfigId();

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_ONE) {
        napi_create_reference(env, parameters[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetConfig", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        NAccessibilityConfig::GetConfigExecute,
        // Execute the complete function
        NAccessibilityConfig::GetConfigComplete,
        (void*)callbackInfo,
        &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityConfig::SubscribeConfigObserver(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_value jsthis;
    napi_get_cb_info(env, info, &argc, parameters, &jsthis, nullptr);
    NAccessibilityConfigClass* obj;
    NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        return nullptr;
    }

    napi_ref handler = nullptr;
    napi_create_reference(env, parameters[PARAM0], 1, &handler);
    std::shared_ptr<NAccessibilityConfigObserver> observer =
        std::make_shared<NAccessibilityConfigObserver>(env, handler, obj->GetConfigId());
    
    configObservers_->SubscribeObserver(observer);
    return nullptr;
}

napi_value NAccessibilityConfig::UnSubscribeConfigObserver(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value jsthis;
    napi_get_cb_info(env, info, 0, nullptr, &jsthis, nullptr);
    NAccessibilityConfigClass* obj;
    NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        return nullptr;
    }

    configObservers_->UnsubscribeObserver(obj->GetConfigId());
    return nullptr;
}

void EnableAbilityListsObserver::OnEnableAbilityListsStateChanged()
{
    HILOG_INFO();

    AccessibilityCallbackInfo *callbackInfo = new(std::nothrow) AccessibilityCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    callbackInfo->env_ = env_;
    callbackInfo->ref_ = callback_;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<AccessibilityCallbackInfo*>(callbackInfo);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AccessibilityCallbackInfo *callbackInfo = static_cast<AccessibilityCallbackInfo*>(work->data);
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

void EnableAbilityListsObserverImpl::SubscribeToFramework()
{
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.SubscribeEnableAbilityListsObserver(shared_from_this());
}

void EnableAbilityListsObserverImpl::OnEnableAbilityListsStateChanged()
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &observer : enableAbilityListsObservers_) {
        observer->OnEnableAbilityListsStateChanged();
    }
}

void EnableAbilityListsObserverImpl::SubscribeObserver(const std::shared_ptr<EnableAbilityListsObserver> &observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    enableAbilityListsObservers_.emplace_back(observer);
    HILOG_INFO("observer size%{public}zu", enableAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::UnsubscribeObserver()
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    enableAbilityListsObservers_.clear();
}