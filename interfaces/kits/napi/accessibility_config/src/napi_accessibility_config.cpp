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
#include "ipc_skeleton.h"
#include "accessibility_utils.h"
#include "tokenid_kit.h"
#include "accesstoken_kit.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityNapi;
using namespace OHOS::AccessibilityConfig;
using namespace Security::AccessToken;

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

std::shared_ptr<NAccessibilityConfigObserverImpl> NAccessibilityConfig::configObservers_ =
    std::make_shared<NAccessibilityConfigObserverImpl>();
std::shared_ptr<EnableAbilityListsObserverImpl> NAccessibilityConfig::enableAbilityListsObservers_ =
    std::make_shared<EnableAbilityListsObserverImpl>();

void NAccessibilityConfig::EnableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
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
            HILOG_DEBUG("ability = %{private}s", ability.c_str());
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

void NAccessibilityConfig::DisableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
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
            HILOG_DEBUG("ability = %{private}s", ability.c_str());
            callbackInfo->abilityName_ = ability;
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }
}

napi_value NAccessibilityConfig::EnableAbility(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
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
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityConfig::DisableAbility(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("DisableAbility callbackInfo is nullptr");
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
        HILOG_ERROR("DisableAbility invalid param");
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
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

bool NAccessibilityConfig::CheckReadPermission(const std::string &permission)
{
    HILOG_DEBUG();
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == TOKEN_INVALID) {
        HILOG_WARN("AccessToken type:%{private}d, permission:%{private}d denied!", tokenType, tokenId);
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(tokenId, permission);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID:%{private}u, permission:%{private}s denied!", tokenId, permission.c_str());
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, tokenId, permission.c_str());
    return true;
}

bool NAccessibilityConfig::IsAvailable(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NOT_SYSTEM_APP);
        napi_throw(env, err);
        HILOG_ERROR("is not system app");
        return false;
    }

    if (!CheckReadPermission(OHOS_PERMISSION_READ_ACCESSIBILITY_CONFIG)) {
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NO_PERMISSION);
        napi_throw(env, err);
        HILOG_ERROR("have no read permission");
        return false;
    }
    
    return true;
}

napi_value NAccessibilityConfig::SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    if (!IsAvailable(env, info)) {
        return nullptr;
    }
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    std::string observerType = "";
    if (errCode == OHOS::Accessibility::RET_OK) {
        if (!ParseString(env, observerType, args[PARAM0])) {
            HILOG_ERROR("observer type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") != 0 &&
                std::strcmp(observerType.c_str(), "installedAccessibilityListChange") != 0) {
                HILOG_ERROR("args[PARAM0] is wrong[%{public}s", observerType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, args[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOG_ERROR("SubscribeState args[PARAM1] format is wrong");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }
    if (enableAbilityListsObservers_ == nullptr) {
        HILOG_ERROR("enableAbilityListsObservers_ is null");
        return nullptr;
    }
    if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
        enableAbilityListsObservers_->SubscribeObserver(env, args[PARAM1]);
    }
    if (std::strcmp(observerType.c_str(), "installedAccessibilityListChange") == 0) {
        enableAbilityListsObservers_->SubscribeInstallObserver(env, args[PARAM1]);
    }
    return nullptr;
}

napi_value NAccessibilityConfig::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    if (!IsAvailable(env, info)) {
        return nullptr;
    }
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }
    std::string observerType = "";
    if (errCode == OHOS::Accessibility::RET_OK) {
        if (!ParseString(env, observerType, args[PARAM0])) {
            HILOG_ERROR("observer type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") != 0 &&
                std::strcmp(observerType.c_str(), "installedAccessibilityListChange") != 0) {
                HILOG_ERROR("args[PARAM0] is wrong[%{public}s", observerType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("UnsubscribeState invalid param");
        napi_throw(env, err);
        return nullptr;
    }
    if (enableAbilityListsObservers_ == nullptr) {
        HILOG_ERROR("enableAbilityListsObservers_ is null");
        return nullptr;
    }
    if (argc > ARGS_SIZE_TWO - 1 && CheckJsFunction(env, args[PARAM1])) {
        if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
            enableAbilityListsObservers_->UnsubscribeObserver(env, args[PARAM1]);
        } else {
            enableAbilityListsObservers_->UnsubscribeInstallObserver(env, args[PARAM1]);
        }
    } else {
        if (std::strcmp(observerType.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
            enableAbilityListsObservers_->UnsubscribeObservers();
        } else {
            enableAbilityListsObservers_->UnsubscribeInstallObservers();
        }
    }
    return nullptr;
}

void NAccessibilityConfig::AsyncWorkComplete(napi_env env, napi_status status, void* data)
{
    HILOG_DEBUG();
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

void NAccessibilityConfig::SetScreenTouchConfigExecute(NAccessibilityConfigData* callbackInfo)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONIFG_CLICK_RESPONSE_TIME) {
        auto time = ConvertStringToClickResponseTimeTypes(callbackInfo->stringConfig_);
        callbackInfo->ret_ = instance.SetClickResponseTime(time);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_TIME) {
        auto time = ConvertStringToIgnoreRepeatClickTimeTypes(callbackInfo->stringConfig_);
        callbackInfo->ret_ = instance.SetIgnoreRepeatClickTime(time);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        callbackInfo->ret_ = instance.SetIgnoreRepeatClickState(callbackInfo->boolConfig_);
    }
}

