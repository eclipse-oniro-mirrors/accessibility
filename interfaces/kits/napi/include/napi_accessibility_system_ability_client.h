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

#ifndef NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H
#define NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H

#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_state_event.h"
#include "accessibility_ability_info.h"

class StateListener : public OHOS::Accessibility::AccessibilityStateObserver {
public:
    StateListener();
    static void NotifyJS(napi_env env, bool enabled, std::string stateType, std::string desc, napi_ref handlerRef);
    std::string GetEventType() {return eventType_;}
    napi_value StartWork(napi_env env, size_t functionIndex, napi_value *args);
    void OnStateChanged(const OHOS::Accessibility::AccessibilityStateEvent& stateEvent) override;
    OHOS::Accessibility::AccessibilityStateEventType GetStateType();
    napi_env GetEnv() {return env_;};
    napi_ref GetHandler() {return handlerRef_;}
private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    std::string eventType_ = "";
    std::string description_ = "";
};

struct NAccessibilitySystemAbilityClient {  
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref thisRefer_ {};
    napi_ref callback_ {};
    napi_env env_ {};
    bool enabled_ = false;
    bool touchEnabled_ = false;
    OHOS::Accessibility::AbilityStateType stateTypes_;
    int32_t abilityTypes_ = 0;
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> abilityList_ {};
    OHOS::Accessibility::AccessibilityEventInfo eventInfo_ {};
    bool result_ = false;
    std::vector<std::shared_ptr<StateListener>> stateListener_ {};
    std::string eventType_ = "";
};

napi_value IsOpenAccessibility(napi_env env, napi_callback_info info);
napi_value IsOpenTouchExploration(napi_env env, napi_callback_info info);
napi_value GetAbilityList(napi_env env, napi_callback_info info);
napi_value SubscribeState(napi_env env, napi_callback_info info);
napi_value UnsubscribeState(napi_env env, napi_callback_info info);
napi_value SendEvent(napi_env env, napi_callback_info info);
#endif // NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H