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
#include "accessibility_setting.h"
#include "accessibility_helper.h"
#include "accessibility_window.h"
#include "AccessibilitySettingObserver_service.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_config_observer_proxy.h"
#include "mock_accessible__config_observer_stub.h"
#include "mock_accessible__observer_proxy.h"
#include "mock_accessible_observer_stub.h"
#include "mock_bundle.h"
#include "system_ability.h"

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

class AccessibilitySettingObserverTest : public ::testing::Test {
public:
    AccessibilitySettingObserverTest()
    {}
    ~AccessibilitySettingObserverTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityClientStub> stub_ = nullptr;
    void RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj);
};

void AccessibilitySettingObserverTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
}

void AccessibilitySettingObserverTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
    AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
}

void AccessibilitySettingObserverTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest SetUp";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityAbilityHelper::accountId_);
    stub_ = new MockAccessibleAbilityClientStubImpl();
}

void AccessibilitySettingObserverTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest TearDown";
    stub_ = nullptr;
}

void AccessibilitySettingObserverTest::RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    // add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    EXPECT_TRUE(accountData);

    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    connection->OnAbilityConnectDoneSync(elementName, obj);
    sleep(SLEEP_TIME_1);
    EXPECT_NE(abilityInfo.get(), nullptr);
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibilitySettingObserverTest, GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_GetInstance_001 start";

    auto ins = &Singleton<AccessibleAbilityManagerService>::GetInstance();
    EXPECT_TRUE(ins);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_GetInstance_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_001 start";

    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibilitySettingObserverTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAbilityList_001 start";
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(
        ACCESSIBILITY_ABILITY_TYPE_ALL, ABILITY_STATE_ENABLE, infos);
    EXPECT_EQ(infos.size(), 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_GetAbilityList_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator AccessibilitySettingObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_001 start";
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

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator AccessibilitySettingObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DeregisterElementOperator_001 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_EQ(RET_OK, aams.DeregisterElementOperator(0));
    sleep(SLEEP_TIME_1);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DeregisterElementOperator_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_DeregisterElementOperator_002
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibilitySettingObserverTest, DeregisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DeregisterElementOperator_002 start";
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams.GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_EQ(RET_OK, aams.RegisterElementOperator(0, nullptr, true));
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(RET_OK, aams.DeregisterElementOperator(0));
    sleep(SLEEP_TIME_1);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DeregisterElementOperator_002 end";
}


/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetTouchEventInjector_001
 * @tc.name: SetTouchEventInjector
 * @tc.desc: Test function SetTouchEventInjector
 */
