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
#include "accessibility_ability_helper.h"
#include "accessibility_mouse_key.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityMouseKeyUnitTest : public ::testing::Test {
public:
    AccessibilityMouseKeyUnitTest()
    {}
    ~AccessibilityMouseKeyUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityMouseKey> mouseKey_ = nullptr;
};

void AccessibilityMouseKeyUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityMouseKeyUnitTest Start ######################";
}

void AccessibilityMouseKeyUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityMouseKeyUnitTest End ######################";
}

void AccessibilityMouseKeyUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    mouseKey_ = std::make_shared<AccessibilityMouseKey>();
}

void AccessibilityMouseKeyUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    mouseKey_ = nullptr;
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_001 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_002 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(1 key down)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_001 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    event->AddKeyItem(item);
    mouseKey_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_002
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(1 key up)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_002 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    MMI::KeyEvent::KeyItem item;
    event->AddKeyItem(item);
    mouseKey_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_003
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '/', '5' key)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_003 start";
    if (!mouseKey_) {
        return;
    }
    // mouse event
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '/' key
    std::shared_ptr<MMI::KeyEvent> event1 = MMI::KeyEvent::Create();
    if (!event1) {
        return;
    }
    event1->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event1->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetPressed(true);
    event1->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event1);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnKeyEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_UP;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_003 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_004
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '-', '5' key)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_004 start";
    if (!mouseKey_) {
        return;
    }
    // mouse event
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '-' key
    std::shared_ptr<MMI::KeyEvent> event1 = MMI::KeyEvent::Create();
    if (!event1) {
        return;
    }
    event1->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT);
    event1->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetPressed(true);
    event1->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event1);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnKeyEvent(*event2); 
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_UP;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_004 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_005
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '*', '5' key)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_005 start";
    if (!mouseKey_) {
        return;
    }
    // mouse event
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '*' key
    std::shared_ptr<MMI::KeyEvent> event1 = MMI::KeyEvent::Create();
    if (!event1) {
        return;
    }
    event1->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY);
    event1->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetPressed(true);
    event1->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event1);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnKeyEvent(*event2); 
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_005 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_006
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '/', '+' key)
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_006 start";
    if (!mouseKey_) {
        return;
    }
    // mouse event
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '/' key
    std::shared_ptr<MMI::KeyEvent> event1 = MMI::KeyEvent::Create();
    if (!event1) {
        return;
    }
    event1->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event1->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetPressed(true);
    event1->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event1);

    // press '+' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_ADD);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnKeyEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_006 end";
}
} // namespace Accessibility
} // namespace OHOS