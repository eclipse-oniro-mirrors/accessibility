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
#include "accessibility_ui_test_ability_impl.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"
#include "mock_accessible_ability_listener.h"
#include "accessible_ability_client_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TEST = "test";
    constexpr int32_t FOCUS_TYPE = 1;
} // namespace

class AccessibilityUITestAbilityImplTest : public ::testing::Test {
public:
    AccessibilityUITestAbilityImplTest()
    {}
    ~AccessibilityUITestAbilityImplTest()
    {}

    std::shared_ptr<AccessibilityUITestAbilityImpl> instance_ = nullptr;
    static void SetUpTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest Start";
    }
    static void TearDownTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest End";
    }
    void SetUp()
    {
        std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest SetUp()";
        instance_ = std::make_shared<AccessibilityUITestAbilityImpl>();
    }
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest TearDown()";
        instance_ = nullptr;
    }
};

/**
 * @tc.number: GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInstance_001 start";
    EXPECT_TRUE(AccessibilityUITestAbility::GetInstance());
    GTEST_LOG_(INFO) << "GetInstance_001 end";
}

/**
 * @tc.number: RegisterAbilityListener_001
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, RegisterAbilityListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    EXPECT_FALSE(instance_->RegisterAbilityListener(listener));

    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 end";
}

/**
 * @tc.number: Connect_001
 * @tc.name: Connect
 * @tc.desc: Test function Connect
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, Connect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Connect_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    EXPECT_EQ(instance_->Connect(), RET_OK);

    GTEST_LOG_(INFO) << "Connect_001 end";
}

/**
 * @tc.number: Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Disconnect_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    EXPECT_TRUE(instance_->Disconnect());

    GTEST_LOG_(INFO) << "Disconnect_001 end";
}

/**
 * @tc.number: GetFocus_001
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetFocus(FOCUS_TYPE, elementInfo));

    GTEST_LOG_(INFO) << "GetFocus_001 end";
}

/**
 * @tc.number: GetFocusByElementInfo_001
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocusByElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE, elementInfo));

    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 end";
}

/**
 * @tc.number: InjectGesture_001
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, InjectGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_FALSE(instance_->InjectGesture(gesturePath));

    GTEST_LOG_(INFO) << "InjectGesture_001 end";
}

/**
 * @tc.number: GetRoot_001
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRoot_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetRoot(elementInfo));

    GTEST_LOG_(INFO) << "GetRoot_001 end";
}

/**
 * @tc.number: GetRootByWindow_001
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRootByWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetRootByWindow(windowInfo, elementInfo));

    GTEST_LOG_(INFO) << "GetRootByWindow_001 end";
}

/**
 * @tc.number: GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::vector<AccessibilityWindowInfo> res {};
    instance_->GetWindows(res);
    EXPECT_EQ(0, res.size());
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: ExecuteCommonAction_001
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, ExecuteCommonAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteCommonAction_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    GlobalAction action = GLOBAL_ACTION_INVALID;
    EXPECT_FALSE(instance_->ExecuteCommonAction(action));

    GTEST_LOG_(INFO) << "ExecuteCommonAction_001 end";
}

/**
 * @tc.number: GetNext_001
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo nextElementInfo {};
    AccessibilityElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_FALSE(instance_->GetNext(elementInfo, direction, elementInfo));

    GTEST_LOG_(INFO) << "GetNext_001 end";
}

/**
 * @tc.number: GetByContent_001
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetByContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo parent {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    EXPECT_FALSE(instance_->GetByContent(parent, TEST, elementInfos));

    GTEST_LOG_(INFO) << "GetByContent_001 end";
}

/**
 * @tc.number: GetAnchor_001
 * @tc.name: GetAnchor
 * @tc.desc: Test function GetAnchor
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetAnchor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnchor_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetAnchor(windowInfo, elementInfo));

    GTEST_LOG_(INFO) << "GetAnchor_001 end";
}

/**
 * @tc.number: GetSource_001
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_FALSE(instance_->GetSource(eventInfo, elementInfo));

    GTEST_LOG_(INFO) << "GetSource_001 end";
}

/**
 * @tc.number: GetParentElementInfo_001
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetParentElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo child {};
    AccessibilityElementInfo parent {};
    EXPECT_FALSE(instance_->GetParentElementInfo(child, parent));

    GTEST_LOG_(INFO) << "GetParentElementInfo_001 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_FALSE(instance_->ExecuteAction(elementInfo, action, actionArguments));

    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}
} // namespace Accessibility
} // namespace OHOS