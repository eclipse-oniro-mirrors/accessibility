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
#include <optional>
#include <gtest/gtest.h>
#include "accessible_ability_listener.h"
#include "accessibility_element_info.h"
#include "accessible_ability.h"
#include "mock_gesture_simulation.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

static uint32_t GestureSequence = 0;

class UnitTestAccessibleAbilityListener : public AccessibleAbilityListener
{
public:
    UnitTestAccessibleAbilityListener() {}
    virtual ~UnitTestAccessibleAbilityListener() {}

    void OnAbilityConnected() override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAbilityConnected";
    }

    void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAccessibilityEvent";
    }

    void OnGesture(uint32_t gestureId) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnGesture";
    }

    void OnInterrupt() override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnInterrupt";
    }

    bool OnKeyPressEvent(const KeyEvent& keyEvent) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnKeyPressEvent";
        return true;
    }
};

class AccessibleAbilityUnitTest : public ::testing::Test {
public:
    AccessibleAbilityUnitTest() {}
    ~AccessibleAbilityUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibleAbilityUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityUnitTest Start ######################";
}

void AccessibleAbilityUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityUnitTest End ######################";
}

void AccessibleAbilityUnitTest::SetUp()
{}

void AccessibleAbilityUnitTest::TearDown()
{}

