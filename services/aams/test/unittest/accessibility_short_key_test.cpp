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
#include "accessibility_short_key.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityShortKeyUnitTest : public ::testing::Test {
public:
    AccessibilityShortKeyUnitTest()
    {}
    ~AccessibilityShortKeyUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<MMI::KeyEvent> CreateKeyEvent(int32_t keyCode, int32_t keyAction);

    std::shared_ptr<AccessibilityShortKey> shortKey_ = nullptr;
};

void AccessibilityShortKeyUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortKeyUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityShortKeyUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortKeyUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityShortKeyUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    shortKey_ = std::make_shared<AccessibilityShortKey>();
}

void AccessibilityShortKeyUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    shortKey_ = nullptr;
}

std::shared_ptr<MMI::KeyEvent> AccessibilityShortKeyUnitTest::CreateKeyEvent(int32_t keyCode, int32_t keyAction)
{
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return nullptr;
    }
    event->SetKeyCode(keyCode);
    event->SetKeyAction(keyAction);
    return event;
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(press '5' key)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_001 start";
    if (!shortKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = CreateKeyEvent(MMI::KeyEvent::KEYCODE_NUMPAD_5,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!event) {
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(true);
    event->AddKeyItem(item);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    shortKey_->OnKeyEvent(*event);
    int32_t keyCode = AccessibilityAbilityHelper::GetInstance().GetKeyCode();
    EXPECT_EQ(keyCode, MMI::KeyEvent::KEYCODE_NUMPAD_5);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_002
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_002 start";
    if (!shortKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    shortKey_->OnKeyEvent(*event);
    int32_t keyCode = AccessibilityAbilityHelper::GetInstance().GetKeyCode();
    EXPECT_EQ(keyCode, MMI::KeyEvent::KEYCODE_POWER);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_002 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_003
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(Long press the power key)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_003 start";
    if (!shortKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!event) {
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(true);
    event->AddKeyItem(item);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    shortKey_->OnKeyEvent(*event);
    sleep(2);
    int32_t keyCode = AccessibilityAbilityHelper::GetInstance().GetKeyCode();
    EXPECT_EQ(keyCode, MMI::KeyEvent::KEYCODE_POWER);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_003 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_004
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(Press the power button 3 times)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_004 start";
    if (!shortKey_) {
        return;
    }
    // power key(down)
    std::shared_ptr<MMI::KeyEvent> downEvent = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!downEvent) {
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(true);
    downEvent->AddKeyItem(item);
    shortKey_->OnKeyEvent(*downEvent);
    // power key(up)
    std::shared_ptr<MMI::KeyEvent> upEvent = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_UP);
    if (!upEvent) {
        return;
    }
    shortKey_->OnKeyEvent(*upEvent);

    // power key(down)
    std::shared_ptr<MMI::KeyEvent> downEvent1 = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!downEvent1) {
        return;
    }
    MMI::KeyEvent::KeyItem item1;
    item1.SetPressed(true);
    downEvent1->AddKeyItem(item1);
    shortKey_->OnKeyEvent(*downEvent1);
    // power key(up)
    std::shared_ptr<MMI::KeyEvent> upEvent1 = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_UP);
    if (!upEvent1) {
        return;
    }
    shortKey_->OnKeyEvent(*upEvent1);

    // power key(down)
    std::shared_ptr<MMI::KeyEvent> downEvent2 = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_DOWN);
    if (!downEvent2) {
        return;
    }
    MMI::KeyEvent::KeyItem item2;
    item2.SetPressed(true);
    downEvent2->AddKeyItem(item2);
    shortKey_->OnKeyEvent(*downEvent2);
    // power key(up)
    std::shared_ptr<MMI::KeyEvent> upEvent2 = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_UP);
    if (!upEvent2) {
        return;
    }
    shortKey_->OnKeyEvent(*upEvent2);
    bool state = AccessibilityAbilityHelper::GetInstance().GetShortKeyTargetAbilityState();
    EXPECT_TRUE(state);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_004 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_005
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(power key up)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_005 start";
    if (!shortKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_UP);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    shortKey_->OnKeyEvent(*event);
    int32_t keyCode = AccessibilityAbilityHelper::GetInstance().GetKeyCode();
    EXPECT_EQ(keyCode, MMI::KeyEvent::KEYCODE_POWER);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_005 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_OnKeyEvent_006
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(power key cancel)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_OnKeyEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_006 start";
    if (!shortKey_) {
        return;
    }
    std::shared_ptr<MMI::KeyEvent> event = CreateKeyEvent(MMI::KeyEvent::KEYCODE_POWER,
        MMI::KeyEvent::KEY_ACTION_CANCEL);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    shortKey_->OnKeyEvent(*event);
    int32_t keyCode = AccessibilityAbilityHelper::GetInstance().GetKeyCode();
    EXPECT_EQ(keyCode, MMI::KeyEvent::KEYCODE_POWER);

    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_006 end";
}
} // namespace Accessibility
} // namespace OHOS