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
#include "json.h"

using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace Json;

extern OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> testStub;
extern int testChannelElementId;
extern int testChannelrequestId;
extern int testChannelmode;
extern std::string testText;
extern int testFocusType;
extern int testDirection;
extern int testAction;
extern int testGestureSimulate;
extern std::map<std::string, std::string> testActionArguments;

namespace OHOS {
namespace Accessibility {

class aamsAccessibleAbilityChannelTest : public testing::Test {
public:

    aamsAccessibleAbilityChannelTest() {}
    ~aamsAccessibleAbilityChannelTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void writefileAll(const char* fname,const char* data);
    void CreateAccessibilityConfigForTouchGuide();
    void CreateAccessibilityConfigNoCapability();
    void AddAccessibleAbilityConnection();
    void AddAccessibilityInteractionConnection();
    void CreateGesturePath(
        GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime);
    std::vector<GesturePathDefine> gestureSteps_;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> aams_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

void aamsAccessibleAbilityChannelTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest SetUpTestCase";
}

void aamsAccessibleAbilityChannelTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest TearDownTestCase";
}

void aamsAccessibleAbilityChannelTest::SetUp()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest SetUp";
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

void aamsAccessibleAbilityChannelTest::TearDown()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest TearDown";
    // reset test value
    testChannelElementId = -1;
    testChannelrequestId = -1;
    testChannelmode = -1;
    testAction = 0;
    testDirection = 0;
    testText = "";
    testFocusType = 0;
    testGestureSimulate = -1;
    testActionArguments.clear();
    // Deregister InteractionOperation
    aams_->DeregisterInteractionOperation(0);
    // stop AAMS
    aams_->OnStop();
    delete mock_;
    aams_ = nullptr;
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
}

void aamsAccessibleAbilityChannelTest::writefileAll(const char* fname,const char* data)
{
    FILE *fp;
    if ((fp=fopen(fname, "w")) == NULL)
    {
        printf("open file %s fail \n", fname);
    }
    
    fprintf(fp, "%s", data);
    fclose(fp);
}

