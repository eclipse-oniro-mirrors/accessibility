/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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
#include "iservice_registry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint8_t TEST_NUM_2 = 2;
} // namespace
class AAMSServerTest : public testing::Test {
public:
    AAMSServerTest()
    {}
    ~AAMSServerTest()
    {}

    sptr<AccessibleAbilityChannel> aastub_ =  nullptr;
    sptr<AccessibleAbilityChannel> aacs_ = nullptr;

    int32_t tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void AddConnection();
    void TearDown();
    void WritefileAll(const char* fname, const char* data) const;
};

void AAMSServerTest::SetUpTestCase(void)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AAMSServerTest::TearDownTestCase(void)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AAMSServerTest::SetUp()
{
    GTEST_LOG_(INFO) << "AAMSServerTest ModuleTest SetUp";

    // Add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_KEY_EVENT_OBSERVER);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sleep(TEST_NUM_2);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, aastub_);
}

void AAMSServerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AAMSServerTest ModuleTest TearDown";
    aastub_ = nullptr;
}

void AAMSServerTest::WritefileAll(const char* fname, const char* data) const
{
    FILE* fp = nullptr;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

/**
 * @tc.number: AAMS_moduletest_SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent aams send event to accessibility,and check the parm of event.
 *
 */
HWTEST_F(AAMSServerTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestSendEvent_001 start";
    // make an event
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    int32_t tempWindowId = aams.GetFocusWindowId();
    AccessibilityEventInfo evtInf(tempWindowId, WINDOW_UPDATE_ACTIVE);
    evtInf.SetEventType(EventType::TYPE_WINDOW_UPDATE);
    AccessibilityEventInfoParcel evtInfParcel(evtInf);
    // aams send event
    aams.SendEvent(evtInfParcel, 0);
    sleep(1);
    // check aa proxy
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestEventType(), int(EventType::TYPE_WINDOW_UPDATE));
    GTEST_LOG_(INFO) << "AAMSServerTestSendEvent_001 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * The func with Illegal parameter 1
 */
HWTEST_F(AAMSServerTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_001 start";
    std::vector<AccessibilityAbilityInfoParcel> infos;
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto ret = aams.GetAbilityList(0, AbilityStateType::ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(RET_OK, ret);
    EXPECT_EQ(infos.size(), 0);

    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_001 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_002
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Enable Ability list.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_002 start";
    std::vector<AccessibilityAbilityInfoParcel> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, AbilityStateType::ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(infos.size(), 0);
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_002 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_003
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Disable Ability list with 2 installed ability accessibility and a connected accessibility ability.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_003 start";
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
    std::vector<AccessibilityAbilityInfoParcel> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType, infos);
    EXPECT_EQ(infos.size(), 1);
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_003 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_004
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Disabled Ability list.
 */
HWTEST_F(AAMSServerTest, GetAbilityList_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_004 start";
    int32_t stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    std::vector<AccessibilityAbilityInfoParcel> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType, infos);
    EXPECT_EQ(infos.size(), 1);
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_004 end";
}

/**
 * @tc.number: AAMS_moduletest_GetAbilityList_005
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 * Get Installed ability
 */
HWTEST_F(AAMSServerTest, GetAbilityList_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_005 start";
    std::vector<AccessibilityAbilityInfoParcel> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN,
        AbilityStateType::ABILITY_STATE_INSTALLED, infos);
    EXPECT_EQ(infos.size(), 1);
    GTEST_LOG_(INFO) << "AAMSServerTestGetAbilityList_005 end";
}

/**
 * @tc.number: AAMS_moduletest_RegisterElementOperatorByWindowId_001
 * @tc.name: RegisterElementOperatorByWindowId
 * @tc.desc: Test function RegisterElementOperatorByWindowId
 * Register a ElementOperator and check account data and event detail.
 */
HWTEST_F(AAMSServerTest, RegisterElementOperatorByWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestRegisterElementOperatorByWindowId_001 start";

    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    EXPECT_EQ(RET_OK, aams.RegisterElementOperatorByWindowId(0, nullptr));
    sleep(1);
    GTEST_LOG_(INFO) << "RegisterElementOperatorByWindowId OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    aams.DeregisterElementOperatorByWindowId(0);
    GTEST_LOG_(INFO) << "AAMSServerTestRegisterElementOperatorByWindowId_001 end";
}

/**
 * @tc.number: AAMS_moduletest_DeregisterElementOperatorByWindowId_001
 * @tc.name: DeregisterElementOperatorByWindowId
 * @tc.desc: Test function DeregisterElementOperatorByWindowId
 * Deregister a ElementOperator and check account data and event detail.
 */
HWTEST_F(AAMSServerTest, DeregisterElementOperatorByWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTestDeregisterElementOperatorByWindowId_001 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    aams.RegisterElementOperatorByWindowId(0, nullptr);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // wrong windowId
    aams.DeregisterElementOperatorByWindowId(1);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // true windowId
    aams.DeregisterElementOperatorByWindowId(0);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);
    GTEST_LOG_(INFO) << "AAMSServerTestDeregisterElementOperatorByWindowId_001 end";
}
} // namespace Accessibility
} // namespace OHOS