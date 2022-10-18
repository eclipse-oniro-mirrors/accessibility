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
#include "accessibility_touchEvent_injector.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_client_proxy.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
    constexpr uint32_t SLEEP_TIME_2 = 2;
} // namespace

class TouchEventInjectorTest : public testing::Test {
public:
    TouchEventInjectorTest()
    {}
    ~TouchEventInjectorTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<TouchEventInjector> touchEventInjector_ = nullptr;
    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;

protected:
    MMI::PointerEvent CreateTouchEvent(int32_t action);
    int32_t pointId_ = -1;
    bool isDestroyEvents_ = false;
};

void TouchEventInjectorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void TouchEventInjectorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void TouchEventInjectorTest::SetUp()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest SetUp";
    touchEventInjector_ = new TouchEventInjector();
    sptr<MockAccessibleAbilityClientStubImpl> stub = new MockAccessibleAbilityClientStubImpl();
    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
}

void TouchEventInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    inputInterceptor_ = nullptr;
}

MMI::PointerEvent TouchEventInjectorTest::CreateTouchEvent(int32_t action)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(pointId_++);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);

    return *pointerEvent;
}
/**
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected single tap event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 start";
    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);

    touchEventInjector_->InjectEvents(gesturePath);

    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction, expectValue);

    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check Some functional interface.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    touchEventInjector_->SetNext(inputInterceptor_);
    touchEventInjector_->OnPointerEvent(*event);
    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);

    MMI::PointerEvent touchevent = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchEventInjector_->OnPointerEvent(touchevent);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    EXPECT_EQ(touchAction, expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check destroy event before sendtouchevent.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100000);

    touchEventInjector_->InjectEvents(gesturePath);
    sleep(SLEEP_TIME_1);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction, expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected UP_THEN_RIGHT move event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point1 {10.0f, 10.0f};
    AccessibilityGesturePosition point2 {10.0f, 20.0f};
    AccessibilityGesturePosition point3 {20.0f, 20.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    touchEventInjector_->InjectEvents(gesturePath);
    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, expectValue);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(4);
    EXPECT_EQ(touchAction, expectValue);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 end";
}
} // namespace Accessibility
} // namespace OHOS