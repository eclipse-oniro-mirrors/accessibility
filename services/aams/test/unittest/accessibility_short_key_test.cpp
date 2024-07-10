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
#include <map>
#include <memory>
#include "accessibility_short_key.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityShortKeyUnitTest : public ::testing::Test {
public:
    AccessibilityShortKeyUnitTest()
    {}
    ~AccessibilityShortKeyUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityShortKey> shortKey_ = nullptr;
};

void AccessibilityShortKeyUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortKeyUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityShortKeyUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortKeyUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityShortKeyUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    shortKey_ = std::make_shared<AccessibilityShortKey>();
}

void AccessibilityShortKeyUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    shortKey_ = nullptr;
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_Register_001
 * @tc.name: Register
 * @tc.desc: Test function Register
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_Register_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_Register_001 start";
    shortKey_->Register();
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: Accessibility_Short_Key_UnitTest_AccessibilityShortKey001
 * @tc.name: AccessibilityShortKey
 * @tc.desc: Test function AccessibilityShortKey(power key cancel)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessibility_Short_Key_UnitTest_AccessibilityShortKey001 start";
    TearDownTestCase();
    AccessibilityShortKey *test = new AccessibilityShortKey();
    delete test;
    SetUpTestCase();
    GTEST_LOG_(INFO) << "Accessibility_Short_Key_UnitTest_AccessibilityShortKey001 end";
}
} // namespace Accessibility
} // namespace OHOS