/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "accessibility_input_filter.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessibility_display_manager.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {

class AccessibilityInputFilterTest : public testing::Test {
public:

    AccessibilityInputFilterTest() {}
    ~AccessibilityInputFilterTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibilityInputFilter> inputFilter_ = nullptr;
};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;

void AccessibilityInputFilterTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest SetUpTestCase";
}

void AccessibilityInputFilterTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest TearDownTestCase";
}

void AccessibilityInputFilterTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest SetUp";
    inputFilter_ = new AccessibilityInputFilter();
    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();
}

void AccessibilityInputFilterTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest TearDown";
    inputFilter_ = nullptr;
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_SetAvailableFunctions001
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_SetAvailableFunctions001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions001 start";

    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_FILTER_KEY_EVENTS;
    inputFilter_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions001 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_SetAvailableFunctions002
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_SetAvailableFunctions002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions002 start";

    uint32_t availableFunctions2 = AccessibilityInputFilter::FEATURE_INJECT_TOUCH_EVENTS;
    inputFilter_->SetAvailableFunctions(availableFunctions2);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions002 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_SetAvailableFunctions003
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_SetAvailableFunctions003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions003 start";
    /* Set Display */
    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    displayMgr.SetDisplay(wmdInfo);

    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_TOUCH_EXPLORATION;

    inputFilter_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions003 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_SetAvailableFunctions004
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_SetAvailableFunctions004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions004 start";
    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_SCREEN_MAGNIFICATION;
    inputFilter_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetAvailableFunctions004 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnTouchEvent001
 * @tc.name: OnTouchEvent
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnTouchEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent001 start";

    TouchEvent event{};
    inputFilter_->OnTouchEvent(event);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent001 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnTouchEvent002
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnTouchEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent002 start";
    TouchEvent touchEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::TOUCH_PANEL,
    };

    struct ManipulationProperty manipulationProperty = {
        .offsetX = 0,
        .offsetY = 0
    };

    struct TouchProperty touch = {
        .action = TouchEnum::PRIMARY_POINT_DOWN,
    };

    touchEvent.Initialize(multimodal, manipulationProperty, touch);

    inputFilter_->OnInputEvent(touchEvent);
    /* wait ProcessTouchEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent002 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnTouchEvent003
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnTouchEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent003 start";
    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_INJECT_TOUCH_EVENTS;
    inputFilter_->SetAvailableFunctions(availableFunctions);

    TouchEvent touchEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::TOUCH_PANEL,
    };

    struct ManipulationProperty manipulationProperty = {
        .offsetX = 1,
        .offsetY = 1
    };

    struct TouchProperty touch = {
        .action = TouchEnum::PRIMARY_POINT_DOWN,
    };

    touchEvent.Initialize(multimodal, manipulationProperty, touch);

    inputFilter_->OnInputEvent(touchEvent);
    /* wait ProcessTouchEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnTouchEvent003 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnMouseEvent1
 * @tc.name: OnMouseEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnMouseEvent1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent1 start";

    MouseEvent event{};
    inputFilter_->OnMouseEvent(event);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent1 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnMouseEvent002
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnMouseEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent002 start";
    MouseEvent mouseEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::MOUSE,
    };

    struct MouseProperty mouseProperty = {
        .xOffset = 1.1,
        .yOffset = 1.1,
    };

    mouseEvent.Initialize(multimodal, mouseProperty);

    inputFilter_->OnInputEvent(mouseEvent);
    /* wait ProcessMouseEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent002 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnMouseEvent003
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnMouseEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent003 start";
    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_TOUCH_EXPLORATION;
    inputFilter_->SetAvailableFunctions(availableFunctions);

    MouseEvent mouseEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::MOUSE,
    };

    struct MouseProperty mouseProperty = {
        .xOffset = 1.1,
        .yOffset = 1.1,
    };

    mouseEvent.Initialize(multimodal, mouseProperty);

    inputFilter_->OnInputEvent(mouseEvent);
    /* wait ProcessMouseEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnMouseEvent003 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnKeyEvent001
 * @tc.name: OnKeyEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnKeyEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent001 start";

    MMI::KeyEvent event{};
    inputFilter_->OnKeyEvent(event);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent001 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnKeyEvent002
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnKeyEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent002 start";
    MMI::KeyEvent keyEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::KEYBOARD,
    };

    struct KeyProperty property = {
        .isPressed = true,
        .keyCode = 1,
    };

    keyEvent.Initialize(multimodal, property);

    inputFilter_->OnInputEvent(keyEvent);
    /* wait ProcessKeyEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent002 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_OnKeyEvent003
 * @tc.name: OnInputEvent
 * @tc.desc: Check the on key event.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_OnKeyEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent003 start";
    uint32_t availableFunctions = AccessibilityInputFilter::FEATURE_FILTER_KEY_EVENTS;
    inputFilter_->SetAvailableFunctions(availableFunctions);

    MMI::KeyEvent keyEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::KEYBOARD,
    };

    struct KeyProperty property = {
        .isPressed = true,
        .keyCode = 1,
    };

    keyEvent.Initialize(multimodal, property);

    inputFilter_->OnInputEvent(keyEvent);
    /* wait ProcessKeyEvent */
    sleep(3);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_OnKeyEvent003 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_NotifyAccessibilityEvent001
 * @tc.name: NotifyAccessibilityEvent
 * @tc.desc: Check the NotifyAccessibilityEvent.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_NotifyAccessibilityEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_NotifyAccessibilityEvent001 start";
    AccessibilityEventInfo event {};
    inputFilter_->NotifyAccessibilityEvent(event);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_NotifyAccessibilityEvent001 end";
}

/**
 * @tc.number: AccessibilityInputFilterTest_Unittest_SetUser001
 * @tc.name: SetUser
 * @tc.desc: Check the SetUser.
 */
HWTEST_F(AccessibilityInputFilterTest, AccessibilityInputFilterTest_Unittest_SetUser001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetUser001 start";
    uint32_t userId = 12;
    inputFilter_->SetUser(userId);

    GTEST_LOG_(INFO) << "AccessibilityInputFilterTest_Unittest_SetUser001 end";
}

}  // namespace Accessibility
}  // namespace OHOS
