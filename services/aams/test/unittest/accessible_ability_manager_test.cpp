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
#include "accessibility_ability_info.h"
#include "accessibility_constants.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager.h"
#include "utils.h"

using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TEST_BUNDLE_NAME = "testBundle";
    const std::string TEST_ABILITY_NAME = "testAbility";
    const std::string TEST_URI = "testBundle/testAbility";
}

class AccessibleAbilityManagerTest : public testing::Test {
public:
    AccessibleAbilityManagerTest() {}
    ~AccessibleAbilityManagerTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<AccessibleAbilityManager> manager_ = nullptr;
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
}

void AccessibleAbilityManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    manager_.reset();
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_Destructor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Destructor_001 start";
    auto manager = std::make_unique<AccessibleAbilityManager>();
    EXPECT_NE(manager, nullptr);
    manager.reset();
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Destructor_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_Clear_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Clear_001 start";
    manager_->AddEnabledAbility(TEST_URI);
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 1);
    manager_->Clear();
    EXPECT_EQ(manager_->GetEnabledAbilities().size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_Clear_001 end";
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
    std::vector<AccessibilityAbilityInfo> abilities;
    manager_->GetAbilitiesByState(ABILITY_STATE_DISABLE, abilities);
    EXPECT_EQ(abilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetAbilitiesByState_002 end";
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

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetImportantEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_001 start";
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->GetImportantEnabledAbilities(importantEnabledAbilities);
    EXPECT_EQ(importantEnabledAbilities.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetImportantEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_UpdateImportantEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_001 start";
    std::map<std::string, uint32_t> importantEnabledAbilities;
    manager_->UpdateImportantEnabledAbilities(importantEnabledAbilities);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_UpdateImportantEnabledAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_IsExistCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_001 start";
    bool result = manager_->IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_IsExistCapability_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetSizeByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_001 start";
    int32_t result = manager_->GetSizeByUri("nonexistent");
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetSizeByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_GetReadableRules_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_001 start";
    std::string readableRules;
    RetError result = manager_->GetReadableRules(readableRules);
    EXPECT_EQ(result, RET_ERR_NOT_INSTALLED);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_GetReadableRules_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AccessibleAbilityManager_NotifyExtensionServiceDeath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_NotifyExtensionServiceDeath_001 start";
    manager_->NotifyExtensionServiceDeath(TEST_URI);
    GTEST_LOG_(INFO) << "AccessibleAbilityManager_NotifyExtensionServiceDeath_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetSize_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetSize_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_Clear_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_Clear_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    ability.Clear();
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_Clear_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetAbilityByName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilityByName_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    auto result = ability.GetAccessibilityAbilityByName("nonexistent");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilityByName_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetAbilityByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilityByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    auto result = ability.GetAccessibilityAbilityByUri("nonexistent");
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilityByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilities_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    std::vector<sptr<AccessibleAbilityConnection>> connectionList;
    ability.GetAccessibilityAbilities(connectionList);
    EXPECT_EQ(connectionList.size(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetAbilitiesInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilitiesInfo_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    std::vector<AccessibilityAbilityInfo> abilities;
    ability.GetAbilitiesInfo(abilities);
    EXPECT_EQ(abilities.size(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilitiesInfo_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_IsExistCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_IsExistCapability_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    bool result = ability.IsExistCapability(CAPABILITY_RETRIEVE);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_IsExistCapability_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetAbilitiesMap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilitiesMap_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap;
    ability.GetAccessibilityAbilitiesMap(connectionMap);
    EXPECT_EQ(connectionMap.size(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetAbilitiesMap_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetDisableAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetDisableAbilities_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    std::vector<AccessibilityAbilityInfo> disabledAbilities;
    ability.GetDisableAbilities(disabledAbilities);
    EXPECT_EQ(disabledAbilities.size(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetDisableAbilities_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_GetSizeByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetSizeByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    int32_t result = ability.GetSizeByUri("nonexistent");
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_GetSizeByUri_001 end";
}

HWTEST_F(AccessibleAbilityManagerTest, AAM_AccessibilityAbility_RemoveByUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_RemoveByUri_001 start";
    AccessibleAbilityManager::AccessibilityAbility ability;
    ability.RemoveAccessibilityAbilityByUri("nonexistent");
    EXPECT_EQ(ability.GetSize(), 0);
    GTEST_LOG_(INFO) << "AAM_AccessibilityAbility_RemoveByUri_001 end";
}
} // namespace Accessibility
} // namespace OHOS