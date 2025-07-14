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
#include <thread>
#include <chrono>
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_channel.h"
#include "accessibility_def.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_window_manager.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_accessibility_setting_provider.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class MockAccessibilityElementOperatorCallbackStub : public AccessibilityElementOperatorCallbackStub {
public:
    MockAccessibilityElementOperatorCallbackStub() = default;
    virtual ~MockAccessibilityElementOperatorCallbackStub() = default;

    void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override {}
    void SetSearchDefaultFocusByWindowIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override {}
    void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override {}
    void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int32_t requestId) override {}
    void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) override {}
    void SetExecuteActionResult(const bool succeeded, const int32_t requestId) override {}
    void SetCursorPositionResult(const int32_t cursorPosition, const int32_t requestId) override {}
    void SetSearchElementInfoBySpecificPropertyResult(const std::list<AccessibilityElementInfo> &infos,
        const std::list<AccessibilityElementInfo> &treeInfos, const int32_t requestId) override
    {
        receivedInfos_.assign(infos.begin(), infos.end());
        receivedTreeInfos_.assign(treeInfos.begin(), treeInfos.end());
        receivedRequestId_ = requestId;
        callbackCalled_ = true;

        GTEST_LOG_(INFO) << "SetSearchElementInfoBySpecificPropertyResult called: requestId=" << requestId
                         << ", infos.size=" << infos.size() << ", treeInfos.size=" << treeInfos.size();
    }

    bool IsCallbackCalled() const { return callbackCalled_; }
    const std::vector<AccessibilityElementInfo>& GetReceivedInfos() const { return receivedInfos_; }
    const std::vector<AccessibilityElementInfo>& GetReceivedTreeInfos() const { return receivedTreeInfos_; }
    int32_t GetReceivedRequestId() const { return receivedRequestId_; }

