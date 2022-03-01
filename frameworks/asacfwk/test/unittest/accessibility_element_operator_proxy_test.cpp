/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <memory>
#include <gtest/gtest.h>
#include "accessibility_element_operator_stub.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_operator.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityInteractionProxyUnitTest : public ::testing::Test {
public:
    sptr<AccessibilityElementOperatorStub> mockStub_ = nullptr;
    sptr<AccessibilityElementOperatorProxy> mockProxy_ = nullptr;
    sptr<IAccessibilityElementOperatorCallback> callback_ = nullptr;
    AccessibilityInteractionProxyUnitTest() {}
    ~AccessibilityInteractionProxyUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyUnitTest SetUp() Start";
        mockStub_ = new AccessibilityElementOperatorStub();
        mockProxy_ = new AccessibilityElementOperatorProxy(mockStub_);
        callback_ = new AccessibilityOperator();
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyUnitTest TearDown()";
        mockStub_ = nullptr;
        mockProxy_ = nullptr;
    }
};

/**
 * @tc.number: SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibilityInteractionProxyUnitTest, SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 start";

    mockProxy_->SearchElementInfoByAccessibilityId(1, 1, callback_, 1);
    GTEST_LOG_(INFO) << "SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibilityInteractionProxyUnitTest, SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SearchElementInfosByText start";

    std::string str = "test";
    mockProxy_->SearchElementInfosByText(1, str, 1, callback_);
    GTEST_LOG_(INFO) << "SearchElementInfosByText end";
}

/**
 * @tc.number: FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibilityInteractionProxyUnitTest, FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 start";

    mockProxy_->FindFocusedElementInfo(1, 1, 1, callback_);
    GTEST_LOG_(INFO) << "FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibilityInteractionProxyUnitTest, FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 start";

    mockProxy_->FocusMoveSearch(1, 1, 1, callback_);
    GTEST_LOG_(INFO) << "FocusMoveSearch_001 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibilityInteractionProxyUnitTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";

    std::map<std::string, std::string> arguments;
    mockProxy_->ExecuteAction(1, 1, arguments, 1, callback_);
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}
} // namespace Accessibility
} // namespace OHOS