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

#include "accessibility_display_manager.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"
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
extern int g_testChannelElementId;
extern int g_testChannelrequestId;
extern int g_testChannelmode;
extern std::string g_testText;
extern int g_testFocusType;
extern int g_testDirection;
extern int g_testAction;
extern int g_testGestureSimulate;
extern std::map<std::string, std::string> g_testActionArguments;

const int timeout = 10000;
const int testNum_2 = 2;
const int testNum_3 = 3;
class AamsAccessibleAbilityChannelTest : public testing::Test {
public:

    AamsAccessibleAbilityChannelTest() {}
    ~AamsAccessibleAbilityChannelTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void WritefileAll(const char* fname, const char* data);
    void CreateAccessibilityConfigForTouchGuide();
    void CreateAccessibilityConfigNoCapability();
    void AddAccessibleAbilityConnection();
    void AddAccessibilityWindowConnection();
    void CreateGesturePath(
        GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int64_t durationTime);
    std::vector<GesturePathDefine> gestureSteps_ {};
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> aams_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

void AamsAccessibleAbilityChannelTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest SetUpTestCase";
}

void AamsAccessibleAbilityChannelTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDownTestCase";
}

void AamsAccessibleAbilityChannelTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest SetUp";
    // Register bundleservice
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

void AamsAccessibleAbilityChannelTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDown";
    // Reset test value
    g_testChannelElementId = -1;
    g_testChannelrequestId = -1;
    g_testChannelmode = -1;
    g_testAction = 0;
    g_testDirection = 0;
    g_testText = "";
    g_testFocusType = 0;
    g_testGestureSimulate = -1;
    g_testActionArguments.clear();
    // Deregister ElementOperator
    aams_->DeregisterElementOperator(0);
    // Stop AAMS
    aams_->OnStop();
    mock_ = nullptr;
    aams_ = nullptr;
    accountData_ = nullptr;
    aastub_ = nullptr;
    elementName_ = nullptr;
    AAConnection_ = nullptr;
}

