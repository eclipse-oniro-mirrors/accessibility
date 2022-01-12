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

#include <memory>
#include <gtest/gtest.h>
#include "accessibility_system_ability_client.h"
#include "ability_context.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_interaction_operation.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_interaction_operation_stub.h"
#include "dummy.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "mock_accessible_ability_manager_service_impl.h"

using namespace OHOS::AppExecFwk;


using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class ASACClientUnitTest : public ::testing::Test {
public:
    ASACClientUnitTest() {}
    ~ASACClientUnitTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACClientUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "ASACClientUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACClientUnitTest SetUp()";
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "ASACClientUnitTest TearDown()";
    }
};

/**
 * @tc.number: ASAC_001
 * @tc.name: Function
 * @tc.desc: Test function
 */
HWTEST_F(ASACClientUnitTest, ASAC_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChannel_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "Interrupt start";
    instance->Interrupt();
    GTEST_LOG_(INFO) << "Interrupt end";

    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(0, nullptr, 1);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

    GTEST_LOG_(INFO) << "DeregisterInteractionOperation start";
    instance->DeregisterInteractionOperation(1);
    GTEST_LOG_(INFO) << "DeregisterInteractionOperation end";

    GTEST_LOG_(INFO) << "GetInteractionObject start";
    instance->GetInteractionObject(1);
    GTEST_LOG_(INFO) << "GetInteractionObject end";

    GTEST_LOG_(INFO) << "SetEnabled start";
    instance->SetEnabled(true);
    GTEST_LOG_(INFO) << "SetEnabled end";

    GTEST_LOG_(INFO) << "IsEnabled start";
    EXPECT_TRUE(instance->IsEnabled());
    GTEST_LOG_(INFO) << "IsEnabled end";

    GTEST_LOG_(INFO) << "SetEnabled start";
    instance->SetEnabled(false);
    GTEST_LOG_(INFO) << "SetEnabled end";

    GTEST_LOG_(INFO) << "IsEnabled start";
    EXPECT_FALSE(instance->IsEnabled());
    GTEST_LOG_(INFO) << "IsEnabled end";

    GTEST_LOG_(INFO) << "SetTouchExplorationEnabled start";
    instance->SetTouchExplorationEnabled(true);
    GTEST_LOG_(INFO) << "SetTouchExplorationEnabled end";

    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled start";
    EXPECT_TRUE(instance->IsTouchExplorationEnabled());
    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled end";

    GTEST_LOG_(INFO) << "GetAbilityList start";
    AbilityStateType type = AbilityStateType::ABILITY_STATE_ENABLE; 
    instance->GetAbilityList(0, type);
    GTEST_LOG_(INFO) << "GetAbilityList end";

    GTEST_LOG_(INFO) << "GetSuggestedInterval start";
    instance->GetSuggestedInterval(1, 1);
    GTEST_LOG_(INFO) << "GetSuggestedInterval end";

    GTEST_LOG_(INFO) << "GetAccessibilityCaptionProperties start";
    instance->GetAccessibilityCaptionProperties();
    GTEST_LOG_(INFO) << "GetAccessibilityCaptionProperties end";

    GTEST_LOG_(INFO) << "GetAccessibilityCaptionProperties start";
    EXPECT_TRUE(instance->IsAccessibilityCaptionEnabled());
    GTEST_LOG_(INFO) << "GetAccessibilityCaptionProperties end";

    GTEST_LOG_(INFO) << "SendEvent start";
    EventType eventType = EventType::TYPE_VIEW_CLICKED_EVENT;
    EXPECT_TRUE(instance->SendEvent(eventType, 1));
    GTEST_LOG_(INFO) << "SendEvent end";

    GTEST_LOG_(INFO) << "SendEvent start";
    AccessibilityEventInfo eventInfo {};
    EXPECT_FALSE(instance->SendEvent(eventInfo));
    eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    EXPECT_TRUE(instance->SendEvent(eventInfo));
    GTEST_LOG_(INFO) << "SendEvent end";

    GTEST_LOG_(INFO) << "SubscribeStateObserver start";
    EXPECT_FALSE(instance->SubscribeStateObserver(nullptr, 1));
    GTEST_LOG_(INFO) << "SubscribeStateObserver end";

    GTEST_LOG_(INFO) << "UnsubscribeStateObserver start";
    EXPECT_FALSE(instance->UnsubscribeStateObserver(nullptr));
    GTEST_LOG_(INFO) << "UnsubscribeStateObserver end";
}

/**
 * @tc.number: AccessibilityStateEvent_001
 * @tc.name: Function
 * @tc.desc: Test function
 */
HWTEST_F(ASACClientUnitTest, AccessibilityStateEvent_001, TestSize.Level1)
{
    AccessibilityStateEvent stateEvent {};
    AccessibilityStateEventType eventType = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;
    GTEST_LOG_(INFO) << "SetEventType start";
    stateEvent.SetEventType(eventType);
    EXPECT_TRUE(stateEvent.GetEventType() == AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
    GTEST_LOG_(INFO) << "SetEventType start";

    GTEST_LOG_(INFO) << "SetEventMsg start";
    string str = "test";
    stateEvent.SetEventMsg(str);
    EXPECT_TRUE(strcmp(stateEvent.GetEventMsg().c_str(), "test") == 0);
    GTEST_LOG_(INFO) << "SetEventMsg start";

    GTEST_LOG_(INFO) << "SetEventResult start";
    stateEvent.SetEventResult(1);
    EXPECT_TRUE(stateEvent.GetEventResult() == 1);
    GTEST_LOG_(INFO) << "SetEventResult start";
}
} //namespace Accessibility
} //namespace OHOS