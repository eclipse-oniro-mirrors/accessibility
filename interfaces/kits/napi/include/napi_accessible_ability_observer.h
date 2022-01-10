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

#ifndef NAPI_ACCESSIBLE_ABILITY_OBSERVER_H
#define NAPI_ACCESSIBLE_ABILITY_OBSERVER_H

#include "accessible_ability_listener.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

class AccessibleAbilityObserver: public OHOS::Accessibility::AccessibleAbilityListener {
public:
    AccessibleAbilityObserver() = default;
    ~AccessibleAbilityObserver() = default;

    void OnAccessibilityEvent(const OHOS::Accessibility::AccessibilityEventInfo& eventInfo) override;
    void OnInterrupt() override;
    void OnAbilityConnected() override;
    void OnGesture(uint32_t gestureId) override;
    bool OnKeyPressEvent(const OHOS::MMI::KeyEvent& keyEvent) override;
};
#endif // NAPI_ACCESSIBLE_ABILITY_OBSERVER_H