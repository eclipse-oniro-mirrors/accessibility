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
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessibility_element_operator_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int RETRY_TIMES = 10;
    constexpr int ELEMENT_ID = 0;
    constexpr int INVALID_ELEMENT_ID = -1;
    constexpr uint32_t INVALID_SEQUENCE = 0;
    constexpr uint64_t DISPLAY_ID = 0;
    constexpr int32_t WINDOW_ID = 0;
    constexpr int32_t ACCOUNT_ID = 0;
} // namespace

class AccessibleAbilityChannelUnitTest : public ::testing::Test {
public:
    AccessibleAbilityChannelUnitTest()
    {}
    ~AccessibleAbilityChannelUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    bool TestElementId();
    sptr<AccessibleAbilityChannel> channel_ = nullptr;
    sptr<AccessibleAbilityConnection> connection_ = nullptr;
};

void AccessibleAbilityChannelUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibleAbilityChannelUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibleAbilityChannelUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Add AA client
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "testBundleName";
    initParams.name = "testAbilityName";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    ASSERT_TRUE(abilityInfo);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    abilityInfo->SetCapabilityValues(CAPABILITY_RETRIEVE | CAPABILITY_GESTURE);
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    connection_ = new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    ASSERT_TRUE(connection_);
    AppExecFwk::ElementName elementName("testDeviceId", "testBundleName", "testAbilityName");
    channel_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    ASSERT_TRUE(channel_);
    connection_->OnAbilityConnectDoneSync(elementName, channel_);

    // Add window connection
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    ASSERT_TRUE(stub);
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    ASSERT_TRUE(proxy);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(WINDOW_ID, proxy, ACCOUNT_ID);
    ASSERT_TRUE(connection);
    accountData->AddAccessibilityWindowConnection(WINDOW_ID, connection);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelElementId(), INVALID_ELEMENT_ID);
}

void AccessibleAbilityChannelUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    connection_ = nullptr;
    channel_ = nullptr;
    AccessibilityAbilityHelper::GetInstance().SetTestChannelElementId(INVALID_ELEMENT_ID);
}

