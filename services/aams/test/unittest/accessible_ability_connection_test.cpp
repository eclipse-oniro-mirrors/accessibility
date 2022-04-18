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

#include <cstdio>
#include <gtest/gtest.h>
#include "accessibility_ability_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessibility_account_data.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
const static uint32_t SLEEP_TIME_2 = 2;
const static uint32_t SLEEP_TIME_3 = 3;
class AccessibleAbilityConnectionUnitTest : public ::testing::Test {
public:
    AccessibleAbilityConnectionUnitTest()
    {}
    ~AccessibleAbilityConnectionUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityConnection> connection_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityClientStub> obj_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

void AccessibleAbilityConnectionUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest Start ######################";
}

void AccessibleAbilityConnectionUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest End ######################";
}

void AccessibleAbilityConnectionUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Start AAMS
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();

    // new Interaction proxy
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(0, proxy, 0);
    // aams RegisterElementOperator
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->RegisterElementOperator(0, proxy, 0);
    // new AAconnection
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData_ = new AccessibilityAccountData(0);
    accountData_->AddAccessibilityWindowConnection(0, connection);
    connection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1", "2", "3");
    obj_ = new MockAccessibleAbilityClientStubImpl();
    connection_->OnAbilityConnectDone(*elementName_, obj_, 0);
}

void AccessibleAbilityConnectionUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    // Deregister ElementOperator
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->DeregisterElementOperator(0);
    if (connection_ != nullptr) {
        connection_->OnAbilityDisconnectDone(*elementName_, 0);
    } else {
        return;
    }
    connection_ = nullptr;
    elementName_ = nullptr;
    mock_ = nullptr;
    obj_ = nullptr;
    accountData_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetAbilityInfo_001
 * @tc.name: GetAbilityInfo
 * @tc.desc: Test function GetAbilityInfo
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetAbilityInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAbilityInfo_001 start";

    auto abilityInfo = connection_->GetAbilityInfo();
    auto abilities = abilityInfo.GetAccessibilityAbilityType();
    EXPECT_EQ(abilities, 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAbilityInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetElementName_001
 * @tc.name: GetElementName
 * @tc.desc: Test function GetElementName
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetElementName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetElementName_001 start";
    sleep(SLEEP_TIME_2);
    auto elementName = connection_->GetElementName();
    auto ret = elementName.GetDeviceID();
    sleep(SLEEP_TIME_2);
    EXPECT_STREQ(ret.c_str(), "1");

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetElementName_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetAccountData_001
 * @tc.name: GetAccountData
 * @tc.desc: Test function GetAccountData
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetAccountData_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAccountData_001 start";

    auto ret = connection_->GetAccountData();
    EXPECT_TRUE(ret.GetRefPtr());

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAccountData_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetAbilityClient_001
 * @tc.name: GetAbilityClient
 * @tc.desc: Test function GetAbilityClient
 */
HWTEST_F(
    AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetAbilityClient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 start";
    sleep(SLEEP_TIME_2);
    auto ret = connection_->GetAbilityClient();
    sleep(SLEEP_TIME_2);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(
    AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 start";
    AccessibilityEventInfo eventInfo;
    /* EventType is in the allowed list */
    eventInfo.SetEventType(EventType::TYPE_PAGE_STATE_UPDATE);
    sleep(SLEEP_TIME_2);
    connection_->OnAccessibilityEvent(eventInfo);
    sleep(SLEEP_TIME_2);
    EXPECT_EQ(int(EventType::TYPE_PAGE_STATE_UPDATE), AccessibilityAbilityHelper::GetInstance().GetTestEventType());

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(
    AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 start";
    AccessibilityEventInfo eventInfo;
    /* EventType is in the allowed list */
    eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    /* invalid window */
    eventInfo.SetWindowId(3);
    sleep(SLEEP_TIME_2);
    connection_->OnAccessibilityEvent(eventInfo);
    sleep(SLEEP_TIME_2);
    EXPECT_EQ(1, AccessibilityAbilityHelper::GetInstance().GetTestEventType());

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001 start";
    sleep(SLEEP_TIME_2);
    OHOS::Accessibility::Rect rect(0, 0, 0, 0);
    connection_->OnGestureInjectResult(1, false);
    sleep(SLEEP_TIME_2);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestGestureSimulateResult(), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_Connect_001
 * @tc.name: Connect
 * @tc.desc: Test function Connect
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_Connect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Connect_001 start";

    AppExecFwk::ElementName element;
    connection_->Connect(element);
    sleep(SLEEP_TIME_3);
    auto accountData = connection_->GetAccountData();
    EXPECT_EQ(int(accountData->GetConnectingA11yAbilities().size()), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Connect_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Disconnect_001 start";

    connection_->Disconnect();
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Reset_001 end";
}
} // namespace Accessibility
} // namespace OHOS