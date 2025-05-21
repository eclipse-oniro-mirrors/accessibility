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

class MultiFingerGestureHandlerTest : public ::testing::Test {
public:
    MultiFingerGestureHandlerTest()
    {}
    ~MultiFingerGestureHandlerTest()
    {}

    std::shared_ptr<MultiFingerGestureHandler> instance_ = nullptr;
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "MultiFingerGestureHandlerTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "MultiFingerGestureHandlerTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "MultiFingerGestureHandlerTest SetUp()";
        instance_ = std::make_shared<MultiFingerGestureHandler>();
    }
    void TearDown()
    {
        GTEST_LOG_(INFO) << "MultiFingerGestureHandlerTest TearDown()";
        instance_ = nullptr;
    }
};

/**
 * @tc.number: IsTapGesture_001
 * @tc.name: IsTapGesture
 * @tc.desc: Test function IsTapGesture
 */
HWTEST_F(MultiFingerGestureHandlerTest, IsTapGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsTapGesture_001 start";
    EXPECT_TRUE(AccessibilityUITestAbility::IsTapGesture());
    GTEST_LOG_(INFO) << "IsTapGesture_001 end";
}

/**
 * @tc.number: ProcessMultiFingerGestureTypeEvent_001
 * @tc.name: ProcessMultiFingerGestureTypeEvent
 * @tc.desc: Test function ProcessMultiFingerGestureTypeEvent(listener is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ProcessMultiFingerGestureTypeEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_001 start";
    EXPECT_TRUE(instance_);

    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    EXPECT_EQ(instance_->ProcessMultiFingerGestureTypeEvent(listener), RET_ERR_INVALID_PARAM);

    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_001 end";
}

/**
 * @tc.number: ProcessMultiFingerGestureTypeEvent_002
 * @tc.name: ProcessMultiFingerGestureTypeEvent
 * @tc.desc: Test function ProcessMultiFingerGestureTypeEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ProcessMultiFingerGestureTypeEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->ProcessMultiFingerGestureTypeEvent(listener), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_002 end";
}

/**
 * @tc.number: ProcessMultiFingerGestureTypeEvent_003
 * @tc.name: ProcessMultiFingerGestureTypeEvent
 * @tc.desc: Test function ProcessMultiFingerGestureTypeEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ProcessMultiFingerGestureTypeEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_003 start";

    EXPECT_TRUE(instance_);
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->ProcessMultiFingerGestureTypeEvent(listener), RET_OK);

    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureTypeEvent_003 end";
}

/**
 * @tc.number: ProcessMultiFingerGestureEvent_001
 * @tc.name: ProcessMultiFingerGestureEvent
 * @tc.desc: Test function ProcessMultiFingerGestureEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, ProcessMultiFingerGestureEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureEvent_001 start";

    EXPECT_TRUE(instance_);
    EXPECT_EQ(RET_OK, instance_->ProcessMultiFingerGestureEvent());

    GTEST_LOG_(INFO) << "ProcessMultiFingerGestureEvent_001 end";
}

/**
 * @tc.number: ProcessEvent_001
 * @tc.name: ProcessEvent
 * @tc.desc: Test function ProcessEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, ProcessEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProcessEvent_001 start";

    EXPECT_TRUE(instance_);
    EXPECT_EQ(instance_->ProcessEvent(), RET_OK);

    GTEST_LOG_(INFO) << "ProcessEvent_001 end";
}

/**
 * @tc.number: AccessibilityMultiTapGestureRecognizer_001
 * @tc.name: AccessibilityMultiTapGestureRecognizer
 * @tc.desc: Test function AccessibilityMultiTapGestureRecognizer(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, AccessibilityMultiTapGestureRecognizer_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMultiTapGestureRecognizer_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->AccessibilityMultiTapGestureRecognizer(FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "AccessibilityMultiTapGestureRecognizer_001 end";
}

/**
 * @tc.number: AccessibilityMultiTapGestureRecognizer_002
 * @tc.name: AccessibilityMultiTapGestureRecognizer
 * @tc.desc: Test function AccessibilityMultiTapGestureRecognizer(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, AccessibilityMultiTapGestureRecognizer_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMultiTapGestureRecognizer_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->AccessibilityMultiTapGestureRecognizer(FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "AccessibilityMultiTapGestureRecognizer_002 end";
}

/**
 * @tc.number: CancelTwoFingerEvent_001
 * @tc.name: CancelTwoFingerEvent
 * @tc.desc: Test function CancelTwoFingerEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelTwoFingerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelTwoFingerEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelTwoFingerEvent(sourceInfo, FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "CancelTwoFingerEvent_001 end";
}

/**
 * @tc.number: CancelTwoFingerEvent_002
 * @tc.name: CancelTwoFingerEvent
 * @tc.desc: Test function CancelTwoFingerEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelTwoFingerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelTwoFingerEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelTwoFingerEvent(sourceInfo, FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "CancelTwoFingerEvent_002 end";
}

/**
 * @tc.number: CancelThreeFingerEvent_001
 * @tc.name: CancelThreeFingerEvent
 * @tc.desc: Test function CancelThreeFingerEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelThreeFingerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelThreeFingerEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelThreeFingerEvent(sourceInfo, FOCUS_TYPE, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "CancelThreeFingerEvent_001 end";
}

/**
 * @tc.number: CancelThreeFingerEvent_002
 * @tc.name: CancelThreeFingerEvent
 * @tc.desc: Test function CancelThreeFingerEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelThreeFingerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelThreeFingerEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelThreeFingerEvent(sourceInfo, FOCUS_TYPE, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "CancelThreeFingerEvent_002 end";
}

/**
 * @tc.number: CancelTapAndHoldGestureEvent_001
 * @tc.name: CancelTapAndHoldGestureEvent
 * @tc.desc: Test function CancelTapAndHoldGestureEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelTapAndHoldGestureEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelTapAndHoldGestureEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->CancelTapAndHoldGestureEvent(gesturePath), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "CancelTapAndHoldGestureEvent_001 end";
}

/**
 * @tc.number: CancelTapAndHoldGestureEvent_002
 * @tc.name: CancelTapAndHoldGestureEvent
 * @tc.desc: Test function CancelTapAndHoldGestureEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelTapAndHoldGestureEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelTapAndHoldGestureEvent_002 start";

    EXPECT_TRUE(instance_);
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->CancelTapAndHoldGestureEvent(gesturePath), RET_OK);

    GTEST_LOG_(INFO) << "CancelTapAndHoldGestureEvent_002 end";
}

/**
 * @tc.number: CancelHoldGestureEvent_001
 * @tc.name: CancelHoldGestureEvent
 * @tc.desc: Test function CancelHoldGestureEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelHoldGestureEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelHoldGestureEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelHoldGestureEvent(elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "CancelHoldGestureEvent_001 end";
}

/**
 * @tc.number: CancelHoldGestureEvent_002
 * @tc.name: CancelHoldGestureEvent
 * @tc.desc: Test function CancelHoldGestureEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelHoldGestureEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelHoldGestureEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelHoldGestureEvent(elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "CancelHoldGestureEvent_002 end";
}

/**
 * @tc.number: CancelAllPenddingEvent_001
 * @tc.name: CancelAllPenddingEvent
 * @tc.desc: Test function CancelAllPenddingEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelAllPenddingEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelAllPenddingEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelAllPenddingEvent(windowInfo, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "CancelAllPenddingEvent_001 end";
}

/**
 * @tc.number: CancelAllPenddingEvent_002
 * @tc.name: CancelAllPenddingEvent
 * @tc.desc: Test function CancelAllPenddingEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelAllPenddingEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelAllPenddingEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityWindowInfo windowInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->CancelAllPenddingEvent(windowInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "CancelAllPenddingEvent_002 end";
}

/**
 * @tc.number: CancelGesture_001
 * @tc.name: CancelGesture
 * @tc.desc: Test function CancelGesture(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelGesture_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityWindowInfo winInfo {};
    EXPECT_EQ(instance_->CancelGesture(WINDOW_ID, winInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "CancelGesture_001 end";
}

/**
 * @tc.number: CancelGesture_002
 * @tc.name: CancelGesture
 * @tc.desc: Test function CancelGesture(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, CancelGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CancelGesture_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityWindowInfo winInfo {};
    EXPECT_EQ(instance_->CancelGesture(WINDOW_ID, winInfo), RET_OK);
    GTEST_LOG_(INFO) << "CancelGesture_002 end";
}

/**
 * @tc.number: ParamCheck_001
 * @tc.name: ParamCheck
 * @tc.desc: Test function ParamCheck(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ParamCheck_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ParamCheck_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->ParamCheck(res), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "ParamCheck_001 end";
}

/**
 * @tc.number: ParamCheck_002
 * @tc.name: ParamCheck
 * @tc.desc: Test function ParamCheck(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ParamCheck_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ParamCheck_002 start";

    EXPECT_TRUE(instance_);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->ParamCheck(res), RET_OK);
    GTEST_LOG_(INFO) << "ParamCheck_002 end";
}

/**
 * @tc.number: ParamCheck_003
 * @tc.name: ParamCheck
 * @tc.desc: Test function ParamCheck(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ParamCheck_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ParamCheck_003 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->ParamCheck(DISPLAY_ID, res), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);
    GTEST_LOG_(INFO) << "ParamCheck_003 end";
}

/**
 * @tc.number: ParamCheck_004
 * @tc.name: ParamCheck
 * @tc.desc: Test function ParamCheck(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, ParamCheck_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ParamCheck_004 start";

    EXPECT_TRUE(instance_);
    std::vector<AccessibilityWindowInfo> res {};
    EXPECT_EQ(instance_->ParamCheck(DISPLAY_ID, res), RET_OK);
    GTEST_LOG_(INFO) << "ParamCheck_004 end";
}

/**
 * @tc.number: GetLastFirstPointUpTime_001
 * @tc.name: GetLastFirstPointUpTime
 * @tc.desc: Test function GetLastFirstPointUpTime(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetLastFirstPointUpTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetLastFirstPointUpTime_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo nextElementInfo {};
    AccessibilityElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetLastFirstPointUpTime(elementInfo, direction, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetLastFirstPointUpTime_001 end";
}

/**
 * @tc.number: GetLastFirstPointUpTime_002
 * @tc.name: GetLastFirstPointUpTime
 * @tc.desc: Test function GetLastFirstPointUpTime(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetLastFirstPointUpTime_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetLastFirstPointUpTime_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo nextElementInfo {};
    AccessibilityElementInfo elementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetLastFirstPointUpTime(elementInfo, direction, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "GetLastFirstPointUpTime_002 end";
}

/**
 * @tc.number: IsDoubelTapSlopConditionMatch_001
 * @tc.name: IsDoubelTapSlopConditionMatch
 * @tc.desc: Test function IsDoubelTapSlopConditionMatch(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, IsDoubelTapSlopConditionMatch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsDoubelTapSlopConditionMatch_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->IsDoubelTapSlopConditionMatch(index, parent, child), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "IsDoubelTapSlopConditionMatch_001 end";
}

/**
 * @tc.number: IsDoubelTapSlopConditionMatch_002
 * @tc.name: IsDoubelTapSlopConditionMatch
 * @tc.desc: Test function IsDoubelTapSlopConditionMatch(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, IsDoubelTapSlopConditionMatch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsDoubelTapSlopConditionMatch_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    int32_t index = 1;
    EXPECT_EQ(instance_->IsDoubelTapSlopConditionMatch(index, parent, child), RET_OK);

    GTEST_LOG_(INFO) << "IsDoubelTapSlopConditionMatch_002 end";
}

/**
 * @tc.number: GetPointerItemWithFingerNum_001
 * @tc.name: GetPointerItemWithFingerNum
 * @tc.desc: Test function GetPointerItemWithFingerNum(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetPointerItemWithFingerNum_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetPointerItemWithFingerNum_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo parent {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetPointerItemWithFingerNum(parent, TEST, elementInfos), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetPointerItemWithFingerNum_001 end";
}

/**
 * @tc.number: GetPointerItemWithFingerNum_002
 * @tc.name: GetPointerItemWithFingerNum
 * @tc.desc: Test function GetPointerItemWithFingerNum(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetPointerItemWithFingerNum_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetPointerItemWithFingerNum_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo parent {};
    std::vector<AccessibilityElementInfo> elementInfos {};
    EXPECT_EQ(instance_->GetPointerItemWithFingerNum(parent, TEST, elementInfos), RET_OK);

    GTEST_LOG_(INFO) << "GetPointerItemWithFingerNum_002 end";
}

/**
 * @tc.number: IsMultiFingerDoubleTap_001
 * @tc.name: IsMultiFingerDoubleTap
 * @tc.desc: Test function IsMultiFingerDoubleTap(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, IsMultiFingerDoubleTap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsMultiFingerDoubleTap_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->IsMultiFingerDoubleTap(eventInfo, elementInfo), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "IsMultiFingerDoubleTap_001 end";
}

/**
 * @tc.number: IsMultiFingerDoubleTap_002
 * @tc.name: IsMultiFingerDoubleTap
 * @tc.desc: Test function IsMultiFingerDoubleTap(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, IsMultiFingerDoubleTap_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsMultiFingerDoubleTap_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->IsMultiFingerDoubleTap(eventInfo, elementInfo), RET_OK);

    GTEST_LOG_(INFO) << "IsMultiFingerDoubleTap_002 end";
}

/**
 * @tc.number: HanleFirstTouchDownEvent_001
 * @tc.name: HanleFirstTouchDownEvent
 * @tc.desc: Test function HanleFirstTouchDownEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HanleFirstTouchDownEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HanleFirstTouchDownEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo child {};
    AccessibilityElementInfo parent {};
    EXPECT_EQ(instance_->HanleFirstTouchDownEvent(child, parent), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "HanleFirstTouchDownEvent_001 end";
}

/**
 * @tc.number: HanleFirstTouchDownEvent_002
 * @tc.name: HanleFirstTouchDownEvent
 * @tc.desc: Test function HanleFirstTouchDownEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HanleFirstTouchDownEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HanleFirstTouchDownEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo child {};
    AccessibilityElementInfo parent {};
    EXPECT_EQ(instance_->HanleFirstTouchDownEvent(child, parent), RET_OK);

    GTEST_LOG_(INFO) << "HanleFirstTouchDownEvent_002 end";
}

/**
 * @tc.number: HandleMultiTapEvent_001
 * @tc.name: HandleMultiTapEvent
 * @tc.desc: Test function HandleMultiTapEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleMultiTapEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleMultiTapEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->HandleMultiTapEvent(elementInfo, action, actionArguments), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "HandleMultiTapEvent_001 end";
}

/**
 * @tc.number: HandleMultiTapEvent_002
 * @tc.name: HandleMultiTapEvent
 * @tc.desc: Test function HandleMultiTapEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleMultiTapEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleMultiTapEvent_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->HandleMultiTapEvent(elementInfo, action, actionArguments), RET_OK);

    GTEST_LOG_(INFO) << "HandleMultiTapEvent_002 end";
}

/**
 * @tc.number: HandleContinueTouchDownEvent_001
 * @tc.name: HandleContinueTouchDownEvent
 * @tc.desc: Test function HandleContinueTouchDownEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleContinueTouchDownEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleContinueTouchDownEvent_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->HandleContinueTouchDownEvent(targetBundleNames), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "HandleContinueTouchDownEvent_001 end";
}

/**
 * @tc.number: HandleContinueTouchDownEvent_002
 * @tc.name: HandleContinueTouchDownEvent
 * @tc.desc: Test function HandleContinueTouchDownEvent(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleContinueTouchDownEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleContinueTouchDownEvent_002 start";

    EXPECT_TRUE(instance_);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->HandleContinueTouchDownEvent(targetBundleNames), RET_OK);

    GTEST_LOG_(INFO) << "HandleContinueTouchDownEvent_002 end";
}

/**
 * @tc.number: storeBaseDownPoint_001
 * @tc.name: storeBaseDownPoint
 * @tc.desc: Test function storeBaseDownPoint
 */
