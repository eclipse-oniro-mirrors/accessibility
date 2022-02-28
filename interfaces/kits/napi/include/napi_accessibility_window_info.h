/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NAPI_ACCESSIBILITY_WINDOW_H
#define NAPI_ACCESSIBILITY_WINDOW_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_window_info.h"
#include "accessibility_element_info.h"

struct NAccessibilityWindowInfoData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref thisRefer_ {};
    napi_ref callback_ {};
    napi_env env_ {};
    bool result_ = false;
    OHOS::Accessibility::AccessibilityWindowInfo nativeWindowInfo_;
    OHOS::Accessibility::AccessibilityElementInfo nodeInfo_ {};
    OHOS::Accessibility::AccessibilityWindowInfo window_ {};
    std::string content_ = "";
    int childIndex_ = 0;
};

class NAccessibilityWindowInfo {
public:
    static void DefineJSAccessibilityWindowInfo(napi_env env);
    static napi_value JSConstructor(napi_env env, napi_callback_info info);

    static napi_value GetAnchorElementInfo(napi_env env, napi_callback_info info);
    static napi_value GetRootElementInfo(napi_env env, napi_callback_info info);
    static napi_value GetParent(napi_env env, napi_callback_info info);
    static napi_value GetChild(napi_env env, napi_callback_info info);

    static napi_value cons_;
    static napi_ref consRef_;
private:
    NAccessibilityWindowInfo() = default;
    ~NAccessibilityWindowInfo() = default;
};
#endif // NAPI_ACCESSIBILITY_WINDOW_H