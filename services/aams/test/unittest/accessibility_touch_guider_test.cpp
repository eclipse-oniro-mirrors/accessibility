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
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_touch_guider.h"
#include "accessibility_ut_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t GET_EVENT_TARGET_INDEX_2 = 2;
    constexpr uint32_t GET_EVENT_TARGET_INDEX_3 = 3;
    constexpr uint32_t SLEEP_TIME_3 = 3;
    constexpr int64_t MS_TO_US = 1000;
    constexpr int32_t ACCOUNT_ID = 100;
    constexpr int32_t WINDOW_ID = 2;
    constexpr int32_t POINT_ID_1 = 1;
    constexpr int32_t POINT_ID_2 = 2;
    constexpr int32_t POINT_ID_3 = 3;
} // namespace

class TouchGuiderTest : public testing::Test {
public:
    TouchGuiderTest()
    {}
    ~TouchGuiderTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    static void SetTouchGuiderPoint(MMI::PointerEvent::PointerItem &point, int id, int x, int y);
    static void TouchGuiderExpect(EventType eventType, int32_t GestureTypeInt);
    void PointerEventTest(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem movepoint2, MMI::PointerEvent::PointerItem movepoint3);
    bool TestEventType();
    bool TestEventAction();
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action,
        std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerCount, int64_t occurredTime,
        int64_t startTime);
    std::shared_ptr<MMI::PointerEvent> CreateMoveEvent(int32_t pointerCount);
    std::unique_ptr<TouchGuider> touchGuider_ = nullptr;
    int32_t pointId_ = -1;
};

void TouchGuiderTest::PointerEventTest(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem movepoint2, MMI::PointerEvent::PointerItem movepoint3)
{
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(GET_EVENT_TARGET_INDEX_2);
    EXPECT_EQ(touchAction, expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
}

void TouchGuiderTest::TouchGuiderExpect(EventType eventType, int32_t GestureTypeInt)
{
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(GET_EVENT_TARGET_INDEX_2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(GET_EVENT_TARGET_INDEX_3);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    auto uTgestureId = AccessibilityAbilityHelper::GetInstance().GetGestureId();
    EXPECT_EQ(uTgestureId, GestureTypeInt);
}

void TouchGuiderTest::SetTouchGuiderPoint(MMI::PointerEvent::PointerItem &point, int id, int x, int y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

bool TouchGuiderTest::TestEventType()
{
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) ==
            EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    return ret;
}

bool TouchGuiderTest::TestEventAction()
{
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    return ret;
}

void TouchGuiderTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void TouchGuiderTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TouchGuiderTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
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
    AccessibilityAbilityHelper::GetInstance().ClearEventTypeActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateTouchEvent(int32_t action)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateTouchEvent(int32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId, int64_t occurredTime,
    int64_t startTime)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    for (auto &point : points) {
        pointerEvent->AddPointerItem(point);
    }
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionStartTime(startTime * MS_TO_US);
    pointerEvent->SetActionTime(occurredTime * MS_TO_US);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> TouchGuiderTest::CreateMoveEvent(int32_t pointerCount)
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
 * @tc.number: OnPointerEvent001
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first down event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_001 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) == EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: OnPointerEvent002
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first up event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_002 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) == EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);

    // create
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_END);
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: OnPointerEvent003
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that down-up slowly.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_003 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: OnPointerEvent004
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_004 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);

    event = CreateMoveEvent(1);
    touchGuider_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) == EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: OnPointerEvent005
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in touchGuiding state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_005 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent5 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent5);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
    if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
        MMI::PointerEvent::POINTER_ACTION_DOWN) {
        return true;
    } else {
        return false;
    }
    }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);

    points.clear();
    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    points.clear();
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        int32_t eventTypeSize =
            static_cast<int32_t>(AccessibilityAbilityHelper::GetInstance().GetEventTypeVector().size());
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(eventTypeSize - 1) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: OnPointerEvent006
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_006 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 10, 10);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetTouchGuiderPoint(otherPoint1, 3, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) ==
            EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
            MMI::PointerEvent::POINTER_ACTION_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: OnPointerEvent007
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the CANCEL event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_007 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent7 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent7);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    retOnPointerEvent7 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent7);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    touchGuider_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_007 end";
}

