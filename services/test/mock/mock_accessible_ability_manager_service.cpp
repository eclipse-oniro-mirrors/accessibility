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
    constexpr int32_t SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    const std::string DEVICE_PROVISIONED = "device_provisioned";
} // namespace

class MockA11yManagerServiceUnitTest : public ::testing::Test {
public:
    MockA11yManagerServiceUnitTest()
    {}
    ~MockA11yManagerServiceUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityClientStub> stub_ = nullptr;
    void RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj);
};

void MockA11yManagerServiceUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
}

void MockA11yManagerServiceUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
}

void MockA11yManagerServiceUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest SetUp";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
    stub_ = new MockAccessibleAbilityClientStubImpl();
}

void MockA11yManagerServiceUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest TearDown";
    stub_ = nullptr;
}

void MockA11yManagerServiceUnitTest::RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj)
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
 * @tc.number: MockA11yManagerServiceUnitTest_GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_GetInstance_001 start";

    auto ins = &Singleton<AccessibleAbilityManagerService>::GetInstance();
    EXPECT_TRUE(ins);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_GetInstance_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_001 start";

    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAbilityList_001 start";
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        ACCESSIBILITY_ABILITY_TYPE_ALL, ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(infos.size(), 0);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_GetAbilityList_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_001 start";
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

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(MockA11yManagerServiceUnitTest, DeregisterElementOperator_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, DeregisterElementOperator_002, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, SetTouchEventInjector_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, SetKeyEventFilter_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 start";
    RegisterAbilityConnectionClient(stub_);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_TRUE(int(accountData->GetConnectedA11yAbilities().size()) >= 1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    // can't to check a11yAccountsData_ because it is private,and don't provite api.
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemovedUser_002
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RemovedUser_002, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, GetBundleMgrProxy_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, PackageChanged_001, TestSize.Level1)
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
 * @tc.number: MockA11yManagerServiceUnitTest_SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetScreenMagnificationState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetShortKeyState_001
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortKeyState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetMouseKeyState_001
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseKeyState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseAutoClick_001 start";
    int32_t ret = 0;
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseAutoClick_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortkeyTarget_001 start";
    std::string name = "test";
    std::string ret = "";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortkeyTarget_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetHighContrastTextState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetHighContrastTextState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetDaltonizationState_001
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetDaltonizationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetInvertColorState_001
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetInvertColorState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetInvertColorState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAnimationOffState_001
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAnimationOffState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAnimationOffState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAudioMonoState_001
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioMonoState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioMonoState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_001 start";
    uint32_t filter = 1;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetContentTimeout_001
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetContentTimeout_001 start";
    uint32_t value = CONTENT_TIMEOUT_VALUE;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(value));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetContentTimeout_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetBrightnessDiscount_001 start";
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    RetError result = Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value);
    EXPECT_EQ(RET_ERR_NULLPTR, result);
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAudioBalance_001
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioBalance_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(AUDIO_BALANCE_VALUE));
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioBalance_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetClickResponseTime_001
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetClickResponseTime_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetClickResponseTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetClickResponseTime(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetClickResponseTime_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickState_001
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetIgnoreRepeatClickState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickState_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickState(true));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickTime_001
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickTime_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickTime(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetAllConfigs_001
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetAllConfigs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAllConfigs_001 start";
    AccessibilityConfigData data;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(data);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAllConfigs_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetCaptionState_001
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetCaptionState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionState_001 start";
    bool state = true;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(ret));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionState_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetActiveWindow_001
 * @tc.name: GetActiveWindow
 * @tc.desc: Test function GetActiveWindow
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetActiveWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetActiveWindow_001 start";
    Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(ACTIVE_WINDOW_VALUE);
    EXPECT_EQ(ACTIVE_WINDOW_VALUE, Singleton<AccessibleAbilityManagerService>::GetInstance().GetActiveWindow());
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetActiveWindow_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetCaptionProperty_001
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionProperty_001 start";
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));

    AccessibilityConfig::CaptionProperty res;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(res));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionProperty_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageAdd_001
 * @tc.name: PackageAdd
 * @tc.desc: Test function PackageAdd
 */
