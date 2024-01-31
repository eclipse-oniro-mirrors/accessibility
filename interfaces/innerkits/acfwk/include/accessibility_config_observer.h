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

#ifndef ACCESSIBILITY_CONFIG_OBSERVER_H
#define ACCESSIBILITY_CONFIG_OBSERVER_H

#include <map>
#include <string>
#include <uv.h>
#include "accessibility_config.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Accessibility {
napi_handle_scope TmpOpenScope(napi_env env);
} // namespace Accessibility
} // namespace OHOS

const uint32_t CONFIG_START_WORK_ARGS_SIZE = 2;

struct NAccessibilityConfigObserver {
public:
    NAccessibilityConfigObserver(napi_env env, napi_ref callback, OHOS::AccessibilityConfig::CONFIG_ID id)
        : env_(env), handlerRef_(callback), configId_(id) {};
    void OnConfigChanged(const OHOS::AccessibilityConfig::ConfigValue& value);
    void OnConfigChangedExtra(const OHOS::AccessibilityConfig::ConfigValue& value);

    void OnDaltonizationColorFilterConfigChanged();
    void NotifyStateChanged2JS(bool enabled);
    int NotifyStateChanged(uv_work_t *work);
    void NotifyPropertyChanged2JS(const OHOS::AccessibilityConfig::CaptionProperty &caption);
    int NotifyPropertyChanged(uv_work_t *work);
    void NotifyStringChanged2JS(const std::string& value);
    void NotifyStringVectorChanged2JS(std::vector<std::string> value);
    int NotifyStringChanged(uv_work_t *work);
    int NotifyStringVectorChanged(uv_work_t *work);
    void NotifyIntChanged2JS(int32_t value);
    int NotifyIntChanged(uv_work_t *work);
    void NotifyUintChanged2JS(uint32_t value);
    int NotifyUintChanged(uv_work_t *work);
    void NotifyFloatChanged2JS(float value);
    int NotifyFloatChanged(uv_work_t *work);

    napi_env env_ = nullptr;
    napi_ref handlerRef_ = nullptr;
    OHOS::AccessibilityConfig::CONFIG_ID configId_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
};

class NAccessibilityConfigObserverImpl : public OHOS::AccessibilityConfig::AccessibilityConfigObserver,
    public std::enable_shared_from_this<NAccessibilityConfigObserverImpl> {
public:
    NAccessibilityConfigObserverImpl() = default;
    virtual void OnConfigChanged(
        const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value) override;
    void SubscribeToFramework();
    void SubscribeObserver(napi_env env, OHOS::AccessibilityConfig::CONFIG_ID id, napi_value observer);
    void UnsubscribeObserver(napi_env env, OHOS::AccessibilityConfig::CONFIG_ID id, napi_value observer);
    void UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id);

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<NAccessibilityConfigObserver>> observers_ = {};
};
#endif // ACCESSIBILITY_CONFIG_OBSERVER_H