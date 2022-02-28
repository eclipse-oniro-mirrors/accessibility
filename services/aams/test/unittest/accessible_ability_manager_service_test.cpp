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

#include "accessible_ability_manager_service.h"
#include <gtest/gtest.h>
#include <optional>
#include "accessibility_display_manager.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service_state_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

extern int g_testChannalId;
extern int g_testEventType;

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceUnitTest : public ::testing::Test {
public:
    AccessibleAbilityManagerServiceUnitTest() {}
    ~AccessibleAbilityManagerServiceUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;

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
    // 注册bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();

    stub_ = new AccessibleAbilityClientStubImpl();
}

void AccessibleAbilityManagerServiceUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest TearDown";

    ins_ = nullptr;
    stub_ = nullptr;
    mock_ = nullptr;
}

void AccessibleAbilityManagerServiceUnitTest::RegisterAbilityConnectionClient(const sptr<IRemoteObject>& obj)
{
    // add an ability connection client
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(extensionInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    connection->OnAbilityConnectDone(elementName, obj, 0);
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
 * @tc.number: AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerServiceUnitTest_Unittest_RegisterStateCallback_001 start";

    sptr<AccessibleAbilityManagerServiceStateStub> stub = new AccessibleAbilityManagerServiceStateStub();
    sptr<IAccessibleAbilityManagerServiceState> state = new AccessibleAbilityManagerServiceStateProxy(stub);

    int ret = ins_->RegisterStateCallback(state, 1);
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
    auto ret = ins_->GetAbilityList(0xFFFFFFFF, 3);
    EXPECT_EQ(int(ret.size()), 0);

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
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    if (accountData == nullptr) {
        GTEST_LOG_(INFO) << "accountData is null";
    } else {
        GTEST_LOG_(INFO) << "accountData is not null";
    }
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    ins_->RegisterElementOperator(0, nullptr, 0);
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
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    ins_->DeregisterElementOperator(0);
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
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    auto ret = ins_->IsWantedKeyEvent(*event);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_IsWantedKeyEvent_001 end";
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
 * @tc.number: AccessibleAbility_ManagerService_UnitTest_RemovedUser_001
 * @tc.name: RemovedUser
 * @tc.desc: Test function RemovedUser
 */

HWTEST_F(AccessibleAbilityManagerServiceUnitTest, RemovedUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_RemovedUser_001 start";
    RegisterAbilityConnectionClient(stub_);
    EXPECT_EQ(int(ins_->GetCurrentAccountData()->GetConnectedA11yAbilities().size()), 1);
    ins_->RemovedUser(1);
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
    auto ret = ins_->GetBundleMgrProxy();
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_GetBundleMgrProxyr_001 end";
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
    EXPECT_EQ(1, int(accountData->GetConnectedA11yAbilities().size()));
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
    std::string bundleName = "bundleName1";
    ins_->PackageChanged(bundleName);
    GTEST_LOG_(INFO) << "PackageChanged end";
    EXPECT_EQ(0, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "GetInstalledAbilities end";
    /* add install ability */
    RegisterAbilityConnectionClient(stub_);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    bundleName = "bundleName2";
    ins_->PackageChanged(bundleName);
    EXPECT_EQ(1, int(accountData->GetInstalledAbilities().size()));
    GTEST_LOG_(INFO) << "Accessible_Ability_Manager_ServiceUnittest_PackageChanged_001 end";
}
} // namespace Accessibility
} // namespace OHOS