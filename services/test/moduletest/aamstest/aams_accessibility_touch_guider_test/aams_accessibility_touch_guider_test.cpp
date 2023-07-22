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
#include <unistd.h>
#include "accessibility_touch_guider.h"
#include "accessibility_account_data.h"
#include "accessibility_common_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessibility_element_operator_callback.h"
#include "mock_accessibility_element_operator_impl.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_input_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t POINT_ID_2 = 2;
    const int32_t SLEEP_TIME_3 = 3;
} // namespace

class AamsTouchGuideTest : public testing::Test {
public:
    AamsTouchGuideTest()
    {}
    ~AamsTouchGuideTest()
    {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    static void TouchGuiderPointSet(MMI::PointerEvent::PointerItem &point, int id, int x, int y);
    bool OnPointerEventOnePointsTest1(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4);
    bool OnPointerEventOnePointsTest3(std::vector<MMI::PointerEvent::PointerItem> &point,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4);
    bool OnPointerEventOnePointsTest7(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<MMI::PointerEvent> CreateMoveEvent(int32_t pointerCount, int32_t pointId);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action, int32_t pointId);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action,
        std::vector<MMI::PointerEvent::PointerItem> &point, int64_t occurredTime, int64_t startTime,
        int32_t pointId);

    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    std::shared_ptr<MMI::IInputEventConsumer> interceptorId_ = nullptr;
    void WritefileAll(const char* fname, const char* data);
    void AddAccessibilityWindowConnection();
};

