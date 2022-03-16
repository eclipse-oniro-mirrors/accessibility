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
#include "accessibility_touch_guider.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
extern std::vector<int> g_touchAction;
extern std::vector<EventType> g_eventType;
extern int g_uTgestureId;
extern bool g_isClearEvents;
extern bool g_isDestroyEvents;
class TouchGuiderTest : public testing::Test {
public:
    TouchGuiderTest() {}
    ~TouchGuiderTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int action);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int action,
        std::vector<MMI::PointerEvent::PointerItem> points,
        int pointerCount, unsigned int occurredTime, unsigned int startTime);
    std::shared_ptr<MMI::PointerEvent> CreateMoveEvent(int pointerCount);
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

    touchGuider_ = std::make_unique<TouchGuider>();
    if (!touchGuider_) {
        GTEST_LOG_(INFO) << "touchGuider new failed!";
        return;
    }
    touchGuider_->StartUp();
    pointId_ = 0;
}

void TouchGuiderTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest TearDown";
    g_eventType.clear();
    g_touchAction.clear();
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateTouchEvent(int action)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateTouchEvent(int action,
    std::vector<MMI::PointerEvent::PointerItem> points, int pointerId,
    unsigned int occurredTime, unsigned int startTime)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    for (auto point : points) {
        pointerEvent->AddPointerItem(point);
    }
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionStartTime(startTime);
    pointerEvent->SetActionTime(occurredTime);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateMoveEvent(int pointerCount)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);

    return pointerEvent;
}

