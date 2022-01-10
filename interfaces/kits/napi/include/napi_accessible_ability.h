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

#ifndef NAPI_ACCESSIBLE_ABILITY_H
#define NAPI_ACCESSIBLE_ABILITY_H

#include <vector>
#include <stdint.h>

#include "accessibility_window_info.h"
#include "accessible_ability_listener.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

const std::string STR_A11Y_OBSERVER_ABILITY_CONNECTED = "AbilityConnected";
const std::string STR_A11Y_OBSERVER_INTERRUPT = "interrupt";
const std::string STR_A11Y_OBSERVER_ACCESSIBILITY_EVENT = "accessibilityEvent";
const std::string STR_A11Y_OBSERVER_GESTURE = "gesture";
const std::string STR_A11Y_OBSERVER_KEY_PRESS = "keyPress";

struct AsyncGetAccessibleAbilityCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
};

struct AsyncDisableAbilityCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
};

struct AsyncGetWindowsCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> accessibilityWindows_;
};

struct AsyncGetRootElementInfoCallbackInfo {
public:
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
    std::optional<OHOS::Accessibility::AccessibilityElementInfo> elementInfo_;
    bool result_ = false;
};

struct AccessibleAbilityCallbackInfo {
    napi_env env_;
    // napi_async_work asyncWork_;
    // napi_deferred deferred_;
    napi_ref callback_ = 0;
};

struct AsyncGetFocusElementInfoCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
    std::string focusType_ = "";
    std::optional<OHOS::Accessibility::AccessibilityElementInfo> elementInfo_;
    bool result_ = false;
};

struct AsyncPerformCommonActionCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
    uint32_t action_ = 0xFFFFFFFF;
    bool performActionResult_ = false;
};

void CreateGlobalAccessibleAbilityJSObject(napi_env env);
napi_value AccessibleAbilityConstructor(napi_env env, napi_callback_info info);
napi_value GetAccessibleAbility(napi_env env, napi_callback_info info);

napi_value RegisterCallback(napi_env env, napi_callback_info info);
napi_value DisableAbility(napi_env env, napi_callback_info info);
napi_value GetWindows(napi_env env, napi_callback_info info);
napi_value GetRootElementInfo(napi_env env, napi_callback_info info);
napi_value GetFocusElementInfo(napi_env env, napi_callback_info info);
napi_value PerformCommonAction(napi_env env, napi_callback_info info);

// Dummy
napi_value ConnectToAAMS(napi_env env, napi_callback_info info);
#endif // NAPI_ACCESSIBLE_ABILITY_H