bool AamsTouchGuideTest::OnPointerEventOnePointsTest1(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, MMI::PointerEvent::PointerItem point3,
    MMI::PointerEvent::PointerItem point4)
{
    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

bool AamsTouchGuideTest::OnPointerEventOnePointsTest3(std::vector<MMI::PointerEvent::PointerItem> &point,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, MMI::PointerEvent::PointerItem point3,
    MMI::PointerEvent::PointerItem point4)
{
    point.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, point, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        // GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent003 inputEventConsumer is null";
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

bool AamsTouchGuideTest::OnPointerEventOnePointsTest7(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, MMI::PointerEvent::PointerItem point3)
{
    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

void AamsTouchGuideTest::TouchGuiderPointSet(MMI::PointerEvent::PointerItem &point, int id, int x, int y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

void AamsTouchGuideTest::WritefileAll(const char* fname, const char* data)
{
    FILE* fp;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsTouchGuideTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AamsTouchGuideTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AamsTouchGuideTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest SetUp";

    interceptorId_ = std::make_shared<AccessibilityInputEventConsumer>();
    MMI::InputManager::GetInstance()->AddInterceptor(interceptorId_);

    // add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetCapabilityValues(Capability::CAPABILITY_TOUCH_GUIDE);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, aastub_);

    AddAccessibilityWindowConnection();
}

void AamsTouchGuideTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest TearDown";

    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    sleep(SLEEP_TIME_3);
    aastub_ = nullptr;
    interceptorId_ = nullptr;
    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
}

std::shared_ptr<MMI::PointerEvent> AamsTouchGuideTest::CreateTouchEvent(int32_t action, int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> touchEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    touchEvent->AddPointerItem(item);
    touchEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    touchEvent->SetPointerAction(action);
    touchEvent->SetPointerId(pointId);

    return touchEvent;
}

std::shared_ptr<MMI::PointerEvent> AamsTouchGuideTest::CreateMoveEvent(int32_t pointerCount, int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> touchEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    for (int32_t i = 0; i < pointerCount; i++) {
        item.SetPointerId(i + 1);
        touchEvent->AddPointerItem(item);
    }
    touchEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    touchEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchEvent->SetPointerId(pointId);

    return touchEvent;
}

std::shared_ptr<MMI::PointerEvent> AamsTouchGuideTest::CreateTouchEvent(int32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int64_t occurredTime, int64_t startTime,
    int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    for (auto &point : points) {
        pointerEvent->AddPointerItem(point);
    }
    pointerEvent->SetPointerId(pointId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionStartTime(startTime);
    pointerEvent->SetActionTime(occurredTime);

    return pointerEvent;
}

void AamsTouchGuideTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest AddAccessibilityWindowConnection";
    // accessibility interaction connection
    int32_t windowId = 0;
    std::shared_ptr<MockAccessibilityElementOperatorCallback> mockCallback =
        std::make_shared<MockAccessibilityElementOperatorCallback>();
    sptr<AccessibilityElementOperatorStub> stub =
        new MockAccessibilityElementOperatorImpl(windowId, nullptr, *mockCallback);
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    GTEST_LOG_(INFO) << "aams  RegisterElementOperator";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(windowId, proxy);
}

/**
 * @tc.number: OnPointerEvent001
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in dragging state.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent001 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 500, 500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 2, 1000, 500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 2000, 500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchGuiderPointSet(point4, 1, 2500, 500);

    bool flag = OnPointerEventOnePointsTest1(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent001 inputEventConsumer is null";
    }

    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (MMI::MockInputManager::GetTouchActionOfTargetIndex(2) == MMI::PointerEvent::POINTER_ACTION_UP) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent001 ENDs";
}

/**
 * @tc.number: OnPointerEvent002
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent002 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent002 inputEventConsumer is null";
        return;
    }
    inputEventConsumer->OnInputEvent(event);
    bool retOnPointerEvent2 =
        AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);

    event = CreateMoveEvent(1, 1);
    inputEventConsumer->OnInputEvent(event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, 1);
    inputEventConsumer->OnInputEvent(event);
    retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2), EventType::TYPE_TOUCH_GUIDE_END);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent002 ENDs";
}

/**
 * @tc.number: OnPointerEvent003
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_LEFT_THEN_RIGHT gesture.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent003 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 1, 1500, 2500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 0, 2500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchGuiderPointSet(point4, 1, 2500, 2250);

    OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);

    // Determine event type
    bool retOnPointerEvent3 =
        AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_MOVE);
    // Determine gesture type
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetGestureId() ==
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent003 ENDs";
}

/**
 * @tc.number: OnPointerEvent004
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_UP gesture.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent004 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchGuiderPointSet(point4, 1, 2250, 5000);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent003 inputEventConsumer is null";
    }

    // Determine event type
    bool retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_MOVE);
    // Determine gesture type
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetGestureId() ==
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_UP)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent004 ENDs";
}

/**
 * @tc.number: OnPointerEvent005
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_RIGHT_THEN_LEFT gesture.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent005 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 1, 3500, 2500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 5000, 2500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchGuiderPointSet(point4, 1, 2500, 2250);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent005 inputEventConsumer is null";
    }

    // Determine event type
    bool retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_MOVE);
    // Determine gesture type
    retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetGestureId() ==
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent005 ENDs";
}

/**
 * @tc.number: OnPointerEvent006
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_UP_THEN_DOWN gesture.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent006 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 2500, 0);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchGuiderPointSet(point4, 1, 2250, 2500);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent006 inputEventConsumer is null";
    }

    // Determine event type
    bool retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_MOVE);
    // Determine gesture type
    retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetGestureId() ==
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP_THEN_DOWN)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent006 ENDs";
}

/**
 * @tc.number: OnPointerEvent007
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the UP gesture.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent007 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchGuiderPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchGuiderPointSet(point2, 1, 2500, 1500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchGuiderPointSet(point3, 1, 2500, 0);

    bool flag = OnPointerEventOnePointsTest7(points, point1, point2, point3);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent007 inputEventConsumer is null";
    }

    // Determine event type
    bool retOnPointerEvent7 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent7);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
    // Determine gesture type
    retOnPointerEvent7 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetGestureId() == static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent7);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent007 ENDs";
}

/**
 * @tc.number: OnPointerEvent008
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the single tap event.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent008 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent009 inputEventConsumer is null";
        return;
    }
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, 1);
    inputEventConsumer->OnInputEvent(event);
    // Determine event type
    bool retOnPointerEvent8 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent8);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1),
        EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2),
        EventType::TYPE_TOUCH_GUIDE_END);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent008 ends";
}

/**
 * @tc.number: OnPointerEvent009
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the double tap and long press event.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent009 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    point1.SetDisplayX(300);
    point1.SetDisplayY(500);
    point1.SetPointerId(1);

    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent009 inputEventConsumer is null";
        return;
    }
    inputEventConsumer->OnInputEvent(event);
    sleep(1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 40, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 200, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    sleep(SLEEP_TIME_3);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    // Determine event type
    bool retOnPointerEvent9 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent9);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent009 ends";
}

/**
 * @tc.number: OnPointerEvent010
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the double-tap event.
 */
HWTEST_F(AamsTouchGuideTest, AamsTouchGuideTest_Moduletest_OnPointerEvent010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent010 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    point1.SetDisplayX(300);
    point1.SetDisplayY(500);
    point1.SetPointerId(1);

    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchGuideTest OnPointerEvent010 inputEventConsumer is null";
        return;
    }
    inputEventConsumer->OnInputEvent(event);
    sleep(1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 100, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    // Determine event type
    bool retOnPointerEvent10 =
        AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent10);
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
    // Determine action
    EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);

    GTEST_LOG_(INFO) << "AamsTouchGuideTest AamsTouchGuideTest_Moduletest_OnPointerEvent010 ends";
}
} // namespace Accessibility
} // namespace OHOS