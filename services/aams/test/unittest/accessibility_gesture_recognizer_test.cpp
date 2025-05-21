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
#include "accessibility_gesture_recognizer.h"
#include "hilog_wrapper.h"
#include <cinttypes>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessAccessibilityGestureRecognizerTest : public ::testing::Test {
public:
    AccessAccessibilityGestureRecognizerTest()
    {}
    ~AccessAccessibilityGestureRecognizerTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void OnPointerEventPresss(MMI::KeyEvent &event, const int32_t keycode);
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessAccessibilityGestureRecognizer> mouseKey_ = nullptr;
};

void AccessAccessibilityGestureRecognizerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessAccessibilityGestureRecognizerTest Start ######################";
}

void AccessAccessibilityGestureRecognizerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessAccessibilityGestureRecognizerTest End ######################";
}

void AccessAccessibilityGestureRecognizerTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    mouseKey_ = std::make_shared<AccessAccessibilityGestureRecognizer>();
}

void AccessAccessibilityGestureRecognizerTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    mouseKey_ = nullptr;
}

void AccessAccessibilityGestureRecognizerTest::OnPointerEventPresss(MMI::KeyEvent &event, const int32_t keycode)
{
    // presss
    event.SetKeyCode(keycode);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keycode);
    item.SetPressed(true);
    event.AddKeyItem(item);
    mouseKey_->OnPointerEvent(event);
    event.RemoveReleasedKeyItems(item);
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_001
 * @tc.name: GetDoubleTapMoveThreshold
 * @tc.desc: Test function GetDoubleTapMoveThreshold
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_001 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_001 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_002
 * @tc.name: GetDoubleTapMoveThreshold
 * @tc.desc: Test function GetDoubleTapMoveThreshold
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_002 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_GetDoubleTapMoveThreshold_002 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_001 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_1);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_2);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_3);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_4);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_6);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_7);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_8);
    OnPointerEventPresss(*event, MMI::KeyEvent::KEYCODE_NUMPAD_9);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_002 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->AddKeyItem(item);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_003 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    std::shared_ptr<MMI::KeyEvent> event3 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event3);

    event3->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event3->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    item1.SetPressed(true);
    event3->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event3);
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event2);

    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item3;
    item3.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item3.SetPressed(true);
    event2->AddKeyItem(item3);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnPointerEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_UP;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_004 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    std::shared_ptr<MMI::KeyEvent> event4 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event4);

    event4->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT);
    event4->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT);
    item1.SetPressed(true);
    event4->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event4);
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event2);

    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item4;
    item4.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item4.SetPressed(true);
    event2->AddKeyItem(item4);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnPointerEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_UP;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_005
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_005 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    std::shared_ptr<MMI::KeyEvent> event5 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event5);

    event5->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY);
    event5->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY);
    item1.SetPressed(true);
    event5->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event5);
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event2);

    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item5;
    item5.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_5);
    item5.SetPressed(true);
    event2->AddKeyItem(item5);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnPointerEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_006
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_006 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_TRUE(event);

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseKey_->GetDoubleTapMoveThreshold(*event);
    std::shared_ptr<MMI::KeyEvent> event6 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event6);

    event6->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    event6->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE);
    item1.SetPressed(true);
    event6->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event6);
    std::shared_ptr<MMI::KeyEvent> event2 = MMI::KeyEvent::Create();
    EXPECT_TRUE(event2);

    event2->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_ADD);
    event2->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_ADD);
    item2.SetPressed(true);
    event2->AddKeyItem(item2);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnPointerEvent(*event2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN;
    EXPECT_EQ(touchAction, expectValue);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_007
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_007 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    item.SetPressed(true);
    event->AddKeyItem(item);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_007 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_008
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_008 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_2);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_2);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_008 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_009
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_009 start";
    EXPECT_TRUE(mouseKey_);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_3);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_3);
    item.SetPressed(true);
    event->AddKeyItem(item);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_009 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_010
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_010 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_4);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_4);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_010 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_011
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_011 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_6);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    item.SetPressed(true);
    event->AddKeyItem(item);
    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_6);
    item1.SetPressed(true);
    event->AddKeyItem(item1);
    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    item2.SetPressed(true);
    event->AddKeyItem(item2);
    mouseKey_->OnPointerEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_011 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_012
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_012 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->AddKeyItem(item);
    EXPECT_EQ(false, mouseKey_->OnPointerEvent(*event));
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_012 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_013
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerTest, AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_013 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

    MMI::KeyEvent::KeyItem item;
    event->AddKeyItem(item);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    mouseKey_->OnPointerEvent(*event);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_OnPointerEvent_013 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_001
 * @tc.name: HandleTouchMoveEvent
 * @tc.desc: Test function HandleTouchMoveEvent
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_001 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

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
    mouseKey_->HandleTouchMoveEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_001 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_002
 * @tc.name: HandleTouchMoveEvent
 * @tc.desc: Test function HandleTouchMoveEvent(press left crtl->2 in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_002 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

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
    mouseKey_->HandleTouchMoveEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_002 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_003
 * @tc.name: HandleTouchMoveEvent
 * @tc.desc: Test function HandleTouchMoveEvent(press 3->left shift->right shift in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_003 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

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
    mouseKey_->HandleTouchMoveEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_003 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_004
 * @tc.name: HandleTouchMoveEvent
 * @tc.desc: Test function HandleTouchMoveEvent(press left ctrl->4->right ctrl in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_004 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

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
    mouseKey_->HandleTouchMoveEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_004 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_005
 * @tc.name: HandleTouchMoveEvent
 * @tc.desc: Test function HandleTouchMoveEvent(press left ctrl->6->right shift in same time)
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_005 start";
    EXPECT_TRUE(mouseKey_);
    
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_TRUE(event);

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
    mouseKey_->HandleTouchMoveEvent(*event);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_HandleTouchMoveEvent_005 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_001
 * @tc.name: StandardGestureRecognizer
 * @tc.desc: Test function StandardGestureRecognizer
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_001 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_FALSE(AccessAccessibilityGestureRecognizer_->StandardGestureRecognizer(*event));

    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_001 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_002
 * @tc.name: StandardGestureRecognizer
 * @tc.desc: Test function StandardGestureRecognizer
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_002 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    EXPECT_TRUE(AccessAccessibilityGestureRecognizer_->StandardGestureRecognizer(*event));

    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_002 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_003
 * @tc.name: StandardGestureRecognizer
 * @tc.desc: Test function StandardGestureRecognizer
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_003 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    EXPECT_TRUE(AccessAccessibilityGestureRecognizer_->StandardGestureRecognizer(*event));

    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_003 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_004
 * @tc.name: StandardGestureRecognizer
 * @tc.desc: Test function StandardGestureRecognizer
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_004, TestSize.Level1)
{
    AccessibilityAbilityInfo abilityInfo;
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_004 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!AccessAccessibilityGestureRecognizerUnittest(accountData, connection, elementName, event,
        MMI::KeyEvent::KEY_ACTION_UP)) {
        return;
    }
    EXPECT_TRUE(AccessAccessibilityGestureRecognizer_->StandardGestureRecognizer(*event));
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_004 end";
}

/**
 * @tc.number: AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_005
 * @tc.name: StandardGestureRecognizer
 * @tc.desc: Test function StandardGestureRecognizer
 */
HWTEST_F(AccessAccessibilityGestureRecognizerUnitTest, AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_005, TestSize.Level1)
{
    AccessibilityAbilityInfo abilityInfo;
    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_005 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!AccessAccessibilityGestureRecognizerUnittest(accountData, connection, elementName, event,
        MMI::KeyEvent::KEY_ACTION_DOWN)) {
        return;
    }
    EXPECT_TRUE(AccessAccessibilityGestureRecognizer_->StandardGestureRecognizer(*event));
    bool ret5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret5);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "AccessAccessibilityGestureRecognizer_Unittest_StandardGestureRecognizer_005 end";
}

