/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "accessible_ability_manager_state_observer_proxy.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
    constexpr size_t IMPORTANT_ABILITIES_SIZE = 0;
} // namespace

class AccessibleAbilityManagerStateObserverProxyTest : public testing::Test {
public:
    AccessibleAbilityManagerStateObserverProxyTest()
    {}
    ~AccessibleAbilityManagerStateObserverProxyTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibleAbilityManagerStateObserverProxyTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerStateObserverProxyTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
}

void AccessibleAbilityManagerStateObserverProxyTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerStateObserverProxyTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibleAbilityManagerStateObserverProxyTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerStateObserverProxyTest SetUp";
}

void AccessibleAbilityManagerStateObserverProxyTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerStateObserverProxyTest TearDown";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_001
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_001 start";
    int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    
    /* Initial state */
    accountData->Init();
    accountData->GetConfig()->SetTouchGuideState(true);
    uint32_t stateType = accountData->GetAccessibilityState();
    uint32_t stateTypeTest = Accessibility::STATE_EXPLORATION_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_001 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_002
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_002 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);

    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    
    // init
    accountData->Init();
    uint32_t stateType = accountData->GetAccessibilityState();
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_002 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_003
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_003 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_KEYEVENT_ENABLED);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_003 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_004
 * @tc.name: GetAccessibilityState
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_004 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_GESTURE_ENABLED);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_004 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_005
 * @tc.name: AddConnectedAbility
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_005 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_005 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_006
 * @tc.name: AddConnectedAbility
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_006 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityConnection> connection = nullptr;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    /* add connected ability */
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_006 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_007
 * @tc.name: RemoveConnectedAbility
 * @tc.desc: Check the remove connected ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_007 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_007 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_008
 * @tc.name: DelAutoStartPrefKeyInRemovePkg
 * @tc.desc: Check DelAutoStartPrefKeyInRemovePkg.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_008 start";
    const int32_t accountId = 1;
    const std::string bundleName = "test";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->DelAutoStartPrefKeyInRemovePkg(bundleName);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_008 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_009
 * @tc.name: DelAutoStartPrefKeyInRemovePkg
 * @tc.desc: Check DelAutoStartPrefKeyInRemovePkg.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_009 start";
    const int32_t accountId = 1;
    const std::string name = "testName";
    const std::string bundleName = "testBundleName";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    /*add*/
    AccessibilityAbilityInitParams initParams;
    initParams.name = name;
    initParams.bundleName = bundleName;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->AddInstalledAbility(*abilityInfo);
    accountData->DelAutoStartPrefKeyInRemovePkg(bundleName);
    std::string abilityId = bundleName + "/" + name;
    bool ret = accountData->GetAbilityAutoStartState(abilityId);
    EXPECT_EQ(false, ret);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_009 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_010
 * @tc.name: AbilityDisconnected
 * @tc.desc: Check the ability disconnected.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_010 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_010 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_011
 * @tc.name: RemoveConnectingA11yAbility
 * @tc.desc: Check the remove connecting A11y ability.GetAccessibilityWindows
 */
