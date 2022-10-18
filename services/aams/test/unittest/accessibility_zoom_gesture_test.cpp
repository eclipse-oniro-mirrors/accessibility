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
#include <map>
#include <memory>
#include "accessibility_ut_helper.h"
#include "accessibility_zoom_gesture.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t TRIPLE_TAP_COUNT = 3;
} // namespace
class AccessibilityZoomGestureUnitTest : public ::testing::Test {
public:
    AccessibilityZoomGestureUnitTest()
    {}
    ~AccessibilityZoomGestureUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<MMI::PointerEvent> CreatePointerEvent(int32_t sourceType, int32_t pointerAction);
    void TripleTaps();

    std::shared_ptr<AccessibilityZoomGesture> zoomGesture_ = nullptr;
};

void AccessibilityZoomGestureUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityZoomGestureUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityZoomGestureUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityZoomGestureUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityZoomGestureUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    zoomGesture_ = std::make_shared<AccessibilityZoomGesture>();
}

void AccessibilityZoomGestureUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    zoomGesture_ = nullptr;
}

std::shared_ptr<MMI::PointerEvent> AccessibilityZoomGestureUnitTest::CreatePointerEvent(int32_t sourceType,
    int32_t pointerAction)
{
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return nullptr;
    }
    event->SetSourceType(sourceType);
    event->SetPointerAction(pointerAction);
    return event;
}

void AccessibilityZoomGestureUnitTest::TripleTaps()
{
    if (!zoomGesture_) {
        return;
    }
    for (uint32_t count = 0; count < TRIPLE_TAP_COUNT; count++) {
        // Pointer down
        std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
            MMI::PointerEvent::POINTER_ACTION_DOWN);
        if (!eventDown) {
            return;
        }
        MMI::PointerEvent::PointerItem item;
        eventDown->AddPointerItem(item);
        zoomGesture_->OnPointerEvent(*eventDown);
        // Pointer up
        std::shared_ptr<MMI::PointerEvent> eventUp = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
            MMI::PointerEvent::POINTER_ACTION_UP);
        if (!eventUp) {
            return;
        }
        MMI::PointerEvent::PointerItem item1;
        eventUp->AddPointerItem(item1);
        zoomGesture_->OnPointerEvent(*eventUp);
    }
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_DOWN timeout)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_001 start";
    if (!zoomGesture_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    sleep(2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(Tap once on the screen)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_002 start";
    if (!zoomGesture_) {
        return;
    }
    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item;
    eventDown->AddPointerItem(item);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*eventDown);
    // Pointer up
    std::shared_ptr<MMI::PointerEvent> eventUp = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP);
    if (!eventUp) {
        return;
    }
    zoomGesture_->OnPointerEvent(*eventUp);
    sleep(2);

    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(3 Taps on the screen)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_003 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // stop zoom
    TripleTaps();
    zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_FALSE(zoomState);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(transfer to sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_004 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_005
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_MOVE in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_005 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    // Pointer move
    std::shared_ptr<MMI::PointerEvent> eventMove = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    if (!eventMove) {
        return;
    }
    MMI::PointerEvent::PointerItem item3;
    item3.SetDisplayX(0);
    item3.SetDisplayY(50);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item4;
    item4.SetDisplayX(70);
    item4.SetDisplayY(50);
    eventDown->AddPointerItem(item4);
    zoomGesture_->OnPointerEvent(*eventMove);
    bool scrollState = AccessibilityAbilityHelper::GetInstance().GetScrollState();
    EXPECT_TRUE(scrollState);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_006
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_DOWN in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_006 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    MMI::PointerEvent::PointerItem item3;
    item3.SetDisplayX(50);
    item3.SetDisplayY(25);
    eventDown->AddPointerItem(item1);
    zoomGesture_->OnPointerEvent(*eventDown);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_007
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_UP in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_007 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);

    MMI::PointerEvent::PointerItem item3;
    item3.SetPointerId(3);
    item3.SetDisplayX(50);
    item3.SetDisplayY(25);
    event->AddPointerItem(item3);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_007 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_008
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(sliding state->zoom state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_008 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    zoomGesture_->OnPointerEvent(*event);
    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    event->SetPointerId(2);
    event->RemovePointerItem(1);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_008 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_009
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in ready state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_009 start";
    if (!zoomGesture_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_009 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_010
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in zoom state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_010 start";
    if (!zoomGesture_) {
        return;
    }
    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // send cancel event
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_010 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_011
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_011 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer cancel
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_011 end";
}
} // namespace Accessibility
} // namespace OHOS