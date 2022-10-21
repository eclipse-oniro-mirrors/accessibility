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
#include "accessibility_common_helper.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_input_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint8_t TEST_NUM_2 = 2;
    constexpr uint8_t TEST_NUM_3 = 3;
    constexpr uint16_t TEST_NUM_THOUSAND = 1000;
} // namespace

static void WaitUntilTaskFinished()
{
    const uint32_t maxRetryCount = TEST_NUM_THOUSAND;
    const uint32_t sleepTime = TEST_NUM_THOUSAND;
    uint32_t count = 0;
    auto handler = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainHandler();
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
    sptr<AccessibleAbilityChannel> aastub_ =  nullptr;
    sptr<AccessibleAbilityChannel> aacs_ = nullptr;

    int32_t tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void AddConnection();
    void TearDown();
    void WritefileAll(const char* fname, const char* data) const;
    std::shared_ptr<MMI::KeyEvent> CreateOnKeyEvent(int32_t keycode) const;
};

void AamsKeyEventFilterTest::SetUpTestCase(void)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AamsKeyEventFilterTest::TearDownTestCase(void)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AamsKeyEventFilterTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest ModuleTest SetUp";

    // Add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetCapabilityValues(CAPABILITY_KEY_EVENT_OBSERVER);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, aastub_);
    interceptorId_ = std::make_shared<AccessibilityInputEventConsumer>();
    MMI::InputManager::GetInstance()->AddInterceptor(interceptorId_);
}

void AamsKeyEventFilterTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest ModuleTest TearDown";
    interceptorId_ = nullptr;
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
 * @tc.desc: AccessibleAbility responds the keyevent within 500 ms.(handled is true)
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent001 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    sleep(TEST_NUM_3);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();
    EXPECT_EQ(connectionMaps.size(), 1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }

    bool handled = true;
    int32_t sequence = 1;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    ASSERT_TRUE(ptr_connect);
    aacs_ = new AccessibleAbilityChannel(accountData->GetAccountId(),
        ptr_connect->GetAbilityInfo().GetId());

    aacs_->SetOnKeyPressEventResult(handled, sequence);
    WaitUntilTaskFinished();

    EXPECT_EQ(AccessibilityHelper::GetInstance().GetTestKeyPressEvent(), 1);
    aacs_ = nullptr;
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent001 end";
}

/**
 * @tc.number: OnKeyEvent002
 * @tc.name: OnKeyEvent
 * @tc.desc: AccessibleAbility responds the keyevent within 500 ms.(handled is false)
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent002 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    sleep(TEST_NUM_3);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();
    EXPECT_EQ(connectionMaps.size(), 1);

    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (inputEventConsumer != nullptr) {
        inputEventConsumer->OnInputEvent(keyEvent);
    }

    bool handled = false;
    int32_t sequence = TEST_NUM_2;

    auto iter = connectionMaps.begin();
    sptr<AccessibleAbilityConnection> ptr_connect = iter->second;
    ASSERT_TRUE(ptr_connect);
    aacs_ = new AccessibleAbilityChannel(accountData->GetAccountId(),
        ptr_connect->GetAbilityInfo().GetId());

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
 * @tc.desc: AccessibleAbility doesn't respond the keyevent within 500 ms.
 */
HWTEST_F(AamsKeyEventFilterTest, AamsKeyEventFilterTest_Moduletest_OnKeyEvent003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AamsKeyEventFilterTest_Moduletest_OnKeyEvent003 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateOnKeyEvent(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    sleep(TEST_NUM_3);
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData()
            ->GetConnectedA11yAbilities();
    EXPECT_EQ(connectionMaps.size(), 1);

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