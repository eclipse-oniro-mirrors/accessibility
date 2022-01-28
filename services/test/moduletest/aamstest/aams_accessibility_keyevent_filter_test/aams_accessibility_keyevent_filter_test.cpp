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
#include <map>
#include <memory>
#include <gtest/gtest.h>
#include "accessibility_input_filter.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_client_stub_impl.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"
#include "accessibility_account_data.h"
#include "accessible_ability_connection.h"
#include "json.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

extern int testKeyPressEvent;
extern int MTkeyCode;

namespace OHOS {
namespace Accessibility {

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    auto handler = OHOS::DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->GetMainHandler();
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            if (count >= maxRetryCount) {
                break;
            }
            usleep(sleepTime);
        }
    }
}

class aamsKeyEventFilterTest : public testing::Test {
public:
    aamsKeyEventFilterTest()
    {}
    ~aamsKeyEventFilterTest()
    {}

    sptr<AccessibilityInputFilter> inputFilter_ = nullptr;

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void AddConnection();
    void TearDown();
    void CreateAccessibilityConfigForKeyEvent();
    void writefileAll(const char* fname,const char* data);
    KeyEvent CreateOnKeyEvent(int keycode);

};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;

void aamsKeyEventFilterTest::SetUpTestCase(void)
{}
void aamsKeyEventFilterTest::TearDownTestCase(void)
{}
void aamsKeyEventFilterTest::SetUp()
{
    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest ModuleTest SetUp";

    CreateAccessibilityConfigForKeyEvent();
    //注册bundleservice
    auto bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject, saExtraProp);

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("KeyEventFilterModuleTest");
    sptr<AccessibleAbilityClientStubImpl> stub =
        new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    shared_ptr<AccessibleAbilityManagerService> aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    aams->RegisterAbilityConnectionClientTmp(stub);

    inputFilter_ = aams->GetInputFilter();
    if (!inputFilter_) {
        GTEST_LOG_(INFO) << "aamsKeyEventFilterTest InputFilter is nullptr";
        return;
    }
}

void aamsKeyEventFilterTest::TearDown()
{
    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest ModuleTest TearDown";
    inputFilter_ = nullptr;
}

void aamsKeyEventFilterTest::writefileAll(const char* fname,const char* data)
{
	FILE *fp;
	if ((fp=fopen(fname, "w")) == NULL)
	{
		printf("open file %s fail \n", fname);
	}
	
	fprintf(fp, "%s", data);
	fclose(fp);
}

void aamsKeyEventFilterTest::CreateAccessibilityConfigForKeyEvent() {

    std::ostringstream os;
	Json::Value	object1, targetBundleNames, 
    accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
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
    accessibilityCapabilities[0] = "keyEventObserver";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
	object1["description"] = "$string:accessibility_service_description";
	object1["settingsAbility"] = "com.example.android.accessibility.ServiceSettingsAbility";
 
    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();

	writefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

KeyEvent aamsKeyEventFilterTest::CreateOnKeyEvent(int keycode) {
    KeyEvent keyEvent{};

    struct MultimodalProperty multimodal = {
        .sourceType = MMI::SourceDevice::KEYBOARD,
    };

    struct KeyProperty property{
        .isPressed = true,
        .keyCode = keycode,
    };

    keyEvent.Initialize(multimodal, property);
    return keyEvent;
}

/**
 * @tc.number: OnKeyEvent001
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility responds the keyevent within 500 ms.(handled is true)
 */
HWTEST_F(aamsKeyEventFilterTest, aamsKeyEventFilterTest_Moduletest_OnKeyEvent001, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent001 start";

    KeyEvent keyEvent = CreateOnKeyEvent(1);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

    inputFilter_->OnInputEvent(keyEvent);

    bool handled = true;
    int sequence = 1;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    WaitUntilTaskFinished();

    EXPECT_EQ(testKeyPressEvent, 1);
    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent001 end";
}

/**
 * @tc.number: OnKeyEvent002
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility responds the keyevent within 500 ms.(handled is false)
 */
HWTEST_F(aamsKeyEventFilterTest, aamsKeyEventFilterTest_Moduletest_OnKeyEvent002, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent002 start";

    KeyEvent keyEvent = CreateOnKeyEvent(2);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

    inputFilter_->OnInputEvent(keyEvent);

    bool handled = false;
    int sequence = 2;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    sleep(1);
    WaitUntilTaskFinished();

    EXPECT_EQ(testKeyPressEvent, 2);
    EXPECT_EQ(MTkeyCode, 2);
    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent002 end";
}

/**
 * @tc.number: OnKeyEvent003
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility doesn't respond the keyevent within 500 ms.
 */
HWTEST_F(aamsKeyEventFilterTest, aamsKeyEventFilterTest_Moduletest_OnKeyEvent003, TestSize.Level1)
{

    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent003 start";

    KeyEvent keyEvent = CreateOnKeyEvent(3);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

    inputFilter_->OnInputEvent(keyEvent);
    sleep(1);
    WaitUntilTaskFinished();
    
    EXPECT_EQ(testKeyPressEvent, 3);
    EXPECT_EQ(MTkeyCode, 3);
    GTEST_LOG_(INFO) << "aamsKeyEventFilterTest_Moduletest_OnKeyEvent003 end";
}

}  // namespace Accessibility
}  // namespace OHOS