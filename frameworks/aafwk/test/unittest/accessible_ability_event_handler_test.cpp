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
#include "accessible_ability_event_handler.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

const string AA_UT_NAME = "AccessibleAbilityEventHandlerUnitTest";

class AccessibleAbilityEventHandlerUnitTest : public ::testing::Test {
public:
    AccessibleAbilityEventHandlerUnitTest() {}
    ~AccessibleAbilityEventHandlerUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibleAbilityEventHandlerUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityEventHandlerUnitTest Start ######################";
}

void AccessibleAbilityEventHandlerUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityEventHandlerUnitTest End ######################";
}

void AccessibleAbilityEventHandlerUnitTest::SetUp()
{}

void AccessibleAbilityEventHandlerUnitTest::TearDown()
{}

/**
 * @tc.number: AccessibleAbilityEventHandler_Unittest_ProcessEvent_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibleAbilityEventHandlerUnitTest, AccessibleAbilityEventHandler_Unittest_ProcessEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityEventHandler_Unittest_ProcessEvent_001 start";

    uint32_t innerEventId = 0;
    int flagParam = 0;

    shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(AA_UT_NAME);
    shared_ptr<AccessibleAbilityEventHandler> accessibleAbilityEventHandler =
        make_shared<AccessibleAbilityEventHandler>(runner);
    auto event = AppExecFwk::InnerEvent::Get(innerEventId, flagParam);

    accessibleAbilityEventHandler->ProcessEvent(event);

    GTEST_LOG_(INFO) << "AccessibleAbilityEventHandler_Unittest_ProcessEvent_001 end";
}