HWTEST_F(MultiFingerGestureHandlerTest, storeBaseDownPoint_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "storeBaseDownPoint_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityElementInfo parent;
    std::vector<AccessibilityElementInfo> children;
    EXPECT_EQ(instance_->storeBaseDownPoint(parent, children), RET_OK);

    GTEST_LOG_(INFO) << "storeBaseDownPoint_001 end";
}

/**
 * @tc.number: GetSwipeDirection_001
 * @tc.name: GetSwipeDirection
 * @tc.desc: Test function GetSwipeDirection
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetSwipeDirection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSwipeDirection_001 start";

    EXPECT_TRUE(instance_);
    instance_->GetSwipeDirection(CACHE_MODE);
    EXPECT_EQ(CACHE_MODE, AccessibilityAbilityUtHelper::IsTapGesture().GetCacheMode());

    GTEST_LOG_(INFO) << "GetSwipeDirection_001 end";
}

/**
 * @tc.number: SaveMoveGesturePointerInfo_001
 * @tc.name: SaveMoveGesturePointerInfo
 * @tc.desc: Test function SaveMoveGesturePointerInfo(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, SaveMoveGesturePointerInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaveMoveGesturePointerInfo_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    std::vector<AccessibilityElementInfo> elementInfos;
    EXPECT_EQ(instance_->SaveMoveGesturePointerInfo(elementInfos), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "SaveMoveGesturePointerInfo_001 end";
}

/**
 * @tc.number: SaveMoveGesturePointerInfo_002
 * @tc.name: SaveMoveGesturePointerInfo
 * @tc.desc: Test function SaveMoveGesturePointerInfo(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, SaveMoveGesturePointerInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaveMoveGesturePointerInfo_002 start";

    EXPECT_TRUE(instance_);
    std::vector<AccessibilityElementInfo> elementInfos;
    EXPECT_EQ(instance_->SaveMoveGesturePointerInfo(elementInfos), RET_OK);

    GTEST_LOG_(INFO) << "SaveMoveGesturePointerInfo_002 end";
}

/**
 * @tc.number: GetBasePointItem_001
 * @tc.name: GetBasePointItem
 * @tc.desc: Test function CancelAllPenddingEvent(AccessibleAbilityClient is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetBasePointItem_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetBasePointItem_001 start";

    EXPECT_TRUE(instance_);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(true);
    AccessibilityWindowInfo windowInfo {};
    std::vector<AccessibilityElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetBasePointItem(windowInfo, elementInfos, true), RET_ERR_NULLPTR);
    AccessibilityAbilityUtHelper::IsTapGesture().SetAbilityClientNullFlag(false);

    GTEST_LOG_(INFO) << "GetBasePointItem_001 end";
}

/**
 * @tc.number: GetBasePointItem_002
 * @tc.name: GetBasePointItem
 * @tc.desc: Test function GetBasePointItem(AccessibleAbilityClient is not null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetBasePointItem_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetBasePointItem_002 start";

    EXPECT_TRUE(instance_);
    AccessibilityWindowInfo windowInfo {};
    std::vector<AccessibilityElementInfo> elementInfos;
    EXPECT_EQ(instance_->GetBasePointItem(windowInfo, elementInfos, true), RET_OK);

    GTEST_LOG_(INFO) << "GetBasePointItem_002 end";
}

/**
 * @tc.number: HandleMultiFingerMoveEvent_001
 * @tc.name: HandleMultiFingerMoveEvent
 * @tc.desc: Test function HandleMultiFingerMoveEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleMultiFingerMoveEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleMultiFingerMoveEvent_001 start";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    EXPECT_TRUE(currentAccount);
    sptr<AccessibilityWindowConnection> operationConnection =
        new AccessibilityWindowConnection(windowId, nullptr, accountId);
    currentAccount->AddAccessibilityWindowConnection(windowId, operationConnection);
    std::string cmdWindow("-w");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdWindow));
    int ret = HandleMultiFingerMoveEventer_->HandleMultiFingerMoveEvent(fd_, args);
    EXPECT_GE(ret, -1);
    currentAccount->RemoveAccessibilityWindowConnection(windowId);
    GTEST_LOG_(INFO) << "HandleMultiFingerMoveEvent_001 end";
}

/**
 * @tc.number: HandleMultiFingerMoveEvent_002
 * @tc.name: HandleMultiFingerMoveEvent
 * @tc.desc: Test function HandleMultiFingerMoveEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleMultiFingerMoveEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleMultiFingerMoveEvent_002 start";
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    EXPECT_TRUE(config);
    config->SetCaptionState(true);
    std::string cmdUser("-u");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdUser));
    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(true);
    int ret = HandleMultiFingerMoveEventer_->HandleMultiFingerMoveEvent(fd_, args);
    EXPECT_GE(ret, -1);
    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(false);
    GTEST_LOG_(INFO) << "HandleMultiFingerMoveEvent_002 end";
}

/**
 * @tc.number: StoreUpPointInPointerRoute_001
 * @tc.name: StoreUpPointInPointerRoute
 * @tc.desc: Test function StoreUpPointInPointerRoute
 */
