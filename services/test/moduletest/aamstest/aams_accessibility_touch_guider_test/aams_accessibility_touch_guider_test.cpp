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

#include <unistd.h>
#include <gtest/gtest.h>
#include "accessibility_input_filter.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessibility_display_manager.h"
#include "accessibility_touch_guider.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"
#include "accessibility_account_data.h"
#include "accessible_ability_connection.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "json.h"

using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace Json;

namespace OHOS {
namespace Accessibility {
extern std::vector<EventType> mTeventType;
extern std::vector<int> mtTouchAction;
extern int MTgestureId;

class aamsTouchGuideTest : public testing::Test {
public:
    aamsTouchGuideTest()
    {}
    ~aamsTouchGuideTest()
    {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
protected:
    TouchEvent CreateMoveEvent(int pointerCount, int pointId);
    TouchEvent CreateTouchEvent(int action, int pointId);
    TouchEvent CreateTouchEvent(int action, MmiPoint &point, int pointerCount, 
                            unsigned int occurredTime, unsigned int startTime, int pointId);

    sptr<AccessibilityInputFilter> inputFilter_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
    shared_ptr<AccessibleAbilityManagerService> ins_ = nullptr;
    void CreateAccessibilityConfigForTouchGuide();
    void writefileAll(const char* fname,const char* data);
    void AddAccessibilityInteractionConnection();
};

void aamsTouchGuideTest::writefileAll(const char* fname,const char* data)
{
	FILE *fp;
	if ((fp=fopen(fname, "w")) == NULL) {
		printf("open file %s fail \n", fname);
	}
	
	fprintf(fp, "%s", data);
	fclose(fp);
}

void aamsTouchGuideTest::CreateAccessibilityConfigForTouchGuide() {

    std::ostringstream os;
	Json::Value	object1, targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (remove("/system/app/dummy_accessibility_ability_config.json") == 0) {
        GTEST_LOG_(INFO) << "remove successful";
    } else{
        GTEST_LOG_(INFO) << "remove failed";
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
    accessibilityCapabilities[0] = "touchGuide";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
	object1["description"] = "$string:accessibility_service_description";
	object1["settingsAbility"] = "com.example.android.accessibility.ServiceSettingsAbility";
 
    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    // printf("json:%s\n",jsonStr.c_str());
	writefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}
void aamsTouchGuideTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest SetUpTestCase";
}
void aamsTouchGuideTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest TearDownTestCase";

}
void aamsTouchGuideTest::SetUp()
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest SetUp";
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

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    stub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    ins_->RegisterAbilityConnectionClientTmp(stub_);
    AddAccessibilityInteractionConnection();

    inputFilter_ = ins_->GetInputFilter();
    if (!inputFilter_) {
        GTEST_LOG_(INFO) << "aamsTouchGuideTest InputFilter is nullptr";
        return;
    }

}

void aamsTouchGuideTest::TearDown()
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest TearDown";

    inputFilter_ = nullptr;
    ins_->DeregisterInteractionOperation(0);
    sleep(2);
    ins_.reset();
    mTeventType.clear();
    mtTouchAction.clear();
}

TouchEvent aamsTouchGuideTest::CreateTouchEvent(int action, int pointId) 
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {};
    struct ManipulationProperty manipulationProperty = {
        .pointerCount = 1,
        .pointerId = pointId,
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

TouchEvent aamsTouchGuideTest::CreateMoveEvent(int pointerCount, int pointId) 
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {};
    struct ManipulationProperty manipulationProperty = {
        .pointerCount = pointerCount,
        .pointerId = pointId,
    };
    struct TouchProperty touch = {
        .action = TouchEnum::POINT_MOVE,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

TouchEvent aamsTouchGuideTest::CreateTouchEvent(int action, MmiPoint &point, int pointerCount,
                            unsigned int occurredTime, unsigned int startTime, int pointId) 
{
    TouchEvent touchEvent;
    struct MultimodalProperty multimodal = {
                .sourceType = MMI::SourceDevice::TOUCH_PANEL,
        .occurredTime = occurredTime,
    };
    struct ManipulationProperty manipulationProperty = {
        .mp = point,
        .startTime = startTime,
        .pointerCount = pointerCount,
        .pointerId = pointId,
    };
    struct TouchProperty touch = {
        .action = action,
        .index = 0,
    };
    touchEvent.Initialize(multimodal, manipulationProperty, touch);
    return touchEvent;
}

void aamsTouchGuideTest::AddAccessibilityInteractionConnection()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest AddAccessibilityInteractionConnection";
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
 * @tc.number: OnTouchEvent001
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that two fingers moving in sanme directions in draging state.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent001 starts";

    MmiPoint point1(500, 500);
    MmiPoint point2(1000, 500);
    MmiPoint point3(2500, 500);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, point1, 1, 0, 0, 1);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 2, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 2, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_UP, point3, 1, 0, 0, 1);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::PRIMARY_POINT_UP);
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent001 ENDs";
}

