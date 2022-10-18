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
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_mt_helper.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_client_proxy.h"
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
    const int32_t SLEEP_TIME_2 = 2;
} // namespace

class AamsInjectorTest : public testing::Test {
public:
    AamsInjectorTest()
    {}
    ~AamsInjectorTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityChannel> aacs_ = nullptr;
    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    void AddAccessibilityWindowConnection();
};

void AamsInjectorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AamsInjectorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AamsInjectorTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUp";

    // add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetCapabilityValues(Capability::CAPABILITY_TOUCH_GUIDE | Capability::CAPABILITY_GESTURE);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, aastub_);

    AddAccessibilityWindowConnection();

    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData()->GetConnectedA11yAbilities();
    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    if (ptr_connect) {
        aacs_ = new AccessibleAbilityChannel(accountData->GetAccountId(),
            ptr_connect->GetAbilityInfo().GetId());
    }
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUp end";
}

void AamsInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    aacs_ = nullptr;
    aastub_ = nullptr;
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    if (!ret) {
        GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDown EventQueue is not empty";
    }
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityHelper::GetInstance().GetEventType().clear();
}

void AamsInjectorTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest AddAccessibilityWindowConnection";
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
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected single-tap event can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point {500.0f, 500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);
    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(1)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (EventType::TYPE_TOUCH_END == AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(EventType::TYPE_TOUCH_BEGIN, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0));
    EXPECT_EQ(EventType::TYPE_TOUCH_GUIDE_BEGIN, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1));
    EXPECT_EQ(EventType::TYPE_TOUCH_GUIDE_END, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 end";
}

/**
 * @tc.number: TouchEventInjector002
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(1)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT) ==
            AccessibilityHelper::GetInstance().GetGestureId()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_002 end";
}

/**
 * @tc.number: TouchEventInjector003
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {3500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {5000.0f, 2500.0f};
    AccessibilityGesturePosition point4 {4000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(2)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP) ==
            AccessibilityHelper::GetInstance().GetGestureId()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_003 end";
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2500.0f};
    AccessibilityGesturePosition point4 {1000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(2)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP) ==
            AccessibilityHelper::GetInstance().GetGestureId()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {0.0f, 4000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(2)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT) ==
            AccessibilityHelper::GetInstance().GetGestureId()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {5000.0f, 4000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (MMI::PointerEvent::POINTER_ACTION_MOVE == MMI::MockInputManager::GetTouchActionOfTargetIndex(2)) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_MOVE, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        if (static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT) ==
            AccessibilityHelper::GetInstance().GetGestureId()) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_2);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 end";
}
} // namespace Accessibility
} // namespace OHOS