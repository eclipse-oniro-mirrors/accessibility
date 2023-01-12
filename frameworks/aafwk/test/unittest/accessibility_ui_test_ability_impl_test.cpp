/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include "accessibility_ability_ut_helper.h"
#include "accessibility_ui_test_ability_impl.h"
#include "mock_accessible_ability_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TEST = "test";
    constexpr int32_t FOCUS_TYPE = 1;
    constexpr int32_t WINDOW_ID = 1;
    constexpr uint64_t DISPLAY_ID = 1;
    constexpr int32_t CACHE_MODE = 2;
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
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityUITestAbilityImplTest End";
    }
    void SetUp()
    {
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
 * @tc.desc: Test function RegisterAbilityListener(listener is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, RegisterAbilityListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_ERR_INVALID_PARAM);

    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 end";
}

/**
 * @tc.number: RegisterAbilityListener_002
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, RegisterAbilityListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "RegisterAbilityListener_002 end";
}

/**
 * @tc.number: RegisterAbilityListener_003
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, RegisterAbilityListener_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_003 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_OK);

    GTEST_LOG_(INFO) << "RegisterAbilityListener_003 end";
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
    EXPECT_EQ(RET_OK, instance_->Connect());

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
    EXPECT_EQ(instance_->Disconnect(), RET_OK);

    GTEST_LOG_(INFO) << "Disconnect_001 end";
}

/**
 * @tc.number: GetFocus_001
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetFocus_001 end";
}

/**
 * @tc.number: GetFocus_002
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocus_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetFocus_002 end";
}

/**
 * @tc.number: GetFocusByElementInfo_001
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocusByElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 end";
}

/**
 * @tc.number: GetFocusByElementInfo_002
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetFocusByElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 end";
}

/**
 * @tc.number: InjectGesture_001
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, InjectGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "InjectGesture_001 end";
}

/**
 * @tc.number: InjectGesture_002
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, InjectGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_OK);

    GTEST_LOG_(INFO) << "InjectGesture_002 end";
}

/**
 * @tc.number: GetRoot_001
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRoot_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRoot(elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRoot_001 end";
}

/**
 * @tc.number: GetRoot_002
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRoot_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRoot(elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetRoot_002 end";
}

/**
 * @tc.number: GetRootByWindow_001
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRootByWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRootByWindow_001 end";
}

/**
 * @tc.number: GetRootByWindow_002
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetRootByWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetRootByWindow_002 end";
}

/**
 * @tc.number: GetWindow_001
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityWindowInfo winInfo {};
    EXPECT_EQ(instance_->GetWindow(WINDOW_ID, winInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindow_001 end";
}

/**
 * @tc.number: GetWindow_002
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityWindowInfo winInfo {};
    EXPECT_EQ(instance_->GetWindow(WINDOW_ID, winInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindow_002 end";
}

/**
 * @tc.number: GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(res), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(res), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_002 end";
}

/**
 * @tc.number: GetWindows_003
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindows_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_003 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(DISPLAY_ID, res), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindows_003 end";
}

/**
 * @tc.number: GetWindows_004
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetWindows_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_004 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(DISPLAY_ID, res), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_004 end";
}

/**
 * @tc.number: GetNext_001
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo nextElementInfo {};
    AccessibilityElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetNext(elementInfo, direction, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetNext_001 end";
}

/**
 * @tc.number: GetNext_002
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetNext_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo nextElementInfo {};
    AccessibilityElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetNext(elementInfo, direction, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetNext_002 end";
}

/**
 * @tc.number: GetChildElementInfo_001
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetChildElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->GetChildElementInfo(index, parent, child), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetChildElementInfo_001 end";
}

/**
 * @tc.number: GetChildElementInfo_002
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetChildElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->GetChildElementInfo(index, parent, child), RET_OK);

    GTEST_LOG_(INFO) << "GetChildElementInfo_002 end";
}

/**
 * @tc.number: GetByContent_001
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetByContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo parent {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetByContent(parent, TEST, elementInfos), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetByContent_001 end";
}

/**
 * @tc.number: GetByContent_002
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetByContent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo parent {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetByContent(parent, TEST, elementInfos), RET_OK);

    GTEST_LOG_(INFO) << "GetByContent_002 end";
}

/**
 * @tc.number: GetSource_001
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetSource_001 end";
}

/**
 * @tc.number: GetSource_002
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetSource_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetSource_002 end";
}

/**
 * @tc.number: GetParentElementInfo_001
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetParentElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo child {};
    AccessibilityElementInfo parent {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetParentElementInfo_001 end";
}

/**
 * @tc.number: GetParentElementInfo_002
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetParentElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo child {};
    AccessibilityElementInfo parent {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_OK);

    GTEST_LOG_(INFO) << "GetParentElementInfo_002 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_OK);

    GTEST_LOG_(INFO) << "ExecuteAction_002 end";
}

/**
 * @tc.number: SetTargetBundleName_001
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName(AccessibleAbilityClient is null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, SetTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "SetTargetBundleName_001 end";
}

/**
 * @tc.number: SetTargetBundleName_002
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName(AccessibleAbilityClient is not null)
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, SetTargetBundleName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_OK);

    GTEST_LOG_(INFO) << "SetTargetBundleName_002 end";
}

/**
 * @tc.number: GetChildren_001
 * @tc.name: GetChildren
 * @tc.desc: Test function GetChildren
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, GetChildren_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildren_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    AccessibilityElementInfo parent;
    std::vector<AccessibilityElementInfo> children;
    EXPECT_EQ(instance_->GetChildren(parent, children), RET_OK);

    GTEST_LOG_(INFO) << "GetChildren_001 end";
}

/**
 * @tc.number: SetCacheMode_001
 * @tc.name: SetCacheMode
 * @tc.desc: Test function SetCacheMode
 */
HWTEST_F(AccessibilityUITestAbilityImplTest, SetCacheMode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCacheMode_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityUITestAbilityImpl instance_";
        return;
    }
    instance_->SetCacheMode(CACHE_MODE);
    EXPECT_EQ(CACHE_MODE, AccessibilityAbilityUtHelper::GetInstance().GetCacheMode());

    GTEST_LOG_(INFO) << "SetCacheMode_001 end";
}
} // namespace Accessibility
} // namespace OHOS