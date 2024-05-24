/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include "accessibility_ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_common_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_ut_helper.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_manager_service_config_observer_proxy.h"
#include "mock_accessible_ability_manager_service_config_observer_stub.h"
#include "mock_accessible_ability_manager_service_state_observer_proxy.h"
#include "mock_accessible_ability_manager_service_state_observer_stub.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_1 = 1;
    constexpr uint32_t CONTENT_TIMEOUT_VALUE = 10;
    constexpr int32_t MOUSE_AUTO_CLICK_VALUE = 3;
    constexpr float BRIGHTNESS_DISCOUNT_VALUE = 0.1f;
    constexpr float AUDIO_BALANCE_VALUE = 0.1f;
    constexpr int32_t ACTIVE_WINDOW_VALUE = 2;
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
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
}

void AccessibleAbilityManagerServiceUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
}

void AccessibleAbilityManagerServiceUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest SetUp";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
    stub_ = new MockAccessibleAbilityClientStubImpl();
}

void AccessibleAbilityManagerServiceUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDown";
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
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    connection->OnAbilityConnectDoneSync(elementName, obj);
    sleep(SLEEP_TIME_1);
    EXPECT_NE(abilityInfo.get(), nullptr);
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
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        ACCESSIBILITY_ABILITY_TYPE_ALL, ABILITY_STATE_ENABLE, infos);
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
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    EXPECT_EQ(RET_OK, aams.RegisterElementOperator(0, nullptr, true));
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
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_EQ(RET_OK, aams.DeregisterElementOperator(0));
    sleep(SLEEP_TIME_1);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterElementOperator_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_002
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DeregisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterElementOperator_002 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_EQ(RET_OK, aams.RegisterElementOperator(0, nullptr, true));
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(RET_OK, aams.DeregisterElementOperator(0));
    sleep(SLEEP_TIME_1);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DeregisterElementOperator_002 end";
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
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemovedUser_002
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RemovedUser_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(1);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(1);
    sleep(SLEEP_TIME_1);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_EQ(accountData->GetAccountId(), 1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    // can't to check a11yAccountsData_ because it is private,and don't provite api.
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_002 end";
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
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001 start";
    int32_t ret = 0;
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001 start";
    std::string name = "test";
    std::string ret = "";
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_001
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK,
     Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001 start";
    uint32_t filter = 1;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001 start";
    uint32_t value = CONTENT_TIMEOUT_VALUE;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(value));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001 start";
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    RetError result = Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value);
    EXPECT_EQ(RET_OK, result);
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001 start";
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(AUDIO_BALANCE_VALUE));
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetClickResponseTime_001
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetClickResponseTime_001 start";
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetClickResponseTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetClickResponseTime(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickState_001
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetIgnoreRepeatClickState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickState_001 start";
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickState(true));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickTime_001
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickTime_001 start";
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickTime(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_001
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAllConfigs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_001 start";
    AccessibilityConfigData data;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(data);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetCaptionState_001
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetCaptionState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionState_001 start";
    bool state = true;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(ret));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetEnabledState_001
 * @tc.name: GetEnabledState
 * @tc.desc: Test function GetEnabledState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetEnabledState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetEnabledState_001 start";
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetEnabledState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_001
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetTouchGuideState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetGestureState_001
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetGestureState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_001
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetKeyEventObserverState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetActiveWindow_001
 * @tc.name: GetActiveWindow
 * @tc.desc: Test function GetActiveWindow
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetActiveWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetActiveWindow_001 start";
    Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(ACTIVE_WINDOW_VALUE);
    EXPECT_EQ(ACTIVE_WINDOW_VALUE, Singleton<AccessibleAbilityManagerService>::GetInstance().GetActiveWindow());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetActiveWindow_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_001
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_001 start";
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));

    AccessibilityConfig::CaptionProperty res;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(res));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageAdd_001
 * @tc.name: PackageAdd
 * @tc.desc: Test function PackageAdd
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageAdd_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_001 start";

    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageRemoved_001
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageRemoved_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_001 start";
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_AddedUser_001
 * @tc.name: AddedUser
 * @tc.desc: Test function AddedUser
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, AddedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_AddedUser_001 start";
    int32_t accountId = 1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(accountId);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_AddedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableShortKeyTargetAbility_001
 * @tc.name: EnableShortKeyTargetAbility
 * @tc.desc: Test function EnableShortKeyTargetAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableShortKeyTargetAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableShortKeyTargetAbility_001
 * @tc.name: DisableShortKeyTargetAbility
 * @tc.desc: Test function DisableShortKeyTargetAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableShortKeyTargetAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableAbility_001
 * @tc.name: EnableAbility
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableAbility_001 start";
    std::string name = "test";
    uint32_t capabilities = 1;
    EXPECT_EQ(RET_ERR_NOT_INSTALLED,
        Singleton<AccessibleAbilityManagerService>::GetInstance().EnableAbility(name, capabilities));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableAbility_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableAbility_001
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableAbility_001 start";
    std::string name = "test";
    EXPECT_EQ(RET_ERR_NOT_ENABLED, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableAbility(name));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableAbility_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetEnabledAbilities_001
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetEnabledAbilities_001 start";
    std::vector<std::string> enabledAbilities;
    EXPECT_EQ(RET_OK,
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledAbilities(enabledAbilities));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetEnabledAbilities_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_001
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableUITestAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableUITestAbility_001 start";
    sptr<IRemoteObject> obj;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj));
    GTEST_LOG_(INFO) << "Accessib le_Ability_Manager_ServiceUnittest_EnableUITestAbility_001 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableUITestAbility_001
 * @tc.name: DisableUITestAbility
 * @tc.desc: Test function DisableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableUITestAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_001 start";
    EXPECT_EQ(RET_ERR_NO_CONNECTION, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterCaptionObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001 start";

    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001 end";
}