bool AccessibleAbilityChannelUnitTest::TestElementId()
{
    int retryCount = 0;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        int elementId = AccessibilityAbilityHelper::GetInstance().GetTestChannelElementId();
        GTEST_LOG_(INFO) << "element id is : " << elementId;
        if (elementId == ELEMENT_ID) {
            GTEST_LOG_(INFO) << "element id is true";
            return true;
        }
        retryCount++;
    }
    return false;
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoByAccessibilityId_001 start";
    channel_->SearchElementInfoByAccessibilityId(WINDOW_ID, ELEMENT_ID, 0, nullptr, 0);
    bool result = TestElementId();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfosByText_001 start";
    channel_->SearchElementInfosByText(WINDOW_ID, ELEMENT_ID, "test", 0, nullptr);
    bool result = TestElementId();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_FindFocusedElementInfo_001 start";
    channel_->FindFocusedElementInfo(WINDOW_ID, ELEMENT_ID, FOCUS_TYPE_INPUT, 0, nullptr);
    bool result = TestElementId();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_FocusMoveSearch_001 start";
    channel_->FocusMoveSearch(WINDOW_ID, ELEMENT_ID, UP, 0, nullptr);
    bool result = TestElementId();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_ExecuteAction_001 start";
    std::map<std::string, std::string> actionArguments;
    channel_->ExecuteAction(WINDOW_ID, ELEMENT_ID, ACCESSIBILITY_ACTION_FOCUS, actionArguments, 0, nullptr);
    bool result = TestElementId();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_ExecuteAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindow_001
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_001 start";
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(channel_->GetWindow(WINDOW_ID, windowInfo), RET_ERR_NO_WINDOW_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindow_002
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string ability = "testGetWindow";
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(ability));

    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), ability);
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(channel->GetWindow(WINDOW_ID, windowInfo), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindow_003
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindow_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    // add a client which has no retieve capability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    AppExecFwk::ElementName elementName("device", "bundle", "ability");
    std::string ability = "bundle/ability";
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), ability);
    connection->OnAbilityConnectDoneSync(elementName, channel);
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(ability));

    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(channel->GetWindow(WINDOW_ID, windowInfo), RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_001 start";
    std::vector<AccessibilityWindowInfo> windows;
    EXPECT_EQ(channel_->GetWindows(windows), RET_OK);
    EXPECT_EQ(static_cast<int>(windows.size()), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_002 start";
    std::string abilityName = "testGetWindows";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(abilityName));
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityName);

    std::vector<AccessibilityWindowInfo> windows;
    EXPECT_EQ(channel->GetWindows(windows), RET_ERR_NO_CONNECTION);
    EXPECT_EQ(static_cast<int>(windows.size()), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindows_003
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindows_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    // Add AA client which has no retieve capability
    AccessibilityAbilityInitParams initParams;
    initParams.bundleName = "bundle";
    initParams.name = "ability";
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    AppExecFwk::ElementName elementName("device", "bundle", "ability");
    sptr<AccessibleAbilityChannel> channel =
        new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, channel);
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(abilityInfo->GetId()));

    std::vector<AccessibilityWindowInfo> windows;
    EXPECT_EQ(channel->GetWindows(windows), RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetWindowsByDisplayId_001
 * @tc.name: GetWindowsByDisplayId
 * @tc.desc: Test function GetWindowsByDisplayId
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetWindowsByDisplayId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindowsByDisplayId_001 start";
    std::vector<AccessibilityWindowInfo> windows;
    EXPECT_EQ(channel_->GetWindowsByDisplayId(DISPLAY_ID, windows), RET_OK);
    EXPECT_EQ(static_cast<int>(windows.size()), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindowsByDisplayId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SetOnKeyPressEventResult_001
 * @tc.name: SetOnKeyPressEventResult
 * @tc.desc: Test function SetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SetOnKeyPressEventResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetOnKeyPressEventResult_001 start";
    AccessibilityAbilityHelper::GetInstance().SetTestSequence(INVALID_SEQUENCE);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestSequence(), INVALID_SEQUENCE);
    sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
    ASSERT_TRUE(keyEventFilter);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetKeyEventFilter(keyEventFilter);

    uint32_t sequence = 1;
    channel_->SetOnKeyPressEventResult(true, sequence);

    int retryCount = 0;
    bool result = false;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        uint32_t sequenceNum = AccessibilityAbilityHelper::GetInstance().GetTestSequence();
        GTEST_LOG_(INFO) << "sequence is : " << sequenceNum;
        if (sequenceNum == sequence) {
            GTEST_LOG_(INFO) << "sequence is true";
            result = true;
            break;
        }
        retryCount++;
    }
    EXPECT_TRUE(result);
    AccessibilityAbilityHelper::GetInstance().SetTestSequence(INVALID_SEQUENCE);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetOnKeyPressEventResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_001 start";
    EXPECT_EQ(channel_->SendSimulateGesture(nullptr), RET_ERR_NO_INJECTOR);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SendSimulateGesture_002
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SendSimulateGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_002 start";
    std::string abilityName = "testGesture";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(abilityName));
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityName);
    EXPECT_EQ(channel->SendSimulateGesture(nullptr), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SetTargetBundleName_001
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SetTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetTargetBundleName_001 start";
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(channel_->SetTargetBundleName(targetBundleNames), RET_OK);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetTargetBundleName_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SetTargetBundleName_002
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SetTargetBundleName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetTargetBundleName_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string abilityName = "testSetTargetBundleName";
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(abilityName));
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityName);
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(channel->SetTargetBundleName(targetBundleNames), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetTargetBundleName_002 end";
}
} // namespace Accessibility
} // namespace OHOS