void NAccessibilityConfig::SetConfigExecute(napi_env env, void* data)
{
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    HILOG_DEBUG("callbackInfo->id_ = %{public}d", callbackInfo->id_);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
        callbackInfo->ret_ = instance.SetHighContrastTextState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_STATE) {
        callbackInfo->ret_ = instance.SetDaltonizationState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR) {
        callbackInfo->ret_ = instance.SetInvertColorState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF) {
        callbackInfo->ret_ = instance.SetAnimationOffState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION) {
        callbackInfo->ret_ = instance.SetScreenMagnificationState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO) {
        callbackInfo->ret_ = instance.SetAudioMonoState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY) {
        callbackInfo->ret_ = instance.SetMouseKeyState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY) {
        callbackInfo->ret_ = instance.SetShortKeyState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE) {
        callbackInfo->ret_ = instance.SetCaptionsState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT) {
        callbackInfo->ret_ = instance.SetContentTimeout(callbackInfo->uint32Config_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK) {
        callbackInfo->ret_ = instance.SetMouseAutoClick(callbackInfo->int32Config_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE) {
        callbackInfo->ret_ = instance.SetAudioBalance(callbackInfo->floatConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT) {
        callbackInfo->ret_ = instance.SetBrightnessDiscount(callbackInfo->floatConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET) {
        callbackInfo->ret_ = instance.SetShortkeyTarget(callbackInfo->stringConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_MULTI_TARGET) {
        callbackInfo->ret_ = instance.SetShortkeyMultiTarget(callbackInfo->stringVectorConfig_);
    }  else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE) {
        callbackInfo->ret_ = instance.SetCaptionsProperty(callbackInfo->captionProperty_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
        auto filter = ConvertStringToDaltonizationTypes(callbackInfo->stringConfig_);
        callbackInfo->ret_ = instance.SetDaltonizationColorFilter(filter);
    }

    SetScreenTouchConfigExecute(callbackInfo);
}

void NAccessibilityConfig::ConfigCompleteInfoById(napi_env env, NAccessibilityConfigData* callbackInfo,
    napi_value* result, size_t len)
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
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_STATE:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE:
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
        case OHOS::AccessibilityConfig::CONFIG_ID::CONIFG_CLICK_RESPONSE_TIME:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_TIME:
            napi_create_string_utf8(env, callbackInfo->stringConfig_.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]);
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_MULTI_TARGET:
            napi_create_array(env, &result[PARAM1]);
            ConvertStringVecToJS(env, result[PARAM1], callbackInfo->stringVectorConfig_);
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
    HILOG_DEBUG();
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }
    HILOG_DEBUG("callbackInfo->id_ = %{public}d", callbackInfo->id_);
    napi_value result[ARGS_SIZE_TWO] = {0};
    ConfigCompleteInfoById(env, callbackInfo, result, ARGS_SIZE_TWO);

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
            HILOG_DEBUG("GetConfigComplete callbackInfo->ret_ is RET_OK");
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            HILOG_DEBUG("GetConfigComplete callbackInfo->ret_ is not RET_OK");
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

void NAccessibilityConfig::GetScreenTouchConfigExecute(NAccessibilityConfigData* callbackInfo)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONIFG_CLICK_RESPONSE_TIME) {
        OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME time;
        callbackInfo->ret_ = instance.GetClickResponseTime(time);
        callbackInfo->stringConfig_ = ConvertClickResponseTimeTypeToString(time);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_TIME) {
        OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME time;
        callbackInfo->ret_ = instance.GetIgnoreRepeatClickTime(time);
        callbackInfo->stringConfig_ = ConvertIgnoreRepeatClickTimeTypeToString(time);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        callbackInfo->ret_ = instance.GetIgnoreRepeatClickState(callbackInfo->boolConfig_);
    }
}

