/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_touch_exploration.h"
#include "accessibility_ut_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t POINT_ID_0 = 0;
    constexpr int32_t POINT_ID_1 = 1;
    constexpr int32_t POINT_ID_2 = 2;
    constexpr int32_t POINT_ID_3 = 3;
    constexpr int32_t SLEEP_US_50 = 50;
    constexpr int32_t SLEEP_US_300 = 300;
    constexpr int32_t ACTION_INVALID = -1;
    constexpr int32_t DISPLAY_10 = 10;
    constexpr int32_t DISPLAY_500 = 500;
    constexpr int32_t DISPLAY_1000 = 1000;
    constexpr int32_t DISPLAY_1500 = 1500;
} // namespace

class TouchExplorationTest : public testing::Test {
public:
    TouchExplorationTest()
    {}
    ~TouchExplorationTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    static void SetTouchExplorationPoint(MMI::PointerEvent::PointerItem &point, int id, int x, int y);
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action, int32_t pointerId, int32_t displayX,
        int32_t displayY);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action,
        std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerCount);
    std::unique_ptr<TouchExploration> touchExploration_ = nullptr;
};

void TouchExplorationTest::SetTouchExplorationPoint(MMI::PointerEvent::PointerItem &point, int id, int x, int y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

void TouchExplorationTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TouchExplorationTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void TouchExplorationTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TouchExplorationTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void TouchExplorationTest::SetUp()
{
    GTEST_LOG_(INFO) << "TouchExplorationTest SetUp";

    touchExploration_ = std::make_unique<TouchExploration>();
    if (!touchExploration_) {
        GTEST_LOG_(INFO) << "TouchExploration new failed!";
        return;
    }
    touchExploration_->StartUp();
}

void TouchExplorationTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchExplorationTest TearDown";
    AccessibilityAbilityHelper::GetInstance().ClearEventTypeActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    usleep(SLEEP_US_300);
    touchExploration_->Clear();
    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
}

std::shared_ptr<MMI::PointerEvent> TouchExplorationTest::CreateTouchEvent(int32_t action, int32_t pointerId,
    int32_t displayX, int32_t displayY)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(pointerId);
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> TouchExplorationTest::CreateTouchEvent(int32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    for (auto &point : points) {
        if (point.GetPointerId() == pointerId) {
            point.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
        } else {
            point.SetPressed(true);
        }
        pointerEvent->AddPointerItem(point);
    }
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return pointerEvent;
}