HWTEST_F(MultiFingerGestureHandlerTest, StoreUpPointInPointerRoute_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StoreUpPointInPointerRoute_001 start";
    EXPECT_TRUE(instance_);

    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance_->StoreUpPointInPointerRoute(info, SEQUENCE_NUM);
    EXPECT_NE(instance_.get(), nullptr);
    GTEST_LOG_(INFO) << "StoreUpPointInPointerRoute_001 end";
}

/**
 * @tc.number: recognizeGesturePath_001
 * @tc.name: recognizeGesturePath
 * @tc.desc: Test function recognizeGesturePath
 */
HWTEST_F(MultiFingerGestureHandlerTest, recognizeGesturePath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "recognizeGesturePath_001 start";
    EXPECT_TRUE(instance_);

    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance_->recognizeGesturePath(info, SEQUENCE_NUM);
    EXPECT_NE(instance_.get(), nullptr);
    GTEST_LOG_(INFO) << "recognizeGesturePath_001 end";
}

/**
 * @tc.number: GetMoveGestureId_001
 * @tc.name: GetMoveGestureId
 * @tc.desc: Test function GetMoveGestureId
 */
HWTEST_F(MultiFingerGestureHandlerTest, GetMoveGestureId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetMoveGestureId_001 start";
    EXPECT_TRUE(instance_);

    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance_->GetMoveGestureId(true, SEQUENCE_NUM);
    EXPECT_NE(instance_.get(), nullptr);
    GTEST_LOG_(INFO) << "GetMoveGestureId_001 end";
}

