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
#include <optional>
#include "accessibility_ability_helper.h"
#include "accessibility_ability_info.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "mock_bundle_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
} // namespace

class AccessibleAbilityManagerServiceUnitTest : public ::testing::Test {
public:
    AccessibleAbilityManagerServiceUnitTest()
    {}
    ~AccessibleAbilityManagerServiceUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityClientStub> stub_ = nullptr;
    void RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj);
};

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
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityAbilityHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
    stub_ = new MockAccessibleAbilityClientStubImpl();
}

void AccessibleAbilityManagerServiceUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDown";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    stub_ = nullptr;
}

void AccessibleAbilityManagerServiceUnitTest::RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    // add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    connection->OnAbilityConnectDoneSync(elementName, obj);
    sleep(SLEEP_TIME_1);
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetInstance_001 start";

    auto ins = &Singleton<AccessibleAbilityManagerService>::GetInstance();
    EXPECT_TRUE(ins);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetInstance_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_001 start";

    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

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
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(0xFFFFFFFF, 3, infos);
    EXPECT_EQ(infos.size(), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetAbilityList_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, nullptr);
    sleep(SLEEP_TIME_1);
    GTEST_LOG_(INFO) << "RegisterElementOperator OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterElementOperator_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    sleep(SLEEP_TIME_1);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterElementOperator_001 end";
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
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(touchEventInjector);
    auto ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetTouchEventInjector_001 end";
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
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(keyEventFilter);
    auto ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_SetKeyEventFilter_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemovedUser_001
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 start";
    RegisterAbilityConnectionClient(stub_);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_EQ(int(accountData->GetConnectedA11yAbilities().size()), 1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    // can't to check a11yAccountsData_ because it is private,and don't provite api.
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
    auto ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetBundleMgrProxyr_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageChanged_001
 * @tc.name: PackageChanged
 * @tc.desc: Test function PackageChanged
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageChanged_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    accountData->ClearInstalledAbility();
    /* install ability is null */
    GTEST_LOG_(INFO) << "GetInstalledAbilities start";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "PackageChanged start";
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
    GTEST_LOG_(INFO) << "PackageChanged end";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "GetInstalledAbilities end";
    /* add install ability */
    sleep(SLEEP_TIME_1);
    RegisterAbilityConnectionClient(stub_);
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    bundleName = "bundleName2";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageChanged_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetScreenMagnificationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortKeyState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortKeyState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    int32_t time = 3;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(time);
    EXPECT_EQ(time, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002 start";
    int32_t time = 3;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(time);
    EXPECT_EQ(time, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string name = "test";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name);
    auto str = Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget();
    EXPECT_STREQ("test", str.c_str());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002 start";
    std::string name = "test";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name);
    auto str = Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget();
    EXPECT_STREQ("test", str.c_str());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetHighContrastTextState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAnimationOffState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    uint32_t filter = 2;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter);
    EXPECT_EQ(filter, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationColorFilter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002 start";
    uint32_t filter = 2;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter);
    EXPECT_EQ(filter, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    uint32_t time = 10;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(time);
    EXPECT_EQ(time, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002 start";
    uint32_t time = 10;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(time);
    EXPECT_EQ(time, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    float discount = 0.1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(discount);
    EXPECT_EQ(discount, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002 start";
    float discount = 0.1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(discount);
    EXPECT_EQ(discount, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002 start";
    bool state = true;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(state);
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002 end";
}
} // namespace Accessibility
} // namespace OHOS