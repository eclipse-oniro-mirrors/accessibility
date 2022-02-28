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
#include <memory>

#include "accessibility_operator.h"
#include "accessibility_window_info.h"
#include "mock_accessible_ability_channel_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowInfoUnitTest : public ::testing::Test {
public:
    AccessibilityWindowInfoUnitTest() {}
    ~AccessibilityWindowInfoUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest TearDown()";
    }
};

/**
 * @tc.number: GetWindowId_001
 * @tc.name: SetWindowId/GetWindowId
 * @tc.desc: Test function SetWindowId/GetWindowId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowId_001 start";
    AccessibilityWindowInfo window {};
    window.SetWindowId(1);
    EXPECT_TRUE(window.GetWindowId() == 1);
    GTEST_LOG_(INFO) << "GetWindowId_001 end";
}

/**
 * @tc.number: GetWindowTitle_001
 * @tc.name: SetWindowTitle/GetWindowTitle
 * @tc.desc: Test function SetWindowTitle/GetWindowTitle
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetWindowTitle_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowTitle_001 start";
    AccessibilityWindowInfo window {};
    string title = "test";
    window.SetWindowTitle(title);
    EXPECT_TRUE(strcmp(window.GetWindowTitle().c_str(), "test") == 0);
    GTEST_LOG_(INFO) << "GetWindowTitle_001 end";
}

/**
 * @tc.number: GetWindowType_001
 * @tc.name: SetWindowType/GetWindowType
 * @tc.desc: Test function SetWindowType/GetWindowType
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetWindowType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowType_001 start";
    AccessibilityWindowInfo window {};
    window.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_TRUE(window.GetWindowType() == WindowType::TYPE_APPLICATION);
    GTEST_LOG_(INFO) << "GetWindowType_001 end";
}

/**
 * @tc.number: GetWindowLayer_001
 * @tc.name: SetWindowLayer/GetWindowLayer
 * @tc.desc: Test function SetWindowLayer/GetWindowLayer
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetWindowLayer_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowLayer_001 start";
    AccessibilityWindowInfo window {};
    window.SetWindowLayer(100);
    EXPECT_TRUE(window.GetWindowLayer() == 100);
    GTEST_LOG_(INFO) << "GetWindowLayer_001 end";
}

/**
 * @tc.number: GetAnchor_001
 * @tc.name: SetAnchorId/GetAnchorId
 * @tc.desc: Test function SetAnchorId/GetAnchorId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetAnchorId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnchorId_001 start";
    AccessibilityWindowInfo window {};
    window.SetAnchorId(1);
    EXPECT_TRUE(window.GetAnchorId() == 1);
    GTEST_LOG_(INFO) << "GetAnchorId_001 end";
}

/**
 * @tc.number: GetParentId_001
 * @tc.name: SetParentId/GetParentId
 * @tc.desc: Test function SetParentId/GetParentId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetParentId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentId_001 start";
    AccessibilityWindowInfo window {};
    window.SetParentId(100);
    EXPECT_TRUE(window.GetParentId() == 100);
    GTEST_LOG_(INFO) << "GetParentId_001 end";
}

/**
 * @tc.number: GetChannelId_001
 * @tc.name: SetChannelId/GetChannelId
 * @tc.desc: Test function SetChannelId/GetChannelId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetChannelId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChannelId_001 start";
    AccessibilityWindowInfo window {};
    window.SetChannelId(1);
    EXPECT_TRUE(window.GetChannelId() == 1);
    GTEST_LOG_(INFO) << "GetChannelId_001 end";
}

/**
 * @tc.number: GetRectInScreen_001
 * @tc.name: SetRectInScreen/GetRectInScreen
 * @tc.desc: Test function SetRectInScreen/GetRectInScreen
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetRectInScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChannelId_001 start";
    AccessibilityWindowInfo window {};
    Rect bounds(1, 2, 3, 4);
    window.SetRectInScreen(bounds);
    EXPECT_TRUE(window.GetRectInScreen().GetLeftTopXScreenPostion() == 1);
    EXPECT_TRUE(window.GetRectInScreen().GetLeftTopYScreenPostion() == 2);
    EXPECT_TRUE(window.GetRectInScreen().GetRightBottomXScreenPostion() == 3);
    EXPECT_TRUE(window.GetRectInScreen().GetRightBottomYScreenPostion() == 4);
    GTEST_LOG_(INFO) << "GetChannelId_001 end";
}

/**
 * @tc.number: IsActive_001
 * @tc.name: IsActive
 * @tc.desc: Test function IsActive
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, IsActive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetChannelId_001 start";
    AccessibilityWindowInfo window {};
    window.SetActive(true);
    EXPECT_TRUE(window.IsActive());
    GTEST_LOG_(INFO) << "SetChannelId_001 end";
}

/**
 * @tc.number: IsFocused_001
 * @tc.name: IsFocused
 * @tc.desc: Test function IsFocused
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, IsFocused_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFocused_001 start";
    AccessibilityWindowInfo window {};
    window.SetFocused(true);
    EXPECT_TRUE(window.IsFocused());
    GTEST_LOG_(INFO) << "IsFocused_001 end";
}

/**
 * @tc.number: IsAccessibilityFocused_001
 * @tc.name: IsAccessibilityFocused
 * @tc.desc: Test function IsAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, IsAccessibilityFocused_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsAccessibilityFocused_001 start";
    AccessibilityWindowInfo window {};
    window.SetAccessibilityFocused(true);
    EXPECT_TRUE(window.IsAccessibilityFocused());
    GTEST_LOG_(INFO) << "IsAccessibilityFocused_001 end";
}

/**
 * @tc.number: AddChild_001
 * @tc.name: IsAccessibilityFocused
 * @tc.desc: Test function IsAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, AddChild_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddChild_001 start";
    AccessibilityWindowInfo window {};
    window.AddChild(1);
    EXPECT_TRUE(window.GetChildNum() == 1);
    window.AddChild(3);
    EXPECT_TRUE(window.GetChildNum() == 2);
    GTEST_LOG_(INFO) << "AddChild_001 end";
}

/**
 * @tc.number: ReadFromParcel_001
 * @tc.name: ReadFromParcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ReadFromParcel_001 start";

    Parcel parcel;
    AccessibilityWindowInfo windowInfo {};
    windowInfo.Marshalling(parcel);
    EXPECT_TRUE(windowInfo.ReadFromParcel(parcel));

    GTEST_LOG_(INFO) << "ReadFromParcel_001 end";
}
} // namespace Accessibility
} // namespace OHOS