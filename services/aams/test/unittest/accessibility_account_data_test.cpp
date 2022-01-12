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

#include <gtest/gtest.h>
#include "accessibility_account_data.h"
#include "accessible_ability_manager_service_state_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_ability_info.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "accessible_ability_client_stub_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

namespace OHOS {
namespace Accessibility {

class AccessibilityAccountDataTest : public testing::Test {
public:

    AccessibilityAccountDataTest() {}
    ~AccessibilityAccountDataTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilityAccountDataTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest SetUpTestCase";
}

void AccessibilityAccountDataTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest TearDownTestCase";
}

void AccessibilityAccountDataTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest SetUp";
}

void AccessibilityAccountDataTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest TearDown";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState001
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState001 start";
    int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    /* Initial state */
    int stateType = accountData->GetAccessibilityState();
    int stateTypeTest = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED + AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    EXPECT_EQ(stateTypeTest, stateType);
    
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState002
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState002 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = connection->GetElementName().GetURI();
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    int stateType = accountData->GetAccessibilityState();
    int stateTypeTest = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED + AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState003
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState003 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    accountData->AddConnectingA11yAbility(elementName);
    bool test = accountData->GetConnectingA11yAbilities().find(elementName.GetURI()) != accountData->GetConnectingA11yAbilities().end();
    EXPECT_TRUE(test);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());

    int stateType = accountData->GetAccessibilityState();
    int stateTypeTest = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED + AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState004
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState004 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(elementName);

    int stateType = accountData->GetAccessibilityState();
    int stateTypeTest = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED + AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState004 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_OnAccountSwitched001
 * @tc.name: OnAccountSwitched
 * @tc.desc: Check the account switched.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_OnAccountSwitched001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_OnAccountSwitched001 start";
    // new accountData
    const int accountId = 101;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    // new abilityInfo
    AppExecFwk::AbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    // new AAconnection
    sptr<AccessibleAbilityConnection> AAConnection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    //new aastub
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    sptr<AccessibleAbilityClientStubImpl> aastub = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    GTEST_LOG_(INFO) << "OnAbilityConnectDone start !!!!!";
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    AAConnection->OnAbilityConnectDone(elementName, aastub, 0);
    GTEST_LOG_(INFO) << "add connected A11y Ability";
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* CapabilityValues */
    accountData->UpdateAccountCapabilities();
    bool test1 = accountData->GetGesturesSimulationFlag();
    EXPECT_TRUE(test1);
    bool test2 = accountData->GetFilteringKeyEventsFlag();
    EXPECT_TRUE(test2);
    bool test3 = accountData->GetEventTouchGuideStateFlag();
    EXPECT_TRUE(test3);
    bool test4 = accountData->GetScreenMagnificationFlag();
    EXPECT_TRUE(test4);
    /* Account Switched */
    GTEST_LOG_(INFO) << "OnAccountSwitched";
    accountData->OnAccountSwitched();
    /* initialization */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    bool test5 = accountData->GetGesturesSimulationFlag();
    EXPECT_FALSE(test5);
    bool test6 = accountData->GetFilteringKeyEventsFlag();
    EXPECT_FALSE(test6);
    bool test7 = accountData->GetEventTouchGuideStateFlag();
    EXPECT_FALSE(test7);
    bool test8 = accountData->GetScreenMagnificationFlag();
    EXPECT_FALSE(test8);
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_OnAccountSwitched001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddConnectedAbility001
 * @tc.name: AddConnectedAbility
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddConnectedAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectedAbility001 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = connection->GetElementName().GetURI();
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectedAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveConnectedAbility001
 * @tc.name: RemoveConnectedAbility
 * @tc.desc: Check the remove connected ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveConnectedAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectedAbility001 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = connection->GetElementName().GetURI();
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    /* remove */
    accountData->RemoveConnectedAbility(connection);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectedAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AbilityDisconnected001
 * @tc.name: AbilityDisconnected
 * @tc.desc: Check the ability disconnected.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AbilityDisconnected001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AbilityDisconnected001 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    const std::string elementNameURI = connection->GetElementName().GetURI();
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementNameURI));

    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* disconnect */
    accountData->AbilityDisconnected(connection);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());
    AppExecFwk::ElementName& elementName = connection->GetElementName();
    bool test = accountData->GetConnectingA11yAbilities().find(elementName.GetURI()) != accountData->GetConnectingA11yAbilities().end();
    EXPECT_TRUE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AbilityDisconnected001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddConnectingA11yAbility001
 * @tc.name: AddConnectingA11yAbility
 * @tc.desc: Check the add connecting A11y ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddConnectingA11yAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectingA11yAbility001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(elementName);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());
    bool test = accountData->GetConnectingA11yAbilities().find(elementName.GetURI()) != accountData->GetConnectingA11yAbilities().end();
    EXPECT_TRUE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectingA11yAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001
 * @tc.name: RemoveConnectingA11yAbility
 * @tc.desc: Check the remove connecting A11y ability.GetAccessibilityWindows
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(elementName);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());
    bool test = accountData->GetConnectingA11yAbilities().find(elementName.GetURI()) != accountData->GetConnectingA11yAbilities().end();
    EXPECT_TRUE(test);
    /* remove */
    accountData->RemoveConnectingA11yAbility(elementName);
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddEnabledAbility001
 * @tc.name: AddEnabledAbility
 * @tc.desc: Check the add enable ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddEnabledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddEnabledAbility001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    EXPECT_EQ(0, (int)accountData->GetEnabledAbilities().size());
    /* add */
    accountData->AddEnabledAbility(elementName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    bool test = accountData->GetEnabledAbilities().find(elementName.GetURI()) != accountData->GetEnabledAbilities().end();
    EXPECT_TRUE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddEnabledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveEnabledAbility001
 * @tc.name: RemoveEnabledAbility
 * @tc.desc: Check the remove enable ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveEnabledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    EXPECT_EQ(0, (int)accountData->GetEnabledAbilities().size());
    /* add */
    accountData->AddEnabledAbility(elementName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    bool test = accountData->GetEnabledAbilities().find(elementName.GetURI()) != accountData->GetEnabledAbilities().end();
    EXPECT_TRUE(test);
    /* remove */
    accountData->RemoveEnabledAbility(elementName);
    EXPECT_EQ(0, (int)accountData->GetEnabledAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddInstalledAbility001
 * @tc.name: AddInstalledAbility
 * @tc.desc: Check the add install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddInstalledAbility001 start";
    const int accountId = 1;
    AppExecFwk::AbilityInfo info;
    info.bundleName = "TEST_BUNDLE_NAME";
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveInstalledAbility001
 * @tc.name: RemoveInstalledAbility
 * @tc.desc: Check the remove install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveInstalledAbility001 start";
    const int accountId = 1;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* remove */
    accountData->RemoveInstalledAbility(*abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_ClearInstalledAbility001
 * @tc.name: ClearInstalledAbility
 * @tc.desc: Check the clear install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ClearInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility001 start";
    const int accountId = 1;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* clear */
    accountData->ClearInstalledAbility();
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = connection->GetElementName().GetURI();
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    accountData->AddConnectedAbility(connection);
    const std::string elementName = "111name";
    /* get */ 
    EXPECT_EQ(nullptr, accountData->GetAccessibleAbilityConnection(elementName));
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001
 * @tc.name: GetConnectedA11yAbilities
 * @tc.desc: Check the get connected abilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001 start";
    const int accountId = 1;
    int connectCounter = 0;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    /* get */
    int size = accountData->GetConnectedA11yAbilities().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddConnectedAbility(connection);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetStateCallbacks001
 * @tc.name: GetStateCallbacks
 * @tc.desc: Check the get state callbacks.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetStateCallbacks001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetStateCallbacks001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    const sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);
    /* get */
    int size = accountData->GetStateCallbacks().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddStateCallback(state);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetStateCallbacks().size());
    
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetStateCallbacks001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAsacConnections001
 * @tc.name: GetAsacConnections
 * @tc.desc: Check the get asac connections.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAsacConnections001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAsacConnections001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    /* get */ 
    int size = accountData->GetAsacConnections().size();
    EXPECT_EQ(0, size);

    const int windowId = 1;
    sptr<AccessibilityInteractionOperationStub> operationStub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> operation = new AccessibilityInteractionOperationProxy(operationStub);
    sptr<AccessibilityInteractionConnection> operationConnection = new AccessibilityInteractionConnection(windowId,
                                                                                                          operation, 
                                                                                                          accountId);
    /* add asacConnections */
    accountData->AddAccessibilityInteractionConnection(windowId, operationConnection);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAsacConnections001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetConnectingA11yAbilities001
 * @tc.name: GetConnectingA11yAbilities
 * @tc.desc: Check the get connecting abilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetConnectingA11yAbilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectingA11yAbilities001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    /* get */ 
    int size = accountData->GetConnectingA11yAbilities().size();
    EXPECT_EQ(0, size);
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(elementName);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectingA11yAbilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetEnabledAbilities001
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Check the get enable abilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetEnabledAbilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetEnabledAbilities001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const AppExecFwk::ElementName elementName("111", "222" , "333");
    /* get */
    int size = accountData->GetEnabledAbilities().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddEnabledAbility(elementName);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetEnabledAbilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetInstalledAbilities001
 * @tc.name: GetInstalledAbilities
 * @tc.desc: Check the get install abilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetInstalledAbilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetInstalledAbilities001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    /* get */
    const int size = accountData->GetInstalledAbilities().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    /* get */
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetInstalledAbilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetInteractiveUiInterval001
 * @tc.name: GetInteractiveUiInterval
 * @tc.desc: Check the get interactive Ui interval.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetInteractiveUiInterval001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetInteractiveUiInterval001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const uint32_t value = accountData->GetInteractiveUiInterval();
    const uint32_t test = 0;
    EXPECT_EQ(test, value);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetInteractiveUiInterval001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetNonInteractiveUiInterval001
 * @tc.name: GetNonInteractiveUiInterval
 * @tc.desc: Check the get nonInteractive Ui interval.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetNonInteractiveUiInterval001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetNonInteractiveUiInterval001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const uint32_t value = accountData->GetNonInteractiveUiInterval();
    const uint32_t test = 0;
    EXPECT_EQ(test, value);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetNonInteractiveUiInterval001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001
 * @tc.name: GetAccountInteractiveUiInterval
 * @tc.desc: Check the get account interactive Ui interval.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const uint32_t value = accountData->GetAccountInteractiveUiInterval();
    const uint32_t test = 0;
    EXPECT_EQ(test, value);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccountNonInteractiveUiInterval001
 * @tc.name: GetAccountNonInteractiveUiInterval
 * @tc.desc: Check the get account nonInteractive Ui interval.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccountNonInteractiveUiInterval001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    const uint32_t value = accountData->GetAccountNonInteractiveUiInterval();
    const uint32_t test = 0;
    EXPECT_EQ(test, value);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccountInteractiveUiInterval001 start";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState001
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState001 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* ABILITY_STATE_ENABLE */
    int stateType = AbilityStateType::ABILITY_STATE_ENABLE;
    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    /* get ability */
    std::vector<AccessibilityAbilityInfo> enabledAbilities = accountData->GetAbilitiesByState(state);
    EXPECT_EQ(1, (int)enabledAbilities.size());
    EXPECT_EQ(connection->GetAbilityInfo().GetPackageName(), enabledAbilities.begin()->GetPackageName());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState002
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState002 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* add install ability */
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* ABILITY_STATE_DISABLE */
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    /* get ability */
    std::vector<AccessibilityAbilityInfo> disabledAbilities = accountData->GetAbilitiesByState(state);
    EXPECT_EQ(0, (int)disabledAbilities.size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState003
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState003 start";
    const int accountId = 1;
    int connectCounter = 0;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AppExecFwk::AbilityInfo info;
    info.name = "TEST_ABILITY_NAME";
    info.bundleName = "TEST_BUNDLE_NAME";
    sptr<AccessibilityAbilityInfo> installAbilityInfo = new AccessibilityAbilityInfo(info);
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_NE(abilityInfo->GetId(), installAbilityInfo->GetId());
    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* add install ability */
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*installAbilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* ABILITY_STATE_DISABLE */
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    /* get ability */
    std::vector<AccessibilityAbilityInfo> disabledAbilities = accountData->GetAbilitiesByState(state);
    EXPECT_EQ(1, (int)disabledAbilities.size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState004
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState004 start";
    const int accountId = 1;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    /* add install ability */
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* ABILITY_STATE_DISABLE */
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    /* get ability */
    std::vector<AccessibilityAbilityInfo> disabledAbilities = accountData->GetAbilitiesByState(state);
    EXPECT_EQ(1, (int)disabledAbilities.size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState004 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState005
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState005 start";
    const int accountId = 1;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    /* add install ability */
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* ABILITY_STATE_INSTALLED */
    int stateType = AbilityStateType::ABILITY_STATE_INSTALLED;
    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    /* get ability */
    std::vector<AccessibilityAbilityInfo> installAbilities = accountData->GetAbilitiesByState(state);
    EXPECT_EQ(1, (int)installAbilities.size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState005 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddAccessibilityInteractionConnection001
 * @tc.name: AddAccessibilityInteractionConnection
 * @tc.desc: Check the add accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddAccessibilityInteractionConnection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddAccessibilityInteractionConnection001 star";
    const int accountId = 1;
    const int windowId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> operation = new AccessibilityInteractionOperationProxy(stub);
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(windowId, operation, 
                                                                                                 accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityInteractionConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibilityInteractionConnection(windowId));

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddAccessibilityInteractionConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityInteractionConnection001
 * @tc.name: GetAccessibilityInteractionConnection
 * @tc.desc: Check the get accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityInteractionConnection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityInteractionConnection001 star";
    const int accountId = 1;
    const int windowId = 1;
    const int windowIdTest = 2;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> operation = new AccessibilityInteractionOperationProxy(stub);
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(windowId, operation, 
                                                                                                 accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityInteractionConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(nullptr, accountData->GetAccessibilityInteractionConnection(windowIdTest));

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityInteractionConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveAccessibilityInteractionConnection001
 * @tc.name: RemoveAccessibilityInteractionConnection
 * @tc.desc: Check the remove accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveAccessibilityInteractionConnection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAccessibilityInteractionConnection001 star";
    const int accountId = 1;
    const int windowId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> operation = new AccessibilityInteractionOperationProxy(stub);
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(windowId, operation, 
                                                                                                 accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityInteractionConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* remove */
    accountData->RemoveAccessibilityInteractionConnection(windowId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAccessibilityInteractionConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddStateCallback001
 * @tc.name: AddStateCallback
 * @tc.desc: Check the add State Callback.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddStateCallback001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddStateCallback001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    const sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);
    EXPECT_EQ(0, (int)accountData->GetStateCallbacks().size());
    /* add */
    accountData->AddStateCallback(state);
    EXPECT_EQ(1, (int)accountData->GetStateCallbacks().size());
    /* get */
    bool test = find(accountData->GetStateCallbacks().begin(), accountData->GetStateCallbacks().end(), state) != 
        accountData->GetStateCallbacks().end();

    EXPECT_TRUE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddStateCallback001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveStateCallback001
 * @tc.name: RemoveStateCallback
 * @tc.desc: Check the remove state Callback.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveStateCallback001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveStateCallback001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    const sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub->AsObject());
    EXPECT_EQ(0, (int)accountData->GetStateCallbacks().size());
    /* add */
    accountData->AddStateCallback(state);
    EXPECT_EQ(1, (int)accountData->GetStateCallbacks().size());
    /* remove */
    accountData->RemoveStateCallback(state);
    EXPECT_EQ(0, (int)accountData->GetStateCallbacks().size());

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveStateCallback001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAccountCapabilities001
 * @tc.name: UpdateAccountCapabilities
 * @tc.desc: Check the UpdateAccountCapabilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAccountCapabilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    accountData->UpdateAccountCapabilities();
    /* no ability */
    bool test1 = accountData->GetGesturesSimulationFlag();
    EXPECT_FALSE(test1);
    bool test2 = accountData->GetFilteringKeyEventsFlag();
    EXPECT_FALSE(test2);
    bool test3 = accountData->GetEventTouchGuideStateFlag();
    EXPECT_FALSE(test3);
    bool test4 = accountData->GetScreenMagnificationFlag();
    EXPECT_FALSE(test4);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAccountCapabilities002
 * @tc.name: UpdateAccountCapabilities
 * @tc.desc: Check the UpdateAccountCapabilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAccountCapabilities002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities002 start";
    int connectCounter = 0;
    const int accountId = 101;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->UpdateAccountCapabilities();
    /* no CapabilityValues */
    bool test1 = accountData->GetGesturesSimulationFlag();
    EXPECT_FALSE(test1);
    bool test2 = accountData->GetFilteringKeyEventsFlag();
    EXPECT_FALSE(test2);
    bool test3 = accountData->GetEventTouchGuideStateFlag();
    EXPECT_FALSE(test3);
    bool test4 = accountData->GetScreenMagnificationFlag();
    EXPECT_FALSE(test4);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAccountCapabilities003
 * @tc.name: UpdateAccountCapabilities
 * @tc.desc: Check the UpdateAccountCapabilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAccountCapabilities003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities003 start";
    int connectCounter = 0;
    const int accountId = 101;
    AppExecFwk::AbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter++, 
        *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->UpdateAccountCapabilities();
    /* CapabilityValues */
    bool test1 = accountData->GetGesturesSimulationFlag();
    EXPECT_TRUE(test1);
    bool test2 = accountData->GetFilteringKeyEventsFlag();
    EXPECT_TRUE(test2);
    bool test3 = accountData->GetEventTouchGuideStateFlag();
    EXPECT_TRUE(test3);
    bool test4 = accountData->GetScreenMagnificationFlag();
    EXPECT_TRUE(test4);
    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAccountCapabilities003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetScreenMagnificationFlag001
 * @tc.name: GetScreenMagnificationFlag
 * @tc.desc: Check the GetScreenMagnificationFlag.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetScreenMagnificationFlag001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetScreenMagnificationFlag001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    bool test = accountData->GetScreenMagnificationFlag();
    EXPECT_FALSE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetScreenMagnificationFlag001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetEventTouchGuideStateFlag001
 * @tc.name: GetEventTouchGuideStateFlag
 * @tc.desc: Check the GetEventTouchGuideStateFlag.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetEventTouchGuideStateFlag001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetEventTouchGuideStateFlag001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    bool test = accountData->GetEventTouchGuideStateFlag();
    EXPECT_FALSE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetEventTouchGuideStateFlag001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetFilteringKeyEventsFlag001
 * @tc.name: GetFilteringKeyEventsFlag
 * @tc.desc: Check the GetFilteringKeyEventsFlag.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetFilteringKeyEventsFlag001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetFilteringKeyEventsFlag001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    bool test = accountData->GetFilteringKeyEventsFlag();
    EXPECT_FALSE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetFilteringKeyEventsFlag001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetGesturesSimulationFlag001
 * @tc.name: GetGesturesSimulationFlag
 * @tc.desc: Check the GetGesturesSimulationFlag.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetGesturesSimulationFlag001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetGesturesSimulationFlag001 start";
    const int accountId = 1;
    AccessibilityAccountData *accountData = new AccessibilityAccountData(accountId);
    bool test = accountData->GetGesturesSimulationFlag();
    EXPECT_FALSE(test);

    delete accountData;
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetGesturesSimulationFlag001 end";
}
}  // namespace Accessibility
}  // namespace OHOS
