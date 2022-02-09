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

#include "amsstabilityb1.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_ability_info.h"

#include "hilog_wrapper.h"
#include "refbase.h"

using namespace OHOS::Accessibility;

namespace OHOS {
namespace AppExecFwk {

#define TESTWINDOWID 0
#define TESTUSERID 0
#define STATETYPE 1

void AccessibleAbilityListenerTest::OnAbilityConnected()
{
    HILOG_INFO("######################AamsStAbilityB1: Accessible application is connected.");
    AbilityContext context;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    int componentId = 1;
    AccessibilitySystemAbilityClient::GetInstance(context)->SendEvent(eventType, componentId);
}

void AccessibleAbilityListenerTest::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_INFO("######################AamsStAbilityB1: An Accessibility event is received and eventType is %{public}d", eventInfo.GetEventType());

    //Accessibility::AccessibleAbility::GetInstance().GetRootElementInfo();
    std::optional<AccessibilityElementInfo> elementInfo;
    Accessibility::AccessibleAbility::GetInstance().GetRootElementInfo(elementInfo);

    if (elementInfo.has_value()) {
        HILOG_INFO("elementInfo has value.");
    } else {
        HILOG_INFO("elementInfo has no value.");
    }
}

void AccessibleAbilityListenerTest::OnGesture(uint32_t gestureId)
{
    HILOG_INFO("######################AamsStAbilityB1: A gesture is received and gestureId is %{public}d.", gestureId);
}

void AccessibleAbilityListenerTest::OnInterrupt()
{
    HILOG_INFO("######################AamsStAbilityB1: Accessible application is on interrupt.");
}

bool AccessibleAbilityListenerTest::OnKeyPressEvent(const MMI::KeyEvent& keyEvent)
{
    HILOG_INFO("######################AamsStAbilityB1: A key press event is received");
    return true;
}

void AamsStAbilityB1::OnStart(const Want &want)
{
    GetWantInfo(want);

    HILOG_INFO("######################AamsStAbilityB1::onStart");
    RegisterInteractionOperation();
    AbilityContext context;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    int componentId = 1;
    AccessibilitySystemAbilityClient::GetInstance(context)->SendEvent(eventType, componentId);
    //RegisterListenerToAA();
    //ConnectToAAMS();
    //SendEventWithType();
}

void AamsStAbilityB1::OnForeground(const Want &want)
{
    GetWantInfo(want);

    HILOG_INFO("AamsStAbilityB1::OnForeground");
    Ability::OnForeground(want);
}

void AamsStAbilityB1::OnNewWant(const Want &want)
{
    GetWantInfo(want);

    HILOG_INFO("AamsStAbilityB1::OnNewWant");
    Ability::OnNewWant(want);
}

void AamsStAbilityB1::OnStop()
{
    HILOG_INFO("AamsStAbilityB1::onStop");
    Ability::OnStop();
}

void AamsStAbilityB1::OnActive()
{
    HILOG_INFO("AamsStAbilityB1::OnActive");
    Ability::OnActive();
    if (std::string::npos != shouldReturn.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
}

void AamsStAbilityB1::OnInactive()
{
    HILOG_INFO("AamsStAbilityB1::OnInactive");
    Ability::OnInactive();
}

void AamsStAbilityB1::OnBackground()
{
    HILOG_INFO("AamsStAbilityB1::OnBackground");
    Ability::OnBackground();
}

void AamsStAbilityB1::Clear()
{
    shouldReturn = "";
    targetBundle = "";
    targetAbility = "";
}

void AamsStAbilityB1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");
}

void AamsStAbilityB1::Interrupt()
{
    HILOG_INFO("AamsStAbilityB1::Interrupt");
    AbilityContext context;
    Accessibility::AccessibilitySystemAbilityClient::GetInstance(context)->Interrupt();
}

void AamsStAbilityB1::RegisterInteractionOperation()
{
    HILOG_INFO("AamsStAbilityB1::RegisterInteractionOperation");
    AbilityContext context;
    int windowId = 0;
    int userId = TESTUSERID;
    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    Accessibility::AccessibilitySystemAbilityClient::GetInstance(context)->RegisterInteractionOperation(
        windowId, interactionOperation, userId);
}

void AamsStAbilityB1::DeregisterInteractionOperation()
{
    HILOG_INFO("AamsStAbilityB1::DeregisterInteractionOperation");
    AbilityContext context;
    int windowId = TESTWINDOWID;
    Accessibility::AccessibilitySystemAbilityClient::GetInstance(context)->DeregisterInteractionOperation(windowId);
}

void AamsStAbilityB1::GetAbilityList()
{
    HILOG_INFO("AamsStAbilityB1::GetAbilityList");
    AbilityContext context;
    int accessibilityAbilityTypes = Accessibility::AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN;
    auto stateType = Accessibility::AbilityStateType::ABILITY_STATE_ENABLE;
    AccessibilitySystemAbilityClient::GetInstance(context)->GetAbilityList(accessibilityAbilityTypes, stateType);
}

void AamsStAbilityB1::GetSuggestedInterval()
{
    HILOG_INFO("AamsStAbilityB1::GetSuggestedInterval");
    AbilityContext context;
    int timeout = 1;
    int contentType = 1;
    AccessibilitySystemAbilityClient::GetInstance(context)->GetSuggestedInterval(timeout, contentType);
}

void AamsStAbilityB1::SendEventWithType()
{
    HILOG_INFO("###################AamsStAbilityB1::SendEventWithType");
    AbilityContext context;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    int componentId = 1;
    AccessibilitySystemAbilityClient::GetInstance(context)->SendEvent(eventType, componentId);
}

void AamsStAbilityB1::SendEvent()
{
    HILOG_INFO("AamsStAbilityB1::SendEvent");
    AbilityContext context;
    AccessibilityEventInfo eventInfo;
    AccessibilitySystemAbilityClient::GetInstance(context)->SendEvent(eventInfo);
}

void AamsStAbilityB1::SubscribeStateObserver()
{
    HILOG_INFO("AamsStAbilityB1::SubscribeStateObserver");
    AbilityContext context;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    std::shared_ptr<AccessibilityStateObserver> stateObserver(new AccessibilityStateObserverTest());

    AccessibilitySystemAbilityClient::GetInstance(context)->SubscribeStateObserver(stateObserver, eventType);
}

void AamsStAbilityB1::UnsubscribeStateObserver()
{
    HILOG_INFO("AamsStAbilityB1::UnsubscribeStateObserver");
    AbilityContext context;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    std::shared_ptr<AccessibilityStateObserver> stateObserver(new AccessibilityStateObserverTest());

    AccessibilitySystemAbilityClient::GetInstance(context)->UnsubscribeStateObserver(stateObserver, eventType);
}

void AamsStAbilityB1::UnsubscribeStateObserverEventType()
{
    HILOG_INFO("AamsStAbilityB1::UnsubscribeStateObserverEventType");
    AbilityContext context;
    std::shared_ptr<AccessibilityStateObserver> stateObserver(new AccessibilityStateObserverTest());
    AccessibilitySystemAbilityClient::GetInstance(context)->UnsubscribeStateObserver(stateObserver);
}

void AamsStAbilityB1::RegisterListenerToAA()
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    listener_ = std::make_shared<AccessibleAbilityListenerTest>();
    if (!listener_) {
        HILOG_ERROR("###############################listener_ is nullptr.");
        return;
    }
    Accessibility::AccessibleAbility::GetInstance().RegisterListener(listener_);
}

