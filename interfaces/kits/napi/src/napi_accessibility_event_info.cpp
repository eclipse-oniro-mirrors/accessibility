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
#include "accessibility_utils.h"

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
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc == ARGS_SIZE_ONE) {
        napi_valuetype valueType;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
        if (valueType != napi_object) {
            HILOG_ERROR("valueType %{public}d is not napi_object", valueType);
            return nullptr;
        }

        return argv[PARAM0];
    } else if (argc == ARGS_SIZE_THREE) {
        napi_valuetype valueType;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
        if (valueType != napi_string) {
            HILOG_ERROR("type mismatch for parameter 0");
            return nullptr;
        }

        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valueType));
        if (valueType != napi_string) {
            HILOG_ERROR("type mismatch for parameter 1");
            return nullptr;
        }

        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valueType));
        if (valueType != napi_string) {
            HILOG_ERROR("type mismatch for parameter 2");
            return nullptr;
        }

        NAPI_CALL(env, napi_set_named_property(env, thisVar, "type", argv[PARAM0]));
        NAPI_CALL(env, napi_set_named_property(env, thisVar, "bundleName", argv[PARAM1]));
        NAPI_CALL(env, napi_set_named_property(env, thisVar, "triggerAction", argv[PARAM2]));
        return thisVar;
    }

    return nullptr;
}