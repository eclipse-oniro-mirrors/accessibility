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

#include "amsstabilitya1.h"
#include <unistd.h>
#include "accessibility_element_info.h"

using namespace OHOS::Accessibility;

namespace OHOS {
namespace AppExecFwk {
void AccessibleAbilityListenerTest::OnAbilityConnected()
{
    HILOG_INFO("AamsStAbilityA1: Accessible application is connected.");
    aamsStAbilityA1_->GetWindows();
}

void AccessibleAbilityListenerTest::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_INFO("AamsStAbilityA1: An Accessibility event is received and eventType is %{public}d",
        eventInfo.GetEventType());
}

void AccessibleAbilityListenerTest::OnGesture(uint32_t gestureId)
{
    HILOG_INFO("AamsStAbilityA1: A gesture is received and gestureId is %{public}d.", gestureId);
}

void AccessibleAbilityListenerTest::OnInterrupt()
{
    HILOG_INFO("AamsStAbilityA1: Accessible application is on interrupt.");
}

bool AccessibleAbilityListenerTest::OnKeyPressEvent(const MMI::KeyEvent& keyEvent)
{
    HILOG_INFO("AamsStAbilityA1: A key press event is received");
    return true;
}

void AamsStAbilityA1::OnStart(const Want &want)
{
    GetWantInfo(want);
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
    Ability::OnStart(want);

    RegisterListenerToAA();
    ConnectToAAMS();
}

void AamsStAbilityA1::OnNewWant(const Want &want)
{
    GetWantInfo(want);

    HILOG_INFO("AamsStAbilityA1::OnNewWant");
    Ability::OnNewWant(want);
}

void AamsStAbilityA1::OnForeground(const Want &want)
{
    GetWantInfo(want);

    HILOG_INFO("AamsStAbilityA1::OnForeground");
    Ability::OnForeground(want);
}

void AamsStAbilityA1::OnStop()
{
    HILOG_INFO("AamsStAbilityA1::onStop");
    Ability::OnStop();
}

void AamsStAbilityA1::OnActive()
{
    HILOG_INFO("AamsStAbilityA1::OnActive");
    Ability::OnActive();
    if (std::string::npos != shouldReturn.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
}

void AamsStAbilityA1::OnInactive()
{
    HILOG_INFO("AamsStAbilityA1::OnInactive");
    Ability::OnInactive();
}

void AamsStAbilityA1::OnBackground()
{
    HILOG_INFO("AamsStAbilityA1::OnBackground");
    Ability::OnBackground();
}

void AamsStAbilityA1::Clear()
{
    shouldReturn = "";
    targetBundle = "";
    targetAbility = "";
}

void AamsStAbilityA1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");
}

void AamsStAbilityA1::RegisterListenerToAA()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
    listener_ = std::make_shared<AccessibleAbilityListenerTest>();
    if (!listener_) {
        HILOG_ERROR("listener_ is nullptr.");
        return;
    }
    Accessibility::AccessibleAbility::GetInstance().RegisterListener(listener_);
}

void AamsStAbilityA1::ConnectToAAMS()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
    Accessibility::AccessibleAbility::GetInstance().ConnectToAAMS();
}

void AamsStAbilityA1::DisableAbility()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

void AamsStAbilityA1::GetFocusElementInfo()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

void AamsStAbilityA1::GestureSimulate()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

void AamsStAbilityA1::GetDisplayResizeController()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

void AamsStAbilityA1::GetRootElementInfo()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
    std::optional<AccessibilityElementInfo> elementInfo;
    Accessibility::AccessibleAbility::GetInstance().GetRootElementInfo(elementInfo);

    if (elementInfo.has_value()) {
        HILOG_INFO("elementInfo has value.");
    } else {
        HILOG_INFO("elementInfo has no value.");
    }
}

void AamsStAbilityA1::GetWindows()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);

    std::vector<AccessibilityWindowInfo> accessibilityWindow =
        Accessibility::AccessibleAbility::GetInstance().GetWindows();

    HILOG_INFO("accessibilityWindow.size() is %{public}d.", accessibilityWindow.size());
}

void AamsStAbilityA1::PerformCommonAction()
{
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

void AamsStAbilityA1::GetFingerprintController() {
    HILOG_INFO("AamsStAbilityA1::%{public}s start", __func__);
}

REGISTER_AA(AamsStAbilityA1)
}  // namespace AppExecFwk
}  // namespace OHOS