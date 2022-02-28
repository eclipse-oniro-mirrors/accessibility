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
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessible_ability_manager_service_state_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "iservice_registry.h"
#include "json.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace Json;

namespace OHOS {
namespace Accessibility {
extern OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> g_testStub;
extern uint32_t g_testStateType;
extern int g_testEventType;
extern int g_testWindowChangeTypes;
extern int g_testWindowId;

const int timeout = 10000;
const int testNum_2 = 2;
const int testNum_3 = 3;

class AAMSServerTest : public testing::Test {
public:
    AAMSServerTest() {}
    ~AAMSServerTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
    void AddAccessibleAbilityConnection();
    void CreateAccessibilityConfigForTouchGuide();
    void WritefileAll(const char* fname, const char* data);
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

    EXPECT_CALL(*mock_, GetUidByBundleName(_, _)).WillRepeatedly(Return(0));
}

void AAMSServerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AAMSServerTest TearDown";
    g_testStub = nullptr;
    g_testStateType = -1;
    g_testEventType = -1;
    g_testWindowChangeTypes = -1;
    g_testWindowId = -1;
    aams_->OnStop();
    mock_ = nullptr;
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
    aams_ = nullptr;
}

void AAMSServerTest::WritefileAll(const char* fname, const char* data)
{
    FILE *fp;
    if((fp = fopen(fname, "w")) == nullptr) {
        printf("open file %s fail \n", fname);
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AAMSServerTest::CreateAccessibilityConfigForTouchGuide()
{
    std::ostringstream os;
    Json::Value object1, targetBundleNames;
    Json::Value accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (remove("/system/app/dummy_accessibility_ability_config.json") == 0) {
        GTEST_LOG_(INFO) << "remove successful";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.aalisttest";
    targetBundleNames[1] = "com.example.aalisttest2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "spoken";
    accessibilityAbilityTypes[testNum_2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = timeout;
    accessibilityCapabilities[0] = "retrieve";
    accessibilityCapabilities[1] = "touchGuide";
    accessibilityCapabilities[testNum_2] = "keyEventObserver";
    accessibilityCapabilities[testNum_3] = "gesture";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.ohos.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    WritefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

void AAMSServerTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AAMSServerTest AddAccessibleAbilityConnection";
    // accessibleAbility connection
    AppExecFwk::ExtensionAbilityInfo info;
    AAFwk::Want want;
    AppExecFwk::ElementName name;
    std::string deviceId;
    name.SetAbilityName("com.example.aalisttest.MainAbility");
    name.SetBundleName("com.example.aalisttest");
    want.SetElement(name);

    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    accountData_ = aams_->GetCurrentAccountData();
    AAConnection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName(deviceId, info.bundleName, info.name);
    aastub_ = new AccessibleAbilityClientStubImpl();
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
    // register AA
    AddAccessibleAbilityConnection();
    // make a event
    AccessibilityEventInfo eventInfo;
    eventInfo.SetEventType(EventType::TYPE_WINDOW_UPDATE);
    // aams send event
    aams_->SendEvent(eventInfo, 100);
    sleep(1);
    // check aa proxy
    EXPECT_EQ(g_testEventType, int(EventType::TYPE_WINDOW_UPDATE));

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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
    CreateAccessibilityConfigForTouchGuide();
    AddAccessibleAbilityConnection();

    AppExecFwk::ExtensionAbilityInfo info;
    sptr<AccessibilityAbilityInfo> installAbilityInfo = new AccessibilityAbilityInfo(info);
    installAbilityInfo->SetPackageName("123");
    auto accountData = aams_->GetCurrentAccountData();

    /* add install ability dummy*/
    EXPECT_EQ(1, (int)accountData->GetInstalledAbilities().size());
    accountData->AddInstalledAbility(*installAbilityInfo);
    EXPECT_EQ(2, (int)accountData->GetInstalledAbilities().size());

    /* ABILITY_STATE_DISABLE */
    int stateType = AbilityStateType::ABILITY_STATE_DISABLE;
    auto ret = aams_->GetAbilityList(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN, stateType);
    EXPECT_EQ(int(ret.size()), 1);

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

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);
    aams_->RegisterElementOperator(0, nullptr, 100);
    sleep(1);
    GTEST_LOG_(INFO) << "RegisterElementOperator OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    aams_->DeregisterElementOperator(0);
    sleep(1);
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
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
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    AddAccessibleAbilityConnection();
    aams_->RegisterElementOperator(0, nullptr, 0);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // wrong windowId
    aams_->DeregisterElementOperator(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 1);

    // true windowId
    aams_->DeregisterElementOperator(0);
    sleep(1);
    map = accountData->GetAsacConnections();
    EXPECT_EQ(int(map.size()), 0);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    accountData_->ClearInstalledAbility();
    GTEST_LOG_(INFO) << "AAMSServerTest DeregisterElementOperator_001 end";
}
} // namespace Accessibility
} // namespace OHOS