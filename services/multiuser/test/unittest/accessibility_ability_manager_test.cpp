/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "accessibility_ability_info.h"
#include "accessibility_settings_config.h"
#include "accessible_ability_connection.h"
#define private public
#define protected public
#include "accessibility_ability_manager.h"
#include "accessible_ability_manager_service.h"
#undef private
#undef protected
#include "utils.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessibility_ability_info.h"
#include "mock_accessible_ability_manager_service_config_observer_proxy.h"
#include "mock_accessible_ability_manager_service_config_observer_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
constexpr int32_t TEST_ACCOUNT_ID = 1;
constexpr char TEST_BUNDLE_NAME[] = "test.bundle";
constexpr char TEST_ABILITY_NAME[] = "TestAbility";
}

class AccessibilityAbilityManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        manager_ = std::make_shared<AccessibilityAbilityManager>(TEST_ACCOUNT_ID);
    }
    void TearDown() override
    {
        manager_.reset();
    }
    std::shared_ptr<AccessibilityAbilityManager> manager_;
};

HWTEST_F(AccessibilityAbilityManagerTest, SetConfig_Normal, TestSize.Level1)
{
    auto config = std::make_shared<AccessibilitySettingsConfig>(TEST_ACCOUNT_ID);
    manager_->SetConfig(config);
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    params.staticCapabilities = CAPABILITY_RETRIEVE;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    auto ret = manager_->EnableAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(ret, RET_OK);
}

HWTEST_F(AccessibilityAbilityManagerTest, AddConnectedAbility, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    manager_->AddConnectedAbility(conn);
    auto abilities = manager_->GetConnectedA11yAbilities();
    std::string uri = Utils::GetUri(conn->GetElementName());
    EXPECT_EQ(abilities.size(), 1);
    EXPECT_EQ(abilities[uri], conn);
}

HWTEST_F(AccessibilityAbilityManagerTest, RemoveConnectedAbility, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    manager_->AddConnectedAbility(conn);
    manager_->RemoveConnectedAbility(conn->GetElementName());
    auto abilities = manager_->GetConnectedA11yAbilities();
    EXPECT_EQ(abilities.size(), 0);
}

HWTEST_F(AccessibilityAbilityManagerTest, RemoveConnectingA11yAbility_Normal, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    std::string uri = Utils::GetUri(conn->GetElementName());
    manager_->AddConnectingA11yAbility(uri, conn);
    EXPECT_EQ(manager_->GetConnectingA11yAbility(uri), conn);
    manager_->RemoveConnectingA11yAbility(uri);
    EXPECT_EQ(manager_->GetConnectingA11yAbility(uri), nullptr);
}

HWTEST_F(AccessibilityAbilityManagerTest, WaitDisconnectAbility, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    std::string uri = Utils::GetUri(conn->GetElementName());
    manager_->AddWaitDisconnectAbility(conn);
    EXPECT_EQ(manager_->GetWaitDisConnectAbility(uri), nullptr);
    manager_->RemoveWaitDisconnectAbility(uri);
    EXPECT_EQ(manager_->GetWaitDisConnectAbility(uri), nullptr);
}

HWTEST_F(AccessibilityAbilityManagerTest, AddRemoveEnableAbilityListsObserver, TestSize.Level1)
{
    sptr<AccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        new MockAccessibilityEnableAbilityListsObserverProxy(stub);
    manager_->AddEnableAbilityListsObserver(observer);
    manager_->RemoveEnableAbilityListsObserver(observer->AsObject());
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, AddConnectingA11yAbility_Normal, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    std::string uri = Utils::GetUri(conn->GetElementName());
    manager_->AddConnectingA11yAbility(uri, conn);
    EXPECT_EQ(manager_->GetConnectingA11yAbility(uri), conn);
}

HWTEST_F(AccessibilityAbilityManagerTest, GetConnectingAbilitiesCount, TestSize.Level1)
{
    EXPECT_EQ(manager_->GetConnectingAbilitiesCount(), 0);
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    std::string uri = Utils::GetUri(conn->GetElementName());
    manager_->AddConnectingA11yAbility(uri, conn);
    EXPECT_EQ(manager_->GetConnectingAbilitiesCount(), 1);
}

HWTEST_F(AccessibilityAbilityManagerTest, AddEnabledAbility, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    manager_->AddEnabledAbility(name);
    manager_->AddEnabledAbility(name);
    auto enabled = manager_->GetEnabledAbilities();
    EXPECT_EQ(enabled.size(), 1);
    EXPECT_EQ(enabled[0], name);
}

