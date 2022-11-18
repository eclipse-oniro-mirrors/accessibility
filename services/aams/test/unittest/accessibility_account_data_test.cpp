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
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_common_helper.h"
#include "accessibility_constants.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessible_ability_manager_service_config_observer_proxy.h"
#include "mock_accessible_ability_manager_service_config_observer_stub.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
    constexpr size_t IMPORTANT_ABILITIES_SIZE = 1;
} // namespace

class AccessibilityAccountDataTest : public testing::Test {
public:
    AccessibilityAccountDataTest()
    {}
    ~AccessibilityAccountDataTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilityAccountDataTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
}

void AccessibilityAccountDataTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityAccountDataTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
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
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState001 start";
    int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    /* Initial state */

    accountData->Init();
    accountData->GetConfig()->SetTouchGuideState(true);
    uint32_t stateType = accountData->GetAccessibilityState();
    uint32_t stateTypeTest = Accessibility::STATE_EXPLORATION_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState002
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState002 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);

    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    accountData->Init();
    uint32_t stateType = accountData->GetAccessibilityState();
    uint32_t stateTypeTest = Accessibility::STATE_ACCESSIBILITY_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState003
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState003 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    const std::string bundleName = "bbb";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    /* add connecting A11y ability */

    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));

    accountData->Init();
    accountData->GetConfig()->SetKeyEventObserverState(true);

    uint32_t stateType = accountData->GetAccessibilityState();
    uint32_t stateTypeTest = Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_KEYEVENT_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityState004
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityState004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState004 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    /* add connecting A11y ability */

    const std::string bundleName = "bbb";
    accountData->AddConnectingA11yAbility(bundleName, connection);

    accountData->Init();
    accountData->GetConfig()->SetGestureState(true);

    uint32_t stateType = accountData->GetAccessibilityState();
    uint32_t stateTypeTest = Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_GESTURE_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityState004 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddConnectedAbility001
 * @tc.name: AddConnectedAbility
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddConnectedAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectedAbility001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectedAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveConnectedAbility001
 * @tc.name: RemoveConnectedAbility
 * @tc.desc: Check the remove connected ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveConnectedAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectedAbility001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    /* remove */
    accountData->RemoveConnectedAbility(connection->GetElementName());
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectedAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AbilityDisconnected001
 * @tc.name: AbilityDisconnected
 * @tc.desc: Check the ability disconnected.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AbilityDisconnected001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AbilityDisconnected001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    const std::string elementNameURI = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementNameURI));

    EXPECT_FALSE(accountData->GetConnectingA11yAbility(elementNameURI));
    /* disconnect */
    accountData->RemoveConnectedAbility(connection->GetElementName());
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AbilityDisconnected001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001
 * @tc.name: RemoveConnectingA11yAbility
 * @tc.desc: Check the remove connecting A11y ability.GetAccessibilityWindows
 */