HWTEST_F(MockA11yManagerServiceUnitTest, PackageAdd_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, PackageRemoved_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, AddedUser_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, EnableShortKeyTargetAbility_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, DisableShortKeyTargetAbility_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, EnableAbility_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, DisableAbility_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, GetEnabledAbilities_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, EnableUITestAbility_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, DisableUITestAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_001 start";
    EXPECT_EQ(RET_ERR_NO_CONNECTION, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterCaptionObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001 start";

    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_001 end";
}

/*
* @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_001
* @tc.name: RegisterConfigObserver
* @tc.desc: Test function RegisterConfigObserver
*/
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterConfigObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_001 start";

    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_001 end";
}

/*
* @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001
* @tc.name: RegisterEnableAbilityListsObserver
* @tc.desc: Test function RegisterEnableAbilityListsObserver
*/
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterEnableAbilityListsObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001 start";

    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_001 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_SwitchedUser_001
 * @tc.name: SwitchedUser
 * @tc.desc: Test function SwitchedUser
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SwitchedUser_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, GetAbilityList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAbilityList_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(0xFFFFFFFF, 3, infos);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_GetAbilityList_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_002
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, nullptr, true);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_RegisterElementOperator_003
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterElementOperator_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_003 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    sptr<IAccessibilityElementOperator> operation;
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterElementOperator_003 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001
 * @tc.name: UpdateAccessibilityManagerService
 * @tc.desc: Test function UpdateAccessibilityManagerService
 */
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateAccessibilityManagerService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_UpdateAccessibilityManagerService_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_002
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterStateCallback_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_00 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterStateCallback_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterCaptionObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterCaptionObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterCaptionObserver_003 end";
}

/*
* @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002
* @tc.name: RegisterEnableAbilityListsObserver
* @tc.desc: Test function RegisterEnableAbilityListsObserver
*/
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterEnableAbilityListsObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_002 end";
}

