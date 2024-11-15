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

#include "accessible_ability_manager_service_stub.h"
#include "accessibility_ability_info_parcel.h"
#include "accessibility_caption_parcel.h"
#include "accessibility_event_info_parcel.h"
#include "accessibility_ipc_interface_code.h"
#include "accesstoken_kit.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

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

class AccessibleAbilityManagerServiceStubTest : public testing::Test {
public:
    AccessibleAbilityManagerServiceStubTest()
    {}
    ~AccessibleAbilityManagerServiceStubTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    static void SetAccessibleAbilityManagerServiceStubPoint(MMI::PointerEvent::PointerItem &point,
        int id, int x, int y);
    static void AccessibleAbilityManagerServiceStubExpect(EventType eventType, int32_t GestureTypeInt);
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
    std::unique_ptr<AccessibleAbilityManagerServiceStub> AccessibleAbilityManagerServiceStub_ = nullptr;
    int32_t pointId_ = -1;
};

void AccessibleAbilityManagerServiceStubTest::PointerEventTest(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem movepoint2, MMI::PointerEvent::PointerItem movepoint3)
{
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(GET_EVENT_TARGET_INDEX_2);
    EXPECT_EQ(touchAction, expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
}

void AccessibleAbilityManagerServiceStubTest::AccessibleAbilityManagerServiceStubExpect(EventType eventType,
    int32_t GestureTypeInt)
{
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(GET_EVENT_TARGET_INDEX_2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(GET_EVENT_TARGET_INDEX_3);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    auto uTgestureId = AccessibilityAbilityHelper::GetInstance().GetGestureId();
    EXPECT_EQ(uTgestureId, GestureTypeInt);
}

void AccessibleAbilityManagerServiceStubTest::SetAccessibleAbilityManagerServiceStubPoint(
    MMI::PointerEvent::PointerItem &point, int id, int x, int y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

bool AccessibleAbilityManagerServiceStubTest::TestEventType()
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

bool AccessibleAbilityManagerServiceStubTest::TestEventAction()
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

void AccessibleAbilityManagerServiceStubTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStubTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibleAbilityManagerServiceStubTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStubTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibleAbilityManagerServiceStubTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStubTest SetUp";

    AccessibleAbilityManagerServiceStub_ = std::make_unique<AccessibleAbilityManagerServiceStub>();
    if (!AccessibleAbilityManagerServiceStub_) {
        GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStub new failed!";
        return;
    }
    AccessibleAbilityManagerServiceStub_->StartUp();
    pointId_ = 0;
}

void AccessibleAbilityManagerServiceStubTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStubTest TearDown";
    AccessibilityAbilityHelper::GetInstance().ClearEventTypeActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
}

std::shared_ptr<MMI::PointerEvent> AccessibleAbilityManagerServiceStubTest::CreateTouchEvent(int32_t action)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(pointId_);
    item.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> AccessibleAbilityManagerServiceStubTest::CreateTouchEvent(int32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId, int64_t occurredTime,
    int64_t startTime)
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
    pointerEvent->SetActionStartTime(startTime * MS_TO_US);
    pointerEvent->SetActionTime(occurredTime * MS_TO_US);

    return pointerEvent;
}

std::shared_ptr<MMI::PointerEvent> AccessibleAbilityManagerServiceStubTest::CreateMoveEvent(int32_t pointerCount)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(pointId_);
    item.SetPressed(true);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);

    return pointerEvent;
}

