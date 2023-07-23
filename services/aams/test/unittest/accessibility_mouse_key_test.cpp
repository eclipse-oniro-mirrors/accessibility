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
#include "accessibility_mouse_key.h"
#include "accessibility_ut_helper.h"

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
    void OnKeyEventPresss(MMI::KeyEvent &event, const int32_t keycode);
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

void AccessibilityMouseKeyUnitTest::OnKeyEventPresss(MMI::KeyEvent &event, const int32_t keycode)
{
    // presss
    event.SetKeyCode(keycode);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keycode);
    item.SetPressed(true);
    event.AddKeyItem(item);
    mouseKey_->OnKeyEvent(event);
    event.RemoveReleasedKeyItems(item);
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_001 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_001 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_002 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_002 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(1~4,6~9 key down)
 * @tc.require: issueI5NTXA
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
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_1);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_2);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_3);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_4);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_6);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_7);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_8);
    OnKeyEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_9);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_002
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(1 key up)
 * @tc.require: issueI5NTXA
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
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->AddKeyItem(item);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_003
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '/', '5' key)
 * @tc.require: issueI5NTXA
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
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_003 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '/' key
    std::shared_ptr<MMI::KeyEvent> event3 = MMI::KeyEvent::Create();
    if (!event3) {
        return;
    }
    event3->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event3->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    item1.SetPressed(true);
    event3->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event3);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item3;
    item3.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item3.SetPressed(true);
    event2->AddKeyItem(item3);
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
 * @tc.require: issueI5NTXA
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
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_004 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '-' key
    std::shared_ptr<MMI::KeyEvent> event4 = MMI::KeyEvent::Create();
    if (!event4) {
        return;
    }
    event4->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT);
    event4->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT);
    item1.SetPressed(true);
    event4->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event4);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item4;
    item4.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item4.SetPressed(true);
    event2->AddKeyItem(item4);
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
 * @tc.require: issueI5NTXA
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
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_005 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '*' key
    std::shared_ptr<MMI::KeyEvent> event5 = MMI::KeyEvent::Create();
    if (!event5) {
        return;
    }
    event5->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY);
    event5->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY);
    item1.SetPressed(true);
    event5->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event5);

    // press '5' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item5;
    item5.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item5.SetPressed(true);
    event2->AddKeyItem(item5);
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
 * @tc.require: issueI5NTXA
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
        GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_006 event is null";
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->OnPointerEvent(*event);

    // press '/' key
    std::shared_ptr<MMI::KeyEvent> event6 = MMI::KeyEvent::Create();
    if (!event6) {
        return;
    }
    event6->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event6->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    item1.SetPressed(true);
    event6->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event6);

    // press '+' key
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    if (!event2) {
        return;
    }
    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_ADD);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_ADD);
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnKeyEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_006 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_007
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press 1->left shift in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_007 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    // presss 1
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    item.SetPressed(true);
    event->AddKeyItem(item);
    // presss left shift
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_007 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_008
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press left crtl->2 in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_008 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_2);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    // presss left ctrl
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    // presss 2
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_2);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_008 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_009
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press 3->left shift->right shift in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_009 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_3);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    // presss 1
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_3);
    item.SetPressed(true);
    event->AddKeyItem(item);
    // presss left shift
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    // presss right shift
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_009 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_010
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press left ctrl->4->right ctrl in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_010 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_4);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    // presss left ctrl
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    // presss 4
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_4);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    // presss right ctrl
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_010 end";
}

/**
 * @tc.number: AccessibilityMouseKey_Unittest_OnKeyEvent_011
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press left ctrl->6->right shift in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityMouseKeyUnitTest, AccessibilityMouseKey_Unittest_OnKeyEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_011 start";
    if (!mouseKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_6);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    // presss left ctrl
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    // presss 4
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_6);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    // presss right shift
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnKeyEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseKey_Unittest_OnKeyEvent_011 end";
}
} // namespace Accessibility
} // namespace OHOS