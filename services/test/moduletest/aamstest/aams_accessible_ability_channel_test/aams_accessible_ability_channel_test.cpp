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

#include <gtest/gtest.h>
#include "accessibility_common_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_manager_service.h"
#include "display_manager.h"
#include "iservice_registry.h"
#include "mock_accessibility_element_operator_callback.h"
#include "mock_accessibility_element_operator_impl.h"
#include "mock_accessibility_element_operator_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AamsAccessibleAbilityChannelTest : public testing::Test {
public:
    AamsAccessibleAbilityChannelTest()
    {}
    ~AamsAccessibleAbilityChannelTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void WritefileAll(const char* fname, const char* data);
    void AddAccessibleAbilityConnection(bool isNoCapability = false);
    void AddAccessibilityWindowConnection();
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
    sptr<MockAccessibilityElementOperatorProxy> proxy_ = nullptr;
};

void AamsAccessibleAbilityChannelTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest SetUpTestCase";
    // Start AAMS
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AamsAccessibleAbilityChannelTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDownTestCase";
    // Stop AAMS
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AamsAccessibleAbilityChannelTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest SetUp";
}

void AamsAccessibleAbilityChannelTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDown";

    Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData()->OnAccountSwitched();
    // Deregister ElementOperator
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    if (!ret) {
        GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDown EventQueue is not empty";
    }
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
    proxy_ = nullptr;
}

void AamsAccessibleAbilityChannelTest::WritefileAll(const char* fname, const char* data)
{
    FILE* fp;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsAccessibleAbilityChannelTest::AddAccessibleAbilityConnection(bool isNoCapability)
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest AddAccessibleAbilityConnection";
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    std::string deviceId;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);

    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    uint32_t capabilities = 0;
    if (!isNoCapability) {
        capabilities = Capability::CAPABILITY_RETRIEVE | Capability::CAPABILITY_TOUCH_GUIDE |
            Capability::CAPABILITY_GESTURE | Capability::CAPABILITY_KEY_EVENT_OBSERVER;
    }
    abilityInfo->SetCapabilityValues(capabilities);
    accountData_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData_->Init();
    AAConnection_ = new AccessibleAbilityConnection(accountData_->GetAccountId(), 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName(deviceId, initParams.bundleName, initParams.name);
    aastub_ = new AccessibleAbilityChannel(accountData_->GetAccountId(), abilityInfo->GetId());
    AAConnection_->OnAbilityConnectDoneSync(*elementName_, aastub_);
    accountData_->AddInstalledAbility(*abilityInfo);
}

void AamsAccessibleAbilityChannelTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest AddAccessibilityWindowConnection";
    // accessibility interaction connection
    int32_t windowId = 0;
    std::shared_ptr<MockAccessibilityElementOperatorCallback> mockCallback =
        std::make_shared<MockAccessibilityElementOperatorCallback>();
    sptr<AccessibilityElementOperatorStub> stub =
        new MockAccessibilityElementOperatorImpl(windowId, nullptr, *mockCallback);
    sptr<MockAccessibilityElementOperatorProxy> proxy = new MockAccessibilityElementOperatorProxy(stub);
    proxy_ = proxy;
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(windowId, proxy);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    if (!ret) {
        GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest EventQueue is not empty";
    }
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId,
 *           check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, proxy_->testChannelElementId_);
    EXPECT_EQ(0, proxy_->testChannelRequestId_);
    EXPECT_EQ(0, proxy_->testChannelMode_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId,
 *           check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfoByAccessibilityId(
        ACTIVE_WINDOW_ID, 0, 0, nullptr, 0);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_EQ(-1, proxy_->testChannelMode_);
    EXPECT_EQ(result, RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    string text = "text";
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfosByText(0, 0, text, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, proxy_->testChannelElementId_);
    EXPECT_EQ(0, proxy_->testChannelRequestId_);
    EXPECT_EQ(text, proxy_->testText_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    string text = "text";
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfosByText(
        ACTIVE_WINDOW_ID, 0, text, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(text, proxy_->testText_);
    EXPECT_EQ(result, RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, proxy_->testChannelElementId_);
    EXPECT_EQ(0, proxy_->testChannelRequestId_);
    EXPECT_EQ(focusType, proxy_->testFocusType_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->FindFocusedElementInfo(
        ACTIVE_WINDOW_ID, 0, focusType, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(focusType, proxy_->testFocusType_);
    EXPECT_EQ(result, RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: FocusType is FOCUS_TYPE_ACCESSIBILITY, check FindFocusedElementInfo.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t focusType = OHOS::Accessibility::FOCUS_TYPE_ACCESSIBILITY;
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->FindFocusedElementInfo(0, 0, focusType, 1, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, proxy_->testChannelElementId_);
    EXPECT_EQ(1, proxy_->testChannelRequestId_);
    EXPECT_EQ(focusType, proxy_->testFocusType_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t direction = FocusMoveDirection::UP;
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->FocusMoveSearch(0, 0, direction, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, proxy_->testChannelElementId_);
    EXPECT_EQ(0, proxy_->testChannelRequestId_);
    EXPECT_EQ(direction, proxy_->testDirection_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t direction = FocusMoveDirection::UP;
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->FocusMoveSearch(ACTIVE_WINDOW_ID, 0, direction, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(direction, proxy_->testDirection_);
    EXPECT_EQ(result, RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(std::make_pair("invalid", "invalid"));

    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->ExecuteAction(0, 4, 3, actionArguments, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(4, proxy_->testChannelElementId_);
    EXPECT_EQ(0, proxy_->testChannelRequestId_);
    EXPECT_EQ(3, proxy_->testAction_);
    EXPECT_EQ(actionArguments, proxy_->testActionArguments_);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_002 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(std::make_pair("invalid", "invalid"));

    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->ExecuteAction(
        ACTIVE_WINDOW_ID, 4, 3, actionArguments, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_EQ(0, proxy_->testAction_);
    EXPECT_EQ(result, RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Not add interaction connection check get windows.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 start";

    // Not add interaction connection,add accessibleAbility connection
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    uint64_t displayId = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    std::vector<AccessibilityWindowInfo> windows;
    AccessibilityHelper::GetInstance().GetTestStub()->GetWindowsByDisplayId(displayId, windows);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, windows.size());
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Add connection check send simulate gesture.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);

    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->SendSimulateGesture(gesturePath);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 end";
}

/***********************************************************************************************************/

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: No retrieve capability, check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_EQ(-1, proxy_->testChannelMode_);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: No retrieve capability, check SearchElementInfosByText.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    string text = "text";
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->SearchElementInfosByText(0, 0, text, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(text, proxy_->testText_);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: No retrieve capability, check FindFocusedElementInfo.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(focusType, proxy_->testFocusType_);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: No retrieve capability, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    int32_t direction = FocusMoveDirection::UP;
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->FocusMoveSearch(0, 0, direction, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_NE(direction, proxy_->testDirection_);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001
 * @tc.name: ExecuteAction
 * @tc.desc: No retrieve capability, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);
    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(std::make_pair("invalid", "invalid"));

    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result =
        AccessibilityHelper::GetInstance().GetTestStub()->ExecuteAction(0, 4, 3, actionArguments, 0, nullptr);
    sleep(2);
    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, proxy_->testChannelElementId_);
    EXPECT_EQ(-1, proxy_->testChannelRequestId_);
    EXPECT_EQ(0, proxy_->testAction_);
    EXPECT_NE(actionArguments, proxy_->testActionArguments_);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001
 * @tc.name: GetWindows
 * @tc.desc: No retrieve capability, check get windows.
 */
HWTEST_F(
    AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    uint64_t displayId = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    std::vector<AccessibilityWindowInfo> windows;
    AccessibilityHelper::GetInstance().GetTestStub()->GetWindowsByDisplayId(displayId, windows);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, windows.size());
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: No gesture capability, check send simulate gesture.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 start";

    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection(true);

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);

    ASSERT_TRUE(AccessibilityHelper::GetInstance().GetTestStub());
    RetError result = AccessibilityHelper::GetInstance().GetTestStub()->SendSimulateGesture(gesturePath);
    EXPECT_EQ(result, RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 end";
}
} // namespace Accessibility
} // namespace OHOS