/**
 * @tc.number: ManagerServiceStub_Test_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first down event.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_001 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) == EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_001 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first up event.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_002 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_BEGIN);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_002 end";
}

/**
 * @tc.number: OnPointerEvent003
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that down-up slowly.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_003 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_003 end";
}

/**
 * @tc.number: OnPointerEvent004
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_004 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) == EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_004 end";
}

/**
 * @tc.number: OnPointerEvent005
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in touchGuiding state.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_005 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint, 2, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    bool retOnPointerEvent5 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent5);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    points.clear();
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_005 end";
}

/**
 * @tc.number: OnPointerEvent006
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_006 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint, 2, 10, 10);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint1, 3, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 3, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 3, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
            MMI::PointerEvent::POINTER_ACTION_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_006 end";
}

/**
 * @tc.number: OnPointerEvent007
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the CANCEL event.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_007 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_007 end";
}

/**
 * @tc.number: OnPointerEvent008
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the OTHER_POINT_DOWN event in draging state.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_008 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint, 2, 500, 500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint1, 3, 500, 500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    bool retOnPointerEvent8 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent8);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_3, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_008 end";
}

/**
 * @tc.number: OnPointerEvent009
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in draging state.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_009 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint, 2, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    bool retOnPointerEvent9 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent9);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    int32_t touchEventSize =
        static_cast<int32_t>(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size());
    EXPECT_EQ(touchEventSize, 0);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_009 end";
}

/**
 * @tc.number: OnPointerEvent010
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger moving in draging state.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_010 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint, 2, 500, 500);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(otherPoint1, 3, 500, 500);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_3, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
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

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_010 end";
}

/**
 * @tc.number: OnPointerEvent012
 * @tc.name: OnPointerEvent
 * @tc.desc: Check cancel the second move event after onstart.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_012 start";

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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t  expectValue= MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(othermovepoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 400, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_012 end";
}

/**
 * @tc.number: OnPointerEvent013
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_RIGHT_THEN_UP gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_013 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetAccessibleAbilityManagerServiceStubPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint1, 1, 3500, 2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint2, 1, 5000, 2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint3, 1, 4000, 0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    AccessibleAbilityManagerServiceStubExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_013 end";
}

/**
 * @tc.number: OnPointerEvent014
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_LEFT_THEN_UP gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_014 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetAccessibleAbilityManagerServiceStubPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint1, 1, 1500, 2500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint2, 1, 0, 2500);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint3, 1, 1000, 0);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    AccessibleAbilityManagerServiceStubExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_014 end";
}

/**
 * @tc.number: OnPointerEvent015
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_LEFT gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_015 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetAccessibleAbilityManagerServiceStubPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint1, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint2, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint3, 1, 0, 4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    AccessibleAbilityManagerServiceStubExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_015 end";
}

/**
 * @tc.number: OnPointerEvent016
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_RIGHT gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_016 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    SetAccessibleAbilityManagerServiceStubPoint(point, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem movepoint1 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint1, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem movepoint2 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint2, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem movepoint3 = {};
    SetAccessibleAbilityManagerServiceStubPoint(movepoint3, 1, 5000, 4000);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    PointerEventTest(points, movepoint2, movepoint3);

    AccessibleAbilityManagerServiceStubExpect(eventType, static_cast<int>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_016 end";
}

/**
 * @tc.number: OnPointerEvent017
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the LEFT gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_017 start";

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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);

    points.clear();
    points.emplace_back(movepoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);

    points.clear();
    points.emplace_back(movepoint2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    auto uTgestureId = AccessibilityAbilityHelper::GetInstance().GetGestureId();
    EXPECT_EQ(uTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_017 end";
}

/**
 * @tc.number: OnPointerEvent018
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the DoubleTap gesture.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_018 start";

    if (!AccessibleAbilityManagerServiceStub_) {
        GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceStub_ is null";
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
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    // send up event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 0, 0);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    // send down event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 200, 200);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);

    // send up event
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 1, 200, 200);
    AccessibleAbilityManagerServiceStub_->OnPointerEvent(*event);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetExecuteAction() == ACCESSIBILITY_ACTION_CLICK) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_GE(ret, 0);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_018 end";
}

/**
 * @tc.number: DestroyEvents
 * @tc.name: DestroyEvents
 * @tc.desc: Check DestroyEvents.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_019 start";

    AccessibleAbilityManagerServiceStub_->DestroyEvents();
    bool isDestroyEvents;
    isDestroyEvents = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents, true);

    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_019 end";
}

/**
 * @tc.number: StartUp001
 * @tc.name: StartUp
 * @tc.desc: Check the first down event.
 */
