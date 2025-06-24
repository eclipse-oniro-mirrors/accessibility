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

#ifndef NAPI_UTILS_H
#define NAPI_UTILS_H

#include "accessibility_def.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Accessibility {
class NAPICbInfo {
public:
    NAPICbInfo(size_t capability);
    ~NAPICbInfo();
    RetError Init(napi_env env, napi_callback_info info);
    RetError ParseInt64(size_t paramIndex, int64_t& result);
    RetError ParseString(size_t paramIndex, std::string& result);

    size_t argc;
    napi_value* argv = nullptr;
    napi_value thisVar = nullptr;
    napi_env env = nullptr;
};
} // namespace OHOS::Accessibility

#endif // NAPI_UTILS_H