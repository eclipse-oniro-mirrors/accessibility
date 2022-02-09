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
#include "accessibility_interaction_bridge.h"
#include "accessibility_display_manager.h"
#include "accessibility_window_manager.h"
#include "accessibility_interaction_operation_stub.h"
#include "mock_bundle_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

namespace OHOS {
namespace Accessibility {

class AccessibilityInteractionBridgeTest : public testing::Test {
public:

    AccessibilityInteractionBridgeTest() {}
    ~AccessibilityInteractionBridgeTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_ = nullptr;
    //static AccessibilityInteractionBridge interactionBridge;
};

void AccessibilityInteractionBridgeTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest SetUpTestCase";
}

void AccessibilityInteractionBridgeTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest TearDownTestCase";
}

void AccessibilityInteractionBridgeTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest SetUp";

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    auto bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();
    //interactionBridge = AccessibilityInteractionBridge::GetInstance();
}

void AccessibilityInteractionBridgeTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest TearDown";
    ins_ = nullptr;
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibilityInteractionBridgeTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_Unittest_FindFocusedElementInfo_001 start";

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    auto focus = interactionBridge.FindFocusedElementInfo();
    auto rect = focus.GetRectInScreen();
    int posx = rect.GetLeftTopXScreenPostion();
    int posy = rect.GetLeftTopYScreenPostion();
    int dx = rect.GetRightBottomXScreenPostion();
    int dy = rect.GetRightBottomYScreenPostion();
    EXPECT_EQ(posx, 0);
    EXPECT_EQ(posy, 200);
    EXPECT_EQ(dx, 600);
    EXPECT_EQ(dy, 800);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_Unittest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_PerformActionOnAccessibilityFocusedItem_001
 * @tc.name: PerformActionOnAccessibilityFocusedItem
 * @tc.desc: Test function PerformActionOnAccessibilityFocusedItem
 */
HWTEST_F(AccessibilityInteractionBridgeTest, PerformActionOnAccessibilityFocusedItem_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_Unittest_PerformActionOnAccessibilityFocusedItem_001 start";

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();

    auto ret = interactionBridge.PerformActionOnAccessibilityFocusedItem(ActionType::ACCESSIBILITY_ACTION_FOCUS);

    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_Unittest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_001
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen 1 focus in window and display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetAccessibilityFocusClickPointInScreen_001, TestSize.Level1)
{
   GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_002 start";
    //regist InteractionConnection
    GTEST_LOG_(INFO) << "4 start";
    sptr<AccessibilityInteractionOperationStub> aamsInteractionOperator = new AccessibilityInteractionOperationStub();
    GTEST_LOG_(INFO) << "5 start";
    ins_->RegisterInteractionOperation(0,aamsInteractionOperator, 0);
    GTEST_LOG_(INFO) << "1 start";
    //Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    AccessibilityWindowInfoManager::GetInstance().OnWindowCreate(0);
    GTEST_LOG_(INFO) << "2 start";
    Rect rect(0,100,800,900);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    GTEST_LOG_(INFO) << "3 start";
    //Set display Info
    WMDisplayInfo displayInfo;
    displayInfo.width = 1000;
    displayInfo.height = 1000;
    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);
    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MmiPoint point;
    interactionBridge.GetAccessibilityFocusClickPointInScreen(point);
    auto ret = interactionBridge.GetAccessibilityFocusClickPointInScreen(point);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetX(), 300);
    EXPECT_EQ(point.GetY(), 500);

    AccessibilityWindowInfoManager::GetInstance().OnWindowDestroy(0);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_002 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_002
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen 2 focus intersect with window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest,GetAccessibilityFocusClickPointInScreen_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_002 start";
    //Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    AccessibilityWindowInfoManager::GetInstance().OnWindowCreate(0);
    Rect rect(0,100,800,400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    //Set display Info
    WMDisplayInfo displayInfo;
    displayInfo.width = 1000;
    displayInfo.height = 1000;
    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MmiPoint point;
    auto ret = interactionBridge.GetAccessibilityFocusClickPointInScreen(point);

    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetX(), 300);
    EXPECT_EQ(point.GetY(), 300);
    AccessibilityWindowInfoManager::GetInstance().OnWindowDestroy(0);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_002 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_003
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen 3 focus intersect with window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest,GetAccessibilityFocusClickPointInScreen_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_003 start";
    //Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    AccessibilityWindowInfoManager::GetInstance().OnWindowCreate(0);
    Rect rect(0,100,800,400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    //Set display Info
    WMDisplayInfo displayInfo;
    displayInfo.width = 1000;
    displayInfo.height = 300;
    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MmiPoint point;
    auto ret = interactionBridge.GetAccessibilityFocusClickPointInScreen(point);

    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetX(), 300);
    EXPECT_EQ(point.GetY(), 250);
    AccessibilityWindowInfoManager::GetInstance().OnWindowDestroy(0);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_003 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_004
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen 4 focus in window and outside display
 */
HWTEST_F(AccessibilityInteractionBridgeTest,GetAccessibilityFocusClickPointInScreen_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_004 start";
    //Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    AccessibilityWindowInfoManager::GetInstance().OnWindowCreate(0);
    Rect rect(0,100,800,400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    //Set display Info
    WMDisplayInfo displayInfo;
    displayInfo.width = -1000;
    displayInfo.height = -1000;
    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MmiPoint point;
    auto ret = interactionBridge.GetAccessibilityFocusClickPointInScreen(point);

    EXPECT_EQ(ret, false);
    AccessibilityWindowInfoManager::GetInstance().OnWindowDestroy(0);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_004 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_004
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen 4 focus outside window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest,GetAccessibilityFocusClickPointInScreen_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_005 start";
    //Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    AccessibilityWindowInfoManager::GetInstance().OnWindowCreate(0);
    Rect rect(0,0,-800,-400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    //Set display Info
    WMDisplayInfo displayInfo;
    displayInfo.width = 1000;
    displayInfo.height = 1000;
    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MmiPoint point;
    auto ret = interactionBridge.GetAccessibilityFocusClickPointInScreen(point);

    EXPECT_EQ(ret, false);
    AccessibilityWindowInfoManager::GetInstance().OnWindowDestroy(0);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetAccessibilityFocusClickPointInScreen_005 end";
}


}  // namespace Accessibility
}  // namespace OHOS