HWTEST_F(AccessibleAbilityManagerServiceStubTest, ManagerServiceStub_Test_020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_020 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    std::unique_ptr<AccessibleAbilityManagerServiceStub> AccessibleAbilityManagerServiceStub =
        std::make_unique<AccessibleAbilityManagerServiceStub>();
    AccessibleAbilityManagerServiceStub->StartUp();
    EXPECT_EQ(nullptr, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_020 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_021
 * @tc.name: ManagerServiceStub_Test_021
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_021 start";
    settingConfig_->Init();
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_021 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_022
 * @tc.name: SetEnabled
 * @tc.desc: Test function SetEnabled GetEnabledState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_022 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetEnabled(state);
    EXPECT_TRUE(settingConfig_->GetEnabledState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_022 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_023
 * @tc.name: SetEnabled
 * @tc.desc: Test function SetEnabled GetEnabledState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_023 start";
    bool state = true;
    settingConfig_->SetEnabled(state);
    EXPECT_FALSE(settingConfig_->GetEnabledState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_023 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_024
 * @tc.name: SetTouchGuideState
 * @tc.desc: Test function SetTouchGuideState GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_024 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetTouchGuideState(state);
    EXPECT_TRUE(settingConfig_->GetTouchGuideState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_024 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_025
 * @tc.name: SetTouchGuideState
 * @tc.desc: Test function SetTouchGuideState GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_025 start";
    bool state = true;
    settingConfig_->SetTouchGuideState(state);
    EXPECT_FALSE(settingConfig_->GetTouchGuideState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_025 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_026
 * @tc.name: SetGestureState
 * @tc.desc: Test function SetGestureState GetGestureState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_026, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_026 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetGestureState(state);
    EXPECT_TRUE(settingConfig_->GetGestureState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_026 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_027
 * @tc.name: SetGestureState
 * @tc.desc: Test function SetGestureState GetGestureState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_027, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_027 start";
    bool state = true;
    settingConfig_->SetGestureState(state);
    EXPECT_FALSE(settingConfig_->GetGestureState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_027 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_028
 * @tc.name: SetKeyEventObserverState
 * @tc.desc: Test function SetKeyEventObserverState GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_028,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_028 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetKeyEventObserverState(state);
    EXPECT_TRUE(settingConfig_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_028 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_029
 * @tc.name: SetKeyEventObserverState
 * @tc.desc: Test function SetKeyEventObserverState GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_029,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_029 start";
    bool state = true;
    settingConfig_->SetKeyEventObserverState(state);
    EXPECT_FALSE(settingConfig_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_029 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_030
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_030, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_030 start";
    AccessibilityConfig::CaptionProperty caption;
    settingConfig_->Init();
    settingConfig_->SetCaptionProperty(caption);
    settingConfig_->GetCaptionProperty();
    EXPECT_NE(settingConfig_.get(), nullptr);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_030 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_031
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_031, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_031 start";
    AccessibilityConfig::CaptionProperty caption;
    settingConfig_->SetCaptionProperty(caption);
    settingConfig_->GetCaptionProperty();
    EXPECT_NE(settingConfig_.get(), nullptr);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_031 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_032
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_032, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_032 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetCaptionState(state);
    EXPECT_TRUE(settingConfig_->GetCaptionState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_032 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_033
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_033, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_033 start";
    bool state = true;
    settingConfig_->SetCaptionState(state);
    EXPECT_FALSE(settingConfig_->GetCaptionState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_033 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_034
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_034, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_034 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetScreenMagnificationState(state);
    EXPECT_TRUE(settingConfig_->GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_034 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_035
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_035, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_035 start";
    bool state = true;
    settingConfig_->SetScreenMagnificationState(state);
    EXPECT_TRUE(settingConfig_->GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_035 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_036
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_036, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_036 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetShortKeyState(state);
    EXPECT_TRUE(settingConfig_->GetShortKeyState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_036 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_037
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_037, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_037 start";
    bool state = true;
    settingConfig_->SetShortKeyState(state);
    EXPECT_FALSE(settingConfig_->GetShortKeyState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_037 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_038
 * @tc.name: SetShortKeyOnLockScreenState
 * @tc.desc: Test function SetShortKeyOnLockScreenState GetShortKeyOnLockScreenState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_038, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_038 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetShortKeyOnLockScreenState(state);
    EXPECT_TRUE(settingConfig_->GetShortKeyOnLockScreenState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_038 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_039
 * @tc.name: SetShortKeyOnLockScreenState
 * @tc.desc: Test function SetShortKeyOnLockScreenState GetShortKeyOnLockScreenState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_039, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_039 start";
    bool state = true;
    settingConfig_->SetShortKeyOnLockScreenState(state);
    EXPECT_FALSE(settingConfig_->GetShortKeyOnLockScreenState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_039 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_040
 * @tc.name: SetShortKeyTimeout
 * @tc.desc: Test function SetShortKeyTimeout GetShortKeyTimeout
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_040, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_040 start";
    settingConfig_->Init();
    settingConfig_->SetShortKeyTimeout(SHORT_KEY_TIMEOUT);
    EXPECT_TRUE(settingConfig_->GetShortKeyTimeout() == SHORT_KEY_TIMEOUT);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_040 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_041
 * @tc.name: SetShortKeyTimeout
 * @tc.desc: Test function SetShortKeyTimeout GetShortKeyTimeout
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_041, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_041 start";
    settingConfig_->SetShortKeyTimeout(SHORT_KEY_TIMEOUT);
    EXPECT_FALSE(settingConfig_->GetShortKeyTimeout() == SHORT_KEY_TIMEOUT);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_041 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_042
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_042, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_042 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetMouseKeyState(state);
    EXPECT_TRUE(settingConfig_->GetMouseKeyState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_042 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_043
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_043, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_043 start";
    bool state = true;
    settingConfig_->SetMouseKeyState(state);
    EXPECT_FALSE(settingConfig_->GetMouseKeyState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_043 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_044
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_044, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_044 start";
    settingConfig_->Init();
    settingConfig_->SetMouseAutoClick(MOUSE_AUTO_CLICK_VALUE);
    EXPECT_EQ(MOUSE_AUTO_CLICK_VALUE, settingConfig_->GetMouseAutoClick());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_044 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_045
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_045, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_045 start";
    settingConfig_->Init();
    settingConfig_->SetMouseAutoClick(MOUSE_AUTO_CLICK_VALUE);
    EXPECT_EQ(-1, settingConfig_->GetMouseAutoClick());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_045 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_046
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_046, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_046 start";
    std::string name = "TEST";
    settingConfig_->Init();
    settingConfig_->SetShortkeyTarget(name);
    EXPECT_STREQ("TEST", settingConfig_->GetShortkeyTarget().c_str());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_046 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_047
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_047, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_047 start";
    std::string name = "TEST";
    settingConfig_->Init();
    settingConfig_->SetShortkeyTarget(name);
    EXPECT_STREQ("", settingConfig_->GetShortkeyTarget().c_str());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_047 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_048
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_048, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_048 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetHighContrastTextState(state);
    EXPECT_TRUE(settingConfig_->GetHighContrastTextState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_048 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_049
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_049, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_049 start";
    bool state = true;
    settingConfig_->SetHighContrastTextState(state);
    EXPECT_FALSE(settingConfig_->GetHighContrastTextState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_049 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_050
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_050, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_050 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetDaltonizationState(state);
    EXPECT_TRUE(settingConfig_->GetDaltonizationState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_050 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_051
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_051, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_051 start";
    bool state = true;
    settingConfig_->SetDaltonizationState(state);
    EXPECT_FALSE(settingConfig_->GetDaltonizationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_DaltonizationState_002 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_052
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_052, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_052 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetInvertColorState(state);
    EXPECT_TRUE(settingConfig_->GetInvertColorState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_052 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_053
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_053, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_053 start";
    bool state = true;
    settingConfig_->SetInvertColorState(state);
    EXPECT_FALSE(settingConfig_->GetInvertColorState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_053 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_054
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_054, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_054 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetAnimationOffState(state);
    EXPECT_TRUE(settingConfig_->GetAnimationOffState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_054 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_055
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_055, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_055 start";
    bool state = true;
    settingConfig_->SetAnimationOffState(state);
    EXPECT_FALSE(settingConfig_->GetAnimationOffState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_055 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_056
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_056, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_056 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetAudioMonoState(state);
    EXPECT_TRUE(settingConfig_->GetAudioMonoState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_056 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_057
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_057, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_057 start";
    bool state = true;
    settingConfig_->SetAudioMonoState(state);
    EXPECT_FALSE(settingConfig_->GetAudioMonoState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_057 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_058
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_058, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_058 start";
    uint32_t filter = 1;
    settingConfig_->Init();
    settingConfig_->SetDaltonizationColorFilter(filter);
    EXPECT_TRUE(settingConfig_->GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_058 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_059
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_059, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_059 start";
    uint32_t filter = 1;
    settingConfig_->SetDaltonizationColorFilter(filter);
    EXPECT_FALSE(settingConfig_->GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_059 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_060
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_060, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_060 start";
    settingConfig_->Init();
    settingConfig_->SetContentTimeout(CONTENT_TIMEOUT_VALUE);
    EXPECT_EQ(CONTENT_TIMEOUT_VALUE, settingConfig_->GetContentTimeout());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_060 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_061
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_061, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_061 start";
    settingConfig_->SetContentTimeout(CONTENT_TIMEOUT_VALUE);
    EXPECT_NE(CONTENT_TIMEOUT_VALUE, settingConfig_->GetContentTimeout());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_061 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_062
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_062, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_062 start";
    settingConfig_->Init();
    settingConfig_->SetBrightnessDiscount(BRIGHTNESS_DISCOUNT_VALUE);
    EXPECT_EQ(BRIGHTNESS_DISCOUNT_VALUE, settingConfig_->GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_062 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_063
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_063, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_063 start";
    settingConfig_->SetBrightnessDiscount(BRIGHTNESS_DISCOUNT_VALUE);
    EXPECT_NE(BRIGHTNESS_DISCOUNT_VALUE, settingConfig_->GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_063 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_064
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_064, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_064 start";
    settingConfig_->Init();
    settingConfig_->SetAudioBalance(AUDIO_BALANCE_VALUE);
    EXPECT_EQ(AUDIO_BALANCE_VALUE, settingConfig_->GetAudioBalance());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_064 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_065
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_065, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_065 start";
    settingConfig_->SetAudioBalance(AUDIO_BALANCE_VALUE);
    EXPECT_NE(AUDIO_BALANCE_VALUE, settingConfig_->GetAudioBalance());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_065 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_066
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    ManagerServiceStub_Test_066, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_066 start";
    settingConfig_->Init();
    settingConfig_->SetClickResponseTime(1);
    EXPECT_EQ(1, settingConfig_->GetClickResponseTime());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_066 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_067
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_067, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_067 start";
    settingConfig_->SetClickResponseTime(1);
    EXPECT_NE(1, settingConfig_->GetClickResponseTime());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_067 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_068
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_068, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_068 start";
    settingConfig_->Init();
    settingConfig_->SetIgnoreRepeatClickState(true);
    EXPECT_EQ(true, settingConfig_->GetIgnoreRepeatClickState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_068 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_069
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_069, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_069 start";
    settingConfig_->SetIgnoreRepeatClickState(true);
    EXPECT_NE(true, settingConfig_->GetIgnoreRepeatClickState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_069 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_070
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_070, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_070 start";
    settingConfig_->Init();
    settingConfig_->SetIgnoreRepeatClickTime(1);
    EXPECT_EQ(1, settingConfig_->GetIgnoreRepeatClickTime());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_070 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_071
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_071, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_071 start";
    settingConfig_->SetIgnoreRepeatClickTime(1);
    EXPECT_NE(1, settingConfig_->GetIgnoreRepeatClickTime());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_071 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_072
 * @tc.name: GetConfigState
 * @tc.desc: Test function GetConfigState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_072, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_072 start";
    uint32_t state = 0x00000000;
    EXPECT_EQ(state, settingConfig_->GetConfigState());
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_072 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_073
 * @tc.name: ClearData
 * @tc.desc: Test function ClearData
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_073, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_073 start";
    settingConfig_->Init();
    settingConfig_->ClearData();
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_073 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_074
 * @tc.name: SetShortkeyMultiTargetInPkgRemove
 * @tc.desc: Test function SetShortkeyMultiTargetInPkgRemove
 */
HWTEST_F(AccessibilitySettingsConfigTest, Unittest_SetShortkeyMultiTargetInPkgRemove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_074 start";
    settingConfig_->Init();
    std::string name = "TEST";
    settingConfig_->SetShortkeyMultiTargetInPkgRemove(name);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_074 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_075
 * @tc.name: SetStartToHosState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_075, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_075 start";
    bool state = true;
    int32_t ret = settingConfig_->SetStartToHosState(state);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_075 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_076
 * @tc.name: SetShortkeyMultiTarget
 * @tc.desc: Test function SetShortkeyMultiTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_076, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_076 start";
    const std::vector<std::string> name;
    int32_t ret = settingConfig_->SetShortkeyMultiTarget(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_076 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_077
 * @tc.name: SetShortkeyMultiTarget
 * @tc.desc: Test function SetShortkeyMultiTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_077, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_077 start";
    const std::vector<std::string> name = {"test"};
    int32_t ret = settingConfig_->SetShortkeyMultiTarget(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_077 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_078
 * @tc.name: SetShortkeyMultiTargetInPkgRemove
 * @tc.desc: Test function SetShortkeyMultiTargetInPkgRemove
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, ManagerServiceStub_Test_078, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_078 start";
    const std::string name = "test";
    int32_t ret = settingConfig_->SetShortkeyMultiTargetInPkgRemove(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_078 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_078
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_078, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_078 start";
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    RetError result = Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value);
    EXPECT_EQ(RET_ERR_NULLPTR, result);
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(ret));
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_078 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_079
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_079, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_079 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(AUDIO_BALANCE_VALUE));
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(ret));
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_079 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_080
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_080, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_080 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetClickResponseTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetClickResponseTime(ret));
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_080 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_081
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_081, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_081 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickState(true));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickState(ret));
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_081 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_082
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_082, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_082 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickTime(ret));
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_082 end";
}

/**
 * @tc.number: ManagerServiceStub_Test_083
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, ManagerServiceStub_Test_083, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_083 start";
    AccessibilityConfigData data;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(data);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "ManagerServiceStub_Test_083 end";
}
} // namespace Accessibility
} // namespace OHOS