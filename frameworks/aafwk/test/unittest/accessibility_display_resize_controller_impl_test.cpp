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
#include "accessibility_display_resize_controller_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class UnitTestDisplayResizeListener : public DisplayResizeListener {
public:
    UnitTestDisplayResizeListener()
    {}
    virtual ~UnitTestDisplayResizeListener()
    {}

    void OnDisplayResized(const Rect& rect, const float scale, const float centerX, const float centerY) override
    {
        GTEST_LOG_(INFO) << "UnitTestDisplayResizeListener OnDisplayResized";
    }
};

class DisplayResizeControllerUnitTest : public ::testing::Test {
public:
    DisplayResizeControllerUnitTest()
    {}
    ~DisplayResizeControllerUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<DisplayResizeControllerImpl> displayResizeController_ = nullptr;
    std::shared_ptr<AccessibleAbilityChannelClient> channelClient_ = nullptr;
};

void DisplayResizeControllerUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### DisplayResizeControllerUnitTest Start ######################";
}

void DisplayResizeControllerUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### DisplayResizeControllerUnitTest End ######################";
}

void DisplayResizeControllerUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "UT SetUp";
    displayResizeController_ = std::make_shared<DisplayResizeControllerImpl>(0, channelClient_);
}

void DisplayResizeControllerUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "UT TearDown";
    displayResizeController_ = nullptr;
}

/**
 * @tc.number: DisplayResizeController_Unittest_AddListener_001
 * @tc.name: AddListener
 * @tc.desc: Test function AddListener
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_AddListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_AddListener_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    std::shared_ptr<DisplayResizeListener> listener = std::make_shared<UnitTestDisplayResizeListener>();
    displayResizeController_->AddListener(listener);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_AddListener_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_DeleteListener_001
 * @tc.name: DeleteListener
 * @tc.desc: Test function DeleteListener
 *           Delete the listener which hasn't been added.
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_DeleteListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DeleteListener_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    std::shared_ptr<DisplayResizeListener> listener = std::make_shared<UnitTestDisplayResizeListener>();
    displayResizeController_->DeleteListener(listener);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DeleteListener_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_DeleteListener_002
 * @tc.name: DeleteListener
 * @tc.desc: Test function DeleteListener
 *           Delete the listener which has been added.
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_DeleteListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DeleteListener_002 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    std::shared_ptr<DisplayResizeListener> listener = std::make_shared<UnitTestDisplayResizeListener>();
    displayResizeController_->AddListener(listener);
    displayResizeController_->DeleteListener(listener);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DeleteListener_002 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_GetCenterX_001
 * @tc.name: GetCenterX
 * @tc.desc: Test function GetCenterX
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_GetCenterX_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetCenterX_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    EXPECT_EQ(displayResizeController_->GetCenterX(), 1);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetCenterX_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_GetCenterY_001
 * @tc.name: GetCenterY
 * @tc.desc: Test function GetCenterY
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_GetCenterY_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetCenterY_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    EXPECT_EQ(displayResizeController_->GetCenterY(), 1);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetCenterY_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_GetDisplayResizeRect_001
 * @tc.name: GetDisplayResizeRect
 * @tc.desc: Test function GetDisplayResizeRect
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_GetDisplayResizeRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetDisplayResizeRect_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    Rect rect = displayResizeController_->GetDisplayResizeRect();
    EXPECT_EQ(rect.GetLeftTopXScreenPostion(), 0);
    EXPECT_EQ(rect.GetLeftTopYScreenPostion(), 0);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetDisplayResizeRect_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_GetScale_001
 * @tc.name: GetScale
 * @tc.desc: Test function GetScale
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_GetScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetScale_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    EXPECT_EQ(displayResizeController_->GetScale(), 1);

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_GetScale_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_Reset_001
 * @tc.name: Reset
 * @tc.desc: Test function Reset
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_Reset_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    EXPECT_FALSE(displayResizeController_->Reset(true));

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_Reset_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_SetCenter_001
 * @tc.name: SetCenter
 * @tc.desc: Test function SetCenter
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_SetCenter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_SetCenter_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    float centerX = 0;
    float centerY = 0;
    bool isShouldAnimate = true;

    EXPECT_FALSE(displayResizeController_->SetCenter(centerX, centerY, isShouldAnimate));

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_SetCenter_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_SetScale_001
 * @tc.name: SetScale
 * @tc.desc: Test function SetScale
 */
HWTEST_F(DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_SetScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_SetScale_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    float scale = 0;
    bool isShouldAnimate = true;

    EXPECT_FALSE(displayResizeController_->SetScale(scale, isShouldAnimate));

    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_SetScale_001 end";
}

/**
 * @tc.number: DisplayResizeController_Unittest_DispatchOnDisplayResized_001
 * @tc.name: DispatchOnDisplayResized
 * @tc.desc: Test function DispatchOnDisplayResized
 */
HWTEST_F(
    DisplayResizeControllerUnitTest, DisplayResizeController_Unittest_DispatchOnDisplayResized_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DispatchOnDisplayResized_001 start";
    if (!displayResizeController_) {
        GTEST_LOG_(INFO) << "Cann't get DisplayResizeControllerImpl displayResizeController_";
        return;
    }
    Rect rect {};
    float scale = 1;
    float centerX = 0;
    float centerY = 0;

    std::shared_ptr<DisplayResizeListener> listener = std::make_shared<UnitTestDisplayResizeListener>();
    displayResizeController_->AddListener(listener);

    displayResizeController_->DispatchOnDisplayResized(rect, scale, centerX, centerY);

    displayResizeController_->DeleteListener(listener);
    GTEST_LOG_(INFO) << "DisplayResizeController_Unittest_DispatchOnDisplayResized_001 end";
}
} // namespace Accessibility
} // namespace OHOS