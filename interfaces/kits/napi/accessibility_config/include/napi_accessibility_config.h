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
#include <mutex>
#include "accessibility_utils.h"
#include "accessibility_config_observer.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_config.h"

namespace OHOS {
namespace Accessibility {
napi_handle_scope TmpOpenScope(napi_env env);
} // namespace Accessibility
} // namespace OHOS

struct EnableAbilityListsObserver {
    EnableAbilityListsObserver(napi_env env, napi_ref callback) : env_(env), callback_(callback) {};
    void OnEnableAbilityListsStateChanged();
    int OnEnableAbilityListsStateChangedWork(uv_work_t *work);
    napi_env env_ = nullptr;
    napi_ref callback_ = nullptr;
};

class EnableAbilityListsObserverImpl : public OHOS::AccessibilityConfig::AccessibilityEnableAbilityListsObserver,
    public std::enable_shared_from_this<EnableAbilityListsObserverImpl> {
public:
    EnableAbilityListsObserverImpl() = default;
    void OnEnableAbilityListsStateChanged() override;
    void OnInstallAbilityListsStateChanged() override;
    void SubscribeToFramework();
    void SubscribeObserver(napi_env env, napi_value observer);
    void SubscribeInstallObserver(napi_env env, napi_value observer);
    void UnsubscribeObserver(napi_env env, napi_value observer);
    void UnsubscribeObservers();
    void UnsubscribeInstallObserver(napi_env env, napi_value observer);
    void UnsubscribeInstallObservers();

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<EnableAbilityListsObserver>> enableAbilityListsObservers_ = {};
    std::vector<std::shared_ptr<EnableAbilityListsObserver>> installAbilityListsObservers_ = {};
};

struct NAccessibilityConfigData {
    napi_async_work work_ {};
    napi_deferred deferred_ {};
    napi_ref callback_ {};
    std::string abilityName_ = "";
    uint32_t capabilities_ = 0;
    std::string stringConfig_ = "";
    uint32_t uint32Config_ = 0;
    int32_t int32Config_ = 0;
    float floatConfig_ = 0;
    std::vector<std::string> stringVectorConfig_ {};
    OHOS::AccessibilityConfig::CaptionProperty captionProperty_ {};
    OHOS::AccessibilityConfig::CONFIG_ID id_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
    bool boolConfig_ = false;
    OHOS::Accessibility::RetError ret_ = OHOS::Accessibility::RET_ERR_FAILED;
};

class NAccessibilityConfigClass {
public:
    NAccessibilityConfigClass(OHOS::AccessibilityConfig::CONFIG_ID id)
        : id_(id) {}
    ~NAccessibilityConfigClass()
    {
        napi_delete_reference(env_, wrapper_);
    }

    static void Destructor(napi_env env, void* nativeObject, void*)
    {
        reinterpret_cast<NAccessibilityConfigClass*>(nativeObject)->~NAccessibilityConfigClass();
    }

    OHOS::AccessibilityConfig::CONFIG_ID GetConfigId() const
    {
        return id_;
    }

    void SetEnv(const napi_env env)
    {
        env_ = env;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    napi_ref* GetWrapper()
    {
        return &wrapper_;
    }
private:
    OHOS::AccessibilityConfig::CONFIG_ID id_;
    napi_env env_ = nullptr;
    napi_ref wrapper_ = nullptr;
};

class NAccessibilityConfig {
public:
    static napi_value EnableAbility(napi_env env, napi_callback_info info);
    static napi_value DisableAbility(napi_env env, napi_callback_info info);
    static napi_value SubscribeState(napi_env env, napi_callback_info info);
    static napi_value UnsubscribeState(napi_env env, napi_callback_info info);
    static napi_value SetConfig(napi_env env, napi_callback_info info);
    static napi_value GetConfig(napi_env env, napi_callback_info info);
    static napi_value SubscribeConfigObserver(napi_env env, napi_callback_info info);
    static napi_value UnSubscribeConfigObserver(napi_env env, napi_callback_info info);
    static std::shared_ptr<NAccessibilityConfigObserverImpl> configObservers_;
    static std::shared_ptr<EnableAbilityListsObserverImpl> enableAbilityListsObservers_;
private:
    static void AsyncWorkComplete(napi_env env, napi_status status, void* data);
    static void SetConfigExecute(napi_env env, void* data);
    static void SetScreenTouchConfigExecute(NAccessibilityConfigData* callbackInfo);
    static void GetConfigComplete(napi_env env, napi_status status, void* data);
    static void GetConfigExecute(napi_env env, void* data);
    static bool CheckReadPermission(const std::string &permission);
    static bool IsAvailable(napi_env env, napi_callback_info info);
    static void GetScreenTouchConfigExecute(NAccessibilityConfigData* callbackInfo);
    static void EnableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
        napi_env env, napi_value* parameters, NAccessibilityConfigData* callbackInfo);
    static void DisableAbilityError(size_t& argc, OHOS::Accessibility::RetError& errCode,
        napi_env env, napi_value* parameters, NAccessibilityConfigData* callbackInfo);
    static void ConfigCompleteInfoById(napi_env env, NAccessibilityConfigData* callbackInfo,
        napi_value* result, size_t len);
    static NAccessibilityConfigData* GetCallbackInfo(napi_env env, napi_callback_info info,
        napi_value* parameters, size_t& argc, NAccessibilityConfigClass*& obj);
    static bool ParseConnectTimeoutData(napi_env env, NAccessibilityConfigData* callbackInfo,
        napi_value* parameters);
    static bool ParseMouseAutoClickData(napi_env env, NAccessibilityConfigData* callbackInfo,
        napi_value* parameters);
    static bool SetConfigParseBoolData(napi_env env, NAccessibilityConfigData* callbackInfo,
        napi_value* parameters);
    static bool SetConfigParseData(napi_env env, NAccessibilityConfigClass* obj,
        NAccessibilityConfigData* callbackInfo, napi_value* parameters, size_t argc);
    NAccessibilityConfig() = default;
    ~NAccessibilityConfig() = default;
};

#endif // NAPI_ACCESSIBILITY_CONFIG_H