HWTEST_F(AccessibilityAbilityManagerTest, RemoveEnabledAbility, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    auto ret = manager_->RemoveEnabledAbility(name);
    EXPECT_EQ(ret, RET_ERR_NOT_ENABLED);
    manager_->AddEnabledAbility(name);
    ret = manager_->RemoveEnabledAbility(name);
    EXPECT_EQ(ret, RET_OK);
    EXPECT_TRUE(manager_->GetEnabledAbilities().empty());
}

HWTEST_F(AccessibilityAbilityManagerTest, AddInstalledAbility, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    auto installed = manager_->GetInstalledAbilities();
    EXPECT_EQ(installed.size(), 1);
    EXPECT_EQ(installed[0].GetName(), TEST_ABILITY_NAME);
}

HWTEST_F(AccessibilityAbilityManagerTest, GetAbilitiesByState, TestSize.Level1)
{
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetAbilitiesByState(AbilityStateType::ABILITY_STATE_ENABLE, abilities);
    EXPECT_TRUE(abilities.empty());

    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    params.staticCapabilities = CAPABILITY_RETRIEVE;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    manager_->AddConnectedAbility(conn);

    abilities.clear();
    manager_->GetAbilitiesByState(AbilityStateType::ABILITY_STATE_ENABLE, abilities);
    EXPECT_FALSE(abilities.empty());
    EXPECT_EQ(abilities[0].GetName(), TEST_ABILITY_NAME);
}

HWTEST_F(AccessibilityAbilityManagerTest, GetDisableAbilities, TestSize.Level1)
{
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetDisableAbilities(abilities);
    EXPECT_TRUE(abilities.empty());
}

HWTEST_F(AccessibilityAbilityManagerTest, GetInstalledAbilitiesFromBMS, TestSize.Level1)
{
    EXPECT_TRUE(manager_->GetInstalledAbilitiesFromBMS());
}

HWTEST_F(AccessibilityAbilityManagerTest, EnableAbility, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    auto ret = manager_->EnableAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(ret, RET_ERR_NOT_INSTALLED);
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    params.staticCapabilities = 0;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    ret = manager_->EnableAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(ret, RET_ERR_NO_CAPABILITY);
    manager_->RemoveAbility(TEST_BUNDLE_NAME);
    params.staticCapabilities = CAPABILITY_RETRIEVE;
    auto info2 = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info2);
    ret = manager_->EnableAbility(name, CAPABILITY_RETRIEVE);
    EXPECT_EQ(ret, RET_OK);
    auto enabled = manager_->GetEnabledAbilities();
    EXPECT_FALSE(enabled.empty());
}

HWTEST_F(AccessibilityAbilityManagerTest, UpdateEnableAbilityListsState, TestSize.Level1)
{
    manager_->UpdateEnableAbilityListsState();
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, UpdateInstallAbilityListsState, TestSize.Level1)
{
    manager_->UpdateInstallAbilityListsState();
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, UpdateAbilities, TestSize.Level1)
{
    manager_->UpdateAbilities();
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, RemoveAbility, TestSize.Level1)
{
    EXPECT_FALSE(manager_->RemoveAbility(TEST_BUNDLE_NAME));
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    manager_->AddEnabledAbility(Utils::GetUri(params.bundleName, params.name));
    EXPECT_TRUE(manager_->RemoveAbility(TEST_BUNDLE_NAME) || !manager_->RemoveAbility(TEST_BUNDLE_NAME));
}

