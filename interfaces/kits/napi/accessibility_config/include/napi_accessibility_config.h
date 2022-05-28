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

#ifndef NAPI_ACCESSIBILITY_CONFIG_H
#define NAPI_ACCESSIBILITY_CONFIG_H

#include <vector>
#include <map>
#include "napi_accessibility_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_config.h"

const uint32_t CONFIG_START_WORK_ARGS_SIZE = 2;
class ConfigListener : public OHOS::AccessibilityConfig::AccessibilityConfigObserver {
public:
    ConfigListener();
    void NotifyStateChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, bool enabled);
    void NotifyStringChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, const std::string &value);
    void NotifyIntChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, int32_t value);
    void NotifyUintChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, uint32_t value);
    void NotifyFloatChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, float value);
    void NotifyPropertyChanged2JS(
        const OHOS::AccessibilityConfig::CONFIG_ID id, OHOS::AccessibilityConfig::CaptionProperty caption);

    napi_value StartWork(napi_env env, size_t functionIndex, napi_value (&args)[CONFIG_START_WORK_ARGS_SIZE]);
    virtual void OnConfigChanged(
        const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value) override;
    OHOS::AccessibilityConfig::CONFIG_ID GetStateType();

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
    static void NotifyStateChangedJS(napi_env env, bool enabled, const std::string &eventType, napi_ref handlerRef);
    static void NotifyPropertyChangedJS(napi_env env, const OHOS::AccessibilityConfig::CaptionProperty &caption,
        const std::string &eventType, napi_ref handlerRef);
    static void NotifyStringChanged2JSInner(
        napi_env env, const std::string& value, const std::string& eventType, napi_ref handlerRef);
    static void NotifyIntChanged2JSInner(
        napi_env env, int32_t value, const std::string& eventType, napi_ref handlerRef);
    static void NotifyUintChanged2JSInner(
        napi_env env, uint32_t value, const std::string& eventType, napi_ref handlerRef);
    static void NotifyFloatChanged2JSInner(
        napi_env env, float value, const std::string& eventType, napi_ref handlerRef);

private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    std::string eventType_ = "";
    std::string description_ = "";
};

struct NAccessibilityConfigStruct {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref thisRefer_ {};
    napi_ref callback_ {};
    napi_env env_ {};
    OHOS::AccessibilityConfig::CaptionProperty captionProperty_ {};
    bool setCaptionPropertyReturn_ = false;
    bool captionState_ = false;
    bool setCaptionStateReturn_ = false;
    bool result_ = false;

    std::vector<std::shared_ptr<ConfigListener>> configListener_;
};
class EnableAbilityListsObserver : public OHOS::AccessibilityConfig::AccessibilityEnableAbilityListsObserver {
public:
    EnableAbilityListsObserver() = default;
    void OnEnableAbilityListsStateChanged() override;

    void SetCallBack(napi_ref callback)
    {
        callback_ = callback;
    }

    void SetEnv(napi_env env)
    {
        env_ = env;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

private:
    napi_ref callback_ = nullptr;
    napi_env env_ = nullptr;
};

struct NAccessibilityConfigData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref callback_ {};
    std::string abilityName_ = "";
    uint32_t capabilities_ = 0;
    bool ret_ = false;
};

class NAccessibilityConfig {
public:
    static napi_value EnableAbility(napi_env env, napi_callback_info info);
    static napi_value DisableAbility(napi_env env, napi_callback_info info);
    static napi_value SubscribeState(napi_env env, napi_callback_info info);
    static napi_value UnsubscribeState(napi_env env, napi_callback_info info);
    static std::vector<std::shared_ptr<EnableAbilityListsObserver>> enableAbilityListsObservers_;
    static std::vector<std::shared_ptr<ConfigListener>> configListeners_;
private:
    NAccessibilityConfig() = default;
    ~NAccessibilityConfig() = default;
};

#endif  // NAPI_ACCESSIBILITY_CONFIG_H
