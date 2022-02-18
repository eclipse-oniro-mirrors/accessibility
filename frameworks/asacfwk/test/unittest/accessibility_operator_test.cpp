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
#include "accessibility_operator.h"
#include "accessible_ability_channel_proxy.h"
#include "accessible_ability_channel_stub.h"
#include "mock_accessible_ability_channel_impl.h"
#include "accessibility_event_info.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityOperatorUnitTest : public ::testing::Test {
public:
    AccessibilityOperatorUnitTest() {}
    ~AccessibilityOperatorUnitTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "AccessibilityOperatorUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "AccessibilityOperatorUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "AccessibilityOperatorUnitTest SetUp()";
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "AccessibilityOperatorUnitTest TearDown()";
    }
};

/**
 * @tc.number: AccessibilityOperator_001
 * @tc.name: Test the function wrapper only in asac.
 * @tc.desc: Test the function wrapper only in asac.
 */
HWTEST_F(AccessibilityOperatorUnitTest, AccessibilityOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChannel_001 start";
    sptr<IAccessibleAbilityChannel> service = new MockAccessibleAbilityChannel();
    AccessibilityOperator instance = AccessibilityOperator::GetInstance();

    GTEST_LOG_(INFO) << "AddChannel 1";
    instance.AddChannel(1, service);
    GTEST_LOG_(INFO) << "AddChannel OK";

    GTEST_LOG_(INFO) << "GetChannel start";
    EXPECT_TRUE(instance.GetChannel(1) != nullptr);

    GTEST_LOG_(INFO) << "AddChannel 2";
    instance.AddChannel(2, service);

    GTEST_LOG_(INFO) << "RemoveChannel 2";
    instance.RemoveChannel(2);

    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult start";
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    info.SetWindowId(100);
    infos.push_back(info);
    instance.SetSearchElementInfoByAccessibilityIdResult(infos, 1);

    GTEST_LOG_(INFO) << "SetOnKeyPressEventResult start";
    instance.SetOnKeyPressEventResult(1, true, 1);

    GTEST_LOG_(INFO) << "GetDisplayResizeScale start";
    instance.GetDisplayResizeScale(1, 1);

    GTEST_LOG_(INFO) << "GetDisplayResizeCenterX start";
    instance.GetDisplayResizeCenterX(1, 1);

    GTEST_LOG_(INFO) << "GetDisplayResizeCenterY start";
    instance.GetDisplayResizeCenterY(1, 1);

    GTEST_LOG_(INFO) << "GetDisplayResizeRect start";
    instance.GetDisplayResizeRect(1, 1);

    GTEST_LOG_(INFO) << "ResetDisplayResize start";
    instance.ResetDisplayResize(1, 1, true);

    GTEST_LOG_(INFO) << "SetDisplayResizeScaleAndCenter start";
    instance.SetDisplayResizeScaleAndCenter(1, 1, 1, 1, 1, true);

    GTEST_LOG_(INFO) << "SendSimulateGesture start";
    std::vector<GesturePathDefine> gestureSteps{};
    instance.SendSimulateGesture(1, 1, gestureSteps);

}

/**
 * @tc.number: AccessibilityOperator_002
 * @tc.name: aams interaction for node info function,
 * @tc.desc: aams interaction for node info function,include element/eventInfo/window
 */
HWTEST_F(AccessibilityOperatorUnitTest, AccessibilityOperator_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChannel_001 start";
    sptr<IAccessibleAbilityChannel> service = new MockAccessibleAbilityChannel();
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();

    if (instance == nullptr) {
        GTEST_LOG_(INFO) << "Cann't get  AccessibilityOperator instance";
        return;
    }

    GTEST_LOG_(INFO) << "AddChannel 1";
    instance->AddChannel(1, service);
    GTEST_LOG_(INFO) << "AddChannel OK";

    GTEST_LOG_(INFO) << "GetChannel start";
    EXPECT_TRUE(instance->GetChannel(1) != nullptr);

    GTEST_LOG_(INFO) << "SetSearchElementInfoByAccessibilityIdResult start";
    std::vector<AccessibilityElementInfo> infos;
    AccessibilityElementInfo info {};
    infos.push_back(info);
    instance->SetSearchElementInfoByAccessibilityIdResult(infos, 1);

    GTEST_LOG_(INFO) << "GetRoot start";
    instance->GetRoot(1, info);
    GTEST_LOG_(INFO) << "GetRoot end";

    GTEST_LOG_(INFO) << "GetWindows start";
    instance->GetWindows(1);

    GTEST_LOG_(INFO) << "SetSearchElementInfoByTextResult start";
    instance->SetSearchElementInfoByTextResult(infos, 2);

    GTEST_LOG_(INFO) << "SearchElementInfosByText start";
    instance->SearchElementInfosByText(1, 1, 1, "test", infos);

    GTEST_LOG_(INFO) << "SetFindFocusedElementInfoResult start";
    instance->SetFindFocusedElementInfoResult(info, 3);

    GTEST_LOG_(INFO) << "FindFocusedElementInfo start";
    instance->FindFocusedElementInfo(1, 1, 1, 1, info);

    GTEST_LOG_(INFO) << "SetFocusMoveSearchResult start";
    instance->SetFocusMoveSearchResult(info, 4);

    GTEST_LOG_(INFO) << "FocusMoveSearch start";
    instance->FocusMoveSearch(1, 1, 1, 1, info);

    std::map<std::string, std::string> actionArguments;
    GTEST_LOG_(INFO) << "SetExecuteActionResult start";
    instance->SetExecuteActionResult(true, 5);

    GTEST_LOG_(INFO) << "ExecuteAction start";
    instance->ExecuteAction(1, 1, 1, 1, actionArguments);

    EXPECT_TRUE(instance->ExecuteCommonAction(1, 1));

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetSource_001 start";
    AccessibilityMemo record {};
    record.SetChannelId(1);
    record.GetSource(info);
    GTEST_LOG_(INFO) << "GetSource_001 end";

    AccessibilityElementInfo element {};
    element.SetChannelId(1);

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetFocus start";
    element.GetFocus(1, info);
    GTEST_LOG_(INFO) << "GetFocus end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetNext start";
    FocusMoveDirection direction = FocusMoveDirection::UP;
    element.GetNext(direction, info);
    GTEST_LOG_(INFO) << "GetNext end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetByContent start";
    string str = "text";
    element.GetByContent(str, infos);
    GTEST_LOG_(INFO) << "GetByContent end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetParent start";
    element.GetParent(element);
    GTEST_LOG_(INFO) << "GetParent end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetLabeled start";
    element.GetLabeled(element);
    GTEST_LOG_(INFO) << "GetLabeled end";

    //Dont' remove the instance
    AccessibilityWindowInfo window {};
    window.SetChannelId(1);
    GTEST_LOG_(INFO) << "GetRootAccessibilityInfo start";
    window.GetRootAccessibilityInfo(element);
    GTEST_LOG_(INFO) << "GetRootAccessibilityInfo end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetAnchor start";
    window.GetAnchor(element);
    GTEST_LOG_(INFO) << "GetAnchor end";

    //Dont' remove the instance
    GTEST_LOG_(INFO) << "GetParent start";
    window.GetParent();
    GTEST_LOG_(INFO) << "GetParent end";

    GTEST_LOG_(INFO) << "Operator_001 end";
}

} // namespace Accessibility
} // namespace OHOS