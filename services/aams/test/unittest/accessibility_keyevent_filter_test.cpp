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
#include "accessibility_common_helper.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t SLEEP_TIME_3 = 3;
    constexpr int32_t ACCOUNT_ID = 100;
    constexpr int32_t CONNECTION_ID = 1;
} // namespace

class KeyEventFilterUnitTest : public ::testing::Test {
public:
    KeyEventFilterUnitTest()
    {}
    ~KeyEventFilterUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<KeyEventFilter> keyEventFilter_ = nullptr;
};

void KeyEventFilterUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### KeyEventFilterUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void KeyEventFilterUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### KeyEventFilterUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void KeyEventFilterUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    keyEventFilter_ = std::make_shared<KeyEventFilter>();
}

void KeyEventFilterUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    keyEventFilter_ = nullptr;
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(invalid key code)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_001 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    EXPECT_FALSE(keyEventFilter_->OnKeyEvent(*event));

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_002
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(volume up)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_002 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_002 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_003
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(volume down)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_003 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_003 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_004
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(volume down)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_004 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_004 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_005
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent(volume down)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_005 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_005 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult(keyEvent is handled)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    keyEventFilter_->SetServiceOnKeyEventResult(*connection, true, 1);
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult(keyEvent is not handled)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    keyEventFilter_->SetServiceOnKeyEventResult(*connection, false, 1);
    EXPECT_EQ(MMI::KeyEvent::KEYCODE_VOLUME_DOWN, AccessibilityAbilityHelper::GetInstance().GetKeyCode());
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult(connection is not same)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    sptr<AccessibleAbilityConnection> connection2 = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    keyEventFilter_->SetServiceOnKeyEventResult(*connection2, true, 1);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult(squence number is not same)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    keyEventFilter_->SetServiceOnKeyEventResult(*connection, true, 2);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_005
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult(handle one)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_005 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    keyEventFilter_->SetServiceOnKeyEventResult(*connection, true, 2);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_005 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_ClearServiceKeyEvents_001
 * @tc.name: ClearServiceKeyEvents
 * @tc.desc: Test function ClearServiceKeyEvents(connection is not same)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_ClearServiceKeyEvents_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_001 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    sptr<AccessibleAbilityConnection> connection2 = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    keyEventFilter_->ClearServiceKeyEvents(*connection2);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetKeyCode() == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
            return true;
        } else {
            return false;
        }
        }), SLEEP_TIME_3);
    EXPECT_TRUE(ret);
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_ClearServiceKeyEvents_002
 * @tc.name: ClearServiceKeyEvents
 * @tc.desc: Test function ClearServiceKeyEvents(clear success)
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_ClearServiceKeyEvents_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_002 start";

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    AccessibilityAbilityInfo abilityInfo;
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(ACCOUNT_ID,
        CONNECTION_ID, abilityInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    connection->Connect(elementName);
    accountData->AddConnectedAbility(connection);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(MMI::KeyEvent::KEYCODE_VOLUME_DOWN);
    event->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    EXPECT_TRUE(keyEventFilter_->OnKeyEvent(*event));
    keyEventFilter_->ClearServiceKeyEvents(*connection);
    EXPECT_EQ(MMI::KeyEvent::KEYCODE_VOLUME_DOWN, AccessibilityAbilityHelper::GetInstance().GetKeyCode());
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([=]() -> bool {
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        if (aams.GetMainRunner()->GetEventQueue()->IsIdle()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    if (!ret) {
        GTEST_LOG_(INFO) << "AamsAccessibleAbilityChannelTest TearDown EventQueue is not empty";
    }
    AccessibilityAbilityHelper::GetInstance().ClearKeyCode();
    accountData->RemoveConnectedAbility(elementName);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_002 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_DestroyEvents_001
 * @tc.name: DestroyEvents
 * @tc.desc: Test function DestroyEvents
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_DestroyEvents_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_DestroyEvents_001 start";

    keyEventFilter_->DestroyEvents();
    EXPECT_TRUE(AccessibilityAbilityHelper::GetInstance().GetDestroyState());

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_DestroyEvents_001 end";
}
} // namespace Accessibility
} // namespace OHOS