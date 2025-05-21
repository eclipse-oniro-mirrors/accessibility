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

#include <cstdio>
#include <gtest/gtest.h>
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessibility_account_data.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service.h"
#include "mock_bundle_manager.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_2 = 2;
    constexpr int32_t CHANNEL_ID = 2;
    constexpr int32_t INVALID_ACCOUNT_ID = -1;
} // namespace

class AccessibilitySettingProviderTest : public ::testing::Test {
public:
    AccessibilitySettingProviderTest()
    {}
    ~AccessibilitySettingProviderTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityConnection> connection_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<IRemoteObject> obj_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
};

void AccessibilitySettingProviderTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilitySettingProviderTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
}

void AccessibilitySettingProviderTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilitySettingProviderTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilitySettingProviderTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // new Interaction proxy
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(0, proxy, 0);

    // new AAconnection
    AccessibilityAbilityInitParams initParams;
    initParams.abilityTypes = ACCESSIBILITY_ABILITY_TYPE_ALL;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    accountData_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData_ != nullptr) {
        accountData_->AddAccessibilityWindowConnection(0, connection);
    }
    connection_ = new AccessibleAbilityConnection(AccessibilityAbilityHelper::accountId_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1", "2", "3");
    obj_ = new IPCObjectProxy(0);
    if (obj_ != nullptr && connection_ != nullptr) {
        connection_->OnAbilityConnectDoneSync(*elementName_, obj_);
    }
}

void AccessibilitySettingProviderTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(INVALID_CHANNEL_ID);
    if (accountData_) {
        accountData_->RemoveAccessibilityWindowConnection(0);
    }

    if (connection_) {
        connection_->OnAbilityDisconnectDoneSync(*elementName_);
    }
    connection_ = nullptr;
    elementName_ = nullptr;
    obj_ = nullptr;
    accountData_ = nullptr;
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateMouseAutoClick_001
 * @tc.name: UpdateMouseAutoClick
 * @tc.desc: Test function UpdateMouseAutoClick
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateMouseAutoClick_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateMouseAutoClick();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateMouseAutoClick_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateShortkeyTarget_001
 * @tc.name: UpdateShortkeyTarget
 * @tc.desc: Test function UpdateShortkeyTarget
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateShortkeyTarget_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateShortkeyTarget();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateShortkeyTarget_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateClickResponseTime_001
 * @tc.name: UpdateClickResponseTime
 * @tc.desc: Test function UpdateClickResponseTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateClickResponseTime_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateClickResponseTime();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateIgnoreRepeatClickTime_001
 * @tc.name: UpdateIgnoreRepeatClickTime
 * @tc.desc: Test function UpdateIgnoreRepeatClickTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateIgnoreRepeatClickTime_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateIgnoreRepeatClickTime();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageRemoved_003
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageRemoved_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_003 start";
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetAbilityList_003
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAbilityList_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAbilityList_003 start";
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    uint32_t type = 0xFFFFFFFF;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(type, -1, infos);
    EXPECT_EQ(infos.size(), 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(type, 5, infos);
    EXPECT_EQ(infos.size(), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetAbilityList_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_001
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function HIGH_CONTRAST_TEXT
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("HIGH_CONTRAST_TEXT");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_002
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function INVERT_COLOR
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_002 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("INVERT_COLOR");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_003
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function ANIMATION_OFF
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_003 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("ANIMATION_OFF");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_004
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function SCREEN_MAGNIFICATION
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_004 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("SCREEN_MAGNIFICATION");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_004 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_005
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function AUDIO_MONO
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_005 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("AUDIO_MONO");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_005 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_006
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function MOUSE_KEY
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_006 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("MOUSE_KEY");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_006 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_007
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function CAPTION_STATE
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_007 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("CAPTION_STATE");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_007 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_008
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function OTHERS
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_008 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("CONTENT_TIMEOUT");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_008 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_009
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function ""
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_009 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    // set
    accountData->GetConfig()->SetShortkeyTarget("");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_009 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SetTargetAbility_010
 * @tc.name: SetTargetAbility
 * @tc.desc: Test function other account
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetTargetAbility_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_010 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->GetConfig()->SetShortkeyTarget("MOUSE_KEY");
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTargetAbility_010 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_AddAccountData_001
 * @tc.name: AddAccountData
 * @tc.desc: Test function add account
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, AddAccountData_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_AddAccountData_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(1);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(1);
    EXPECT_NE(accountData.GetRefPtr(), nullptr);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(1);
    EXPECT_EQ(accountData.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_AddAccountData_001 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_FindInnerWindowId_001
 * @tc.name: FindInnerWindowId
 * @tc.desc: Test function find inner windowId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, FindInnerWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_FindInnerWindowId_001 start";
    AccessibilityEventInfo event;
    event.SetWindowId(1);
    event.SetSource(1);
    int32_t windowId = 1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().FindInnerWindowId(event, windowId);
    EXPECT_EQ(windowId, 1);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_FindInnerWindowId_001 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_GetFocusedWindowId_001
 * @tc.name: GetFocusedWindowId
 * @tc.desc: Test function find focused windowId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetFocusedWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetFocusedWindowId_001 start";
    int32_t windowId = 1;
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetFocusedWindowId(windowId);
    EXPECT_EQ(ret, RET_OK);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetFocusedWindowId_001 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_InsertWindowIdEventPair_001
 * @tc.name: InsertWindowIdEventPair
 * @tc.desc: Test insert foucus window event.
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, InsertWindowIdEventPair_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_InsertWindowIdEventPair_001 start";
    AccessibilityEventInfo event;
    event.SetWindowId(1);
    event.SetSource(1);
    int32_t windowId = 1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().InsertWindowIdEventPair(windowId, event);
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().CheckWindowIdEventExist(windowId);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_InsertWindowIdEventPair_001 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_OnAddSystemAbility_001
 * @tc.name: OnAddSystemAbility
 * @tc.desc: Test insert foucus window event.
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnAddSystemAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAddSystemAbility_001 start";
    TearDownTestCase();
    std::string deviceId = "test";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(0, deviceId);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAddSystemAbility_001 end";
}

/**
 * @tc.number: Accessible_Ability_Manager_ServiceUnittest_OnAddSystemAbility_002
 * @tc.name: OnAddSystemAbility
 * @tc.desc: Test insert foucus window event. (SystemAbilityId not found)
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnAddSystemAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAddSystemAbility_002 start";
    std::string deviceId = "test";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(7, deviceId);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAddSystemAbility_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_003
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnRemoveSystemAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_003 start";
    TearDownTestCase();
    std::string deviceId = "test";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(0, deviceId);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_001 start";
    TearDownTestCase();
    AccessibilityEventInfo event;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(event);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SendEvent_002
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SendEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_002 start";
    AccessibilityEventInfo event;
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(event);
    EXPECT_EQ(ret, RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_002 end";
}


/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_003
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DeregisterElementOperator_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_003 start";
    TearDownTestCase();
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetEnabledState_002
 * @tc.name: GetEnabledState
 * @tc.desc: Test function GetEnabledState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetEnabledState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetEnabledState_002 start";
    TearDownTestCase();
    bool ret = true;
    ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledState();
    EXPECT_EQ(ret, false);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetEnabledState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_003
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetTouchGuideState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_003 start";
    TearDownTestCase();
    bool ret = true;
    ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState();
    EXPECT_EQ(ret, false);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetGestureState_003
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetGestureState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_003 start";
    TearDownTestCase();
    bool ret = true;
    ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState();
    EXPECT_EQ(ret, false);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_003
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetKeyEventObserverState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_003 start";
    TearDownTestCase();
    bool ret = true;
    ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState();
    EXPECT_EQ(ret, false);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_003 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetEnabledAbilities_003
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetEnabledAbilities_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetEnabledAbilities_003 start";
    TearDownTestCase();
    std::vector<std::string> enabledAbilities;
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledAbilities(enabledAbilities);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetEnabledAbilities_003 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_Dump_001
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, Dump_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_Dump_001 start";
    TearDownTestCase();
    std::vector<std::u16string> args;
    Singleton<AccessibleAbilityManagerService>::GetInstance().Dump(0, args);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_Dump_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_Dump_002
 * @tc.name: Dump
 * @tc.desc: Test function Dump
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, Dump_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_Dump_002 start";
    std::vector<std::u16string> args;
    Singleton<AccessibleAbilityManagerService>::GetInstance().Dump(0, args);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_Dump_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_003
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableUITestAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_003 start";
    TearDownTestCase();
    sptr<IRemoteObject> obj = nullptr;
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_003 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_004
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableUITestAbility_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_004 start";
    sptr<IRemoteObject> obj = nullptr;
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_004 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableAbility_003
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DisableAbility_003 start";
    TearDownTestCase();
    std::string name = "test";
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().DisableAbility(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DisableAbility_003 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableUITestAbility_003
 * @tc.name: DisableUITestAbility
 * @tc.desc: Test function DisableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableUITestAbility_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DisableUITestAbility_003 start";
    TearDownTestCase();
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility();
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_DisableUITestAbility_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateObserver_001
 * @tc.name: RegisterStateObserver
 * @tc.desc: Test function RegisterStateObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterStateObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateObserver_001 start";
    TearDownTestCase();
    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateObserver_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_001
 * @tc.name: GetRealWindowAndElementId
 * @tc.desc: Test function GetRealWindowAndElementId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetRealWindowAndElementId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_001 start";
    TearDownTestCase();
    int32_t windowId = 0;
    int64_t elementId = 0;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_002
 * @tc.name: GetRealWindowAndElementId
 * @tc.desc: Test function GetRealWindowAndElementId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetRealWindowAndElementId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_002 start";
    int32_t windowId = 0;
    int64_t elementId = 0;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetRealWindowAndElementId_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_001
 * @tc.name: GetSceneBoardInnerWinId
 * @tc.desc: Test function GetSceneBoardInnerWinId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetSceneBoardInnerWinId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_001 start";
    TearDownTestCase();
    int32_t windowId = 0;
    int64_t elementId = 0;
    int32_t innerWid = 0;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_002
 * @tc.name: GetSceneBoardInnerWinId
 * @tc.desc: Test function GetSceneBoardInnerWinId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetSceneBoardInnerWinId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_002 start";
    TearDownTestCase();
    int32_t windowId = 0;
    int64_t elementId = 0;
    int32_t innerWid = 0;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_GetSceneBoardInnerWinId_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_UpdateShortkeyMultiTarget_001
 * @tc.name: UpdateShortkeyMultiTarget
 * @tc.desc: Test function UpdateShortkeyMultiTarget
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_UpdateShortkeyMultiTarget_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateShortkeyMultiTarget();
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_UpdateShortkeyMultiTarget_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_001
 * @tc.name: OnDeviceProvisioned
 * @tc.desc: Test function OnDeviceProvisioned
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnDeviceProvisioned_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_001 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string deviceId = "false";
    aams.OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, deviceId);
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_002
 * @tc.name: OnDeviceProvisioned
 * @tc.desc: Test function OnDeviceProvisioned
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnDeviceProvisioned_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_002 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.OnStop();
    auto accountData = aams.GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
    }
    std::string deviceId = "false";
    aams.OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, deviceId);
    aams.OnStart();
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_OnDeviceProvisioned_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_001
 * @tc.name: RegisterShortKeyEvent
 * @tc.desc: Test function RegisterShortKeyEvent
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterShortKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_001 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.OnStop();
    auto accountData = aams.GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
    }
    std::string deviceId = "false";
    aams.OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, deviceId);
    aams.OnStart();
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_002
 * @tc.name: RegisterShortKeyEvent
 * @tc.desc: Test function RegisterShortKeyEvent
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterShortKeyEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_002 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string deviceId = "false";
    aams.OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, deviceId);
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_RegisterShortKeyEvent_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemoveRequestId_001
 * @tc.name: RemoveRequestId
 * @tc.desc: Test function AddRequestId RemoveRequestId
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RemoveRequestId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_RemoveRequestId_001 start";
    int32_t windowId = 1;
    int32_t treeId = 2;
    int32_t requestId = 3;
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId, requestId, nullptr);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_RemoveRequestId_001 end";
}

/**
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected single tap event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_001 start";
    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100);

    touchEventInjector_->InjectEvents(gesturePath);

    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction, expectValue);

    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check Some functional interface.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    touchEventInjector_->SetNext(inputInterceptor_);
    touchEventInjector_->OnPointerEvent(*event);
    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);

    MMI::PointerEvent touchevent = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchEventInjector_->OnPointerEvent(touchevent);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    EXPECT_EQ(touchAction, expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check destroy event before sendtouchevent.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point {10.0f, 10.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(100000);

    touchEventInjector_->InjectEvents(gesturePath);
    sleep(SLEEP_TIME_1);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    touchEventInjector_->DestroyEvents();
    isDestroyEvents_ = AccessibilityAbilityHelper::GetInstance().GetDestroyState();
    EXPECT_EQ(isDestroyEvents_, true);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_EQ(touchAction, expectValue);

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check injected UP_THEN_RIGHT move event.
 */
HWTEST_F(TouchEventInjectorTest, TouchEventInjector_Unittest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 start";

    touchEventInjector_->SetNext(inputInterceptor_);

    AccessibilityGesturePosition point1 {10.0f, 10.0f};
    AccessibilityGesturePosition point2 {10.0f, 20.0f};
    AccessibilityGesturePosition point3 {20.0f, 20.0f};
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    gesturePath->AddPosition(point1);
    gesturePath->AddPosition(point2);
    gesturePath->AddPosition(point3);
    gesturePath->SetDurationTime(200);

    touchEventInjector_->InjectEvents(gesturePath);
    sleep(SLEEP_TIME_2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_DOWN;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, expectValue);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, expectValue);
    expectValue = MMI::PointerEvent::POINTER_ACTION_UP;
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(4);
    EXPECT_EQ(touchAction, expectValue);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();

    GTEST_LOG_(INFO) << "TouchEventInjector_Unittest_TouchEventInjector_006 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_DOWN timeout)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_001 start";
    if (!zoomGesture_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    sleep(2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(Tap once on the screen)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_002 start";
    if (!zoomGesture_) {
        return;
    }
    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item;
    eventDown->AddPointerItem(item);
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*eventDown);
    // Pointer up
    std::shared_ptr<MMI::PointerEvent> eventUp = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_UP);
    if (!eventUp) {
        return;
    }
    zoomGesture_->OnPointerEvent(*eventUp);
    sleep(2);

    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(3 Taps on the screen)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_003 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // stop zoom
    TripleTaps();
    zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_FALSE(zoomState);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(transfer to sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_004 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_005
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_MOVE in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_005 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    // Pointer move
    std::shared_ptr<MMI::PointerEvent> eventMove = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    if (!eventMove) {
        return;
    }
    MMI::PointerEvent::PointerItem item3;
    item3.SetDisplayX(0);
    item3.SetDisplayY(50);
    eventMove->AddPointerItem(item3);

    MMI::PointerEvent::PointerItem item4;
    item4.SetDisplayX(70);
    item4.SetDisplayY(50);
    eventMove->AddPointerItem(item4);
    zoomGesture_->OnPointerEvent(*eventMove);
    zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_006
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_DOWN in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_006 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> eventDown = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!eventDown) {
        return;
    }
    MMI::PointerEvent::PointerItem item1;
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    eventDown->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    eventDown->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*eventDown);

    MMI::PointerEvent::PointerItem item3;
    item3.SetDisplayX(50);
    item3.SetDisplayY(25);
    eventDown->AddPointerItem(item3);
    zoomGesture_->OnPointerEvent(*eventDown);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_007
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_UP in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_007 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);

    MMI::PointerEvent::PointerItem item3;
    item3.SetPointerId(3);
    item3.SetDisplayX(50);
    item3.SetDisplayY(25);
    event->AddPointerItem(item3);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_007 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_008
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(sliding state->zoom state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_008 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    zoomGesture_->OnPointerEvent(*event);
    // Pointer up
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    event->SetPointerId(2);
    event->RemovePointerItem(1);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_008 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_009
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in ready state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_009 start";
    if (!zoomGesture_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_009 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_010
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in zoom state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_010 start";
    if (!zoomGesture_) {
        return;
    }
    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);
    // send cancel event
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_CANCEL);
    if (!event) {
        return;
    }
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();
    zoomGesture_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_CANCEL);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_010 end";
}