/*
* @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003
* @tc.name: RegisterEnableAbilityListsObserver
* @tc.desc: Test function RegisterEnableAbilityListsObserver
*/
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterEnableAbilityListsObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(nullptr);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterEnableAbilityListsObserver_003 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetCaptionProperty_002
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetCaptionProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionProperty_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(caption));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionProperty_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetCaptionState_002
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetCaptionState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetCaptionState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetTouchGuideState_002
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetTouchGuideState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetTouchGuideState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState());
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetTouchGuideState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetGestureState_002
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetGestureState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetGestureState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState());
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetGestureState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetKeyEventObserverState_002
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetKeyEventObserverState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetKeyEventObserverState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetKeyEventObserverState_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableAbility_002
 * @tc.name: EnableAbility
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(MockA11yManagerServiceUnitTest, EnableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableAbility_002 start";
    sleep(SLEEP_TIME_1);
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
HWTEST_F(MockA11yManagerServiceUnitTest, GetEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetEnabledAbilities_002 start";
    sleep(SLEEP_TIME_1);
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
HWTEST_F(MockA11yManagerServiceUnitTest, DisableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableAbility_002 start";
    sleep(SLEEP_TIME_1);
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
HWTEST_F(MockA11yManagerServiceUnitTest, EnableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableUITestAbility_002 start";
    sleep(SLEEP_TIME_1);
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
HWTEST_F(MockA11yManagerServiceUnitTest, DisableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_EQ(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility(), RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableUITestAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_EnableShortKeyTargetAbility_002
 * @tc.name: EnableShortKeyTargetAbility
 * @tc.desc: Test function EnableShortKeyTargetAbility
 */
HWTEST_F(MockA11yManagerServiceUnitTest, EnableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_EnableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_DisableShortKeyTargetAbility_002
 * @tc.name: DisableShortKeyTargetAbility
 * @tc.desc: Test function DisableShortKeyTargetAbility
 */
HWTEST_F(MockA11yManagerServiceUnitTest, DisableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_DisableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_PackageRemoved_002
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(MockA11yManagerServiceUnitTest, PackageRemoved_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageRemoved_002 start";
    sleep(SLEEP_TIME_1);
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
HWTEST_F(MockA11yManagerServiceUnitTest, PackageAdd_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageAdd_002 end";
}

/*
* @tc.number: MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_002
* @tc.name: RegisterConfigObserver
* @tc.desc: Test function RegisterConfigObserver
*/
HWTEST_F(MockA11yManagerServiceUnitTest, RegisterConfigObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_002 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_Unittest_RegisterConfigObserver_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */

HWTEST_F(MockA11yManagerServiceUnitTest, SetScreenMagnificationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetScreenMagnificationState_000 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetShortKeyState_002
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortKeyState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortKeyState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortKeyState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetMouseKeyState_002
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseKeyState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseKeyState_002 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetSearchElementInfoByAccessibilityIdResult_001
 * @tc.name: SetSearchElementInfoByAccessibilityIdResult
 * @tc.desc: Test function SetSearchElementInfoByAccessibilityIdResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetSearchElementInfoByAccessibilityIdResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ManagerService_UnitTest_SetSearchElementInfoByAccessibilityIdResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "ManagerService_UnitTest_SetSearchElementInfoByAccessibilityIdResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetSearchElementInfoByTextResult_001
 * @tc.name: SetSearchElementInfoByTextResult
 * @tc.desc: Test function SetSearchElementInfoByTextResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetSearchElementInfoByTextResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetSearchElementInfoByTextResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetSearchElementInfoByTextResult(infos, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetSearchElementInfoByTextResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetFindFocusedElementInfoResult_001
 * @tc.name: SetFindFocusedElementInfoResult
 * @tc.desc: Test function SetFindFocusedElementInfoResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetFindFocusedElementInfoResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetFindFocusedElementInfoResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetFindFocusedElementInfoResult(info, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetFindFocusedElementInfoResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetFocusMoveSearchResult_001
 * @tc.name: SetFocusMoveSearchResult
 * @tc.desc: Test function SetFocusMoveSearchResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetFocusMoveSearchResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetFocusMoveSearchResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetFocusMoveSearchResult(info, requestId);
    delete obj;;
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetFocusMoveSearchResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetExecuteActionResult_001
 * @tc.name: SetExecuteActionResult
 * @tc.desc: Test function SetExecuteActionResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetExecuteActionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetExecuteActionResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    bool succeeded = true;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetExecuteActionResult(succeeded, requestId);
    delete obj;;
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetExecuteActionResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetCursorPositionResult_001
 * @tc.name: SetCursorPositionResult
 * @tc.desc: Test function SetCursorPositionResult
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetCursorPositionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetCursorPositionResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    int32_t cursorPosition = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetCursorPositionResult(cursorPosition, requestId);
    delete obj;

    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetCursorPositionResult_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_SetShortkeyMultiTarget_001
 * @tc.name: SetShortkeyMultiTarget
 * @tc.desc: Test function SetShortkeyMultiTarget
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetShortkeyMultiTarget_001 start";
    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_SetShortkeyMultiTarget_001 end";
}

/**
 * @tc.number: Accessible_Ability_ManagerService_UnitTest_GetShortkeyMultiTarget_001
 * @tc.name: GetShortkeyMultiTarget
 * @tc.desc: Test function GetShortkeyMultiTarget
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_GetShortkeyMultiTarget_001 start";
    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyMultiTarget(name);
    GTEST_LOG_(INFO) << "Accessible_Ability_ManagerService_UnitTest_GetShortkeyMultiTarget_001 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_001
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData() == nullptr);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_001 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_002
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_001 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, false, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetShortKeyTimeout(), SHORT_KEY_TIMEOUT_BEFORE_USE);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_002 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_003
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_003 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetShortKeyTimeout(), SHORT_KEY_TIMEOUT_BEFORE_USE);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_003 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_004
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_004 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    EXPECT_EQ(accountData->GetConfig()->GetShortkeyMultiTarget().size(), 0);

    size_t size = accountData->GetConfig()->GetEnabledAccessibilityServices().size();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetEnabledAccessibilityServices().size(), size);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_004 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_005
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_005 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    std::vector<std::string> name;
    name.push_back("test1");
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    EXPECT_EQ(accountData->GetConfig()->GetShortkeyMultiTarget().size(), 1);

    size_t size = accountData->GetConfig()->GetEnabledAccessibilityServices().size();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetEnabledAccessibilityServices().size(), size);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_005 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_006
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnShortKeyProcess_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_006 start";
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        GTEST_LOG_(INFO) << "service is nullptr";
        return;
    }
    service->PutBoolValue(DEVICE_PROVISIONED, true, true);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    std::vector<std::string> name;
    name.push_back("test1");
    name.push_back("test2");
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    EXPECT_EQ(accountData->GetConfig()->GetShortkeyMultiTarget().size(), 2);

    size_t size = accountData->GetConfig()->GetEnabledAccessibilityServices().size();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_EQ(accountData->GetConfig()->GetEnabledAccessibilityServices().size(), size);
    GTEST_LOG_(INFO) << "AccessibleAbility_ManagerService_UnitTest_OnShortKeyProcess_006 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseAutoClick_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(value));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetMouseAutoClick_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortkeyTarget_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(name));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetShortkeyTarget_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetHighContrastTextState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetHighContrastTextState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetHighContrastTextState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetDaltonizationState_002
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetDaltonizationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK,
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_NE(RET_OK,
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetInvertColorState_002
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetInvertColorState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetInvertColorState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAnimationOffState_002
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAnimationOffState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAnimationOffState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAnimationOffState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAudioMonoState_002
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioMonoState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(state));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioMonoState_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetDaltonizationColorFilter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t filter = 1;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(filter));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetContentTimeout_002
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetContentTimeout_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t time = CONTENT_TIMEOUT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(time));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(time));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetContentTimeout_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetBrightnessDiscount_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(value));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_SetAudioBalance_002
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(MockA11yManagerServiceUnitTest, SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioBalance_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = AUDIO_BALANCE_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(value));
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_SetAudioBalance_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_GetAllConfigs_002
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(MockA11yManagerServiceUnitTest, GetAllConfigs_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAllConfigs_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::shared_ptr<AccessibilityConfigData> data = std::make_shared<AccessibilityConfigData>();
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(*data);
    EXPECT_NE(data.get(), nullptr);
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_GetAllConfigs_002 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_001
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnRemoveSystemAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_001 start";
    int32_t systemAbilityId = 1;
    std::string deviceId = "test";
    auto &ins = Singleton<AccessibleAbilityManagerService>::GetInstance();
    ins.OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_TRUE(ins.IsServiceReady());
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_001 end";
}

/**
 * @tc.number: MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_002
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(MockA11yManagerServiceUnitTest, OnRemoveSystemAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_002 start";
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
    GTEST_LOG_(INFO) << "MockA11yManagerServiceUnitTest_OnRemoveSystemAbility_002 end";
}

/**
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_UpdateConfigState_001
 * @tc.name: UpdateConfigState
 * @tc.desc: Test function UpdateConfigState
 */
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateConfigState_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateAudioBalance_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateBrightnessDiscount_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateContentTimeout_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateDaltonizationColorFilter_001, TestSize.Level1)
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
HWTEST_F(MockA11yManagerServiceUnitTest, UpdateMouseAutoClick_001, TestSize.Level1)
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
} // namespace Accessibility
} // namespace OHOS