HWTEST_F(
    AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_011 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    const std::string bundleName = "bbb";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    /* add connecting A11y ability */
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, 0, *abilityInfo);
    /* add */
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    /* remove */
    accountData->RemoveConnectingA11yAbility(bundleName);
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_011 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_012
 * @tc.name: AddEnabledAbility
 * @tc.desc: Check the add enable ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_012 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    const std::string bundleName = "/bbb";
    accountData->Init();
    /* add */
    AccessibilityAbilityInitParams initParams;
    initParams.name = "bbb";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->AddInstalledAbility(*abilityInfo);
    accountData->AddEnabledAbility(bundleName);
    accountData->AddEnabledAbility(bundleName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_012 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_013
 * @tc.name: RemoveEnabledAbility
 * @tc.desc: Check the remove enable ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_013 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    const std::string bundleName = "/bbb";
    accountData->Init();

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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_013 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_014
 * @tc.name: AddInstalledAbility
 * @tc.desc: Check the add install ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_014 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "TEST_BUNDLE_NAME";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_014 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_015
 * @tc.name: RemoveInstalledAbility
 * @tc.desc: Check the remove install ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_015 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->Init();
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* remove */
    accountData->RemoveInstalledAbility(abilityInfo->GetPackageName());
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_015 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_016
 * @tc.name: ClearInstalledAbility
 * @tc.desc: Check the clear install ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_016 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    /* add */
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    /* clear */
    accountData->ClearInstalledAbility();
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_016 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_017
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(
    AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_017 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(elementName));
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_017 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_018
 * @tc.name: GetAccessibleAbilityConnection
 * @tc.desc: Check the get accessibility connection.
 */
HWTEST_F(
    AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_018 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    accountData->AddConnectedAbility(connection);
    const std::string elementName = "111name";
    /* get */
    EXPECT_EQ(nullptr, accountData->GetAccessibleAbilityConnection(elementName));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_018 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_019
 * @tc.name: GetConnectedA11yAbilities
 * @tc.desc: Check the get connected abilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_019 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_019 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_020
 * @tc.name: GetAsacConnections
 * @tc.desc: Check the get asac connections.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_020 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_020 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_021 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->Init();
    bool state = true;
    RetError test = accountData->GetConfig()->SetEnabled(state);
    EXPECT_EQ(test, RET_OK);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_021 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_022
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_022 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_022 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_023
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_023 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    /* add connected ability */
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->AddConnectedAbility(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    /* ABILITY_STATE_ENABLE */
    AbilityStateType state = AbilityStateType::ABILITY_STATE_INSTALLED;;
    /* get ability */
    std::vector<AccessibilityAbilityInfo> enabledAbilities;
    accountData->GetAbilitiesByState(state, enabledAbilities);
    EXPECT_EQ(0, (int)enabledAbilities.size());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_023 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_024
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_024 start";
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
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_024 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_025
 * @tc.name: GetAbilitiesByState
 * @tc.desc: Check the get ability state.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_025 start";
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
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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
    AbilityStateType state = AbilityStateType::ABILITY_STATE_INVALID;

    /* get ability */
    std::vector<AccessibilityAbilityInfo> disabledAbilities;
    accountData->GetAbilitiesByState(state, disabledAbilities);
    EXPECT_EQ(1, (int)disabledAbilities.size());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_025 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_026
 * @tc.name: AddAccessibilityWindowConnection
 * @tc.desc: Check the add accessibility interaction connection.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_026, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_026 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityWindowConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(connection, accountData->GetAccessibilityWindowConnection(windowId));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_026 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_027
 * @tc.name: UpdateEnableAbilityListsState
 * @tc.desc: Check the add connecting A11y ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_027, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_027 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        new MockAccessibilityEnableAbilityListsObserverProxy(stub);
    accountData->AddEnableAbilityListsObserver(observer);
    EXPECT_NE(accountData.GetRefPtr(), nullptr);
    accountData->UpdateEnableAbilityListsState();
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_027 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_028
 * @tc.name: UpdateEnableAbilityListsState
 * @tc.desc: Check the add connecting A11y ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_028, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_028 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        new AccessibilityEnableAbilityListsObserverProxy(stub);
    accountData->AddEnableAbilityListsObserver(observer);
    EXPECT_NE(accountData.GetRefPtr(), nullptr);
    accountData->UpdateEnableAbilityListsState();
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_028 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_029
 * @tc.name: GetAccessibilityWindowConnection
 * @tc.desc: Check the get accessibility interaction connection.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_029, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_029 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    const int32_t windowIdTest = 2;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> operation = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(windowId, operation, accountId);
    EXPECT_EQ(0, (int)accountData->GetAsacConnections().size());
    /* add */
    accountData->AddAccessibilityWindowConnection(windowId, connection);
    EXPECT_EQ(1, (int)accountData->GetAsacConnections().size());
    /* get */
    EXPECT_EQ(nullptr, accountData->GetAccessibilityWindowConnection(windowIdTest));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_029 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_030
 * @tc.name: RemoveAccessibilityWindowConnection
 * @tc.desc: Check the remove accessibility interaction connection.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_030, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_030 star";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_030 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_031, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_031 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->Init();
    bool state = true;
    accountData->GetConfig()->SetCaptionState(state);

    bool result = accountData->GetConfig()->GetCaptionState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_031 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_032
 * @tc.name: OnAccountSwitched
 * @tc.desc: Check the account switched.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_032, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_032 start";
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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_032 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_033
 * @tc.name: AddConnectingA11yAbility
 * @tc.desc: Check the add connecting A11y ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_033, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_033 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    const std::string bundleName = "bbb";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    /* add connecting A11y ability */
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(accountId, 0, *abilityInfo);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_033 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_034, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_034 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->Init();
    bool state = true;
    RetError test = accountData->GetConfig()->SetTouchGuideState(state);
    EXPECT_EQ(test, RET_OK);

    bool result = accountData->GetConfig()->GetTouchGuideState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_034 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_035, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_035 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->Init();
    bool state = false;
    RetError test = accountData->GetConfig()->SetGestureState(state);
    EXPECT_EQ(test, RET_OK);

    bool result = accountData->GetConfig()->GetGestureState();
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_035 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_036, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_036 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->Init();
    bool state = true;
    RetError test = accountData->GetConfig()->SetKeyEventObserverState(state);
    EXPECT_EQ(test, RET_OK);

    bool result = accountData->GetConfig()->GetKeyEventObserverState();
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_036 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_037, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_037 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    AccessibilityConfig::CaptionProperty caption;
    accountData->Init();
    accountData->GetConfig()->SetCaptionProperty(caption);

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_037 end";
}

HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_038, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_038 start";
    const int32_t accountId = 1;
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "TEST_BUNDLE_NAME";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    accountData->AddInstalledAbility(*abilityInfo);
    accountData->ClearInstalledAbility();
    EXPECT_NE(accountData.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_038 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_039
 * @tc.name: RemoveEnabledAbility
 * @tc.desc: Remove the enabled ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_039, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_039 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);

    const std::string bundleName = "bbb";
    accountData->Init();
    accountData->RemoveEnabledAbility(bundleName);
    EXPECT_NE(accountData.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_039 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_040
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability with no capability
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_040, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_040 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_040 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_041
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability which is already enabled.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_041, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_041 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
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

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_041 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_042
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability successfully.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_042, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_042 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    accountData->Init();

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    initParams.staticCapabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    const std::string name = "bundle/ability";
    EXPECT_EQ(RET_OK, accountData->EnableAbility(name, CAPABILITY_RETRIEVE));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_042 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_043
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability which is not installed.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_043, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_043 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    EXPECT_EQ(RET_ERR_NOT_INSTALLED, accountData->EnableAbility("bundle/ability", 0));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_043 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_044
 * @tc.name: EnableAbility
 * @tc.desc: Enable specified ability successfully.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_044, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_044 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    accountData->Init();
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    initParams.staticCapabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    const std::string name = "bundle/ability";
    EXPECT_EQ(RET_ERR_NO_CAPABILITY, accountData->EnableAbility(name, 0));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_044 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_045
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities when there is no installed ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_045, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_045 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    std::map<std::string, uint32_t> abilities;
    accountData->GetImportantEnabledAbilities(abilities);
    EXPECT_EQ(0, static_cast<int>(abilities.size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_045 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_046
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities when there is no enabled ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_046, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_046 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    std::map<std::string, uint32_t> abilities;
    accountData->GetImportantEnabledAbilities(abilities);
    EXPECT_EQ(0, static_cast<int>(abilities.size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_046 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_047
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_047, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_047 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_047 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_048
 * @tc.name: GetImportantEnabledAbilities
 * @tc.desc: Get important enabled abilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_048, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_048 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

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
    abilities.emplace(std::make_pair("testBundle2/testAbility2", CAPABILITY_RETRIEVE));
    EXPECT_NE(IMPORTANT_ABILITIES_SIZE, abilities.size());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_048 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_049
 * @tc.name: UpdateImportantEnabledAbilities
 * @tc.desc: Update important enabled abilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_049, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_049 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_049 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_050
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an installed ability which is connecting.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_050, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_050 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_050 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_051
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an ability which is already connected.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_051, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_051 start";
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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_051 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_052
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an enabled ability which is no connected.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_052, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_052 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    initParams.name = "testAbility";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);

    accountData->AddEnabledAbility("testBundle/testAbility");

    accountData->UpdateAbilities();
    EXPECT_TRUE(accountData->GetConnectingA11yAbility("testBundle/testAbility"));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_052 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_053
 * @tc.name: UpdateAbilities
 * @tc.desc: Update all abilities when there is an connected ability which is no enabled.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_053, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_053 start";
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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_053 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_054
 * @tc.name: RemoveAbility
 * @tc.desc: Remove ability when there is no ability
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_054, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_054 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    EXPECT_FALSE(accountData->RemoveAbility("bundle"));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_054 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_055
 * @tc.name: RemoveAbility
 * @tc.desc: Remove ability successfully
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_055, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_055 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

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
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_055 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_056
 * @tc.name: ChangeAbility
 * @tc.desc: Change ability when there is no ability
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_056, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_056 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    accountData->ChangeAbility("bundle");
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_056 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_057
 * @tc.name: ChangeAbility
 * @tc.desc: Change ability which is not installed.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_057, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_057 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    accountData->ChangeAbility("test");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_057 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_058
 * @tc.name: ChangeAbility
 * @tc.desc: Change ability which is installed.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_058, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_058 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundle";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    accountData->AddInstalledAbility(*abilityInfo);
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    const std::string abilityName = "testBundle";
    accountData->SetAbilityAutoStartState("testBundle", true);
    accountData->ChangeAbility("testBundle");
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_058 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_059
 * @tc.name: AddUITestClient
 * @tc.desc: Add ui test client
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_059, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_059 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_059 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_060
 * @tc.name: RemoveUITestClient
 * @tc.desc: Remove ui test client failed.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_060, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_060 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    sptr<AccessibleAbilityConnection> connection = nullptr;
    accountData->RemoveUITestClient(connection, "uitestBundle");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_060 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_061
 * @tc.name: RemoveUITestClient
 * @tc.desc: Remove ui test client successfully.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_061, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_061 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);

    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "uitestBundle";
    initParams.name = "uitestAbility";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(0, 0, *abilityInfo);

    accountData->AddUITestClient(nullptr, "uitestBundle", "uitestAbility");
    EXPECT_EQ(1, static_cast<int>(accountData->GetInstalledAbilities().size()));

    accountData->RemoveUITestClient(connection, "uitestBundle");
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_061 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_062
 * @tc.name: CaptionPropertyCallback
 * @tc.desc: Check the functions of AddCaptionPropertyCallback and RemoveCaptionPropertyCallback.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_062, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_062 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    sptr<AccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> caption =
        new MockAccessibleAbilityManagerCaptionObserverProxy(stub);
    EXPECT_EQ(0, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    accountData->AddCaptionPropertyCallback(caption);
    EXPECT_EQ(1, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    accountData->RemoveCaptionPropertyCallback(caption->AsObject());
    EXPECT_EQ(0, static_cast<int>(accountData->GetCaptionPropertyCallbacks().size()));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_062 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_063
 * @tc.name: EnableAbilityListsObserver
 * @tc.desc: Check the functions of AddEnableAbilityListsObserver and RemoveEnableAbilityListsObserver.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_063, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_063 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    sptr<AccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    accountData->AddEnableAbilityListsObserver(observer);
    accountData->AddEnableAbilityListsObserver(observer);
    accountData->RemoveEnableAbilityListsObserver(observer->AsObject());
    EXPECT_NE(accountData.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_063 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_064
 * @tc.name: ConfigCallback
 * @tc.desc: Check the functions of AddConfigCallback and RemoveConfigCallback.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_064, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_064 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
    sptr<AccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> observer = new MockAccessibleAbilityManagerConfigObserverProxy(stub);
    EXPECT_EQ(0, static_cast<int>(accountData->GetConfigCallbacks().size()));

    accountData->AddConfigCallback(observer);
    EXPECT_EQ(1, static_cast<int>(accountData->GetConfigCallbacks().size()));

    accountData->RemoveConfigCallback(observer->AsObject());
    EXPECT_EQ(0, static_cast<int>(accountData->GetConfigCallbacks().size()));

    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_064 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_065
 * @tc.name: AddAccessibilityAbility
 * @tc.desc: Check add accessibility ability.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_065, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_065 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    std::string uri = Utils::GetUri(connection->GetElementName());
    accountData->AddConnectingA11yAbility(uri, connection);
    auto connnection = accountData->GetConnectingA11yAbility(uri);
    EXPECT_NE(connnection.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_065 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_066
 * @tc.name: SetAbilityAutoStartState
 * @tc.desc: Check set and get AbilityAutoStartState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_066, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_066 start";
    const int32_t accountId = 1;
    const std::string abilityName = "test";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->SetAbilityAutoStartState(abilityName, false);
    EXPECT_EQ(accountData->GetAbilityAutoStartState(abilityName), false);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_066 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_067
 * @tc.name: SetAbilityAutoStartState
 * @tc.desc: Check set and get AbilityAutoStartState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_067, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_067 start";
    const int32_t accountId = 1;
    const std::string abilityName = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->SetAbilityAutoStartState(abilityName, false);
    EXPECT_EQ(accountData->GetAbilityAutoStartState(abilityName), false);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_067 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_068
 * @tc.name: SetAbilityAutoStartState
 * @tc.desc: Check set and get AbilityAutoStartState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_068, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_068 start";
    const int32_t accountId = 1;
    const std::string abilityName = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->SetAbilityAutoStartState(abilityName, true);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_068 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_069
 * @tc.name: UpdateAutoStartEnabledAbilities
 * @tc.desc: Check UpdateAutoStartEnabledAbilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_069, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_069 start";
    const int32_t accountId = -1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->UpdateAutoStartEnabledAbilities();
    EXPECT_EQ(-1, accountData->GetAccountId());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_069 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_070
 * @tc.name: UpdateAutoStartEnabledAbilities
 * @tc.desc: Check UpdateAutoStartEnabledAbilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_070, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_070 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->UpdateAutoStartEnabledAbilities();
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_070 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_071
 * @tc.name: UpdateAutoStartEnabledAbilities
 * @tc.desc: Check UpdateAutoStartEnabledAbilities.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_071, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_071 start";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData = new AccessibleAbilityManagerStateObserverProxy(0);
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
    accountData->Init();
    accountData->UpdateAutoStartEnabledAbilities();
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_071 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_072
 * @tc.name: GetInputFilterFlag
 * @tc.desc: Check GetInputFilterFlag.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_072, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_072 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    EXPECT_EQ(0, accountData->GetInputFilterFlag());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_072 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_073
 * @tc.name: GetInputFilterFlag
 * @tc.desc: Check GetInputFilterFlag.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_073, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_073 start";
    const int32_t accountId = 0;
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->Init();
    EXPECT_EQ(0, accountData->GetInputFilterFlag());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_073 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_074
 * @tc.name: GetInputFilterFlag
 * @tc.desc: Check GetInputFilterFlag.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_074, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_074 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->Init();
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_ZOOM);
    sptr<AccessibleAbilityConnection> AAConnection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    sptr<AccessibleAbilityClientStub> aastub = new MockAccessibleAbilityClientStubImpl();
    const AppExecFwk::ElementName elementName("aaa", "bbb", "ccc");
    AAConnection->OnAbilityConnectDoneSync(elementName, aastub);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->UpdateAccountCapabilities();
    EXPECT_EQ(0, accountData->GetInputFilterFlag());
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_074 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_075
 * @tc.name: SetScreenReaderState
 * @tc.desc: Check SetScreenReaderState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_075, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_075 start";
    const int32_t accountId = 1;
    const std::string abilityName = "test";
    const std::string state = "off";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->SetScreenReaderState(abilityName, state);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_075 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_076
 * @tc.name: SetScreenReaderState
 * @tc.desc: Check SetScreenReaderState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_076, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_076 start";
    const int32_t accountId = 1;
    const std::string name = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string state = "on";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->SetScreenReaderState(name, state);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_076 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_077
 * @tc.name: GetAbilityAutoStartState
 * @tc.desc: Check GetAbilityAutoStartState.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_077, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_077 start";
    const int32_t accountId = 1;
    const std::string abilityName = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->Init();
    accountData->SetAbilityAutoStartState(abilityName, false);
    EXPECT_EQ(accountData->GetAbilityAutoStartState(abilityName), false);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_077 end";
}

/**
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_078
 * @tc.name: AddAbility
 * @tc.desc: Check the AddAbility.
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_078, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_078 start";
    const int32_t accountId = 1;
    const std::string bundleName = "test";
    sptr<AccessibleAbilityManagerStateObserverProxy> accountData =
        new AccessibleAbilityManagerStateObserverProxy(accountId);
    accountData->AddAbility(bundleName);
    EXPECT_EQ(0, static_cast<int>(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_078 end";
}

/**OnShortKeyProcess
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_079
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_079, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_079 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetShortKeyTimeout(), SHORT_KEY_TIMEOUT_BEFORE_USE);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_079 end";
}

/**OnShortKeyProcess
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_080
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_080, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_080 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    EXPECT_EQ(accountData->GetConfig()->GetShortkeyMultiTarget().size(), 0);

    size_t size = accountData->GetConfig()->GetEnabledAccessibilityServices().size();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetEnabledAccessibilityServices().size(), size);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_080 end";
}

/**OnShortKeyProcess
 * @tc.number: ManagerStateObserverProxy_Unittest_Test_081
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibleAbilityManagerStateObserverProxyTest, ManagerStateObserverProxy_Unittest_Test_081, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_081 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    std::vector<std::string> name;
    name.push_back("test1");
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    EXPECT_EQ(accountData->GetConfig()->GetShortkeyMultiTarget().size(), 1);

    size_t size = accountData->GetConfig()->GetEnabledAccessibilityServices().size();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetEnabledAccessibilityServices().size(), size);
    GTEST_LOG_(INFO) << "ManagerStateObserverProxy_Unittest_Test_081 end";
}
} // namespace Accessibility
} // namespace OHOS