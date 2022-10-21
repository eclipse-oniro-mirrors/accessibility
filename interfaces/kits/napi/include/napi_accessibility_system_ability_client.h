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

#ifndef NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H
#define NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H

#include <vector>
#include <map>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_config.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_state_event.h"
#include "accessibility_ability_info.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_config_observer.h"

const uint32_t START_WORK_ARGS_SIZE = 2;
struct StateListener {
    StateListener(napi_env env, napi_ref ref) : handlerRef_(ref), env_(env) {};
    static void NotifyJS(napi_env env, bool state, napi_ref handlerRef);
    void OnStateChanged(const bool state);

    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
};

class StateListenerImpl : public OHOS::Accessibility::AccessibilityStateObserver,
    public std::enable_shared_from_this<StateListenerImpl> {
public:
    StateListenerImpl(OHOS::Accessibility::AccessibilityStateEventType type) : type_(type) {};
    void OnStateChanged(const bool state) override;
    void SubscribeToFramework();
    void SubscribeObserver(napi_env env, napi_value observer);
    void UnsubscribeObserver(napi_env env, napi_value observer);
    void UnsubscribeObservers();

private:
    std::mutex mutex_;
    OHOS::Accessibility::AccessibilityStateEventType type_;
    std::vector<std::shared_ptr<StateListener>> observers_ = {};
};

struct NAccessibilitySystemAbilityClient {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref thisRefer_ {};
    napi_ref callback_ {};
    napi_env env_ {};
    bool enabled_ = false;
    bool touchEnabled_ = false;
    OHOS::Accessibility::AbilityStateType stateTypes_ = OHOS::Accessibility::ABILITY_STATE_INVALID;
    uint32_t abilityTypes_ = 0;
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> abilityList_ {};
    OHOS::AccessibilityConfig::CaptionProperty captionProperty_ {};
    bool setCaptionPropertyReturn_ = false;
    bool captionState_ = false;
    bool setCaptionStateReturn_ = false;
    bool enabledState_ = false;
    bool setEnabledReturn_ = false;
    bool touchGuideState_ = false;
    bool setTouchGuideStateReturn_ = false;
    bool gestureState_ = false;
    bool setGestureStateReturn_ = false;
    bool keyEventObserverState_ = false;
    bool setKeyEvenReturn_ = false;
    OHOS::Accessibility::AccessibilityEventInfo eventInfo_ {};
    bool result_ = false;
    OHOS::Accessibility::RetError ret_ = OHOS::Accessibility::RET_ERR_FAILED;
    std::string eventType_ = "";

    std::map<std::string, std::vector<std::shared_ptr<StateListener>>> stateListeners_;
    std::vector<std::shared_ptr<NAccessibilityConfigObserver>> captionListener_;
};

class NAccessibilityClient {
public:
    static napi_value IsOpenAccessibility(napi_env env, napi_callback_info info);
    static napi_value IsOpenTouchExploration(napi_env env, napi_callback_info info);
    static napi_value GetAbilityList(napi_env env, napi_callback_info info);
    static napi_value GetAccessibilityExtensionList(napi_env env, napi_callback_info info);
    static napi_value SubscribeState(napi_env env, napi_callback_info info);
    static napi_value UnsubscribeState(napi_env env, napi_callback_info info);
    static napi_value SendEvent(napi_env env, napi_callback_info info);
    static napi_value SendAccessibilityEvent(napi_env env, napi_callback_info info);

    static void DefineJSCaptionsManager(napi_env env);
    static napi_value AccessibleAbilityConstructor(napi_env env, napi_callback_info info);
    static napi_value GetCaptionsManager(napi_env env, napi_callback_info info);
    static napi_value GetCaptionStateEnabled(napi_env env, napi_callback_info info);
    static napi_value SetCaptionStateEnabled(napi_env env, napi_callback_info info);
    static napi_value GetCaptionStyle(napi_env env, napi_callback_info info);
    static napi_value SetCaptionStyle(napi_env env, napi_callback_info info);
    static napi_value RegisterCaptionStateCallback(napi_env env, napi_callback_info info);
    static napi_value DeregisterCaptionStateCallback(napi_env env, napi_callback_info info);

    static void DefineJSCaptionsStyle(napi_env env);
    static napi_value AccessibleAbilityConstructorStyle(napi_env env, napi_callback_info info);
    static napi_value GetCaptionsFontFamily(napi_env env, napi_callback_info info);
    static napi_value SetCaptionsFontFamily(napi_env env, napi_callback_info info);
    static napi_value GetCaptionsFontScale(napi_env env, napi_callback_info info);
    static napi_value SetCaptionsFontScale(napi_env env, napi_callback_info info);
    static napi_value GetCaptionFrontColor(napi_env env, napi_callback_info info);
    static napi_value SetCaptionFrontColor(napi_env env, napi_callback_info info);
    static napi_value GetCaptionFontEdgeType(napi_env env, napi_callback_info info);
    static napi_value SetCaptionFontEdgeType(napi_env env, napi_callback_info info);
    static napi_value GetCaptionBackgroundColor(napi_env env, napi_callback_info info);
    static napi_value SetCaptionBackgroundColor(napi_env env, napi_callback_info info);
    static napi_value GetCaptionWindowColor(napi_env env, napi_callback_info info);
    static napi_value SetCaptionWindowColor(napi_env env, napi_callback_info info);

    static thread_local napi_ref aaConsRef_;
    static thread_local napi_ref aaStyleConsRef_;
    static std::shared_ptr<StateListenerImpl> accessibilityStateListeners_;
    static std::shared_ptr<StateListenerImpl> touchGuideStateListeners_;
    static std::shared_ptr<NAccessibilityConfigObserverImpl> captionListeners_;

private:
    static void GetAbilityListExecute(napi_env env, void* data);
    static void GetAbilityListComplete(napi_env env, napi_status status, void* data);
    static void SendEventExecute(napi_env env, void* data);
    static void SendEventComplete(napi_env env, napi_status status, void* data);
    static bool CheckAbilityType(const std::string& abilityType);
    static bool CheckStateType(const std::string& stateType);
    NAccessibilityClient() = default;
    ~NAccessibilityClient() = default;
};

#endif // NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H