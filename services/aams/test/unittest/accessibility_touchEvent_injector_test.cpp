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
#include "accessibility_ability_helper.h"
#include "accessibility_touchEvent_injector.h"
#include "accessible_ability_client_proxy.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
const static uint32_t SLEEP_TIME_2 = 2;
const static uint32_t SLEEP_TIME_1 = 1;
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
    void CreateGesturePath(
        AccessibilityGesturePathPosition startpoint, AccessibilityGesturePathPosition endpoint, int64_t durationTime);
    sptr<IAccessibleAbilityClient> service = nullptr;
    MMI::PointerEvent CreateTouchEvent(int32_t action);
    int32_t pointId_ = -1;
    std::vector<int32_t> touchAction;
    std::vector<AccessibilityGesturePath> getGesturePath;
    bool isClearEvents_ = false;
    bool isDestroyEvents_ = false;
};

void TouchEventInjectorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest SetUpTestCase";
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();
}

void TouchEventInjectorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDownTestCase";
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStop();
}

void TouchEventInjectorTest::SetUp()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest SetUp";
    touchEventInjector_ = new TouchEventInjector();

    sptr<MockAccessibleAbilityClientStubImpl> stub = new MockAccessibleAbilityClientStubImpl();

    service = new AccessibleAbilityClientProxy(stub->AsObject());
    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
}

void TouchEventInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    service = nullptr;
    getGesturePath.clear();
    inputInterceptor_ = nullptr;
    touchAction.clear();
}

void TouchEventInjectorTest::CreateGesturePath(
    AccessibilityGesturePathPosition startpoint, AccessibilityGesturePathPosition endpoint, int64_t durationTime)
{
    AccessibilityGesturePath gesturePathDefine = AccessibilityGesturePath(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    getGesturePath.push_back(gesturePathDefine);
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
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 122";
    touchEventInjector_->SetNext(inputInterceptor_);
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 124";

    AccessibilityGesturePathPosition startpoint = {10.0f, 10.0f};
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 127";

    AccessibilityGesturePathPosition endpoint = {10.0f, 10.0f};
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 130";

    CreateGesturePath(startpoint, endpoint, 100);
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 133";

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 136";

    sleep(SLEEP_TIME_2);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 139";

    EXPECT_EQ(touchAction[0], expectValue);
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 143";

    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 146";

    EXPECT_EQ(touchAction[1], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 end";
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

    touchEventInjector_->ClearEvents(1);
    isClearEvents_ = AccessibilityAbilityHelper::GetInstance().GetClearState();
    EXPECT_EQ(isClearEvents_, true);

    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);

    MMI::PointerEvent touchevent = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchEventInjector_->OnPointerEvent(touchevent);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    sleep(SLEEP_TIME_2);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    EXPECT_EQ(touchAction[1], expectValue);

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

    AccessibilityGesturePathPosition startpoint = {10.0f, 10.0f};
    AccessibilityGesturePathPosition endpoint = {10.0f, 10.0f};

    CreateGesturePath(startpoint, endpoint, 100000);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(SLEEP_TIME_1);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction[0], expectValue);

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

    AccessibilityGesturePathPosition startpoint1 = {10.0f, 10.0f};
    AccessibilityGesturePathPosition endpoint1 = {10.0f, 20.0f};
    AccessibilityGesturePathPosition startpoint2 = {10.0f, 20.0f};
    AccessibilityGesturePathPosition endpoint2 = {20.0f, 20.0f};

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 100);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(SLEEP_TIME_2);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector();
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction[1], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(touchAction[2], expectValue);
    EXPECT_EQ(touchAction[3], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(touchAction[4], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 end";
}
} // namespace Accessibility
} // namespace OHOS