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
#include "display_manager.h"
#include "mock_accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;

const static int32_t SEQUENCE = 1;
const static int32_t ACCESSIBILITY_WINDOW_ID = 1;
const static int32_t FOCUS_TYPE = 1;
const static int32_t SEQUENCE_NUM = 1;
const static int32_t ELEMENT_ID = 1;
const static int32_t ACTION = 1;
const static std::string TEST = "test";
const static int32_t DIRECTION = 1;
const static int32_t MODE = 1;

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityChannelClientTest : public ::testing::Test {
public:
    AccessibleAbilityChannelClientTest()
    {}
    ~AccessibleAbilityChannelClientTest()
    {}

    sptr<MockAccessibleAbilityChannelStub> stub_ = new MockAccessibleAbilityChannelStub();
    sptr<IAccessibleAbilityChannel> channel_ = new MockAccessibleAbilityChannelProxy(stub_->AsObject());
    int32_t channelId_ = 1;

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
        instance_ = std::make_shared<AccessibleAbilityChannelClient>(channelId_, channel_);
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityChannelClientTest TearDown()";
        instance_ = nullptr;
    }
};

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
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }

    AccessibilityElementInfo info {};
    EXPECT_FALSE(instance_->FindFocusedElementInfo(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, FOCUS_TYPE, info));
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendSimulateGesture_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }

    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    instance_->SendSimulateGesture(SEQUENCE_NUM, gesturePath);
    GTEST_LOG_(INFO) << "SendSimulateGesture_001 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelClientTest, ExecuteAction, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }
    std::map<std::string, std::string> actionArguments;
    EXPECT_FALSE(instance_->ExecuteAction(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, ACTION, actionArguments));
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_AddListener_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelClientTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }
    std::vector<AccessibilityWindowInfo> res {};
    uint64_t displayId = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    res = instance_->GetWindows(displayId);
    EXPECT_EQ(0, res.size());
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get SearchElementInfosByText instance_";
        return;
    }
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    EXPECT_FALSE(instance_->SearchElementInfosByText(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, TEST, infos));
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 end";
}

/**
 * @tc.number: FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelClientTest, FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }
    AccessibilityElementInfo info {};
    EXPECT_FALSE(instance_->FocusMoveSearch(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, DIRECTION, info));
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 end";
}

/**
 * @tc.number: ExecuteCommonAction_001
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 */
HWTEST_F(AccessibleAbilityChannelClientTest, ExecuteCommonAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteCommonAction_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }
    EXPECT_FALSE(instance_->ExecuteCommonAction(ACTION));
    GTEST_LOG_(INFO) << "ExecuteCommonAction_001 end";
}

/**
 * @tc.number: SearchElementInfosByAccessibilityId_001
 * @tc.name: SearchElementInfosByAccessibilityId
 * @tc.desc: Test function SearchElementInfosByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelClientTest, SearchElementInfosByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_001 start";
    if (!instance_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibleAbilityChannelClient instance_";
        return;
    }
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    EXPECT_FALSE(instance_->SearchElementInfosByAccessibilityId(ACCESSIBILITY_WINDOW_ID, ELEMENT_ID, MODE, infos));

    GTEST_LOG_(INFO) << "SearchElementInfosByAccessibilityId_001 end";
}
} // namespace Accessibility
} // namespace OHOS