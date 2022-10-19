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
#include "accessible_ability_channel_client.h"
#include "mock_accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TEST = "test";
    constexpr int32_t SEQUENCE = 1;
    constexpr int32_t ACCESSIBILITY_WINDOW_ID = 1;
    constexpr int32_t ELEMENT_ID = 1;
    constexpr int32_t MODE = 1;
    constexpr int32_t CHANNEL_ID = 1;
} // namespace

class AccessibleAbilityChannelClientTest : public ::testing::Test {
public:
    AccessibleAbilityChannelClientTest()
    {}
    ~AccessibleAbilityChannelClientTest()
    {}

    sptr<MockAccessibleAbilityChannelStub> stub_ = nullptr;
    sptr<IAccessibleAbilityChannel> channel_ = nullptr;
    std::shared_ptr<AccessibleAbilityChannelClient> instance_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityChannelClientTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityChannelClientTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityChannelClientTest SetUp()";
        stub_ = new MockAccessibleAbilityChannelStub();
        ASSERT_TRUE(stub_);
        channel_ = iface_cast<IAccessibleAbilityChannel>(stub_);
        ASSERT_TRUE(channel_);
        instance_ = std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, channel_);
        ASSERT_TRUE(instance_);
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityChannelClientTest TearDown()";
        stub_ = nullptr;
        channel_ = nullptr;
        instance_ = nullptr;
    }
};

