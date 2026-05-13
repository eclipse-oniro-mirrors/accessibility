/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_common_helper.h"
#include "accessibility_constants.h"
#include "accessibility_enable_ability_lists_observer_stub.h"
#include "accessibility_enable_ability_callback_observer_stub.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t ACCOUNT_ID = 100;
    constexpr uint32_t CAPABILITY_TEST = CAPABILITY_RETRIEVE | CAPABILITY_TOUCH_GUIDE;
    const std::string TEST_BUNDLE_NAME = "testBundle";
    const std::string TEST_ABILITY_NAME = "testAbility";
    const std::string TEST_URI = "testBundle/testAbility";
    const std::string SCREEN_READER_BUNDLE_NAME = "com.ohos.hmos.screenreader";
}

class MockAccessibilityEnableAbilityListsObserver : public AccessibilityEnableAbilityListsObserverStub {
public:
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD0(OnAccessibilityEnableAbilityListsChanged, void());
    MOCK_METHOD0(OnAccessibilityInstallAbilityListsChanged, void());
};

class MockAccessibilityEnableAbilityCallbackObserver : public AccessibilityEnableAbilityCallbackObserverStub {
public:
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD1(OnEnableAbilityRemoteDied, void(const std::string& uri));
};

class AccessibleAbilityManagerTest : public testing::Test {
public:
    AccessibleAbilityManagerTest() {}
    ~AccessibleAbilityManagerTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<AccessibleAbilityManager> manager_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
};

void AccessibleAbilityManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityManagerTest Start ######################";
}

void AccessibleAbilityManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityManagerTest End ######################";
}

void AccessibleAbilityManagerTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    manager_ = std::make_unique<AccessibleAbilityManager>();
    accountData_ = new AccessibilityAccountData(ACCOUNT_ID);
}

void AccessibleAbilityManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    manager_.reset();
    accountData_ = nullptr;
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_Destructor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Destructor_001 start";
    auto manager = std::make_unique<AccessibleAbilityManager>();
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager->AddConnectedAbility(connection);
    manager->AddConnectingAbility(TEST_URI, connection);
    manager->AddWaitDisconnectAbility(connection);
    manager->AddAppStateObserverAbility(TEST_URI, connection);
    
    EXPECT_NE(manager, nullptr);
    manager.reset();
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Destructor_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_Clear_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Clear_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    manager_->AddEnabledAbility(TEST_URI);
    
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    
    manager_->Clear();
    
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Clear_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddConnectedAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectedAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectedAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddConnectedAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectedAbility_002 start";
    sptr<AccessibleAbilityConnection> connection = nullptr;
    
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectedAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveConnectedAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    
    AppExecFwk::ElementName element("", TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    manager_->RemoveConnectedAbility(element);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveConnectedAbilityByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByUri_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    manager_->RemoveConnectedAbilityByUri(uri);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveConnectedAbilityByName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByName_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    
    bool result = false;
    manager_->RemoveConnectedAbilityByName(TEST_BUNDLE_NAME, result);
    EXPECT_TRUE(result);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByName_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectedAbilityByName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilityByName_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::string elementName = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    auto result = manager_->GetConnectedAbilityByName(elementName);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilityByName_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectedAbilityByName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilityByName_002 start";
    std::string elementName = "nonexistent";
    auto result = manager_->GetConnectedAbilityByName(elementName);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilityByName_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectedAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilities_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    manager_->GetConnectedAbilities(connectionList);
    EXPECT_EQ(connectionList.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectedAbilitiesInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilitiesInfo_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetConnectedAbilitiesInfo(abilities);
    EXPECT_EQ(abilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilitiesInfo_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectedAbilitiesMap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilitiesMap_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    manager_->GetConnectedAbilitiesMap(connectionMap);
    EXPECT_EQ(connectionMap.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectedAbilitiesMap_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_ClearConnectedAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ClearConnectedAbilities_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 1);
    
    manager_->ClearConnectedAbilities();
    EXPECT_EQ(manager_->GetConnectedAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ClearConnectedAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddConnectingAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectingAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    EXPECT_EQ(manager_->GetConnectingAbilitiesSize(), 0);
    manager_->AddConnectingAbility(TEST_URI, connection);
    EXPECT_EQ(manager_->GetConnectingAbilitiesSize(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddConnectingAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveConnectingAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectingAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectingAbility(TEST_URI, connection);
    EXPECT_EQ(manager_->GetConnectingAbilitiesSize(), 1);
    
    manager_->RemoveConnectingAbility(TEST_URI);
    EXPECT_EQ(manager_->GetConnectingAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectingAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectingAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectingAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectingAbility(TEST_URI, connection);
    
    auto result = manager_->GetConnectingAbility(TEST_URI);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectingAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetConnectingAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectingAbility_002 start";
    auto result = manager_->GetConnectingAbility("nonexistent");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetConnectingAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddWaitDisconnectAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddWaitDisconnectAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    EXPECT_EQ(manager_->GetWaitDisconnectAbilitiesSize(), 0);
    manager_->AddWaitDisconnectAbility(connection);
    EXPECT_EQ(manager_->GetWaitDisconnectAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddWaitDisconnectAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddWaitDisconnectAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddWaitDisconnectAbility_002 start";
    sptr<AccessibleAbilityConnection> connection = nullptr;
    
    EXPECT_EQ(manager_->GetWaitDisconnectAbilitiesSize(), 0);
    manager_->AddWaitDisconnectAbility(connection);
    EXPECT_EQ(manager_->GetWaitDisconnectAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddWaitDisconnectAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveWaitDisconnectAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveWaitDisconnectAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    manager_->RemoveWaitDisconnectAbility(uri);
    EXPECT_EQ(manager_->GetWaitDisconnectAbilitiesSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveWaitDisconnectAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetWaitDisConnectAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetWaitDisConnectAbility_001 start";
    std::string elementName = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    auto result = manager_->GetWaitDisConnectAbility(elementName);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetWaitDisConnectAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetWaitDisConnectAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetWaitDisConnectAbility_002 start";
    std::string elementName = "nonexistent";
    auto result = manager_->GetWaitDisConnectAbility(elementName);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetWaitDisConnectAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAppStateObserverAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddAppStateObserverAbility(TEST_URI, connection);
    auto result = manager_->GetAppStateObserverAbility(TEST_URI);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAppStateObserverAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_002 start";
    sptr<AccessibleAbilityConnection> connection = nullptr;
    
    manager_->AddAppStateObserverAbility(TEST_URI, connection);
    auto result = manager_->GetAppStateObserverAbility(TEST_URI);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAppStateObserverAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_003 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddAppStateObserverAbility("", connection);
    auto result = manager_->GetAppStateObserverAbility("");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAppStateObserverAbility_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveAppStateObserverAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAppStateObserverAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddAppStateObserverAbility(TEST_URI, connection);
    manager_->RemoveAppStateObserverAbility(TEST_URI);
    auto result = manager_->GetAppStateObserverAbility(TEST_URI);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAppStateObserverAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveAppStateObserverAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAppStateObserverAbility_002 start";
    manager_->RemoveAppStateObserverAbility("");
    EXPECT_EQ(manager_->GetAppStateObserverAbility(""), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAppStateObserverAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddEnabledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddEnabledAbility_001 start";
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 0);
    manager_->AddEnabledAbility(TEST_URI);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddEnabledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddEnabledAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddEnabledAbility_002 start";
    manager_->AddEnabledAbility(TEST_URI);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    manager_->AddEnabledAbility(TEST_URI);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddEnabledAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveEnabledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveEnabledAbility_001 start";
    manager_->AddEnabledAbility(TEST_URI);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    
    RetError result = manager_->RemoveEnabledAbility(TEST_URI);
    EXPECT_EQ(result, RET_OK);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveEnabledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveEnabledAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveEnabledAbility_002 start";
    RetError result = manager_->RemoveEnabledAbility(TEST_URI);
    EXPECT_EQ(result, RET_ERR_NOT_ENABLED);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveEnabledAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetEnabledAbilities_001 start";
    manager_->AddEnabledAbility(TEST_URI);
    manager_->AddEnabledAbility("testBundle/testAbility2");
    
    const auto& abilities = manager_->GetEnabledAbilities();
    EXPECT_EQ(abilities.size(), 2);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddInstalledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddInstalledAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    manager_->AddInstalledAbility(abilityInfo);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddInstalledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddInstalledAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddInstalledAbility_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    manager_->AddInstalledAbility(abilityInfo);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddInstalledAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveInstalledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveInstalledAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    
    manager_->RemoveInstalledAbility(TEST_BUNDLE_NAME);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveInstalledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_ClearInstalledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ClearInstalledAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    
    manager_->ClearInstalledAbility();
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ClearInstalledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateInstalledAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_TEST;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::string name = TEST_BUNDLE_NAME + "/" + TEST_ABILITY_NAME;
    RetError result = manager_->UpdateInstalledAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateInstalledAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_TEST;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::string name = TEST_BUNDLE_NAME + "/" + TEST_ABILITY_NAME;
    RetError result = manager_->UpdateInstalledAbility(name, CAPABILITY_GESTURE);
    EXPECT_EQ(result, RET_ERR_NOT_ENABLED);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateInstalledAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_003 start";
    std::string name = TEST_BUNDLE_NAME + "/" + TEST_ABILITY_NAME;
    RetError result = manager_->UpdateInstalledAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(result, RET_ERR_NOT_INSTALLED);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateInstalledAbility_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetAbilitiesByState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetAbilitiesByState(ABILITY_STATE_INSTALLED, abilities);
    EXPECT_EQ(abilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetAbilitiesByState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetAbilitiesByState(ABILITY_STATE_ENABLE, abilities);
    EXPECT_EQ(abilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetAbilitiesByState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    std::shared_ptr<AccessibilityAbilityInfo> connectedAbilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *connectedAbilityInfo, accountData_);
    
    manager_->AddInstalledAbility(abilityInfo);
    manager_->AddConnectedAbility(connection);
    
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetAbilitiesByState(ABILITY_STATE_DISABLE, abilities);
    EXPECT_EQ(abilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetDisableAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetDisableAbilities_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::vector<AccessibilityAbilityInfo> disabledAbilities;
    manager_->GetDisableAbilities(disabledAbilities);
    EXPECT_EQ(disabledAbilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetDisableAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    manager_->AddEnabledAbility(TEST_URI);
    
    bool result = manager_->RemoveAbility(TEST_BUNDLE_NAME);
    EXPECT_TRUE(result);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_002 start";
    bool result = manager_->RemoveAbility(TEST_BUNDLE_NAME);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    bool result = manager_->RemoveAbility("nonexistent");
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveAbility_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_TEST;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    std::vector<AccessibilityAbilityInfo> abilityInfos;
    abilityInfos.push_back(abilityInfo);
    
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return true;
    };
    
    manager_->AddAbility(TEST_BUNDLE_NAME, abilityInfos, autoStartChecker);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    std::vector<AccessibilityAbilityInfo> abilityInfos;
    abilityInfos.push_back(abilityInfo);
    
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return false;
    };
    
    manager_->AddAbility(TEST_BUNDLE_NAME, abilityInfos, autoStartChecker);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AddAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    std::vector<AccessibilityAbilityInfo> abilityInfos;
    abilityInfos.push_back(abilityInfo);
    
    manager_->AddAbility(TEST_BUNDLE_NAME, abilityInfos, nullptr);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AddAbility_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_ChangeAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return true;
    };
    auto autoStartSetter = [](const std::string& name, bool state) {};
    
    manager_->ChangeAbility(TEST_BUNDLE_NAME, autoStartChecker, autoStartSetter);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_ChangeAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_002 start";
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return true;
    };
    auto autoStartSetter = [](const std::string& name, bool state) {};
    
    manager_->ChangeAbility(TEST_BUNDLE_NAME, autoStartChecker, autoStartSetter);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_ChangeAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    manager_->ChangeAbility(TEST_BUNDLE_NAME, nullptr, nullptr);
    EXPECT_EQ(manager_->GetInstalledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_ChangeAbility_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetImportantEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_001 start";
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->GetImportantEnabledAbilities(importantEnabledAbilities);
    EXPECT_EQ(importantEnabledAbilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetImportantEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.isImportant = true;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    manager_->AddEnabledAbility(TEST_URI);
    
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->GetImportantEnabledAbilities(importantEnabledAbilities);
    EXPECT_EQ(importantEnabledAbilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetImportantEnabledAbilities_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.isImportant = false;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    manager_->AddEnabledAbility(TEST_URI);
    
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->GetImportantEnabledAbilities(importantEnabledAbilities);
    EXPECT_EQ(importantEnabledAbilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateImportantEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_001 start";
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->UpdateImportantEnabledAbilities(importantEnabledAbilities);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateImportantEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_TEST;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::map<std::string, uint32_t> importantEnabledAbilities;
    importantEnabledAbilities[TEST_URI] = CAPABILITY_RETRIEVE;
    
    manager_->UpdateImportantEnabledAbilities(importantEnabledAbilities);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_001 start";
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return true;
    };
    
    manager_->UpdateAutoStartEnabledAbilities(autoStartChecker);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_TEST;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    auto autoStartChecker = [](const std::string& abilityId) -> bool {
        return true;
    };
    
    manager_->UpdateAutoStartEnabledAbilities(autoStartChecker);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_003 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    manager_->UpdateAutoStartEnabledAbilities(nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateAutoStartEnabledAbilities_003 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_IsExistCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    bool result = manager_->IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_IsExistCapability_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_002 start";
    bool result = manager_->IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetSizeByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    int32_t result = manager_->GetSizeByUri(uri);
    EXPECT_EQ(result, 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetSizeByUri_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_002 start";
    int32_t result = manager_->GetSizeByUri("nonexistent");
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetReadableRules_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = SCREEN_READER_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.readableRules = "testRules";
    AccessibilityAbilityInfo abilityInfo(initParams);
    
    manager_->AddInstalledAbility(abilityInfo);
    
    std::string readableRules;
    RetError result = manager_->GetReadableRules(readableRules);
    EXPECT_EQ(result, RET_OK);
    EXPECT_EQ(readableRules, "testRules");
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetReadableRules_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_002 start";
    std::string readableRules;
    RetError result = manager_->GetReadableRules(readableRules);
    EXPECT_EQ(result, RET_ERR_NOT_INSTALLED);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_NotifyExtensionServiceDeath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_NotifyExtensionServiceDeath_001 start";
    manager_->NotifyExtensionServiceDeath(TEST_URI);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_NotifyExtensionServiceDeath_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    EXPECT_EQ(ability.GetSize(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_002 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    ability.AddAccessibilityAbility(uri, connection);
    EXPECT_EQ(ability.GetSize(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_AddAccessibilityAbility_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    auto result = ability.GetAccessibilityAbilityByName(TEST_ABILITY_NAME);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_002 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    auto result = ability.GetAccessibilityAbilityByName("nonexistent");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByName_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    auto result = ability.GetAccessibilityAbilityByUri(uri);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_002 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    auto result = ability.GetAccessibilityAbilityByUri("nonexistent");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilityByUri_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilities_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    ability.GetAccessibilityAbilities(connectionList);
    EXPECT_EQ(connectionList.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAbilitiesInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAbilitiesInfo_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    std::vector<AccessibilityAbilityInfo> abilities;
    ability.GetAbilitiesInfo(abilities);
    EXPECT_EQ(abilities.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAbilitiesInfo_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    initParams.capabilities = CAPABILITY_RETRIEVE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    bool result = ability.IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_002 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    bool result = ability.IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_IsExistCapability_002 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilitiesMap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilitiesMap_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    ability.GetAccessibilityAbilitiesMap(connectionMap);
    EXPECT_EQ(connectionMap.size(), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetAccessibilityAbilitiesMap_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetDisableAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetDisableAbilities_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    AccessibilityAbilityInfo installedAbilityInfo(initParams);
    std::shared_ptr<AccessibilityAbilityInfo> connectedAbilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *connectedAbilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    std::vector<AccessibilityAbilityInfo> disabledAbilities;
    disabledAbilities.push_back(installedAbilityInfo);
    ability.GetDisableAbilities(disabledAbilities);
    EXPECT_EQ(disabledAbilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetDisableAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetSizeByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetSizeByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    int32_t result = ability.GetSizeByUri(uri);
    EXPECT_EQ(result, 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetSizeByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "com.test";
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri("com.test", TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    
    bool result = false;
    ability.RemoveAccessibilityAbilityByName("test", result);
    EXPECT_TRUE(result);
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    EXPECT_EQ(ability.GetSize(), 1);
    
    ability.RemoveAccessibilityAbilityByUri(uri);
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_Clear_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_Clear_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    ability.AddAccessibilityAbility(uri, connection);
    EXPECT_EQ(ability.GetSize(), 1);
    
    ability.Clear();
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_Clear_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_GetSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetSize_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection1 =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    sptr<AccessibleAbilityConnection> connection2 =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 1, *abilityInfo, accountData_);
    
    ability.AddAccessibilityAbility(Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME), connection1);
    ability.AddAccessibilityAbility(Utils::GetUri(TEST_BUNDLE_NAME, "ability2"), connection2);
    
    EXPECT_EQ(ability.GetSize(), 2);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_GetSize_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_WrongUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_WrongUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    std::string wrongUri = "wronguri";
    ability.AddAccessibilityAbility(wrongUri, connection);
    
    bool result = false;
    ability.RemoveAccessibilityAbilityByName("test", result);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_AccessibilityAbility_RemoveAccessibilityAbilityByName_WrongUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_RemoveConnectedAbilityByName_NotFound_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByName_NotFound_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = TEST_BUNDLE_NAME;
    initParams.name = TEST_ABILITY_NAME;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(ACCOUNT_ID, 0, *abilityInfo, accountData_);
    
    manager_->AddConnectedAbility(connection);
    
    bool result = false;
    manager_->RemoveConnectedAbilityByName("nonexistent", result);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_RemoveConnectedAbilityByName_NotFound_001 end";
}

} // namespace Accessibility
} // namespace OHOS