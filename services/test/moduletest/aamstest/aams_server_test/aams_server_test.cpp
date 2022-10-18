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
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_manager_state_observer_proxy.h"
#include "accessible_ability_manager_state_observer_stub.h"
#include "iservice_registry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AAMSServerTest : public testing::Test {
public:
    AAMSServerTest()
    {}
    ~AAMSServerTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
    void AddAccessibleAbilityConnection();
    void WritefileAll(const char* fname, const char* data);
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

void AAMSServerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AAMSServerTest SetUpTestCase";
}

void AAMSServerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AAMSServerTest TearDownTestCase";
}

void AAMSServerTest::SetUp()
{
    GTEST_LOG_(INFO) << "AAMSServerTest SetUp";

    // Start AAMS
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AAMSServerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AAMSServerTest TearDown";
    AccessibilityHelper::GetInstance().SetTestStub(nullptr);
    AccessibilityHelper::GetInstance().SetTestStateType(-1);
    AccessibilityHelper::GetInstance().SetTestEventType(-1);
    AccessibilityHelper::GetInstance().SetTestWindowChangeTypes(-1);
    AccessibilityHelper::GetInstance().SetTestWindowId(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
}

void AAMSServerTest::WritefileAll(const char* fname, const char* data)
{
    FILE* fp;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AAMSServerTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AAMSServerTest AddAccessibleAbilityConnection";
    // accessibleAbility connection
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    std::string deviceId;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);

    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    accountData_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    AAConnection_ = new AccessibleAbilityConnection(accountData_->GetAccountId(), 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName(deviceId, initParams.bundleName, initParams.name);
    aastub_ = new AccessibleAbilityChannel(accountData_->GetAccountId(), abilityInfo->GetId());
    AAConnection_->OnAbilityConnectDoneSync(*elementName_, aastub_);
    accountData_->AddInstalledAbility(*abilityInfo);
}

/**
 * @tc.number: AAMS_moduletest_SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent aams send event to accessibility,and check the parm of event.
 *
 */
HWTEST_F(AAMSServerTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest SendEvent_001 start";
    // register AA
    AddAccessibleAbilityConnection();
    // make an event
    AccessibilityEventInfo eventInfo;
    eventInfo.SetEventType(EventType::TYPE_WINDOW_UPDATE);
    // aams send event
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
    sleep(1);
    // check aa proxy
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestEventType(), int(EventType::TYPE_WINDOW_UPDATE));

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();

    GTEST_LOG_(INFO) << "AAMSServerTest SendEvent_001 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * The func with Illegal parameter 1
 */
HWTEST_F(AAMSServerTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_001 start";
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto ret = aams.GetAbilityList(0, AbilityStateType::ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(RET_OK, ret);
    EXPECT_EQ(infos.size(), 0);

    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_001 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_002
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Enable Ability list.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_002 start";
    AddAccessibleAbilityConnection();

    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, AbilityStateType::ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(infos.size(), 1);

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_002 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_003
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Disable Ability list with 2 installed ability accessibility and a connected accessibility ability.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_003 start";

    AddAccessibleAbilityConnection();

    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> installAbilityInfo =
        std::make_shared<AccessibilityAbilityInfo>(initParams);
    installAbilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    installAbilityInfo->SetPackageName("123");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();

    // add install ability dummy
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*installAbilityInfo);
    EXPECT_EQ(2, (int)accountData->GetInstalledAbilities().size());

    // ABILITY_STATE_DISABLE
    int32_t stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType, infos);
    EXPECT_EQ(infos.size(), 1);

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_003 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_004
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Disabled Ability list.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_004 start";

    AddAccessibleAbilityConnection();
    int32_t stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType, infos);
    EXPECT_EQ(infos.size(), 0);

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_004 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_005
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Installed ability
 */
HWTEST_F(AAMSServerTest, GetAbilityList_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_005 start";

    AddAccessibleAbilityConnection();

    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN,
        AbilityStateType::ABILITY_STATE_INSTALLED, infos);
    EXPECT_EQ(infos.size(), 1);

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_005 end";
}

/**
 * @tc.number: AAMS_moduletest_RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 * Register a ElementOperator and check account data and event detail.
 */
HWTEST_F(AAMSServerTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest RegisterElementOperator_001 start";
    AddAccessibleAbilityConnection();
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    EXPECT_EQ(RET_OK, aams.RegisterElementOperator(0, nullptr));
    sleep(1);
    GTEST_LOG_(INFO) << "RegisterElementOperator OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    aams.DeregisterElementOperator(0);
    sleep(1);
    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest RegisterElementOperator_001 end";
}

/**
 * @tc.number: AAMS_moduletest_DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 * Deregister a ElementOperator and check account data and event detail.
 */
HWTEST_F(AAMSServerTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest DeregisterElementOperator_001 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    AddAccessibleAbilityConnection();
    sleep(1);
    aams.RegisterElementOperator(0, nullptr);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // wrong windowId
    aams.DeregisterElementOperator(1);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // true windowId
    aams.DeregisterElementOperator(0);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    AAConnection_->OnAbilityDisconnectDoneSync(*elementName_);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest DeregisterElementOperator_001 end";
}
} // namespace Accessibility
} // namespace OHOS