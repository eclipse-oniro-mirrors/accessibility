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
#include "accessibility_touch_guider.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {

extern std::vector<int> TouchAction;
extern std::vector<EventType> eventType;
extern int UTgestureId;
extern bool isClearEvents;
extern bool isDestroyEvents;
class TouchGuiderTest : public testing::Test {
public:

    TouchGuiderTest() {}
    ~TouchGuiderTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    TouchEvent CreateTouchEvent(int action);
    TouchEvent CreateTouchEvent(int action, MmiPoint &point, int pointerCount,
                            unsigned int occurredTime, unsigned int startTime);
    TouchEvent CreateMoveEvent(int pointerCount);
    std::unique_ptr<TouchGuider> touchGuider_ = nullptr;
    int pointId_ = -1;
};

void TouchGuiderTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest SetUpTestCase";
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();
}

void TouchGuiderTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest TearDownTestCase";
}

void TouchGuiderTest::SetUp()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest SetUp";

    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    wmdInfo.width = 1000;
    wmdInfo.height = 1000;

    displayMgr.SetDisplay(wmdInfo);

    touchGuider_ = std::make_unique<TouchGuider>();
    if (touchGuider_ == nullptr) {
        GTEST_LOG_(INFO) << "touchGuider new failed!";
        return;
    }
    touchGuider_->StartUp();
    pointId_ = 0;
}

void TouchGuiderTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest TearDown";
    eventType.clear();
    TouchAction.clear();
}

