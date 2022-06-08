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

#ifndef NAPI_ACCESSIBILITY_CONFIG_OBSERVER_H
#define NAPI_ACCESSIBILITY_CONFIG_OBSERVER_H

#include <map>
#include <string>
#include "accessibility_config.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

const uint32_t CONFIG_START_WORK_ARGS_SIZE = 2;

class NAccessibilityConfigObserver : public OHOS::AccessibilityConfig::AccessibilityConfigObserver {
public:
    NAccessibilityConfigObserver() = default;
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

    void SetEnv(const napi_env env)
    {
        env_ = env;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    void SetHandler(const napi_ref handler)
    {
        handlerRef_ = handler;
    }

    napi_ref GetHandler() const
    {
        return handlerRef_;
    }

    void SetConfigId(const OHOS::AccessibilityConfig::CONFIG_ID id)
    {
        configId_ = id;
    }

    OHOS::AccessibilityConfig::CONFIG_ID GetConfigId() const
    {
        return configId_;
    }
private:
    static void NotifyStateChangedJS(napi_env env, bool enabled, const OHOS::AccessibilityConfig::CONFIG_ID id,
                                     napi_ref handlerRef);
    static void NotifyPropertyChangedJS(napi_env env, OHOS::AccessibilityConfig::CaptionProperty caption,
                                        const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyStringChanged2JSInner(napi_env env, const std::string& value,
                                            const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyIntChanged2JSInner(
        napi_env env, int32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyUintChanged2JSInner(
        napi_env env, uint32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyFloatChanged2JSInner(
        napi_env env, float value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);

private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    OHOS::AccessibilityConfig::CONFIG_ID configId_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
};
#endif  // NAPI_ACCESSIBILITY_CONFIG_OBSERVER_H