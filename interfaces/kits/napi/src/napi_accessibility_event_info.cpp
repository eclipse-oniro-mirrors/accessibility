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
#include "napi_accessibility_utils.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

thread_local napi_ref  NAccessibilityEventInfo::consRef_ = nullptr;

void NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(napi_env env)
{
    napi_value constructor = nullptr;

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "EventInfo",
            NAPI_AUTO_LENGTH,
            NAccessibilityEventInfo::JSConstructor,
            nullptr,
            0,
            nullptr,
            &constructor));
    napi_create_reference(env, constructor, 1, &NAccessibilityEventInfo::consRef_);
}

napi_value NAccessibilityEventInfo::JSConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}
