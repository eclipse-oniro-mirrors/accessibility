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

#include <vector>
#include <gtest/gtest.h>
#include "accessibility_display_manager.h"
#include "dummy.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

static WMDisplayInfo displayInfo;

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

    displayInfo.id = 1;
    displayInfo.width = 1;
    displayInfo.height = 1;
    displayInfo.phyWidth = 1;
    displayInfo.phyHeight = 1;
    displayInfo.vsync = 1;
    displayInfo.dpi = 203;

    AccessibilityDisplayManager::GetInstance().SetDisplay(displayInfo);
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
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDisplay_001
 * @tc.name: GetDisplay
 * @tc.desc: Test function GetDisplay
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDisplay_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_001 start";

    WMDisplayInfo result = AccessibilityDisplayManager::GetInstance().GetDisplay(displayInfo.id);
    EXPECT_EQ(result.id, displayInfo.id);
    EXPECT_EQ(result.width, displayInfo.width);
    EXPECT_EQ(result.height, displayInfo.height);
    EXPECT_EQ(result.phyWidth, displayInfo.phyWidth);
    EXPECT_EQ(result.phyHeight, displayInfo.phyHeight);
    EXPECT_EQ(result.vsync, displayInfo.vsync);
    EXPECT_EQ(result.dpi, displayInfo.dpi);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDisplay_002
 * @tc.name: GetDisplay
 * @tc.desc: Test function GetDisplay
 *           Get display which is not exit.
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDisplay_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_002 start";

    int ErrorDisplayId = 2;

    WMDisplayInfo result = AccessibilityDisplayManager::GetInstance().GetDisplay(ErrorDisplayId);
    EXPECT_EQ(result.id, displayInfo.id);
    EXPECT_EQ(result.width, displayInfo.width);
    EXPECT_EQ(result.height, displayInfo.height);
    EXPECT_EQ(result.phyWidth, displayInfo.phyWidth);
    EXPECT_EQ(result.phyHeight, displayInfo.phyHeight);
    EXPECT_EQ(result.vsync, displayInfo.vsync);
    EXPECT_EQ(result.dpi, displayInfo.dpi);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplay_002 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDisplays_001
 * @tc.name: GetDisplays
 * @tc.desc: Test function GetDisplays
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDisplays_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplays_001 start";

    vector<WMDisplayInfo> result = AccessibilityDisplayManager::GetInstance().GetDisplays();
    ASSERT_EQ((int)result.size(), 1);
    EXPECT_EQ(result[0].id, displayInfo.id);
    EXPECT_EQ(result[0].width, displayInfo.width);
    EXPECT_EQ(result[0].height, displayInfo.height);
    EXPECT_EQ(result[0].phyWidth, displayInfo.phyWidth);
    EXPECT_EQ(result[0].phyHeight, displayInfo.phyHeight);
    EXPECT_EQ(result[0].vsync, displayInfo.vsync);
    EXPECT_EQ(result[0].dpi, displayInfo.dpi);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDisplays_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001
 * @tc.name: GetDefaultDisplay
 * @tc.desc: Test function GetDefaultDisplay
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001 start";

    WMDisplayInfo result = AccessibilityDisplayManager::GetInstance().GetDefaultDisplay();
    EXPECT_EQ(result.id, displayInfo.id);
    EXPECT_EQ(result.width, displayInfo.width);
    EXPECT_EQ(result.height, displayInfo.height);
    EXPECT_EQ(result.phyWidth, displayInfo.phyWidth);
    EXPECT_EQ(result.phyHeight, displayInfo.phyHeight);
    EXPECT_EQ(result.vsync, displayInfo.vsync);
    EXPECT_EQ(result.dpi, displayInfo.dpi);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_GetDefaultDisplay_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_RegisterDisplayChangeListener_001
 * @tc.name: RegisterDisplayChangeListener
 * @tc.desc: Test function RegisterDisplayChangeListener
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_RegisterDisplayChangeListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_RegisterDisplayChangeListener_001 start";

    AccessibilityDisplayManager::GetInstance().RegisterDisplayChangeListener();

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_RegisterDisplayChangeListener_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_OnScreenPlugin_001
 * @tc.name: OnScreenPlugin
 * @tc.desc: Test function OnScreenPlugin
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_OnScreenPlugin_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnScreenPlugin_001 start";

    AccessibilityDisplayManager::GetInstance().OnScreenPlugin(0);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnScreenPlugin_001 end";
}

/**
 * @tc.number: AccessibilityDisplayManager_Unittest_OnScreenPlugout_001
 * @tc.name: OnScreenPlugout
 * @tc.desc: Test function OnScreenPlugout
 */
HWTEST_F(AccessibilityDisplayManagerUnitTest, AccessibilityDisplayManager_Unittest_OnScreenPlugout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnScreenPlugout_001 start";

    AccessibilityDisplayManager::GetInstance().OnScreenPlugout(0);

    GTEST_LOG_(INFO) << "AccessibilityDisplayManager_Unittest_OnScreenPlugout_001 end";
}