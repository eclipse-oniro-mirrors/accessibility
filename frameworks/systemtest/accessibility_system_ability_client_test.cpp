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

#include <memory>
#include <gtest/gtest.h>
#include "accessibility_system_ability_client.h"
#include "ability_context.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_element_operator.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_element_info.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"

using namespace OHOS::AppExecFwk;
using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
const int windowID_100 = 100;
const int windowID_200 = 200;
const int windowID_300 = 300;
const int windowID_400 = 400;
const int windowID_500 = 500;
const int sleepTimeSec2 = 2;
class AccessibilityElementOperatorTest : public Accessibility::AccessibilityElementOperator {
public:
    AccessibilityElementOperatorTest()
    {}
    virtual ~AccessibilityElementOperatorTest() = default;
    void SearchElementInfoByAccessibilityId(const long elementId, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int mode) override;
    void SearchElementInfosByText(const long elementId, const std::string& text, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback) override;
    void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback) override;
    void FocusMoveSearch(const long elementId, const int direction, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback) override;
    void ExecuteAction(const long elementId, const int action, const std::map<std::string, std::string> actionArguments,
        const int requestId, Accessibility::AccessibilityElementOperatorCallback& callback) override;
    void ClearFocus() override;
    void OutsideTouch() override;
};

void AccessibilityElementOperatorTest::SearchElementInfoByAccessibilityId(const long elementId, const int requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback, const int mode)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(windowID_100);
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
}

void AccessibilityElementOperatorTest::SearchElementInfosByText(const long elementId, const std::string& text,
    const int requestId, Accessibility::AccessibilityElementOperatorCallback& callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(windowID_200);
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    callback.SetSearchElementInfoByTextResult(infos, requestId);
}

void AccessibilityElementOperatorTest::FindFocusedElementInfo(const long elementId, const int focusType,
    const int requestId, Accessibility::AccessibilityElementOperatorCallback& callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(windowID_300);
    callback.SetFindFocusedElementInfoResult(info, requestId);
}

void AccessibilityElementOperatorTest::FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(windowID_400);

    callback.SetFocusMoveSearchResult(info, requestId);
}

void AccessibilityElementOperatorTest::ExecuteAction(const long elementId, const int action,
    const std::map<std::string, std::string> actionArguments, int requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(windowID_500);
    callback.SetExecuteActionResult(true, requestId);
}

void AccessibilityElementOperatorTest::ClearFocus()
{
    HILOG_INFO("%{public}s start", __func__);
}

void AccessibilityElementOperatorTest::OutsideTouch()
{
    HILOG_INFO("%{public}s start", __func__);
}

class ASACClientSystemTest : public ::testing::Test {
public:
    ASACClientSystemTest()
    {}
    ~ASACClientSystemTest()
    {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "ASACClientSystemTest Start";
        Install("asacSystemTestA");
        sleep(sleepTimeSec2);
    }
    static void TearDownTestCase()
    {
        Uninstall("asacSystemTestA");
        GTEST_LOG_(INFO) << "ASACClientSystemTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "ASACClientSystemTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "ASACClientSystemTest TearDown()";
    }
    static void Install(std::string hapName);
    static void Uninstall(std::string bundleName);
};

void ASACClientSystemTest::Install(std::string hapName)
{
    system(("bm install -p /system/vendor/" + hapName + ".hap > /dev/null 2>&1").c_str());
}

void ASACClientSystemTest::Uninstall(std::string bundleName)
{
    system(("bm uninstall -n " + bundleName + " > /dev/null 2>&1").c_str());
}
/**
 * @tc.number: RegisterElementOperator_001
 * @tc.name: Function
 * @tc.desc: Test RegisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(100, elementOperator, 1);

    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(100);
    eventInfo.SetEventType(eventType);
    eventInfo.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    eventInfo.SetBundleName("test");
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterElementOperator end";
}

/**
 * @tc.number: RegisterElementOperator_002
 * @tc.name: Function
 * @tc.desc: Test RegisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, RegisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_002 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(200, elementOperator, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(200);
    eventInfo.SetEventType(eventType);
    eventInfo.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    eventInfo.SetBundleName("test");
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterElementOperator end";
}

/**
 * @tc.number: RegisterElementOperator_003
 * @tc.name: Function
 * @tc.desc: Test RegisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, RegisterElementOperator_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_003 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(300, elementOperator, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(300);
    eventInfo.SetEventType(eventType);
    eventInfo.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    eventInfo.SetBundleName("test");
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterElementOperator end";
}

/**
 * @tc.number: RegisterElementOperator_004
 * @tc.name: Function
 * @tc.desc: Test RegisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, RegisterElementOperator_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_004 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(400, elementOperator, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(400);
    eventInfo.SetEventType(eventType);
    eventInfo.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    eventInfo.SetBundleName("test");
    instance->SendEvent(eventInfo);

    GTEST_LOG_(INFO) << "RegisterElementOperator end";
}

/**
 * @tc.number: RegisterElementOperator_005
 * @tc.name: Function
 * @tc.desc: Test RegisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, RegisterElementOperator_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_005 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(500, elementOperator, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(500);
    eventInfo.SetEventType(eventType);
    eventInfo.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    eventInfo.SetBundleName("test");
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterElementOperator end";
}

/**
 * @tc.number: DeregisterElementOperator_001
 * @tc.name: Function
 * @tc.desc: Test DeregisterElementOperator
 */
