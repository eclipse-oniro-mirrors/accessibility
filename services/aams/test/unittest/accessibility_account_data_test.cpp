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
#include "accessibility_constants.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    const std::string bundleName = "bbb";

    accountData->AddConnectingA11yAbility(bundleName);

    std::vector<std::string> res;
    res = accountData->GetConnectingA11yAbilities();

    std::vector<string>::iterator iter;
    iter = find(res.begin(), res.end(), bundleName);
    bool test = false;
    if (iter != res.end()) {
        test = true;
    }
    EXPECT_TRUE(test);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());

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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    /* add connecting A11y ability */

    const std::string bundleName = "bbb";
    accountData->AddConnectingA11yAbility(bundleName);

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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    /* remove */
    accountData->RemoveConnectedAbility(connection);
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add */
    accountData->AddConnectedAbility(connection);
    const std::string elementNameURI = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementNameURI));

    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* disconnect */
    accountData->RemoveConnectedAbility(connection);
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
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(bundleName);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());
    std::vector<std::string> res;
    res = accountData->GetConnectingA11yAbilities();

    std::vector<string>::iterator iter;
    iter = find(res.begin(), res.end(), bundleName);
    bool test = false;
    if (iter != res.end()) {
        test = true;
    }
    EXPECT_TRUE(test);
    /* remove */
    accountData->RemoveConnectingA11yAbility(bundleName);
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());

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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = Utils::GetUri(connection->GetElementName());
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
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
 * @tc.number: AccessibilityAccountData_Unittest_GetStateCallbacks001
 * @tc.name: GetStateCallbacks
 * @tc.desc: Check the get state callbacks.
 */
HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_GetStateCallbacks001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetStateCallbacks001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    const sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);
    /* get */
    size_t size = accountData->GetStateCallbacks().size();
    EXPECT_EQ(0, size);
    /* add */
    accountData->AddStateCallback(state);
    /* get */
    EXPECT_EQ(1, accountData->GetStateCallbacks().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_GetStateCallbacks001 end";
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);
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
        new MockAccessibleAbilityConnection(accountData, connectCounter++, *abilityInfo);

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
    const int32_t accountId = 101;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    accountData->Init();
    // new abilityInfo
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE | CAPABILITY_TOUCH_GUIDE | CAPABILITY_GESTURE |
                                     CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_ZOOM);
    // new AAconnection
    sptr<AccessibleAbilityConnection> AAConnection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    // new aastub
    sptr<AccessibleAbilityClientStub> aastub = new MockAccessibleAbilityClientStubImpl();
    GTEST_LOG_(INFO) << "OnAbilityConnectDoneSync start !!!!!";
    const AppExecFwk::ElementName elementName("aaa", "bbb", "ccc");
    AAConnection->OnAbilityConnectDoneSync(elementName, aastub, 0);
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
    AAConnection->OnAbilityDisconnectDoneSync(elementName, 0);
    accountData->UpdateAccountCapabilities();
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
    EXPECT_EQ(0, (int)accountData->GetConnectingA11yAbilities().size());
    /* add connecting A11y ability */
    accountData->AddConnectingA11yAbility(bundleName);
    EXPECT_EQ(1, (int)accountData->GetConnectingA11yAbilities().size());
    std::vector<std::string> res;
    res = accountData->GetConnectingA11yAbilities();

    std::vector<string>::iterator iter;
    iter = find(res.begin(), res.end(), bundleName);
    bool test = false;
    if (iter != res.end()) {
        test = true;
    }
    EXPECT_TRUE(test);

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

HWTEST_F(AccessibilityAccountDataTest, AccessibilityAccountData_Unittest_EnableAbility, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility start";
    const int32_t accountId = 1;
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    accountData->Init();
    const std::string name = "bundle/ability";
    uint32_t capabilities = 0;
    bool test = accountData->EnableAbility(name, capabilities);
    EXPECT_FALSE(test);
    ASSERT_EQ(0, (int)accountData->GetConfig()->GetEnabledAbilityInfos().size());

    GTEST_LOG_(INFO) << "AccessibilityAccountData_Unittest_EnableAbility end";
}
} // namespace Accessibility
} // namespace OHOS