/**
 * @tc.number: IsMoveGestureRecognize_001
 * @tc.name: IsMoveGestureRecognize
 * @tc.desc: Test function IsMoveGestureRecognize
 */
HWTEST_F(
    MultiFingerGestureHandlerTest, IsMoveGestureRecognize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsMoveGestureRecognize_001 start";

    EXPECT_TRUE(instance_);

    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance_->IsMoveGestureRecognize(infos, SEQUENCE_NUM);
    EXPECT_NE(instance_.get(), nullptr);
    GTEST_LOG_(INFO) << "IsMoveGestureRecognize_001 end";
}

/**
 * @tc.number: HandleMultiFingerTouchUpEvent_001
 * @tc.name: HandleMultiFingerTouchUpEvent
 * @tc.desc: Test function HandleMultiFingerTouchUpEvent
 */
HWTEST_F(MultiFingerGestureHandlerTest, HandleMultiFingerTouchUpEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleMultiFingerTouchUpEvent_001 start";
    EXPECT_TRUE(instance_);

    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance_->HandleMultiFingerTouchUpEvent(infos, SEQUENCE_NUM);
    EXPECT_NE(instance_.get(), nullptr);
    GTEST_LOG_(INFO) << "HandleMultiFingerTouchUpEvent_001 end";
}

/**
 * @tc.number: OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(listener is null)
 */
HWTEST_F(MultiFingerGestureHandlerTest, OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnPointerEvent_001 start";

    EXPECT_TRUE(instance_);
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    EXPECT_EQ(instance_->OnPointerEvent(listener), RET_ERR_INVALID_PARAM);

    GTEST_LOG_(INFO) << "OnPointerEvent_001 end";
}
} // namespace Accessibility
} // namespace OHOS