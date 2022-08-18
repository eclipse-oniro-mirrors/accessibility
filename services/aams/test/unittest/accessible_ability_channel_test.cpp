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
#include "accessibility_ability_helper.h"
#include "accessibility_account_data.h"
#include "accessibility_element_operator_proxy.h"
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
}

void AccessibleAbilityChannelUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelUnitTest End ######################";
}

void AccessibleAbilityChannelUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Start AAMS
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();

    // Add AA client
    AccessibilityAbilityInitParams initParams;
    initParams.capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    ASSERT_TRUE(abilityInfo);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(ACCOUNT_ID);
    ASSERT_TRUE(accountData);
    connection_ = new AccessibleAbilityConnection(accountData, 0, *abilityInfo);
    ASSERT_TRUE(connection_);
    channel_ = new AccessibleAbilityChannel(*connection_);
    ASSERT_TRUE(channel_);

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
            break;
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
    EXPECT_FALSE(channel_->GetWindow(WINDOW_ID, windowInfo));
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindow_001 end";
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
    EXPECT_TRUE(channel_->GetWindows(windows));
    EXPECT_EQ(static_cast<int>(windows.size()), 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_GetWindows_001 end";
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
    EXPECT_TRUE(channel_->GetWindowsByDisplayId(DISPLAY_ID, windows));
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
    EXPECT_FALSE(channel_->SendSimulateGesture(nullptr));
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SendSimulateGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannel_Unittest_SetEventTypeFilter_001
 * @tc.name: SetEventTypeFilter
 * @tc.desc: Test function SetEventTypeFilter
 */
HWTEST_F(AccessibleAbilityChannelUnitTest,
    AccessibleAbilityChannel_Unittest_SetEventTypeFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetEventTypeFilter_001 start";
    EXPECT_TRUE(channel_->SetEventTypeFilter(TYPE_VIEW_CLICKED_EVENT));
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetEventTypeFilter_001 end";
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
    EXPECT_TRUE(channel_->SetTargetBundleName(targetBundleNames));
    GTEST_LOG_(INFO) << "AccessibleAbilityChannel_Unittest_SetTargetBundleName_001 end";
}
} // namespace Accessibility
} // namespace OHOS