/*
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_001
 * @tc.name: RegisterConfigObserver
 * @tc.desc: Test function RegisterConfigObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterConfigObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_001 start";

    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_001 end";
}

/*
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterEnableAbilityListsObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001 start";

    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SwitchedUser_001
 * @tc.name: SwitchedUser
 * @tc.desc: Test function SwitchedUser
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SwitchedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_SwitchedUser_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(100);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(2);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_SwitchedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetAbilityList_002
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAbilityList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAbilityList_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(0xFFFFFFFF, 3, infos);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_GetAbilityList_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_002
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, nullptr, true);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_003
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterElementOperator_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_003 start";
    RegisterAbilityConnectionClient(stub_);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    sptr<IAccessibilityElementOperator> operation;
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterElementOperator_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001
 * @tc.name: UpdateAccessibilityManagerService
 * @tc.desc: Test function UpdateAccessibilityManagerService
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateAccessibilityManagerService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_002
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterStateCallback_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_00 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterCaptionObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterCaptionObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003 end";
}

/*
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterEnableAbilityListsObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002 end";
}

/*
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterEnableAbilityListsObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(nullptr);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_002
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetCaptionProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(caption));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionProperty_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetCaptionState_002
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetCaptionState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetCaptionState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_002
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetTouchGuideState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetTouchGuideState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetGestureState_002
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetGestureState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetGestureState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_002
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetKeyEventObserverState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetKeyEventObserverState_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableAbility_002
 * @tc.name: EnableAbility
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    uint32_t capabilities = 1;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().EnableAbility(name, capabilities));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableAbility_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetEnabledAbilities_002
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetEnabledAbilities_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::vector<std::string> enabledAbilities;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledAbilities(enabledAbilities));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetEnabledAbilities_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableAbility_002
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableAbility(name));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableAbility_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableUITestAbility_002
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableUITestAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<IRemoteObject> obj;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj));
    GTEST_LOG_(INFO) << "Accessib le_Ability_Manager_ServiceUnittest_EnableUITestAbility_002 end";
}

/*
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableUITestAbility_002
 * @tc.name: DisableUITestAbility
 * @tc.desc: Test function DisableUITestAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_EQ(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility(), RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableShortKeyTargetAbility_002
 * @tc.name: EnableShortKeyTargetAbility
 * @tc.desc: Test function EnableShortKeyTargetAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, EnableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableShortKeyTargetAbility_002
 * @tc.name: DisableShortKeyTargetAbility
 * @tc.desc: Test function DisableShortKeyTargetAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, DisableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageRemoved_002
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageRemoved_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageAdd_002
 * @tc.name: PackageAdd
 * @tc.desc: Test function PackageAdd
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, PackageAdd_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_002 end";
}

/*
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_002
 * @tc.name: RegisterConfigObserver
 * @tc.desc: Test function RegisterConfigObserver
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterConfigObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_002 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterConfigObserver_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetScreenMagnificationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_000 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortKeyState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortKeyState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseKeyState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(value));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetMouseAutoClick_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(name));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetShortkeyTarget_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetHighContrastTextState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetHighContrastTextState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_002
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK,
     Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_NE(RET_OK,
     Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetInvertColorState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAnimationOffState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAnimationOffState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(state));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioMonoState_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetDaltonizationColorFilter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t filter = 1;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(filter));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t time = CONTENT_TIMEOUT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(time));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(time));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetContentTimeout_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(value));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = AUDIO_BALANCE_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(value));
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SetAudioBalance_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_002
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, GetAllConfigs_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::shared_ptr<AccessibilityConfigData> data = std::make_shared<AccessibilityConfigData>();
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(*data);
    EXPECT_NE(data.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_GetAllConfigs_002 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_001
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnRemoveSystemAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_001 start";
    int32_t systemAbilityId = 1;
    std::string deviceId = "test";
    auto &ins = Singleton<AccessibleAbilityManagerService>::GetInstance();
    ins.OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_TRUE(ins.IsServiceReady());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_001 end";
}

/**
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_002
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, OnRemoveSystemAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_002 start";
    auto &ins = Singleton<AccessibleAbilityManagerService>::GetInstance();
    EXPECT_TRUE(ins.IsServiceReady());
    int32_t systemAbilityId = SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN;
    std::string deviceId = "test";
    ins.OnRemoveSystemAbility(systemAbilityId, deviceId);
    AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().IsServiceReady() == false) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_1);
    EXPECT_FALSE(ins.IsServiceReady());
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_OnRemoveSystemAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateConfigState_001
 * @tc.name: UpdateConfigState
 * @tc.desc: Test function UpdateConfigState
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateConfigState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateConfigState_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateConfigState();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateConfigState_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateAudioBalance_001
 * @tc.name: UpdateAudioBalance
 * @tc.desc: Test function UpdateAudioBalance
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAudioBalance_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAudioBalance();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAudioBalance_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateBrightnessDiscount_001
 * @tc.name: UpdateBrightnessDiscount
 * @tc.desc: Test function UpdateBrightnessDiscount
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateBrightnessDiscount_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateBrightnessDiscount();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateContentTimeout_001
 * @tc.name: UpdateContentTimeout
 * @tc.desc: Test function UpdateContentTimeout
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateContentTimeout_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateContentTimeout();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateContentTimeout_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateDaltonizationColorFilter_001
 * @tc.name: UpdateDaltonizationColorFilter
 * @tc.desc: Test function UpdateDaltonizationColorFilter
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, UpdateDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateDaltonizationColorFilter_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateDaltonizationColorFilter();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateDaltonizationColorFilter_001 end";
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "current account data is null";
        return;
    }
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
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(event);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_SendEvent_001 end";
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
    RetError ret = Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
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
    int ret = Singleton<AccessibleAbilityManagerService>::GetInstance().Dump(0, args);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    SetUpTestCase();
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_Dump_001 end";
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
} // namespace Accessibility
} // namespace OHOS