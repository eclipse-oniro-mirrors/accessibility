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
    constexpr int32_t CHANNEL_ID = 2;
    constexpr int32_t INVALID_ACCOUNT_ID = -1;
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
    sptr<IRemoteObject> obj_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
};

void AccessibleAbilityConnectionUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
}

void AccessibleAbilityConnectionUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibleAbilityConnectionUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // new Interaction proxy
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(0, proxy, 0);

    // new AAconnection
    AccessibilityAbilityInitParams initParams;
    initParams.abilityTypes = ACCESSIBILITY_ABILITY_TYPE_ALL;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    accountData_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData_ != nullptr) {
        accountData_->AddAccessibilityWindowConnection(0, connection);
    }
    connection_ = new AccessibleAbilityConnection(AccessibilityAbilityHelper::accountId_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1", "2", "3");
    obj_ = new IPCObjectProxy(0);
    if (obj_ != nullptr && connection_ != nullptr) {
        connection_->OnAbilityConnectDoneSync(*elementName_, obj_);
    }
}

void AccessibleAbilityConnectionUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(INVALID_CHANNEL_ID);
    if (accountData_) {
        accountData_->RemoveAccessibilityWindowConnection(0);
    }

    if (connection_) {
        connection_->OnAbilityDisconnectDoneSync(*elementName_);
    }
    connection_ = nullptr;
    elementName_ = nullptr;
    obj_ = nullptr;
    accountData_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnRemoteDied_001
 * @tc.name: OnRemoteDied
 * @tc.desc: Test function OnRemoteDied
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnRemoteDied_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnRemoteDied_001 start";
    if (connection_ && obj_) {
        std::string uri = Utils::GetUri(*elementName_);
        accountData_->AddEnabledAbility(uri);
        EXPECT_EQ(1, accountData_->GetEnabledAbilities().size());
        IPCObjectProxy* proxy = static_cast<IPCObjectProxy*>(obj_.GetRefPtr());
        proxy->SendObituary();
        bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
            size_t count = accountData_->GetEnabledAbilities().size();
            auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
            if (count == 0 && aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
                return true;
            } else {
                return false;
            }
            }), 1);
        EXPECT_TRUE(ret);
    }
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnRemoteDied_001 end";
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
        auto elementName = connection_->GetElementName();
        auto ret = elementName.GetDeviceID();
        EXPECT_STREQ(ret.c_str(), "1");
    }
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetElementName_001 end";
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
        auto ret = connection_->GetAbilityClient();
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
        EXPECT_TRUE(accountData_->GetAccessibleAbilityConnection(Utils::GetUri(
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

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: Test function OnAbilityConnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    sptr<AppExecFwk::ElementName> elementName = new AppExecFwk::ElementName("device", "bundle", "ability");
    accountData->AddEnabledAbility(Utils::GetUri(*elementName));
    EXPECT_EQ(static_cast<int>(accountData->GetEnabledAbilities().size()), 1);

    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId(), 0, *abilityInfo);
    accountData->AddConnectingA11yAbility(Utils::GetUri(*elementName), connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(Utils::GetUri(*elementName)));

    ASSERT_TRUE(connection);
    sptr<AccessibleAbilityClientStub> client = new MockAccessibleAbilityClientStubImpl();
    connection->OnAbilityConnectDone(*elementName, client, 1);

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (static_cast<int>(accountData->GetEnabledAbilities().size()) == 0 &&
            !(accountData->GetConnectingA11yAbility(Utils::GetUri(*elementName))) &&
            aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_002
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: Test function OnAbilityConnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_002 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    // add aa client
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId(), CHANNEL_ID, *abilityInfo);
    sptr<AppExecFwk::ElementName> name = new AppExecFwk::ElementName("device", "bundle", "ability");
    accountData->AddConnectingA11yAbility(Utils::GetUri(*name), connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(Utils::GetUri(*name)));

    ASSERT_TRUE(connection);
    sptr<AccessibleAbilityClientStub> client = new MockAccessibleAbilityClientStubImpl();
    connection->OnAbilityConnectDone(*name, client, NO_ERROR);

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTestChannelId() == CHANNEL_ID) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_002 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_003
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: Test function OnAbilityConnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_003 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        accountData->GetAccountId(), CHANNEL_ID, *abilityInfo);
    sptr<AppExecFwk::ElementName> elementName = new AppExecFwk::ElementName("device", "bundle", "ability");
    accountData->AddConnectingA11yAbility(Utils::GetUri(*elementName), connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(Utils::GetUri(*elementName)));
    connection->OnAbilityConnectDone(*elementName, nullptr, NO_ERROR);
    sleep(SLEEP_TIME_2);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (accountData->GetConnectingA11yAbility(Utils::GetUri(*elementName)) &&
            aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_003 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_004
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: Test function OnAbilityConnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_004 start";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        INVALID_ACCOUNT_ID, CHANNEL_ID, *abilityInfo);
    sptr<AppExecFwk::ElementName> elementName = new AppExecFwk::ElementName("device", "bundle", "ability");

    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(INVALID_ACCOUNT_ID));
    connection->OnAbilityConnectDone(*elementName, nullptr, NO_ERROR);
    sleep(SLEEP_TIME_2);

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_004 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_005
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: Test function OnAbilityConnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_005 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    // add aa client
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId(), CHANNEL_ID, *abilityInfo);
    sptr<AppExecFwk::ElementName> name =
        new AppExecFwk::ElementName("clientDevice", "clientBundleName", "clientAbilityName");
    sptr<AccessibleAbilityClientStub> client = new MockAccessibleAbilityClientStubImpl();

    connection->OnAbilityConnectDone(*name, client, NO_ERROR);
    sleep(SLEEP_TIME_2);

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_005 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityDisconnectDone_001
 * @tc.name: OnAbilityDisconnectDone
 * @tc.desc: Test function OnAbilityDisconnectDone
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnAbilityDisconnectDone_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityDisconnectDone_001 start";
    ASSERT_TRUE(connection_);
    AppExecFwk::ElementName element;
    connection_->OnAbilityDisconnectDone(element, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityDisconnectDone_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    // add aa client
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    abilityInfo->SetCapabilityValues(0);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(
        accountData->GetAccountId(), CHANNEL_ID, *abilityInfo);
    ASSERT_TRUE(connection);
    AppExecFwk::ElementName element("deviceId", "bundleName", "abilityName");
    sptr<AccessibleAbilityClientStub> obj = new MockAccessibleAbilityClientStubImpl();
    connection->OnAbilityConnectDoneSync(element, obj);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), CHANNEL_ID);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_FALSE(connection_->OnKeyPressEvent(*keyEvent, 0));
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_SetAbilityInfoTargetBundleName_001
 * @tc.name: SetAbilityInfoTargetBundleName
 * @tc.desc: Test function SetAbilityInfoTargetBundleName.
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_SetAbilityInfoTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SetAbilityInfoTargetBundleName_001 start";
    ASSERT_TRUE(connection_);
    std::vector<std::string> targetBundleNames;
    targetBundleNames.push_back("target");
    connection_->SetAbilityInfoTargetBundleName(targetBundleNames);
    ASSERT_EQ(static_cast<int>(connection_->GetAbilityInfo().GetFilterBundleNames().size()), 1);
    EXPECT_STREQ(connection_->GetAbilityInfo().GetFilterBundleNames().front().c_str(), "target");
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SetAbilityInfoTargetBundleName_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetChannelId_001
 * @tc.name: GetChannelId
 * @tc.desc: Test function GetChannelId.
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest,
    AccessibleAbilityConnection_Unittest_GetChannelId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetChannelId_001 start";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(0, CHANNEL_ID, *abilityInfo);
    EXPECT_EQ(connection->GetChannelId(), CHANNEL_ID);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetChannelId_001 end";
}
} // namespace Accessibility
} // namespace OHOS