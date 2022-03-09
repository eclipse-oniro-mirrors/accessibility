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

#include <cstdio>
#include <gtest/gtest.h>
#include "accessible_ability_connection.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_element_operator_proxy.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

extern int g_testChannalId;
extern int g_testEventType;
extern int g_testGestureSimulateResult;
extern int g_testKeyPressEvent;

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityConnectionUnitTest : public ::testing::Test {
public:
    AccessibleAbilityConnectionUnitTest() {}
    ~AccessibleAbilityConnectionUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityConnection> connection_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> obj_ = nullptr;
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
    sptr<AccessibilityElementOperatorStub> stub = new AccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy =
        new AccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(0, proxy, 0);
    // aams RegisterElementOperator
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->
        RegisterElementOperator(0, proxy, 0);
    // new AAconnection
    AppExecFwk::ExtensionAbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    accountData_ = new AccessibilityAccountData(0);
    accountData_->AddAccessibilityWindowConnection(0, connection);
    connection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1", "2", "3");
    obj_ = new AccessibleAbilityClientStubImpl();
    connection_->OnAbilityConnectDone(*elementName_, obj_, 0);
}

void AccessibleAbilityConnectionUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    // Deregister ElementOperator
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->DeregisterElementOperator(0);
    connection_ ->OnAbilityDisconnectDone(*elementName_, 0);
    connection_ = nullptr;
    elementName_ = nullptr;
    mock_ = nullptr;
    g_testEventType = -1;
    obj_= nullptr;
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

    auto elementName = connection_->GetElementName();
    auto ret = elementName.GetDeviceID();
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
 * @tc.number: AccessibleAbilityConnection_Unittest_GetProxy_001
 * @tc.name: GetProxy
 * @tc.desc: Test function GetProxy
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetProxy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 start";

    auto ret = connection_->GetProxy();
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 start";
    AccessibilityEventInfo eventInfo;
    /* EventType is in the allowed list */
    eventInfo.SetEventType(EventType::TYPE_PAGE_STATE_UPDATE);
    connection_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(int(EventType::TYPE_PAGE_STATE_UPDATE), g_testEventType);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 start";
    AccessibilityEventInfo eventInfo;
    /* EventType is in the allowed list */
    eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    /* invalid window */
    eventInfo.SetWindowId(3);
    connection_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(-1, g_testEventType);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001 start";

    OHOS::Accessibility::Rect rect(0, 0, 0, 0);
    connection_->OnGestureSimulateResult(1, false);
    EXPECT_EQ(g_testGestureSimulateResult, 1);

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
    EXPECT_EQ(g_testChannalId, 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Reset_001 end";
}
} // namespace Accessibility
} // namespace OHOS
