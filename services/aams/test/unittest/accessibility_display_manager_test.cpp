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
#include <vector>

#include "accessibility_display_manager.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityDisplayManagerUnitTest : public ::testing::Test {
public:
    AccessibilityDisplayManagerUnitTest() {}
    ~AccessibilityDisplayManagerUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilityDisplayManagerUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDisplayManagerUnitTest Start ######################";
}

void AccessibilityDisplayManagerUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDisplayManagerUnitTest End ######################";
}

void AccessibilityDisplayManagerUnitTest::SetUp()
{
}

void AccessibilityDisplayManagerUnitTest::TearDown()
{
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_OnCreate_001
 * @tc.name: OnScreeOnCreatenPlugin
 * @tc.desc: Test function OnCreate
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_OnCreate_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnCreate_001 start";

    AccessibilityDisplayManager::GetInstance().OnCreate(0);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnCreate_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_OnDestroy_001
 * @tc.name: OnDestroy
 * @tc.desc: Test function OnDestroy
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_OnDestroy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnDestroy_001 start";

    AccessibilityDisplayManager::GetInstance().OnDestroy(0);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnDestroy_001 end";
}
} // namespace Accessibility
} // namespace OHOS