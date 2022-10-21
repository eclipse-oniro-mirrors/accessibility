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

#ifndef NAPI_ACCESSIBILITY_DEF_H
#define NAPI_ACCESSIBILITY_DEF_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "accessibility_def.h"

constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ZERO = 0;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;
constexpr int32_t CHAE_BUFFER_MAX = 1024;
const std::string ERROR_MESSAGE_PARAMETER_ERROR = "Input parameter error";
const std::string ERROR_MESSAGE_NO_PERMISSION = "Permission denied";
const std::string ERROR_MESSAGE_NO_RIGHT = "Do not have accessibility right for this operation";
const std::string ERROR_MESSAGE_SYSTEM_ABNORMALITY = "System abnormality";
const std::string ERROR_MESSAGE_PROPERTY_NOT_EXIST = "This property does not exist";
const std::string ERROR_MESSAGE_ACTION_NOT_SUPPORT = "This action is not supported";
const std::string ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME = "Invalid bundle name or ability name";
const std::string ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED = "Target ability already enabled";

enum class NAccessibilityErrorCode : int32_t {
    ACCESSIBILITY_OK = 0,
    ACCESSIBILITY_ERROR_NO_PERMISSION = 201,
    ACCESSIBILITY_ERROR_INVALID_PARAM = 401,
    ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY = 9300000,
    ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME = 9300001,
    ACCESSIBILITY_ERROR_TARGET_ABILITY_ALREADY_ENABLED = 9300002,
    ACCESSIBILITY_ERROR_NO_RIGHT = 9300003,
    ACCESSIBILITY_ERROR_PROPERTY_NOT_EXIST = 9300004,
    ACCESSIBILITY_ERROR_ACTION_NOT_SUPPORT = 9300005,
};

struct NAccessibilityErrMsg {
    NAccessibilityErrorCode errCode;
    std::string message;
};

const std::map<OHOS::Accessibility::RetError, NAccessibilityErrMsg> ACCESSIBILITY_JS_TO_ERROR_CODE_MAP {
    {OHOS::Accessibility::RetError::RET_OK,
        {NAccessibilityErrorCode::ACCESSIBILITY_OK, ""}},
    {OHOS::Accessibility::RetError::RET_ERR_FAILED,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_INVALID_PARAM,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM, ERROR_MESSAGE_PARAMETER_ERROR}},
    {OHOS::Accessibility::RetError::RET_ERR_NULLPTR,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_IPC_FAILED,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_SAMGR,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_PERMISSION,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_PERMISSION, ERROR_MESSAGE_NO_PERMISSION}},
    {OHOS::Accessibility::RetError::RET_ERR_TIME_OUT,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_REGISTER_EXIST,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_REGISTER,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_CONNECTION_EXIST,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_TARGET_ABILITY_ALREADY_ENABLED,
        ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_CONNECTION,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_WINDOW_CONNECTION,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_CAPABILITY,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_RIGHT, ERROR_MESSAGE_NO_RIGHT}},
    {OHOS::Accessibility::RetError::RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_PERFORM_ACTION_FAILED_BY_ACE,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NO_INJECTOR,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY, ERROR_MESSAGE_SYSTEM_ABNORMALITY}},
    {OHOS::Accessibility::RetError::RET_ERR_NOT_INSTALLED,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME,
        ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME}},
    {OHOS::Accessibility::RetError::RET_ERR_NOT_ENABLED,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME,
        ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME}},
    {OHOS::Accessibility::RetError::RET_ERR_PROPERTY_NOT_EXIST,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_PROPERTY_NOT_EXIST, ERROR_MESSAGE_PROPERTY_NOT_EXIST}},
    {OHOS::Accessibility::RetError::RET_ERR_ACTION_NOT_SUPPORT,
        {NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ACTION_NOT_SUPPORT, ERROR_MESSAGE_ACTION_NOT_SUPPORT}},
};
#endif // NAPI_ACCESSIBILITY_DEF_H