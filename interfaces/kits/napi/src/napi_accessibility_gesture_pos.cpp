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

#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_gesture_pos.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

thread_local napi_ref NGesturePos::consRef_;

void NGesturePos::DefineJSGesturePos(napi_env env)
{
    napi_value constructor = nullptr;

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "GesturePos",
            NAPI_AUTO_LENGTH,
            NGesturePos::JSPosConstructor,
            nullptr,
            0,
            nullptr,
            &constructor));
    
    napi_create_reference(env, constructor, 1, &NGesturePos::consRef_);
}

napi_value NGesturePos::JSPosConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr));
    uint32_t x = 0, y = 0;
    ParseUint32(env, x, argv[PARAM0]);
    ParseUint32(env, y, argv[PARAM1]);
    HILOG_INFO("JSPosConstructor, x:%{public}u, y:%{public}u", x, y);
    napi_set_named_property(env, jsthis, "posX", argv[PARAM0]);
    napi_set_named_property(env, jsthis, "posY", argv[PARAM1]);

    return jsthis;
}