/**
 * @tc.number: AccessibilityZoomGesture_Unittest_OnPointerEvent_011
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent(POINTER_ACTION_CANCEL in sliding state)
 */
HWTEST_F(AccessibilityZoomGestureUnitTest, AccessibilityZoomGesture_Unittest_OnPointerEvent_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_011 start";

    // start zoom
    TripleTaps();
    bool zoomState = AccessibilityAbilityHelper::GetInstance().GetZoomState();
    EXPECT_TRUE(zoomState);

    // Pointer down
    std::shared_ptr<MMI::PointerEvent> event = CreatePointerEvent(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (!event) {
        return;
    }
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(10);
    item1.SetDisplayY(20);
    event->AddPointerItem(item1);

    MMI::PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDisplayX(40);
    item2.SetDisplayY(20);
    event->AddPointerItem(item2);
    zoomGesture_->OnPointerEvent(*event);

    // Pointer cancel
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    zoomGesture_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityZoomGesture_Unittest_OnPointerEvent_011 end";
}
/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(1);
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(1);
    item1.SetDisplayY(1);
    event->UpdatePointerItem(1, item1);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(1);
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_005
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_005 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_006
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_006 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: AccessibilityShortKey_Unittest_Register_001
 * @tc.name: Register
 * @tc.desc: Test function Register
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_Unittest_Register_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_Register_001 start";
    shortKey_->Unregister();
    shortKey_->Register();
    GTEST_LOG_(INFO) << "AccessibilityShortKey_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: Accessibility_Short_Key_UnitTest_AccessibilityShortKey001
 * @tc.name: AccessibilityShortKey
 * @tc.desc: Test function AccessibilityShortKey(power key cancel)
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityShortKeyUnitTest, AccessibilityShortKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessibility_Short_Key_UnitTest_AccessibilityShortKey001 start";
    TearDownTestCase();
    sptr<AccessibilityShortKey> test = new (std::nothrow) AccessibilityShortKey();
    test = nullptr;
    SetUpTestCase();
    GTEST_LOG_(INFO) << "Accessibility_Short_Key_UnitTest_AccessibilityShortKey001 end";
}

