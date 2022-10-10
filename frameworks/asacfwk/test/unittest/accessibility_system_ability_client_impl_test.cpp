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
#include <memory>
#include "accessibility_system_ability_client_impl.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessibility_element_operator.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t WINDOW_ID = 1;
    constexpr uint32_t ACCESSIBILITY_ABILITY_TYPES = 1;
    constexpr int32_t COMPONENT_ID = 1;
    constexpr uint32_t EVENT_TYPE = 1;
    constexpr uint32_t STATE_TYPE = 1;
} // namespace

class AccessibilitySystemAbilityClientImplTest : public ::testing::Test {
public:
    AccessibilitySystemAbilityClientImplTest()
    {}
    ~AccessibilitySystemAbilityClientImplTest()
    {}
    std::shared_ptr<AccessibilitySystemAbilityClientImpl> impl_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest SetUp()";
        impl_ = std::make_shared<AccessibilitySystemAbilityClientImpl>();
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest TearDown()";
        impl_ = nullptr;
    }
};

/**
 * @tc.number: RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityElementOperator> operator_ = std::make_shared<MockAccessibilityElementOperator>();
    EXPECT_EQ(0, impl_->RegisterElementOperator(WINDOW_ID, operator_));
    impl_->DeregisterElementOperator(WINDOW_ID);
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 end";
}

/**
 * @tc.number: DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityElementOperator> operator_ = std::make_shared<MockAccessibilityElementOperator>();
    impl_->RegisterElementOperator(WINDOW_ID, operator_);
    impl_->DeregisterElementOperator(WINDOW_ID);
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 end";
}

/**
 * @tc.number: GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAbilityList_001 start";
    AbilityStateType stateType = ABILITY_STATE_INVALID;
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }

    std::vector<AccessibilityAbilityInfo> infos {};
    impl_->GetAbilityList(ACCESSIBILITY_ABILITY_TYPES, stateType, infos);
    EXPECT_EQ(0, infos.size());
    GTEST_LOG_(INFO) << "GetAbilityList_001 end";
}

/**
 * @tc.number: IsEnabled_001
 * @tc.name: IsEnabled
 * @tc.desc: Test function IsEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_FALSE(impl_->IsEnabled());
    GTEST_LOG_(INFO) << "IsEnabled_001 end";
}

/**
 * @tc.number: IsTouchExplorationEnabled_001
 * @tc.name: IsTouchExplorationEnabled
 * @tc.desc: Test function IsTouchExplorationEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsTouchExplorationEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_FALSE(impl_->IsTouchExplorationEnabled());
    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled_001 end";
}

/**
 * @tc.number: SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendEvent_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EventType eventType = TYPE_VIEW_LONG_CLICKED_EVENT;
    EXPECT_TRUE(impl_->SendEvent(eventType, COMPONENT_ID));
    GTEST_LOG_(INFO) << "SendEvent_001 end";
}

/**
 * @tc.number: SendEvent_002
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SendEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendEvent_002 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityEventInfo event {};
    EXPECT_FALSE(impl_->SendEvent(event));
    GTEST_LOG_(INFO) << "SendEvent_002 end";
}

/**
 * @tc.number: SubscribeStateObserver_001
 * @tc.name: SubscribeStateObserver
 * @tc.desc: Test function SubscribeStateObserver
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SubscribeStateObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SubscribeStateObserver_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityStateObserver> observer = nullptr;
    EXPECT_FALSE(impl_->SubscribeStateObserver(observer, EVENT_TYPE));
    impl_->UnsubscribeStateObserver(observer, EVENT_TYPE);
    GTEST_LOG_(INFO) << "SubscribeStateObserver_001 end";
}

/**
 * @tc.number: UnsubscribeStateObserver_001
 * @tc.name: UnsubscribeStateObserver
 * @tc.desc: Test function UnsubscribeStateObserver
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, UnsubscribeStateObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UnsubscribeStateObserver_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityStateObserver> observer = nullptr;
    impl_->SubscribeStateObserver(observer, EVENT_TYPE);
    EXPECT_FALSE(impl_->UnsubscribeStateObserver(observer, EVENT_TYPE));
    GTEST_LOG_(INFO) << "UnsubscribeStateObserver_001 end";
}

/**
 * @tc.number: ResetService_001
 * @tc.name: ResetService
 * @tc.desc: Test function ResetService
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, ResetService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ResetService_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    wptr<IRemoteObject> remote = nullptr;
    impl_->ResetService(remote);
    GTEST_LOG_(INFO) << "ResetService_001 end";
}

/**
 * @tc.number: OnAccessibleAbilityManagerStateChanged_001
 * @tc.name: OnAccessibleAbilityManagerStateChanged
 * @tc.desc: Test function OnAccessibleAbilityManagerStateChanged
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, OnAccessibleAbilityManagerStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerStateChanged_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    impl_->OnAccessibleAbilityManagerStateChanged(STATE_TYPE);
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerStateChanged_001 end";
}

/**
 * @tc.number: GetEnabledAbilities_001
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetEnabledAbilities_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::vector<std::string> enabledAbilities;
    impl_->GetEnabledAbilities(enabledAbilities);
    EXPECT_EQ(0, enabledAbilities.size());
    GTEST_LOG_(INFO) << "GetEnabledAbilities_001 end";
}
} // namespace Accessibility
} // namespace OHOS