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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include "accessibility_dumper.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessible_ability_manager_service.h"
#include "string_ex.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityDumperUnitTest : public ::testing::Test {
public:
    AccessibilityDumperUnitTest()
    {}
    ~AccessibilityDumperUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    int fd_ = -1;
    sptr<AccessibilityDumper> dumper_ = nullptr;
};

void AccessibilityDumperUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDumperUnitTest Start ######################";
}

void AccessibilityDumperUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDumperUnitTest End ######################";
}

void AccessibilityDumperUnitTest::SetUp()
{
    fd_ = open("/dev/stdout", O_WRONLY);
    dumper_ = new AccessibilityDumper();
}

void AccessibilityDumperUnitTest::TearDown()
{
    close(fd_);
    dumper_ = nullptr;
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_001
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_001 start";

    std::vector<std::u16string> args;
    int ret = dumper_->Dump(-1, args);
    EXPECT_EQ(-1, ret);

    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_001 end";
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_002
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_002 start";

    std::vector<std::u16string> args;
    int ret = dumper_->Dump(fd_, args);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_002 end";
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_003
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_003 start";

    std::string cmdHelp("-h");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdHelp));
    int ret = dumper_->Dump(fd_, args);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_003 end";
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_004
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_004 start";
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        GTEST_LOG_(ERROR) << "Account data is nullptr";
        return;
    }
    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    if (!config) {
        GTEST_LOG_(ERROR) << "Config is nullptr";
        return;
    }
    config->SetCaptionState(true);

    std::string cmdUser("-u");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdUser));
    int ret = dumper_->Dump(fd_, args);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_004 end";
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_005
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_005 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "ohos";
    initParams.moduleName = "accessibility";
    initParams.name = "test";
    initParams.description = "for dumper-ut";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        GTEST_LOG_(ERROR) << "Account data is nullptr";
        return;
    }
    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(currentAccount->GetAccountId(), 0, *abilityInfo);
    if (!connection) {
        GTEST_LOG_(ERROR) << "Connection is nullptr";
        return;
    }
    currentAccount->AddConnectedAbility(connection);

    std::string cmdClient("-c");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdClient));
    int ret = dumper_->Dump(fd_, args);
    EXPECT_EQ(0, ret);

    currentAccount->RemoveConnectedAbility(connection->GetElementName());
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_005 end";
}

/**
 * @tc.number: AccessibilityDumper_Unittest_Dump_006
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibilityDumperUnitTest, AccessibilityDumper_Unittest_Dump_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_006 start";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        GTEST_LOG_(ERROR) << "Account data is nullptr";
        return;
    }
    sptr<AccessibilityWindowConnection> operationConnection =
        new AccessibilityWindowConnection(windowId, nullptr, accountId);
    /* add asacConnections */
    currentAccount->AddAccessibilityWindowConnection(windowId, operationConnection);

    std::string cmdWindow("-w");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdWindow));
    int ret = dumper_->Dump(fd_, args);
    EXPECT_EQ(0, ret);

    currentAccount->RemoveAccessibilityWindowConnection(windowId);
    GTEST_LOG_(INFO) << "AccessibilityDumper_Unittest_Dump_006 end";
}
} // namespace Accessibility
} // namespace OHOS