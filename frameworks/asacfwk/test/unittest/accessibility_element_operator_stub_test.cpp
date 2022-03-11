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
#include <memory>

#include "accessibility_element_operator_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityInteractionStubUnitTest : public ::testing::Test {
public:
    sptr<AccessibilityElementOperatorStub> mockStub_ = nullptr;
    AccessibilityElementOperatorStub::CallbackImpl callback_ {};
    AccessibilityInteractionStubUnitTest() {}
    ~AccessibilityInteractionStubUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionStubUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionStubUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionStubUnitTest SetUp() Start";
        mockStub_ = new AccessibilityElementOperatorStub();
        GTEST_LOG_(INFO) << "AccessibilityInteractionStubUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionStubUnitTest TearDown()";
        mockStub_ = nullptr;
    }
};

/**
 * @tc.number: OnRemoteRequest_001
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::SEARCH_BY_ACCESSIBILITY_ID),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 end";
}

HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(static_cast<uint32_t>(
        IAccessibilityElementOperator::Message::SEARCH_BY_TEXT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 end";
}

/**
 * @tc.number: OnRemoteRequest_003
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_003 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::FIND_FOCUSED_INFO), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_003 end";
}

/**
 * @tc.number: OnRemoteRequest_004
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_004 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::FOCUS_FIND), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_004 end";
}

/**
 * @tc.number: OnRemoteRequest_005
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_005 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::PERFORM_ACTION), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_005 end";
}

/**
 * @tc.number: OnRemoteRequest_006
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_006 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::CLEAR_FOCUS), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_006 end";
}

/**
 * @tc.number: OnRemoteRequest_007
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OnRemoteRequest_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_007 start";
    // Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibilityElementOperator::Message::OUTSIDE_TOUCH), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_007 end";
}

/**
 * @tc.number: SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 start";

    mockStub_->SearchElementInfoByAccessibilityId(1, 1, nullptr, 0);
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 start";

    std::string str = "test";
    mockStub_->SearchElementInfosByText(1, str, 1, nullptr);
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 end";
}

/**
 * @tc.number: FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 start";

    mockStub_->FindFocusedElementInfo(1, 1, 1, nullptr);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 start";

    mockStub_->FocusMoveSearch(1, 1, 1, nullptr);
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";

    std::map<std::string, std::string> actionArguments;
    mockStub_->ExecuteAction(1, 1, actionArguments, 1, nullptr);
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ClearFocus_001
 * @tc.name: ClearFocus
 * @tc.desc: Test function ClearFocus
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, ClearFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ClearFocus_001 start";

    mockStub_->ClearFocus();
    GTEST_LOG_(INFO) << "ClearFocus_001 end";
}

/**
 * @tc.number: OutsideTouch_001
 * @tc.name: OutsideTouch
 * @tc.desc: Test function OutsideTouch
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, OutsideTouch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutsideTouch_001 start";

    mockStub_->OutsideTouch();
    GTEST_LOG_(INFO) << "OutsideTouch_001 end";
}

/**
 * @tc.number: OutsideTouch_001
 * @tc.name: SetSearchElementInfoByAccessibilityIdResult
 * @tc.desc: Test function SetSearchElementInfoByAccessibilityIdResult
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SetSearchElementInfoByAccessibilityIdResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 start";

    std::list<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    callback_.SetSearchElementInfoByAccessibilityIdResult(infos, 1);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 end";
}

/**
 * @tc.number: SetSearchElementInfoByTextResult_001
 * @tc.name: SetSearchElementInfoByTextResult
 * @tc.desc: Test function SetSearchElementInfoByTextResult
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SetSearchElementInfoByTextResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 start";

    std::list<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    callback_.SetSearchElementInfoByTextResult(infos, 1);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 end";
}

/**
 * @tc.number: SetFindFocusedElementInfoResult_001
 * @tc.name: SetFindFocusedElementInfoResult
 * @tc.desc: Test function SetFindFocusedElementInfoResult
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SetFindFocusedElementInfoResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 start";
    AccessibilityElementInfo info {};
    callback_.SetFindFocusedElementInfoResult(info, 1);
    GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 end";
}

/**
 * @tc.number: SetFocusMoveSearchResult_001
 * @tc.name: SetFocusMoveSearchResult
 * @tc.desc: Test function SetFocusMoveSearchResult
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SetFocusMoveSearchResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 start";

    AccessibilityElementInfo info {};
    callback_.SetFocusMoveSearchResult(info, 1);
    GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 end";
}

/**
 * @tc.number: SetExecuteActionResult_001
 * @tc.name: SetExecuteActionResult
 * @tc.desc: Test function SetExecuteActionResult
 */
HWTEST_F(AccessibilityInteractionStubUnitTest, SetExecuteActionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 start";

    callback_.SetExecuteActionResult(true, 1);
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 end";
}
} // namespace Accessibility
} // namespace OHOS