HWTEST_F(AccessibilitySettingObserverTest, SetTouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_SetTouchEventInjector_001 start";
    sptr<TouchEventInjector> touchEventInjector = new TouchEventInjector();
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(touchEventInjector);
    auto ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_SetTouchEventInjector_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetKeyEventFilter_001
 * @tc.name: SetKeyEventFilter
 * @tc.desc: Test function SetKeyEventFilter
 */
HWTEST_F(AccessibilitySettingObserverTest, SetKeyEventFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_SetKeyEventFilter_001 start";
    sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(keyEventFilter);
    auto ret = Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_SetKeyEventFilter_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_RemovedUser_001
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */
HWTEST_F(AccessibilitySettingObserverTest, RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_RemovedUser_001 start";
    RegisterAbilityConnectionClient(stub_);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_TRUE(int(accountData->GetConnectedA11yAbilities().size()) >= 1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    // can't to check a11yAccountsData_ because it is private,and don't provite api.
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_RemovedUser_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_RemovedUser_002
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */
HWTEST_F(AccessibilitySettingObserverTest, RemovedUser_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_RemovedUser_002 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(1);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(1);
    sleep(SLEEP_TIME_1);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_EQ(accountData->GetAccountId(), 1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(1);
    // can't to check a11yAccountsData_ because it is private,and don't provite api.
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_RemovedUser_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_PackageChanged_001
 * @tc.name: PackageChanged
 * @tc.desc: Test function PackageChanged
 */
HWTEST_F(AccessibilitySettingObserverTest, PackageChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageChanged_001 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    accountData->ClearInstalledAbility();
    std::string bundleName = "bundle1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
    GTEST_LOG_(INFO) << "PackageChanged end";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "GetInstalledAbilities end";
    /* add install ability */
    sleep(SLEEP_TIME_1);
    RegisterAbilityConnectionClient(stub_);
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    bundleName = "bundle2";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
    sleep(SLEEP_TIME_1);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageChanged_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetScreenMagnificationState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetShortKeyState_001
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingObserverTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortKeyState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetMouseKeyState_001
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingObserverTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseKeyState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseKeyState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingObserverTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseAutoClick_001 start";
    int32_t ret = 0;
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseAutoClick_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingObserverTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortkeyTarget_001 start";
    std::string name = "test";
    std::string ret = "";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortkeyTarget_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingObserverTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetHighContrastTextState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetHighContrastTextState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetDaltonizationState_001
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetDaltonizationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetInvertColorState_001
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingObserverTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetInvertColorState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetInvertColorState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAnimationOffState_001
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAnimationOffState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAnimationOffState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAudioMonoState_001
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioMonoState_001 start";
    bool state = true;
    bool ret = false;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioMonoState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingObserverTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationColorFilter_001 start";
    uint32_t filter = 1;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetContentTimeout_001
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingObserverTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetContentTimeout_001 start";
    uint32_t value = CONTENT_TIMEOUT_VALUE;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(value));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetContentTimeout_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingObserverTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetBrightnessDiscount_001 start";
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    RetError result = Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value);
    EXPECT_EQ(RET_ERR_NULLPTR, result);
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAudioBalance_001
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioBalance_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(AUDIO_BALANCE_VALUE));
    float ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioBalance_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetClickResponseTime_001
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibilitySettingObserverTest, SetClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetClickResponseTime_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetClickResponseTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetClickResponseTime(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetIgnoreRepeatClickState_001
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetIgnoreRepeatClickState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetIgnoreRepeatClickState_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickState(true));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetIgnoreRepeatClickTime_001
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilitySettingObserverTest, SetIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetIgnoreRepeatClickTime_001 start";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetIgnoreRepeatClickTime(1));
    uint32_t ret = 0;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetIgnoreRepeatClickTime(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetAllConfigs_001
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(AccessibilitySettingObserverTest, GetAllConfigs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAllConfigs_001 start";
    AccessibilityConfigData data;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(data);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAllConfigs_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetCaptionState_001
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetCaptionState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionState_001 start";
    bool state = true;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    bool ret = false;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(ret));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetEnabledState_001
 * @tc.name: GetEnabledState
 * @tc.desc: Test function GetEnabledState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetEnabledState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetEnabledState_001 start";
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetEnabledState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetTouchGuideState_001
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetTouchGuideState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetTouchGuideState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetTouchGuideState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetGestureState_001
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetGestureState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetGestureState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetGestureState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetKeyEventObserverState_001
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetKeyEventObserverState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetKeyEventObserverState_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetKeyEventObserverState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetActiveWindow_001
 * @tc.name: GetActiveWindow
 * @tc.desc: Test function GetActiveWindow
 */
HWTEST_F(AccessibilitySettingObserverTest, GetActiveWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetActiveWindow_001 start";
    Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(ACTIVE_WINDOW_VALUE);
    EXPECT_EQ(ACTIVE_WINDOW_VALUE, Singleton<AccessibleAbilityManagerService>::GetInstance().GetActiveWindow());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetActiveWindow_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetCaptionProperty_001
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingObserverTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionProperty_001 start";
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));

    AccessibilityConfig::CaptionProperty res;
    EXPECT_EQ(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(res));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionProperty_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_PackageAdd_001
 * @tc.name: PackageAdd
 * @tc.desc: Test function PackageAdd
 */
HWTEST_F(AccessibilitySettingObserverTest, PackageAdd_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageAdd_001 start";

    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageAdd_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_PackageRemoved_001
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(AccessibilitySettingObserverTest, PackageRemoved_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageRemoved_001 start";
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageRemoved_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_AddedUser_001
 * @tc.name: AddedUser
 * @tc.desc: Test function AddedUser
 */
HWTEST_F(AccessibilitySettingObserverTest, AddedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_AddedUser_001 start";
    int32_t accountId = 1;
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(accountId);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_AddedUser_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableShortKeyTargetAbility_001
 * @tc.name: EnableShortKeyTargetAbility
 * @tc.desc: Test function EnableShortKeyTargetAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableShortKeyTargetAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableShortKeyTargetAbility_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableShortKeyTargetAbility_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableShortKeyTargetAbility_001
 * @tc.name: DisableShortKeyTargetAbility
 * @tc.desc: Test function DisableShortKeyTargetAbility AccessibilitySettingObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableShortKeyTargetAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableShortKeyTargetAbility_001 start";
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableShortKeyTargetAbility_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableAbility_001
 * @tc.name: EnableAbility
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableAbility_001 start";
    std::string name = "test";
    uint32_t capabilities = 1;
    EXPECT_EQ(RET_ERR_NOT_INSTALLED,
        Singleton<AccessibleAbilityManagerService>::GetInstance().EnableAbility(name, capabilities));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableAbility_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableAbility_001
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableAbility_001 start";
    std::string name = "test";
    EXPECT_EQ(RET_ERR_NOT_ENABLED, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableAbility(name));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableAbility_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_GetEnabledAbilities_001
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibilitySettingObserverTest, GetEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_GetEnabledAbilities_001 start";
    std::vector<std::string> enabledAbilities;
    EXPECT_EQ(RET_OK,
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledAbilities(enabledAbilities));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_GetEnabledAbilities_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableUITestAbility_001
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableUITestAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableUITestAbility_001 start";
    sptr<IRemoteObject> obj;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj));
    GTEST_LOG_(INFO) << "Accessib le_Ability_Manager_ServiceUnittest_EnableUITestAbility_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableUITestAbility_001
 * @tc.name: DisableUITestAbility
 * @tc.desc: Test function DisableUITestAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableUITestAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableUITestAbility_001 start";
    EXPECT_EQ(RET_ERR_NO_CONNECTION, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableUITestAbility_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_001
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterCaptionObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_001 start";

    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_001
 * @tc.name: RegisterConfigObserver
 * @tc.desc: Test function RegisterConfigObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterConfigObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_001 start";

    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_001 end";
}

/*
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_001
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterEnableAbilityListsObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_001 start";

    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SwitchedUser_001
 * @tc.name: SwitchedUser
 * @tc.desc: Test function SwitchedUser
 */
HWTEST_F(AccessibilitySettingObserverTest, SwitchedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SwitchedUser_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(100);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(2);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SwitchedUser_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_GetAbilityList_002
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibilitySettingObserverTest, GetAbilityList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAbilityList_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo> infos;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAbilityList(0xFFFFFFFF, 3, infos);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_GetAbilityList_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_RegisterElementOperator_002
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterElementOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, nullptr, true);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_RegisterElementOperator_003
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterElementOperator_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_003 start";
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    sptr<IAccessibilityElementOperator> operation;
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(0, operation, true);
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterElementOperator_003 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateAccessibilityManagerService_001
 * @tc.name: UpdateAccessibilityManagerService
 * @tc.desc: Test function UpdateAccessibilityManagerService
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateAccessibilityManagerService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateAccessibilityManagerService_001 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateAccessibilityManagerService_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_002
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterStateCallback_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_00 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<AccessibleAbilityManagerStateObserverStub> stub = new MockAccessibleAbilityManagerStateObserverStub();
    sptr<IAccessibleAbilityManagerStateObserver> state = new MockAccessibleAbilityManagerStateObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterStateObserver(state);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterStateCallback_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_002
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterCaptionObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibleAbilityManagerCaptionObserverStub> stub = new MockAccessibleAbilityManagerCaptionObserverStub();
    sptr<IAccessibleAbilityManagerCaptionObserver> state = new MockAccessibleAbilityManagerCaptionObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(state);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_003
 * @tc.name: RegisterCaptionObserver
 * @tc.desc: Test function RegisterCaptionObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterCaptionObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_003 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterCaptionObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterCaptionObserver_003 end";
}

/*
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_002
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterEnableAbilityListsObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<MockAccessibilityEnableAbilityListsObserverStub> stub = new MockAccessibilityEnableAbilityListsObserverStub();
    sptr<IAccessibilityEnableAbilityListsObserver> state = new MockAccessibilityEnableAbilityListsObserverProxy(stub);

    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(state);
    EXPECT_NE(stub.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_003
 * @tc.name: RegisterEnableAbilityListsObserver
 * @tc.desc: Test function RegisterEnableAbilityListsObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterEnableAbilityListsObserver_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_003 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterEnableAbilityListsObserver(nullptr);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterEnableAbilityListsObserver_003 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetCaptionProperty_002
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingObserverTest, SetCaptionProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionProperty_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    AccessibilityConfig::CaptionProperty caption;
    int scale = 1;
    caption.SetFontScale(scale);
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionProperty(caption));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionProperty(caption));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionProperty_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetCaptionState_002
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetCaptionState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetCaptionState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetCaptionState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetCaptionState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetTouchGuideState_002
 * @tc.name: GetTouchGuideState
 * @tc.desc: Test function GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetTouchGuideState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetTouchGuideState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetTouchGuideState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetGestureState_002
 * @tc.name: GetGestureState
 * @tc.desc: Test function GetGestureState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetGestureState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetGestureState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetGestureState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetGestureState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetKeyEventObserverState_002
 * @tc.name: GetKeyEventObserverState
 * @tc.desc: Test function GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingObserverTest, GetKeyEventObserverState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetKeyEventObserverState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetKeyEventObserverState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableAbility_002
 * @tc.name: EnableAbility
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    uint32_t capabilities = 1;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().EnableAbility(name, capabilities));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableAbility_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_GetEnabledAbilities_002
 * @tc.name: GetEnabledAbilities
 * @tc.desc: Test function GetEnabledAbilities
 */
HWTEST_F(AccessibilitySettingObserverTest, GetEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_GetEnabledAbilities_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::vector<std::string> enabledAbilities;
    EXPECT_EQ(RET_ERR_NULLPTR,
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetEnabledAbilities(enabledAbilities));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_GetEnabledAbilities_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableAbility_002
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().DisableAbility(name));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableAbility_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableUITestAbility_002
 * @tc.name: EnableUITestAbility
 * @tc.desc: Test function EnableUITestAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableUITestAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sptr<IRemoteObject> obj;
    EXPECT_EQ(RET_ERR_NULLPTR, Singleton<AccessibleAbilityManagerService>::GetInstance().EnableUITestAbility(obj));
    GTEST_LOG_(INFO) << "Accessib le_Ability_Manager_ServiceUnittest_EnableUITestAbility_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableUITestAbility_002
 * @tc.name: DisableUITestAbility
 * @tc.desc: Test function DisableUITestAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableUITestAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableUITestAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_EQ(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableUITestAbility(), RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableUITestAbility_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_EnableShortKeyTargetAbility_002
 * @tc.name: EnableShortKeyTargetAbility
 * @tc.desc: Test function EnableShortKeyTargetAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, EnableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableShortKeyTargetAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_EnableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_DisableShortKeyTargetAbility_002
 * @tc.name: DisableShortKeyTargetAbility
 * @tc.desc: Test function DisableShortKeyTargetAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, DisableShortKeyTargetAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableShortKeyTargetAbility_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    EXPECT_FALSE(Singleton<AccessibleAbilityManagerService>::GetInstance().DisableShortKeyTargetAbility());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_DisableShortKeyTargetAbility_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_PackageRemoved_002
 * @tc.name: PackageRemoved
 * @tc.desc: Test function PackageRemoved
 */
HWTEST_F(AccessibilitySettingObserverTest, PackageRemoved_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageRemoved_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageRemoved_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_PackageAdd_002
 * @tc.name: PackageAdd
 * @tc.desc: Test function PackageAdd
 */
HWTEST_F(AccessibilitySettingObserverTest, PackageAdd_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageAdd_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string bundleName = "bundleName1";
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_ServiceUnittest_PackageAdd_002 end";
}

/*
 * @tc.number: AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_002
 * @tc.name: RegisterConfigObserver
 * @tc.desc: Test function RegisterConfigObserver
 */
HWTEST_F(AccessibilitySettingObserverTest, RegisterConfigObserver_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_002 start";
    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_Unittest_RegisterConfigObserver_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */

HWTEST_F(AccessibilitySettingObserverTest, SetScreenMagnificationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetScreenMagnificationState_000 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetScreenMagnificationState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetScreenMagnificationState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetShortKeyState_002
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingObserverTest, SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortKeyState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortKeyState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortKeyState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetMouseKeyState_002
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingObserverTest, SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseKeyState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseKeyState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseKeyState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseKeyState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetSearchElementInfoByAccessibilityIdResult_001
 * @tc.name: SetSearchElementInfoByAccessibilityIdResult
 * @tc.desc: Test function SetSearchElementInfoByAccessibilityIdResult 
 */
HWTEST_F(AccessibilitySettingObserverTest, SetSearchElementInfoByAccessibilityIdResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetSearchElementInfoByAccessibilityIdResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetSearchElementInfoByAccessibilityIdResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetSearchElementInfoByTextResult_001
 * @tc.name: SetSearchElementInfoByTextResult
 * @tc.desc: Test function SetSearchElementInfoByTextResult
 */
HWTEST_F(AccessibilitySettingObserverTest, SetSearchElementInfoByTextResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetSearchElementInfoByTextResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetSearchElementInfoByTextResult(infos, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetSearchElementInfoByTextResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetFindFocusedElementInfoResult_001
 * @tc.name: SetFindFocusedElementInfoResult
 * @tc.desc: Test function SetFindFocusedElementInfoResult
 */
HWTEST_F(AccessibilitySettingObserverTest, SetFindFocusedElementInfoResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetFindFocusedElementInfoResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetFindFocusedElementInfoResult(info, requestId);
    delete obj;
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetFindFocusedElementInfoResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetFocusMoveSearchResult_001
 * @tc.name: SetFocusMoveSearchResult
 * @tc.desc: Test function SetFocusMoveSearchResult
 */
HWTEST_F(AccessibilitySettingObserverTest, SetFocusMoveSearchResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetFocusMoveSearchResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetFocusMoveSearchResult(info, requestId);
    delete obj;;
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetFocusMoveSearchResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetExecuteActionResult_001
 * @tc.name: SetExecuteActionResult
 * @tc.desc: Test function SetExecuteActionResult
 */
HWTEST_F(AccessibilitySettingObserverTest, SetExecuteActionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetExecuteActionResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    bool succeeded = true;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetExecuteActionResult(succeeded, requestId);
    delete obj;;
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetExecuteActionResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetCursorPositionResult_001
 * @tc.name: SetCursorPositionResult
 * @tc.desc: Test function SetCursorPositionResult
 */
HWTEST_F(AccessibilitySettingObserverTest, SetCursorPositionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetCursorPositionResult_001 start";
    AccessibilityElementInfo info;
    int32_t requestId = 0;
    int32_t cursorPosition = 0;
    AccessibleAbilityManagerService::ElementOperatorCallbackImpl *obj = new
        AccessibleAbilityManagerService::ElementOperatorCallbackImpl();
    obj->SetCursorPositionResult(cursorPosition, requestId);
    delete obj;

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetCursorPositionResult_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_SetShortkeyMultiTarget_001
 * @tc.name: SetShortkeyMultiTarget
 * @tc.desc: Test function SetShortkeyMultiTarget
 */
HWTEST_F(AccessibilitySettingObserverTest, SetShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetShortkeyMultiTarget_001 start";
    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyMultiTarget(name);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_SetShortkeyMultiTarget_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_GetShortkeyMultiTarget_001
 * @tc.name: GetShortkeyMultiTarget
 * @tc.desc: Test function GetShortkeyMultiTarget
 */
HWTEST_F(AccessibilitySettingObserverTest, GetShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_GetShortkeyMultiTarget_001 start";
    std::vector<std::string> name;
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyMultiTarget(name);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_GetShortkeyMultiTarget_001 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_001
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_001 start";
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
    EXPECT_TRUE(Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData() == nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_001 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_002
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_001 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_002 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_003
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_003 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_003 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_004
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_004 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_004 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_005
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_005 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_005 end";
}

/**OnShortKeyProcess
 * @tc.number: AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_006
 * @tc.name: OnShortKeyProcess
 * @tc.desc: Test function OnShortKeyProcess
 */
HWTEST_F(AccessibilitySettingObserverTest, OnShortKeyProcess_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_006 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_OnShortKeyProcess_006 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingObserverTest, SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseAutoClick_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    int32_t value = MOUSE_AUTO_CLICK_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetMouseAutoClick(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetMouseAutoClick(value));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetMouseAutoClick_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingObserverTest, SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortkeyTarget_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::string name = "test";
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetShortkeyTarget(name));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetShortkeyTarget(name));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetShortkeyTarget_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingObserverTest, SetHighContrastTextState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetHighContrastTextState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetHighContrastTextState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetHighContrastTextState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetHighContrastTextState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetDaltonizationState_002
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetDaltonizationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK,
     Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationState(state));
    EXPECT_NE(RET_OK,
     Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetInvertColorState_002
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingObserverTest, SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetInvertColorState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetInvertColorState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetInvertColorState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetInvertColorState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAnimationOffState_002
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAnimationOffState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAnimationOffState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAnimationOffState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAnimationOffState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAnimationOffState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAudioMonoState_002
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioMonoState_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    bool state = true;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioMonoState(state));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioMonoState(state));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioMonoState_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingObserverTest, SetDaltonizationColorFilter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationColorFilter_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t filter = 1;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetDaltonizationColorFilter(filter));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetDaltonizationColorFilter(filter));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetContentTimeout_002
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingObserverTest, SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetContentTimeout_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    uint32_t time = CONTENT_TIMEOUT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetContentTimeout(time));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetContentTimeout(time));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetContentTimeout_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingObserverTest, SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetBrightnessDiscount_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetBrightnessDiscount(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetBrightnessDiscount(value));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_SetAudioBalance_002
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingObserverTest, SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioBalance_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    float value = AUDIO_BALANCE_VALUE;
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().SetAudioBalance(value));
    EXPECT_NE(RET_OK, Singleton<AccessibleAbilityManagerService>::GetInstance().GetAudioBalance(value));
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_SetAudioBalance_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_GetAllConfigs_002
 * @tc.name: GetAllConfigs
 * @tc.desc: Test function GetAllConfigs
 */
HWTEST_F(AccessibilitySettingObserverTest, GetAllConfigs_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAllConfigs_002 start";
    sleep(SLEEP_TIME_1);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(-1);
    std::shared_ptr<AccessibilityConfigData> data = std::make_shared<AccessibilityConfigData>();
    Singleton<AccessibleAbilityManagerService>::GetInstance().GetAllConfigs(*data);
    EXPECT_NE(data.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_GetAllConfigs_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_OnRemoveSystemAbility_001
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, OnRemoveSystemAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_OnRemoveSystemAbility_001 start";
    int32_t systemAbilityId = 1;
    std::string deviceId = "test";
    auto &ins = Singleton<AccessibleAbilityManagerService>::GetInstance();
    ins.OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_TRUE(ins.IsServiceReady());
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_OnRemoveSystemAbility_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserverTest_OnRemoveSystemAbility_002
 * @tc.name: OnRemoveSystemAbility
 * @tc.desc: Test function OnRemoveSystemAbility
 */
HWTEST_F(AccessibilitySettingObserverTest, OnRemoveSystemAbility_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_OnRemoveSystemAbility_002 start";
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
    GTEST_LOG_(INFO) << "AccessibilitySettingObserverTest_OnRemoveSystemAbility_002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateConfigState_001
 * @tc.name: UpdateConfigState
 * @tc.desc: Test function UpdateConfigState
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateConfigState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateConfigState_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateConfigState();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateConfigState_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateAudioBalance_001
 * @tc.name: UpdateAudioBalance
 * @tc.desc: Test function UpdateAudioBalance
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateAudioBalance_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAudioBalance();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateAudioBalance_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateBrightnessDiscount_001
 * @tc.name: UpdateBrightnessDiscount
 * @tc.desc: Test function UpdateBrightnessDiscount
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateBrightnessDiscount_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateBrightnessDiscount();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateContentTimeout_001
 * @tc.name: UpdateContentTimeout
 * @tc.desc: Test function UpdateContentTimeout
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateContentTimeout_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateContentTimeout();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateContentTimeout_001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_UnitTest_UpdateDaltonizationColorFilter_001
 * @tc.name: UpdateDaltonizationColorFilter
 * @tc.desc: Test function UpdateDaltonizationColorFilter
 */
HWTEST_F(AccessibilitySettingObserverTest, UpdateDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateDaltonizationColorFilter_001 start";
    sptr<MockAccessibleAbilityManagerConfigObserverStub> stub = new MockAccessibleAbilityManagerConfigObserverStub();
    sptr<IAccessibleAbilityManagerConfigObserver> state = new MockAccessibleAbilityManagerConfigObserverProxy(stub);

    uint32_t ret = Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterConfigObserver(state);
    EXPECT_EQ(ret, 0);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateDaltonizationColorFilter();
    EXPECT_NE(stub_.GetRefPtr(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_UnitTest_UpdateDaltonizationColorFilter_001 end";
}


/**
 * @tc.number: AccessibilitySettingObserver_Unittest_SetAvailableFunctions002
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_SetAvailableFunctions002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions002 start";

    uint32_t availableFunctions2 = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions2);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_SetAvailableFunctions003
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_SetAvailableFunctions003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions003 start";

    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;

    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions003 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_SetAvailableFunctions004
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_SetAvailableFunctions004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions004 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions004 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_SetAvailableFunctions005
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_SetAvailableFunctions005,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions005 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION |
        AccessibilityInputInterceptor::FEATURE_MOUSE_AUTOCLICK |
        AccessibilityInputInterceptor::FEATURE_MOUSE_KEY;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions005 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_SetAvailableFunctions006
 * @tc.name: SetAvailableFunctions
 * @tc.desc: Check the set available Functions.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_SetAvailableFunctions006,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions006 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_MOUSE_AUTOCLICK;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_SetAvailableFunctions006 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_OnTouchEvent001
 * @tc.name: OnTouchEvent
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_OnTouchEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent001 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    inputInterceptor_->OnPointerEvent(*event);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent001 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_OnTouchEvent002
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_OnTouchEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent002 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessTouchEvent */
    sleep(SLEEP_TIME_3);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent002 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_OnTouchEvent003
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on touch event.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_OnTouchEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent003 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    item.SetDisplayX(1);
    item.SetDisplayY(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessTouchEvent */ 
    sleep(SLEEP_TIME_3);

    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnTouchEvent003 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_OnMouseEvent1
 * @tc.name: OnMouseEvent
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_OnMouseEvent1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnMouseEvent1 start";

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    EXPECT_NE(event.get(), nullptr);
    inputInterceptor_->OnPointerEvent(*event);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnMouseEvent1 end";
}

/**
 * @tc.number: AccessibilitySettingObserver_Unittest_OnMouseEvent002
 * @tc.name: InterceptPointerEventCallBack
 * @tc.desc: Check the on mouse event.
 */
HWTEST_F(AccessibilitySettingObserver, AccessibilitySettingObserver_Unittest_OnMouseEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnMouseEvent002 start";
    uint32_t availableFunctions = AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    inputInterceptor_->SetAvailableFunctions(availableFunctions);

    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    event->AddPointerItem(item);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(event);
    }
    /* wait ProcessMouseEvent */
    sleep(SLEEP_TIME_3);
    EXPECT_NE(event.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingObserver_Unittest_OnMouseEvent002 end";
}
} // namespace Accessibility
} // namespace OHOS