void aamsAccessibleAbilityChannelTest::CreateAccessibilityConfigForTouchGuide() {

    std::ostringstream os;
    Json::Value object1, targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (remove("/system/app/dummy_accessibility_ability_config.json") == 0) {
        GTEST_LOG_(INFO) << "remove successful";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
    accessibilityAbilityTypes[2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = 10000;
    accessibilityCapabilities[0] = "retrieve";
    accessibilityCapabilities[1] = "touchGuide";
    accessibilityCapabilities[2] = "keyEventObserver";
    accessibilityCapabilities[3] = "gesture";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.android.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    writefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

void aamsAccessibleAbilityChannelTest::CreateAccessibilityConfigNoCapability() {

    std::ostringstream os;
    Json::Value object1, targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (remove("/system/app/dummy_accessibility_ability_config.json") == 0) {
        GTEST_LOG_(INFO) << "remove successful";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
    accessibilityAbilityTypes[2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = 10000;
    accessibilityCapabilities[0] = "keyEventObserver";
    accessibilityCapabilities[1] = "touchGuide";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.android.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    writefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

void aamsAccessibleAbilityChannelTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest AddAccessibleAbilityConnection";
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
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    aastub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(*elementName_, aastub_, 0);
    accountData_->AddInstalledAbility(*abilityInfo);
}

void aamsAccessibleAbilityChannelTest::AddAccessibilityInteractionConnection()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest AddAccessibilityInteractionConnection";
    // accessibility interaction connection
    int windowId = 0;
    sptr<IAccessibilityInteractionOperation> operation = nullptr;
    int accountId = 0;
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> proxy = new AccessibilityInteractionOperationProxy(stub);
    GTEST_LOG_(INFO) << "aams RegisterInteractionOperation";
    aams_->RegisterInteractionOperation(windowId, proxy, accountId);
}

void aamsAccessibleAbilityChannelTest::CreateGesturePath(
    GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime) {                           
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest CreateGesturePath";
    // create gesture
    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    gestureSteps_.push_back(gesturePathDefine);
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_DisableAbility_001
 * @tc.name: DisableAbility
 * @tc.desc: Check disable ability after accessibleAbility connection success.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_DisableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_DisableAbility_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // accessibleAbility connect
    AddAccessibleAbilityConnection();
    // get accountData
    EXPECT_EQ(1, int(accountData_->GetConnectedA11yAbilities().size()));
    ASSERT_TRUE(testStub);
    // disable ability
    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    EXPECT_EQ(0, int(accountData_->GetConnectedA11yAbilities().size()));
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_DisableAbility_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    bool result = testStub->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, testChannelElementId);
    EXPECT_EQ(0, testChannelrequestId);
    EXPECT_EQ(0, testChannelmode);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    bool result = testStub->SearchElementInfoByAccessibilityId(ACTIVE_WINDOW_ID, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_EQ(-1, testChannelmode);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    string text = "text";
    bool result = testStub->SearchElementInfosByText(0, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, testChannelElementId);
    EXPECT_EQ(0, testChannelrequestId);
    EXPECT_EQ(text, testText);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    string text = "text";
    bool result = testStub->SearchElementInfosByText(ACTIVE_WINDOW_ID, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(text, testText);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = testStub->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, testChannelElementId);
    EXPECT_EQ(0, testChannelrequestId);
    EXPECT_EQ(focusType, testFocusType);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = testStub->FindFocusedElementInfo(ACTIVE_WINDOW_ID, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(focusType, testFocusType);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: FocusType is FOCUS_TYPE_ACCESSIBILITY, check FindFocusedElementInfo.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_ACCESSIBILITY;
    bool result = testStub->FindFocusedElementInfo(0, 0, focusType, 1, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, testChannelElementId);
    EXPECT_EQ(1, testChannelrequestId);
    EXPECT_EQ(focusType, testFocusType);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int direction = FocusMoveDirection::UP;
    bool result = testStub->FocusMoveSearch(0, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, testChannelElementId);
    EXPECT_EQ(0, testChannelrequestId);
    EXPECT_EQ(direction, testDirection);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int direction = FocusMoveDirection::UP;
    bool result = testStub->FocusMoveSearch(ACTIVE_WINDOW_ID, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(direction, testDirection);
    EXPECT_FALSE(result);
         
    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_PerformAction_001
 * @tc.name: PerformAction
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check PerformAction.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_PerformAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(testStub);
    bool result = testStub->PerformAction(0, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(4, testChannelElementId);
    EXPECT_EQ(0, testChannelrequestId);
    EXPECT_EQ(3, testAction);
    EXPECT_EQ(actionArguments, testActionArguments);
    EXPECT_TRUE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_PerformAction_002
 * @tc.name: PerformAction
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check PerformAction.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_PerformAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(testStub);
    bool result = testStub->PerformAction(ACTIVE_WINDOW_ID, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_EQ(0, testAction);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Add interaction connection check get windows.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    auto result = testStub->GetWindows();

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(1, int(result.size()));

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Not add interaction connection check get windows.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // not add interaction connection,add accessibleAbility connection
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    auto result = testStub->GetWindows();

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, int(result.size()));

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Add connection check send simulate gesture.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 start";
    // create json
    CreateAccessibilityConfigForTouchGuide();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    GesturePathPositionDefine startpoint(10.0f , 10.0f);
    GesturePathPositionDefine endpoint(10.0f , 10.0f);
    CreateGesturePath(startpoint, endpoint, 100);

    ASSERT_TRUE(testStub);
    testStub->SendSimulateGesture(1, gestureSteps_);
    sleep(2);

    int result = aams_->GetTouchEventInjector()->GetSequence();
    EXPECT_EQ(1, result);
    gestureSteps_.clear();
    testStub->DisableAbility(); 
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 end";
}

/***********************************************************************************************************/

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: No retrieve capability, check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    bool result = testStub->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_EQ(-1, testChannelmode);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: No retrieve capability, check SearchElementInfosByText.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    string text = "text";
    bool result = testStub->SearchElementInfosByText(0, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(text, testText);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: No retrieve capability, check FindFocusedElementInfo.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = testStub->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(focusType, testFocusType);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: No retrieve capability, check FocusMoveSearch.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    int direction = FocusMoveDirection::UP;
    bool result = testStub->FocusMoveSearch(0, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_NE(direction, testDirection);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_PerformAction_NoCapability_001
 * @tc.name: PerformAction
 * @tc.desc: No retrieve capability, check PerformAction.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_PerformAction_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(testStub);
    bool result = testStub->PerformAction(0, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, testChannelElementId);
    EXPECT_EQ(-1, testChannelrequestId);
    EXPECT_EQ(0, testAction);
    EXPECT_NE(actionArguments, testActionArguments);
    EXPECT_FALSE(result);

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_PerformAction_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001
 * @tc.name: GetWindows
 * @tc.desc: No retrieve capability, check get windows.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(testStub);
    auto result = testStub->GetWindows();

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, int(result.size()));

    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: No gesture capability, check send simulate gesture.
 */
HWTEST_F(aamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 start";
    // create json
    CreateAccessibilityConfigNoCapability();
    // add connection
    AddAccessibilityInteractionConnection();
    AddAccessibleAbilityConnection();

    GesturePathPositionDefine startpoint(10.0f , 10.0f);
    GesturePathPositionDefine endpoint(10.0f , 10.0f);
    CreateGesturePath(startpoint, endpoint, 100);

    ASSERT_TRUE(testStub);
    testStub->SendSimulateGesture(1, gestureSteps_);

    EXPECT_EQ(1, testGestureSimulate);

    gestureSteps_.clear();
    GTEST_LOG_(INFO) << "clear end";
    testStub->DisableAbility();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 end";
}

} // namespace Accessibility
} // namespace OHOS