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
#include <memory>

#include "ability_context.h"
#include "accessibility_element_operator.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_system_ability_client.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_manager_service_impl.h"
#include "system_ability_definition.h"


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

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "ASACClientUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "ASACClientUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "ASACClientUnitTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "ASACClientUnitTest TearDown()";
    }
};

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
} // namespace Accessibility
} // namespace OHOS