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
#include "A11y_ability_ut_helper.h"
#include "A11y_ui_test_ability_impl.h"
#include "mock_accessible_ability_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace A11y {
namespace {
    const std::string TEST = "test";
    constexpr int32_t FOCUS_TYPE = 1;
    constexpr int32_t WINDOW_ID = 1;
    constexpr uint64_t DISPLAY_ID = 1;
    constexpr int32_t CACHE_MODE = 2;
} // namespace

class A11yUITestAbilityImplTest : public ::testing::Test {
public:
    A11yUITestAbilityImplTest()
    {}
    ~A11yUITestAbilityImplTest()
    {}

    std::shared_ptr<A11yUITestAbilityImpl> instance_ = nullptr;
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "A11yUITestAbilityImplTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "A11yUITestAbilityImplTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "A11yUITestAbilityImplTest SetUp()";
        instance_ = std::make_shared<A11yUITestAbilityImpl>();
    }
    void TearDown()
    {
        GTEST_LOG_(INFO) << "A11yUITestAbilityImplTest TearDown()";
        instance_ = nullptr;
    }
};

/**
 * @tc.number: GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(A11yUITestAbilityImplTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInstance_001 start";
    EXPECT_TRUE(A11yUITestAbility::GetInstance());
    GTEST_LOG_(INFO) << "GetInstance_001 end";
}

/**
 * @tc.number: RegisterAbilityListener_001
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener(listener is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, RegisterAbilityListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
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
HWTEST_F(A11yUITestAbilityImplTest, RegisterAbilityListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "RegisterAbilityListener_002 end";
}

/**
 * @tc.number: RegisterAbilityListener_003
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, RegisterAbilityListener_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_003 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
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
HWTEST_F(A11yUITestAbilityImplTest, Connect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Connect_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
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
HWTEST_F(A11yUITestAbilityImplTest, Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Disconnect_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
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
HWTEST_F(A11yUITestAbilityImplTest, GetFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetFocus_001 end";
}

/**
 * @tc.number: GetFocus_002
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetFocus_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetFocus_002 end";
}

/**
 * @tc.number: GetFocusByElementInfo_001
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetFocusByElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo sourceInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 end";
}

/**
 * @tc.number: GetFocusByElementInfo_002
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetFocusByElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo sourceInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 end";
}

/**
 * @tc.number: InjectGesture_001
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, InjectGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::shared_ptr<A11yGestureInjectPath> gesturePath = std::make_shared<A11yGestureInjectPath>();
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "InjectGesture_001 end";
}

/**
 * @tc.number: InjectGesture_002
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, InjectGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    std::shared_ptr<A11yGestureInjectPath> gesturePath = std::make_shared<A11yGestureInjectPath>();
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_OK);

    GTEST_LOG_(INFO) << "InjectGesture_002 end";
}

/**
 * @tc.number: GetRoot_001
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRoot_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRoot(elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRoot_001 end";
}

/**
 * @tc.number: GetRoot_002
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRoot_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRoot(elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetRoot_002 end";
}

/**
 * @tc.number: GetRootByWindow_001
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootByWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yWindowInfo windowInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRootByWindow_001 end";
}

/**
 * @tc.number: GetRootByWindow_002
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootByWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yWindowInfo windowInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetRootByWindow_002 end";
}

/**
 * @tc.number: GetWindow_001
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yWindowInfo winInfo {};
    EXPECT_EQ(instance_->GetWindow(WINDOW_ID, winInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindow_001 end";
}

/**
 * @tc.number: GetWindow_002
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yWindowInfo winInfo {};
    EXPECT_EQ(instance_->GetWindow(WINDOW_ID, winInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindow_002 end";
}

/**
 * @tc.number: GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<A11yWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(res), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    std::vector<A11yWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(res), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_002 end";
}

/**
 * @tc.number: GetWindows_003
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindows_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_003 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<A11yWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(DISPLAY_ID, res), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "GetWindows_003 end";
}

/**
 * @tc.number: GetWindows_004
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetWindows_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_004 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    std::vector<A11yWindowInfo> res {};
    EXPECT_EQ(instance_->GetWindows(DISPLAY_ID, res), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_004 end";
}

/**
 * @tc.number: GetNext_001
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo nextElementInfo {};
    A11yElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetNext(elementInfo, direction, elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetNext_001 end";
}

/**
 * @tc.number: GetNext_002
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetNext_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo nextElementInfo {};
    A11yElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetNext(elementInfo, direction, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetNext_002 end";
}

/**
 * @tc.number: GetChildElementInfo_001
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetChildElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo parent {};
    A11yElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->GetChildElementInfo(index, parent, child), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetChildElementInfo_001 end";
}

/**
 * @tc.number: GetChildElementInfo_002
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetChildElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo parent {};
    A11yElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->GetChildElementInfo(index, parent, child), RET_OK);

    GTEST_LOG_(INFO) << "GetChildElementInfo_002 end";
}

/**
 * @tc.number: GetByContent_001
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetByContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo parent {};
    std::vector<A11yElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetByContent(parent, TEST, elementInfos), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetByContent_001 end";
}

/**
 * @tc.number: GetByContent_002
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetByContent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo parent {};
    std::vector<A11yElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetByContent(parent, TEST, elementInfos), RET_OK);

    GTEST_LOG_(INFO) << "GetByContent_002 end";
}

/**
 * @tc.number: GetSource_001
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yEventInfo eventInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetSource_001 end";
}

/**
 * @tc.number: GetSource_002
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetSource_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yEventInfo eventInfo {};
    A11yElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetSource_002 end";
}

/**
 * @tc.number: GetParentElementInfo_001
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetParentElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo child {};
    A11yElementInfo parent {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetParentElementInfo_001 end";
}

/**
 * @tc.number: GetParentElementInfo_002
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetParentElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo child {};
    A11yElementInfo parent {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_OK);

    GTEST_LOG_(INFO) << "GetParentElementInfo_002 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yElementInfo elementInfo {};
    ActionType action = A11y_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo elementInfo {};
    ActionType action = A11y_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_OK);

    GTEST_LOG_(INFO) << "ExecuteAction_002 end";
}

/**
 * @tc.number: SetTargetBundleName_001
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, SetTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "SetTargetBundleName_001 end";
}

/**
 * @tc.number: SetTargetBundleName_002
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, SetTargetBundleName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
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
HWTEST_F(A11yUITestAbilityImplTest, GetChildren_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildren_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yElementInfo parent;
    std::vector<A11yElementInfo> children;
    EXPECT_EQ(instance_->GetChildren(parent, children), RET_OK);

    GTEST_LOG_(INFO) << "GetChildren_001 end";
}

/**
 * @tc.number: SetCacheMode_001
 * @tc.name: SetCacheMode
 * @tc.desc: Test function SetCacheMode
 */
