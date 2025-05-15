/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#define private public
#define protected public
#include <gtest/gtest.h>
#include "accessibility_common_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_mt_helper.h"
#include "accessibility_touch_exploration.h"
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
#undef private
#undef protected

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
    AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
    Singleton<AccessibleAbilityManagerService>::GetInstance().runner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().handler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().actionRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().actionHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().sendEventRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().sendEventHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().channelRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().channelHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().inputManagerRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().gestureRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().hoverEnterRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().hoverEnterHandler_.reset();
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
    sleep(1);
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
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperatorByWindowId(0);
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
    GTEST_LOG_(INFO) << "aams  RegisterElementOperatorByWindowId";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperatorByWindowId(windowId, proxy);
}

/**
 * @tc.number: SingleTap001
 * @tc.name:SingleTap
 * @tc.desc: Check that the injected single-tap event can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_SingleTap_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_SingleTap_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point {500.0f, 500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);
    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    EXPECT_EQ(EventType::TYPE_TOUCH_BEGIN, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0));
    EXPECT_EQ(EventType::TYPE_TOUCH_END, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_SingleTap_001 end";
}

/**
 * @tc.number: SingleTap002
 * @tc.name:SingleTap
 * @tc.desc: Check that the injected single-tap event can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_SingleTap_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_SingleTap_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point {500.0f, 500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(300);
    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER, MMI::MockInputManager::GetTouchActionOfTargetIndex(0));
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT, MMI::MockInputManager::GetTouchActionOfTargetIndex(1));

    EXPECT_EQ(EventType::TYPE_TOUCH_BEGIN, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0));
    EXPECT_EQ(EventType::TYPE_TOUCH_END, AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_SingleTap_002 end";
}

/**
 * @tc.number: Left001
 * @tc.name:Left
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Left_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_001 start";

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
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_001 end";
}

/**
 * @tc.number: Left002
 * @tc.name:Left
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Left_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_002 end";
}

/**
 * @tc.number: Left003
 * @tc.name:Left
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Left_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2300.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_003 end";
}

/**
 * @tc.number: Left004
 * @tc.name:Left
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Left_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 1000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Left_004 end";
}

/**
 * @tc.number: Right001
 * @tc.name:Right
 * @tc.desc: Check that the injected RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Right_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {0.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_001 end";
}

/**
 * @tc.number: Right002
 * @tc.name:Right
 * @tc.desc: Check that the injected RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Right_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {0.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_002 end";
}

/**
 * @tc.number: Right003
 * @tc.name:Right
 * @tc.desc: Check that the injected RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Right_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {0.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 2300.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_003 end";
}

/**
 * @tc.number: Right004
 * @tc.name:Right
 * @tc.desc: Check that the injected RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Right_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {0.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 1000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Right_004 end";
}

/**
 * @tc.number: Down001
 * @tc.name:Down
 * @tc.desc: Check that the injected DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Down_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 0.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_001 end";
}

/**
 * @tc.number: Down002
 * @tc.name:Down
 * @tc.desc: Check that the injected DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Down_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 0.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_002 end";
}

/**
 * @tc.number: Down003
 * @tc.name:Down
 * @tc.desc: Check that the injected DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Down_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 0.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {800.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_003 end";
}

/**
 * @tc.number: Down004
 * @tc.name:Down
 * @tc.desc: Check that the injected DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Down_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 0.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1500.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Down_004 end";
}

/**
 * @tc.number: Up001
 * @tc.name:Up
 * @tc.desc: Check that the injected UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Up_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);
    
    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_001 end";
}

/**
 * @tc.number: Up002
 * @tc.name:Up
 * @tc.desc: Check that the injected UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Up_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_002 end";
}

/**
 * @tc.number: Up003
 * @tc.name:Up
 * @tc.desc: Check that the injected UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Up_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {800.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_003 end";
}

/**
 * @tc.number: Up004
 * @tc.name:Up
 * @tc.desc: Check that the injected UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_Up_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1000.0f, 1000.0f};
    AccessibilityGesturePosition point3 {1500.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_Up_004 end";
}

/**
 * @tc.number: RightThenUp001
 * @tc.name:RightThenUp
 * @tc.desc: Check that the injected RIGHT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_001 start";

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
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_001 end";
}

/**
 * @tc.number: RightThenUp002
 * @tc.name:RightThenUp
 * @tc.desc: Check that the injected RIGHT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {3500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {5000.0f, 2500.0f};
    AccessibilityGesturePosition point4 {5000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_002 end";
}

/**
 * @tc.number: RightThenUp003
 * @tc.name:RightThenUp
 * @tc.desc: Check that the injected RIGHT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {3500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {5000.0f, 2500.0f};
    AccessibilityGesturePosition point4 {6000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_003 end";
}

/**
 * @tc.number: RightThenUp004
 * @tc.name:RightThenUp
 * @tc.desc: Check that the injected RIGHT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenUp_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {3500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {5000.0f, 2500.0f};
    AccessibilityGesturePosition point4 {8000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenUp_004 end";
}

/**
 * @tc.number: RightThenDown001
 * @tc.name:RightThenDown
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {3500.0f, 0.0f};
    AccessibilityGesturePosition point3 {5000.0f, 0.0f};
    AccessibilityGesturePosition point4 {4000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_001 end";
}

/**
 * @tc.number: RightThenDown002
 * @tc.name:RightThenDown
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {3500.0f, 0.0f};
    AccessibilityGesturePosition point3 {5000.0f, 0.0f};
    AccessibilityGesturePosition point4 {5000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_002 end";
}

/**
 * @tc.number: RightThenDown003
 * @tc.name:RightThenDown
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {3500.0f, 0.0f};
    AccessibilityGesturePosition point3 {5000.0f, 0.0f};
    AccessibilityGesturePosition point4 {6000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_003 end";
}

/**
 * @tc.number: RightThenDown004
 * @tc.name:RightThenDown
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_RightThenDown_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {3500.0f, 0.0f};
    AccessibilityGesturePosition point3 {5000.0f, 0.0f};
    AccessibilityGesturePosition point4 {8000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_RightThenDown_004 end";
}

/**
 * @tc.number: LeftThenUp001
 * @tc.name:LeftThenUp
 * @tc.desc: Check that the injected LEFT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenUp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2500.0f};
    AccessibilityGesturePosition point4 {0.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_001 end";
}

/**
 * @tc.number: LeftThenUp002
 * @tc.name:LeftThenUp
 * @tc.desc: Check that the injected LEFT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenUp_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_002 start";

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

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_002 end";
}

/**
 * @tc.number: LeftThenUp003
 * @tc.name:LeftThenUp
 * @tc.desc: Check that the injected LEFT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenUp_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {0.0f, 2500.0f};
    AccessibilityGesturePosition point4 {2000.0f, 0.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_003 end";
}

/**
 * @tc.number: LeftThenUp004
 * @tc.name: LeftThenUp
 * @tc.desc: Check that the injected LEFT_THEN_UP gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenUp_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_004 start";

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
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_UP));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenUp_004 end";
}

/**
 * @tc.number: LeftThenDown001
 * @tc.name:LeftThenDown
 * @tc.desc: Check that the injected LEFT_THEN_DWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenDown_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {1500.0f, 0.0f};
    AccessibilityGesturePosition point3 {0.0f, 0.0f};
    AccessibilityGesturePosition point4 {0.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_001 end";
}

/**
 * @tc.number: LeftThenDown002
 * @tc.name:LeftThenDown
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenDown_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {1500.0f, 0.0f};
    AccessibilityGesturePosition point3 {0.0f, 0.0f};
    AccessibilityGesturePosition point4 {1000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_002 end";
}

/**
 * @tc.number: LeftThenDown003
 * @tc.name:LeftThenDown
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenDown_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {1500.0f, 0.0f};
    AccessibilityGesturePosition point3 {0.0f, 0.0f};
    AccessibilityGesturePosition point4 {2000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_003 end";
}

/**
 * @tc.number: LeftThenDown004
 * @tc.name:LeftThenDown
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenDown_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 0.0f};
    AccessibilityGesturePosition point2 {1500.0f, 0.0f};
    AccessibilityGesturePosition point3 {0.0f, 0.0f};
    AccessibilityGesturePosition point4 {1000.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenDown_004 end";
}

/**
 * @tc.number: DownThenLeft001
 * @tc.name:DownThenLeft
 * @tc.desc: Check that the injected DOWN_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenLeft_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {0.0f, 5000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_001 end";
}

/**
 * @tc.number: DownThenLeft002
 * @tc.name:DownThenLeft
 * @tc.desc: Check that the injected DOWN_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenLeft_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_002 start";

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

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_002 end";
}

/**
 * @tc.number: DownThenLeft003
 * @tc.name:DownThenLeft
 * @tc.desc: Check that the injected DOWN_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenLeft_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {0.0f, 6000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_003 end";
}

/**
 * @tc.number: DownThenLeft004
 * @tc.name:DownThenLeft
 * @tc.desc: Check that the injected DOWN_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenLeft_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {0.0f, 8000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_004 end";
}

/**
 * @tc.number: DownThenLeft005
 * @tc.name:DownThenLeft
 * @tc.desc: Check that the injected DOWN_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenLeft_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_005 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {0.0f, 5000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenLeft_005 end";
}

/**
 * @tc.number: DownThenRight001
 * @tc.name:DownThenRight
 * @tc.desc: Check that the injected DOWN_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenRight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {5000.0f, 5000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_001 end";
}

/**
 * @tc.number: DownThenRight002
 * @tc.name:DownThenRight
 * @tc.desc: Check that the injected DOWN_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenRight_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_002 start";

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

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_002 end";
}

/**
 * @tc.number: DownThenRight003
 * @tc.name:DownThenRight
 * @tc.desc: Check that the injected DOWN_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenRight_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_003 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {5000.0f, 6000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_003 end";
}

/**
 * @tc.number: DownThenRight004
 * @tc.name:DownThenRight
 * @tc.desc: Check that the injected DOWN_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenRight_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_004 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {5000.0f, 8000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_004 end";
}

/**
 * @tc.number: DownThenRight005
 * @tc.name:DownThenRight
 * @tc.desc: Check that the injected DOWN_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_DownThenRight_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_005 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {2500.0f, 3500.0f};
    AccessibilityGesturePosition point3 {2500.0f, 5000.0f};
    AccessibilityGesturePosition point4 {5000.0f, 5000.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->SetDurationTime(800);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_NE(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_DownThenRight_005 end";
}

/**
 * @tc.number: LeftThenRight001
 * @tc.name:LeftThenRight
 * @tc.desc: Check that the injected LEFT_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenRight_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenRight_001 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1500.0f, 2500.0f};
    AccessibilityGesturePosition point3 {1000.0f, 2500.0f};
    AccessibilityGesturePosition point4 {1500.0f, 2500.0f};
    AccessibilityGesturePosition point5 {2500.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->AddPosition(point5);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenRight_001 end";
}

/**
 * @tc.number: LeftThenRight002
 * @tc.name:LeftThenRight
 * @tc.desc: Check that the injected LEFT_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_LeftThenRight_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenRight_002 start";

    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
    AccessibilityGesturePosition point1 {2500.0f, 2500.0f};
    AccessibilityGesturePosition point2 {1500.0f, 2000.0f};
    AccessibilityGesturePosition point3 {1000.0f, 1500.0f};
    AccessibilityGesturePosition point4 {1500.0f, 2000.0f};
    AccessibilityGesturePosition point5 {2500.0f, 2500.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->AddPosition(point4);
    gesturePath->AddPosition(point5);
    gesturePath->SetDurationTime(300);

    aacs_->SendSimulateGesture(gesturePath);

    sleep(1);

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_LeftThenRight_002 end";
}
} // namespace Accessibility
} // namespace OHOS