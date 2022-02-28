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

#ifndef AAMS_ST_ABILITY_A1
#define AAMS_ST_ABILITY_A1

#include <memory>
#include <string>
#include "ability_loader.h"
#include "accessible_ability.h"
#include "accessibility_event_info.h"
#include "hilog_wrapper.h"
#include "key_event.h"

namespace OHOS {
namespace AppExecFwk {
class AccessibleAbilityListenerTest;

class AamsStAbilityA1 : public Ability {
public:
    AamsStAbilityA1() {}
    ~AamsStAbilityA1() {}
    void DisableAbility();
    void GetFocusElementInfo();
    void GestureSimulate();
    void GetDisplayResizeController();
    void GetRootElementInfo();
    void GetWindows();
    void PerformCommonAction();
    void GetFingerprintController();

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

    void RegisterListenerToAA();
    void ConnectToAAMS();

    std::string shouldReturn;
    std::string targetBundle;
    std::string targetAbility;

    std::shared_ptr<AccessibleAbilityListenerTest> listener_ = nullptr;
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
    std::shared_ptr<AamsStAbilityA1> aamsStAbilityA1_ = std::make_shared<AamsStAbilityA1>();
};
} // namespace AppExecFwk
} // namespace OHOS
#endif  // AAMS_ST_ABILITY_A1