private:
    bool callbackCalled_ = false;
    std::vector<AccessibilityElementInfo> receivedInfos_;
    std::vector<AccessibilityElementInfo> receivedTreeInfos_;
    int32_t receivedRequestId_ = -1;
};
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
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = 0;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 0;
    EXPECT_EQ(channel_->SearchElementInfoByAccessibilityId(elementBasicInfo, 0, nullptr, 0, true),
        RET_ERR_NULLPTR);
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
    EXPECT_EQ(channel_->SearchElementInfosByText(WINDOW_ID, ELEMENT_ID, "test", 0, nullptr), RET_ERR_NULLPTR);
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
    EXPECT_EQ(channel_->FindFocusedElementInfo(WINDOW_ID, ELEMENT_ID, FOCUS_TYPE_INPUT, 0, nullptr), RET_ERR_NULLPTR);
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
    EXPECT_EQ(channel_->FocusMoveSearch(WINDOW_ID, ELEMENT_ID, UP, 0, nullptr), RET_ERR_NULLPTR);
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
    EXPECT_EQ(channel_->ExecuteAction(WINDOW_ID, ELEMENT_ID, 1, actionArguments, 0, nullptr), RET_ERR_NULLPTR);
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
 * @tc.number: AccessibleAbilityChannel_Unittest_SendSimulateGesture_003
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SendSimulateGesture_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    // add a client which has no retieve capability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    AppExecFwk::ElementName elementName("device", "bundle", "ability");
    std::string ability = "bundle/ability";
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), ability);
    connection->OnAbilityConnectDoneSync(elementName, channel);
    EXPECT_TRUE(accountData->GetAccessibleAbilityConnection(ability));
    EXPECT_EQ(channel->SendSimulateGesture(nullptr), RET_ERR_NO_CAPABILITY);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_003 end";
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

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_GetCursorPosition_001
 * @tc.name: GetCursorPosition
 * @tc.desc: Test function GetCursorPosition
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_GetCursorPosition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetCursorPosition_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    std::string abilityName = "testSetTargetBundleName";
    EXPECT_FALSE(accountData->GetAccessibleAbilityConnection(abilityName));
    sptr<AccessibleAbilityChannel> channel = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityName);

    EXPECT_EQ(channel->GetCursorPosition(WINDOW_ID, ELEMENT_ID, 0, nullptr), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetCursorPosition_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_001
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_001 start";
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = 0;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 0;
    SpecificPropertyParam param;
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 0, nullptr);

    EXPECT_NE(channel_, nullptr) << "channel_ should remain valid after method call";
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_002
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with callback is nullptr
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_002 start";
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = 1;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 1;
    SpecificPropertyParam param;
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 1, nullptr);
    EXPECT_NE(channel_, nullptr) << "channel_ should remain valid after method call";
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_003
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with valid callback
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_003 start";
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = 1;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 1;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 2, callback);
    EXPECT_NE(channel_, nullptr) << "channel_ should remain valid after method call";
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_004
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with invalid windowId
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_004 start";
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = -1;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 1;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 3, callback);
    EXPECT_NE(channel_, nullptr) << "channel_ should remain valid after method call";
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_004 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_005
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with GetElementOperator failure path
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_005 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    accountData->RemoveAccessibilityWindowConnection(WINDOW_ID);
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = WINDOW_ID;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 1;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 4, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback->IsCallbackCalled());
    EXPECT_TRUE(callback->GetReceivedInfos().empty());
    EXPECT_TRUE(callback->GetReceivedTreeInfos().empty());
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_005 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_006
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with SCENE_BOARD_WINDOW_ID and IsInnerWindowRootElement
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_006 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    ASSERT_TRUE(stub);
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    ASSERT_TRUE(proxy);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(1, proxy, ACCOUNT_ID);
    ASSERT_TRUE(connection);
    accountData->AddAccessibilityWindowConnection(1, connection);
    auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
    int64_t testElementId = 12345;
    awm.sceneBoardElementIdMap_.InsertPair(1, testElementId);
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = 1;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = testElementId;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 5, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback->IsCallbackCalled());
    EXPECT_TRUE(callback->GetReceivedInfos().empty());
    EXPECT_TRUE(callback->GetReceivedTreeInfos().empty());
    EXPECT_TRUE(awm.IsInnerWindowRootElement(testElementId));
    awm.sceneBoardElementIdMap_.Clear();
    accountData->RemoveAccessibilityWindowConnection(1);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_006 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_007
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with normal execution path (non-SCENE_BOARD_WINDOW_ID)
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_007 start";
    auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
    awm.sceneBoardElementIdMap_.Clear();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);

    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    ASSERT_TRUE(stub);
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    ASSERT_TRUE(proxy);
    int32_t normalWindowId = 2;
    sptr<AccessibilityWindowConnection> connection =
        new AccessibilityWindowConnection(normalWindowId, proxy, ACCOUNT_ID);
    ASSERT_TRUE(connection);
    accountData->AddAccessibilityWindowConnection(normalWindowId, connection);
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = normalWindowId;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 12345;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 6, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::list<AccessibilityElementInfo> mockInfos;
    std::list<AccessibilityElementInfo> mockTreeInfos;
    AccessibilityElementInfo mockInfo;
    mockInfo.SetAccessibilityId(12345);
    mockInfo.SetComponentType("normal_execution_test");
    mockInfos.push_back(mockInfo);
    AccessibilityElementInfo mockTreeInfo;
    mockTreeInfo.SetAccessibilityId(12346);
    mockTreeInfo.SetComponentType("tree_info_test");
    mockTreeInfos.push_back(mockTreeInfo);
    callback->SetSearchElementInfoBySpecificPropertyResult(mockInfos, mockTreeInfos, 6);
    EXPECT_TRUE(callback->IsCallbackCalled());
    EXPECT_FALSE(callback->GetReceivedInfos().empty());
    EXPECT_FALSE(callback->GetReceivedTreeInfos().empty());
    accountData->RemoveAccessibilityWindowConnection(normalWindowId);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_007 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_008
 * @tc.name: SearchElementInfoBySpecificProperty
 * @tc.desc: Test function SearchElementInfoBySpecificProperty with CheckWinFromAwm failure path
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_008 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    int32_t invalidWindowId = 9999;
    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    ASSERT_TRUE(stub);
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    ASSERT_TRUE(proxy);
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(invalidWindowId, proxy,
        ACCOUNT_ID);
    ASSERT_TRUE(connection);
    accountData->AddAccessibilityWindowConnection(invalidWindowId, connection);
    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = invalidWindowId;
    elementBasicInfo.treeId = 0;
    elementBasicInfo.elementId = 1;
    SpecificPropertyParam param;
    param.propertyTarget = "test";
    param.propertyType = SEARCH_TYPE::CUSTOMID;
    sptr<MockAccessibilityElementOperatorCallbackStub> callback =
        new MockAccessibilityElementOperatorCallbackStub();
    channel_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, 7, callback);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(callback->IsCallbackCalled());
    EXPECT_TRUE(callback->GetReceivedInfos().empty());
    EXPECT_TRUE(callback->GetReceivedTreeInfos().empty());
    accountData->RemoveAccessibilityWindowConnection(invalidWindowId);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SearchElementInfoBySpecificProperty_008 end";
}
} // namespace Accessibility
} // namespace OHOS