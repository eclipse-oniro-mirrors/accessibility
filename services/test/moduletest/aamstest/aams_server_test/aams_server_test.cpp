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
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_stub_impl.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"
#include "accessibility_display_manager.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessible_ability_manager_service_state_proxy.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

extern OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> testStub;
extern int testStateType;
extern int testEventType;
extern int testWindowChangeTypes_;
extern int testWindowId;
extern int testInterrupt;
namespace OHOS {
namespace Accessibility {

class AAMSServerTest : public testing::Test {
public:

    AAMSServerTest() {}
    ~AAMSServerTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void AddAccessibleAbilityConnection();
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> aams_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> aastub_ = nullptr;
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
    // register bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    // Start AAMS
    aams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams_->OnStart();

    // Set display manager
    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    wmdInfo.width = 20;
    wmdInfo.height = 20;
    displayMgr.SetDisplay(wmdInfo);

    EXPECT_CALL(*mock_, GetUidByBundleName(_, _)).WillRepeatedly(Return(0));
}

void AAMSServerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AAMSServerTest TearDown";
    testStub = nullptr;
    testStateType = -1;
    testEventType = -1;
    testWindowChangeTypes_ = -1;
    testWindowId = -1;
    testInterrupt = -1;
    aams_->OnStop();
    delete mock_;
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
    aams_ = nullptr;
}

void AAMSServerTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AAMSServerTest AddAccessibleAbilityConnection";
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
    elementName_ = new AppExecFwk::ElementName(info.deviceId, info.bundleName, info.name);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("AAMSServerTest");
    aastub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(*elementName_, aastub_, 0);
    accountData_->AddInstalledAbility(*abilityInfo);
}

/**
 * @tc.number: AAMS_moduletest_SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent aams send a event to accessibility,and check the parm of event.
 *
 */
HWTEST_F(AAMSServerTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest SendEvent_001 start";
    //register AA
    AddAccessibleAbilityConnection();
    //make a event
    AccessibilityEventInfo eventInfo;
    eventInfo.SetEventType(EventType::TYPE_WINDOW_UPDATE);
    //aams send event
    aams_->SendEvent(eventInfo,0);
    sleep(1);
    //check aa proxy
    EXPECT_EQ(testEventType, int(EventType::TYPE_WINDOW_UPDATE));

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();

    GTEST_LOG_(INFO) << "AAMSServerTest SendEvent_001 end";
}

/**
 * @tc.number: AAMS_moduletest_RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 * step1: register a statecallback and check return value of func
 * step2: connect a accessibilityability and then trigger the stateback
 * step3::disconnect the accessibilityability and trigger the stateback
 */
HWTEST_F(AAMSServerTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest RegisterStateCallback_001 start";

    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);

    int ret = aams_->RegisterStateCallback(state,0);
    EXPECT_EQ(ret, int(AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED|
    AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED));

    AddAccessibleAbilityConnection();
    EXPECT_EQ(testStateType, int(AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED|
    AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED));

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    EXPECT_EQ(testStateType, int(AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED|
    AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED));

    auto accountData = aams_->GetCurrentAccountData();
    accountData->RemoveStateCallback(state);

    GTEST_LOG_(INFO) << "AAMSServerTest RegisterStateCallback_001 end";
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
    auto ret = aams_->GetAbilityList(0, AbilityStateType::ABILITY_STATE_ENABLE);
    EXPECT_EQ(int(ret.size()), 0);

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

    auto ret = aams_->GetAbilityList(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN,
     AbilityStateType::ABILITY_STATE_ENABLE);
    EXPECT_EQ(int(ret.size()), 1);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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

    AppExecFwk::AbilityInfo info;
    info.name = "TEST_ABILITY_NAME";
    info.bundleName = "TEST_BUNDLE_NAME";
    sptr<AccessibilityAbilityInfo> installAbilityInfo = new AccessibilityAbilityInfo(info);
    auto accountData = aams_->GetCurrentAccountData();

    /* add install ability dummy*/
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*installAbilityInfo);
    EXPECT_EQ(2, (int)accountData->GetInstalledAbilities().size());

    /* ABILITY_STATE_DISABLE */
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    auto ret = aams_->GetAbilityList(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN,stateType);
    EXPECT_EQ(int(ret.size()), 1);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    auto ret = aams_->GetAbilityList(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType);
    EXPECT_EQ(int(ret.size()), 0);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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

    auto ret = aams_->GetAbilityList(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN,
     AbilityStateType::ABILITY_STATE_INSTALLED);
    EXPECT_EQ(int(ret.size()), 1);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetAbilityList_005 end";
}