/**
 * @tc.number: AccessibleAbility_Unittest_GetInstance_001
 * @tc.name: GetInstance
 * @tc.desc: Test function GetInstance
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetInstance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetInstance_001 start";

    AccessibleAbility::GetInstance();

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetInstance_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_RegisterListener_001
 * @tc.name: RegisterListener
 * @tc.desc: Test function RegisterListener
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_RegisterListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_RegisterListener_001 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    AccessibleAbility::GetInstance().RegisterListener(listener);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_RegisterListener_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_RegisterListener_002
 * @tc.name: RegisterListener
 * @tc.desc: Test function RegisterListener
 *           Register a listener which is null.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_RegisterListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_RegisterListener_002 start";

    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    AccessibleAbility::GetInstance().RegisterListener(listener);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_RegisterListener_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_ConnectToAAMS_001
 * @tc.name: ConnectToAAMS
 * @tc.desc: Test function ConnectToAAMS
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_ConnectToAAMS_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_ConnectToAAMS_001 start";

    AccessibleAbility::GetInstance().ConnectToAAMS();

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_ConnectToAAMS_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_DisableAbility_001
 * @tc.name: DisableAbility
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_DisableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DisableAbility_001 start";

    AccessibleAbility::GetInstance().DisableAbility();

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DisableAbility_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetFocusElementInfo_001
 * @tc.name: GetFocusElementInfo
 * @tc.desc: Test function GetFocusElementInfo
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetFocusElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFocusElementInfo_001 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    bool result = AccessibleAbility::GetInstance().GetFocusElementInfo(FOCUS_TYPE_INPUT, elementInfo);
    EXPECT_TRUE(elementInfo.has_value());
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFocusElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetFocusElementInfo_002
 * @tc.name: GetFocusElementInfo
 * @tc.desc: Test function GetFocusElementInfo
 *           Get focus elementInfo when focusType is not allowed.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetFocusElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFocusElementInfo_002 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    bool result = AccessibleAbility::GetInstance().GetFocusElementInfo(FOCUS_TYPE_INVALID, elementInfo);
    EXPECT_FALSE(elementInfo.has_value());
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFocusElementInfo_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GestureSimulate_001
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture without any path.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GestureSimulate_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_001 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    std::shared_ptr<GestureResultListener> listener = nullptr;
    EXPECT_FALSE(AccessibleAbility::GetInstance().GestureSimulate(gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GestureSimulate_002
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture whose total duration time is not allowed.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GestureSimulate_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_002 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePath.SetDurationTime(gesturePath.GetMaxStrokeDuration() + 1);
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = nullptr;

    EXPECT_FALSE(AccessibleAbility::GetInstance().GestureSimulate(gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GestureSimulate_003
 * @tc.name: GestureSimulate
 * @tc.desc: Test function GestureSimulate
 *           Send a gesture with one path.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GestureSimulate_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_003 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = make_shared<MockGestureResultListener>();
    GestureSequence ++;

    EXPECT_TRUE(AccessibleAbility::GetInstance().GestureSimulate(gesturePathDefineList, listener));

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GestureSimulate_003 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetDisplayResizeController_001
 * @tc.name: GetDisplayResizeController
 * @tc.desc: Test function GetDisplayResizeController
 *           Get a default displayResize controller.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetDisplayResizeController_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetDisplayResizeController_001 start";

    EXPECT_TRUE(AccessibleAbility::GetInstance().GetDisplayResizeController() != nullptr);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetDisplayResizeController_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetDisplayResizeController_002
 * @tc.name: GetDisplayResizeController
 * @tc.desc: Test function GetDisplayResizeController
 *           Get an informal displayResize controller.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetDisplayResizeController_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetDisplayResizeController_002 start";

    uint32_t displayId = 0x2;
    AccessibleAbility::GetInstance().GetDisplayResizeController(displayId);
    EXPECT_TRUE(AccessibleAbility::GetInstance().GetDisplayResizeController(displayId) != nullptr);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetDisplayResizeController_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetRootElementInfo_001
 * @tc.name: GetRootElementInfo
 * @tc.desc: Test function GetRootElementInfo
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetRootElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetRootElementInfo_001 start";

    std::optional<AccessibilityElementInfo> elementInfo;
    AccessibleAbility::GetInstance().GetRootElementInfo(elementInfo);
    EXPECT_TRUE(elementInfo.has_value());

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetRootElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetWindows_001 start";

    EXPECT_EQ(int(AccessibleAbility::GetInstance().GetWindows().size()), 0);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_PerformCommonAction_001
 * @tc.name: PerformCommonAction
 * @tc.desc: Test function PerformCommonAction
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_PerformCommonAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_PerformCommonAction_001 start";

    EXPECT_TRUE(AccessibleAbility::GetInstance().PerformCommonAction(GLOBAL_ACTION_BACK));

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_PerformCommonAction_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_PerformCommonAction_002
 * @tc.name: PerformCommonAction
 * @tc.desc: Test function PerformCommonAction
 *           Perform common action which is not allowed.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_PerformCommonAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_PerformCommonAction_002 start";

    EXPECT_FALSE(AccessibleAbility::GetInstance().PerformCommonAction(0xFFFFFFFF));

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_PerformCommonAction_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_GetFingerprintController_001
 * @tc.name: GetFingerprintController
 * @tc.desc: Test function GetFingerprintController
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_GetFingerprintController_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFingerprintController_001 start";

    EXPECT_TRUE(AccessibleAbility::GetInstance().GetFingerprintController() != nullptr);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_GetFingerprintController_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_001
 * @tc.name: DispatchOnSimulationGestureResult
 * @tc.desc: Test function DispatchOnSimulationGestureResult
 *           Dispatch simulation gesture's result without listener.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_001 start";

    AccessibleAbility::GetInstance().DispatchOnSimulationGestureResult(0, 0);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_001 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_002
 * @tc.name: DispatchOnSimulationGestureResult
 * @tc.desc: Test function DispatchOnSimulationGestureResult
 *           Dispatch simulation gesture's result with listener and the result is completed.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_002 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = std::make_shared<MockGestureResultListener>();
    GestureSequence ++;

    AccessibleAbility::GetInstance().GestureSimulate(gesturePathDefineList, listener);
    AccessibleAbility::GetInstance().DispatchOnSimulationGestureResult(GestureSequence, 1);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_002 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_003
 * @tc.name: DispatchOnSimulationGestureResult
 * @tc.desc: Test function DispatchOnSimulationGestureResult
 *           Dispatch simulation gesture's result with listener and the result is cancelled.
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_003 start";

    std::vector<GesturePathDefine> gesturePathDefineList {};
    GesturePathDefine gesturePath;
    gesturePathDefineList.push_back(gesturePath);
    std::shared_ptr<GestureResultListener> listener = std::make_shared<MockGestureResultListener>();
    GestureSequence ++;

    AccessibleAbility::GetInstance().GestureSimulate(gesturePathDefineList, listener);
    AccessibleAbility::GetInstance().DispatchOnSimulationGestureResult(GestureSequence, 0);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_DispatchOnSimulationGestureResult_003 end";
}

/**
 * @tc.number: AccessibleAbility_Unittest_SetChannelId_001
 * @tc.name: SetChannelId
 * @tc.desc: Test function SetChannelId
 */
HWTEST_F(AccessibleAbilityUnitTest, AccessibleAbility_Unittest_SetChannelId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_SetChannelId_001 start";

    AccessibleAbility::GetInstance().SetChannelId(0);

    GTEST_LOG_(INFO) << "AccessibleAbility_Unittest_SetChannelId_001 end";
}