/**
 * @tc.number: OnPointerEvent008
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the OTHER_POINT_DOWN event in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_008 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 500, 500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetTouchGuiderPoint(otherPoint1, 3, 500, 500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent8 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent8);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_008 end";
}

/**
 * @tc.number: OnPointerEvent009
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_009 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent9 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent9);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent9 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
    if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
        MMI::PointerEvent::POINTER_ACTION_DOWN) {
        return true;
    } else {
        return false;
    }
    }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent9);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    int32_t touchEventSize =
        static_cast<int32_t>(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size());
    EXPECT_EQ(touchEventSize, 0);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_009 end";
}

/**
 * @tc.number: OnPointerEvent010
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger moving in draging state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_010 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 500, 500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetTouchGuiderPoint(otherPoint1, 3, 500, 500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent10 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) ==
            EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent10);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent10 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1) !=
            MMI::PointerEvent::POINTER_ACTION_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent10);
    int32_t expectValue = MMI::PointerEvent::BUTTON_NONE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_010 end";
}

/**
 * @tc.number: OnPointerEvent012
 * @tc.name: OnPointerEvent
 * @tc.desc: Check cancel the second move event after onstart.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_012 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetDisplayX(500);
    point.SetDisplayY(500);
    MMI::PointerEvent::PointerItem movepoint = {};
    movepoint.SetPointerId(1);
    movepoint.SetDisplayX(2500);
    movepoint.SetDisplayY(500);
    MMI::PointerEvent::PointerItem othermovepoint = {};
    othermovepoint.SetPointerId(1);
    othermovepoint.SetDisplayX(2500);
    othermovepoint.SetDisplayY(500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t  expectValue= MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(othermovepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 400, 0);
    touchGuider_->OnPointerEvent(*event);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_012 end";
}

/**
 * @tc.number: OnPointerEvent013
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_RIGHT_THEN_UP gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_013 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchGuiderPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetTouchGuiderPoint(movepoint1, 1, 3500, 2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetTouchGuiderPoint(movepoint2, 1, 5000, 2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetTouchGuiderPoint(movepoint3, 1, 4000, 0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    TouchGuiderExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_013 end";
}

/**
 * @tc.number: OnPointerEvent014
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_LEFT_THEN_UP gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_014 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchGuiderPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetTouchGuiderPoint(movepoint1, 1, 1500, 2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetTouchGuiderPoint(movepoint2, 1, 0, 2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetTouchGuiderPoint(movepoint3, 1, 1000, 0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    TouchGuiderExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_014 end";
}

/**
 * @tc.number: OnPointerEvent015
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_015 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchGuiderPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetTouchGuiderPoint(movepoint1, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetTouchGuiderPoint(movepoint2, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetTouchGuiderPoint(movepoint3, 1, 0, 4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    TouchGuiderExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_015 end";
}

/**
 * @tc.number: OnPointerEvent016
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_RIGHT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_016 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetTouchGuiderPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetTouchGuiderPoint(movepoint1, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetTouchGuiderPoint(movepoint2, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetTouchGuiderPoint(movepoint3, 1, 5000, 4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    TouchGuiderExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_016 end";
}

/**
 * @tc.number: OnPointerEvent017
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the LEFT gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_017 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetDisplayX(2500);
    point.SetDisplayY(2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    movepoint1.SetPointerId(1);
    movepoint1.SetDisplayX(1000);
    movepoint1.SetDisplayY(2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    movepoint2.SetPointerId(1);
    movepoint2.SetDisplayX(0);
    movepoint2.SetDisplayY(2500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    auto uTgestureId = AccessibilityAbilityHelper::GetInstance().GetGestureId();
    EXPECT_EQ(uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_017 end";
}

/**
 * @tc.number: OnPointerEvent018
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the DoubleTap gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_018 start";

    if (!touchGuider_) {
        GTEST_LOG_(INFO) << "touchGuider_ is null";
        return;
    }

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }

    AccessibilityAbilityHelper::GetInstance().SetRealId(WINDOW_ID);
    sptr<IAccessibilityElementOperator> proxy = new(std::nothrow) AccessibilityElementOperatorProxy(nullptr);
    sptr<AccessibilityWindowConnection> windowConnection =
        new(std::nothrow) AccessibilityWindowConnection(WINDOW_ID, proxy, ACCOUNT_ID);
    accountData->AddAccessibilityWindowConnection(WINDOW_ID, windowConnection);

    // send down event
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    point.SetDisplayX(2500);
    point.SetDisplayY(2500);
    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    // send up event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    // send down event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 200, 200);
    touchGuider_->OnPointerEvent(*event);

    // send up event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 200, 200);
    touchGuider_->OnPointerEvent(*event);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetExecuteAction() == ACCESSIBILITY_ACTION_CLICK) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_GE(ret, 0);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_018 end";
}

/**
 * @tc.number: DestroyEvents
 * @tc.name: DestroyEvents
 * @tc.desc: Check DestroyEvents.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_DestroyEvents_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_DestroyEvents_001 start";

    touchGuider_->DestroyEvents();
    bool isDestroyEvents;
    isDestroyEvents = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents, true);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_DestroyEvents_001 end";
}

/**
 * @tc.number: StartUp001
 * @tc.name: StartUp
 * @tc.desc: Check the first down event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_StartUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_StartUp_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    std::unique_ptr<TouchGuider> touchGuider = std::make_unique<TouchGuider>();
    touchGuider->StartUp();
    EXPECT_EQ(nullptr, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_StartUp_001 end";
}
} // namespace Accessibility
} // namespace OHOS