/**
 * @tc.number: HandleInitStateDown001
 * @tc.name: HandleInitStateDown
 * @tc.desc: Test func HandleInitStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInitStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateDown_001 end";
}

/**
 * @tc.number: HandleInitStateDown002
 * @tc.name: HandleInitStateDown
 * @tc.desc: Test func HandleInitStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInitStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchExplorationPoint(point, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchExplorationPoint(otherPoint, POINT_ID_1, 0, 0);
    points.emplace_back(point);
    points.emplace_back(otherPoint);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::PASSING_THROUGH);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateDown_002 end";
}

/**
 * @tc.number: HandleInitStateUp001
 * @tc.name: HandleInitStateUp
 * @tc.desc: Test func HandleInitStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInitStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateUp_001 end";
}

/**
 * @tc.number: HandleInitStateUp002
 * @tc.name: HandleInitStateUp
 * @tc.desc: Test func HandleInitStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInitStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::PASSING_THROUGH);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateUp_002 end";
}

/**
 * @tc.number: HandleInitStateMove001
 * @tc.name: HandleInitStateMove
 * @tc.desc: Test func HandleInitStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInitStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::PASSING_THROUGH);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInitStateMove_001 end";
}

/**
 * @tc.number: HandlePassingThroughState001
 * @tc.name: HandlePassingThroughState
 * @tc.desc: Test func HandlePassingThroughState.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandlePassingThroughState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandlePassingThroughState_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::PASSING_THROUGH);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::PASSING_THROUGH);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandlePassingThroughState_001 end";
}

/**
 * @tc.number: HandlePassingThroughState002
 * @tc.name: HandlePassingThroughState
 * @tc.desc: Test func HandlePassingThroughState.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandlePassingThroughState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandlePassingThroughState_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::PASSING_THROUGH);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandlePassingThroughState_002 end";
}

/**
 * @tc.number: HandleInvalidState001
 * @tc.name: HandleInvalidState
 * @tc.desc: Test func HandleInvalidState.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInvalidState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInvalidState_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::INVALID);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0), ACTION_INVALID);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInvalidState_001 end";
}

/**
 * @tc.number: HandleInvalidState002
 * @tc.name: HandleInvalidState
 * @tc.desc: Test func HandleInvalidState.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleInvalidState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInvalidState_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::INVALID);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0), ACTION_INVALID);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleInvalidState_002 end";
}

/**
 * @tc.number: HandleCancelEvent001
 * @tc.name: HandleCancelEvent
 * @tc.desc: Test func HandleCancelEvent.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleCancelEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleCancelEvent_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_CANCEL, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_HOVER_CANCEL);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleCancelEvent_001 end";
}

/**
 * @tc.number: HandleOneFingerDownStateDown001
 * @tc.name: HandleOneFingerDownStateDown
 * @tc.desc: Test func HandleOneFingerDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_DOWN);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0), ACTION_INVALID);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateDown_001 end";
}

/**
 * @tc.number: HandleOneFingerDownStateUp001
 * @tc.name: HandleOneFingerDownStateUp
 * @tc.desc: Test func HandleOneFingerDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_DOWN);

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0), ACTION_INVALID);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SINGLE_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateUp_001 end";
}

/**
 * @tc.number: HandleOneFingerDownStateMove001
 * @tc.name: HandleOneFingerDownStateMove
 * @tc.desc: Test func HandleOneFingerDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateMove_001 end";
}

/**
 * @tc.number: HandleOneFingerDownStateMove002
 * @tc.name: HandleOneFingerDownStateMove
 * @tc.desc: Test func HandleOneFingerDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerDownStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, 0, DISPLAY_500);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SWIPE);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerDownStateMove_002 end";
}

/**
 * @tc.number: HandleOneFingerLongPressStateDown001
 * @tc.name: HandleOneFingerLongPressStateDown
 * @tc.desc: Test func HandleOneFingerLongPressStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerLongPressStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_1,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateDown_001 end";
}

/**
 * @tc.number: HandleOneFingerLongPressStateUp001
 * @tc.name: HandleOneFingerLongPressStateUp
 * @tc.desc: Test func HandleOneFingerLongPressStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerLongPressStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateUp_001 end";
}

/**
 * @tc.number: HandleOneFingerLongPressStateMove001
 * @tc.name: HandleOneFingerLongPressStateMove
 * @tc.desc: Test func HandleOneFingerLongPressStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerLongPressStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_LONG_PRESS);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_LONG_PRESS);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerLongPressStateMove_001 end";
}

/**
 * @tc.number: HandleOneFingerSwipeStateDown001
 * @tc.name: HandleOneFingerSwipeStateDown
 * @tc.desc: Test func HandleOneFingerSwipeStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSwipeStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SWIPE);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_1,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateDown_001 end";
}

/**
 * @tc.number: HandleOneFingerSwipeStateUp001
 * @tc.name: HandleOneFingerSwipeStateUp
 * @tc.desc: Test func HandleOneFingerSwipeStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSwipeStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, DISPLAY_500, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, DISPLAY_500, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateUp_001 end";
}

/**
 * @tc.number: HandleOneFingerSwipeStateUp002
 * @tc.name: HandleOneFingerSwipeStateUp
 * @tc.desc: Test func HandleOneFingerSwipeStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSwipeStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_DOWN);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, DISPLAY_500, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, DISPLAY_1000, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, DISPLAY_1500, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetGestureId(),
        static_cast<int>(GestureType::GESTURE_SWIPE_RIGHT));
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSwipeStateUp_002 end";
}

/**
 * @tc.number: HandleOneFingerSingleTapStateDown001
 * @tc.name: HandleOneFingerSingleTapStateDown
 * @tc.desc: Test func HandleOneFingerSingleTapStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSingleTapStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapStateDown_001 end";
}

/**
 * @tc.number: HandleOneFingerSingleTapThenDownStateDown001
 * @tc.name: HandleOneFingerSingleTapThenDownStateDown
 * @tc.desc: Test func HandleOneFingerSingleTapThenDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_1,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateDown_001 end";
}

/**
 * @tc.number: HandleOneFingerSingleTapThenDownStateUp001
 * @tc.name: HandleOneFingerSingleTapThenDownStateUp
 * @tc.desc: Test func HandleOneFingerSingleTapThenDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_001 end";
}

/**
 * @tc.number: HandleOneFingerSingleTapThenDownStateUp002
 * @tc.name: HandleOneFingerSingleTapThenDownStateUp
 * @tc.desc: Test func HandleOneFingerSingleTapThenDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchExplorationPoint(point, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchExplorationPoint(otherPoint, POINT_ID_1, 0, 0);
    points.emplace_back(point);
    points.emplace_back(otherPoint);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateUp_002 end";
}

/**
 * @tc.number: HandleOneFingerSingleTapThenDownStateMove001
 * @tc.name: HandleOneFingerSingleTapThenDownStateMove
 * @tc.desc: Test func HandleOneFingerSingleTapThenDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::ONE_FINGER_SINGLE_TAP_THEN_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleOneFingerSingleTapThenDownStateMove_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateDown001
 * @tc.name: HandleTwoFingersDownStateDown
 * @tc.desc: Test func HandleTwoFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateDown002
 * @tc.name: HandleTwoFingersDownStateDown
 * @tc.desc: Test func HandleTwoFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_002 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateDown003
 * @tc.name: HandleTwoFingersDownStateDown
 * @tc.desc: Test func HandleTwoFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    points.emplace_back(thirdPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::THREE_FINGERS_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateDown_003 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateUp001
 * @tc.name: HandleTwoFingersDownStateUp
 * @tc.desc: Test func HandleTwoFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateUp002
 * @tc.name: HandleTwoFingersDownStateUp
 * @tc.desc: Test func HandleTwoFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_002 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateUp003
 * @tc.name: HandleTwoFingersDownStateUp
 * @tc.desc: Test func HandleTwoFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateUp_003 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateMove001
 * @tc.name: HandleTwoFingersDownStateMove
 * @tc.desc: Test func HandleTwoFingersDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateMove002
 * @tc.name: HandleTwoFingersDownStateMove
 * @tc.desc: Test func HandleTwoFingersDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_500);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1),
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_DRAG);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_002 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateMove003
 * @tc.name: HandleTwoFingersDownStateMove
 * @tc.desc: Test func HandleTwoFingersDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDownStateMove_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_500, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2),
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::PASSING_THROUGH);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDownStateMove_003 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateDown001
 * @tc.name: HandleTwoFingersDragStateDown
 * @tc.desc: Test func HandleTwoFingersDragStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateDown_001 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateDown002
 * @tc.name: HandleTwoFingersDragStateDown
 * @tc.desc: Test func HandleTwoFingersDragStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_DRAG);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateDown_002 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateUp001
 * @tc.name: HandleTwoFingersDragStateUp
 * @tc.desc: Test func HandleTwoFingersDragStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateUp_001 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateUp002
 * @tc.name: HandleTwoFingersDragStateUp
 * @tc.desc: Test func HandleTwoFingersDragStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_DRAG);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateUp_002 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateMove001
 * @tc.name: HandleTwoFingersDragStateMove
 * @tc.desc: Test func HandleTwoFingersDragStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_DRAG);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0), ACTION_INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateMove_001 end";
}

/**
 * @tc.number: HandleTwoFingersDragStateMove002
 * @tc.name: HandleTwoFingersDragStateMove
 * @tc.desc: Test func HandleTwoFingersDragStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersDragStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_500);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_1000);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_1000);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0),
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1),
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2),
        MMI::PointerEvent::POINTER_ACTION_MOVE);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersDragStateMove_002 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateDown001
 * @tc.name: HandleMultiFingersTapStateDown
 * @tc.desc: Test func HandleMultiFingersTapStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_001 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateDown002
 * @tc.name: HandleMultiFingersTapStateDown
 * @tc.desc: Test func HandleMultiFingersTapStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, DISPLAY_10 + DISPLAY_10, 0);
    points.emplace_back(thirdPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_002 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateDown003
 * @tc.name: HandleMultiFingersTapStateDown
 * @tc.desc: Test func HandleMultiFingersTapStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateDown_003 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateUp001
 * @tc.name: HandleMultiFingersTapStateUp
 * @tc.desc: Test func HandleMultiFingersTapStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateUp_001 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateUp002
 * @tc.name: HandleMultiFingersTapStateUp
 * @tc.desc: Test func HandleMultiFingersTapStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateUp_002 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateMove001
 * @tc.name: HandleMultiFingersTapStateMove
 * @tc.desc: Test func HandleMultiFingersTapStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_TAP);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_001 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateMove002
 * @tc.name: HandleMultiFingersTapStateMove
 * @tc.desc: Test func HandleMultiFingersTapStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_002 end";
}

/**
 * @tc.number: HandleMultiFingersTapStateMove003
 * @tc.name: HandleMultiFingersTapStateMove
 * @tc.desc: Test func HandleMultiFingersTapStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersTapStateMove_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0, DISPLAY_500, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersTapStateMove_003 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateDown001
 * @tc.name: HandleMultiFingersContinueDownStateDown
 * @tc.desc: Test func HandleMultiFingersContinueDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateDown_001 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateUp001
 * @tc.name: HandleMultiFingersContinueDownStateUp
 * @tc.desc: Test func HandleMultiFingersContinueDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_001 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateUp002
 * @tc.name: HandleMultiFingersContinueDownStateUp
 * @tc.desc: Test func HandleMultiFingersContinueDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_002 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateUp003
 * @tc.name: HandleMultiFingersContinueDownStateUp
 * @tc.desc: Test func HandleMultiFingersContinueDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, DISPLAY_10, 0);
    points.emplace_back(thirdPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_003 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateUp004
 * @tc.name: HandleMultiFingersContinueDownStateUp
 * @tc.desc: Test func HandleMultiFingersContinueDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_004 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_CONTINUE_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::THREE_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_004 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateUp005
 * @tc.name: HandleMultiFingersContinueDownStateUp
 * @tc.desc: Test func HandleMultiFingersContinueDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_005 start";

    touchExploration_->SetCurrentState(TouchExplorationState::FOUR_FINGERS_CONTINUE_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::FOUR_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateUp_005 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateMove001
 * @tc.name: HandleMultiFingersContinueDownStateMove
 * @tc.desc: Test func HandleMultiFingersContinueDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_CONTINUE_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_001 end";
}

/**
 * @tc.number: HandleMultiFingersContinueDownStateMove002
 * @tc.name: HandleMultiFingersContinueDownStateMove
 * @tc.desc: Test func HandleMultiFingersContinueDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0, 0, 0);
    touchExploration_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_500);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleMultiFingersContinueDownStateMove_002 end";
}

/**
 * @tc.number: HandleTwoFingersUnknownStateDown001
 * @tc.name: HandleTwoFingersUnknownStateDown
 * @tc.desc: Test func HandleTwoFingersUnknownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_001 end";
}

/**
 * @tc.number: HandleTwoFingersUnknownStateDown002
 * @tc.name: HandleTwoFingersUnknownStateDown
 * @tc.desc: Test func HandleTwoFingersUnknownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateDown_002 end";
}

/**
 * @tc.number: HandleTwoFingersUnknownStateUp001
 * @tc.name: HandleTwoFingersUnknownStateUp
 * @tc.desc: Test func HandleTwoFingersUnknownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TOUCH_INIT);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_001 end";
}

/**
 * @tc.number: HandleTwoFingersUnknownStateUp002
 * @tc.name: HandleTwoFingersUnknownStateUp
 * @tc.desc: Test func HandleTwoFingersUnknownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateUp_002 end";
}

/**
 * @tc.number: HandleTwoFingersUnknownStateMove001
 * @tc.name: HandleTwoFingersUnknownStateMove
 * @tc.desc: Test func HandleTwoFingersUnknownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleTwoFingersUnknownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TWO_FINGERS_UNKNOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::TWO_FINGERS_UNKNOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleTwoFingersUnknownStateMove_001 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateDown001
 * @tc.name: HandleThreeFingersDownStateDown
 * @tc.desc: Test func HandleThreeFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_001 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateDown002
 * @tc.name: HandleThreeFingersDownStateDown
 * @tc.desc: Test func HandleThreeFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    MMI::PointerEvent::PointerItem fourthPoint = {};
    SetTouchExplorationPoint(fourthPoint, POINT_ID_3, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    points.emplace_back(fourthPoint);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_3);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_002 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateDown003
 * @tc.name: HandleThreeFingersDownStateDown
 * @tc.desc: Test func HandleThreeFingersDownStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    points.emplace_back(thirdPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem fourthPoint = {};
    SetTouchExplorationPoint(fourthPoint, POINT_ID_3, 0, 0);
    points.emplace_back(fourthPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_3);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::FOUR_FINGERS_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateDown_003 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateUp001
 * @tc.name: HandleThreeFingersDownStateUp
 * @tc.desc: Test func HandleThreeFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::THREE_FINGERS_DOWN);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_001 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateUp002
 * @tc.name: HandleThreeFingersDownStateUp
 * @tc.desc: Test func HandleThreeFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::THREE_FINGERS_TAP);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_002 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateUp003
 * @tc.name: HandleThreeFingersDownStateUp
 * @tc.desc: Test func HandleThreeFingersDownStateUp.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_003 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_DOWN);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, 0, 0);
    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, 0, 0);
    MMI::PointerEvent::PointerItem fourthPoint = {};
    SetTouchExplorationPoint(fourthPoint, POINT_ID_3, 0, 0);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    points.emplace_back(fourthPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points,
        POINT_ID_3);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateUp_003 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateMove001
 * @tc.name: HandleThreeFingersDownStateMove
 * @tc.desc: Test func HandleThreeFingersDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateMove_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, DISPLAY_10 + DISPLAY_10, 0);
    points.emplace_back(thirdPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_500);
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, DISPLAY_10 + DISPLAY_10, DISPLAY_500);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    points.emplace_back(thirdPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::THREE_FINGERS_SWIPE);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateMove_001 end";
}

/**
 * @tc.number: HandleThreeFingersDownStateMove002
 * @tc.name: HandleThreeFingersDownStateMove
 * @tc.desc: Test func HandleThreeFingersDownStateMove.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersDownStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateMove_002 start";

    touchExploration_->SetCurrentState(TouchExplorationState::TOUCH_INIT);
    
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, 0);
    points.emplace_back(firstPoint);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points,
        POINT_ID_0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem secondPoint = {};
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, 0);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    MMI::PointerEvent::PointerItem thirdPoint = {};
    SetTouchExplorationPoint(thirdPoint, POINT_ID_2, DISPLAY_10 + DISPLAY_10, 0);
    points.emplace_back(thirdPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    points.clear();
    SetTouchExplorationPoint(firstPoint, POINT_ID_0, 0, DISPLAY_500);
    SetTouchExplorationPoint(secondPoint, POINT_ID_1, DISPLAY_10, DISPLAY_500);
    points.emplace_back(firstPoint);
    points.emplace_back(secondPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersDownStateMove_002 end";
}

/**
 * @tc.number: HandleThreeFingersSwipeStateDown001
 * @tc.name: HandleThreeFingersSwipeStateDown
 * @tc.desc: Test func HandleThreeFingersSwipeStateDown.
 */
HWTEST_F(TouchExplorationTest, TouchExploration_Unittest_HandleThreeFingersSwipeStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersSwipeStateDown_001 start";

    touchExploration_->SetCurrentState(TouchExplorationState::THREE_FINGERS_SWIPE);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, POINT_ID_0,
        0, 0);
    touchExploration_->OnPointerEvent(*event);

    usleep(SLEEP_US_50);

    EXPECT_EQ(touchExploration_->GetCurrentState(), TouchExplorationState::INVALID);

    GTEST_LOG_(INFO) << "TouchExploration_Unittest_HandleThreeFingersSwipeStateDown_001 end";
}
} // namespace Accessibility
} // namespace OHOS