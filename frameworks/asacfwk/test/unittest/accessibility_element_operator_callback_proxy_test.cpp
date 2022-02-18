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

#include <gtest/gtest.h>
#include <memory>

#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_operator.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityInteractionProxyCallbackUnitTest : public ::testing::Test {
public:
    sptr<AccessibilityOperator> mockStub_ = nullptr;
    sptr<AccessibilityElementOperatorCallbackProxy> mockProxy_ = nullptr;
    AccessibilityInteractionProxyCallbackUnitTest() {}
    ~AccessibilityInteractionProxyCallbackUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyCallbackUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyCallbackUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyCallbackUnitTest SetUp() Start";
        mockStub_ = new AccessibilityOperator();
        mockProxy_ = new AccessibilityElementOperatorCallbackProxy(mockStub_);
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyCallbackUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityInteractionProxyCallbackUnitTest TearDown()";
        mockStub_ = nullptr;
        mockProxy_ = nullptr;
    }
};

/**
 * @tc.number: SetSearchElementInfoByAccessibilityIdResult_001
 * @tc.name: SetSearchElementInfoByAccessibilityIdResult
 * @tc.desc: Test function SetSearchElementInfoByAccessibilityIdResult
 */
HWTEST_F(AccessibilityInteractionProxyCallbackUnitTest,
    SetSearchElementInfoByAccessibilityIdResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    mockProxy_->SetSearchElementInfoByAccessibilityIdResult(infos, 1);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult_001 end";
}

/**
 * @tc.number: SetSearchElementInfoByTextResult_001
 * @tc.name: SetSearchElementInfoByTextResult
 * @tc.desc: Test function SetSearchElementInfoByTextResult
 */
HWTEST_F(AccessibilityInteractionProxyCallbackUnitTest, SetSearchElementInfoByTextResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 start";
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    mockProxy_->SetSearchElementInfoByTextResult(infos, 1);
    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult_001 end";
}

/**
 * @tc.number: SetFindFocusedElementInfoResult_001
 * @tc.name: SetFindFocusedElementInfoResult
 * @tc.desc: Test function SetFindFocusedElementInfoResult
 */
// HWTEST_F(AccessibilityInteractionProxyCallbackUnitTest, SetFindFocusedElementInfoResult_001, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 start";
//     AccessibilityElementInfo info {};
//     mockProxy_->SetFindFocusedElementInfoResult(info, 1);
//     GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult_001 end";
// }

/**
 * @tc.number: SetFocusMoveSearchResult_001
 * @tc.name: SetFocusMoveSearchResult
 * @tc.desc: Test function SetFocusMoveSearchResult
 */
// HWTEST_F(AccessibilityInteractionProxyCallbackUnitTest, SetFocusMoveSearchResult_001, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 start";
//     AccessibilityElementInfo info {};
//     mockProxy_->SetFocusMoveSearchResult(info, 1);
//     GTEST_LOG_(INFO) << "SetFocusMoveSearchResult_001 end";
// }

/**
 * @tc.number: SetExecuteActionResult_001
 * @tc.name: SetExecuteActionResult
 * @tc.desc: Test function SetExecuteActionResult
 */
HWTEST_F(AccessibilityInteractionProxyCallbackUnitTest, SetExecuteActionResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 start";
    mockProxy_->SetExecuteActionResult(true, 1);
    GTEST_LOG_(INFO) << "SetExecuteActionResult_001 end";
}

} // namespace Accessibility
} // namespace OHOS