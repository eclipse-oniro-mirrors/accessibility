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

#include "napi_accessibility_event_info.h"
#include "hilog_wrapper.h"
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

void NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(napi_env env, napi_value exports)
{
    napi_value constructor = nullptr;

    NAPI_CALL_RETURN_VOID(env, napi_define_class(env, "EventInfo", sizeof("EventInfo"),
        NAccessibilityEventInfo::JSConstructor, nullptr, 0, nullptr, &constructor));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "EventInfo", constructor));
}

napi_value NAccessibilityEventInfo::JSConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_valuetype valueType;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != ARGS_SIZE_ONE) {
        HILOG_ERROR("argc %{public}zu is not 1", argc);
        return nullptr;
    }
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_object) {
        HILOG_ERROR("valueType %{public}d is not napi_object", valueType);
        return nullptr;
    }
    return argv[PARAM0];
}