/**
 * @tc.number: OnTouchEvent001
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first down event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_001 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_001 end";
}

/**
 * @tc.number: OnTouchEvent002
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first up event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_002 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);

    sleep(3);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);
    EXPECT_EQ(g_touchAction[1], expectValue);
    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_002 end";
}

/**
 * @tc.number: OnTouchEvent003
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that down-up slowly.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_003 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);

    sleep(3);

    EXPECT_EQ(g_touchAction[1], expectValue);
    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_003 end";
}

/**
 * @tc.number: OnTouchEvent004
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_004 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    event = CreateMoveEvent(1);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    sleep(3);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_004 end";
}

/**
 * @tc.number: OnTouchEvent006
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in touchGuiding state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_006 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(10);
    otherPoint.SetGlobalY(10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
                                                                points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    points.clear();
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(g_touchAction[1], expectValue);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_006 end";
}

/**
 * @tc.number: OnTouchEvent007
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_007 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(10);
    otherPoint.SetGlobalY(10);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    otherPoint1.SetPointerId(3);
    otherPoint1.SetGlobalX(10);
    otherPoint1.SetGlobalY(10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_007 end";
}

/**
 * @tc.number: OnTouchEvent008
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the CANCEL event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_008 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    touchGuider_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_008 end";
}

/**
 * @tc.number: OnTouchEvent009
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the OTHER_POINT_DOWN event in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_009 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(500);
    otherPoint.SetGlobalY(500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    otherPoint1.SetPointerId(3);
    otherPoint1.SetGlobalX(500);
    otherPoint1.SetGlobalY(500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(g_touchAction[1], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[2], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_009 end";
}

/**
 * @tc.number: OnTouchEvent010
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in sanme directions in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_010 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(10);
    otherPoint.SetGlobalY(10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[1], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_010 end";
}

/**
 * @tc.number: OnTouchEvent011
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger moving in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_011 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(500);
    otherPoint.SetGlobalY(500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    otherPoint1.SetPointerId(3);
    otherPoint1.SetGlobalX(500);
    otherPoint1.SetGlobalY(500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(g_touchAction[1], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[2], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_011 end";
}

/**
 * @tc.number: OnTouchEvent012
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the drag event .
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_012 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(10);
    point.SetGlobalY(10);
    MMI::PointerEvent::PointerItem otherPoint = {};
    otherPoint.SetPointerId(2);
    otherPoint.SetGlobalX(50);
    otherPoint.SetGlobalY(10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_012 end";
}

/**
 * @tc.number: OnTouchEvent013
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the accessibility event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_013 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    sleep(1);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    event = CreateMoveEvent(1);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);

    AccessibilityEventInfo accessibilityEvent;
    accessibilityEvent.SetEventType(EventType::TYPE_VIEW_HOVER_EXIT_EVENT);
    touchGuider_->OnAccessibilityEvent(accessibilityEvent);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_013 end";
}

/**
 * @tc.number: OnTouchEvent014
 * @tc.name: OnPointerEvent
 * @tc.desc: Check cancel the second move event after onstart.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_014 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(500);
    point.SetGlobalY(500);
    MMI::PointerEvent::PointerItem movepoint = {};
    movepoint.SetPointerId(1);
    movepoint.SetGlobalX(2500);
    movepoint.SetGlobalY(500);
    MMI::PointerEvent::PointerItem othermovepoint = {};
    othermovepoint.SetPointerId(1);
    othermovepoint.SetGlobalX(2500);
    othermovepoint.SetGlobalY(750);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(othermovepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 400, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_014 end";
}

/**
 * @tc.number: OnTouchEvent015
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the RIGHT_THEN_DOWN gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_015 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(3500);
    movepoint1.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetGlobalX(5000);
    movepoint2.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    movepoint3.SetPointerId(1);
    movepoint3.SetGlobalX(4000);
    movepoint3.SetGlobalY(0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[2], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(g_uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_015 end";
}

/**
 * @tc.number: OnTouchEvent016
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the LEFT_THEN_DOWN gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_016 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(1500);
    movepoint1.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetGlobalX(0);
    movepoint2.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    movepoint3.SetPointerId(1);
    movepoint3.SetGlobalX(1000);
    movepoint3.SetGlobalY(0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[2], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(g_uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_016 end";
}

/**
 * @tc.number: OnTouchEvent017
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the UP_THEN_LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_017 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(2500);
    movepoint1.SetGlobalY(3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetGlobalX(2500);
    movepoint2.SetGlobalY(5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    movepoint3.SetPointerId(1);
    movepoint3.SetGlobalX(0);
    movepoint3.SetGlobalY(4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[2], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(g_uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_UP_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_017 end";
}

/**
 * @tc.number: OnTouchEvent018
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the UP_THEN_RIGHT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_018 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(2500);
    movepoint1.SetGlobalY(3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetGlobalX(2500);
    movepoint2.SetGlobalY(5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    movepoint3.SetPointerId(1);
    movepoint3.SetGlobalX(5000);
    movepoint3.SetGlobalY(4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[2], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(g_uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_UP_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_018 end";
}

/**
 * @tc.number: OnTouchEvent019
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_019 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(1000);
    movepoint1.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetGlobalX(0);
    movepoint2.SetGlobalY(2500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_touchAction[1], expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EXPECT_EQ(g_eventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_eventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(g_uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_019 end";
}

/**
 * @tc.number: OnTouchEvent020
 * @tc.name: OnPointerEvent
 * @tc.desc: Check cancel the first move event without onstart.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_020 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetGlobalX(2500);
    point.SetGlobalY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetGlobalX(2250);
    movepoint1.SetGlobalY(2500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[0], EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 400, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(g_eventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[0], expectValue);
    EXPECT_EQ(g_touchAction[1], expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_020 end";
}

/**
 * @tc.number: OnTouchEvent023
 * @tc.name: OnPointerEvent
 * @tc.desc: Check ClearEvents and DestroyEvents.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnTouchEvent_023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_023 start";

    touchGuider_->ClearEvents(MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD);
    EXPECT_EQ(g_isClearEvents, true);

    touchGuider_->DestroyEvents();
    EXPECT_EQ(g_isDestroyEvents, true);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent_023 end";
}
} // namespace Accessibility
} // namespace OHOS