void AamsAccessibleAbilityChannelTest::WritefileAll(const char* fname, const char* data)
{
    FILE *fp;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsAccessibleAbilityChannelTest::CreateAccessibilityConfigForTouchGuide()
{
    std::ostringstream os;
    Json::Value object1,
        targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (!remove("/system/app/dummy_accessibility_ability_config.json")) {
        GTEST_LOG_(INFO) << "remove successful";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
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

void AamsAccessibleAbilityChannelTest::CreateAccessibilityConfigNoCapability()
{
    std::ostringstream os;
    Json::Value object1,
        targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (!remove("/system/app/dummy_accessibility_ability_config.json")) {
        GTEST_LOG_(INFO) << "remove successful";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
    accessibilityAbilityTypes[testNum_2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = timeout;
    accessibilityCapabilities[0] = "keyEventObserver";
    accessibilityCapabilities[1] = "touchGuide";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.ohos.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    WritefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

void AamsAccessibleAbilityChannelTest::AddAccessibleAbilityConnection()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest AddAccessibleAbilityConnection";
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

void AamsAccessibleAbilityChannelTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest AddAccessibilityWindowConnection";
    int windowId = 0;
    sptr<IAccessibilityElementOperator> operation = nullptr;
    int accountId = 0;
    sptr<AccessibilityElementOperatorStub> stub = new AccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    GTEST_LOG_(INFO) << "aams RegisterElementOperator";
    aams_->RegisterElementOperator(windowId, proxy, accountId);
}

void AamsAccessibleAbilityChannelTest::CreateGesturePath(
    GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int64_t durationTime)
{
    GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest CreateGesturePath";
    // Create gesture
    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    gestureSteps_.push_back(gesturePathDefine);
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId,
 *           check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, g_testChannelElementId);
    EXPECT_EQ(0, g_testChannelrequestId);
    EXPECT_EQ(0, g_testChannelmode);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId,
 *           check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->SearchElementInfoByAccessibilityId(ACTIVE_WINDOW_ID, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_EQ(-1, g_testChannelmode);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    string text = "text";
    bool result = g_testStub->SearchElementInfosByText(0, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, g_testChannelElementId);
    EXPECT_EQ(0, g_testChannelrequestId);
    EXPECT_EQ(text, g_testText);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002
 * @tc.name: SearchElementInfosByText
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check SearchElementInfosByText.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    string text = "text";
    bool result = g_testStub->SearchElementInfosByText(ACTIVE_WINDOW_ID, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(text, g_testText);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = g_testStub->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, g_testChannelElementId);
    EXPECT_EQ(0, g_testChannelrequestId);
    EXPECT_EQ(focusType, g_testFocusType);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FindFocusedElementInfo.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = g_testStub->FindFocusedElementInfo(ACTIVE_WINDOW_ID, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(focusType, g_testFocusType);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: FocusType is FOCUS_TYPE_ACCESSIBILITY, check FindFocusedElementInfo.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_ACCESSIBILITY;
    bool result = g_testStub->FindFocusedElementInfo(0, 0, focusType, 1, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, g_testChannelElementId);
    EXPECT_EQ(1, g_testChannelrequestId);
    EXPECT_EQ(focusType, g_testFocusType);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int direction = FocusMoveDirection::UP;
    bool result = g_testStub->FocusMoveSearch(0, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, g_testChannelElementId);
    EXPECT_EQ(0, g_testChannelrequestId);
    EXPECT_EQ(direction, g_testDirection);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002
 * @tc.name: FocusMoveSearch
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int direction = FocusMoveDirection::UP;
    bool result = g_testStub->FocusMoveSearch(ACTIVE_WINDOW_ID, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(direction, g_testDirection);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: AccessibilityWindowId is accessibility interaction connection windowId, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->ExecuteAction(0, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(4, g_testChannelElementId);
    EXPECT_EQ(0, g_testChannelrequestId);
    EXPECT_EQ(3, g_testAction);
    EXPECT_EQ(actionArguments, g_testActionArguments);
    EXPECT_TRUE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: AccessibilityWindowId not accessibility interaction connection windowId, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->ExecuteAction(ACTIVE_WINDOW_ID, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_EQ(0, g_testAction);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Not add interaction connection check get windows.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Not add interaction connection,add accessibleAbility connection
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    auto result = g_testStub->GetWindows();

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, int(result.size()));

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Add connection check send simulate gesture.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest, AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 start";
    // Create json
    CreateAccessibilityConfigForTouchGuide();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    GesturePathPositionDefine startpoint(10.0f, 10.0f);
    GesturePathPositionDefine endpoint(10.0f, 10.0f);
    CreateGesturePath(startpoint, endpoint, 100);

    ASSERT_TRUE(g_testStub);
    g_testStub->SendSimulateGesture(1, gestureSteps_);
    sleep(2);

    int result = aams_->GetTouchEventInjector()->GetSequence();
    EXPECT_EQ(1, result);
    gestureSteps_.clear();
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_001 end";
}

/***********************************************************************************************************/

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: No retrieve capability, check SearchElementInfoByAccessibilityId.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->SearchElementInfoByAccessibilityId(0, 0, 0, nullptr, 0);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_EQ(-1, g_testChannelmode);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfoByAccessibilityId_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: No retrieve capability, check SearchElementInfosByText.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    string text = "text";
    bool result = g_testStub->SearchElementInfosByText(0, 0, text, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(text, g_testText);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SearchElementInfosByText_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: No retrieve capability, check FindFocusedElementInfo.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int focusType = OHOS::Accessibility::FOCUS_TYPE_INPUT;
    bool result = g_testStub->FindFocusedElementInfo(0, 0, focusType, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(focusType, g_testFocusType);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FindFocusedElementInfo_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: No retrieve capability, check FocusMoveSearch.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    int direction = FocusMoveDirection::UP;
    bool result = g_testStub->FocusMoveSearch(0, 0, direction, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_NE(direction, g_testDirection);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_FocusMoveSearch_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001
 * @tc.name: ExecuteAction
 * @tc.desc: No retrieve capability, check ExecuteAction.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    std::map<std::string, std::string> actionArguments;
    actionArguments.insert(make_pair("invalid", "invalid"));

    ASSERT_TRUE(g_testStub);
    bool result = g_testStub->ExecuteAction(0, 4, 3, actionArguments, 0, nullptr);

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(-1, g_testChannelElementId);
    EXPECT_EQ(-1, g_testChannelrequestId);
    EXPECT_EQ(0, g_testAction);
    EXPECT_NE(actionArguments, g_testActionArguments);
    EXPECT_FALSE(result);

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_ExecuteAction_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001
 * @tc.name: GetWindows
 * @tc.desc: No retrieve capability, check get windows.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    ASSERT_TRUE(g_testStub);
    auto result = g_testStub->GetWindows();

    GTEST_LOG_(INFO) << "Test result";
    EXPECT_EQ(0, int(result.size()));

    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_GetWindows_NoCapability_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: No gesture capability, check send simulate gesture.
 */
HWTEST_F(AamsAccessibleAbilityChannelTest,
    AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 start";
    // Create json
    CreateAccessibilityConfigNoCapability();
    // Add connection
    AddAccessibilityWindowConnection();
    AddAccessibleAbilityConnection();

    GesturePathPositionDefine startpoint(10.0f, 10.0f);
    GesturePathPositionDefine endpoint(10.0f, 10.0f);
    CreateGesturePath(startpoint, endpoint, 100);

    ASSERT_TRUE(g_testStub);
    g_testStub->SendSimulateGesture(1, gestureSteps_);

    EXPECT_EQ(1, g_testGestureSimulate);

    gestureSteps_.clear();
    GTEST_LOG_(INFO) << "clear end";
    AAConnection_->OnAbilityDisconnectDone(*elementName_, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_ModuleTest_SendSimulateGesture_NoCapability_001 end";
}
} // namespace Accessibility
} // namespace OHOS