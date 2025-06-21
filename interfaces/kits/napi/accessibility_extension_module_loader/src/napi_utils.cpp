/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "napi_utils.h"
#include "accessibility_utils.h"
#include "hilog_wrapper.h"


namespace OHOS::Accessibility {
NAPICbInfo::NAPICbInfo(size_t capability)
{
    argc = capability;
}

NAPICbInfo::~NAPICbInfo()
{
    if (argv) {
        delete[] argv;
        argv = nullptr;
    }
}

RetError NAPICbInfo::Init(napi_env env, napi_callback_info info)
{
    if (!argv) {
        argv = new(std::nothrow) napi_value[argc];
    }

    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR("Failed to get cb info");
        return RetError::RET_ERR_FAILED;
    }

    this->env = env;
    return RetError::RET_OK;
}

RetError NAPICbInfo::ParseInt64(size_t paramIndex, int64_t& result)
{
    if (!argv) {
        HILOG_ERROR("argv is null");
        return RetError::RET_ERR_NULLPTR;
    }

    if (argc < paramIndex + 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        return RetError::RET_ERR_INVALID_PARAM;
    }

    if (!OHOS::AccessibilityNapi::ParseInt64(env, result, argv[paramIndex])) {
        HILOG_ERROR("parse int64 failed");
        return RetError::RET_ERR_INVALID_PARAM;
    }

    return RetError::RET_OK;
}

RetError NAPICbInfo::ParseString(size_t paramIndex, std::string& result)
{
    if (!argv) {
        HILOG_ERROR("argv is null");
        return RetError::RET_ERR_NULLPTR;
    }

    if (argc < paramIndex + 1) {
        HILOG_ERROR("argc is invalid: %{public}zu", argc);
        return RetError::RET_ERR_INVALID_PARAM;
    }

    if (!OHOS::AccessibilityNapi::ParseString(env, result, argv[paramIndex])) {
        HILOG_ERROR("parse string failed");
        return RetError::RET_ERR_INVALID_PARAM;
    }

    return RetError::RET_OK;
}
} // namespace OHOS::Accessibility
