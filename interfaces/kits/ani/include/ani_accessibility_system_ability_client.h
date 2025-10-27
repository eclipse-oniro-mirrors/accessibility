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

#ifndef ANI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H
#define ANI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H

#include <vector>
#include <map>
#include <ani.h>
#include "accessibility_system_ability_client.h"
#include "accessibility_state_event.h"
#include "accessibility_config.h"
#include "ffrt.h"

struct ANIStateCallbackInfo {
    ani_env *env_;
    ani_ref fnRef_;
    bool state_;
};

struct StateListener {
    StateListener(ani_env *env, ani_ref fnRef) : env_(env), fnRef_(fnRef) {};
    static void NotifyETS(ani_env *env, bool state, ani_ref fnRef);
    void OnStateChanged(const bool state);

    ani_env* env_;
    ani_ref fnRef_;
};

class StateListenerImpl : public OHOS::Accessibility::AccessibilityStateObserver,
    public std::enable_shared_from_this<StateListenerImpl> {
public:
    StateListenerImpl(OHOS::Accessibility::AccessibilityStateEventType type) : type_(type) {};
    void OnStateChanged(const bool state) override;
    void SubscribeToFramework();
    void UnsubscribeFromFramework();
    void SubscribeObserver(ani_env *env, ani_object observer);
    void UnsubscribeObserver(ani_env *env, ani_object observer);
    void UnsubscribeObservers();

private:
    ffrt::mutex mutex_;
    OHOS::Accessibility::AccessibilityStateEventType type_;
    std::vector<std::shared_ptr<StateListener>> observers_ = {};
};

struct ANICaptionCallbackInfo {
    ani_env *env_;
    ani_ref fnRef_;
    OHOS::AccessibilityConfig::CaptionProperty caption_;
};

struct AccessibilityCaptionsObserver {
public:
    AccessibilityCaptionsObserver(ani_env *env, ani_ref fnRef, OHOS::AccessibilityConfig::CONFIG_ID id)
        : env_(env), fnRef_(fnRef), configId_(id) {};
    static void NotifyCaptionsStateToETS(ani_env *env, bool state, ani_ref fnRef);
    static void NotifyCaptionsStyleToETS(ani_env *env, OHOS::AccessibilityConfig::CaptionProperty style, ani_ref fnRef);
    void OnConfigChanged(const OHOS::AccessibilityConfig::ConfigValue& value);

    ani_env* env_;
    ani_ref fnRef_;
    OHOS::AccessibilityConfig::CONFIG_ID configId_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
};

class AccessibilityCaptionsObserverImpl : public OHOS::AccessibilityConfig::AccessibilityConfigObserver,
    public std::enable_shared_from_this<AccessibilityCaptionsObserverImpl> {
public:
    AccessibilityCaptionsObserverImpl() = default;
    void OnConfigChanged(const OHOS::AccessibilityConfig::CONFIG_ID id,
        const OHOS::AccessibilityConfig::ConfigValue& value);
    void SubscribeToFramework();
    void UnsubscribeFromFramework();
    void SubscribeObserver(ani_env *env, OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer);
    void UnsubscribeObserver(ani_env *env, OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer);
    void UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id);

private:
    ffrt::mutex mutex_;
    std::vector<std::shared_ptr<AccessibilityCaptionsObserver>> observers_ = {};
};

class ANIAccessibilityClient {
public:
    static ani_boolean IsOpenTouchGuideSync([[maybe_unused]] ani_env *env);
    static ani_boolean IsOpenAccessibilitySync([[maybe_unused]] ani_env *env);
    static ani_boolean IsScreenReaderOpenSync([[maybe_unused]] ani_env *env);
    static ani_object GetAccessibilityExtensionListSync(ani_env *env, ani_string abilityType, ani_string stateType);
    static void SubscribeState(ani_env *env, ani_string type, ani_object callback);
    static void UnsubscribeState(ani_env *env, ani_string type, ani_object callback);
    static void UnsubscribeStateAll(ani_env *env, ani_string type);
    static void SendAccessibilityEvent(ani_env *env, ani_object eventObject);
    static ani_object GetCaptionsManager(ani_env *env);
    static ani_boolean GetEnabled(ani_env *env, ani_object object);
    static void SetEnabled(ani_env *env, ani_object object, ani_boolean enabled);
    static ani_object GetStyle(ani_env *env, ani_object object);
    static void SetStyle(ani_env *env, ani_object object, ani_object style);
    static void SubscribeEnableChange(ani_env *env, ani_object object, ani_object callback);
    static void SubscribeStyleChange(ani_env *env, ani_object object, ani_object callback);
    static void UnsubscribeEnableChangeWithCallback(ani_env *env, ani_object object, ani_object callback);
    static void UnsubscribeEnableChangeAll(ani_env *env, ani_object object);
    static void UnsubscribeStyleChangeWithCallback(ani_env *env, ani_object object, ani_object callback);
    static void UnsubscribeStyleChangeAll(ani_env *env, ani_object object);
    static ani_object CreateAccessibilityCaptionProperty(ani_env *env,
        OHOS::AccessibilityConfig::CaptionProperty &captionProperty);
    static ani_object CreateCaptionPropertyInfoInner(ani_env *env, ani_class cls,
        ani_object &object, OHOS::AccessibilityConfig::CaptionProperty &captionProperty);

    static std::shared_ptr<StateListenerImpl> accessibilityStateListeners_;
    static std::shared_ptr<StateListenerImpl> touchGuideStateListeners_;
    static std::shared_ptr<StateListenerImpl> screenReaderStateListeners_;
    static std::shared_ptr<AccessibilityCaptionsObserverImpl> captionListeners_;

private:
    ANIAccessibilityClient() = default;
    ~ANIAccessibilityClient() = default;
    static ani_object CreateJsOtherInfoInner(ani_env *env, ani_class cls, ani_object &object,
        OHOS::Accessibility::AccessibilityAbilityInfo &accessibilityAbilityInfo);
    static ani_object CreateJsAccessibilityAbilityInfoInner(ani_env *env, ani_class cls, ani_object &object,
        OHOS::Accessibility::AccessibilityAbilityInfo& accessibleAbilityInfo);
    static ani_object CreateJsAccessibilityAbilityInfo(ani_env *env,
        OHOS::Accessibility::AccessibilityAbilityInfo& accessibleAbilityInfo);
    static ani_object ConvertAccessibleAbilityInfosToJs(ani_env *env,
        std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);
};

#endif // ANI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H