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
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessibility_account_data.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service.h"
#include "mock_bundle_manager.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_2 = 2;
} // namespace

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
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);

    // new Interaction proxy
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(0, proxy, 0);
    // aams RegisterElementOperator
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, proxy);
    // new AAconnection
    AccessibilityAbilityInitParams initParams;
    initParams.abilityTypes = ACCESSIBILITY_ABILITY_TYPE_ALL;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    accountData_ = new AccessibilityAccountData(0);
    if (accountData_ != nullptr) {
        accountData_->AddAccessibilityWindowConnection(0, connection);
    }
    connection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1", "2", "3");
    obj_ = new MockAccessibleAbilityClientStubImpl();
    if (obj_ != nullptr && connection_ != nullptr) {
        connection_->OnAbilityConnectDoneSync(*elementName_, obj_);
    }
}

void AccessibleAbilityConnectionUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    // Deregister ElementOperator
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    if (connection_ != nullptr) {
        connection_->OnAbilityDisconnectDoneSync(*elementName_);
    } else {
        return;
    }
    connection_ = nullptr;
    elementName_ = nullptr;
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
    if (connection_ != nullptr) {
        auto abilityInfo = connection_->GetAbilityInfo();
        auto abilities = abilityInfo.GetAccessibilityAbilityType();
        EXPECT_EQ(abilities, ACCESSIBILITY_ABILITY_TYPE_ALL);
    }
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
    if (connection_ != nullptr) {
        sleep(SLEEP_TIME_2);
        auto elementName = connection_->GetElementName();
        auto ret = elementName.GetDeviceID();
        sleep(SLEEP_TIME_2);
        EXPECT_STREQ(ret.c_str(), "1");
    }
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
    if (connection_ != nullptr) {
        auto ret = connection_->GetAccountData();
        EXPECT_TRUE(ret.GetRefPtr());
    }
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
    if (connection_ != nullptr) {
        sleep(SLEEP_TIME_2);
        auto ret = connection_->GetAbilityClient();
        sleep(SLEEP_TIME_2);
        EXPECT_TRUE(ret);
    }
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
    if (connection_ != nullptr) {
        AccessibilityEventInfo eventInfo;
        /* EventType is in the allowed list */
        eventInfo.SetEventType(EventType::TYPE_PAGE_STATE_UPDATE);
        sleep(SLEEP_TIME_2);
        connection_->OnAccessibilityEvent(eventInfo);
        sleep(SLEEP_TIME_2);
        EXPECT_EQ(int(EventType::TYPE_PAGE_STATE_UPDATE), AccessibilityAbilityHelper::GetInstance().GetTestEventType());
    }
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
    if (connection_ != nullptr) {
        AccessibilityEventInfo eventInfo;
        /* EventType is in the allowed list */
        eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
        /* invalid window */
        eventInfo.SetWindowId(3);
        sleep(SLEEP_TIME_2);
        connection_->OnAccessibilityEvent(eventInfo);
        sleep(SLEEP_TIME_2);
        EXPECT_EQ(1, AccessibilityAbilityHelper::GetInstance().GetTestEventType());
    }
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_Connect_001
 * @tc.name: Connect
 * @tc.desc: Test function Connect
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_Connect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Connect_001 start";
    if (connection_ != nullptr) {
        AppExecFwk::ElementName element("deviceId", "bundleName", "abilityName");
        connection_->Connect(element);
        sptr<AccessibleAbilityClientStub> obj = new MockAccessibleAbilityClientStubImpl();
        connection_->OnAbilityConnectDoneSync(element, obj);
        auto accountData = connection_->GetAccountData();
        EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(Utils::GetUri(
            element.GetBundleName(), element.GetAbilityName())));
    }
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
    if (connection_ != nullptr) {
        connection_->Disconnect();
        EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), 0);
    }
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Reset_001 end";
}
} // namespace Accessibility
} // namespace OHOS