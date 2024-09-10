/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "accessibility_short_key_dialog.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityShortkeyDialogUnitTest : public ::testing::Test {
public:
    AccessibilityShortkeyDialogUnitTest()
    {}
    ~AccessibilityShortkeyDialogUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityShortkeyDialog> shortKeyDialog_ = nullptr;
};

void AccessibilityShortkeyDialogUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortkeyDialogUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityShortkeyDialogUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityShortkeyDialogUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityShortkeyDialogUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    shortKeyDialog_ = std::make_shared<AccessibilityShortKeyDialog>();
}

void AccessibilityShortkeyDialogUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    shortKeyDialog_ = nullptr;
}

/**
 * @tc.number: AccessibilityShortkeyDialog_Unittest_ConnectDialog_001
 * @tc.name: ConnectDialog
 * @tc.desc: Test function ConnectDialog
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortkeyDialogUnitTest, AccessibilityShortkeyDialog_Unittest_ConnectDialog_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortkeyDialog_Unittest_ConnectDialog_001 start";
    EXPECT_TRUE(shortKeyDialog_->ConnectDialog(ShortKeyDialogType::FUNCTION_SELECT));
    GTEST_LOG_(INFO) << "AccessibilityShortkeyDialog_Unittest_ConnectDialog_001 end";
}

/**
 * @tc.number: AccessibilityShortkeyDialog_Unittest_ConnectDialog_002
 * @tc.name: ConnectDialog
 * @tc.desc: Test function ConnectDialog
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortkeyDialogUnitTest, AccessibilityShortkeyDialog_Unittest_ConnectDialog_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortkeyDialog_Unittest_ConnectDialog_002 start";
    EXPECT_TRUE(shortKeyDialog_->ConnectDialog(ShortKeyDialogType::RECONFIRM));
    GTEST_LOG_(INFO) << "AccessibilityShortkeyDialog_Unittest_ConnectDialog_002 end";
}
} // namespace Accessibility
} // namespace OHOS