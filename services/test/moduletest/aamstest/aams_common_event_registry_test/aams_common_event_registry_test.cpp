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
#include "accessibility_common_helper.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "common_event_manager.h"
#include "iservice_registry.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonEventRegistryTest : public ::testing::Test {
public:
    AccessibilityCommonEventRegistryTest()
    {}
    ~AccessibilityCommonEventRegistryTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void AddAccessibleAbilityConnection();

    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

void AccessibilityCommonEventRegistryTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistryTest SetUpTestCase";
    // Start AAMS
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AccessibilityCommonEventRegistryTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistryTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityCommonEventRegistryTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
}

void AccessibilityCommonEventRegistryTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
}

void AccessibilityCommonEventRegistryTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistryTest AddAccessibleAbilityConnection";
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);

    accountData_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    AAConnection_ = new AccessibleAbilityConnection(accountData_->GetAccountId(), 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("name", "bundleName", "id");
    aastub_ = new AccessibleAbilityChannel(accountData_->GetAccountId(), abilityInfo->GetId());
    AAConnection_->OnAbilityConnectDoneSync(*elementName_, aastub_);
    accountData_->AddInstalledAbility(*abilityInfo);
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_ModuleTest_PackageChanged_001
 * @tc.name: PackageChanged
 * @tc.desc: There is a connected ability. The package has changed. Empty all install ability.
 */
HWTEST_F(AccessibilityCommonEventRegistryTest, AccessibilityCommonEvent_ModuleTest_PackageChanged_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_ModuleTest_PackageChanged_001 start";
    AddAccessibleAbilityConnection();
    EXPECT_EQ(1, int(accountData_->GetInstalledAbilities().size()));
    // PackageChanged
    std::string bundleName = "bundleName";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
    EXPECT_EQ(1, int(accountData_->GetInstalledAbilities().size()));

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_ModuleTest_PackageChanged_001 end";
}
} // namespace Accessibility
} // namespace OHOS