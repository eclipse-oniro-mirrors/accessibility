/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include <map>
#include <memory>
#include "accessibility_ut_helper.h"
#include "window_magnification_gesture.h"
#include "accessible_ability_manager_service.h"
#include "window_magnification_manager.h"
#include "magnification_menu_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t SLEEP_MS_2 = 2;
    constexpr int32_t SLEEP_MS_100 = 100;
    constexpr int32_t MOCK_STATE_ACTION = -3;
    constexpr int32_t POINT_ID_0 = 0;
    constexpr int32_t POINT_ID_1 = 1;
    constexpr uint32_t ONE_TAP_COUNT = 1;
    constexpr uint32_t TRIPLE_TAP_COUNT = 3;
}
class WindowMagnificationGestureTest : public testing::Test {
public:
    WindowMagnificationGestureTest()
    {}
    ~WindowMagnificationGestureTest()
    {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<MMI::PointerEvent> CreatePointerEvent(int32_t sourceType,
        int32_t pointerAction, int32_t displayX, int32_t displayY);
    std::shared_ptr<MMI::PointerEvent> CreatePointerEvent(int32_t pointerAction,
        std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId);
    void SetPointerItem(MMI::PointerEvent::PointerItem &point,
        int32_t id, int32_t x, int32_t y);
    void MultiTaps(uint32_t tapCount);
    std::shared_ptr<WindowMagnificationGesture> magnificationGesture_ = nullptr;
    std::shared_ptr<WindowMagnificationManager> manager_ = nullptr;
};

void WindowMagnificationGestureTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### WindowMagnificationGestureTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void WindowMagnificationGestureTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### WindowMagnificationGestureTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void WindowMagnificationGestureTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    manager_ = std::make_shared<WindowMagnificationManager>();
    std::shared_ptr<MagnificationMenuManager> menuManager = std::make_shared<MagnificationMenuManager>();
    magnificationGesture_ = std::make_shared<WindowMagnificationGesture>(manager_, menuManager);
}

void WindowMagnificationGestureTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    AccessibilityAbilityHelper::GetInstance().ClearEventTypeActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearZoom();
    magnificationGesture_ = nullptr;
}

std::shared_ptr<MMI::PointerEvent> WindowMagnificationGestureTest::CreatePointerEvent(int32_t sourceType,
    int32_t pointerAction, int32_t displayX, int32_t displayY)
{
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return nullptr;
    }
    event->SetSourceType(sourceType);
    event->SetPointerAction(pointerAction);
    MMI::PointerEvent::PointerItem item = {};
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    event->AddPointerItem(item);
    return event;
}

std::shared_ptr<MMI::PointerEvent> WindowMagnificationGestureTest::CreatePointerEvent(int32_t pointerAction,
    std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId)
{
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return nullptr;
    }

    for (auto &point : points) {
        event->AddPointerItem(point);
    }
    event->SetPointerId(pointerId);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(pointerAction);
    return event;
}

