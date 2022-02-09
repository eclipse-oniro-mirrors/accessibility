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
#include "accessibility_input_filter.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessibility_display_manager.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_touch_guider.h"
#include "accessible_ability_event_handler.h"
#include "accessible_ability_client_proxy.h"
#include "accessible_ability_connection.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "system_ability_definition.h"
#include "accessibility_window_manager.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include <stdio.h>
#include "json.h"


using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {

extern int MTgestureId;
extern std::vector<EventType> mTeventType;
extern std::vector<int> mtTouchAction;
class aamsInjectorTest : public testing::Test {
public:

    aamsInjectorTest() {}
    ~aamsInjectorTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibilityInputFilter> inputFilter_ = nullptr;
    void CreateGesturePath(
            GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime);
    std::vector<GesturePathDefine> getGesturePath;
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = nullptr;
    void CreateAccessibilityConfigForTouchGuide();
    void writefileAll(const char* fname,const char* data);
    void AddAccessibilityInteractionConnection();

};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;

void aamsInjectorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "aamsInjectorTest SetUpTestCase";

}

void aamsInjectorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "aamsInjectorTest TearDownTestCase";
}

void aamsInjectorTest::SetUp()
{
    GTEST_LOG_(INFO) << "aamsInjectorTest SetUp";
    CreateAccessibilityConfigForTouchGuide();
    //注册bundleservice
    auto bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();

    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    wmdInfo.width = 1000;
    wmdInfo.height = 1000;

    displayMgr.SetDisplay(wmdInfo);
    // accessibleAbility connection
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = AppExecFwk::EventRunner::Create("aamsAccessibleAbilityInjectorTest");
    sptr<AccessibleAbilityClientStubImpl> accessibleAbility =
                            new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner_));
    ins_->RegisterAbilityConnectionClientTmp(accessibleAbility);
    AddAccessibilityInteractionConnection();
    GTEST_LOG_(INFO) << "aams RegisterAbilityConnectionClientTmp";

    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
                                                        ins_->GetCurrentAccountData()->GetConnectedA11yAbilities();
    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);
    GTEST_LOG_(INFO) << "aamsInjectorTest SetUp end";

    AccessibilityWindowInfoManager::GetInstance().SetInputFocusedWindow(0);
}

void aamsInjectorTest::TearDown()
{
    GTEST_LOG_(INFO) << "TouchEventInjectorTest TearDown";
    inputFilter_ = nullptr;
    aacs_ = nullptr;
    getGesturePath.clear();
    ins_->DeregisterInteractionOperation(0);
    sleep(2);
    mtTouchAction.clear();
    mTeventType.clear();
}

void aamsInjectorTest::CreateGesturePath(
        GesturePathPositionDefine startpoint, GesturePathPositionDefine endpoint, int durationTime)
{
    GesturePathDefine gesturePathDefine = GesturePathDefine(startpoint, endpoint, durationTime);
    gesturePathDefine.SetStartPosition(startpoint);
    gesturePathDefine.SetEndPosition(endpoint);
    gesturePathDefine.SetDurationTime(durationTime);
    getGesturePath.push_back(gesturePathDefine);
}

void aamsInjectorTest::writefileAll(const char* fname,const char* data)
{
	FILE *fp;
	if ((fp=fopen(fname, "w")) == NULL) {
		printf("open file %s fail \n", fname);
	}
	
	fprintf(fp, "%s", data);
	fclose(fp);
}

