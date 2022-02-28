/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_display_manager.h"
#include "accessibility_input_interceptor.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
extern std::shared_ptr<MMI::IInputEventConsumer> g_inputEventConsumer;
namespace Accessibility {
class AccessibilityInputInterceptorTest : public testing::Test {
public:

    AccessibilityInputInterceptorTest() {}
    ~AccessibilityInputInterceptorTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> g_ins;

void AccessibilityInputInterceptorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest SetUpTestCase";
}

void AccessibilityInputInterceptorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest TearDownTestCase";
}

void AccessibilityInputInterceptorTest::SetUp()
{
    g_inputEventConsumer = nullptr;
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest SetUp";
    g_ins = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    g_ins->OnStart();
    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
}

void AccessibilityInputInterceptorTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest TearDown";
    inputInterceptor_ = nullptr;
    mock_ = nullptr;
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001 start";

    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions001 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002 start";

    uint32_t availableFunctions2 = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions2);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003 start";

    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;

    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_SetAvailableFunctions004 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnTouchEvent001
 * @tc.name: OnTouchEvent
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnTouchEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent001 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    inputInterceptor_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent001 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnTouchEvent002
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnTouchEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent002 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    if (g_inputEventConsumer != nullptr) {
        g_inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessTouchEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnTouchEvent003
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnTouchEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent003 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    item.SetGlobalX(1);
    item.SetGlobalY(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    if (g_inputEventConsumer != nullptr) {
        g_inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessTouchEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnTouchEvent003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnMouseEvent1
 * @tc.name: OnMouseEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnMouseEvent1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent1 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    inputInterceptor_->OnPointerEvent(*event);
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent1 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnMouseEvent002
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnMouseEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent002 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    if (g_inputEventConsumer != nullptr) {
        g_inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessMouseEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003
 * @tc.name: InterceptKeyEventCallBack
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    if (g_inputEventConsumer != nullptr) {
        g_inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessMouseEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnMouseEvent003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnKeyEvent001
 * @tc.name: OnKeyEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnKeyEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent001 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    inputInterceptor_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent001 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002
 * @tc.name: InterceptKeyEventCallBack
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};

    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);

    /* wait ProcessKeyEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent002 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003
 * @tc.name: InterceptKeyEventCallBack
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};

    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);

    /* wait ProcessKeyEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_OnKeyEvent003 end";
}

/**
 * @tc.number: AccessibilityInputInterceptorTest_Unittest_NotifyAccessibilityEvent001
 * @tc.name: NotifyAccessibilityEvent
 * @tc.desc: Check the NotifyAccessibilityEvent.
 */
HWTEST_F(AccessibilityInputInterceptorTest, AccessibilityInputInterceptorTest_Unittest_NotifyAccessibilityEvent001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_NotifyAccessibilityEvent001 start";
    AccessibilityEventInfo event {};
    inputInterceptor_->NotifyAccessibilityEvent(event);

    GTEST_LOG_(INFO) << "AccessibilityInputInterceptorTest_Unittest_NotifyAccessibilityEvent001 end";
}
} // namespace Accessibility
} // namespace OHOS
