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

#include "accessibility_input_interceptor.h"
#include "accessibility_touchEvent_injector.h"
#include "accessible_ability_client_proxy.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
extern std::vector<int> g_touchAction;
extern bool g_isMouseEvent;
extern bool g_isClearEvents;
extern bool g_isDestroyEvents;

class TouchEventInjectorTest : public testing::Test {
public:
    TouchEventInjectorTest() {}
    ~TouchEventInjectorTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<TouchEventInjector> touchEventInjector_ = nullptr;
    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;
protected:
    void CreateGesturePath(GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime);
    sptr<IAccessibleAbilityClient> service = nullptr;
    MMI::PointerEvent CreateTouchEvent(int action);
    int pointId_ = -1;

    std::vector<GesturePathDefine> getGesturePath;
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

    sptr<AccessibleAbilityClientStubImpl> stub = new AccessibleAbilityClientStubImpl();

    service = new AccessibleAbilityClientProxy(stub->AsObject());
    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
}

void TouchEventInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    service = nullptr;
    getGesturePath.clear();
    inputInterceptor_ = nullptr;
    g_touchAction.clear();
}

void TouchEventInjectorTest::CreateGesturePath(
    GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime)
{
    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    getGesturePath.push_back(gesturePathDefine);
}

MMI::PointerEvent TouchEventInjectorTest::CreateTouchEvent(int action)
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
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected single tap event .
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    GesturePathPositionDefine startpoint(10.0f, 10.0f);
    GesturePathPositionDefine endpoint(10.0f, 10.0f);

    CreateGesturePath(startpoint, endpoint, 100);
    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(g_touchAction[1], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 end";
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check Some functional interface .
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    touchEventInjector_->SetNext(inputInterceptor_);
    touchEventInjector_->OnPointerEvent(*event);

    touchEventInjector_->ClearEvents(1);
    EXPECT_EQ(g_isClearEvents, true);

    touchEventInjector_->DestroyEvents();
    EXPECT_EQ(g_isDestroyEvents, true);

    MMI::PointerEvent touchevent = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchEventInjector_->OnPointerEvent(touchevent);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    sleep(2);
    EXPECT_EQ(g_touchAction[1], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check destroy event before sendtouchevent.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    GesturePathPositionDefine startpoint(10.0f, 10.0f);
    GesturePathPositionDefine endpoint(10.0f, 10.0f);

    CreateGesturePath(startpoint, endpoint, 100000);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(1);
    touchEventInjector_->DestroyEvents();
    EXPECT_EQ(g_isDestroyEvents, true);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected UP_THEN_RIGHT move event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    GesturePathPositionDefine startpoint1(10.0f, 10.0f);
    GesturePathPositionDefine endpoint1(10.0f, 20.0f);
    GesturePathPositionDefine startpoint2(10.0f, 20.0f);
    GesturePathPositionDefine endpoint2(20.0f, 20.0f);

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 100);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(g_touchAction[0], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_touchAction[1], expectValue);
    EXPECT_EQ(g_touchAction[2], expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    EXPECT_EQ(g_touchAction[3], expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 end";
}
} // namespace Accessibility
} // namespace OHOS