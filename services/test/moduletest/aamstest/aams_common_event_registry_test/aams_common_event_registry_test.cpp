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
#include "common_event_manager.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_client_stub_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::EventFwk;
using namespace OHOS::Accessibility;
using namespace std;

class AccessibilityCommonEventRegistryTest : public ::testing::Test {
public:
    AccessibilityCommonEventRegistryTest() {}
    ~AccessibilityCommonEventRegistryTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void AddAccessibleAbilityConnection();

    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> aams_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

void AccessibilityCommonEventRegistryTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventRegistryTest Start ######################";
}

void AccessibilityCommonEventRegistryTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventRegistryTest End ######################";
}

void AccessibilityCommonEventRegistryTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // register bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    // Start AAMS
    aams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams_->OnStart();
    EXPECT_CALL(*mock_, GetUidByBundleName(_, _)).WillRepeatedly(Return(0));
}

void AccessibilityCommonEventRegistryTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    aams_->OnStop();
    delete mock_;
    aams_ = nullptr;
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
}

void AccessibilityCommonEventRegistryTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistryTest AddAccessibleAbilityConnection";
    // accessibleAbility connection
    AppExecFwk::AbilityInfo info;
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);
    aams_->GetBundleMgrProxy()->QueryAbilityInfo(want, info);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    accountData_ = aams_->GetCurrentAccountData();
    AAConnection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("name","bundleName","id");
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    aastub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(*elementName_, aastub_, 0);
    accountData_->AddInstalledAbility(*abilityInfo);
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_SwitchedUser_001
 * @tc.name: SwitchedUser
 * @tc.desc: Switched user, the old user data is cleared and new user is added.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_SwitchedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_SwitchedUser_001 start";
    AddAccessibleAbilityConnection();
    EXPECT_EQ(0, aams_->GetCurrentAccountId());
    EXPECT_EQ(1, int(accountData_->GetInstalledAbilities().size()));
    /* SwitchedUser */
    GTEST_LOG_(INFO) << "SwitchedUser start";
    aams_->SwitchedUser(2);
    EXPECT_EQ(0, int(accountData_->GetInstalledAbilities().size()));
    EXPECT_EQ(2, aams_->GetCurrentAccountId());

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_SwitchedUser_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_PackageRemoved_001
 * @tc.name: PackageRemoved
 * @tc.desc: There is a connected ability. The package is Removed. Remove connecting ability and install ability.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_PackageRemoved_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageRemoved_001 start";
    AddAccessibleAbilityConnection();
    /* add enable ability */
    accountData_->AddEnabledAbility(*elementName_);
    /* add connecting ability */
    accountData_->AddConnectingA11yAbility(*elementName_);
    EXPECT_EQ(1, int(accountData_->GetEnabledAbilities().size()));
    EXPECT_EQ(1, int(accountData_->GetConnectingA11yAbilities().size()));
    std::string str = "bundleName";
    /* PackageRemoved */
    aams_->PackageRemoved(str);
    EXPECT_EQ(0, int(accountData_->GetEnabledAbilities().size()));
    EXPECT_EQ(0, int(accountData_->GetConnectingA11yAbilities().size()));

    accountData_->ClearInstalledAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageRemoved_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_PackageUpdateFinished_001
 * @tc.name: PackageUpdateFinished
 * @tc.desc: There is a connected ability. The package is added to the connecting ability during the update.
 *           After the package update. Remove connecting ability.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_PackageUpdateFinished_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageUpdateFinished_001 start";
    AddAccessibleAbilityConnection();
    accountData_->AddConnectingA11yAbility(*elementName_);
    EXPECT_EQ(1, int(accountData_->GetConnectingA11yAbilities().size()));
    /* PackageUpdateFinished */
    std::string str = "bundleName";
    aams_->PackageUpdateFinished(str);
    EXPECT_EQ(0, int(accountData_->GetConnectingA11yAbilities().size()));

    accountData_->ClearInstalledAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageUpdateFinished_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_PackageChanged_001
 * @tc.name: PackageChanged
 * @tc.desc: There is a connected ability. The package has changed. Empty all install ability.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_PackageChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageChanged_001 start";
    AddAccessibleAbilityConnection();
    EXPECT_EQ(1, int(accountData_->GetInstalledAbilities().size()));
    /* PackageChanged */
    aams_->PackageChanged();
    EXPECT_EQ(0, int(accountData_->GetInstalledAbilities().size()));

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PackageChanged_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_UnlockedUser_001
 * @tc.name: UnlockedUser
 * @tc.desc: There is an enabled ability which is not connected. The ability will be connected after Unlocking account.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_UnlockedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_UnlockedUser_001 start";
    ASSERT_TRUE(aams_);

    GTEST_LOG_(INFO) << "Add an installed ability.";
    AppExecFwk::AbilityInfo info;
    AAFwk::Want want;
    AppExecFwk::ElementName elementname;
    elementname.SetAbilityName("com.example.aamsModuleTest.MainAbility");
    elementname.SetBundleName("com.example.aamsModuleTest");
    want.SetElement(elementname);
    aams_->GetBundleMgrProxy()->QueryAbilityInfo(want, info);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    aams_->GetCurrentAccountData()->AddInstalledAbility(*abilityInfo);
    aams_->GetCurrentAccountData()->AddEnabledAbility(elementname);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetInstalledAbilities().size(), 1);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetEnabledAbilities().size(), 1);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 0);

    GTEST_LOG_(INFO) << "Unlock account.";
    int accountId = aams_->GetCurrentAccountId();
    aams_->UnlockedUser((int32_t)accountId);
    AAConnection_ = new AccessibleAbilityConnection(aams_->GetCurrentAccountData(), 0, *abilityInfo);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    aastub_ = new Accessibility::AccessibleAbilityClientStubImpl(
        std::make_shared<Accessibility::AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(elementname, aastub_, 0);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 1);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_UnlockedUser_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_RemovedUser_001
 * @tc.name: RemovedUser
 * @tc.desc: 1. Swich account0 to account1.
 *           2. Remove account0.
 *           3. Swich account1 to account0. The account0 is not the previous one, it's a new one.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_RemovedUser_001 start";
    ASSERT_TRUE(aams_);

    GTEST_LOG_(INFO) << "Add an account0.";
    AddAccessibleAbilityConnection();
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetInstalledAbilities().size(), 1);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 1);

    GTEST_LOG_(INFO) << "Switch to account1.";
    int32_t accountId = 1;
    aams_->SwitchedUser(accountId);
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);

    GTEST_LOG_(INFO) << "Remove account0.";
    aams_->RemovedUser(0);

    GTEST_LOG_(INFO) << "Switch to account0 and account0 is not the previous one, it's a new one.";
    accountId = 0;
    aams_->SwitchedUser(accountId);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetInstalledAbilities().size(), 0);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_RemovedUser_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_PresentUser_001
 * @tc.name: PresentUser
 * @tc.desc: After presenting user, the ability which is connected before is still connected.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEventRegistry_ModuleTest_PresentUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PresentUser_001 start";
    ASSERT_TRUE(aams_);

    GTEST_LOG_(INFO) << "Add an account.";
    AddAccessibleAbilityConnection();
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetInstalledAbilities().size(), 1);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 1);

    GTEST_LOG_(INFO) << "Present account";
    aams_->PresentUser();

    GTEST_LOG_(INFO) << "Check account data";
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetInstalledAbilities().size(), 1);
    EXPECT_EQ((int)aams_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 1);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_ModuleTest_PresentUser_001 end";
}