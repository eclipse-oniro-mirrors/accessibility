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
#include <memory>
#include "accessibility_window_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
constexpr int32_t INNER_WINDOW_ID = 2;
constexpr int64_t UI_NODE_ID = 2;

class AccessibilityWindowInfoUnitTest : public ::testing::Test {
public:
    AccessibilityWindowInfoUnitTest()
    {}
    ~AccessibilityWindowInfoUnitTest()
    {}
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
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest SetUp() Start";
        windowInfo_ = std::make_shared<AccessibilityWindowInfo>();
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoUnitTest TearDown()";
        windowInfo_ = nullptr;
    }

    std::shared_ptr<AccessibilityWindowInfo> windowInfo_ = nullptr;
};

/**
 * @tc.number: SetAccessibilityWindowType_001
 * @tc.name: SetAccessibilityWindowType
 * @tc.desc: Test function SetAccessibilityWindowType
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetAccessibilityWindowType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityWindowType_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
    EXPECT_EQ(windowInfo_->GetAccessibilityWindowType(), AccessibilityWindowType::TYPE_APPLICATION);
    GTEST_LOG_(INFO) << "SetAccessibilityWindowType_001 end";
}

/**
 * @tc.number: SetWindowLayer_001
 * @tc.name: SetWindowLayer
 * @tc.desc: Test function SetWindowLayer
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetWindowLayer_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowLayer_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetWindowLayer(1);
    EXPECT_EQ(windowInfo_->GetWindowLayer(), 1);
    GTEST_LOG_(INFO) << "SetWindowLayer_001 end";
}

/**
 * @tc.number: SetWindowId_001
 * @tc.name: SetWindowId
 * @tc.desc: Test function SetWindowId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowId_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetWindowId(1);
    EXPECT_EQ(windowInfo_->GetWindowId(), 1);
    GTEST_LOG_(INFO) << "SetWindowId_001 end";
}

/**
 * @tc.number: SetRectInScreen_001
 * @tc.name: SetRectInScreen
 * @tc.desc: Test function SetRectInScreen
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetRectInScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetRectInScreen_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    Rect boundParam(1, 1, 2, 2);
    windowInfo_->SetRectInScreen(boundParam);
    Rect boundRet = windowInfo_->GetRectInScreen();
    EXPECT_EQ(boundRet.GetLeftTopXScreenPostion(), 1);
    EXPECT_EQ(boundRet.GetLeftTopYScreenPostion(), 1);
    EXPECT_EQ(boundRet.GetRightBottomXScreenPostion(), 2);
    EXPECT_EQ(boundRet.GetRightBottomYScreenPostion(), 2);
    GTEST_LOG_(INFO) << "SetRectInScreen_001 end";
}

/**
 * @tc.number: SetActive_001
 * @tc.name: SetActive
 * @tc.desc: Test function SetActive
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetActive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetActive_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetActive(true);
    EXPECT_TRUE(windowInfo_->IsActive());
    GTEST_LOG_(INFO) << "SetActive_001 end";
}

/**
 * @tc.number: SetFocused_001
 * @tc.name: SetFocused
 * @tc.desc: Test function SetFocused
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetFocused_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFocused_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetFocused(true);
    EXPECT_TRUE(windowInfo_->IsFocused());
    GTEST_LOG_(INFO) << "SetFocused_001 end";
}

/**
 * @tc.number: SetAccessibilityFocused_001
 * @tc.name: SetAccessibilityFocused
 * @tc.desc: Test function SetAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetAccessibilityFocused_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityFocused_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetAccessibilityFocused(true);
    EXPECT_TRUE(windowInfo_->IsAccessibilityFocused());
    GTEST_LOG_(INFO) << "SetAccessibilityFocused_001 end";
}

/**
 * @tc.number: SetDisplayId_001
 * @tc.name: SetDisplayId
 * @tc.desc: Test function SetDisplayId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetDisplayId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDisplayId_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetDisplayId(100);
    EXPECT_EQ(windowInfo_->GetDisplayId(), 100);
    GTEST_LOG_(INFO) << "SetDisplayId_001 end";
}

/**
 * @tc.number: SetWindowType_001
 * @tc.name: SetWindowType
 * @tc.desc: Test function SetWindowType
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetWindowType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowType_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetWindowType(1);
    EXPECT_EQ(windowInfo_->GetWindowType(), 1);
    GTEST_LOG_(INFO) << "SetWindowType_001 end";
}

/**
 * @tc.number: SetWindowMode_001
 * @tc.name: SetWindowMode
 * @tc.desc: Test function SetWindowMode
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetWindowMode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowMode_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    GTEST_LOG_(INFO) << "SetWindowMode_001 begin";
    windowInfo_->SetWindowMode(1);
    EXPECT_EQ(windowInfo_->GetWindowMode(), 1);
    GTEST_LOG_(INFO) << "SetWindowMode_001 end";
}

/**
 * @tc.number: SetDecorEnable_001
 * @tc.name: SetDecorEnable
 * @tc.desc: Test function SetDecorEnable
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetDecorEnable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDecorEnable_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetDecorEnable(true);
    EXPECT_TRUE(windowInfo_->IsDecorEnable());
    GTEST_LOG_(INFO) << "SetDecorEnable_001 end";
}

/**
 * @tc.number: GetInnerWid_001
 * @tc.name: GetInnerWid
 * @tc.desc: Test function GetInnerWid
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetInnerWid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInnerWid_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetInnerWid(INNER_WINDOW_ID);
    EXPECT_EQ(windowInfo_->GetInnerWid(), INNER_WINDOW_ID);
    GTEST_LOG_(INFO) << "GetInnerWid_001 end";
}

/**
 * @tc.number: GetUiNodeId_001
 * @tc.name: GetUiNodeId
 * @tc.desc: Test function GetUiNodeId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetUiNodeId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetUiNodeId_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetUiNodeId(UI_NODE_ID);
    EXPECT_EQ(windowInfo_->GetUiNodeId(), UI_NODE_ID);
    GTEST_LOG_(INFO) << "GetUiNodeId_001 end";
}

/**
 * @tc.number: GetScaleVal_001
 * @tc.name: GetScaleVal
 * @tc.desc: Test function GetScaleVal
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetScaleVal_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetScaleVal_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetScaleVal(1.1);
    EXPECT_FLOAT_EQ(windowInfo_->GetScaleVal(), 1.1);
    GTEST_LOG_(INFO) << "GetScaleVal_001 end";
}

/**
 * @tc.number: GetScaleX_001
 * @tc.name: GetScaleX
 * @tc.desc: Test function GetScaleX
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetScaleX_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetScaleX_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetScaleX(1.1);
    EXPECT_FLOAT_EQ(windowInfo_->GetScaleX(), 1.1);
    GTEST_LOG_(INFO) << "GetScaleX_001 end";
}

/**
 * @tc.number: GetScaleY_001
 * @tc.name: GetScaleY
 * @tc.desc: Test function GetScaleY
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, GetScaleY_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetScaleY_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetScaleY(1.1);
    EXPECT_FLOAT_EQ(windowInfo_->GetScaleY(), 1.1);
    GTEST_LOG_(INFO) << "GetScaleY_001 end";
}

/**
 * @tc.number: SetMainWindowId_001
 * @tc.name: SetMainWindowId
 * @tc.desc: Test function SetMainWindowId
 */
HWTEST_F(AccessibilityWindowInfoUnitTest, SetMainWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMainWindowId_001 start";
    if (!windowInfo_) {
        GTEST_LOG_(INFO) << "windowInfo_ is null";
        return;
    }
    windowInfo_->SetMainWindowId(1);
    EXPECT_EQ(windowInfo_->GetMainWindowId(), 1);
    GTEST_LOG_(INFO) << "SetMainWindowId_001 end";
}
} // namespace Accessibility
} // namespace OHOS