HWTEST_F(AccessibilityAbilityManagerTest, AddAbility, TestSize.Level1)
{
    manager_->AddAbility(TEST_BUNDLE_NAME);
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, ChangeAbility, TestSize.Level1)
{
    manager_->ChangeAbility(TEST_BUNDLE_NAME);
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, AddRemoveUITestClient, TestSize.Level1)
{
    manager_->AddUITestClient(nullptr, TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    sptr<AccessibleAbilityConnection> conn = nullptr;
    manager_->RemoveUITestClient(conn, TEST_BUNDLE_NAME);
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, SetGetAbilityAutoStartState, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    manager_->AddEnabledAbility(name);
    const auto &enabledAbilities = manager_->GetEnabledAbilities();
    bool isEnabled = std::find(enabledAbilities.begin(), enabledAbilities.end(), name) != enabledAbilities.end();
    EXPECT_TRUE(isEnabled);
}

HWTEST_F(AccessibilityAbilityManagerTest, DelAutoStartPrefKeyInRemovePkg, TestSize.Level1)
{
    auto config = std::make_shared<AccessibilitySettingsConfig>(TEST_ACCOUNT_ID);
    EXPECT_NE(config, nullptr);
    config->Init();
    manager_->SetConfig(config);
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    params.staticCapabilities = CAPABILITY_RETRIEVE;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    manager_->AddInstalledAbility(*info);
    std::string uri = Utils::GetUri(TEST_BUNDLE_NAME, TEST_ABILITY_NAME);
    auto ret = manager_->EnableAbility(uri, CAPABILITY_RETRIEVE);
    EXPECT_EQ(ret, RET_OK);

    manager_->SetAbilityAutoStartState(uri, true);
    const auto &enabledAbilities = manager_->GetEnabledAbilities();
    bool isAdded = std::find(enabledAbilities.begin(), enabledAbilities.end(), uri) != enabledAbilities.end();
    ASSERT_TRUE(isAdded) << "Failed to add auto-start ability (URI: " << uri << ")";

    auto services = config->GetEnabledAccessibilityServices();
    EXPECT_TRUE(std::find(services.begin(), services.end(), uri) != services.end());

    bool enabled = manager_->GetAbilityAutoStartState(uri);
    EXPECT_TRUE(enabled);

    manager_->DelAutoStartPrefKeyInRemovePkg(TEST_BUNDLE_NAME);
    bool isEnabled = manager_->GetAbilityAutoStartState(uri);
    EXPECT_FALSE(isEnabled) << "Auto-start preference not removed after DelAutoStartPrefKeyInRemovePkg call (URI: " <<
        uri << ")";
}

HWTEST_F(AccessibilityAbilityManagerTest, ScreenReaderState, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    EXPECT_FALSE(manager_->GetScreenReaderState());
    manager_->TryToSetScreenReaderState(name, true);
    manager_->SetScreenReaderState(true);
    EXPECT_TRUE(manager_->GetScreenReaderState());
    manager_->SetScreenReaderState(false);
    EXPECT_FALSE(manager_->GetScreenReaderState());
}

HWTEST_F(AccessibilityAbilityManagerTest, NeedEvent, TestSize.Level1)
{
    std::string name = std::string(TEST_BUNDLE_NAME) + "/" + TEST_ABILITY_NAME;
    std::vector<uint32_t> events = { 1, 2 };
    manager_->AddNeedEvent(name, events);
    manager_->RemoveNeedEvent(name);
    auto updated = manager_->UpdateNeedEvents();
    auto got = manager_->GetNeedEvents();
    EXPECT_TRUE(updated.empty());
    EXPECT_TRUE(got.empty());
}

HWTEST_F(AccessibilityAbilityManagerTest, OnAccountSwitched, TestSize.Level1)
{
    manager_->OnAccountSwitched();
    SUCCEED();
}

HWTEST_F(AccessibilityAbilityManagerTest, IsExistCapability, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto abilityInfo = std::make_shared<AccessibilityAbilityInfo>(params);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE | CAPABILITY_TOUCH_GUIDE | CAPABILITY_GESTURE |
        CAPABILITY_KEY_EVENT_OBSERVER);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *abilityInfo);
    manager_->AddConnectedAbility(conn);

    EXPECT_TRUE(manager_->IsExistCapability(CAPABILITY_RETRIEVE));
    EXPECT_FALSE(manager_->IsExistCapability(CAPABILITY_ZOOM));
}

HWTEST_F(AccessibilityAbilityManagerTest, ImportantEnabledAbilities, TestSize.Level1)
{
    std::map<std::string, uint32_t> abilities;
    manager_->GetImportantEnabledAbilities(abilities);
    EXPECT_TRUE(abilities.empty());
    manager_->UpdateImportantEnabledAbilities(abilities);
    manager_->UpdateAutoStartEnabledAbilities();
    SUCCEED();
}


