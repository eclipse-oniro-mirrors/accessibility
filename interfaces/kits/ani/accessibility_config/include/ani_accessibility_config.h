/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ANI_ACCESSIBILITY_CONFIG_H
#define ANI_ACCESSIBILITY_CONFIG_H

#include <vector>
#include <map>
#include <ani.h>
#include "accessibility_config.h"
#include "accessibility_caption.h"
#include "ffrt.h"

struct AccessibilityCallbackInfo {
    ani_env *env_;
    ani_ref fnRef_;
};

struct EnableAbilityListsObserver {
    EnableAbilityListsObserver(ani_env *env, ani_ref callback) : env_(env), callback_(callback) {};
    void OnEnableAbilityListsStateChanged();
    ani_env* env_;
    ani_ref callback_;
};

struct ANIStateCallbackInfo : public AccessibilityCallbackInfo {
    ani_env *env_;
    ani_ref fnRef_;
    bool state_;
    std::string stringValue_;
    uint32_t uint32Value_;
    int32_t int32Value_;
    float   floatValue_;
    std::vector<std::string> stringVector_;
};

struct ANICaptionCallbackInfo : public AccessibilityCallbackInfo {
    ani_env *env_;
    ani_ref fnRef_;
    OHOS::AccessibilityConfig::CaptionProperty caption_;
};

class EnableAbilityListsObserverImpl : public OHOS::AccessibilityConfig::AccessibilityEnableAbilityListsObserver,
    public std::enable_shared_from_this<EnableAbilityListsObserverImpl> {
public:
    EnableAbilityListsObserverImpl() = default;
    void OnEnableAbilityListsStateChanged() override;
    void OnInstallAbilityListsStateChanged() override;
    void SubscribeToFramework();
    void UnsubscribeFromFramework();
    void SubscribeObserver(ani_env *env, ani_object observer);
    void SubscribeInstallObserver(ani_env *env, ani_object observer);
    void UnsubscribeObserver(ani_env *env, ani_object observer);
    void UnsubscribeObservers();
    void UnsubscribeInstallObserver(ani_env *env, ani_object observer);
    void UnsubscribeInstallObservers();

private:
    ffrt::mutex mutex_;
    std::vector<std::shared_ptr<EnableAbilityListsObserver>> enableAbilityListsObservers_ = {};
    std::vector<std::shared_ptr<EnableAbilityListsObserver>> installAbilityListsObservers_ = {};
};

struct ANIAccessibilityConfigObserver {
public:
    ANIAccessibilityConfigObserver(ani_env* env, ani_ref callback, OHOS::AccessibilityConfig::CONFIG_ID id)
        : env_(env), handlerRef_(callback), configId_(id) {};
    void OnConfigChanged(const OHOS::AccessibilityConfig::ConfigValue& value);
    void OnConfigChangedExtra(const OHOS::AccessibilityConfig::ConfigValue& value);

    void OnDaltonizationColorFilterConfigChanged();
    void NotifyStateChangedToJs(bool enabled);
    void NotifyPropertyChangedToJs(const OHOS::AccessibilityConfig::CaptionProperty &caption);
    void NotifyStringChangedToJs(const std::string& value);
    void NotifyStringVectorChangedToJs(std::vector<std::string> value);
    void NotifyIntChangedToJs(int32_t value);
    void NotifyUintChangedToJs(uint32_t value);
    void NotifyFloatChangedToJs(float value);

    ani_env* env_ = nullptr;
    ani_ref handlerRef_ = nullptr;
    OHOS::AccessibilityConfig::CONFIG_ID configId_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
};

class ANIAccessibilityConfigObserverImpl : public OHOS::AccessibilityConfig::AccessibilityConfigObserver,
    public std::enable_shared_from_this<ANIAccessibilityConfigObserverImpl> {
public:
    ANIAccessibilityConfigObserverImpl() = default;
    void OnConfigChanged(
        const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value) override;
    void SubscribeToFramework();
    void UnsubscribeFromFramework();
    void SubscribeObserver(ani_env* env, OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer);
    void UnsubscribeObserver(ani_env* env, OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer);
    void UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id);

private:
    ffrt::mutex mutex_;
    std::vector<std::shared_ptr<ANIAccessibilityConfigObserver>> observers_ = {};
};

class ANIAccessibilityConfig {
public:
    static void SetSyncboolean(ani_env *env, ani_object object, ani_enum_item id, ani_boolean state);
    static ani_boolean GetSyncboolean(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncInt(ani_env *env, ani_object object, ani_enum_item id, ani_int value);
    static ani_int GetSyncInt(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncDouble(ani_env *env, ani_object object, ani_enum_item id, ani_double value);
    static ani_double GetSyncDouble(ani_env *env, ani_object object, ani_enum_item id);
    static void SubscribeConfigObserver(ani_env *env, ani_object object, ani_enum_item id, ani_object callback);
    static void UnSubscribeConfigObservers(ani_env *env, ani_object object, ani_enum_item id);
    static void UnSubscribeConfigObserver(ani_env *env, ani_object object, ani_enum_item id, ani_object observer);
    static void SetSyncString(ani_env *env, ani_object object, ani_enum_item id, ani_string value);
    static ani_string GetSyncString(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncVectorString(ani_env *env, ani_object object, ani_enum_item id, ani_object value);
    static ani_object GetSyncVectorString(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncDaltonizationColorFilter(ani_env *env, ani_object object, ani_enum_item id, ani_string value);
    static ani_string GetSyncDaltonizationColorFilter(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncClickResponseTime(ani_env *env, ani_object object, ani_enum_item id, ani_string value);
    static ani_string GetSyncClickResponseTime(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncRepeatClickInterval(ani_env *env, ani_object object, ani_enum_item id, ani_string value);
    static ani_string GetSyncRepeatClickInterval(ani_env *env, ani_object object, ani_enum_item id);
    static void SetSyncCaptionsStyle(ani_env *env, ani_object object, ani_enum_item id, ani_object value);
    static ani_object GetSyncCaptionsStyle(ani_env *env, ani_object object, ani_enum_item id);
    static void EnableAbilitySync(ani_env *env, ani_string name, ani_array_ref capability);
    static void DisableAbilitySync(ani_env *env, ani_string name);
    static void SubscribeState(ani_env *env, ani_string type, ani_object observer);
    static void UnsubscribeState(ani_env *env, ani_string type, ani_object observer);
    static void UnsubscribeStates(ani_env *env, ani_string type);
    static ani_object CreateJsAccessibilityCaptionProperty(ani_env *env,
        OHOS::AccessibilityConfig::CaptionProperty &captionProperty);
    static ani_object CreateJsCaptionPropertyInfoInner(ani_env *env, ani_class cls,
        ani_object &object, OHOS::AccessibilityConfig::CaptionProperty &captionProperty);
    static std::shared_ptr<EnableAbilityListsObserverImpl> enableAbilityListsObservers_;
    static std::shared_ptr<ANIAccessibilityConfigObserverImpl> configObservers_;
private:
    static bool CheckReadPermission(const std::string &permission);
    static bool IsAvailable(ani_env *env);
    ANIAccessibilityConfig() = default;
    ~ANIAccessibilityConfig() = default;
};

#endif // ANI_ACCESSIBILITY_CONFIG_H