TouchEvent TouchGuiderTest::CreateTouchEvent(int action)
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {};
    struct ManipulationProperty manipulationProperty = {
        .pointerCount = 1,
        .pointerId = pointId_++,
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

TouchEvent TouchGuiderTest::CreateTouchEvent(int action, MmiPoint &point, int pointerCount,
                            unsigned int occurredTime, unsigned int startTime)
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {
        .occurredTime = occurredTime,
    };
    struct ManipulationProperty manipulationProperty = {
        .mp = point,
        .startTime = startTime,
        .pointerCount = pointerCount,
        .pointerId = pointId_++,
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

TouchEvent TouchGuiderTest::CreateMoveEvent(int pointerCount)
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {};
    struct ManipulationProperty manipulationProperty = {
        .pointerCount = pointerCount,
        .pointerId = pointId_++,
    };
    struct TouchProperty touch = {
        .action = TouchEnum::POINT_MOVE,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

/**
 * @tc.number: OnTouchEvent001
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the first down event.
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_001 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_001 end";
}

/**
 * @tc.number: OnTouchEvent002
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the first up event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_002 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);

    sleep(3);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(TouchAction[1], TouchEnum::HOVER_POINTER_EXIT);
    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_002 end";
}

/**
 * @tc.number: OnTouchEvent003
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that down-up slowly.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_003 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);

    sleep(3);

    EXPECT_EQ(TouchAction[1], TouchEnum::HOVER_POINTER_EXIT);
    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_003 end";
}


/**
 * @tc.number: OnTouchEvent004
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_004 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);

    event = CreateMoveEvent(1);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[2], TouchEnum::HOVER_POINTER_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[3], TouchEnum::HOVER_POINTER_EXIT);
    sleep(3);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_004 end";
}

/**
 * @tc.number: OnTouchEvent005
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that two fingers moving in directions.(The parameter index has no effect)
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_005 start";

    MmiPoint point(0, 0);
    MmiPoint otherPoint(10, 10);
    MmiPoint movePoint(15,5);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, otherPoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movePoint, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_005 end";
}

/**
 * @tc.number: OnTouchEvent006
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that two fingers moving in sanme directions in touchGuiding state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_006 start";

    MmiPoint point(0, 0);
    MmiPoint otherPoint(10, 10);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, otherPoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_UP);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_006 end";
}

/**
 * @tc.number: OnTouchEvent007
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that multi-finger gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_007 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);

    event = CreateMoveEvent(3);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_007 end";
}

/**
 * @tc.number: OnTouchEvent008
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the CANCEL event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_008 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    event = CreateTouchEvent(TouchEnum::CANCEL);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(TouchAction[1], TouchEnum::HOVER_POINTER_EXIT);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_008 end";
}

/**
 * @tc.number: OnTouchEvent009
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the OTHER_POINT_DOWN event in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_009 start";

    MmiPoint point(0, 0);
    MmiPoint otherPoint(500, 500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, otherPoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(TouchAction[2], TouchEnum::PRIMARY_POINT_DOWN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_009 end";
}

/**
 * @tc.number: OnTouchEvent010
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that two fingers moving in sanme directions in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_010 start";

    MmiPoint point(0, 0);
    MmiPoint otherPoint(10, 10);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, otherPoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_010 end";
}

/**
 * @tc.number: OnTouchEvent011
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that multi-finger moving in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_011 start";

    MmiPoint point(0, 0);
    MmiPoint otherPoint(500, 500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, otherPoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point, 3, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(TouchAction[2], TouchEnum::PRIMARY_POINT_DOWN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_011 end";
}

/**
 * @tc.number: OnTouchEvent012
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the drag event .
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_012 start";

    MmiPoint point(10, 10);
    MmiPoint otherPoint(50, 10);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, point, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, otherPoint, 2, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_012 end";
}

/**
 * @tc.number: OnTouchEvent013
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the accessibility event .
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_013 start";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);

    event = CreateMoveEvent(1);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(TouchAction[2], TouchEnum::HOVER_POINTER_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[3], TouchEnum::HOVER_POINTER_EXIT);

    AccessibilityEventInfo accessibilityEvent;
    accessibilityEvent.SetEventType(EventType::TYPE_VIEW_HOVER_EXIT_EVENT);
    touchGuider_->OnAccessibilityEvent(accessibilityEvent);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_013 end";
}

/**
 * @tc.number: OnTouchEvent014
 * @tc.name:OnTouchEvent
 * @tc.desc: Check cancel the second move event after onstart.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_014 start";

    MmiPoint point(500, 500);
    MmiPoint movepoint(2500, 500);
    MmiPoint othermovepoint(2500,750);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, othermovepoint, 1, 400, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_014 end";
}

/**
 * @tc.number: OnTouchEvent015
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the RIGHT_THEN_DOWN gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_015 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(3500, 2500);
    MmiPoint movepoint2(5000,2500);
    MmiPoint movepoint3(4000,0);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[2], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(UTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_015 end";
}

/**
 * @tc.number: OnTouchEvent016
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the LEFT_THEN_DOWN gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_016 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(1500, 2500);
    MmiPoint movepoint2(0,2500);
    MmiPoint movepoint3(1000,0);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[2], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(UTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_016 end";
}

/**
 * @tc.number: OnTouchEvent017
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the UP_THEN_LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_017 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(2500, 3500);
    MmiPoint movepoint2(2500,5000);
    MmiPoint movepoint3(0,4000);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[2], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(UTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_UP_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_017 end";
}

/**
 * @tc.number: OnTouchEvent018
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the UP_THEN_RIGHT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_018 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(2500, 3500);
    MmiPoint movepoint2(2500,5000);
    MmiPoint movepoint3(5000,4000);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[2], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,movepoint3, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(UTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_UP_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_018 end";
}

/**
 * @tc.number: OnTouchEvent019
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_019 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(1000, 2500);
    MmiPoint movepoint2(0,2500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,movepoint2, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(UTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_019 end";
}

/**
 * @tc.number: OnTouchEvent020
 * @tc.name:OnTouchEvent
 * @tc.desc: Check cancel the first move event without onstart.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_020 start";

    MmiPoint point(2500, 2500);
    MmiPoint movepoint1(2250, 2500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, movepoint1, 1, 400, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(TouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(TouchAction[1], TouchEnum::HOVER_POINTER_MOVE);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_020 end";
}

/**
 * @tc.number: OnTouchEvent021
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the double-tap event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_021 start";

    MmiPoint point(500, 500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 200, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 0, 0);
    touchGuider_->OnTouchEvent(event);

    EXPECT_EQ(eventType[1], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_021 end";
}

/**
 * @tc.number: OnTouchEvent022
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the double tap and long press event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_022 start";

    MmiPoint point(500, 500);
    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0);
    touchGuider_->OnTouchEvent(event);
    EXPECT_EQ(eventType[0], EventType::TYPE_TOUCH_BEGIN);


    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 40, 0);
    touchGuider_->OnTouchEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 200, 0);
    touchGuider_->OnTouchEvent(event);
    sleep(1);

    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_022 end";
}

/**
 * @tc.number: OnTouchEvent023
 * @tc.name:OnTouchEvent
 * @tc.desc: Check ClearEvents and DestroyEvents.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_023 start";

    touchGuider_->ClearEvents(CompositeEvent::ROTATION);
    EXPECT_EQ(isClearEvents, true);

    touchGuider_->DestroyEvents();
    EXPECT_EQ(isDestroyEvents, true);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_023 end";
}
}  // namespace Accessibility
}  // namespace OHOS
