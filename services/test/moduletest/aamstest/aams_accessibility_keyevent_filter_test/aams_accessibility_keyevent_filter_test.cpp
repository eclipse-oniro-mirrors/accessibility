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
#include <map>
#include <memory>
#include <unistd.h>
#include "accessibility_account_data.h"
#include "accessibility_helper.h"
#include "accessibility_input_interceptor.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "json.h"
#include "mock_bundle_manager.h"
#include "mock_input_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
static constexpr uint8_t TEST_NUM_2 = 2;
static constexpr uint8_t TEST_NUM_3 = 3;
static constexpr uint16_t TEST_TIME_OUT = 10000;
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

    std::shared_ptr<MMI::IInputEventConsumer> interceptorId_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
    sptr<AccessibleAbilityChannel> aastub_ =  nullptr;
    sptr<AccessibleAbilityChannel> aacs_ = nullptr;

    int32_t tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void AddConnection();
    void TearDown();
    void CreateAccessibilityConfigForKeyEvent();
    void WritefileAll(const char* fname, const char* data) const;
    std::shared_ptr<MMI::KeyEvent> CreateOnKeyEvent(int32_t keycode) const;
};

static std::shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;

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

    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();

    // Add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(*connection);
    connection->OnAbilityConnectDone(elementName, aastub_, 0);
    sleep(TEST_NUM_3);
    interceptorId_ = std::make_shared<AccessibilityInputEventConsumer>();
    MMI::InputManager::GetInstance()->AddInterceptor(interceptorId_);
}

void AamsKeyEventFilterTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest ModuleTest TearDown";
    interceptorId_ = nullptr;
    mock_ = nullptr;
    aastub_ = nullptr;
}

void AamsKeyEventFilterTest::WritefileAll(const char* fname, const char* data) const
{
    FILE* fp;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsKeyEventFilterTest::CreateAccessibilityConfigForKeyEvent()
{
    std::ostringstream os;
    Json::Value object1, targetBundleNames, accessibilityAbilityTypes, accessibilityEventTypes,
        accessibilityCapabilities;
    string jsonStr;

    if (!remove("/system/app/dummy_accessibility_ability_config.json")) {
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
    object1["settingsAbility"] = "com.example.ohos.accessibility.ServiceSettingsAbility";
    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();

    WritefileAll("/system/app/dummy_accessibility_ability_config.json", jsonStr.c_str());
}

std::shared_ptr<MMI::KeyEvent> AamsKeyEventFilterTest::CreateOnKeyEvent(int32_t keycode) const
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
    sleep(TEST_NUM_3);
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
            ->GetCurrentAccountData()
            ->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }

    bool handled = true;
    int32_t sequence = 1;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    aacs_ = new AccessibleAbilityChannel(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    WaitUntilTaskFinished();

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestKeyPressEvent(), 1);
    aacs_ = nullptr;
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
    sleep(TEST_NUM_3);
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
            ->GetCurrentAccountData()
            ->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }

    bool handled = false;
    int32_t sequence = TEST_NUM_2;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    aacs_ = new AccessibleAbilityChannel(*ptr_connect);

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    sleep(1);
    WaitUntilTaskFinished();

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestKeyPressEvent(), TEST_NUM_2);
    EXPECT_EQ(MMI::MockInputManager::GetKeyCode(), MMI::KeyEvent::KEYCODE_VOLUME_UP);
    aacs_ = nullptr;
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
    sleep(TEST_NUM_3);
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
            ->GetCurrentAccountData()
            ->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }
    sleep(1);
    WaitUntilTaskFinished();

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestKeyPressEvent(), TEST_NUM_3);
    EXPECT_EQ(MMI::MockInputManager::GetKeyCode(), MMI::KeyEvent::KEYCODE_VOLUME_UP);
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent003 end";
}
} // namespace Accessibility
} // namespace OHOS