void AamsStAbilityB1::ConnectToAAMS()
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    Accessibility::AccessibleAbility::GetInstance().ConnectToAAMS();
}

void AamsStAbilityB1::GetRootElementInfo()
{
    HILOG_INFO("####################AamsStAbilityB1::%{public}s start", __func__);
    std::optional<AccessibilityElementInfo> elementInfo;
    Accessibility::AccessibleAbility::GetInstance().GetRootElementInfo(elementInfo);

    if (elementInfo.has_value()) {
        HILOG_INFO("elementInfo has value.");
    } else {
        HILOG_INFO("elementInfo has no value.");
    }
}

void AccessibilityInteractionOperationTest::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback, const int mode)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetBundleName("testapp_B1");
    info.SetWindowId(0);
    info.SetCheckable(true);
    info.SetHint("testapp_B1_findFocus_fang-ch");
    info.AddChild(2);
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    //Accessibility::AccessibilityInteractionOperationStub::CallbackImpl tempCallback = const_cast<Accessibility::AccessibilityInteractionOperationCallback&>(callback);
    callback.SetSearchElementInfoByAccessibilityIdResult(infos,requestId);
}

void AccessibilityInteractionOperationTest::SearchElementInfosByText(const long elementId, const std::string &text,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetBundleName("fang-ch");
    info.SetWindowId(0);
    info.SetComponentType("2021年");
    info.SetCheckable(true);
    info.SetHint("testapp_B1_findFocus_fang-ch");
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    callback.SetSearchElementInfoByTextResult(infos, requestId);
    HILOG_INFO("###############################AamsStAbilityB1::=================SearchElementInfosByText===========success=========");
}

void AccessibilityInteractionOperationTest::FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(0);
    info.SetBundleName("testapp_B1_findFocus");
    info.SetCheckable(true);
    info.SetHint("testapp_B1_findFocus_fang-ch");
    callback.SetFindFocusedElementInfoResult(info, requestId);
    HILOG_INFO("###############################AamsStAbilityB1::=================FindFocusedElementInfo===========success=========");
}

void AccessibilityInteractionOperationTest::FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(0);
    info.SetBundleName("testapp_B1_FocusMoveSearch");
    info.SetCheckable(true);
    info.SetHint("testapp_B1_FocusMoveSearch_fang-ch");
    info.SetParent(2);

    callback.SetFocusMoveSearchResult(info, requestId);
    HILOG_INFO("###############################AamsStAbilityB1::=================FocusMoveSearch===========success=========");
}

void AccessibilityInteractionOperationTest::PerformAction(const long elementId, const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(0);
    info.SetBundleName("testapp_B1_findFocus");
    info.SetCheckable(true);
    info.SetHint("testapp_B1_findFocus_fang-ch");
    callback.SetPerformActionResult(true, requestId);
    HILOG_INFO("###############################AamsStAbilityB1::=================PerformAction===========success=========");
}

void AccessibilityInteractionOperationTest::ClearFocus()
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
}

void AccessibilityInteractionOperationTest::OutsideTouch()
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
}

void AccessibilityStateObserverTest::OnStateChanged(const Accessibility::AccessibilityStateEvent& stateEvent)
{
    HILOG_INFO("###############################AamsStAbilityB1::%{public}s start", __func__);
}

REGISTER_AA(AamsStAbilityB1);
}  // namespace AppExecFwk
}  // namespace OHOS