HWTEST_F(
    AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    const std::string bundleName = "bbb";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    /* add connecting A11y ability */

    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, 0, *abilityInfo);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    /* remove */
    accountData->RemoveConnectingA11yAbility(bundleName);
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveConnectingA11yAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddEnabledAbility001
 * @tc.name: AddEnabledAbility
 * @tc.desc: Check the add enable ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddEnabledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddEnabledAbility001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    const std::string bundleName = "/bbb";
    accountData->Init();
    EXPECT_EQ(0, (int)accountData->GetConfig()->GetEnabledAbilityInfos().size());
    /* add */
    AccessibilityAbilityInitParams initParams;
    initParams.name = "bbb";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->AddInstalledAbility(*abilityInfo);
    accountData->AddEnabledAbility(bundleName);
    accountData->AddEnabledAbility(bundleName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddEnabledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveEnabledAbility001
 * @tc.name: RemoveEnabledAbility
 * @tc.desc: Check the remove enable ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveEnabledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    const std::string bundleName = "/bbb";
    accountData->Init();
    EXPECT_EQ(0, (int)accountData->GetConfig()->GetEnabledAbilityInfos().size());

    /* remove */
    accountData->RemoveEnabledAbility(bundleName);
    EXPECT_EQ(0, (int)accountData->GetEnabledAbilities().size());

    /* add */
    AccessibilityAbilityInitParams initParams;
    initParams.name = "bbb";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->AddInstalledAbility(*abilityInfo);

    accountData->AddEnabledAbility(bundleName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());

    /* remove */
    accountData->RemoveEnabledAbility(bundleName);
    EXPECT_EQ(0, (int)accountData->GetEnabledAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddInstalledAbility001
 * @tc.name: AddInstalledAbility
 * @tc.desc: Check the add install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddInstalledAbility001 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "TEST_BUNDLE_NAME";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveInstalledAbility001
 * @tc.name: RemoveInstalledAbility
 * @tc.desc: Check the remove install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveInstalledAbility001 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* remove */
    accountData->RemoveInstalledAbility(abilityInfo->GetPackageName());
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_ClearInstalledAbility001
 * @tc.name: ClearInstalledAbility
 * @tc.desc: Check the clear install ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ClearInstalledAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility001 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* clear */
    accountData->ClearInstalledAbility();
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(
    AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(elementName));
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(
    AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    accountData->AddConnectedAbility(connection);
    const std::string elementName = "111name";
    /* get */
    EXPECT_EQ(nullptr, accountData->GetAccessibleAbilityConnection(elementName));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibleAbilityConnection002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001
 * @tc.name: GetConnectedA11yAbilities
 * @tc.desc: Check the get connected abilities.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    /* get */
    size_t size = accountData->GetConnectedA11yAbilities().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddConnectedAbility(connection);
    /* get */
    EXPECT_EQ(1, accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetConnectedA11yAbilities001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAsacConnections001
 * @tc.name: GetAsacConnections
 * @tc.desc: Check the get asac connections.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAsacConnections001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAsacConnections001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    /* get */
    size_t size = accountData->GetAsacConnections().size();
    EXPECT_EQ(0, size);

    const int32_t windowId = 1;
    sptr<AccessibilityElementOperatorStub> operationStub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(operationStub);
    sptr<AccessibilityWindowConnection> operationConnection =
        new AccessibilityWindowConnection(windowId, operation, accountId);
    /* add asacConnections */
    accountData->AddAccessibilityWindowConnection(windowId, operationConnection);
    /* get */
    EXPECT_EQ(1, accountData->GetAsacConnections().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAsacConnections001 end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetEnabled, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetEnabled start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->Init();
    bool state = true;
    bool test = accountData->GetConfig()->SetEnabled(state);
    EXPECT_TRUE(test);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetEnabled end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState001
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* ABILITY_STATE_ENABLE */
    AbilityStateType state = AbilityStateType::ABILITY_STATE_ENABLE;;
    /* get ability */
    std::vector<AccessibilityAbilityInfo> enabledAbilities;
    accountData->GetAbilitiesByState(state, enabledAbilities);
    EXPECT_EQ(1, (int)enabledAbilities.size());
    EXPECT_EQ(connection->GetAbilityInfo().GetPackageName(), enabledAbilities.begin()->GetPackageName());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAbilitiesByState003
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAbilitiesByState003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState003 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;

    AccessibilityAbilityInitParams initParamsDefault;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParamsDefault);
    AccessibilityAbilityInitParams initParams;
    initParams.name = "TEST_ABILITY_NAME";
    initParams.bundleName = "TEST_BUNDLE_NAME";
    std::shared_ptr<AccessibilityAbilityInfo> installAbilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);

    EXPECT_NE(abilityInfo->GetId(), installAbilityInfo->GetId());

    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    /* add install ability */
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*installAbilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    sleep(SLEEP_TIME_1);

    /* ABILITY_STATE_DISABLE */
    AbilityStateType state = AbilityStateType::ABILITY_STATE_DISABLE;

    /* get ability */
    std::vector<AccessibilityAbilityInfo> disabledAbilities;
    accountData->GetAbilitiesByState(state, disabledAbilities);
    EXPECT_EQ(1, (int)disabledAbilities.size());
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAbilitiesByState003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddAccessibilityWindowConnection001
 * @tc.name: AddAccessibilityWindowConnection
 * @tc.desc: Check the add accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddAccessibilityWindowConnection001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddAccessibilityWindowConnection001 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityWindowConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibilityWindowConnection(windowId));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddAccessibilityWindowConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetAccessibilityWindowConnection001
 * @tc.name: GetAccessibilityWindowConnection
 * @tc.desc: Check the get accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetAccessibilityWindowConnection001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityWindowConnection001 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    const int32_t windowIdTest = 2;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityWindowConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(nullptr, accountData->GetAccessibilityWindowConnection(windowIdTest));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetAccessibilityWindowConnection001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveAccessibilityWindowConnection001
 * @tc.name: RemoveAccessibilityWindowConnection
 * @tc.desc: Check the remove accessibility interaction connection.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveAccessibilityWindowConnection001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAccessibilityWindowConnection001 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityWindowConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* remove */
    accountData->RemoveAccessibilityWindowConnection(windowId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAccessibilityWindowConnection001 end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetCaptionState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetCaptionState start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->Init();
    bool state = true;
    accountData->GetConfig()->SetCaptionState(state);

    bool result = accountData->GetConfig()->GetCaptionState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetCaptionState end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_OnAccountSwitched001
 * @tc.name: OnAccountSwitched
 * @tc.desc: Check the account switched.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_OnAccountSwitched001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_OnAccountSwitched001 start";
    // new accountData
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->Init();
    // new abilityInfo
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE | CAPABILITY_TOUCH_GUIDE | CAPABILITY_GESTURE |
                                     CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_ZOOM);
    // new AAconnection
    sptr<AccessibleAbilityConnection> AAConnection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    // new aastub
    sptr<AccessibleAbilityClientStub> aastub = new MockAccessibleAbilityClientStubImpl();
    GTEST_LOG_(INFO) << "OnAbilityConnectDoneSync start !!!!!";
    const AppExecFwk::ElementName elementName("aaa", "bbb", "ccc");
    AAConnection->OnAbilityConnectDoneSync(elementName, aastub);
    GTEST_LOG_(INFO) << "add connected A11y Ability";
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* CapabilityValues */
    accountData->UpdateAccountCapabilities();
    EXPECT_TRUE(accountData->GetGesturesSimulationFlag());
    EXPECT_TRUE(accountData->GetFilteringKeyEventsFlag());
    EXPECT_TRUE(accountData->GetEventTouchGuideStateFlag());
    EXPECT_TRUE(accountData->GetScreenMagnificationFlag());
    /* Account Switched */
    GTEST_LOG_(INFO) << "OnAccountSwitched";
    accountData->OnAccountSwitched();
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTestChannelId() == INVALID_CHANNEL_ID) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    accountData->UpdateAccountCapabilities();
    /* initialization */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    EXPECT_FALSE(accountData->GetGesturesSimulationFlag());
    EXPECT_FALSE(accountData->GetFilteringKeyEventsFlag());
    EXPECT_FALSE(accountData->GetEventTouchGuideStateFlag());
    EXPECT_FALSE(accountData->GetScreenMagnificationFlag());
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_OnAccountSwitched001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddConnectingA11yAbility001
 * @tc.name: AddConnectingA11yAbility
 * @tc.desc: Check the add connecting A11y ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddConnectingA11yAbility001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectingA11yAbility001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    const std::string bundleName = "bbb";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    /* add connecting A11y ability */
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(accountId, 0, *abilityInfo);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddConnectingA11yAbility001 end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetTouchGuideState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetTouchGuideState start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->Init();
    bool state = true;
    bool test = accountData->GetConfig()->SetTouchGuideState(state);
    EXPECT_TRUE(test);

    bool result = accountData->GetConfig()->GetTouchGuideState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetTouchGuideState end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetGestureState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetGestureState start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->Init();
    bool state = false;
    bool test = accountData->GetConfig()->SetGestureState(state);
    EXPECT_TRUE(test);

    bool result = accountData->GetConfig()->GetGestureState();
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetGestureState end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetKeyEventObserverState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetKeyEventObserverState start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->Init();
    bool state = true;
    bool test = accountData->GetConfig()->SetKeyEventObserverState(state);
    EXPECT_TRUE(test);

    bool result = accountData->GetConfig()->GetKeyEventObserverState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetKeyEventObserverState end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_SetCaptionProperty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetCaptionProperty start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    AccessibilityConfig::CaptionProperty caption;
    accountData->Init();
    accountData->GetConfig()->SetCaptionProperty(caption);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_SetCaptionProperty end";
}

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ClearInstalledAbility, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "TEST_BUNDLE_NAME";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    accountData->AddInstalledAbility(*abilityInfo);
    accountData->ClearInstalledAbility();

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ClearInstalledAbility end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveEnabledAbility
 * @tc.name: RemoveEnabledAbility
 * @tc.desc: Remove the enabled ability.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveEnabledAbility, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);

    const std::string bundleName = "bbb";
    accountData->Init();
    accountData->RemoveEnabledAbility(bundleName);

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveEnabledAbility end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_EnableAbility_001
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability with no capability
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    accountData->Init();

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    const std::string name = "bundle/ability";
    uint32_t capabilities = 0;
    RetError test = accountData->EnableAbility(name, capabilities);
    EXPECT_EQ(test, RET_ERR_NO_CAPABILITY);
    ASSERT_EQ(0, (int)accountData->GetConfig()->GetEnabledAbilityInfos().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_EnableAbility_002
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability which is already enabled.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_002 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    accountData->Init();

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    initParams.staticCapabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    std::string name = "bundle/ability";
    accountData->AddEnabledAbility(name);
    EXPECT_EQ(RET_ERR_CONNECTION_EXIST, accountData->EnableAbility(name, CAPABILITY_RETRIEVE));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_EnableAbility_003
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability successfully.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_003 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    accountData->Init();

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    initParams.staticCapabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    const std::string name = "bundle/ability";
    EXPECT_EQ(RET_OK, accountData->EnableAbility(name, CAPABILITY_RETRIEVE));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_EnableAbility_004
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability which is not installed.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbility_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_004 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    EXPECT_EQ(RET_ERR_NOT_INSTALLED, accountData->EnableAbility("bundle/ability", 0));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility_004 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_001
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities when there is no installed ability.
 */
HWTEST_F(AccessibilityAccountDataTest,
    AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_001 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    std::map<std::string, uint32_t> abilities;
    accountData->GetImportantEnabledAbilities(abilities);
    EXPECT_EQ(0, static_cast<int>(abilities.size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_002
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities when there is no enabled ability.
 */
HWTEST_F(AccessibilityAccountDataTest,
    AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_002 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    std::map<std::string, uint32_t> abilities;
    accountData->GetImportantEnabledAbilities(abilities);
    EXPECT_EQ(0, static_cast<int>(abilities.size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_003
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities.
 */
HWTEST_F(AccessibilityAccountDataTest,
    AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_003 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle1";
    initParams.name = "testAbility1";
    initParams.isImportant = false;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    initParams.bundleName = "testBundle2";
    initParams.name = "testAbility2";
    initParams.isImportant = true;
    std::shared_ptr<AccessibilityAbilityInfo> importantAbilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*importantAbilityInfo);

    accountData->AddEnabledAbility("testBundle1/testAbility1");
    accountData->AddEnabledAbility("testBundle2/testAbility2");

    std::map<std::string, uint32_t> abilities;
    accountData->GetImportantEnabledAbilities(abilities);
    EXPECT_EQ(IMPORTANT_ABILITIES_SIZE, abilities.size());
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetImportantEnabledAbilities_003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateImportantEnabledAbilities
 * @tc.name: UpdateImportantEnabledAbilities
 * @tc.desc: Update important enabled abilities.
 */
HWTEST_F(AccessibilityAccountDataTest,
    AccessibilityAccountData_Unittest_UpdateImportantEnabledAbilities, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateImportantEnabledAbilities start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    std::map<std::string, uint32_t> abilities;
    accountData->UpdateImportantEnabledAbilities(abilities);

    abilities.emplace(std::make_pair("testBundle/testAbility", CAPABILITY_RETRIEVE));
    accountData->UpdateImportantEnabledAbilities(abilities);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    initParams.name = "testAbility";
    initParams.isImportant = true;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE | CAPABILITY_GESTURE);
    accountData->AddInstalledAbility(*abilityInfo);

    accountData->UpdateImportantEnabledAbilities(abilities);
    ASSERT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    EXPECT_EQ(accountData->GetInstalledAbilities().front().GetCapabilityValues(), CAPABILITY_RETRIEVE);
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateImportantEnabledAbilities end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAbilities_001
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an installed ability which is connecting.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_001 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "connectingBundle";
    initParams.name = "connectingAbility";
    std::shared_ptr<AccessibilityAbilityInfo> connectingAbilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*connectingAbilityInfo);

    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(0, 0, *connectingAbilityInfo);
    accountData->AddConnectingA11yAbility("connectingBundle/connectingAbility", connection);
    accountData->UpdateAbilities();
    EXPECT_TRUE(accountData->GetConnectingA11yAbility("connectingBundle/connectingAbility"));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAbilities_002
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an ability which is already connected.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_002 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    accountData->AddInstalledAbility(*abilityInfo);

    AppExecFwk::ElementName elementName("testDeviceId", "testBundle", "testAbility");
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    accountData->AddEnabledAbility(Utils::GetUri(connection->GetElementName()));

    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(Utils::GetUri(connection->GetElementName())));

    accountData->UpdateAbilities();
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(Utils::GetUri(connection->GetElementName())));
    accountData->OnAccountSwitched();
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAbilities_003
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an enabled ability which is no connected.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAbilities_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_003 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    initParams.name = "testAbility";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    accountData->AddEnabledAbility("testBundle/testAbility");

    accountData->UpdateAbilities();
    EXPECT_TRUE(accountData->GetConnectingA11yAbility("testBundle/testAbility"));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_003 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_UpdateAbilities_004
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an connected ability which is no enabled.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_UpdateAbilities_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_004 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    accountData->AddInstalledAbility(*abilityInfo);

    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    AppExecFwk::ElementName elementName("testDeviceId", "testBundle", "testAbility");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(Utils::GetUri(connection->GetElementName())));

    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(0);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), 0);

    accountData->UpdateAbilities();
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), -1);
    accountData->OnAccountSwitched();
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_UpdateAbilities_004 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveAbility_001
 * @tc.name: RemoveAbility
 * @tc.desc: Remove ability when there is no ability
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAbility_001 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    EXPECT_FALSE(accountData->RemoveAbility("bundle"));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAbility_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveAbility_002
 * @tc.name: RemoveAbility
 * @tc.desc: Remove ability successfully
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAbility_002 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    initParams.name = "testAbility";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    accountData->AddEnabledAbility("testBundle/testAbility");

    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(0, 0, *abilityInfo);
    accountData->AddConnectedAbility(connection);

    EXPECT_TRUE(accountData->RemoveAbility("testBundle"));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveAbility_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_ChangeAbility_001
 * @tc.name: ChangeAbility
 * @tc.desc: Change ability when there is no ability
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ChangeAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ChangeAbility_001 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    accountData->ChangeAbility("bundle");
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ChangeAbility_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_ChangeAbility_002
 * @tc.name: ChangeAbility
 * @tc.desc: Change ability which is not installed.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ChangeAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ChangeAbility_002 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    accountData->ChangeAbility("test");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ChangeAbility_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_AddUITestClient
 * @tc.name: AddUITestClient
 * @tc.desc: Add ui test client
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_AddUITestClient, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddUITestClient start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_AddUITestClient end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveUITestClient_001
 * @tc.name: RemoveUITestClient
 * @tc.desc: Remove ui test client failed.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveUITestClient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveUITestClient_001 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    sptr<AccessibleAbilityConnection> connection = nullptr;
    accountData->RemoveUITestClient(connection, "uitestBundle");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveUITestClient_001 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_RemoveUITestClient_002
 * @tc.name: RemoveUITestClient
 * @tc.desc: Remove ui test client successfully.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_RemoveUITestClient_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveUITestClient_002 start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "uitestBundle";
    initParams.name = "uitestAbility";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(0, 0, *abilityInfo);

    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    accountData->RemoveUITestClient(connection, "uitestBundle");
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_RemoveUITestClient_002 end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_CaptionPropertyCallback
 * @tc.name: CaptionPropertyCallback
 * @tc.desc: Check the functions of AddCaptionPropertyCallback and RemoveCaptionPropertyCallback.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_CaptionPropertyCallback, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_CaptionPropertyCallback start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    sptr<AccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> caption =
        new MockAccessibleAbilityManagerCaptionObserverProxy(stub);
    EXPECT_EQ(0, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    accountData->AddCaptionPropertyCallback(caption);
    EXPECT_EQ(1, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    accountData->RemoveCaptionPropertyCallback(caption->AsObject());
    EXPECT_EQ(0, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_CaptionPropertyCallback end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_EnableAbilityListsObserver
 * @tc.name: EnableAbilityListsObserver
 * @tc.desc: Check the functions of AddEnableAbilityListsObserver and RemoveEnableAbilityListsObserver.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbilityListsObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbilityListsObserver start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    sptr<AccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    accountData->AddEnableAbilityListsObserver(observer);
    accountData->AddEnableAbilityListsObserver(observer);
    accountData->RemoveEnableAbilityListsObserver(observer->AsObject());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbilityListsObserver end";
}

/**
 * @tc.number: AccessibilityAccountData_Unittest_ConfigCallback
 * @tc.name: ConfigCallback
 * @tc.desc: Check the functions of AddConfigCallback and RemoveConfigCallback.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_ConfigCallback, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ConfigCallback start";
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(0);
    sptr<AccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> observer = new MockAccessibleAbilityManagerConfigObserverProxy(stub);
    EXPECT_EQ(0, static_cast<int>(accountData->GetConfigCallbacks().size()));

    accountData->AddConfigCallback(observer);
    EXPECT_EQ(1, static_cast<int>(accountData->GetConfigCallbacks().size()));

    accountData->RemoveConfigCallback(observer->AsObject());
    EXPECT_EQ(0, static_cast<int>(accountData->GetConfigCallbacks().size()));

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_ConfigCallback end";
}
} // namespace Accessibility
} // namespace OHOS