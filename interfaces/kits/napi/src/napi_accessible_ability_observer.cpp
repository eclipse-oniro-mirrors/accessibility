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

#include <map>
#include <memory>
#include <string>

#include "hilog_wrapper.h"
#include "napi_accessible_ability_observer.h"
#include "napi_accessible_ability.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

extern std::map<std::string, std::shared_ptr<AccessibleAbilityCallbackInfo>> g_JsAACallbacks;

void AccessibleAbilityObserver::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) {
    HILOG_INFO("NAPI OnAccessibilityEvent called");
    if (!g_JsAACallbacks[STR_A11Y_OBSERVER_ACCESSIBILITY_EVENT]) {
        HILOG_WARN("This callback is not registered by ability.");
        return;
    }

    std::shared_ptr<AccessibleAbilityCallbackInfo> callbackInfo =
        g_JsAACallbacks[STR_A11Y_OBSERVER_ACCESSIBILITY_EVENT];
    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value argv = 0;
    napi_get_undefined(callbackInfo->env_, &undefined);
    napi_new_instance(callbackInfo->env_, NAccessibilityEventInfo::cons_, 0, nullptr, &argv);
    ConvertAccessibilityEventInfoToJS(callbackInfo->env_, argv, eventInfo);
    napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
    napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &argv, &result);
}

void AccessibleAbilityObserver::OnInterrupt() {
    HILOG_INFO("NAPI OnInterrupt called");
    if (!g_JsAACallbacks[STR_A11Y_OBSERVER_INTERRUPT]) {
        HILOG_WARN("This callback is not registered by ability.");
        return;
    }

    std::shared_ptr<AccessibleAbilityCallbackInfo> callbackInfo = g_JsAACallbacks[STR_A11Y_OBSERVER_INTERRUPT];
    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(callbackInfo->env_, &undefined);
    napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
    napi_call_function(callbackInfo->env_, undefined, callback, 0, nullptr, &result);
}

void AccessibleAbilityObserver::OnAbilityConnected() {
    HILOG_INFO("NAPI OnAbilityConnected called");
    if (!g_JsAACallbacks[STR_A11Y_OBSERVER_ABILITY_CONNECTED]) {
        HILOG_WARN("This callback is not registered by ability.");
        return;
    }

    std::shared_ptr<AccessibleAbilityCallbackInfo> callbackInfo = g_JsAACallbacks[STR_A11Y_OBSERVER_ABILITY_CONNECTED];
    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(callbackInfo->env_, &undefined);
    napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
    napi_call_function(callbackInfo->env_, undefined, callback, 0, nullptr, &result);
}

void AccessibleAbilityObserver::OnGesture(uint32_t gestureId) {
    HILOG_INFO("NAPI OnGesture called");
    if (!g_JsAACallbacks[STR_A11Y_OBSERVER_GESTURE]) {
        HILOG_WARN("This callback is not registered by ability.");
        return;
    }

    std::shared_ptr<AccessibleAbilityCallbackInfo> callbackInfo = g_JsAACallbacks[STR_A11Y_OBSERVER_GESTURE];
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value result = 0;
    napi_value callResult = 0;
    napi_get_undefined(callbackInfo->env_, &undefined);
    napi_create_uint32(callbackInfo->env_, gestureId, &result);
    napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
    napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

bool AccessibleAbilityObserver::OnKeyPressEvent(const OHOS::MMI::KeyEvent& keyEvent) {
    HILOG_INFO("NAPI OnKeyPressEvent called");
    if (!g_JsAACallbacks[STR_A11Y_OBSERVER_KEY_PRESS]) {
        HILOG_WARN("This callback is not registered by ability.");
        return false;
    }
    return false;
}