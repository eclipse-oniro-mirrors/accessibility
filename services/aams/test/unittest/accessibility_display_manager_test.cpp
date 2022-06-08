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

namespace OHOS {
namespace Accessibility {
class AccessibilityDisplayManagerUnitTest : public ::testing::Test {
public:
    AccessibilityDisplayManagerUnitTest()
    {}
    ~AccessibilityDisplayManagerUnitTest()
    {}

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
{}

void AccessibilityDisplayManagerUnitTest::TearDown()
{}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_RegisterDisplayListener_001
 * @tc.name: RegisterDisplayListener
 * @tc.desc: Test function RegisterDisplayListener
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest,
    AccessibilityDisplayManager_Unittest_RegisterDisplayListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_RegisterDisplayListener_001 start";

    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(nullptr);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_RegisterDisplayListener_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_UnregisterDisplayListener_001
 * @tc.name: UnregisterDisplayListener
 * @tc.desc: Test function UnregisterDisplayListener
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest,
    AccessibilityDisplayManager_Unittest_UnregisterDisplayListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_UnregisterDisplayListener_001 start";

    Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_UnregisterDisplayListener_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDisplay_001
 * @tc.name: GetDisplay
 * @tc.desc: Test function GetDisplay
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDisplay_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_001 start";

    auto display = Singleton<AccessibilityDisplayManager>::GetInstance().GetDisplay(0);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDisplays_001
 * @tc.name: GetDisplays
 * @tc.desc: Test function GetDisplays
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDisplays_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplays_001 start";

    auto displays = Singleton<AccessibilityDisplayManager>::GetInstance().GetDisplays();

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplays_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001
 * @tc.name: GetDefaultDisplay
 * @tc.desc: Test function GetDefaultDisplay
 */
HWTEST_F(
    AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001 start";

    auto display = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplay();

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDefaultDisplayId_001
 * @tc.name: GetDefaultDisplayId
 * @tc.desc: Test function GetDefaultDisplayId
 */
HWTEST_F(
    AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDefaultDisplayId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplayId_001 start";

    uint64_t displayId = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplayId();
    GTEST_LOG_(INFO) << "DefaultDisplayId" << displayId;

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplayId_001 end";
}
} // namespace Accessibility
} // namespace OHOS