HWTEST_F(ASACClientSystemTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityElementOperatorTest> elementOperator =
        make_shared<AccessibilityElementOperatorTest>();
    GTEST_LOG_(INFO) << "RegisterElementOperator start";
    instance->RegisterElementOperator(100, elementOperator, 1);
    GTEST_LOG_(INFO) << "RegisterElementOperator end";

    GTEST_LOG_(INFO) << "DeregisterElementOperator start";
    instance->DeregisterElementOperator(1);
    GTEST_LOG_(INFO) << "DeregisterElementOperator end";
}

HWTEST_F(ASACClientSystemTest, GetInstalledAbilities, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetInstalledAbilities start";
    std::vector<AccessibilityAbilityInfo> it{};
    it = instance->GetInstalledAbilities();

    std::string BundleName = "";
    for (auto AbilityInfo = it.begin(); AbilityInfo != it.end(); AbilityInfo++) {
        BundleName = AbilityInfo->GetPackageName();
        printf("test");
        printf("%s", BundleName.c_str());
    }
    // EXPECT_FALSE(it.empty());
    GTEST_LOG_(INFO) << "GetInstalledAbilities end";
}

HWTEST_F(ASACClientSystemTest, GetEnabledAbilities, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetEnabledAbilities start";
    std::map<std::string, AppExecFwk::ElementName> it{};
    it = instance->GetEnabledAbilities();
    std::string BundleName = "";
    for (auto& enableAbility : it) {
        BundleName = enableAbility.second.GetBundleName();
        printf("test");
        printf("%s", BundleName.c_str());
    }
    // EXPECT_FALSE(it.empty());
    GTEST_LOG_(INFO) << "GetEnabledAbilities end";
}

HWTEST_F(ASACClientSystemTest, SetInstalled, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetInstalled start";
    std::vector<AccessibilityAbilityInfo> it{};
    it = instance->GetInstalledAbilities();

    bool result = instance->SetInstalled(it);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "SetInstalled end";
}

HWTEST_F(ASACClientSystemTest, SetEnabledObj, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetEnabledObj start";
    std::map<std::string, AppExecFwk::ElementName> it{};
    it = instance->GetEnabledAbilities();
    bool result = instance->SetEnabledObj(it);
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "SetEnabledObj end";
}

HWTEST_F(ASACClientSystemTest, GetKeyEventObserverState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetKeyEventObserverState start";
    bool result = instance->GetKeyEventObserverState();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GetKeyEventObserverState end";
}

HWTEST_F(ASACClientSystemTest, GetGestureState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetGestureState start";
    bool result = instance->GetGestureState();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GetGestureState end";
}

HWTEST_F(ASACClientSystemTest, GetTouchGuideState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetTouchGuideState start";
    bool result = instance->GetTouchGuideState();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GetTouchGuideState end";
}

HWTEST_F(ASACClientSystemTest, GetCaptionState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetCaptionState start";
    bool result = instance->GetCaptionState();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GetCaptionState end";
}

HWTEST_F(ASACClientSystemTest, GetEnabledState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetEnabledState start";
    bool result = instance->GetEnabledState();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "GetEnabledState end";
}


HWTEST_F(ASACClientSystemTest, SetCaptionProperty, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetCaptionProperty start";

    CaptionProperty caption;
    bool result = instance->SetCaptionProperty(caption);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetCaptionProperty end";
}

HWTEST_F(ASACClientSystemTest, SetKeyEventObserverState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetKeyEventObserverState start";

    bool state = false;
    bool result = instance->SetKeyEventObserverState(state);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetKeyEventObserverState end";
}

HWTEST_F(ASACClientSystemTest, SetGestureState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetGestureState start";

    bool state = false;
    bool result = instance->SetGestureState(state);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetGestureState end";
}

HWTEST_F(ASACClientSystemTest, SetTouchGuideState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetTouchGuideState start";

    bool state = false;
    bool result = instance->SetTouchGuideState(state);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetTouchGuideState end";
}

HWTEST_F(ASACClientSystemTest, SetEnabled, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetEnabled start";

    bool state = false;
    bool result = instance->SetEnabled(state);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetEnabled end";
}

HWTEST_F(ASACClientSystemTest, SetCaptionState, TestSize.Level1)
{
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "SetCaptionState start";

    bool state = false;
    bool result = instance->SetCaptionState(state);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SetCaptionState end";
}

}  // namespace Accessibility
}  // namespace OHOS