/**
 * @tc.number: AAMS_moduletest_RegisterInteractionOperation_001
 * @tc.name: RegisterInteractionOperation
 * @tc.desc: Test function RegisterInteractionOperation
 * Register a InteractionOperation and check account data and event detail.
 */
HWTEST_F(AAMSServerTest,RegisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest RegisterInteractionOperation_001 start";
    AddAccessibleAbilityConnection();
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    aams_->RegisterInteractionOperation(0,nullptr,0);
    sleep(1);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    EXPECT_EQ(testEventType, TYPE_WINDOW_UPDATE);
    EXPECT_EQ(testWindowId, 0);
    EXPECT_EQ(testWindowChangeTypes_, WindowUpdateType::WINDOW_UPDATE_ADDED);
    aams_->DeregisterInteractionOperation(0);
    sleep(1);
    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest RegisterInteractionOperation_001 end";
}

/**
 * @tc.number: AAMS_moduletest_DeregisterInteractionOperation_001
 * @tc.name: DeregisterInteractionOperation
 * @tc.desc: Test function DeregisterInteractionOperation
 * Deregister a InteractionOperation and check account data and event detail.
 */
HWTEST_F(AAMSServerTest,DeregisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest DeregisterInteractionOperation_001 start";
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    AddAccessibleAbilityConnection();
    aams_->RegisterInteractionOperation(0,nullptr,0);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    //wrong windowId
    aams_->DeregisterInteractionOperation(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    //true windowId
    aams_->DeregisterInteractionOperation(0);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    EXPECT_EQ(testEventType, TYPE_WINDOW_UPDATE);
    EXPECT_EQ(testWindowId, 0);
    EXPECT_EQ(testWindowChangeTypes_, WindowUpdateType::WINDOW_UPDATE_REMOVED);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest DeregisterInteractionOperation_001 end";
}

/**
 * @tc.number: AAMS_moduletest_Interrupt_001
 * @tc.name: Interrupt
 * @tc.desc: Test function Interrupt
 * Register ability and aams send Interrupt
 */
HWTEST_F(AAMSServerTest, Interrupt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest Interrupt_001 start";
    AddAccessibleAbilityConnection();
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    aams_->Interrupt(0);
    sleep(1);
    EXPECT_EQ(testInterrupt, 1);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest Interrupt_001 end";
}

/**
 * @tc.number: AAMS_moduletest_GetSuggestedInterval_001
 * @tc.name: GetSuggestedInterval
 * @tc.desc: Test function GetSuggestedInterval
 * step1: aams GetSuggestedInterval chheck return value
 * step2: register a AA, and check the return value
 */
HWTEST_F(AAMSServerTest, GetSuggestedInterval_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AAMSServerTest GetSuggestedInterval_001 start";

    auto ret = aams_->GetSuggestedInterval();
    EXPECT_EQ(int(ret) , 0);
    AddAccessibleAbilityConnection();
    ASSERT_TRUE(testStub);

    ret = aams_->GetSuggestedInterval();
    uint32_t uiInteractiveTimeout = 10000;
    uint32_t uiNoninteractiveTimeout = 2000;
    uint64_t result = (static_cast<uint64_t>(uiNoninteractiveTimeout) << INT32_BIT_NUM) | uiInteractiveTimeout;
    EXPECT_EQ(ret ,result);
    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest GetSuggestedInterval_001 end";
}

} // namespace Accessibility
} // namespace OHOS