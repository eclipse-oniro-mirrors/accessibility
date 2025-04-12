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

class ANIAccessibilityClient {
public:
    static ani_boolean IsOpenTouchGuideSync([[maybe_unused]] ani_env *env);
    static ani_boolean IsOpenAccessibilitySync([[maybe_unused]] ani_env *env);
    static void SubscribeState(ani_env *env, ani_string type, ani_object callback);
    static void UnsubscribeState(ani_env *env, ani_string type, ani_object callback);
    static void UnsubscribeStateAll(ani_env *env, ani_string type);
    static ani_int SendAccessibilityEvent(ani_env *env, ani_object eventObject);

    static std::shared_ptr<StateListenerImpl> accessibilityStateListeners_;
    static std::shared_ptr<StateListenerImpl> touchGuideStateListeners_;

private:
    ANIAccessibilityClient() = default;
    ~ANIAccessibilityClient() = default;
};

#endif // ANI_ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H