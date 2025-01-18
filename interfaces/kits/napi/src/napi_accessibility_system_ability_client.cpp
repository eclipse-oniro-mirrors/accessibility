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
#include "accessibility_utils.h"
#include "api_reporter_helper.h"

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityNapi;

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

std::shared_ptr<StateListenerImpl> NAccessibilityClient::accessibilityStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> NAccessibilityClient::touchGuideStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> NAccessibilityClient::screenReaderStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED);
std::shared_ptr<NAccessibilityConfigObserverImpl> NAccessibilityClient::captionListeners_ =
    std::make_shared<NAccessibilityConfigObserverImpl>();

napi_ref NAccessibilityClient::aaConsRef_;
napi_ref NAccessibilityClient::aaStyleConsRef_;

#define ACCESSIBILITY_NAPI_ASSERT(env, cond, errCode) \
do { \
    if (!(cond)) { \
        napi_value err = CreateBusinessError(env, errCode); \
        napi_throw(env, err); \
        napi_value res = nullptr; \
        napi_get_undefined(env, &res); \
        return res; \
    } \
} while (0)

napi_value NAccessibilityClient::IsScreenReaderOpenSync(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);
    ACCESSIBILITY_NAPI_ASSERT(env, argc == ARGS_SIZE_ZERO, OHOS::Accessibility::RET_ERR_INVALID_PARAM);

    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    ACCESSIBILITY_NAPI_ASSERT(env, asaClient != nullptr, OHOS::Accessibility::RET_ERR_NULLPTR);
    bool status = false;
    auto ret = asaClient->IsScreenReaderEnabled(status);
    ACCESSIBILITY_NAPI_ASSERT(env, ret == RET_OK, OHOS::Accessibility::RET_ERR_FAILED);
    napi_value result = nullptr;
    napi_get_boolean(env, status, &result);
    return result;
}

napi_value NAccessibilityClient::IsOpenAccessibilitySync(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);
    ACCESSIBILITY_NAPI_ASSERT(env, argc == ARGS_SIZE_ZERO, OHOS::Accessibility::RET_ERR_INVALID_PARAM);

    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    ACCESSIBILITY_NAPI_ASSERT(env, asaClient != nullptr, OHOS::Accessibility::RET_ERR_NULLPTR);
    bool status = false;
    auto ret = asaClient->IsEnabled(status);
    ACCESSIBILITY_NAPI_ASSERT(env, ret == RET_OK, OHOS::Accessibility::RET_ERR_FAILED);
    napi_value result = nullptr;
    napi_get_boolean(env, status, &result);
    return result;
}

