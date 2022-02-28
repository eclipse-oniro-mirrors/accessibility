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

#include "accessibility_display_manager.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_touch_guider.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_client_proxy.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "json.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
extern std::vector<int32_t> g_mtTouchAction;
namespace Accessibility {
extern int g_mTgestureId;
extern std::vector<EventType> g_mTeventType;
const int timeout = 10000;
const int sleepTime = 2;
const int testNum_2 = 2;
class AamsInjectorTest : public testing::Test {
public:
    AamsInjectorTest() {}
    ~AamsInjectorTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;
    void CreateGesturePath(
        GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime);
    std::vector<GesturePathDefine> getGesturePath {};
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
    void CreateAccessibilityConfigForTouchGuide();
    void WritefileAll(const char* fname, const char* data);
    void AddAccessibilityWindowConnection();
};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> g_ins;

void AamsInjectorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUpTestCase";
}

void AamsInjectorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest TearDownTestCase";
}

void AamsInjectorTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUp";
    CreateAccessibilityConfigForTouchGuide();
    // 注册bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    g_ins = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    g_ins->OnStart();

    // accessibleAbility connection
    sptr<AccessibleAbilityClientStubImpl> accessibleAbility = new AccessibleAbilityClientStubImpl();

    // add an ability connection client
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(extensionInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    sptr<AccessibilityAccountData> accountData = g_ins->GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    connection->OnAbilityConnectDone(elementName, accessibleAbility, 0);

    AddAccessibilityWindowConnection();

    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
                                                        g_ins->GetCurrentAccountData()->GetConnectedA11yAbilities();
    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);
    GTEST_LOG_(INFO) << "AamsInjectorTest SetUp end";

    AccessibilityWindowInfoManager::GetInstance().SetInputFocusedWindow(0);
}

void AamsInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    inputInterceptor_ = nullptr;
    aacs_ = nullptr;
    mock_ = nullptr;
    getGesturePath.clear();
    g_ins->DeregisterElementOperator(0);
    sleep(sleepTime);
    g_mtTouchAction.clear();
    g_mTeventType.clear();
}

void AamsInjectorTest::CreateGesturePath(
    GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime)
{
    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    getGesturePath.push_back(gesturePathDefine);
}

void AamsInjectorTest::WritefileAll(const char* fname, const char* data)
{
    FILE *fp;
    if ((fp = fopen(fname, "w")) == nullptr) {
        printf("open file %s fail \n", fname);
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsInjectorTest::CreateAccessibilityConfigForTouchGuide()
{
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
    accessibilityAbilityTypes[testNum_2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = timeout;
    accessibilityCapabilities[0] = "gesture";
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

void AamsInjectorTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "AamsInjectorTest AddAccessibilityWindowConnection";
    // accessibility interaction connection
    int windowId = 0;
    sptr<IAccessibilityElementOperator> operation = nullptr;
    int accountId = 0;
    sptr<AccessibilityElementOperatorStub> stub = new AccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    GTEST_LOG_(INFO) << "aams  RegisterElementOperator";
    g_ins->RegisterElementOperator(windowId, proxy, accountId);
}

/**
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected single-tap event can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint(500.0f, 500.0f);
    GesturePathPositionDefine endpoint(500.0f, 500.0f);

    CreateGesturePath(startpoint, endpoint, 100);
    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 end";
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(2500.0f, 2500.0f);
    GesturePathPositionDefine endpoint1(1000.0f, 2500.0f);
    GesturePathPositionDefine startpoint2(1000.0f, 2500.0f);
    GesturePathPositionDefine endpoint2(0.0f, 2500.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(g_mTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(2500.0f, 2500.0f);
    GesturePathPositionDefine endpoint1(3500.0f, 2500.0f);
    GesturePathPositionDefine startpoint2(3500.0f, 2500.0f);
    GesturePathPositionDefine endpoint2(5000.0f, 2500.0f);
    GesturePathPositionDefine startpoint3(5000.0f, 2500.0f);
    GesturePathPositionDefine endpoint3(4000.0f, 0.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);
    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);
    EXPECT_EQ(g_mtTouchAction[2], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(g_mTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(2500.0f, 2500.0f);
    GesturePathPositionDefine endpoint1(1500.0f, 2500.0f);
    GesturePathPositionDefine startpoint2(1500.0f, 2500.0f);
    GesturePathPositionDefine endpoint2(0.0f, 2500.0f);
    GesturePathPositionDefine startpoint3(0.0f, 2500.0f);
    GesturePathPositionDefine endpoint3(1000.0f, 0.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);
    EXPECT_EQ(g_mtTouchAction[2], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(g_mTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 end";
}

/**
 * @tc.number: TouchEventInjector007
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_007 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(2500.0f, 2500.0f);
    GesturePathPositionDefine endpoint1(2500.0f, 3500.0f);
    GesturePathPositionDefine startpoint2(2500.0f, 3500.0f);
    GesturePathPositionDefine endpoint2(2500.0f, 5000.0f);
    GesturePathPositionDefine startpoint3(2500.0f, 5000.0f);
    GesturePathPositionDefine endpoint3(0.0f, 4000.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);
    EXPECT_EQ(g_mtTouchAction[2], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(g_mTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_UP_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_007 end";
}

/**
 * @tc.number: TouchEventInjector008
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_008 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(2500.0f, 2500.0f);
    GesturePathPositionDefine endpoint1(2500.0f, 3500.0f);
    GesturePathPositionDefine startpoint2(2500.0f, 3500.0f);
    GesturePathPositionDefine endpoint2(2500.0f, 5000.0f);
    GesturePathPositionDefine startpoint3(2500.0f, 5000.0f);
    GesturePathPositionDefine endpoint3(5000.0f, 4000.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);
    EXPECT_EQ(g_mtTouchAction[1], expectValue);
    EXPECT_EQ(g_mtTouchAction[2], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(g_mTgestureId, static_cast<int>(GestureType::GESTURE_SWIPE_UP_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_008 end";
}

/**
 * @tc.number: TouchEventInjector009
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected cancel-move event after onstart can be recognized in touchGuide.
 */
HWTEST_F(AamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_009 start";

    g_mTeventType = {};
    g_mtTouchAction = {};
    GesturePathPositionDefine startpoint1(500.0f, 500.0f);
    GesturePathPositionDefine endpoint1(2500.0f, 500.0f);
    GesturePathPositionDefine startpoint2(2500.0f, 500.0f);
    GesturePathPositionDefine endpoint2(2500.0f, 750.0f);

    CreateGesturePath (startpoint1, endpoint1, 400);
    CreateGesturePath (startpoint2, endpoint2, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    int32_t expectValue = MMI::PointerEvent::POINTER_ACTION_MOVE;
    EXPECT_EQ(g_mtTouchAction[0], expectValue);

    EXPECT_EQ(g_mTeventType[0], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(g_mTeventType[1], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(g_mTeventType[2], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(g_mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_009 end";
}
} // namespace Accessibility
} // namespace OHOS