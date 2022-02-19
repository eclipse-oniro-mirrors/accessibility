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

const uint32_t START_WORK_ARGS_SIZE = 2;
class StateListener : public OHOS::Accessibility::AccessibilityStateObserver {
public:
    StateListener();
    static void NotifyJS(napi_env env, bool enabled, std::string stateType, std::string desc, napi_ref handlerRef);
    napi_value StartWork(napi_env env, size_t functionIndex, napi_value (&args)[START_WORK_ARGS_SIZE]);
    void OnStateChanged(const OHOS::Accessibility::AccessibilityStateEvent& stateEvent) override;
    OHOS::Accessibility::AccessibilityStateEventType GetStateType();

    std::string GetEventType() const
    {
        return eventType_;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    napi_ref GetHandler() const
    {
        return handlerRef_;
    }

private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    std::string eventType_ = "";
    std::string description_ = "";
};


class CaptionListener : public OHOS::Accessibility::CaptionObserver {
public:
    CaptionListener();
    static void NotifyStateChangedJS(napi_env env, bool enabled, std::string eventType, napi_ref handlerRef);
    static void NotifyPropertyChangedJS(napi_env env, OHOS::Accessibility::CaptionProperty caption, std::string eventType, napi_ref handlerRef);
    napi_value StartWork(napi_env env, size_t functionIndex, napi_value (&args)[START_WORK_ARGS_SIZE]);
    void OnCaptionStateChanged(const bool& enable) override;
    void OnCaptionPropertyChanged(const OHOS::Accessibility::CaptionProperty& caption) override;
    OHOS::Accessibility::CaptionObserverType GetStateType();

    std::string GetEventType() const
    {
        return eventType_;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    napi_ref GetHandler() const
    {
        return handlerRef_;
    }

private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    std::string eventType_ = "";
    std::string description_ = "";
};

struct NAccessibilitySystemAbilityClient {
    napi_async_work work_{};
    napi_deferred deferred_{};
    napi_ref thisRefer_{};
    napi_ref callback_{};
    napi_env env_{};
    bool enabled_ = false;
    bool touchEnabled_ = false;
    OHOS::Accessibility::AbilityStateType stateTypes_;
    int32_t abilityTypes_ = 0;
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> abilityList_{};
    std::map<std::string, OHOS::AppExecFwk::ElementName> enabledAbilities_{};
    OHOS::Accessibility::CaptionProperty captionProperty_{};
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
    bool setExtentionReturn_ = false;
    OHOS::Accessibility::AccessibilityEventInfo eventInfo_{};
    bool result_ = false;
    std::vector<std::shared_ptr<StateListener>> stateListener_{};
    std::vector<std::shared_ptr<CaptionListener>> captionListener_{};
    std::string eventType_ = "";
};

class NAccessibilityClient {
public:
    static napi_value IsOpenAccessibility(napi_env env, napi_callback_info info);
    static napi_value IsOpenTouchExploration(napi_env env, napi_callback_info info);
    static napi_value GetAbilityList(napi_env env, napi_callback_info info);
    static napi_value SubscribeState(napi_env env, napi_callback_info info);
    static napi_value UnsubscribeState(napi_env env, napi_callback_info info);
    static napi_value SendEvent(napi_env env, napi_callback_info info);
    static napi_value GetCaptionProperty(napi_env env, napi_callback_info info);
    static napi_value SetCaptionProperty(napi_env env, napi_callback_info info);
    static napi_value GetCaptionState(napi_env env, napi_callback_info info);
    static napi_value SetCaptionState(napi_env env, napi_callback_info info);
    static napi_value GetEnabled(napi_env env, napi_callback_info info);
    static napi_value SetEnabled(napi_env env, napi_callback_info info);
    static napi_value GetTouchGuideState(napi_env env, napi_callback_info info);
    static napi_value SetTouchGuideState(napi_env env, napi_callback_info info);
    static napi_value GetGestureState(napi_env env, napi_callback_info info);
    static napi_value SetGestureState(napi_env env, napi_callback_info info);
    static napi_value GetKeyEventObserverState(napi_env env, napi_callback_info info);
    static napi_value SetKeyEventObserverState(napi_env env, napi_callback_info info);
    static napi_value GetInstalled(napi_env env, napi_callback_info info);
    static napi_value GetExtentionEnabled(napi_env env, napi_callback_info info);
    static napi_value ExtentionEnabled(napi_env env, napi_callback_info info);
    static napi_value ExtentionDisabled(napi_env env, napi_callback_info info);

    static void DefineJSCaptionsManager(napi_env env);
    static napi_value AccessibleAbilityConstructor(napi_env env, napi_callback_info info);
    static napi_value GetCaptionsManager(napi_env env, napi_callback_info info);
    static napi_value GetCaptionStateEnabled(napi_env env, napi_callback_info info);
    static napi_value SetCaptionStateEnabled(napi_env env, napi_callback_info info);
    static napi_value GetCaptionStyle(napi_env env, napi_callback_info info);
    static napi_value SetCaptionStyle(napi_env env, napi_callback_info info);
    static napi_value RegisterCaptionStateCallback(napi_env env, napi_callback_info info);
    static napi_value DeregisterCaptionStateCallback(napi_env env, napi_callback_info info);

    static napi_value aaCons_;

    static std::vector<std::shared_ptr<StateListener>> listeners_;
    static std::vector<std::shared_ptr<CaptionListener>> captionListeners_;

private:
    NAccessibilityClient() = default;
    ~NAccessibilityClient() = default;
};

#endif  // NAPI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H