void aamsInjectorTest::CreateAccessibilityConfigForTouchGuide()
{
    std::ostringstream os;
	Json::Value	object1, targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
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
    accessibilityCapabilities[0] = "gesture";
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

void aamsInjectorTest::AddAccessibilityInteractionConnection()
{
    GTEST_LOG_(INFO) << "aamsInjectorTest AddAccessibilityInteractionConnection";
    // accessibility interaction connection
    int windowId = 0;
    sptr<IAccessibilityInteractionOperation> operation = nullptr;
    int accountId = 0;
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> proxy = new AccessibilityInteractionOperationProxy(stub);
    GTEST_LOG_(INFO) << "aams  RegisterInteractionOperation";
    ins_->RegisterInteractionOperation(windowId, proxy, accountId);
}

/**
 * @tc.number: TouchEventInjector001
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected single-tap event can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 start";

    GesturePathPositionDefine startpoint(500.0f , 500.0f);
    GesturePathPositionDefine endpoint(500.0f , 500.0f);

    CreateGesturePath(startpoint, endpoint, 100);
    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::HOVER_POINTER_EXIT);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_001 end";
}

/**
 * @tc.number: TouchEventInjector002
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected double-tap event can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_002 start";

    GesturePathPositionDefine startpoint1(300.0f , 500.0f);
    GesturePathPositionDefine endpoint1(300.0f , 500.0f);
    GesturePathPositionDefine startpoint2(300.0f , 500.0f);
    GesturePathPositionDefine endpoint2(300.0f , 500.0f);

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 100);
    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::PRIMARY_POINT_UP);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_002 end";
}

/**
 * @tc.number: TouchEventInjector003
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected double-tap and long press event can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_003 start";

    GesturePathPositionDefine startpoint1(300.0f , 500.0f);
    GesturePathPositionDefine endpoint1(300.0f , 500.0f);
    GesturePathPositionDefine startpoint2(300.0f , 500.0f);
    GesturePathPositionDefine endpoint2(300.0f , 500.0f);

    CreateGesturePath(startpoint1, endpoint1, 100);
    CreateGesturePath(startpoint2, endpoint2, 1000);
    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::PRIMARY_POINT_UP);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_003 end";
}

/**
 * @tc.number: TouchEventInjector004
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(1000.0f , 2500.0f);
    GesturePathPositionDefine startpoint2(1000.0f , 2500.0f);
    GesturePathPositionDefine endpoint2(0.0f , 2500.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_004 end";
}

/**
 * @tc.number: TouchEventInjector005
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected RIGHT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(3500.0f , 2500.0f);
    GesturePathPositionDefine startpoint2(3500.0f , 2500.0f);
    GesturePathPositionDefine endpoint2(5000.0f , 2500.0f);
    GesturePathPositionDefine startpoint3(5000.0f , 2500.0f);
    GesturePathPositionDefine endpoint3(4000.0f , 0.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_RIGHT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_005 end";
}

/**
 * @tc.number: TouchEventInjector006
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected LEFT_THEN_DOWN gesture can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(1500.0f , 2500.0f);
    GesturePathPositionDefine startpoint2(1500.0f , 2500.0f);
    GesturePathPositionDefine endpoint2(0.0f , 2500.0f);
    GesturePathPositionDefine startpoint3(0.0f , 2500.0f);
    GesturePathPositionDefine endpoint3(1000.0f , 0.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_LEFT_THEN_DOWN));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_006 end";
}

/**
 * @tc.number: TouchEventInjector007
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_LEFT gesture can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_007 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(2500.0f , 3500.0f);
    GesturePathPositionDefine startpoint2(2500.0f , 3500.0f);
    GesturePathPositionDefine endpoint2(2500.0f , 5000.0f);
    GesturePathPositionDefine startpoint3(2500.0f , 5000.0f);
    GesturePathPositionDefine endpoint3(0.0f , 4000.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_UP_THEN_LEFT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_007 end";
}

/**
 * @tc.number: TouchEventInjector008
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected UP_THEN_RIGHT gesture can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_008 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(2500.0f , 3500.0f);
    GesturePathPositionDefine startpoint2(2500.0f , 3500.0f);
    GesturePathPositionDefine endpoint2(2500.0f , 5000.0f);
    GesturePathPositionDefine startpoint3(2500.0f , 5000.0f);
    GesturePathPositionDefine endpoint3(5000.0f , 4000.0f);

    CreateGesturePath (startpoint1, endpoint1, 100);
    CreateGesturePath (startpoint2, endpoint2, 100);
    CreateGesturePath (startpoint3, endpoint3, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_UP_THEN_RIGHT));

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_008 end";
}

/**
 * @tc.number: TouchEventInjector009
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected cancel-move event after onstart can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_009 start";

    GesturePathPositionDefine startpoint1(500.0f , 500.0f);
    GesturePathPositionDefine endpoint1(2500.0f , 500.0f);
    GesturePathPositionDefine startpoint2(2500.0f , 500.0f);
    GesturePathPositionDefine endpoint2(2500.0f , 750.0f);

    CreateGesturePath (startpoint1, endpoint1, 400);
    CreateGesturePath (startpoint2, endpoint2, 100);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_009 end";
}

/**
 * @tc.number: TouchEventInjector010
 * @tc.name:TouchEventInjector
 * @tc.desc: Check that the injected cancel-move event without onstart can be recognized in touchGuide.
 */
HWTEST_F(aamsInjectorTest, TouchEventInjector_ModuleTest_TouchEventInjector_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_010 start";

    GesturePathPositionDefine startpoint1(2500.0f , 2500.0f);
    GesturePathPositionDefine endpoint1(2350.0f , 2500.0f);

    CreateGesturePath (startpoint1, endpoint1, 300);

    aacs_->SendSimulateGesture(1, getGesturePath);
    sleep(3);

    EXPECT_EQ(mtTouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::HOVER_POINTER_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::HOVER_POINTER_EXIT);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "TouchEventInjector_ModuleTest_TouchEventInjector_010 end";
}
}  // namespace Accessibility
}  // namespace OHOS