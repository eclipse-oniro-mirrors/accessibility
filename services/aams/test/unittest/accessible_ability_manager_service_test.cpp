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

#include <optional>
#include <gtest/gtest.h>
#include "accessible_ability_manager_service.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessible_ability_manager_service_state_proxy.h"
#include "accessible_ability_client_stub_impl.h"
#include "mock_bundle_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "accessibility_display_manager.h"
#include "dummy.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

extern int testChannalId;
extern int testEventType;
extern int testInterrupt;
extern int testDisplayId;
extern int testStateType;
extern int testGesture; 
class AccessibleAbilityManagerServiceUnitTest : public ::testing::Test {
public:
    AccessibleAbilityManagerServiceUnitTest() {}
    ~AccessibleAbilityManagerServiceUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;
    sptr<AccessibleAbilityClientStubImpl> stub_;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

// static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;
// static sptr<AccessibleAbilityClientStubImpl> stub_;

void AccessibleAbilityManagerServiceUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest SetUpTestCase";
}

void AccessibleAbilityManagerServiceUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDownTestCase";
}

void AccessibleAbilityManagerServiceUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest SetUp";
    //注册bundleservice
    auto bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    stub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));

    /* touchGuider->StartUp */
    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    displayMgr.SetDisplay(wmdInfo);
}

void AccessibleAbilityManagerServiceUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDown";

    ins_ = nullptr;
    stub_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetInstance_001 start";

    auto ins = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    EXPECT_TRUE(ins);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetInstance_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnStart_001
 * @tc.name: OnStart
 * @tc.desc: Test function OnStart
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnStart_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_OnStart_001 start";

    auto ins = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins->OnStart();
    EXPECT_TRUE(ins->GetMainHandler());
    EXPECT_TRUE(ins->GetMainRunner());
    sleep(1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_OnStart_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RegisterAbilityConnectionClientTmp_001_001
 * @tc.name: RegisterAbilityConnectionClientTmp
 * @tc.desc: Test function RegisterAbilityConnectionClientTmp
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterAbilityConnectionClientTmp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterAbilityConnectionClientTmp_001 start";

    ins_->RegisterAbilityConnectionClientTmp(stub_);
    EXPECT_EQ(testChannalId, 1);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_RegisterAbilityConnectionClientTmp_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RRegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterAbilityConnectionClientTmp_001 start";
    
    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);

    int ret = ins_->RegisterStateCallback(state,1);
    EXPECT_EQ(ret,0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAbilityList_001 start";
    auto ret = ins_->GetAbilityList(0xFFFFFFFF,3);
    EXPECT_EQ(int(ret.size()), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetAbilityList_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_001 start";
    AccessibilityEventInfo eventInfo;
    eventInfo.SetEventType(EventType::TYPE_PAGE_STATE_UPDATE);
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent";

    ins_->SendEvent(eventInfo,0);
    sleep(1);
    EXPECT_EQ(testEventType, int(EventType::TYPE_PAGE_STATE_UPDATE));

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_SendEvent_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterInteractionOperation_001
 * @tc.name: RegisterInteractionOperation
 * @tc.desc: Test function RegisterInteractionOperation
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterInteractionOperation_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    if (accountData == nullptr) {
         GTEST_LOG_(INFO) << "accountData is null";
    }else{
         GTEST_LOG_(INFO) << "accountData is not null";
    }
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    ins_->RegisterInteractionOperation(0,nullptr,0);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterInteractionOperation_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DeregisterInteractionOperation_001
 * @tc.name: DeregisterInteractionOperation
 * @tc.desc: Test function DeregisterInteractionOperation
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DeregisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterInteractionOperation_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    ins_->DeregisterInteractionOperation(0);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterInteractionOperation_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_Interrupt_001
 * @tc.name: Interrupt
 * @tc.desc: Test function Interrupt
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, Interrupt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceInterrupt_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    ins_->Interrupt(0);
    sleep(1);
    EXPECT_EQ(testInterrupt, 1);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_Interrupt_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_GetSuggestedInterval_001
 * @tc.name: GetSuggestedInterval
 * @tc.desc: Test function GetSuggestedInterval
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetSuggestedInterval_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetSuggestedInterval_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    auto ret = ins_->GetSuggestedInterval();
    uint32_t uiInteractiveTimeout = 10000;
    uint32_t uiNoninteractiveTimeout = 2000;
    uint64_t result = (static_cast<uint64_t>(uiNoninteractiveTimeout) << INT32_BIT_NUM) | uiInteractiveTimeout;
    EXPECT_EQ(ret ,result);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceManager_ServiceUnittest_GetSuggestedInterval_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PersistElementNamesToSetting_001
 * @tc.name: PersistElementNamesToSetting
 * @tc.desc: Test function PersistElementNamesToSetting
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PersistElementNamesToSetting_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PersistElementNamesToSetting_001 start";
    //TODO

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PersistElementNamesToSetting_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetTouchEventInjector_001
 * @tc.name: GetTouchEventInjector
 * @tc.desc: Test function GetTouchEventInjector
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetTouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetTouchEventInjector_001 start";
    auto ret = ins_->GetTouchEventInjector();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetTouchEventInjector_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTouchEventInjector_001
 * @tc.name: SetTouchEventInjector
 * @tc.desc: Test function SetTouchEventInjector
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTouchEventInjector_001 start";
    sptr<TouchEventInjector> touchEventInjector = new TouchEventInjector();
    ins_->SetTouchEventInjector(touchEventInjector);
    auto ret = ins_->GetTouchEventInjector();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTouchEventInjector_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_IsWantedKeyEvent_001
 * @tc.name: IsWantedKeyEvent
 * @tc.desc: Test function IsWantedKeyEvent
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, IsWantedKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_IsWantedKeyEvent_001 start";
    MMI::KeyEvent event;
    auto ret = ins_->IsWantedKeyEvent(event);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_IsWantedKeyEvent_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetKeyEventFilter_001
 * @tc.name: GetKeyEventFilter
 * @tc.desc: Test function GetKeyEventFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetKeyEventFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetKeyEventFilter_001 start";
    auto ret = ins_->GetKeyEventFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetKeyEventFilter_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetKeyEventFilter_001
 * @tc.name: SetKeyEventFilter
 * @tc.desc: Test function SetKeyEventFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetKeyEventFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetKeyEventFilter_001 start";
    sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
    ins_->SetKeyEventFilter(keyEventFilter);
    auto ret = ins_->GetKeyEventFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetKeyEventFilter_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_NotifyDisplayResizeStateChanged_001
 * @tc.name: NotifyDisplayResizeStateChanged
 * @tc.desc: Test function NotifyDisplayResizeStateChanged
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, NotifyDisplayResizeStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_NotifyDisplayResizeStateChanged_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    Rect rect;
    ins_->NotifyDisplayResizeStateChanged(1,rect,0,0,0);
    EXPECT_EQ(testDisplayId,1);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_NotifyDisplayResizeStateChanged_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnTouchInteractionStart_001
 * @tc.name: OnTouchInteractionStart
 * @tc.desc: Test function OnTouchInteractionStart
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnTouchInteractionStart_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnTouchInteractionStart_001 start";
    ins_->OnTouchInteractionStart();
    //todo
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnTouchInteractionStart_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnTouchInteractionEnd_001
 * @tc.name: OnTouchInteractionEnd
 * @tc.desc: Test function OnTouchInteractionEnd
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnTouchInteractionEnd_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnTouchInteractionEnd_001 start";
    ins_->OnTouchInteractionEnd();
    //todo
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnTouchInteractionEnd_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnGesture_001
 * @tc.name: OnGesture
 * @tc.desc: Test function OnGesture
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnGesture_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    ins_->OnGesture(0);
    EXPECT_EQ(testGesture,0);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_OnGesture_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetInputFilter_001
 * @tc.name: GetInputFilter
 * @tc.desc: Test function GetInputFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetInputFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetInputFilter_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    auto ret = ins_->GetInputFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetInputFilter_001 end";
}

// /**
//  * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetZoomProxy_001
//  * @tc.name: GetZoomProxy
//  * @tc.desc: Test function GetZoomProxy
//  */

// HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetZoomProxy_001, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetZoomProxy_001 start";
//     auto ret = ins_->GetZoomProxy();
//     EXPECT_FALSE(ret);
//     GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetZoomProxy_001 end";
// }

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_HasInputFilter_001
 * @tc.name: HasInputFilter
 * @tc.desc: Test function HasInputFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, HasInputFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_HasInputFilter_001 start";
    auto ret = ins_->HasInputFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_HasInputFilter_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetInputManager_001
 * @tc.name: GetInputManager
 * @tc.desc: Test function GetInputManager
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetInputManager_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetInputManager_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    auto ret = ins_->GetInputManager();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetInputManager_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_FindFocusedElementInfo_001 start";
    auto ret = ins_->FindFocusedElementInfo();
    EXPECT_EQ(ret.GetWindowId(), -1);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PerformActionOnAccessibilityFocusedItem_002
 * @tc.name: PerformActionOnAccessibilityFocusedItem
 * @tc.desc: Test function PerformActionOnAccessibilityFocusedItem
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PerformActionOnAccessibilityFocusedItem_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PerformActionOnAccessibilityFocusedItem_002 start";
    auto ret = ins_->PerformActionOnAccessibilityFocusedItem(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PerformActionOnAccessibilityFocusedItem_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetAccessibilityFocusClickPointInScreen_001
 * @tc.name: GetAccessibilityFocusClickPointInScreen
 * @tc.desc: Test function GetAccessibilityFocusClickPointInScreen
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAccessibilityFocusClickPointInScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetAccessibilityFocusClickPointInScreen_001 start";
    MmiPoint point;
    auto ret = ins_->GetAccessibilityFocusClickPointInScreen(point);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetAccessibilityFocusClickPointInScreen_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SwitchedUser_001
 * @tc.name: SwitchedUser
 * @tc.desc: Test function SwitchedUser Wrong branch
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SwitchedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SwitchedUser_001 start";
    ins_->SwitchedUser(0);
    auto ret = ins_->GetCurrentAccountId();
    EXPECT_EQ(ret,0);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SwitchedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UnlockedUser_001
 * @tc.name: UnlockedUser
 * @tc.desc: Test function UnlockedUser
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UnlockedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_UnlockedUser_001 start";
    ins_->GetCurrentAccountData()->ClearInstalledAbility();
    AppExecFwk::AbilityInfo info;
    AAFwk::Want want;
    AppExecFwk::ElementName elementname;
    elementname.SetAbilityName("com.example.aamsModuleTest.MainAbility");
    elementname.SetBundleName("com.example.aamsModuleTest");
    want.SetElement(elementname);
    ins_->GetBundleMgrProxy()->QueryAbilityInfo(want, info);
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    ins_->GetCurrentAccountData()->AddInstalledAbility(*abilityInfo);
    ins_->GetCurrentAccountData()->AddEnabledAbility(elementname);
    EXPECT_EQ((int)ins_->GetCurrentAccountData()->GetInstalledAbilities().size(), 1);
    EXPECT_EQ((int)ins_->GetCurrentAccountData()->GetEnabledAbilities().size(), 1);
    EXPECT_EQ((int)ins_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 1);

    GTEST_LOG_(INFO) << "Unlock account.";
    int accountId = ins_->GetCurrentAccountId();
    ins_->UnlockedUser((int32_t)accountId);
    AAConnection_ = new AccessibleAbilityConnection(ins_->GetCurrentAccountData(), 0, *abilityInfo);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    stub_ = new Accessibility::AccessibleAbilityClientStubImpl(
        std::make_shared<Accessibility::AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(elementname, stub_, 0);
    EXPECT_EQ((int)ins_->GetCurrentAccountData()->GetConnectedA11yAbilities().size(), 2);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_UnlockedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SwitchedUser_003
 * @tc.name: SwitchedUser
 * @tc.desc: Test function SwitchedUser Wrong branch
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SwitchedUser_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SwitchedUser_003 start";
    // register statecallback
    sptr<AccessibleAbilityManagerServiceStateStub> state = new AccessibleAbilityManagerServiceStateStub();
    sptr<IAccessibleAbilityManagerServiceState> proxy = new AccessibleAbilityManagerServiceStateProxy(state);
    ins_->RegisterStateCallback(proxy, 0);
    int testAccountId = 2;
    ins_->SwitchedUser(testAccountId);
    sleep(1);
    EXPECT_EQ(testStateType,0);
    EXPECT_EQ(ins_->GetCurrentAccountId(), testAccountId);
    
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SwitchedUser_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemovedUser_001
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    EXPECT_EQ(int(ins_->GetCurrentAccountData()->GetInstalledAbilities().size()), 1);
    ins_->RemovedUser(1);
    //can't to check a11yAccountsData_ because it is private,and don't provite api.
    //EXPECT_EQ(int(ins_->GetCurrentAccountData()->GetInstalledAbilities().size()), 0);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetBundleMgrProxy_001
 * @tc.name: GetBundleMgrProxy
 * @tc.desc: Test function GetBundleMgrProxy
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetBundleMgrProxy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetBundleMgrProxy_001 start";
    auto ret = ins_->GetBundleMgrProxy();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetBundleMgrProxyr_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetWindowMgrProxy_001
 * @tc.name: GetWindowMgrProxy
 * @tc.desc: Test function GetBundGetWindowMgrProxyleMgrProxy
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetWindowMgrProxy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetWindowMgrProxy_001 start";
    auto ret = ins_->GetWindowMgrProxy();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetWindowMgrProxy_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PresentUser_001
 * @tc.name: PresentUser
 * @tc.desc: Test function PresentUser
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PresentUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PresentUser_001 start";
    ins_->PresentUser();
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PresentUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageChanged_001
 * @tc.name: PackageChanged
 * @tc.desc: Test function PackageChanged
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageChanged_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    accountData->ClearInstalledAbility();
    /* install ability is null */
    GTEST_LOG_(INFO) << "GetInstalledAbilities start";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "PackageChanged start";
    ins_->PackageChanged();
    GTEST_LOG_(INFO) << "PackageChanged end";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "GetInstalledAbilities end";
    /* add install ability */
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    GTEST_LOG_(INFO) << "RegisterAbilityConnectionClientTmp end";
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    ins_->PackageChanged();
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageChanged_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageRemoved_001
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageRemoved_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    AppExecFwk::ElementName elementName("1", "test", "test01");
    accountData->AddEnabledAbility(elementName);
    accountData->AddConnectingA11yAbility(elementName);
    EXPECT_EQ(1, int(accountData->GetEnabledAbilities().size()));
    EXPECT_EQ(1, int(accountData->GetConnectingA11yAbilities().size()));
    std::string str = "test";
    ins_->PackageRemoved(str);
    EXPECT_EQ(0, int(accountData->GetEnabledAbilities().size()));
    EXPECT_EQ(0, int(accountData->GetConnectingA11yAbilities().size()));

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageUpdateFinished_001
 * @tc.name: PackageUpdateFinished
 * @tc.desc: Test function PackageUpdateFinished
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageUpdateFinished_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageUpdateFinished_001 start";
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    AppExecFwk::ElementName elementName("1", "test", "test01");
    accountData->AddConnectingA11yAbility(elementName);
    EXPECT_EQ(1, int(accountData->GetConnectingA11yAbilities().size()));
    std::string str = "test";
    ins_->PackageUpdateFinished(str);
    EXPECT_EQ(0, int(accountData->GetConnectingA11yAbilities().size()));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageUpdateFinished_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001
 * @tc.name: UpdateAccessibilityManagerService
 * @tc.desc: Test function UpdateAccessibilityManagerService
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateAccessibilityManagerService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_UpdateAccessibilityManagerService_001 start";
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);
    ins_->RegisterStateCallback(state,0);
    ins_->UpdateAccessibilityManagerService();
    EXPECT_EQ(testStateType, int(AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED|
    AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED));
    auto accountData = ins_->GetCurrentAccountData();
    accountData->RemoveStateCallback(state);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_UpdateAccessibilityManagerService_001 end";
}