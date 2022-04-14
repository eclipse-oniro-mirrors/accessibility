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
#include "accessibility_element_operator_impl.h"
#include "mock_accessibility_element_operator.h"

using namespace testing;
using namespace testing::ext;

const static int32_t ELEMENT_ID = 1;
const static int32_t REQUEST_ID = 1;
const static int32_t MODE = 0;
const static std::string TEST = "test";
const static int32_t FOCUS_TYPE = 1;
const static int32_t DIRECTION = 1;
const static int32_t ACTION = 1;

namespace OHOS {
namespace Accessibility {
class AccessibilityElementOperatorImplUnitTest : public ::testing::Test {
public:
    sptr<AccessibilityElementOperatorImpl> mockStub_ = nullptr;
    AccessibilityElementOperatorImpl::OperatorCallbackImpl callback_ {};
    AccessibilityElementOperatorImplUnitTest()
    {}
    ~AccessibilityElementOperatorImplUnitTest()
    {}
    int32_t windowID_ = 1;
    std::shared_ptr<MockAccessibilityElementOperator> operation_ = nullptr;
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementOperatorImplUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementOperatorImplUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementOperatorImplUnitTest SetUp() Start";
        operation_ = std::make_shared<MockAccessibilityElementOperator>();
        mockStub_ = new AccessibilityElementOperatorImpl(windowID_, operation_);
        GTEST_LOG_(INFO) << "AccessibilityElementOperatorImplUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementOperatorImplUnitTest TearDown()";
        mockStub_ = nullptr;
    }
};

/**
 * @tc.number: SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->SearchElementInfoByAccessibilityId(ELEMENT_ID, REQUEST_ID, nullptr, MODE);
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->SearchElementInfosByText(ELEMENT_ID, TEST, REQUEST_ID, nullptr);
    GTEST_LOG_(INFO) << "SearchElementInfosByText_001 end";
}

/**
 * @tc.number: FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->FindFocusedElementInfo(ELEMENT_ID, FOCUS_TYPE, REQUEST_ID, nullptr);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->FocusMoveSearch(ELEMENT_ID, DIRECTION, REQUEST_ID, nullptr);
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    std::map<std::string, std::string> actionArguments;
    mockStub_->ExecuteAction(ELEMENT_ID, ACTION, actionArguments, REQUEST_ID, nullptr);
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ClearFocus_001
 * @tc.name: ClearFocus
 * @tc.desc: Test function ClearFocus
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, ClearFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ClearFocus_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->ClearFocus();
    GTEST_LOG_(INFO) << "ClearFocus_001 end";
}

/**
 * @tc.number: OutsideTouch_001
 * @tc.name: OutsideTouch
 * @tc.desc: Test function OutsideTouch
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, OutsideTouch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutsideTouch_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    mockStub_->OutsideTouch();
    GTEST_LOG_(INFO) << "OutsideTouch_001 end";
}

/**
 * @tc.number: GetWindowId
 * @tc.name: OutsideTouch
 * @tc.desc: Test function OutsideTouch
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, GetWindowId, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowId start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    EXPECT_EQ(1, mockStub_->GetWindowId());
    GTEST_LOG_(INFO) << "GetWindowId end";
}

/**
 * @tc.number: AddRequest_001
 * @tc.name: AddRequest
 * @tc.desc: Test function AddRequest
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, AddRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddRequest_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback = nullptr;
    callback_.AddRequest(REQUEST_ID, callback);
    GTEST_LOG_(INFO) << "AddRequest_001 end";
}

/**
 * @tc.number: SetSearchElementInfoByAccessibilityIdResult_001
 * @tc.name: SetSearchElementInfoByAccessibilityIdResult
 * @tc.desc: Test function SetSearchElementInfoByAccessibilityIdResult
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SetSearchElementInfoByAccessibilityIdResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    std::list<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    callback_.SetSearchElementInfoByAccessibilityIdResult(infos, REQUEST_ID);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 end";
}

/**
 * @tc.number: SetSearchElementInfoByTextResult_001
 * @tc.name: SetSearchElementInfoByTextResult
 * @tc.desc: Test function SetSearchElementInfoByTextResult
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SetSearchElementInfoByTextResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    std::list<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    callback_.SetSearchElementInfoByTextResult(infos, REQUEST_ID);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 end";
}

/**
 * @tc.number: SetFindFocusedElementInfoResult_001
 * @tc.name: SetFindFocusedElementInfoResult
 * @tc.desc: Test function SetFindFocusedElementInfoResult
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SetFindFocusedElementInfoResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    AccessibilityElementInfo info {};
    callback_.SetFindFocusedElementInfoResult(info, REQUEST_ID);
    GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 end";
}

/**
 * @tc.number: SetFocusMoveSearchResult_001
 * @tc.name: SetFocusMoveSearchResult
 * @tc.desc: Test function SetFocusMoveSearchResult
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SetFocusMoveSearchResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    AccessibilityElementInfo info {};
    callback_.SetFocusMoveSearchResult(info, REQUEST_ID);
    GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 end";
}

/**
 * @tc.number: SetExecuteActionResult_001
 * @tc.name: SetExecuteActionResult
 * @tc.desc: Test function SetExecuteActionResult
 */
HWTEST_F(AccessibilityElementOperatorImplUnitTest, SetExecuteActionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 start";
    if (!mockStub_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilityElementOperatorImpl mockStub_";
        return;
    }
    callback_.SetExecuteActionResult(true, REQUEST_ID);
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 end";
}
} // namespace Accessibility
} // namespace OHOS