/**
 * @tc.number: GetRemote_001
 * @tc.name: GetRemote
 * @tc.desc: Test function GetRemote
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetRemote_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRemote_001 start";
    EXPECT_TRUE(instance_->GetRemote());
    GTEST_LOG_(INFO) << "GetRemote_001 end";
}

/**
 * @tc.number: SetOnKeyPressEventResult_001
 * @tc.name: SetOnKeyPressEventResult
 * @tc.desc: Test function SetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SetOnKeyPressEventResult, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetOnKeyPressEventResult_001 start";

    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }

    instance_->SetOnKeyPressEventResult(true, SEQUENCE);

    GTEST_LOG_(INFO) << "SetOnKeyPressEventResult_001 end";
}

/**
 * @tc.number: FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 start";
    EXPECT_CALL(*stub_, FindFocusedElementInfo(_, _, _, _, _)).Times(1).WillOnce(Return(RET_ERR_FAILED));
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->FindFocusedElementInfo(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, FOCUS_TYPE_INPUT, info), RET_ERR_FAILED);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: FindFocusedElementInfo_002
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FindFocusedElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_002 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    AccessibilityElementInfo info {};
    EXPECT_EQ(client->FindFocusedElementInfo(
        ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, FOCUS_TYPE_INPUT, info), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_002 end";
}

/**
 * @tc.number: FindFocusedElementInfo_003
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FindFocusedElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_003 start";
    EXPECT_CALL(*stub_, FindFocusedElementInfo(_, _, _, _, _)).Times(1).WillOnce(Return(RET_OK));
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->FindFocusedElementInfo(
        ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, FOCUS_TYPE_INPUT, info), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_003 end";
}

/**
 * @tc.number: SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendSimulateGesture_001 start";
    EXPECT_CALL(*stub_, SendSimulateGesture(_)).Times(1).WillOnce(Return(RET_OK));
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->SendSimulateGesture(gesturePath), RET_OK);
    GTEST_LOG_(INFO) << "SendSimulateGesture_001 end";
}

/**
 * @tc.number: SendSimulateGesture_002
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SendSimulateGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendSimulateGesture_002 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(client->SendSimulateGesture(gesturePath), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "SendSimulateGesture_002 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelClientTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";
    EXPECT_CALL(*stub_, ExecuteAction(_, _, _, _, _, _)).Times(1).WillOnce(Return(RET_ERR_FAILED));
    std::map<std::string, std::string> actionArguments;
    EXPECT_EQ(instance_->ExecuteAction(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, ActionType::ACCESSIBILITY_ACTION_SELECT, actionArguments), RET_ERR_FAILED);
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelClientTest, ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_002 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::map<std::string, std::string> actionArguments;
    EXPECT_EQ(client->ExecuteAction(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, ActionType::ACCESSIBILITY_ACTION_SELECT, actionArguments), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "ExecuteAction_002 end";
}

/**
 * @tc.number: ExecuteAction_003
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelClientTest, ExecuteAction_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_003 start";
    EXPECT_CALL(*stub_, ExecuteAction(_, _, _, _, _, _)).Times(1).WillOnce(Return(RET_OK));
    std::map<std::string, std::string> actionArguments;
    EXPECT_EQ(instance_->ExecuteAction(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID,
        ActionType::ACCESSIBILITY_ACTION_SELECT, actionArguments), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "ExecuteAction_003 end";
}

/**
 * @tc.number: GetWindow_001
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_001 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(client->GetWindow(0, windowInfo), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "GetWindow_001 end";
}

/**
 * @tc.number: GetWindow_002
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_002 start";
    EXPECT_CALL(*stub_, GetWindow(_, _)).Times(1).WillOnce(Return(RET_OK));
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(instance_->GetWindow(0, windowInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindow_002 end";
}

/**
 * @tc.number: GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::vector<AccessibilityWindowInfo> windowInfo {};
    EXPECT_EQ(client->GetWindows(windowInfo), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_002 start";
    EXPECT_CALL(*stub_, GetWindows(_)).Times(1).WillOnce(Return(RET_OK));
    std::vector<AccessibilityWindowInfo> windowInfo {};
    EXPECT_EQ(instance_->GetWindows(windowInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_002 end";
}

/**
 * @tc.number: GetWindows_003
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindows_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_003 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::vector<AccessibilityWindowInfo> windowInfo {};
    EXPECT_EQ(client->GetWindows(0, windowInfo), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "GetWindows_003 end";
}

/**
 * @tc.number: GetWindows_004
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindows_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_004 start";
    EXPECT_CALL(*stub_, GetWindowsByDisplayId(_, _)).Times(1).WillOnce(Return(RET_OK));
    std::vector<AccessibilityWindowInfo> windowInfo {};
    EXPECT_EQ(instance_->GetWindows(0, windowInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_004 end";
}

/**
 * @tc.number: SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(client->SearchElementInfosByText(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, TEST, infos), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 end";
}

/**
 * @tc.number: SearchElementInfosByText_002
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByText_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_002 start";
    EXPECT_CALL(*stub_, SearchElementInfosByText(_, _, _, _, _)).Times(1).WillOnce(Return(RET_ERR_FAILED));
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(instance_->SearchElementInfosByText(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, TEST, infos), RET_ERR_FAILED);
    GTEST_LOG_(INFO) << "SearchElementInfosByText_002 end";
}

/**
 * @tc.number: SearchElementInfosByText_003
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByText_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_003 start";
    EXPECT_CALL(*stub_, SearchElementInfosByText(_, _, _, _, _)).Times(1).WillOnce(Return(RET_OK));
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(instance_->SearchElementInfosByText(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, TEST, infos), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "SearchElementInfosByText_003 end";
}

/**
 * @tc.number: FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    AccessibilityElementInfo info {};
    EXPECT_EQ(client->FocusMoveSearch(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID,
        FocusMoveDirection::DOWN, info), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 end";
}

/**
 * @tc.number: FocusMoveSearch_002
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FocusMoveSearch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_002 start";
    EXPECT_CALL(*stub_, FocusMoveSearch(_, _, _, _, _)).Times(1).WillOnce(Return(RET_ERR_FAILED));
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->FocusMoveSearch(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID,
        FocusMoveDirection::DOWN, info), RET_ERR_FAILED);
    GTEST_LOG_(INFO) << "FocusMoveSearch_002 end";
}

/**
 * @tc.number: FocusMoveSearch_003
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FocusMoveSearch_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_003 start";
    EXPECT_CALL(*stub_, FocusMoveSearch(_, _, _, _, _)).Times(1).WillOnce(Return(RET_OK));
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->FocusMoveSearch(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID,
        FocusMoveDirection::DOWN, info), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "FocusMoveSearch_003 end";
}

/**
 * @tc.number: SetTargetBundleName_001
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SetTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(client->SetTargetBundleName(targetBundleNames), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 end";
}

/**
 * @tc.number: SetTargetBundleName_002
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SetTargetBundleName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 start";
    EXPECT_CALL(*stub_, SetTargetBundleName(_)).Times(1).WillOnce(Return(RET_OK));
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_OK);
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 end";
}

/**
 * @tc.number: SearchElementInfosByAccessibilityId_001
 * @tc.name: SearchElementInfosByAccessibilityId
 * @tc.desc: Test function SearchElementInfosByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_001 start";
    EXPECT_CALL(*stub_, SearchElementInfoByAccessibilityId(_, _, _, _, _)).Times(1).WillOnce(Return(RET_ERR_FAILED));
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(instance_->SearchElementInfosByAccessibilityId(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, MODE, infos), RET_ERR_FAILED);
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_001 end";
}

/**
 * @tc.number: SearchElementInfosByAccessibilityId_002
 * @tc.name: SearchElementInfosByAccessibilityId
 * @tc.desc: Test function SearchElementInfosByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByAccessibilityId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_002 start";
    std::shared_ptr<AccessibleAbilityChannelClient> client =
        std::make_shared<AccessibleAbilityChannelClient>(CHANNEL_ID, nullptr);
    ASSERT_TRUE(client);
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(client->SearchElementInfosByAccessibilityId(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, MODE, infos), RET_ERR_SAMGR);
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_002 end";
}

/**
 * @tc.number: SearchElementInfosByAccessibilityId_003
 * @tc.name: SearchElementInfosByAccessibilityId
 * @tc.desc: Test function SearchElementInfosByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByAccessibilityId_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_003 start";
    EXPECT_CALL(*stub_, SearchElementInfoByAccessibilityId(_, _, _, _, _)).Times(1).WillOnce(Return(RET_OK));
    std::vector<AccessibilityElementInfo> infos;
    EXPECT_EQ(instance_->SearchElementInfosByAccessibilityId(ACCESSIBILITY_WINDOW_ID,
        ELEMENT_ID, MODE, infos), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_003 end";
}
} // namespace Accessibility
} // namespace OHOS