napi_value NAccessibilityClient::IsOpenAccessibility(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    if (argc > 0 && CheckJsFunction(env, argv)) {
        HILOG_DEBUG("IsOpenAccessibility callback mode");
        napi_create_reference(env, argv, 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("IsOpenAccessibility promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "IsOpenAccessibility", NAPI_AUTO_LENGTH, &resource);

    auto ret = napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
            if (asaClient) {
                callbackInfo->ret_ = asaClient->IsEnabled(callbackInfo->enabled_);
                HILOG_INFO("IsOpenAccessibility Executing enabled[%{public}d]", callbackInfo->enabled_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            Completefunction(env, "IsOpenAccessibility", data);
        },
        reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    if (ret != napi_ok) {
        delete callbackInfo;
        callbackInfo = nullptr;
        HILOG_ERROR("failed to create async work.");
        return nullptr;
    }
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityClient::IsOpenTouchExplorationSync(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);
    ACCESSIBILITY_NAPI_ASSERT(env, argc == ARGS_SIZE_ZERO, OHOS::Accessibility::RET_ERR_INVALID_PARAM);

    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    ACCESSIBILITY_NAPI_ASSERT(env, asaClient != nullptr, OHOS::Accessibility::RET_ERR_NULLPTR);
    bool status = false;
    auto ret = asaClient->IsTouchExplorationEnabled(status);
    ACCESSIBILITY_NAPI_ASSERT(env, ret == RET_OK, OHOS::Accessibility::RET_ERR_FAILED);
    napi_value result = nullptr;
    napi_get_boolean(env, status, &result);
    return result;
}

napi_value NAccessibilityClient::IsOpenTouchExploration(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);

    napi_value promise = nullptr;
    if (argc > 0 && CheckJsFunction(env, argv)) {
        HILOG_DEBUG("IsOpenTouchExploration callback mode");
        napi_create_reference(env, argv, 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("IsOpenTouchExploration promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "IsOpenTouchExploration", NAPI_AUTO_LENGTH, &resource);

    auto ret = napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        [](napi_env env, void* data) {
            NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
            auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
            if (asaClient) {
                callbackInfo->ret_ = asaClient->IsTouchExplorationEnabled(callbackInfo->touchEnabled_);
                HILOG_INFO("IsOpenTouchExploration Executing touchEnabled[%{public}d]", callbackInfo->touchEnabled_);
            }
        },
        // Execute the complete function
        [](napi_env env, napi_status status, void* data) {
            Completefunction(env, "IsOpenTouchExploration", data);
        },
        reinterpret_cast<void*>(callbackInfo), &callbackInfo->work_);
    if (ret != napi_ok) {
        delete callbackInfo;
        callbackInfo = nullptr;
        HILOG_ERROR("failed to create async work.");
        return nullptr;
    }
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

void NAccessibilityClient::Completefunction(napi_env env, std::string type, void* data)
{
    NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);

    if (type == "IsOpenAccessibility") {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->enabled_, &result[PARAM1]));
        HILOG_INFO("IsOpenAccessibility completed enabled[%{public}d]", callbackInfo->enabled_);
    } else if (type == "IsOpenTouchExploration") {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, callbackInfo->touchEnabled_, &result[PARAM1]));
        HILOG_INFO("IsOpenTouchExploration completed touchEnabled_[%{public}d]", callbackInfo->touchEnabled_);
    } else {
        napi_delete_async_work(env, callbackInfo->work_);
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal = nullptr;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        if (callbackInfo->ret_ == OHOS::Accessibility::RET_OK) {
            HILOG_DEBUG("Completefunction callbackInfo->ret_ is RET_OK");
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            HILOG_DEBUG("Completefunction callbackInfo->ret_ is not RET_OK");
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

void NAccessibilityClient::GetAbilityListExecute(napi_env env, void* data)
{
    NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        callbackInfo->ret_ = asaClient->GetAbilityList(callbackInfo->abilityTypes_,
            callbackInfo->stateTypes_, callbackInfo->abilityList_);
    }
}

void NAccessibilityClient::GetAbilityListComplete(napi_env env, napi_status status, void* data)
{
    NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_create_array(env, &result[PARAM1]);
    ConvertAccessibleAbilityInfosToJS(env, result[PARAM1], callbackInfo->abilityList_);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    if (callbackInfo->callback_) {
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal = nullptr;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        if (callbackInfo->ret_ == OHOS::Accessibility::RET_OK) {
            HILOG_DEBUG("GetAbilityListComplete callbackInfo->ret_ is RET_OK");
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            HILOG_DEBUG("GetAbilityListComplete callbackInfo->ret_ is not RET_OK");
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

napi_value NAccessibilityClient::GetAbilityList(napi_env env, napi_callback_info info)
{
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }

    size_t argc = 3;
    napi_value parameters[3] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    std::string abilityTypes = GetStringFromNAPI(env, parameters[0]);
    std::string stateTypes = GetStringFromNAPI(env, parameters[1]);
    HILOG_INFO("abilityTypes[%{public}s] stateTypes[%{public}s]", abilityTypes.c_str(), stateTypes.c_str());

    callbackInfo->abilityTypes_ = ConvertStringToAccessibilityAbilityTypes(abilityTypes);
    callbackInfo->stateTypes_ = ConvertStringToAbilityStateType(stateTypes);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_TWO && CheckJsFunction(env, parameters[ARGS_SIZE_TWO])) {
        napi_create_reference(env, parameters[ARGS_SIZE_TWO], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAbilityList", NAPI_AUTO_LENGTH, &resource);

    auto ret = napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        NAccessibilityClient::GetAbilityListExecute,
        // Execute the complete function
        NAccessibilityClient::GetAbilityListComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    if (ret != napi_ok) {
        delete callbackInfo;
        callbackInfo = nullptr;
        HILOG_ERROR("failed to create async work.");
        return nullptr;
    }
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityClient::GetAccessibilityExtensionList(napi_env env, napi_callback_info info)
{
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    size_t argc = ARGS_SIZE_THREE;
    napi_value parameters[ARGS_SIZE_THREE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    do {
        if (argc < ARGS_SIZE_THREE - 1) {
            HILOG_ERROR("argc is invalid: %{public}zu", argc);
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }

        // parse ability type
        std::string abilityTypes = "";
        std::string stateTypes = "";
        if (!ParseString(env, abilityTypes, parameters[PARAM0]) ||
            !ParseString(env, stateTypes, parameters[PARAM1])) {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }

        HILOG_INFO("abilityTypes = %{private}s", abilityTypes.c_str());
        if (CheckAbilityType(abilityTypes)) {
            callbackInfo->abilityTypes_ = ConvertStringToAccessibilityAbilityTypes(abilityTypes);
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }
        // parse ability state
        HILOG_INFO("stateTypes = %{private}s", stateTypes.c_str());
        if (CheckStateType(stateTypes)) {
            callbackInfo->stateTypes_ = ConvertStringToAbilityStateType(stateTypes);
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    } while (0);

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    return GetAccessibilityExtensionListAsync(env, argc, parameters, callbackInfo);
}

napi_value NAccessibilityClient::GetAccessibilityExtensionListAsync(
    napi_env env, size_t argc, napi_value* parameters, NAccessibilitySystemAbilityClient* callbackInfo)
{
    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_THREE - 1 && CheckJsFunction(env, parameters[PARAM2])) {
        napi_create_reference(env, parameters[PARAM2], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetAccessibilityExtensionList", NAPI_AUTO_LENGTH, &resource);

    auto ret = napi_create_async_work(env, nullptr, resource,
        // Execute async to call c++ function
        NAccessibilityClient::GetAbilityListExecute,
        // Execute the complete function
        NAccessibilityClient::GetAbilityListComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    if (ret != napi_ok) {
        delete callbackInfo;
        callbackInfo = nullptr;
        HILOG_ERROR("failed to create async work.");
        return nullptr;
    }
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);
    return promise;
}

napi_value NAccessibilityClient::GetAccessibilityExtensionListSync(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value parameters[ARGS_SIZE_THREE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    OHOS::Accessibility::AbilityStateType stateTypes = OHOS::Accessibility::ABILITY_STATE_INVALID;
    uint32_t abilityTypes = 0;

    do {
        if (argc < ARGS_SIZE_THREE - 1) {
            HILOG_ERROR("argc is invalid: %{public}zu", argc);
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }

        // parse ability type
        std::string abilityTypeStr = "";
        std::string stateTypeStr = "";
        if (!ParseString(env, abilityTypeStr, parameters[PARAM0]) ||
            !ParseString(env, stateTypeStr, parameters[PARAM1])) {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }

        HILOG_DEBUG("abilityTypeStr = %{private}s", abilityTypeStr.c_str());
        if (CheckAbilityType(abilityTypeStr)) {
            abilityTypes = ConvertStringToAccessibilityAbilityTypes(abilityTypeStr);
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            break;
        }

        // parse ability state
        HILOG_DEBUG("stateTypeStr = %{private}s", stateTypeStr.c_str());
        if (CheckStateType(stateTypeStr)) {
            stateTypes = ConvertStringToAbilityStateType(stateTypeStr);
        } else {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    } while (0);

    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> abilityList {};
    if (errCode == OHOS::Accessibility::RET_OK) {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (asaClient) {
            errCode = asaClient->GetAbilityList(abilityTypes, stateTypes, abilityList);
        }
    }

    ACCESSIBILITY_NAPI_ASSERT(env, errCode == OHOS::Accessibility::RET_OK,
        OHOS::Accessibility::RET_ERR_INVALID_PARAM);

    napi_value resultAbilityList = nullptr;
    napi_create_array(env, &resultAbilityList);
    ConvertAccessibleAbilityInfosToJS(env, resultAbilityList, abilityList);
    return resultAbilityList;
}

bool NAccessibilityClient::CheckAbilityType(const std::string& abilityType)
{
    if (std::strcmp(abilityType.c_str(), "audible") == 0 ||
        std::strcmp(abilityType.c_str(), "generic") == 0 ||
        std::strcmp(abilityType.c_str(), "haptic") == 0 ||
        std::strcmp(abilityType.c_str(), "spoken") == 0 ||
        std::strcmp(abilityType.c_str(), "visual") == 0 ||
        std::strcmp(abilityType.c_str(), "all") == 0) {
        return true;
    } else {
        return false;
    }
}

bool NAccessibilityClient::CheckStateType(const std::string& stateType)
{
    if (std::strcmp(stateType.c_str(), "enable") == 0 ||
        std::strcmp(stateType.c_str(), "disable") == 0 ||
        std::strcmp(stateType.c_str(), "install") == 0) {
        return true;
    } else {
        return false;
    }
}

void NAccessibilityClient::SendEventExecute(napi_env env, void* data)
{
    NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (callbackInfo->result_ && asaClient) {
        callbackInfo->ret_ = asaClient->SendEvent(callbackInfo->eventInfo_);
    }
    HILOG_INFO("SendEvent result[%{public}d]", callbackInfo->ret_);
}

void NAccessibilityClient::SendEventComplete(napi_env env, napi_status status, void* data)
{
    NAccessibilitySystemAbilityClient* callbackInfo = static_cast<NAccessibilitySystemAbilityClient*>(data);
    napi_value result[ARGS_SIZE_TWO] = {0};
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value ret = 0;
    napi_get_undefined(env, &undefined);
    napi_get_undefined(env, &ret);
    result[PARAM0] = CreateBusinessError(env, callbackInfo->ret_);
    result[PARAM1] = ret;
    if (callbackInfo->callback_) {
        napi_get_reference_value(env, callbackInfo->callback_, &callback);
        napi_value returnVal = nullptr;
        napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &returnVal);
        napi_delete_reference(env, callbackInfo->callback_);
    } else {
        if (callbackInfo->ret_ == OHOS::Accessibility::RET_OK) {
            HILOG_DEBUG("SendEventComplete callbackInfo->ret_ is RET_OK");
            napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
        } else {
            HILOG_DEBUG("SendEventComplete callbackInfo->ret_ is not RET_OK");
            napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM0]);
        }
    }
    napi_delete_async_work(env, callbackInfo->work_);
    delete callbackInfo;
    callbackInfo = nullptr;
}

napi_value NAccessibilityClient::SendEvent(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return nullptr;
    }

    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    callbackInfo->result_ = ConvertEventInfoJSToNAPI(env, parameters[0], callbackInfo->eventInfo_);

    napi_value promise = nullptr;

    if (argc > ARGS_SIZE_ONE && CheckJsFunction(env, parameters[1])) {
        HILOG_DEBUG("SendEvent callback mode");
        napi_create_reference(env, parameters[1], 1, &callbackInfo->callback_);
    } else {
        HILOG_DEBUG("SendEvent promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SendEvent", NAPI_AUTO_LENGTH, &resource);

    auto ret = napi_create_async_work(env, nullptr, resource,
        NAccessibilityClient::SendEventExecute,
        NAccessibilityClient::SendEventComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    if (ret != napi_ok) {
        delete callbackInfo;
        callbackInfo = nullptr;
        HILOG_ERROR("failed to create async work.");
        return nullptr;
    }
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);

    return promise;
}

napi_value NAccessibilityClient::SendAccessibilityEvent(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    NAccessibilitySystemAbilityClient* callbackInfo = new(std::nothrow) NAccessibilitySystemAbilityClient();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_NULLPTR);
        napi_throw(env, err);
        return nullptr;
    }

    size_t argc = ARGS_SIZE_TWO;
    napi_value parameters[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        callbackInfo->result_ = ConvertEventInfoJSToNAPI(env, parameters[0], callbackInfo->eventInfo_);
        if (!callbackInfo->result_) {
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        delete callbackInfo;
        callbackInfo = nullptr;
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("SendAccessibilityEvent invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    napi_value promise = nullptr;
    if (argc > ARGS_SIZE_TWO - 1 && CheckJsFunction(env, parameters[PARAM1])) {
        napi_create_reference(env, parameters[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOG_DEBUG("SendEvent promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SendAccessibilityEvent", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(env, nullptr, resource,
        NAccessibilityClient::SendEventExecute,
        NAccessibilityClient::SendEventComplete,
        reinterpret_cast<void*>(callbackInfo),
        &callbackInfo->work_);
    napi_queue_async_work_with_qos(env, callbackInfo->work_, napi_qos_user_initiated);

    return promise;
}

napi_value NAccessibilityClient::SubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    ApiReportHelper reporter("NAccessibilityClient.SubscribeState");
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("SubscribeState Failed to get event type");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO) {
        HILOG_ERROR("SubscribeState argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }
    uint32_t type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    GetAccessibilityStateEventType(env, args, errCode, type);
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
        HILOG_ERROR("SubscribeState invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    switch (type) {
        case AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.SubscribeState.accessibilityStateChange");
            accessibilityStateListeners_->SubscribeObserver(env, args[PARAM1]);
            break;
        case AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.SubscribeState.touchGuideStateChange");
            touchGuideStateListeners_->SubscribeObserver(env, args[PARAM1]);
            break;
        case AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.SubscribeState.screenReaderStateChange");
            screenReaderStateListeners_->SubscribeObserver(env, args[PARAM1]);
            break;
        default:
            break;
    }
    return nullptr;
}

void NAccessibilityClient::GetAccessibilityStateEventType(
    napi_env env, napi_value* args, OHOS::Accessibility::RetError& errCode, uint32_t& type)
{
    if (errCode == OHOS::Accessibility::RET_OK) {
        std::string eventType = "";
        if (!ParseString(env, eventType, args[PARAM0])) {
            HILOG_ERROR("eventType type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
                type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
            } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
                type = AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED;
            } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
                type = AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED;
            } else {
                HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }
}

napi_value NAccessibilityClient::UnsubscribeState(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    ApiReportHelper reporter("NAccessibilityClient.UnsubscribeState");
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("UnsubscribeState argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    uint32_t type = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    GetAccessibilityStateEventType(env, args, errCode, type);

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }
    switch (type) {
        case AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.UnsubscribeState.accessibilityStateChange");
            if (argc >= ARGS_SIZE_TWO && CheckJsFunction(env, args[PARAM1])) {
                accessibilityStateListeners_->UnsubscribeObserver(env, args[PARAM1]);
            } else {
                accessibilityStateListeners_->UnsubscribeObservers();
            }
            break;
        case AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.UnsubscribeState.touchGuideStateChange");
            if (argc >= ARGS_SIZE_TWO && CheckJsFunction(env, args[PARAM1])) {
                touchGuideStateListeners_->UnsubscribeObserver(env, args[PARAM1]);
            } else {
                touchGuideStateListeners_->UnsubscribeObservers();
            }
            break;
        case AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED:
            reporter.setApiName("NAccessibilityClient.UnsubscribeState.screenReaderStateChange");
            if (argc >= ARGS_SIZE_TWO && CheckJsFunction(env, args[PARAM1])) {
                screenReaderStateListeners_->UnsubscribeObserver(env, args[PARAM1]);
            } else {
                screenReaderStateListeners_->UnsubscribeObservers();
            }
            break;
        default:
            break;
    }

    return nullptr;
}

void StateListener::NotifyJS(napi_env env, bool state, napi_ref handlerRef)
{
    HILOG_INFO("state = [%{public}s]", state ? "true" : "false");
    
    std::shared_ptr<StateCallbackInfo> callbackInfo = std::make_shared<StateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->state_ = state;
    callbackInfo->env_ = env;
    callbackInfo->ref_ = handlerRef;
    auto task = [callbackInfo]() {
        if (callbackInfo == nullptr) {
            return;
        }

        napi_env tmpEnv = callbackInfo->env_;
        auto closeScope = [tmpEnv](napi_handle_scope scope) {
            napi_close_handle_scope(tmpEnv, scope);
        };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(
            OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
        napi_value handler = nullptr;
        napi_value callResult = nullptr;
        napi_value jsEvent = nullptr;
        napi_get_boolean(callbackInfo->env_, callbackInfo->state_, &jsEvent);
        napi_get_reference_value(callbackInfo->env_, callbackInfo->ref_, &handler);
        napi_value undefined = nullptr;
        napi_get_undefined(callbackInfo->env_, &undefined);
        napi_call_function(callbackInfo->env_, undefined, handler, 1, &jsEvent, &callResult);
    };
    if (napi_send_event(env, task, napi_eprio_high) != napi_status::napi_ok) {
        HILOG_ERROR("failed to send event");
    }
}

void StateListener::OnStateChanged(const bool state)
{
    NotifyJS(env_, state, handlerRef_);
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
            "CaptionsManager",
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
    HILOG_INFO();
    ApiReportHelper reporter("NAccessibilityClient.GetCaptionsManager");
    napi_value result = 0;
    napi_value aaCons = nullptr;
    napi_get_reference_value(env, NAccessibilityClient::aaConsRef_, &aaCons);
    napi_new_instance(env, aaCons, 0, nullptr, &result);
    return result;
}

napi_value NAccessibilityClient::SetCaptionStateEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        bool captionState = false;
        OHOS::Accessibility::RetError ret = OHOS::Accessibility::RET_OK;
        if (!ParseBool(env, captionState, parameters[PARAM0])) {
            ret = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            HILOG_INFO("captionState = %{public}s", captionState ? "True" : "False");

            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            ret = instance.SetCaptionsState(captionState);
        }
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionStateEnabled argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }

    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionStateEnabled(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value captionStateEnabled = nullptr;

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool captionState = false;
    instance.GetCaptionsState(captionState);
    napi_get_boolean(env, captionState, &captionStateEnabled);

    HILOG_INFO("captionState = %{public}s", captionState ? "True" : "False");

    return captionStateEnabled;
}

napi_value NAccessibilityClient::SetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
        OHOS::Accessibility::RetError ret = OHOS::Accessibility::RET_OK;
        if (!ConvertObjToCaptionProperty(env, parameters[PARAM0], &captionProperty)) {
            ret = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            ret = instance.SetCaptionsProperty(captionProperty);
        }
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionStyle argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }

    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value captionStyle = nullptr;
    napi_get_reference_value(env, NAccessibilityClient::aaStyleConsRef_, &captionStyle);

    return captionStyle;
}

napi_value NAccessibilityClient::RegisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO();

    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("RegisterCaptionStateCallback Failed to get event type");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_FAILED);
        napi_throw(env, err);
        return nullptr;
    }

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO) {
        HILOG_ERROR("RegisterCaptionStateCallback argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    OHOS::AccessibilityConfig::CONFIG_ID type = OHOS::AccessibilityConfig::CONFIG_ID_MAX;
    if (errCode == OHOS::Accessibility::RET_OK) {
        std::string eventType = "";
        if (!ParseString(env, eventType, args[PARAM0])) {
            HILOG_ERROR("eventType type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
                type = OHOS::AccessibilityConfig::CONFIG_CAPTION_STATE;
            } else if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
                type =  OHOS::AccessibilityConfig::CONFIG_CAPTION_STYLE;
            } else {
                HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_OK) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, args[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOG_ERROR("RegisterCaptionStateCallback args[PARAM1] format is wrong");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    captionListeners_->SubscribeObserver(env, type, args[PARAM1]);

    return nullptr;
}

napi_value NAccessibilityClient::DeregisterCaptionStateCallback(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_TWO;
    napi_value args[ARGS_SIZE_TWO] = {0};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    OHOS::Accessibility::RetError errCode = OHOS::Accessibility::RET_OK;
    if (argc < ARGS_SIZE_TWO - 1) {
        HILOG_ERROR("DeregisterCaptionStateCallback argc is invalid: %{public}zu", argc);
        errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
    }

    OHOS::AccessibilityConfig::CONFIG_ID type = OHOS::AccessibilityConfig::CONFIG_ID_MAX;
    if (errCode == OHOS::Accessibility::RET_OK) {
        std::string eventType = "";
        if (!ParseString(env, eventType, args[PARAM0])) {
            HILOG_ERROR("eventType type parse failed");
            errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        } else {
            if (std::strcmp(eventType.c_str(), "enableChange") == 0) {
                type = OHOS::AccessibilityConfig::CONFIG_CAPTION_STATE;
            } else if (std::strcmp(eventType.c_str(), "styleChange") == 0) {
                type =  OHOS::AccessibilityConfig::CONFIG_CAPTION_STYLE;
            } else {
                HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
                errCode = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
            }
        }
    }

    if (errCode == OHOS::Accessibility::RET_ERR_INVALID_PARAM) {
        napi_value err = CreateBusinessError(env, errCode);
        HILOG_ERROR("DeregisterCaptionStateCallback invalid param");
        napi_throw(env, err);
        return nullptr;
    }

    if (argc >= ARGS_SIZE_TWO && CheckJsFunction(env, args[PARAM1])) {
        captionListeners_->UnsubscribeObserver(env, type, args[PARAM1]);
    } else {
        captionListeners_->UnsubscribeObservers(type);
    }

    return nullptr;
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
    napi_value captionStyle = nullptr;

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "CaptionsStyle",
            NAPI_AUTO_LENGTH,
            NAccessibilityClient::AccessibleAbilityConstructorStyle,
            nullptr,
            sizeof(captionsStyleDesc) / sizeof(captionsStyleDesc[0]),
            captionsStyleDesc,
            &aaStyleCons));

    napi_new_instance(env, aaStyleCons, 0, nullptr, &captionStyle);
    napi_create_reference(env, captionStyle, 1, &NAccessibilityClient::aaStyleConsRef_);
}

napi_value NAccessibilityClient::AccessibleAbilityConstructorStyle(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

napi_value NAccessibilityClient::GetCaptionsFontFamily(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    napi_create_string_utf8(env, captionProperty.GetFontFamily().c_str(), NAPI_AUTO_LENGTH, &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionsFontFamily(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        // Get input FontFamily
        std::string fontFamily = "";
        OHOS::Accessibility::RetError ret = OHOS::Accessibility::RET_OK;
        if (ParseString(env, fontFamily, parameters[PARAM0]) && fontFamily.length() > 0) {
            HILOG_INFO("FontFamily = %{public}s", fontFamily.c_str());
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
            instance.GetCaptionsProperty(captionProperty);
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontFamily(std::string(fontFamily));
            ret = instance.SetCaptionsProperty(captionProperty);
        } else {
            ret = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionsFontFamily argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionsFontScale(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    napi_create_int32(env, captionProperty.GetFontScale(), &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionsFontScale(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        // Get input FontScale
        int32_t num = 100;
        OHOS::Accessibility::RetError ret = OHOS::Accessibility::RET_OK;
        if (ParseInt32(env, num, parameters[PARAM0])) {
            HILOG_INFO("FontScale = %{public}d", num);
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
            instance.GetCaptionsProperty(captionProperty);
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontScale(num);
            ret = instance.SetCaptionsProperty(captionProperty);
        } else {
            ret = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionsFontScale argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionFrontColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    uint32_t color = captionProperty.GetFontColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionFrontColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("SetCaptionFrontColor argc > 1");
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
        OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
        instance.GetCaptionsProperty(captionProperty);
        // Change the input info and then set the CaptionProperty
        captionProperty.SetFontColor(color);
        OHOS::Accessibility::RetError ret = instance.SetCaptionsProperty(captionProperty);
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionFrontColor argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionFontEdgeType(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    napi_create_string_utf8(env, captionProperty.GetFontEdgeType().c_str(), NAPI_AUTO_LENGTH, &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionFontEdgeType(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};

    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        // Get input FontEdgeType
        std::string fontEdgeType = "";
        OHOS::Accessibility::RetError ret = OHOS::Accessibility::RET_OK;
        if (ParseString(env, fontEdgeType, parameters[PARAM0]) && fontEdgeType.length() > 0) {
            HILOG_INFO("fontEdgeType = %{public}s", fontEdgeType.c_str());
            auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
            OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
            instance.GetCaptionsProperty(captionProperty);
            // Change the input info and then set the CaptionProperty
            captionProperty.SetFontEdgeType(std::string(fontEdgeType));
            ret = instance.SetCaptionsProperty(captionProperty);
        } else {
            ret = OHOS::Accessibility::RET_ERR_INVALID_PARAM;
        }
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionFontEdgeType argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionBackgroundColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    uint32_t color = captionProperty.GetBackgroundColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionBackgroundColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("SetCaptionBackgroundColor argc > 1");
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
        OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
        instance.GetCaptionsProperty(captionProperty);
        // Change the input info and then set the CaptionProperty
        captionProperty.SetBackgroundColor(color);
        OHOS::Accessibility::RetError ret = instance.SetCaptionsProperty(captionProperty);
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionBackgroundColor argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

napi_value NAccessibilityClient::GetCaptionWindowColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    napi_value returnValue = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty = {};
    instance.GetCaptionsProperty(captionProperty);
    uint32_t color = captionProperty.GetWindowColor();
    std::string colorStr = ConvertColorToString(color);
    napi_create_string_utf8(env, colorStr.c_str(), NAPI_AUTO_LENGTH, &returnValue);
    return returnValue;
}

napi_value NAccessibilityClient::SetCaptionWindowColor(napi_env env, napi_callback_info info)
{
    HILOG_INFO();
    size_t argc = ARGS_SIZE_ONE;
    napi_value parameters[ARGS_SIZE_ONE] = {0};
    napi_get_cb_info(env, info, &argc, parameters, nullptr, nullptr);
    if (argc >= ARGS_SIZE_ONE) {
        HILOG_DEBUG("SetCaptionWindowColor argc > 1");
        uint32_t color = GetColorValue(env, parameters[PARAM0]);
        auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
        OHOS::AccessibilityConfig::CaptionProperty captionProperty {};
        instance.GetCaptionsProperty(captionProperty);
        // Change the input info and then set the CaptionProperty
        captionProperty.SetWindowColor(color);
        OHOS::Accessibility::RetError ret = instance.SetCaptionsProperty(captionProperty);
        if (ret != OHOS::Accessibility::RET_OK) {
            napi_value err = CreateBusinessError(env, ret);
            napi_throw(env, err);
        }
    } else {
        HILOG_ERROR("SetCaptionWindowColor argc size Error");
        napi_value err = CreateBusinessError(env, OHOS::Accessibility::RET_ERR_INVALID_PARAM);
        napi_throw(env, err);
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    return undefined;
}

void StateListenerImpl::SubscribeToFramework()
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->SubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("UnsubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->UnsubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::OnStateChanged(const bool state)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        observer->OnStateChanged(state);
    }
}

void StateListenerImpl::DeleteObserverReference(napi_env env, std::shared_ptr<StateListener> observer)
{
    if (observer == nullptr) {
        return;
    }
    std::shared_ptr<AccessibilityCallbackInfo> callbackInfo =
        std::make_shared<AccessibilityCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("failed to create callbackInfo");
        return;
    }
    callbackInfo->env_ = observer->env_;
    callbackInfo->ref_ = observer->handlerRef_;
    auto task = [callbackInfo]() {
        if (callbackInfo == nullptr) {
            return;
        }
        napi_env tmpEnv = callbackInfo->env_;
        auto closeScope = [tmpEnv](napi_handle_scope scope) {
            napi_close_handle_scope(tmpEnv, scope);
        };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(
            OHOS::Accessibility::TmpOpenScope(callbackInfo->env_), closeScope);
        napi_delete_reference(tmpEnv, callbackInfo->ref_);
    };
    if (napi_send_event(env, task, napi_eprio_high) != napi_status::napi_ok) {
        HILOG_ERROR("failed to send event");
    }
}

void StateListenerImpl::SubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->handlerRef_)) {
            HILOG_DEBUG("SubscribeObserver Observer exist");
            return;
        } else {
            iter++;
        }
    }

    napi_ref ref;
    napi_create_reference(env, observer, 1, &ref);
    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>(env, ref);

    observers_.emplace_back(stateListener);
    HILOG_INFO("observer size%{public}zu", observers_.size());
}

void StateListenerImpl::UnsubscribeObserver(napi_env env, napi_value observer)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if (CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->handlerRef_)) {
            DeleteObserverReference(env, *iter);
            observers_.erase(iter);
            return;
        } else {
            iter++;
        }
    }
}

void StateListenerImpl::UnsubscribeObservers()
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        DeleteObserverReference((*iter)->env_, *iter);
        observers_.erase(iter);
    }
    observers_.clear();
}