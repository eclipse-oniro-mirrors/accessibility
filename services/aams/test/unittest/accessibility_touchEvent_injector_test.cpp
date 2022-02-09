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
#include "accessibility_touchEvent_injector.h"
#include "accessibility_input_filter.h"
#include "accessible_ability_event_handler.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_client_proxy.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

const string AA_TOUCHEVENT_INJECTOR_UT = "AATouchEventInjectorUT";

namespace OHOS {
namespace Accessibility {

extern std::vector<int> TouchAction;
extern bool isMouseEvent;
extern bool isClearEvents;
extern bool isDestroyEvents;

class TouchEventInjectorTest : public testing::Test {
public:

    TouchEventInjectorTest() {}
    ~TouchEventInjectorTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<TouchEventInjector> touchEventInjector_ = nullptr;
    sptr<AccessibilityInputFilter> inputFilter_ = nullptr;
protected:
    void CreateGesturePath(
            GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime);
    sptr<IAccessibleAbilityClient> service = nullptr;
    TouchEvent CreateTouchEvent(int action);
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

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(AA_TOUCHEVENT_INJECTOR_UT);
    std::shared_ptr<AccessibleAbilityEventHandler> accessibleAbilityEventHandler =
        std::make_shared<AccessibleAbilityEventHandler>(runner);

    sptr<AccessibleAbilityClientStubImpl> stub = new AccessibleAbilityClientStubImpl(accessibleAbilityEventHandler);

    service = new AccessibleAbilityClientProxy(stub->AsObject());
    inputFilter_ = new AccessibilityInputFilter();
}

void TouchEventInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    service = nullptr;
    getGesturePath.clear();
    inputFilter_ = nullptr;
    TouchAction.clear();
}

void TouchEventInjectorTest::CreateGesturePath(
        GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime) {

    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    getGesturePath.push_back(gesturePathDefine);
}

TouchEvent TouchEventInjectorTest::CreateTouchEvent(int action) {
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {};
    struct ManipulationProperty manipulationProperty = {
        .pointerCount = 1,
        .pointerId = pointId_++,
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}
/**
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected single tap event .
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 start";

    touchEventInjector_->SetNext(inputFilter_);

    GesturePathPositionDefine startpoint(10.0f , 10.0f);
    GesturePathPositionDefine endpoint(10.0f , 10.0f);

    CreateGesturePath(startpoint, endpoint, 100);
    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 end";
}

/**
 * @tc.number: TouchEventInjector002
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected Double-Tap event .
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_002 start";

    touchEventInjector_->SetNext(inputFilter_);

    GesturePathPositionDefine startpoint1(10.0f , 10.0f);
    GesturePathPositionDefine endpoint1(10.0f , 10.0f);
    GesturePathPositionDefine startpoint2(20.0f , 20.0f);
    GesturePathPositionDefine endpoint2(20.0f , 20.0f);

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 100);
    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(TouchAction[2], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[3], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_002 end";
}
/**
 * @tc.number: TouchEventInjector003
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injiected move event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_003 start";

    touchEventInjector_->SetNext(inputFilter_);

    GesturePathPositionDefine startpoint(10.0f , 10.0f);
    GesturePathPositionDefine endpoint(10.0f , 20.0f);

    CreateGesturePath (startpoint, endpoint, 100);
    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(TouchAction[2], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_003 end";
}
/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check Some functional interface .
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 start";

    MouseEvent event{};
    touchEventInjector_->SetNext(inputFilter_);
    touchEventInjector_->OnMouseEvent(event);
    EXPECT_EQ(isMouseEvent, true);

    touchEventInjector_->ClearEvents(1);
    EXPECT_EQ(isClearEvents, true);

    touchEventInjector_->DestroyEvents();
    EXPECT_EQ(isDestroyEvents, true);

    TouchEvent touchevent = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN);
    touchEventInjector_->OnTouchEvent(touchevent);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);

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

    touchEventInjector_->SetNext(inputFilter_);

    GesturePathPositionDefine startpoint(10.0f , 10.0f);
    GesturePathPositionDefine endpoint(10.0f , 10.0f);

    CreateGesturePath(startpoint, endpoint, 100000);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(1);
    touchEventInjector_->DestroyEvents();
    EXPECT_EQ(isDestroyEvents, true);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::CANCEL);

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

    touchEventInjector_->SetNext(inputFilter_);

    GesturePathPositionDefine startpoint1(10.0f , 10.0f);
    GesturePathPositionDefine endpoint1(10.0f , 20.0f);
    GesturePathPositionDefine startpoint2(10.0f , 20.0f);
    GesturePathPositionDefine endpoint2(20.0f , 20.0f);

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 100);

    touchEventInjector_->InjectEvents(getGesturePath, service, 1);
    sleep(2);
    EXPECT_EQ(TouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(TouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(TouchAction[2], TouchEnum::POINT_MOVE);
    EXPECT_EQ(TouchAction[3], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 end";
}
}  // namespace Accessibility
}  // namespace OHOS