void WindowMagnificationGestureTest::SetPointerItem(MMI::PointerEvent::PointerItem &point,
    int32_t id, int32_t x, int32_t y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

void WindowMagnificationGestureTest::MultiTaps(uint32_t tapCount)
{
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    for (uint32_t count = 0; count < tapCount; count++) {
        // Pointer down
        std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
            MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
        EXPECT_TRUE(eventDown != nullptr);
        magnificationGesture_->OnPointerEvent(*eventDown);
        // Pointer up
        std::shared_ptr<MMI::PointerEvent> eventUp = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
            MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
        EXPECT_TRUE(eventUp != nullptr);
        magnificationGesture_->OnPointerEvent(*eventUp);
    }
}

/**
 * @tc.number: HandleReadyStateDown_001
 * @tc.name: HandleReadyStateDown
 * @tc.desc: Test fun HandleReadyStateDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_001 end";
}

/**
 * @tc.number: HandleReadyStateDown_002
 * @tc.name: HandleReadyStateDown
 * @tc.desc: Test fun HandleReadyStateDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_001 end";
}

/**
 * @tc.number: HandleReadyStateDown_003
 * @tc.name: HandleReadyStateDown
 * @tc.desc: Test fun HandleReadyStateDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_003 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateDown_003 end";
}

/**
 * @tc.number: HandleReadyStateUp_001
 * @tc.name: HandleReadyStateUp
 * @tc.desc: Test fun HandleReadyStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateUp_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateUp_001 end";
}

/**
 * @tc.number: HandleReadyStateUp_002
 * @tc.name: HandleReadyStateUp
 * @tc.desc: Test fun HandleReadyStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateUp_002 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_UP,
        points, POINT_ID_1);
    EXPECT_TRUE(event != nullptr);

    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateUp_002 end";
}

/**
 * @tc.number: HandleReadyStateMove_001
 * @tc.name: HandleReadyStateMove
 * @tc.desc: Test fun HandleReadyStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateMove_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateMove_001 end";
}

/**
 * @tc.number: HandleReadyStateCancel_001
 * @tc.name: HandleReadyStateCancel
 * @tc.desc: Test fun HandleReadyStateCancel
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateCancel_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateCancel_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateCancel_001 end";
}

/**
 * @tc.number: HandleReadyStateCancel_002
 * @tc.name: HandleReadyStateCancel
 * @tc.desc: Test fun HandleReadyStateCancel
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateCancel_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateCancel_002 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    manager_->ShowWindowMagnification();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateCancel_002 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStateDown_001
 * @tc.name: HandleReadyStateOneFingerDownStateDown
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStateDown
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_001 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStateDown_002
 * @tc.name: HandleReadyStateOneFingerDownStateDown
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStateDown
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_002 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, MOCK_STATE_ACTION);
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateDown_002 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStateUp_001
 * @tc.name: HandleReadyStateOneFingerDownStateUp
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_001 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStateUp_002
 * @tc.name: HandleReadyStateOneFingerDownStateUp
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStateUp
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_002 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    bool state = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(state);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    sleep(SLEEP_MS_2);
    MultiTaps(TRIPLE_TAP_COUNT);
    state = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_FALSE(state);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateUp_002 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStateMove_001
 * @tc.name: HandleReadyStateOneFingerDownStateMove
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateMove_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 100, 100);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStateMove_001 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStatCancel_001
 * @tc.name: HandleReadyStateOneFingerDownStatCancel
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStatCance
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_001 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerDownStatCancel_002
 * @tc.name: HandleReadyStateOneFingerDownStatCancel
 * @tc.desc: Test fun HandleReadyStateOneFingerDownStatCance
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_002 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::READY_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL, 10, 10);
    EXPECT_TRUE(event != nullptr);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    manager_->ShowWindowMagnification();
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    sleep(SLEEP_MS_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerDownStatCancel_002 end";
}

/**
 * @tc.number: HandleReadyStateOneFingerTapDown_001
 * @tc.name: HandleReadyStateOneFingerTapDown
 * @tc.desc: Test fun HandleReadyStateOneFingerTapDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerTapDown_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerTapDown_001 start";
    MultiTaps(ONE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_TAP));
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleReadyStateOneFingerTapDown_001 end";
}

/**
 * @tc.number: HandleZoomInStateDown_001
 * @tc.name: HandleZoomInStateDown
 * @tc.desc: Test fun HandleZoomInStateDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleZoomInStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateDown_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateDown_001 end";
}

/**
 * @tc.number: HandleZoomInStateUp_001
 * @tc.name: HandleZoomInStateUp
 * @tc.desc: Test fun HandleZoomInStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleZoomInStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateUp_001 end";
}

/**
 * @tc.number: HandleZoomInStateMove_001
 * @tc.name: HandleZoomInStateMove
 * @tc.desc: Test fun HandleZoomInStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleZoomInStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateMove_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 10, 10);
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateMove_001 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateDown_001
 * @tc.name: HandleZoomInStateOneFingerDownStateDown
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateDown
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event1 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event1); // down event 1
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> event2 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event2); // down event 2
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_001 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateDown_002
 * @tc.name: HandleZoomInStateOneFingerDownStateDown
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateDown
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event1 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event1);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMagnificationWindow(true);
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event2 = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    magnificationGesture_->OnPointerEvent(*event2); // two fingers down
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateDown_002 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateUp_001
 * @tc.name: HandleZoomInStateOneFingerDownStateUp
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateUp
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    bool state = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(state);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    sleep(SLEEP_MS_2);
    MultiTaps(TRIPLE_TAP_COUNT);
    state = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_FALSE(state);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateUp_001 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateMove_001
 * @tc.name: HandleZoomInStateOneFingerDownStateMove
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::PASSING_THROUGH));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_001 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateMove_002
 * @tc.name: HandleZoomInStateOneFingerDownStateMove
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_002 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMenu(true);
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::MENU_SLIDING_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_002 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerDownStateMove_003
 * @tc.name: HandleZoomInStateOneFingerDownStateMove
 * @tc.desc: Test fun HandleZoomInStateOneFingerDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest,
    WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_003 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    AccessibilityAbilityHelper::GetInstance().SetTapOnHotArea(true);
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::SLIDING_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerDownStateMove_003 end";
}

/**
 * @tc.number: HandleZoomInStateOneFingerTapDown_001
 * @tc.name: HandleZoomInStateOneFingerTapDown
 * @tc.desc: Test fun HandleZoomInStateOneFingerTapDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerTapDown_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerTapDown_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    sleep(SLEEP_MS_2);
    MultiTaps(ONE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_TAP));
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleZoomInStateOneFingerTapDown_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateUp_001
 * @tc.name: HandleTwoFingersDownStateUp
 * @tc.desc: Test fun HandleTwoFingersDownStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateUp_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event1 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event1);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMagnificationWindow(true);
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event2 = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    magnificationGesture_->OnPointerEvent(*event2); // two fingers down
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN));
    std::shared_ptr<MMI::PointerEvent> upEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    magnificationGesture_->OnPointerEvent(*upEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateUp_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateMove_001
 * @tc.name: HandleTwoFingersDownStateMove
 * @tc.desc: Test fun HandleTwoFingersDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateMove_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event1 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event1);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMagnificationWindow(true);
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event2 = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    magnificationGesture_->OnPointerEvent(*event2); // two fingers down
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::SLIDING_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateMove_001 end";
}

/**
 * @tc.number: HandleTwoFingersDownStateMove_002
 * @tc.name: HandleTwoFingersDownStateMove
 * @tc.desc: Test fun HandleTwoFingersDownStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateMove_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    std::shared_ptr<MMI::PointerEvent> event1 = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*event1);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMagnificationWindow(true);
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    SetPointerItem(point1, POINT_ID_0, 10, 10);
    MMI::PointerEvent::PointerItem point2 = {};
    SetPointerItem(point2, POINT_ID_1, 20, 20);
    points.emplace_back(point1);
    points.emplace_back(point2);
    std::shared_ptr<MMI::PointerEvent> event2 = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_DOWN,
        points, POINT_ID_1);
    magnificationGesture_->OnPointerEvent(*event2); // two fingers down
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_TWO_FINGERS_DOWN));
    points = {};
    SetPointerItem(point1, POINT_ID_0, 100, 100);
    SetPointerItem(point2, POINT_ID_1, 110, 110);
    points.emplace_back(point1);
    points.emplace_back(point2);
    event2 = CreatePointerEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_1);
    magnificationGesture_->OnPointerEvent(*event2); // two fingers down
    EXPECT_TRUE(AccessibilityAbilityHelper::GetInstance().GetZoomMoveState());
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleTwoFingersDownStateMove_002 end";
}

/**
 * @tc.number: HandleSlidingStateDown_001
 * @tc.name: HandleSlidingStateDown
 * @tc.desc: Test fun HandleSlidingStateDown
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleSlidingStateDown_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateDown_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::SLIDING_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    EXPECT_TRUE(event != nullptr);
    magnificationGesture_->OnPointerEvent(*event);
    sleep(SLEEP_MS_2);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::SLIDING_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateDown_001 end";
}

/**
 * @tc.number: HandleSlidingStateUp_001
 * @tc.name: HandleSlidingStateUp
 * @tc.desc: Test fun HandleSlidingStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleSlidingStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateUp_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::SLIDING_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    EXPECT_TRUE(event != nullptr);
    magnificationGesture_->OnPointerEvent(*event);
    sleep(SLEEP_MS_2);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateUp_001 end";
}

/**
 * @tc.number: HandleSlidingStateMove_001
 * @tc.name: HandleSlidingStateMove
 * @tc.desc: Test fun HandleSlidingStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleSlidingStateMove_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateMove_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::SLIDING_STATE, MOCK_STATE_ACTION);
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 10, 10);
    EXPECT_TRUE(event != nullptr);
    magnificationGesture_->OnPointerEvent(*event);
    sleep(SLEEP_MS_2);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::SLIDING_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleSlidingStateMove_001 end";
}

/**
 * @tc.number: HandleMenuSlidingStateUp_001
 * @tc.name: HandleMenuSlidingStateUp
 * @tc.desc: Test fun HandleMenuSlidingStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleMenuSlidingStateUp_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleMenuSlidingStateUp_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMenu(true);
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::MENU_SLIDING_STATE));
    std::shared_ptr<MMI::PointerEvent> upEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 200, 200);
    magnificationGesture_->OnPointerEvent(*upEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleMenuSlidingStateUp_001 end";
}

/**
 * @tc.number: HandleMenuSlidingStateMove_001
 * @tc.name: HandleMenuSlidingStateMove
 * @tc.desc: Test fun HandleMenuSlidingStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleMenuSlidingStateMove_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleMenuSlidingStateMove_001 start";
    MultiTaps(TRIPLE_TAP_COUNT);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE));
    AccessibilityAbilityHelper::GetInstance().SetTapOnMenu(true);
    std::shared_ptr<MMI::PointerEvent> downEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN, 10, 10);
    magnificationGesture_->OnPointerEvent(*downEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::ZOOMIN_STATE_ONE_FINGER_DOWN));
    std::shared_ptr<MMI::PointerEvent> moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 200, 200);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::MENU_SLIDING_STATE));
    moveEvent = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 250, 250);
    magnificationGesture_->OnPointerEvent(*moveEvent);
    EXPECT_TRUE(AccessibilityAbilityHelper::GetInstance().GetMenuMoveState());
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleMenuSlidingStateMove_001 end";
}

/**
 * @tc.number: HandleDraggingStateUp_001
 * @tc.name: HandleDraggingStateUp
 * @tc.desc: Test fun HandleDraggingStateUp
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleDraggingStateUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleDraggingStateUp_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::DRAGGING_STATE, MOCK_STATE_ACTION);
    manager_->ShowWindowMagnification();
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP, 10, 10);
    event->SetPointerId(POINT_ID_0);
    EXPECT_TRUE(event != nullptr);
    magnificationGesture_->OnPointerEvent(*event);
    sleep(SLEEP_MS_2);
    EXPECT_EQ(static_cast<uint32_t>(magnificationGesture_->GetGestureState()),
        static_cast<uint32_t>(MagnificationGestureState::READY_STATE));
    EXPECT_FALSE(AccessibilityAbilityHelper::GetInstance().GetZoomState());
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleDraggingStateUp_001 end";
}

/**
 * @tc.number: HandleDraggingStateMove_001
 * @tc.name: HandleDraggingStateMove
 * @tc.desc: Test fun HandleDraggingStateMove
 */
HWTEST_F(WindowMagnificationGestureTest, WindowMagnificationGesture_Unittest_HandleDraggingStateMove_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleDraggingStateMove_001 start";
    EXPECT_TRUE(magnificationGesture_ != nullptr);
    magnificationGesture_->SetGestureState(MagnificationGestureState::DRAGGING_STATE, MOCK_STATE_ACTION);
    manager_->ShowWindowMagnification();
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE, 100, 100);
    EXPECT_TRUE(event != nullptr);
    magnificationGesture_->OnPointerEvent(*event);
    EXPECT_FALSE(AccessibilityAbilityHelper::GetInstance().GetZoomMoveState());
    GTEST_LOG_(INFO) << "WindowMagnificationGesture_Unittest_HandleDraggingStateMove_001 end";
}
} // namespace Accessibility
} // namespace OHOS