void NAccessibilityConfig::GetConfigExecute(napi_env env, void* data)
{
    NAccessibilityConfigData* callbackInfo = static_cast<NAccessibilityConfigData*>(data);
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        return;
    }

    HILOG_DEBUG("callbackInfo->id_ = %{public}d", callbackInfo->id_);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
        callbackInfo->ret_ = instance.GetHighContrastTextState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_STATE) {
        callbackInfo->ret_ = instance.GetDaltonizationState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_INVERT_COLOR) {
        callbackInfo->ret_ = instance.GetInvertColorState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ANIMATION_OFF) {
        callbackInfo->ret_ = instance.GetAnimationOffState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION) {
        callbackInfo->ret_ = instance.GetScreenMagnificationState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_MONO) {
        callbackInfo->ret_ = instance.GetAudioMonoState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_KEY) {
        callbackInfo->ret_ = instance.GetMouseKeyState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY) {
        callbackInfo->ret_ = instance.GetShortKeyState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE) {
        callbackInfo->ret_ = instance.GetCaptionsState(callbackInfo->boolConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK) {
        callbackInfo->ret_ = instance.GetMouseAutoClick(callbackInfo->int32Config_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_AUDIO_BALANCE) {
        callbackInfo->ret_ = instance.GetAudioBalance(callbackInfo->floatConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT) {
        callbackInfo->ret_ = instance.GetBrightnessDiscount(callbackInfo->floatConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_TARGET) {
        callbackInfo->ret_ = instance.GetShortkeyTarget(callbackInfo->stringConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_MULTI_TARGET) {
        callbackInfo->ret_ = instance.GetShortkeyMultiTarget(callbackInfo->stringVectorConfig_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE) {
        callbackInfo->ret_ = instance.GetCaptionsProperty(callbackInfo->captionProperty_);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT) {
        uint32_t timeout = 0;
        callbackInfo->ret_ = instance.GetContentTimeout(timeout);
        callbackInfo->int32Config_ = static_cast<int32_t>(timeout);
    } else if (callbackInfo->id_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
        OHOS::AccessibilityConfig::DALTONIZATION_TYPE type;
        callbackInfo->ret_ = instance.GetDaltonizationColorFilter(type);
        callbackInfo->stringConfig_ = ConvertDaltonizationTypeToString(type);
    }

    GetScreenTouchConfigExecute(callbackInfo);
}

NAccessibilityConfigData* NAccessibilityConfig::GetCallbackInfo(napi_env env, napi_callback_info info,
    napi_value* parameters, size_t& argc, NAccessibilityConfigClass*& obj)
{
    napi_value jsthis;
    napi_get_cb_info(env, info, &argc, parameters, &jsthis, nullptr);

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
    HILOG_DEBUG("ConfigID = %{public}d", obj->GetConfigId());
    NAccessibilityConfigData* callbackInfo = new(std::nothrow) NAccessibilityConfigData();
    if (!callbackInfo) {
        HILOG_ERROR("callbackInfo is nullptr");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }
    return callbackInfo;
}

bool NAccessibilityConfig::ParseConnectTimeoutData(napi_env env, NAccessibilityConfigData* callbackInfo,
    napi_value* parameters)
{
    int32_t timeout = 0;
    bool ret = ParseInt32(env, timeout, parameters[PARAM0]);
    callbackInfo->uint32Config_ = static_cast<uint32_t>(timeout);
    return ret;
}

bool NAccessibilityConfig::ParseMouseAutoClickData(napi_env env, NAccessibilityConfigData* callbackInfo,
    napi_value* parameters)
{
    int32_t time = 0;
    bool ret = ParseInt32(env, time, parameters[PARAM0]);
    callbackInfo->int32Config_ = time;
    return ret;
}

bool NAccessibilityConfig::SetConfigParseBoolData(napi_env env, NAccessibilityConfigData* callbackInfo,
    napi_value* parameters)
{
    bool state = false;
    bool ret = ParseBool(env, state, parameters[PARAM0]);
    callbackInfo->boolConfig_ = state;
    return ret;
}

bool NAccessibilityConfig::SetConfigParseData(napi_env env, NAccessibilityConfigClass* obj,
    NAccessibilityConfigData* callbackInfo, napi_value* parameters, size_t argc)
{
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
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_DALTONIZATION_STATE:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE:
            return SetConfigParseBoolData(env, callbackInfo, parameters);
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CONTENT_TIMEOUT:
            return ParseConnectTimeoutData(env, callbackInfo, parameters);
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_MOUSE_AUTOCLICK:
            return ParseMouseAutoClickData(env, callbackInfo, parameters);
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
        case OHOS::AccessibilityConfig::CONFIG_ID::CONIFG_CLICK_RESPONSE_TIME:
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_TIME:
            {
                std::string target = "";
                ret = ParseString(env, target, parameters[PARAM0]) && target.length() > 0;
                callbackInfo->stringConfig_ = target;
            }
            break;
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_SHORT_KEY_MULTI_TARGET:
            {
                std::vector<std::string> stringArray;
                ConvertStringArrayJSToNAPICommon(env, parameters[PARAM0], stringArray);
                callbackInfo->stringVectorConfig_ = stringArray;
                return true;
            }
        case OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE:
            return ConvertObjToCaptionProperty(env, parameters[PARAM0], &callbackInfo->captionProperty_);
        default:
            break;
    }
    return ret;
}

napi_value NAccessibilityConfig::SetConfig(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();

    NAccessibilityConfigClass* obj;
    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    NAccessibilityConfigData* callbackInfo = GetCallbackInfo(env, info, parameters, argc, obj);
    if (callbackInfo == nullptr) {
        return nullptr;
    }

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        if (!SetConfigParseData(env, obj, callbackInfo, parameters, argc)) {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("SetConfig invalid param");
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
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityConfig::GetConfig(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG("ConfigID = %{public}d", obj->GetConfigId());
    
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
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityConfig::SubscribeConfigObserver(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    if (!IsAvailable(env, info)) {
        return nullptr;
    }
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
    HILOG_DEBUG();
    if (!IsAvailable(env, info)) {
        return nullptr;
    }
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
    HILOG_DEBUG();

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

    int ret = OnEnableAbilityListsStateChangedWork(work);
    if (ret != 0) {
        HILOG_ERROR("Failed to execute OnEnableAbilityListsStateChanged work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

int EnableAbilityListsObserver::OnEnableAbilityListsStateChangedWork(uv_work_t *work)
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
            AccessibilityCallbackInfo *callbackInfo = static_cast<AccessibilityCallbackInfo*>(work->data);
            napi_env env = callbackInfo->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
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
        },
        uv_qos_default);
    return ret;
}

void EnableAbilityListsObserverImpl::SubscribeToFramework()
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SubscribeEnableAbilityListsObserver(shared_from_this());
}

void EnableAbilityListsObserverImpl::OnEnableAbilityListsStateChanged()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &observer : enableAbilityListsObservers_) {
        observer->OnEnableAbilityListsStateChanged();
    }
}

void EnableAbilityListsObserverImpl::OnInstallAbilityListsStateChanged()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &observer : installAbilityListsObservers_) {
        if (observer) {
            observer->OnEnableAbilityListsStateChanged();
        } else {
            HILOG_ERROR("observer is null");
        }
    }
}

void EnableAbilityListsObserverImpl::SubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG("observer size%{public}zu", enableAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::SubscribeInstallObserver(napi_env env, napi_value observer)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = installAbilityListsObservers_.begin(); iter != installAbilityListsObservers_.end();) {
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

    installAbilityListsObservers_.emplace_back(observerPtr);
    HILOG_DEBUG("observer size%{public}zu", installAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::UnsubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    enableAbilityListsObservers_.clear();
}

void EnableAbilityListsObserverImpl::UnsubscribeInstallObserver(napi_env env, napi_value observer)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = installAbilityListsObservers_.begin(); iter != installAbilityListsObservers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->callback_)) {
            installAbilityListsObservers_.erase(iter);
            return;
        } else {
            iter++;
        }
    }
}

void EnableAbilityListsObserverImpl::UnsubscribeInstallObservers()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    installAbilityListsObservers_.clear();
}
