/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <map>
#include <memory>
#include <gtest/gtest.h>
#include <unistd.h>

#include "accessibility_account_data.h"
#include "accessibility_input_interceptor.h"
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
extern int g_mTkeyCode;
extern std::shared_ptr<MMI::IInputEventConsumer> g_inputEventConsumer;
namespace Accessibility {
extern int g_testKeyPressEvent;
static constexpr uint8_t TEST_NUM_2         = 2;
static constexpr uint8_t TEST_NUM_3         = 3;
static constexpr uint16_t TEST_TIME_OUT     = 10000;
static constexpr uint16_t TEST_NUM_THOUSAND = 1000;

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = TEST_NUM_THOUSAND;
    const uint32_t sleepTime = TEST_NUM_THOUSAND;
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

class AamsKeyEventFilterTest : public testing::Test {
public:
    AamsKeyEventFilterTest()
    {}
    ~AamsKeyEventFilterTest()
    {}

    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void AddConnection();
    void TearDown();
    void CreateAccessibilityConfigForKeyEvent();
    void WritefileAll(const char* fname, const char* data) const;
    std::shared_ptr<MMI::KeyEvent> CreateOnKeyEvent(int keycode) const;
};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> g_ins;

void AamsKeyEventFilterTest::SetUpTestCase(void)
{}
void AamsKeyEventFilterTest::TearDownTestCase(void)
{}
void AamsKeyEventFilterTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest ModuleTest SetUp";

    CreateAccessibilityConfigForKeyEvent();

    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    g_ins = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    g_ins->OnStart();

    sptr<AccessibleAbilityClientStubImpl> stub = new AccessibleAbilityClientStubImpl();

    // add an ability connection client
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(extensionInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    sptr<AccessibilityAccountData> accountData = g_ins->GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    connection->OnAbilityConnectDone(elementName, stub, 0);

    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
    if (!inputInterceptor_) {
        GTEST_LOG_(INFO) << "AamsKeyEventFilterTest InputInterceptor is nullptr";
        return;
    }
}

void AamsKeyEventFilterTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest ModuleTest TearDown";
    inputInterceptor_ = nullptr;
    mock_ = nullptr;
}

void AamsKeyEventFilterTest::WritefileAll(const char* fname, const char* data) const
{
    FILE *fp;
    if ((fp = fopen(fname, "w")) == nullptr) {
        printf("open file %s fail \n", fname);
    }

    fprintf(fp, "%s", data);
    fclose(fp);
}

void AamsKeyEventFilterTest::CreateAccessibilityConfigForKeyEvent()
{
    std::ostringstream os;
    Json::Value object1, targetBundleNames,
    accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (remove("/system/app/dummy_accessibility_ability_config.json") == 0) {
        GTEST_LOG_(INFO) << "remove successful";
    } else {
        GTEST_LOG_(INFO) << "remove failed";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
    accessibilityAbilityTypes[TEST_NUM_2] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = TEST_TIME_OUT;
    accessibilityCapabilities[0] = "keyEventObserver";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.android.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();

    WritefileAll("/system/app/dummy_accessibility_ability_config.json", jsonStr.c_str());
}

std::shared_ptr<MMI::KeyEvent> AamsKeyEventFilterTest::CreateOnKeyEvent(int keycode) const
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};

    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(keycode);

    return keyEvent;
}

/**
 * @tc.number: OnKeyEvent001
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility responds the keyevent within 500 ms.(handled is true)
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent001 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

	if (g_inputEventConsumer != nullptr) {
		g_inputEventConsumer->OnInputEvent(keyEvent);
	}

    bool handled = true;
    int sequence = 1;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    WaitUntilTaskFinished();

    EXPECT_EQ(g_testKeyPressEvent, 1);
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent001 end";
}

/**
 * @tc.number: OnKeyEvent002
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility responds the keyevent within 500 ms.(handled is false)
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent002 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

    if (g_inputEventConsumer != nullptr) {
		g_inputEventConsumer->OnInputEvent(keyEvent);
	}

    bool handled = false;
    int sequence = TEST_NUM_2;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    sleep(1);
    WaitUntilTaskFinished();

    EXPECT_EQ(g_testKeyPressEvent, TEST_NUM_2);
    EXPECT_EQ(g_mTkeyCode, MMI::KeyEvent::KEYCODE_VOLUME_UP);
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent002 end";
}

/**
 * @tc.number: OnKeyEvent003
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessiblityAbility doesn't respond the keyevent within 500 ms.
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent003 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);

    GTEST_LOG_(INFO) << "AddConnection result start-----------";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);
    GTEST_LOG_(INFO) << "AddConnection result end ----------";

    if (g_inputEventConsumer != nullptr) {
		g_inputEventConsumer->OnInputEvent(keyEvent);
	}
    sleep(1);
    WaitUntilTaskFinished();

    EXPECT_EQ(g_testKeyPressEvent, TEST_NUM_3);
    EXPECT_EQ(g_mTkeyCode, MMI::KeyEvent::KEYCODE_VOLUME_UP);
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent003 end";
}
}  // namespace Accessibility
}  // namespace OHOS