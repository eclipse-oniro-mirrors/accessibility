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

#include <map>
#include <memory>
#include <gtest/gtest.h>
#include "accessibility_keyevent_filter.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class KeyEventFilterUnitTest : public ::testing::Test {
public:
    KeyEventFilterUnitTest() {}
    ~KeyEventFilterUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void AddConnection();
    shared_ptr<KeyEventFilter> keyEventFilter_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

void KeyEventFilterUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### KeyEventFilterUnitTest Start ######################";
}

void KeyEventFilterUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### KeyEventFilterUnitTest End ######################";
}

void KeyEventFilterUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Register bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();

    keyEventFilter_ = make_shared<KeyEventFilter>();
}

void KeyEventFilterUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    keyEventFilter_ = nullptr;
    mock_ = nullptr;
}

void KeyEventFilterUnitTest::AddConnection()
{
    GTEST_LOG_(INFO) << "KeyEventFilterUnitTest AddConnection";
    sptr<AccessibleAbilityClientStubImpl> stub = new AccessibleAbilityClientStubImpl();
    shared_ptr<AccessibleAbilityManagerService> aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();

    // add an ability connection client
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(extensionInfo);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    sptr<AccessibilityAccountData> accountData = aams->GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    connection->OnAbilityConnectDone(elementName, stub, 0);
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent
 *           Dispatch MMI::KeyEvent when there is no connection.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_001 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_OnKeyEvent_002
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent
 *           Dispatch MMI::KeyEvent after connected.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_OnKeyEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_002 start";

    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    AddConnection();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_OnKeyEvent_002 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult
 *           Set result when there is no processingEvent.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001 start";

    bool isHandled = true;
    uint32_t sequenceNum = 0;
    int connectionId = 0;
    int accountId = 0;
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(accountId);
    AccessibilityAbilityInfo abilityInfo;
    shared_ptr<AccessibleAbilityConnection> connection =
        make_shared<AccessibleAbilityConnection>(accountData, connectionId, abilityInfo);

    AddConnection();
    keyEventFilter_->OnKeyEvent(*event);
    keyEventFilter_->SetServiceOnKeyEventResult(*connection, isHandled, sequenceNum);

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult
 *           Find processing event failed and set result after connected.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002 start";

    AddConnection();

    GTEST_LOG_(INFO) << "Dispatch MMI::KeyEvent";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "Set result";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    if (connectionMaps.size() == 1) {
        auto iter = connectionMaps.begin();
        bool isHandled = true;
        uint32_t sequenceNum = 0;
        keyEventFilter_->SetServiceOnKeyEventResult(*(iter->second), isHandled, sequenceNum);
    }

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_002 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult
 *           Find processing event successfully and set result after connected.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003 start";

    AddConnection();

    GTEST_LOG_(INFO) << "Dispatch MMI::KeyEvent";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "Set result";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    if (connectionMaps.size() == 1) {
        auto iter = connectionMaps.begin();
        bool isHandled = true;
        uint32_t sequenceNum = 1;
        keyEventFilter_->SetServiceOnKeyEventResult(*(iter->second), isHandled, sequenceNum);
    }

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_003 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004
 * @tc.name: SetServiceOnKeyEventResult
 * @tc.desc: Test function SetServiceOnKeyEventResult
 *           Find processing event successfully and set result after connected.
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004 start";

    AddConnection();

    GTEST_LOG_(INFO) << "Dispatch MMI::KeyEvent";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "Set result";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    if (connectionMaps.size() == 1) {
        auto iter = connectionMaps.begin();
        bool isHandled = false;
        uint32_t sequenceNum = 1;
        keyEventFilter_->SetServiceOnKeyEventResult(*(iter->second), isHandled, sequenceNum);
    }

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_SetServiceOnKeyEventResult_004 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_ClearServiceKeyEvents_001
 * @tc.name: ClearServiceKeyEvents
 * @tc.desc: Test function ClearServiceKeyEvents
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_ClearServiceKeyEvents_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_001 start";

    AddConnection();

    GTEST_LOG_(INFO) << "Dispatch MMI::KeyEvent";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "Clear service KeyEvents";
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps =
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()
        ->GetCurrentAccountData()->GetConnectedA11yAbilities();
    EXPECT_EQ((int)connectionMaps.size(), 1);

    if (connectionMaps.size() == 1) {
        auto iter = connectionMaps.begin();
        keyEventFilter_->ClearServiceKeyEvents(*(iter->second));
    }

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ClearServiceKeyEvents_001 end";
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

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_DestroyEvents_001 end";
}

/**
 * @tc.number: KeyEventFilter_Unittest_ProcessEvent_001
 * @tc.name: ProcessEvent
 * @tc.desc: Test function ProcessEvent
 */
HWTEST_F(KeyEventFilterUnitTest, KeyEventFilter_Unittest_ProcessEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ProcessEvent_001 start";

    GTEST_LOG_(INFO) << "Add connection";
    AddConnection();

    GTEST_LOG_(INFO) << "Dispatch MMI::KeyEvent";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    keyEventFilter_->OnKeyEvent(*event);

    GTEST_LOG_(INFO) << "Process event";
    sleep(3); // wait for ProcessEvent

    GTEST_LOG_(INFO) << "KeyEventFilter_Unittest_ProcessEvent_001 end";
}
} // namespace Accessibility
} // namespace OHOS