/**
 * @tc.number: AccessibilityGestureRecognizer_Unittest_recognizeDirectionGesture_001
 * @tc.name: recognizeDirectionGesture
 * @tc.desc: Test function recognizeDirectionGesture
 */
HWTEST_F(
    AccessibilityGestureRecognizerUnitTest, AccessibilityGestureRecognizer_Unittest_recognizeDirectionGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_recognizeDirectionGesture_001 start";

    uint64_t displayId = Singleton<AccessibilityGestureRecognizer>::GetInstance().recognizeDirectionGesture();
    GTEST_LOG_(INFO) << "DefaultDisplayId" << displayId;

    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_recognizeDirectionGesture_001 end";
}

/**
 * @tc.number: AccessibilityGestureRecognizer_Unittest_GetSwipeDirection_001
 * @tc.name: GetSwipeDirection
 * @tc.desc: Test function GetSwipeDirection
 */
HWTEST_F(
    AccessibilityGestureRecognizerUnitTest, AccessibilityGestureRecognizer_Unittest_GetSwipeDirection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_GetSwipeDirection_001 start";

    auto dpi = Singleton<AccessibilityGestureRecognizer>::GetInstance().GetSwipeDirection();
    EXPECT_NE(dpi, 0);

    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_GetSwipeDirection_001 end.";
}

/**
 * @tc.number: AccessibilityGestureRecognizer_Unittest_GetPointerPath_001
 * @tc.name: GetPointerPath
 * @tc.desc: Test function GetPointerPath
 */
HWTEST_F(
    AccessibilityGestureRecognizerUnitTest, AccessibilityGestureRecognizer_Unittest_GetPointerPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_GetPointerPath_001 start";

    auto status = Singleton<AccessibilityGestureRecognizer>::GetInstance().GetPointerPath();
    EXPECT_GE(status, 0);

    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_GetPointerPath_001 end.";
}

/**
 * @tc.number: AccessibilityGestureRecognizer_Unittest_isDoubleTap_001
 * @tc.name: isDoubleTap
 * @tc.desc: Test function isDoubleTap
 */
HWTEST_F(
    AccessibilityGestureRecognizerUnitTest, AccessibilityGestureRecognizer_Unittest_isDoubleTap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_isDoubleTap_001 start";

    auto mode = Singleton<AccessibilityGestureRecognizer>::GetInstance().isDoubleTap();
    EXPECT_GE(static_cast<uint64_t>(mode), 0);

    GTEST_LOG_(INFO) << "AccessibilityGestureRecognizer_Unittest_isDoubleTap_001 end.";
}
} // namespace Accessibility
} // namespace OHOS