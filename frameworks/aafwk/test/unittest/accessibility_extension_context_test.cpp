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
#include <optional>
#include <memory>
#include "accessibility_element_info.h"
#include "accessibility_extension_context.h"
#include "accessibility_extension.h"
#include "mock_gesture_simulation.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityExtensionContextUnitTest : public ::testing::Test {
public:
    AccessibilityExtensionContextUnitTest() {}
    ~AccessibilityExtensionContextUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityExtensionContext> context_ = nullptr;
};

void AccessibilityExtensionContextUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityExtensionContextUnitTest Start ######################";
}

void AccessibilityExtensionContextUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityExtensionContextUnitTest End ######################";
}

void AccessibilityExtensionContextUnitTest::SetUp()
{
    context_ = std::make_shared<AccessibilityExtensionContext>();
}

void AccessibilityExtensionContextUnitTest::TearDown()
{
    context_ = nullptr;
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetFocusElementInfo_001
 * @tc.name: GetFocusElementInfo
 * @tc.desc: Test function GetFocusElementInfo
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GetFocusElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetFocusElementInfo_001 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    bool result = context_->GetFocusElementInfo(FOCUS_TYPE_INPUT, elementInfo);
    EXPECT_TRUE(elementInfo.has_value());
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetFocusElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetFocusElementInfo_002
 * @tc.name: GetFocusElementInfo
 * @tc.desc: Test function GetFocusElementInfo
 *           Get focus elementInfo when focusType is not allowed.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GetFocusElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetFocusElementInfo_002 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    bool result = context_->GetFocusElementInfo(FOCUS_TYPE_INVALID, elementInfo);
    EXPECT_FALSE(elementInfo.has_value());
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetFocusElementInfo_002 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GestureSimulate_001
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture without any path.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GestureSimulate_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_001 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    std::shared_ptr<GestureResultListener> listener = nullptr;
    EXPECT_FALSE(context_->GestureSimulate(0, gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GestureSimulate_002
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture whose total duration time is not allowed.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GestureSimulate_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_002 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePath.SetDurationTime(gesturePath.GetMaxStrokeDuration() + 1);
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = nullptr;

    EXPECT_FALSE(context_->GestureSimulate(0, gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_002 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GestureSimulate_003
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture with one path.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GestureSimulate_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_003 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = make_shared<MockGestureResultListener>();

    EXPECT_TRUE(context_->GestureSimulate(0, gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GestureSimulate_003 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetDisplayResizeController_001
 * @tc.name: GetDisplayResizeController
 * @tc.desc: Test function GetDisplayResizeController
 *           Get a default displayResize controller.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GetDisplayResizeController_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetDisplayResizeController_001 start";

    EXPECT_TRUE(context_->GetDisplayResizeController() != nullptr);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetDisplayResizeController_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetDisplayResizeController_002
 * @tc.name: GetDisplayResizeController
 * @tc.desc: Test function GetDisplayResizeController
 *           Get an informal displayResize controller.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GetDisplayResizeController_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetDisplayResizeController_002 start";

    uint32_t displayId = 0x2;
    context_->GetDisplayResizeController(displayId);
    EXPECT_TRUE(context_->GetDisplayResizeController(displayId) != nullptr);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetDisplayResizeController_002 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetRootElementInfo_001
 * @tc.name: GetRootElementInfo
 * @tc.desc: Test function GetRootElementInfo
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_GetRootElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetRootElementInfo_001 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    context_->GetRootElementInfo(elementInfo);
    EXPECT_TRUE(elementInfo.has_value());

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetRootElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibilityExtensionContextUnitTest, AccessibilityExtensionContext_Unittest_GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetWindows_001 start";

    EXPECT_EQ(int(context_->GetWindows().size()), 0);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_PerformCommonAction_001
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_PerformCommonAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_PerformCommonAction_001 start";

    EXPECT_TRUE(context_->ExecuteCommonAction(GLOBAL_ACTION_BACK));

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_PerformCommonAction_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_PerformCommonAction_002
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 *           Perform common action which is not allowed.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_PerformCommonAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_PerformCommonAction_002 start";

    EXPECT_FALSE(context_->ExecuteCommonAction(0xFFFFFFFF));

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_PerformCommonAction_002 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_001
 * @tc.name: DispatchOnSimulationGestureResult
 * @tc.desc: Test function DispatchOnSimulationGestureResult
 *           Dispatch simulation gesture's result without listener.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_001 start";

    context_->DispatchOnSimulationGestureResult(0, 0);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_001 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_002
 * @tc.name: DispatchOnSimulationGestureResult
 * @tc.desc: Test function DispatchOnSimulationGestureResult
 *           Dispatch simulation gesture's result with listener and the result is completed.
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_002 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = std::make_shared<MockGestureResultListener>();
    uint32_t sequence = 0;

    context_->GestureSimulate(sequence, gesturePathDefineList, listener);
    context_->DispatchOnSimulationGestureResult(sequence, 1);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_DispatchOnSimulationGestureResult_002 end";
}

/**
 * @tc.number: AccessibilityExtensionContext_Unittest_SetChannelId_001
 * @tc.name: SetChannelId
 * @tc.desc: Test function SetChannelId
 */
HWTEST_F(AccessibilityExtensionContextUnitTest,
    AccessibilityExtensionContext_Unittest_SetChannelId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_SetChannelId_001 start";

    context_->SetChannelId(0);

    GTEST_LOG_(INFO) << "AccessibilityExtensionContext_Unittest_SetChannelId_001 end";
}
} // namespace Accessibility
} // namespace OHOS