/**
 * @tc.number: OnPointerEvent001
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first down event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_001 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) == EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: OnPointerEvent002
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the first up event.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_002 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) == EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);

    // create
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent2 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent2);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER;
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, expectValue);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_GUIDE_BEGIN);
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: OnPointerEvent003
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that down-up slowly.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_003 start";

    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent3 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) ==
            EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent3);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: OnPointerEvent004
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_004 start";
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(1) ==
            EventType::TYPE_TOUCH_GUIDE_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    EventType eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_BEGIN);
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) ==
            MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);

    event = CreateMoveEvent(1);
    touchGuider_->OnPointerEvent(*event);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent4 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(3) == EventType::TYPE_TOUCH_GUIDE_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent4);
    eventType = AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(2);
    EXPECT_EQ(eventType, EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: OnPointerEvent005
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in touchGuiding state.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_005 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent5 = TestEventType();
    EXPECT_TRUE(retOnPointerEvent5);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
    if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
        MMI::PointerEvent::POINTER_ACTION_DOWN) {
        return true;
    } else {
        return false;
    }
    }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);

    points.clear();
    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_2, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    points.clear();
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, POINT_ID_1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent5 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        int32_t eventTypeSize =
            static_cast<int32_t>(AccessibilityAbilityHelper::GetInstance().GetEventTypeVector().size());
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(eventTypeSize - 1) ==
            EventType::TYPE_TOUCH_END) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent5);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: OnPointerEvent006
 * @tc.name: OnPointerEvent
 * @tc.desc: Check the event that multi-finger gesture.
 */
HWTEST_F(TouchGuiderTest, TouchGuider_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_006 start";

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(1);
    MMI::PointerEvent::PointerItem otherPoint = {};
    SetTouchGuiderPoint(otherPoint, 2, 10, 10);
    MMI::PointerEvent::PointerItem otherPoint1 = {};
    SetTouchGuiderPoint(otherPoint1, 3, 10, 10);

    points.emplace_back(point);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    bool retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventTypeOfTargetIndex(0) ==
            EventType::TYPE_TOUCH_BEGIN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    points.emplace_back(otherPoint);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 2, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    points.emplace_back(otherPoint1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 3, 0, 0);
    touchGuider_->OnPointerEvent(*event);
    retOnPointerEvent6 = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0) !=
            MMI::PointerEvent::POINTER_ACTION_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(retOnPointerEvent6);

    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnPointerEvent_006 end";
}
}
}