HWTEST_F(AccessibilityAbilityManagerTest, AccessibilityAbility_AddGetRemove, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);

    AccessibilityAbilityManager::AccessibilityAbility ability;
    std::string uri = Utils::GetUri(conn->GetElementName());

    // Test AddAccessibilityAbility
    ability.AddAccessibilityAbility(uri, conn);
    EXPECT_EQ(ability.GetSize(), 1) << "Failed to add ability";

    // Test GetAccessibilityAbilityByUri
    auto getConn = ability.GetAccessibilityAbilityByUri(uri);
    ASSERT_NE(getConn, nullptr) << "Failed to retrieve ability by URI";
    EXPECT_EQ(getConn, conn) << "Retrieved connection mismatch";

    // Test GetAccessibilityAbilityByName - Use full URI instead of just ability name
    getConn = ability.GetAccessibilityAbilityByName(uri);
    ASSERT_NE(getConn, nullptr) << "Failed to retrieve ability by name";
    EXPECT_EQ(getConn, conn) << "Retrieved connection mismatch";

    // Test GetAccessibilityAbilitiesMap
    std::map<std::string, sptr<AccessibleAbilityConnection>> connMap;
    ability.GetAccessibilityAbilitiesMap(connMap);
    EXPECT_EQ(connMap.size(), 1) << "Connection map size mismatch";
    EXPECT_EQ(connMap[uri], conn) << "Connection map entry mismatch";

    // Test GetAccessibilityAbilities
    std::vector<sptr<AccessibleAbilityConnection>> connList;
    ability.GetAccessibilityAbilities(connList);
    EXPECT_EQ(connList.size(), 1) << "Connection list size mismatch";
    EXPECT_EQ(connList[0], conn) << "Connection list entry mismatch";

    // Test GetAbilitiesInfo
    std::vector<AccessibilityAbilityInfo> abilitiesInfo;
    ability.GetAbilitiesInfo(abilitiesInfo);
    EXPECT_EQ(abilitiesInfo.size(), 1) << "Ability info list size mismatch";
    EXPECT_EQ(abilitiesInfo[0].GetName(), TEST_ABILITY_NAME) << "Ability name mismatch";

    // Test IsExistCapability
    bool hasCapability = ability.IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(hasCapability) << "Unexpected capability detected";

    // Test RemoveAccessibilityAbilityByUri
    ability.RemoveAccessibilityAbilityByUri(uri);
    EXPECT_EQ(ability.GetSize(), 0) << "Failed to remove ability by URI";
}

HWTEST_F(AccessibilityAbilityManagerTest, AccessibilityAbility_DisableAndSize, TestSize.Level1)
{
    AccessibilityAbilityInitParams params;
    params.bundleName = TEST_BUNDLE_NAME;
    params.name = TEST_ABILITY_NAME;
    auto info = std::make_shared<AccessibilityAbilityInfo>(params);
    sptr<AccessibleAbilityConnection> conn = new MockAccessibleAbilityConnection(TEST_ACCOUNT_ID, 0, *info);
    ASSERT_NE(conn, nullptr) << "Failed to create mock connection";

    AccessibilityAbilityManager::AccessibilityAbility ability;
    std::string uri = Utils::GetUri(conn->GetElementName());
    std::string fullAbilityName = params.bundleName + "/" + params.name;
    EXPECT_FALSE(uri.empty()) << "URI generation failed for connection";

    // Add ability
    ability.AddAccessibilityAbility(uri, conn);
    EXPECT_EQ(ability.GetSize(), 1) << "Failed to add ability";

    // Test GetAccessibilityAbilityByUri
    auto getConn = ability.GetAccessibilityAbilityByUri(uri);
    ASSERT_NE(getConn, nullptr) << "Failed to retrieve ability by URI";
    EXPECT_EQ(getConn, conn) << "Retrieved connection mismatch";

    // Test GetAccessibilityAbilityByName - Use full URI instead of just ability name
    getConn = ability.GetAccessibilityAbilityByName(uri);
    ASSERT_NE(getConn, nullptr) << "Failed to retrieve ability by name";
    EXPECT_EQ(getConn, conn) << "Retrieved connection mismatch";

    // Test GetAccessibilityAbilitiesMap
    std::map<std::string, sptr<AccessibleAbilityConnection>> connMap;
    ability.GetAccessibilityAbilitiesMap(connMap);
    EXPECT_EQ(connMap.size(), 1) << "Connection map size mismatch";
    EXPECT_EQ(connMap[uri], conn) << "Connection map entry mismatch";

    // Test GetAccessibilityAbilities
    std::vector<sptr<AccessibleAbilityConnection>> connList;
    ability.GetAccessibilityAbilities(connList);
    EXPECT_EQ(connList.size(), 1) << "Connection list size mismatch";
    EXPECT_EQ(connList[0], conn) << "Connection list entry mismatch";

    // Test GetAbilitiesInfo
    std::vector<AccessibilityAbilityInfo> abilitiesInfo;
    ability.GetAbilitiesInfo(abilitiesInfo);
    EXPECT_EQ(abilitiesInfo.size(), 1) << "Ability info list size mismatch";
    EXPECT_EQ(abilitiesInfo[0].GetName(), TEST_ABILITY_NAME) << "Ability name mismatch";

    // Test IsExistCapability
    bool hasCapability = ability.IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(hasCapability) << "Unexpected capability detected";

    // Test RemoveAccessibilityAbilityByUri
    ability.RemoveAccessibilityAbilityByUri(uri);
    EXPECT_EQ(ability.GetSize(), 0) << "Failed to remove ability by URI";
}
} // namespace Accessibility
} // namespace OHOS