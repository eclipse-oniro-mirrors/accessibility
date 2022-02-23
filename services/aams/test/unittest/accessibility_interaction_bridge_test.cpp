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

#include "accessibility_display_manager.h"
#include "accessibility_interaction_bridge.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_window_manager.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
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
    sptr<IRemoteObject> bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();
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
 * @tc.number: AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_001
 * @tc.name: GetPointerItermOfAccessibilityFocusClick
 * @tc.desc: Test function GetPointerItermOfAccessibilityFocusClick 1 focus in window and display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetPointerItermOfAccessibilityFocusClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_001 start";
    // regist InteractionConnection
    sptr<AccessibilityElementOperatorStub> aamsInteractionOperator = new AccessibilityElementOperatorStub();
    ins_->RegisterElementOperator(0, aamsInteractionOperator, 0);
    // Set window info
    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    // auto AccessibilityWindowInfoManager::GetInstance() = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    int windowId = 0;
    AccessibilityWindowInfo info = AccessibilityWindowInfoManager::GetInstance().CreateAccessibilityWindowInfo(*winInfo);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.insert(std::make_pair(windowId, info));

    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = 0;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    GTEST_LOG_(INFO) << "2 start";
    Rect rect(0, 100, 800, 900);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);
    GTEST_LOG_(INFO) << "3 start";
    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MMI::PointerEvent::PointerItem point;
    auto ret = interactionBridge.GetPointerItermOfAccessibilityFocusClick(point);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetGlobalX(), 300);
    EXPECT_EQ(point.GetGlobalY(), 500);

    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);

    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_002 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_002
 * @tc.name: GetPointerItermOfAccessibilityFocusClick
 * @tc.desc: Test function GetPointerItermOfAccessibilityFocusClick 2 focus intersect with window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetPointerItermOfAccessibilityFocusClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_002 start";
    // Set window info
    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    int windowId = 0;
    AccessibilityWindowInfo info = AccessibilityWindowInfoManager::GetInstance().CreateAccessibilityWindowInfo(*winInfo);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.insert(std::make_pair(windowId, info));
    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = 0;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect rect(0, 100, 800, 400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MMI::PointerEvent::PointerItem point;
    auto ret = interactionBridge.GetPointerItermOfAccessibilityFocusClick(point);

    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetGlobalX(), 300);
    EXPECT_EQ(point.GetGlobalY(), 300);
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_002 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_003
 * @tc.name: GetPointerItermOfAccessibilityFocusClick
 * @tc.desc: Test function GetPointerItermOfAccessibilityFocusClick 3 focus intersect with window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetPointerItermOfAccessibilityFocusClick_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_003 start";
    // Set window info
    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    int windowId = 0;
    AccessibilityWindowInfo info = AccessibilityWindowInfoManager::GetInstance().CreateAccessibilityWindowInfo(*winInfo);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.insert(std::make_pair(windowId, info));
    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = 0;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect rect(0, 100, 800, 400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MMI::PointerEvent::PointerItem point;
    auto ret = interactionBridge.GetPointerItermOfAccessibilityFocusClick(point);

    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.GetGlobalX(), 300);
    EXPECT_EQ(point.GetGlobalY(), 300);
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    AccessibilityWindowInfoManager::GetInstance().a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_003 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_004
 * @tc.name: GetPointerItermOfAccessibilityFocusClick
 * @tc.desc: Test function GetPointerItermOfAccessibilityFocusClick 4 focus in window and outside display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetPointerItermOfAccessibilityFocusClick_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_004 start";
    // Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = 0;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect rect(0, 100, 800, 400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MMI::PointerEvent::PointerItem point;
    auto ret = interactionBridge.GetPointerItermOfAccessibilityFocusClick(point);

    EXPECT_EQ(ret, true);
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_004 end";
}

/**
 * @tc.number: AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_004
 * @tc.name: GetPointerItermOfAccessibilityFocusClick
 * @tc.desc: Test function GetPointerItermOfAccessibilityFocusClick 4 focus outside window and in display
 */
HWTEST_F(AccessibilityInteractionBridgeTest, GetPointerItermOfAccessibilityFocusClick_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_005 start";
    // Set window info
    AccessibilityWindowInfoManager::GetInstance().activeWindowId_ = 0;
    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = 0;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect rect(0, 0, -800, -400);
    AccessibilityWindowInfoManager::GetInstance().SetWindowSize(0, rect);

    auto interactionBridge = AccessibilityInteractionBridge::GetInstance();
    MMI::PointerEvent::PointerItem point;
    interactionBridge.GetPointerItermOfAccessibilityFocusClick(point);

    AccessibilityWindowInfoManager::GetInstance().windowListener_->OnWindowUpdate(
        winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    GTEST_LOG_(INFO) << "AccessibilityInteractionBridgeTest_GetPointerItermOfAccessibilityFocusClick_005 end";
}
}  // namespace Accessibility
}  // namespace OHOS
