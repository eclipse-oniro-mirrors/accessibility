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
using namespace OHOS::AccessibilityNapi;
using namespace OHOS::AccessibilityConfig;

std::shared_ptr<NAccessibilityConfigObserverImpl> NAccessibilityConfig::configObservers_ =
    std::make_shared<NAccessibilityConfigObserverImpl>();
std::shared_ptr<EnableAbilityListsObserverImpl> NAccessibilityConfig::enableAbilityListsObservers_ =
    std::make_shared<EnableAbilityListsObserverImpl>();

void EnableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
    napi_env env, napi_value* parameters, NAccessibilityConfigData* callbackInfo)
{
    if (argc < ARGS_SIZE_THREE - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        // parse name
        std::string ability = "";
        if (ParseString(env, ability, parameters[PARAM0])) {
            HILOG_INFO("ability = %{private}s", ability.c_str());
            callbackInfo->abilityName_ = ability;
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }

        // parse capability
        if (!ConvertJSToCapabilities(env, parameters[PARAM1], callbackInfo->capabilities_)) {
            HILOG_ERROR("convert capabilities failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }
}

void DisableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
    napi_env env, napi_value* parameters, NAccessibilityConfigData* callbackInfo)
{
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        // parse name
        std::string ability = "";
        if (ParseString(env, ability, parameters[PARAM0])) {
            HILOG_INFO("ability = %{private}s", ability.c_str());
            callbackInfo->abilityName_ = ability;
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }
}

napi_value NAccessibilityConfig::EnableAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    size_t argc = ARGS_SIZE_THREE;
    napi_value parameters[ARGS_SIZE_THREE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    EnableAbilityError(argc, errCode, env, parameters, callbackInfo);

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_THREE - 1 && CheckJsFunction(env, parameters[PARAM2])) {
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
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            if (callbackInfo->capabilities_ != 0) {
                callbackInfo->ret_ = instance.EnableAbility(
                    callbackInfo->abilityName_, callbackInfo->capabilities_);
            }
        }, NAccessibilityConfig::AsyncWorkComplete, reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityConfig::DisableAbility(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    DisableAbilityError(argc, errCode, env, parameters, callbackInfo);

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_TWO - 1 && CheckJsFunction(env, parameters[PARAM1])) {
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
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            if (callbackInfo) {
                callbackInfo->ret_ = instance.DisableAbility(callbackInfo->abilityName_);
            }
        }, NAccessibilityConfig::AsyncWorkComplete,
        reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    napi_queue_async_work(env, callbackInfo->work_);
    return promise;
}

napi_value NAccessibilityConfig::SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        std::string observerType = "";
        if (!ParseString(env, observerType, args[PARAM0])) {
            HILOG_ERROR("observer type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") != 0) {
                HILOG_ERROR("args[PARAM0] is wrong[%{public}s", observerType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, args[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOG_ERROR("args[PARAM1] format is wrong");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    enableAbilityListsObservers_->SubscribeObserver(env, args[PARAM1]);
    return nullptr;
}

napi_value NAccessibilityConfig::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        std::string observerType = "";
        if (!ParseString(env, observerType, args[PARAM0])) {
            HILOG_ERROR("observer type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") != 0) {
                HILOG_ERROR("args[PARAM0] is wrong[%{public}s", observerType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    if (argc > ARGS_SIZE_TWO - 1 && CheckJsFunction(env, args[PARAM1])) {
        enableAbilityListsObservers_->UnsubscribeObserver(env, args[PARAM1]);
    } else {
        enableAbilityListsObservers_->UnsubscribeObservers();
    }
    return nullptr;
}

void NAccessibilityConfig::AsyncWorkComplete(napi_env env, napi_status status, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    napi_value result[ARGS_SIZE_ONE] = {0};
    napi_value callback = 0;
    napi_value returnVal = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_call_function(env, undefined, callback, ARGS_SIZE_ONE, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
        HILOG_DEBUG("complete function callback mode");
    } else {
        if (callbackInfo->ret_ == OHOS::Accessibility::RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, undefined);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
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
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT:
            callbackInfo->ret_ = instance.SetHighContrastTextState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
            callbackInfo->ret_ = instance.SetInvertColorState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
            callbackInfo->ret_ = instance.SetAnimationOffState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
            callbackInfo->ret_ = instance.SetScreenMagnificationState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
            callbackInfo->ret_ = instance.SetAudioMonoState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
            callbackInfo->ret_ = instance.SetMouseKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
            callbackInfo->ret_ = instance.SetShortKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            callbackInfo->ret_ = instance.SetCaptionsState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            callbackInfo->ret_ = instance.SetContentTimeout(callbackInfo->uint32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            callbackInfo->ret_ = instance.SetMouseAutoClick(callbackInfo->int32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
            callbackInfo->ret_ = instance.SetAudioBalance(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            callbackInfo->ret_ = instance.SetBrightnessDiscount(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
            {
                auto filter = ConvertStringToDaltonizationTypes(callbackInfo->stringConfig_);
                callbackInfo->ret_ = instance.SetDaltonizationColorFilter(filter);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            callbackInfo->ret_ = instance.SetShortkeyTarget(callbackInfo->stringConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            callbackInfo->ret_ = instance.SetCaptionsProperty(callbackInfo->captionProperty_);
            break;
        default:
            break;
    }
}

void ConfigCompleteInfoById(napi_env env, NAccessibilityConfigData* callbackInfo, napi_value* result)
{
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT:
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
}

void NAccessibilityConfig::GetConfigComplete(napi_env env, napi_status status, void* data)
{
    HILOG_INFO();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    HILOG_INFO("callbackInfo->id_ = %{public}d", callbackInfo->id_);
    napi_value result[ARGS_SIZE_TWO] = {0};
    ConfigCompleteInfoById(env, callbackInfo, result);

    napi_value returnVal = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
        HILOG_DEBUG("complete function callback mode");
    } else {
        if (callbackInfo->ret_ == OHOS::Accessibility::RET_OK) {
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
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
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    switch (callbackInfo->id_) {
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT:
            callbackInfo->ret_ = instance.GetHighContrastTextState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
            callbackInfo->ret_ = instance.GetInvertColorState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
            callbackInfo->ret_ = instance.GetAnimationOffState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
            callbackInfo->ret_ = instance.GetScreenMagnificationState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
            callbackInfo->ret_ = instance.GetAudioMonoState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
            callbackInfo->ret_ = instance.GetMouseKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
            callbackInfo->ret_ = instance.GetShortKeyState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
            callbackInfo->ret_ = instance.GetCaptionsState(callbackInfo->boolConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            {
                uint32_t timeout = 0;
                callbackInfo->ret_ = instance.GetContentTimeout(timeout);
                callbackInfo->int32Config_ = static_cast<int32_t>(timeout);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            callbackInfo->ret_ = instance.GetMouseAutoClick(callbackInfo->int32Config_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
            callbackInfo->ret_ = instance.GetAudioBalance(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
            callbackInfo->ret_ = instance.GetBrightnessDiscount(callbackInfo->floatConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
            {
                OHOS::AccessibilityConfig::DALTONIZATION_TYPE type;
                callbackInfo->ret_ = instance.GetDaltonizationColorFilter(type);
                callbackInfo->stringConfig_ = ConvertDaltonizationTypeToString(type);
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
            callbackInfo->ret_ = instance.GetShortkeyTarget(callbackInfo->stringConfig_);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            callbackInfo->ret_ = instance.GetCaptionsProperty(callbackInfo->captionProperty_);
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

    NAccessibilityConfigClass* obj;
    napi_status status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get unwrap obj");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    HILOG_INFO("ConfigID = %{public}d", obj->GetConfigId());
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }
    if (errCode == OHOS::Accessibility::RET_OK) {
        bool ret = false;
        switch (obj->GetConfigId()) {
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE:
                {
                    bool state = false;
                    ret = ParseBool(env, state, parameters[PARAM0]);
                    callbackInfo->boolConfig_ = state;
                }
                break;
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
                {
                    int32_t timeout = 0;
                    ret = ParseInt32(env, timeout, parameters[PARAM0]);
                    callbackInfo->uint32Config_ = static_cast<uint32_t>(timeout);
                }
                break;
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
                {
                    int32_t time = 0;
                    ret = ParseInt32(env, time, parameters[PARAM0]);
                    callbackInfo->int32Config_ = time;
                }
                break;
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT:
                {
                    double doubleTemp = 0;
                    ret = ParseDouble(env, doubleTemp, parameters[PARAM0]);
                    callbackInfo->floatConfig_ = static_cast<float>(doubleTemp);
                }
                break;
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER:
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET:
                {
                    std::string target = "";
                    ret = ParseString(env, target, parameters[PARAM0]) && target.length() > 0;
                    callbackInfo->stringConfig_ = target;
                }
                break;
            case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
                ret = ConvertObjToCaptionProperty(env, parameters[PARAM0], &callbackInfo->captionProperty_);
                break;
            default:
                break;
        }
        if (!ret) {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }
    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }
    callbackInfo->id_ = obj->GetConfigId();

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_TWO && CheckJsFunction(env, parameters[PARAM1])) {
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
        NAccessibilityConfig::AsyncWorkComplete,
        reinterpret_cast<void*>(callbackInfo),
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

    NAccessibilityConfigClass* obj;
    napi_status status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get unwrap obj");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    HILOG_INFO("ConfigID = %{public}d", obj->GetConfigId());
    
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    callbackInfo->id_ = obj->GetConfigId();

    // parse function if it needs
    napi_value promise = nullptr;
    if (argc >= ARGS_SIZE_ONE && CheckJsFunction(env, parameters[PARAM0])) {
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
        reinterpret_cast<void*>(callbackInfo),
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
    napi_status status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get unwrap obj");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_ONE) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, parameters[PARAM0], &valueType);
        if (valueType != napi_function) {
            HILOG_ERROR("parameters[PARAM1] format is wrong");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    configObservers_->SubscribeObserver(env, obj->GetConfigId(), parameters[PARAM0]);
    return nullptr;
}

napi_value NAccessibilityConfig::UnSubscribeConfigObserver(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value jsthis;
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, &jsthis, nullptr);
    NAccessibilityConfigClass* obj;
    napi_status status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get unwrap obj");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }
    if (!obj) {
        HILOG_ERROR("obj is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    if (argc >= ARGS_SIZE_ONE && CheckJsFunction(env, parameters[PARAM0])) {
        configObservers_->UnsubscribeObserver(env, obj->GetConfigId(), parameters[PARAM0]);
    } else {
        configObservers_->UnsubscribeObservers(obj->GetConfigId());
    }

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

    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create work.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }

    callbackInfo->env_ = env_;
    callbackInfo->ref_ = callback_;
    work->data = static_cast<void*>(callbackInfo);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    int ret = uv_queue_work(
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
    if (ret != 0) {
        HILOG_ERROR("Failed to execute OnEnableAbilityListsStateChanged work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void EnableAbilityListsObserverImpl::SubscribeToFramework()
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
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

void EnableAbilityListsObserverImpl::SubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->callback_)) {
            HILOG_DEBUG("Observer exist");
            return;
        } else {
            iter++;
        }
    }

    napi_ref callback = nullptr;
    napi_create_reference(env, observer, 1, &callback);
    std::shared_ptr<EnableAbilityListsObserver> observerPtr =
        std::make_shared<EnableAbilityListsObserver>(env, callback);

    enableAbilityListsObservers_.emplace_back(observerPtr);
    HILOG_INFO("observer size%{public}zu", enableAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::UnsubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->callback_)) {
            enableAbilityListsObservers_.erase(iter);
            return;
        } else {
            iter++;
        }
    }
}

void EnableAbilityListsObserverImpl::UnsubscribeObservers()
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    enableAbilityListsObservers_.clear();
}