/**
 * @tc.number: OnTouchEvent002
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent002 starts";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, 0);
    inputFilter_->OnInputEvent(event);
    sleep(1);
    event = CreateMoveEvent(1, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::HOVER_POINTER_MOVE);
    EXPECT_EQ(mtTouchAction[3], TouchEnum::HOVER_POINTER_EXIT);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent002 ENDs";
}

#if 0
/**
 * @tc.number: OnTouchEvent003
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that two fingers moving in directions.(The parameter index has no effect)
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent003 starts";

    MmiPoint point1(0, 0);
    MmiPoint point2(500, 500);
    MmiPoint point3(750, 250);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, point2, 1, 0, 0, 1);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 2, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_UP, 1);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::OTHER_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::OTHER_POINT_UP);
    EXPECT_EQ(mtTouchAction[3], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent003 ENDs";
}
#endif

/**
 * @tc.number: OnTouchEvent004
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the LEFT_THEN_RIGHT gesture.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent004 starts";

    MmiPoint point1(2500, 2500);
    MmiPoint point2(1500, 2500);
    MmiPoint point3(0, 2500);
    MmiPoint point4(2500, 2250);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_LEFT_THEN_RIGHT));
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent004 ENDs";
}

/**
 * @tc.number: OnTouchEvent005
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the UP_THEN_DOWN gesture.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent005 starts";

    MmiPoint point1(2500, 2500);
    MmiPoint point2(2500, 3500);
    MmiPoint point3(2500, 5000);
    MmiPoint point4(2250, 2500);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_UP_THEN_DOWN));
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent005 ENDs";
}

/**
 * @tc.number: OnTouchEvent006
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the RIGHT_THEN_LEFT gesture.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent006 starts";

    MmiPoint point1(2500, 2500);
    MmiPoint point2(3500, 2500);
    MmiPoint point3(5000, 2500);
    MmiPoint point4(2500, 2250);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_RIGHT_THEN_LEFT));
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent006 ENDs";
}

/**
 * @tc.number: OnTouchEvent007
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the DOWN_THEN_UP gesture.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent007 starts";

    MmiPoint point1(2500, 2500);
    MmiPoint point2(2500, 1500);
    MmiPoint point3(2500, 0);
    MmiPoint point4(2250, 2500);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point4, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_DOWN_THEN_UP));
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent007 ENDs";
}

/**
 * @tc.number: OnTouchEvent008
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the DOWN gesture.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent008 starts";

    MmiPoint point1(2500, 2500);
    MmiPoint point2(2500, 1500);
    MmiPoint point3(2500, 0);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point1, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point2, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::POINT_MOVE, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, point3, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::POINT_MOVE);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_GESTURE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(MTgestureId, static_cast<int>(GestureTypes::GESTURE_SWIPE_DOWN));
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent008 ENDs";
}

/**
 * @tc.number: OnTouchEvent008
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the single tap event.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent009 starts";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, 0);
    inputFilter_->OnInputEvent(event);   
    sleep(3);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_GUIDE_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::HOVER_POINTER_ENTER);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::HOVER_POINTER_EXIT);
    EXPECT_EQ(mTeventType[4], EventType::TYPE_TOUCH_GUIDE_END);
    EXPECT_EQ(mTeventType[5], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent009 ends";
}

/**
 * @tc.number: OnTouchEvent010
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the double tap and long press event.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent010 starts";

    MmiPoint point(300, 500);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 40, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 200, 0, 0);
    inputFilter_->OnInputEvent(event);
    sleep(1);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);
    sleep(3); 

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);    
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent010 ends";
}

/**
 * @tc.number: OnTouchEvent011
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the double-tap event.
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent011 starts";

    MmiPoint point(300, 500);

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, point,1, 100, 0, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP,point, 1, 0, 0, 0);
    inputFilter_->OnInputEvent(event);

    sleep(3);
    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::PRIMARY_POINT_UP);

    GTEST_LOG_(INFO) << "aamsTouchGuideTest aamsTouchGuideTest_Moduletest_OnTouchEvent011 ends";
}

# if 0
/**
 * @tc.name: OnTouchEvent012
 * @tc.name:OnTouchEvent
 * @tc.desc: Check the event that multi-finger moving in draging state.(The parameter index has no effect)
 */
HWTEST_F(aamsTouchGuideTest, aamsTouchGuideTest_Moduletest_OnTouchEvent012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << " aamsTouchGuideTest_Moduletest_OnTouchEvent012 starts";

    TouchEvent event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_DOWN, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::OTHER_POINT_DOWN, 0);
    inputFilter_->OnInputEvent(event);

    event = CreateMoveEvent(3);
    inputFilter_->OnInputEvent(event);

    event = CreateTouchEvent(TouchEnum::PRIMARY_POINT_UP, 0);
    inputFilter_->OnInputEvent(event);  
    sleep(3);

    EXPECT_EQ(mTeventType[2], EventType::TYPE_TOUCH_BEGIN);
    EXPECT_EQ(mtTouchAction[0], TouchEnum::PRIMARY_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[1], TouchEnum::OTHER_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[2], TouchEnum::OTHER_POINT_DOWN);
    EXPECT_EQ(mtTouchAction[3], TouchEnum::PRIMARY_POINT_UP);
    EXPECT_EQ(mTeventType[3], EventType::TYPE_TOUCH_END);

    GTEST_LOG_(INFO) << " aamsTouchGuideTest_Moduletest_OnTouchEvent012 ends";
}
#endif   

}  // namespace Accessibility
}  // namespace OHO