/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef AAMS_ST_ABILITY_B1
#define AAMS_ST_ABILITY_B1
#include <string>
#include "ability_loader.h"
#include "accessibility_element_operator.h"
#include "accessibility_state_event.h"
#include "accessible_ability.h"
#include "key_event.h"

namespace OHOS {
namespace AppExecFwk {
class AccessibleAbilityListenerTest;

class AamsStAbilityB1 : public Ability {
public:
    // aams
    void Interrupt();
    void RegisterInteractionOperation();
    void DeregisterInteractionOperation();
    void GetAbilityList();
    void GetSuggestedInterval();
    void GetAccessibilityCaptionProperties();
    void SendEventWithType();
    void SendEvent();
    // state callback
    void SubscribeStateObserver();
    void UnsubscribeStateObserver();
    void UnsubscribeStateObserverEventType();

    // AA->AAMS
    void RegisterListenerToAA();
    void ConnectToAAMS();
    void GetRootElementInfo();
    std::string shouldReturn;
protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;
private:
    void Clear();
    void GetWantInfo(const Want &want);

    std::string targetBundle;
    std::string targetAbility;

    std::shared_ptr<AccessibleAbilityListenerTest> listener_ = nullptr;
};

class AccessibilityInteractionOperationTest : public Accessibility::AccessibilityInteractionOperation {
public:
    AccessibilityInteractionOperationTest() {}
    virtual ~AccessibilityInteractionOperationTest() = default;
    void SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback, const int mode)override;
    void SearchElementInfosByText(const long elementId, const std::string &text,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void PerformAction(const long elementId, const int action, const std::map<std::string, std::string> actionArguments,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void ClearFocus()override;
    void OutsideTouch()override;
};

class AccessibilityStateObserverTest : public Accessibility::AccessibilityStateObserver {
public:
    AccessibilityStateObserverTest() {}
    virtual ~AccessibilityStateObserverTest() = default;
    void OnStateChanged(const Accessibility::AccessibilityStateEvent& stateEvent) override;
};

class AccessibleAbilityListenerTest : public Accessibility::AccessibleAbilityListener {
public:
    AccessibleAbilityListenerTest() {}
    ~AccessibleAbilityListenerTest() {}

    void OnAbilityConnected() override;
    void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& eventInfo) override;
    void OnGesture(uint32_t gestureId) override;
    void OnInterrupt() override;
    bool OnKeyPressEvent(const MMI::KeyEvent& keyEvent) override;

private:
    std::shared_ptr<AamsStAbilityB1> aamsStAbilityB1_ = std::make_shared<AamsStAbilityB1>();
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // AAMS_ST_ABILITY_B1