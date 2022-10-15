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
#include "accessibility_input_interceptor.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_manager_service.h"
#include "mock_bundle_manager.h"
#include "mock_input_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_3 = 3;
} // namespace

class AccessibilityInputInterceptorTest : public testing::Test {
public:
    AccessibilityInputInterceptorTest()
    {}
    ~AccessibilityInputInterceptorTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityInterceptorManager> inputInterceptor_ = nullptr;
};

void AccessibilityInputInterceptorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityInputInterceptorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityInputInterceptorTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest SetUp";
    inputInterceptor_ = AccessibilityInterceptorManager::GetInstance();
}

void AccessibilityInputInterceptorTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest TearDown";
    inputInterceptor_ = nullptr;
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001 start";

    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_FILTER_KEY_EVENTS;
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001 94";
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002 start";

    uint32_t availableFunctions2 = AccessibilityInterceptorManager::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions2);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003 start";

    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_TOUCH_EXPLORATION;

    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004 start";
    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_SCREEN_MAGNIFICATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions005
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions005,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions005 start";
    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_SCREEN_MAGNIFICATION |
        AccessibilityInterceptorManager::FEATURE_MOUSE_AUTOCLICK |
        AccessibilityInterceptorManager::FEATURE_SHORT_KEY |
        AccessibilityInterceptorManager::FEATURE_MOUSE_KEY;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions005 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003 start";
    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_TOUCH_EXPLORATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    event->AddPointerItem(item);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(event);
    }

    MMI::MockInputManager::ClearTouchActions();
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (MMI::MockInputManager::GetTouchActionOfTargetIndex(0) == MMI::PointerEvent::POINTER_ACTION_CANCEL) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    MMI::MockInputManager::ClearTouchActions();

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002 start";
    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_TOUCH_EXPLORATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    if (!keyEvent) {
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (MMI::MockInputManager::GetKeyCode() == MMI::KeyEvent::KEYCODE_NUMPAD_1) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003
 * @tc.name: OnKeyEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003 start";
    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_FILTER_KEY_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    if (!keyEvent) {
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_2);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (MMI::MockInputManager::GetKeyCode() == MMI::KeyEvent::KEYCODE_NUMPAD_2) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnMoveMouse001
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnMoveMouse001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMoveMouse001 start";

    uint32_t availableFunctions = AccessibilityInterceptorManager::FEATURE_MOUSE_KEY;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    if (!event) {
        return;
    }
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_1);
    item.SetPressed(true);
    event->AddKeyItem(item);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        GTEST_LOG_(INFO) << "inputEventConsumer is null";
        inputEventConsumer->OnInputEvent(event);
    }

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (MMI::MockInputManager::GetMouseMoveFlag()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    MMI::MockInputManager::ResetMouseMoveFlag();

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMoveMouse001 end";
}
} // namespace Accessibility
} // namespace OHOS