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

#include "accessibility_circle_drawing_manager.h"
#include <gtest/gtest.h>
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessible_ability_manager_service_config_observer_proxy.h"
#include "mock_accessible_ability_manager_service_config_observer_stub.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "mock_accessibility_setting_provider.h"
#include "utils.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
    constexpr size_t IMPORTANT_ABILITIES_SIZE = 0;
} // namespace

class AccessibilityCircleDrawingManagerTest : public testing::Test {
public:
    AccessibilityCircleDrawingManagerTest()
    {}
    ~AccessibilityCircleDrawingManagerTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilityCircleDrawingManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManagerTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
}

void AccessibilityCircleDrawingManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManagerTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityCircleDrawingManagerTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManagerTest SetUp";
}

void AccessibilityCircleDrawingManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManagerTest TearDown";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow001
 * @tc.name: CreatePointerWindow
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow001 start";
    int32_t accountId = 1;
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    
    /* Initial state */
    accountData->Init();
    accountData->GetConfig()->SetTouchGuideState(true);
    uint32_t stateType = accountData->CreatePointerWindow();
    uint32_t stateTypeTest = Accessibility::STATE_EXPLORATION_ENABLED;
    EXPECT_EQ(stateTypeTest, stateType);
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow002
 * @tc.name: CreatePointerWindow
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_002 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);

    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    
    /* add connected ability */
    accountData->DrawingProgressByOpenSource(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    
    // init
    accountData->Init();
    uint32_t stateType = accountData->CreatePointerWindow();
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED);
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_002 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow003
 * @tc.name: CreatePointerWindow
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_003 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    const std::string bundleName = "abc";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    accountData->Init();
    accountData->GetConfig()->SetKeyEventObserverState(true);
    uint32_t stateType = accountData->CreatePointerWindow();
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_KEYEVENT_ENABLED);
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_003 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow004
 * @tc.name: CreatePointerWindow
 * @tc.desc: Check the get accessibility state.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_004 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    accountData->DrawingProgressByOpenSource(connection);
    accountData->Init();
    accountData->GetConfig()->SetGestureState(true);
    uint32_t stateType = accountData->CreatePointerWindow();
    EXPECT_EQ(stateType, Accessibility::STATE_ACCESSIBILITY_ENABLED | Accessibility::STATE_GESTURE_ENABLED);

    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_CreatePointerWindow_004 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource001
 * @tc.name: DrawingProgressByOpenSource
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource_001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->DrawingProgressByOpenSource(connection);
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());

    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource_001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource002
 * @tc.name: DrawingProgressByOpenSource
 * @tc.desc: Check the add connected ability.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource_002 start";
    const int32_t accountId = 1;
    sptr<AccessibleAbilityConnection> connection = nullptr;
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    accountData->DrawingProgressByOpenSource(connection);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByOpenSource_002 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_DrawingProgressByRosenDrawing001
 * @tc.name: DrawingProgressByRosenDrawing
 * @tc.desc: Check the remove connected ability.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, 
        AccessibilityCircleDrawingManager_Unittest_DrawingProgressByRosenDrawing001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByRosenDrawing001 start";
    const int32_t accountId = 1;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection = new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    accountData->DrawingProgressByOpenSource(connection);
    EXPECT_EQ(1, (int)accountData->GetConnectedA11yAbilities().size());
    const std::string elementName = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementName));
    accountData->DrawingProgressByRosenDrawing(connection->GetElementName());
    EXPECT_EQ(0, (int)accountData->GetConnectedA11yAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgressByRosenDrawing001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_DrawingProgress001
 * @tc.name: DrawingProgress
 * @tc.desc: Check DrawingProgress.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, 
        AccessibilityCircleDrawingManager_Unittest_DrawingProgress001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgress001 start";
    const int32_t accountId = 0;
    const std::string bundleName = "abc";
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    accountData->DrawingProgress(bundleName);
    EXPECT_EQ(0, (int)accountData->GetInstalledAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawingProgress001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_SetPointerLocation001
 * @tc.name: SetPointerLocation
 * @tc.desc: Check SetPointerLocation.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest,
        AccessibilityCircleDrawingManager_Unittest_SetPointerLocation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_SetPointerLocation002 start";
    const int32_t accountId = 1;
    const std::string name = "aaa";
    const std::string bundleName = "abc";
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    AccessibilityAbilityInitParams initParams;
    initParams.name = name;
    initParams.bundleName = bundleName;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->GetStagingProperties(*abilityInfo);
    accountData->SetPointerLocation(bundleName);
    std::string abilityId = bundleName + "/" + name;
    bool ret = accountData->GetAbilityAutoStartState(abilityId);
    EXPECT_EQ(false, ret);
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_SetPointerLocation002 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_DrawPointer001
 * @tc.name: DrawPointer
 * @tc.desc: Check the DrawPointer.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_DrawPointer001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawPointer001 start";
    const int32_t accountId = 2;
    int32_t connectCounter = 0;
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, connectCounter++, *abilityInfo);
    EXPECT_EQ(0, (int)accountData->UpdatePointerVisible().size());
    /* add */
    accountData->DrawingProgressByOpenSource(connection);
    const std::string elementNameURI = Utils::GetUri(connection->GetElementName());
    EXPECT_EQ(1, (int)accountData->SetPointerLocation().size());
    EXPECT_EQ(connection, accountData->GetAccessibleAbilityConnection(elementNameURI));
    EXPECT_FALSE(accountData->CreatePointerWindow(elementNameURI));
    /* disconnect */
    accountData->DrawingProgress(connection->GetElementName());
    EXPECT_EQ(0, (int)accountData->DrawPointer().size());
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_DrawPointer001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_ParseStringToInteger001
 * @tc.name: ParseStringToInteger
 * @tc.desc: Check the remove connecting A11y ability.GetAccessibilityWindows
 */
HWTEST_F(
    AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_ParseStringToInteger001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_ParseStringToInteger001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    const std::string bundleName = "abc";
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(accountId, 0, *abilityInfo);
    accountData->AddConnectingA11yAbility(bundleName, connection);
    EXPECT_TRUE(accountData->GetConnectingA11yAbility(bundleName));
    accountData->ParseStringToInteger(bundleName);
    EXPECT_FALSE(accountData->GetConnectingA11yAbility(bundleName));
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_ParseStringToInteger001 end";
}

/**
 * @tc.number: AccessibilityCircleDrawingManager_Unittest_ParseColorString001
 * @tc.name: ParseColorString
 * @tc.desc: Check the add enable ability.
 */
HWTEST_F(AccessibilityCircleDrawingManagerTest, AccessibilityCircleDrawingManager_Unittest_ParseColorString001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_ParseColorString001 start";
    const int32_t accountId = 1;
    sptr<AccessibilityCircleDrawingManager> accountData = new AccessibilityCircleDrawingManager(accountId);
    const std::string bundleName = "/ccc";
    accountData->Init();
    AccessibilityAbilityInitParams initParams;
    initParams.name = "ccc";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(1);
    accountData->AddInstalledAbility(*abilityInfo);
    accountData->ParseColorString(bundleName);
    accountData->ParseColorString(bundleName);
    EXPECT_EQ(1, (int)accountData->GetEnabledAbilities().size());
    GTEST_LOG_(INFO) << "AccessibilityCircleDrawingManager_Unittest_ParseColorString001 end";
}