HWTEST_F(A11yUITestAbilityImplTest, SetCacheMode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCacheMode_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    instance_->SetCacheMode(CACHE_MODE);
    EXPECT_EQ(CACHE_MODE, A11yAbilityUtHelper::GetInstance().GetCacheMode());

    GTEST_LOG_(INFO) << "SetCacheMode_001 end";
}

/**
 * @tc.number: GetRootBatch_001
 * @tc.name: GetRootBatch
 * @tc.desc: Test function GetRootBatch(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootBatch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootBatch_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    std::vector<A11yElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetRootBatch(elementInfos), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRootBatch_001 end";
}

/**
 * @tc.number: GetRootBatch_002
 * @tc.name: GetRootBatch
 * @tc.desc: Test function GetRootBatch(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootBatch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootBatch_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    std::vector<A11yElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetRootBatch(elementInfos), RET_OK);

    GTEST_LOG_(INFO) << "GetRootBatch_002 end";
}

/**
 * @tc.number: GetRootByWindowBatch_001
 * @tc.name: GetRootByWindowbatch
 * @tc.desc: Test function GetRootByWindow(AccessibleAbilityClient is null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootByWindowBatch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindowBatch_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(true);
    A11yWindowInfo windowInfo {};
    std::vector<A11yElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetRootByWindowBatch(windowInfo, elementInfos, true), RET_ERR_NULLPTR);
    A11yAbilityUtHelper::GetInstance().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetRootByWindowBatch_001 end";
}

/**
 * @tc.number: GetRootByWindowBatch_002
 * @tc.name: GetRootByWindowBatch
 * @tc.desc: Test function GetRootByWindowBatch(AccessibleAbilityClient is not null)
 */
HWTEST_F(A11yUITestAbilityImplTest, GetRootByWindowBatch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindowBatch_002 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get A11yUITestAbilityImpl instance_";
        return;
    }
    A11yWindowInfo windowInfo {};
    std::vector<A11yElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetRootByWindowBatch(windowInfo, elementInfos, true), RET_OK);

    GTEST_LOG_(INFO) << "GetRootByWindowBatch_002 end";
}
} // namespace A11y
} // namespace OHOS