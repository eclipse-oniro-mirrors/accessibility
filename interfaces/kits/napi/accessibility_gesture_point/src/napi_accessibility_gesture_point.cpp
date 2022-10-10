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

#include "napi_accessibility_gesture_point.h"
#include "napi_accessibility_def.h"
#include "hilog_wrapper.h"

using namespace OHOS;

napi_value NAccessibilityGesturePoint::JSConstructor(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG();
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_valuetype valueType;
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr));
    if (argc != ARGS_SIZE_TWO) {
        HILOG_ERROR("argc %{public}zu is not 2", argc);
        return nullptr;
    }
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_number) {
        HILOG_ERROR("PARAM0's valueType %{public}d is not napi_number", valueType);
        return nullptr;
    }
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
    if (valueType != napi_number) {
        HILOG_ERROR("PARAM1's valueType %{public}d is not napi_number", valueType);
        return nullptr;
    }
    NAPI_CALL(env, napi_set_named_property(env, jsthis, "positionX", argv[PARAM0]));
    NAPI_CALL(env, napi_set_named_property(env, jsthis, "positionY", argv[PARAM1]));
    return jsthis;
}
