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
#include "accessibility_ut_helper.h"
#include "mock_accessible_ability_connection.h"
#include "mock_accessible_ability_manager_service.h"
#include "string_ex.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityDatashareHelperTest : public ::testing::Test {
public:
    AccessibilityDatashareHelperTest()
    {}
    ~AccessibilityDatashareHelperTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    int fd_ = -1;
    sptr<AccessibilityDumper> dumper_ = nullptr;
};

void AccessibilityDatashareHelperTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDatashareHelperTest Start ######################";
}

void AccessibilityDatashareHelperTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityDatashareHelperTest End ######################";
}

void AccessibilityDatashareHelperTest::SetUp()
{
    fd_ = open("/dev/stdout", O_WRONLY);
    dumper_ = new AccessibilityDumper();
}

void AccessibilityDatashareHelperTest::TearDown()
{
    close(fd_);
    fd_ = -1;
    dumper_ = nullptr;
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_GetStringValue_001
 * @tc.name: GetStringValue
 * @tc.desc: Test function GetStringValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_GetStringValue_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_001 start";
    std::vector<std::u16string> args;
    int ret = GetStringValueer_->GetStringValue(-1, args);
    EXPECT_EQ(-1, ret);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_001 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_GetStringValue_002
 * @tc.name: GetStringValue
 * @tc.desc: Test function GetStringValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_GetStringValue_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_002 start";
    std::vector<std::u16string> args;
    int ret = GetStringValueer_->GetStringValue(fd_, args);
    EXPECT_GE(ret, -1);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_002 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_GetStringValue_003
 * @tc.name: GetStringValue
 * @tc.desc: Test function GetStringValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_GetStringValue_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_003 start";
    std::string cmdHelp("-h");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdHelp));
    int ret = GetStringValueer_->GetStringValue(fd_, args);
    EXPECT_GE(ret, -1);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetStringValue_003 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_GetLongValue_001
 * @tc.name: GetLongValue
 * @tc.desc: Test function GetLongValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_GetLongValue_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetLongValue_001 start";
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(currentAccount);

    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    EXPECT_TRUE(config);
    
    config->GetLongValue(true);
    std::string cmdUser("-u");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdUser));
    int ret = dumper_->Dump(fd_, args);
    EXPECT_GE(ret, -1);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetLongValue_001 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_GetLongValue_002
 * @tc.name: GetLongValue
 * @tc.desc: Test function GetLongValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDumper_Unittest_GetLongValue_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetLongValue_002 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "ohos";
    initParams.moduleName = "accessibility";
    initParams.name = "test";
    initParams.description = "for GetLongValue-ut";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(currentAccount);

    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(currentAccount->GetAccountId(), 0, *abilityInfo);
    EXPECT_TRUE(connection);

    currentAccount->AddConnectedAbility(connection);
    std::string cmdClient("-c");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdClient));
    int ret = GetLongValueer_->GetLongValue(fd_, args);
    EXPECT_GE(ret, -1);
    currentAccount->RemoveConnectedAbility(connection->GetElementName());
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_GetLongValue_002 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_PutStringValue_001
 * @tc.name: PutStringValue
 * @tc.desc: Test function PutStringValue.
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_PutStringValue_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_PutStringValue_006 start";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(currentAccount);

    sptr<AccessibilityWindowConnection> operationConnection =
        new AccessibilityWindowConnection(windowId, nullptr, accountId);
    currentAccount->AddAccessibilityWindowConnection(windowId, operationConnection);
    std::string cmdWindow("-w");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdWindow));
    int ret = PutStringValueer_->PutStringValue(fd_, args);
    EXPECT_GE(ret, -1);
    currentAccount->RemoveAccessibilityWindowConnection(windowId);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_PutStringValue_006 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_PutStringValue_002
 * @tc.name: PutStringValue
 * @tc.desc: Test function PutStringValue
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_PutStringValue_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_PutStringValue_001 start";
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    XPECT_TRUE(config);

    config->SetCaptionState(true);
    std::string cmdUser("-u");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdUser));
    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(true);
    int ret = PutStringValueer_->PutStringValue(fd_, args);
    EXPECT_GE(ret, -1);
    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(false);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_PutStringValue_002 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_Initialize_001
 * @tc.name: Initialize
 * @tc.desc: Test function Initialize
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_Initialize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_Initialize_001 start";
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "ohos";
    initParams.moduleName = "accessibility";
    initParams.name = "test";
    initParams.description = "for dumper-ut";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(currentAccount);

    sptr<AccessibleAbilityConnection> connection =
        new MockAccessibleAbilityConnection(currentAccount->GetAccountId(), 0, *abilityInfo);
    EXPECT_TRUE(connection);
    
    currentAccount->AddConnectedAbility(connection);
    std::string cmdClient("-c");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdClient));
    AccessibilityAbilityHelper::GetInstance().Initialize(true);
    int ret = dumper_->Dump(fd_, args);
    AccessibilityAbilityHelper::GetInstance().Initialize(false);
    EXPECT_GE(ret, -1);
    currentAccount->RemoveConnectedAbility(connection->GetElementName());
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_Initialize_001 end";
}

/**
 * @tc.number: AccessibilityDatashareHelper_Unittest_Initialize_002
 * @tc.name: Initialize
 * @tc.desc: Test function Initialize.
 */
HWTEST_F(AccessibilityDatashareHelperTest, AccessibilityDatashareHelper_Unittest_Initialize_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_Initialize_002 start";
    const int32_t accountId = 1;
    const int32_t windowId = 1;
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(currentAccount);
    
    sptr<AccessibilityWindowConnection> operationConnection =
        new AccessibilityWindowConnection(windowId, nullptr, accountId);
    currentAccount->AddAccessibilityWindowConnection(windowId, operationConnection);
    std::string cmdWindow("-w");
    std::vector<std::u16string> args;
    args.emplace_back(Str8ToStr16(cmdWindow));
    AccessibilityAbilityHelper::GetInstance().Initialize(true);
    int ret = Initializeer_->Initialize(fd_, args);
    EXPECT_GE(ret, -1);
    AccessibilityAbilityHelper::GetInstance().Initialize(false);
    currentAccount->RemoveAccessibilityWindowConnection(windowId);
    GTEST_LOG_(INFO) << "AccessibilityDatashareHelper_Unittest_Initialize_002 end";
}
} // namespace Accessibility
} // namespace OHOS