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
        MMI::PointerEvent::PointerItem item1;
        eventUp->AddPointerItem(item1);
        if (!eventUp) {
            return;
        }
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
} // namespace Accessibility
} // namespace OHOS