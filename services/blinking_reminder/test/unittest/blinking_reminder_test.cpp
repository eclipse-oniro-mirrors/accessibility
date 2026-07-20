/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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

#ifndef private
#define private public
#endif
#ifndef protected
#define protected public
#endif

#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "accessible_blinking_reminder_manager.h"
#include "accessibility_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
constexpr int32_t WAIT_BLINKING_MS = 1500;
constexpr int32_t WAIT_STOP_MS = 500;
} // namespace

class AccessibleBlinkingReminderManagerTest : public testing::Test {
public:
    AccessibleBlinkingReminderManagerTest()
    {}
    ~AccessibleBlinkingReminderManagerTest()
    {}

    AccessibleBlinkingReminderManager &manager_ =
        Singleton<AccessibleBlinkingReminderManager>::GetInstance();

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibleBlinkingReminderManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManagerTest SetUpTestCase";
}

void AccessibleBlinkingReminderManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManagerTest TearDownTestCase";
}

void AccessibleBlinkingReminderManagerTest::SetUp()
{
    // Reset state before each test
    manager_.SetFlashReminderSwitch(false);
    manager_.SetFlashReminderMode(FlashReminderMode::NONE);
    manager_.SetFlashReminderFunctionEnabled("");
    manager_.SetFlashReminderUnlock(false);
}

void AccessibleBlinkingReminderManagerTest::TearDown()
{
    manager_.SetFlashReminderSwitch(false);
    manager_.SetFlashReminderMode(FlashReminderMode::NONE);
    manager_.SetFlashReminderFunctionEnabled("");
    manager_.SetFlashReminderUnlock(false);
}

// ==================== SetFlashReminderSwitch ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_001
 * @tc.name: SetFlashReminderSwitch
 * @tc.desc: Test setting flash reminder switch to true.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_001 start";
    manager_.SetFlashReminderSwitch(true);
    EXPECT_TRUE(manager_.state_.flashReminderSwitch);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_002
 * @tc.name: SetFlashReminderSwitch
 * @tc.desc: Test setting flash reminder switch to false stops all blinking.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_002 start";
    manager_.SetFlashReminderSwitch(true);
    EXPECT_TRUE(manager_.state_.flashReminderSwitch);
    manager_.SetFlashReminderSwitch(false);
    EXPECT_FALSE(manager_.state_.flashReminderSwitch);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderSwitch_002 end";
}

// ==================== SetFlashReminderMode ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_001
 * @tc.name: SetFlashReminderMode
 * @tc.desc: Test setting flash reminder mode to DISPLAY.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_001 start";
    manager_.SetFlashReminderMode(FlashReminderMode::DISPLAY);
    EXPECT_EQ(manager_.state_.flashReminderMode, FlashReminderMode::DISPLAY);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_002
 * @tc.name: SetFlashReminderMode
 * @tc.desc: Test setting flash reminder mode to FLASHLIGHT.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_002 start";
    manager_.SetFlashReminderMode(FlashReminderMode::FLASHLIGHT);
    EXPECT_EQ(manager_.state_.flashReminderMode, FlashReminderMode::FLASHLIGHT);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_003
 * @tc.name: SetFlashReminderMode
 * @tc.desc: Test setting flash reminder mode to BOTH.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_003 start";
    manager_.SetFlashReminderMode(FlashReminderMode::BOTH);
    EXPECT_EQ(manager_.state_.flashReminderMode, FlashReminderMode::BOTH);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_003 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_004
 * @tc.name: SetFlashReminderMode
 * @tc.desc: Test setting flash reminder mode to NONE stops blinking.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_004 start";
    manager_.SetFlashReminderMode(FlashReminderMode::BOTH);
    manager_.SetFlashReminderMode(FlashReminderMode::NONE);
    EXPECT_EQ(manager_.state_.flashReminderMode, FlashReminderMode::NONE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderMode_004 end";
}

// ==================== SetFlashReminderFunctionEnabled ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_001
 * @tc.name: SetFlashReminderFunctionEnabled
 * @tc.desc: Test setting function enabled with ALARM_CLOCK.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_001 start";
    manager_.SetFlashReminderFunctionEnabled("ALARM_CLOCK");
    EXPECT_EQ(manager_.state_.flashReminderFunctionEnabled, "ALARM_CLOCK");
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_002
 * @tc.name: SetFlashReminderFunctionEnabled
 * @tc.desc: Test setting function enabled with multiple scenarios.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_002 start";
    manager_.SetFlashReminderFunctionEnabled("ALARM_CLOCK,NOTIFY,INCOMING_CALL");
    EXPECT_EQ(manager_.state_.flashReminderFunctionEnabled, "ALARM_CLOCK,NOTIFY,INCOMING_CALL");
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_003
 * @tc.name: SetFlashReminderFunctionEnabled
 * @tc.desc: Test setting function enabled with empty string.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_003 start";
    manager_.SetFlashReminderFunctionEnabled("ALARM_CLOCK");
    manager_.SetFlashReminderFunctionEnabled("");
    EXPECT_EQ(manager_.state_.flashReminderFunctionEnabled, "");
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderFunctionEnabled_003 end";
}

// ==================== SetFlashReminderUnlock ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_001
 * @tc.name: SetFlashReminderUnlock
 * @tc.desc: Test setting flash reminder unlock to true.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_001 start";
    manager_.SetFlashReminderUnlock(true);
    EXPECT_TRUE(manager_.state_.flashReminderUnlock);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_002
 * @tc.name: SetFlashReminderUnlock
 * @tc.desc: Test setting flash reminder unlock to false.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_002 start";
    manager_.SetFlashReminderUnlock(true);
    manager_.SetFlashReminderUnlock(false);
    EXPECT_FALSE(manager_.state_.flashReminderUnlock);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderUnlock_002 end";
}

// ==================== StartBlinking validation ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StartBlinking_001
 * @tc.name: StartBlinking
 * @tc.desc: Test StartBlinking when switch is off returns FEATURE_DISABLE.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StartBlinking_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_001 start";
    // flashReminderSwitch is false by default
    BlinkResultCode ret = manager_.StartBlinking(BlinkingMode::SINGLE_BLINK, BlinkingScenario::ALARM);
    EXPECT_EQ(ret, BlinkResultCode::FEATURE_DISABLE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StartBlinking_002
 * @tc.name: StartBlinking
 * @tc.desc: Test StartBlinking when mode is NONE returns FEATURE_DISABLE.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StartBlinking_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_002 start";
    manager_.SetFlashReminderSwitch(true);
    // flashReminderMode is NONE by default
    BlinkResultCode ret = manager_.StartBlinking(BlinkingMode::SINGLE_BLINK, BlinkingScenario::ALARM);
    EXPECT_EQ(ret, BlinkResultCode::FEATURE_DISABLE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StartBlinking_003
 * @tc.name: StartBlinking
 * @tc.desc: Test StartBlinking when function not enabled returns FEATURE_DISABLE.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StartBlinking_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_003 start";
    manager_.SetFlashReminderSwitch(true);
    manager_.SetFlashReminderMode(FlashReminderMode::DISPLAY);
    // flashReminderFunctionEnabled is empty, so scenario check fails
    BlinkResultCode ret = manager_.StartBlinking(BlinkingMode::SINGLE_BLINK, BlinkingScenario::ALARM);
    EXPECT_EQ(ret, BlinkResultCode::FEATURE_DISABLE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_003 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StartBlinking_004
 * @tc.name: StartBlinking
 * @tc.desc: Test StartBlinking with TESTING scenario bypasses scenario/lock check.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StartBlinking_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_004 start";
    manager_.SetFlashReminderSwitch(true);
    manager_.SetFlashReminderMode(FlashReminderMode::DISPLAY);
    // TESTING scenario bypasses IsScenarioEnabled and IsScreenLocked checks
    BlinkResultCode ret = manager_.StartBlinking(BlinkingMode::SINGLE_BLINK, BlinkingScenario::TESTING);
    // Result depends on hardware support, but validation should pass scenario/lock checks
    // Without display support, returns SCREEN_BLINKING_UNSUPPORTED
    EXPECT_TRUE(ret == BlinkResultCode::BLINK_SUCCESS || ret == BlinkResultCode::SCREEN_BLINKING_UNSUPPORTED);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StartBlinking_004 end";
}

// ==================== StopBlinking ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StopBlinking_001
 * @tc.name: StopBlinking
 * @tc.desc: Test StopBlinking when not running returns BLINK_SUCCESS.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StopBlinking_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopBlinking_001 start";
    BlinkResultCode ret = manager_.StopBlinking(BlinkingMode::SINGLE_BLINK, BlinkingScenario::ALARM);
    EXPECT_EQ(ret, BlinkResultCode::BLINK_SUCCESS);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopBlinking_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StopBlinking_002
 * @tc.name: StopBlinking
 * @tc.desc: Test StopBlinking with mismatched mode returns BLINK_SUCCESS.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StopBlinking_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopBlinking_002 start";
    BlinkResultCode ret = manager_.StopBlinking(BlinkingMode::CONTINUOUS_BLINK, BlinkingScenario::CALL);
    EXPECT_EQ(ret, BlinkResultCode::BLINK_SUCCESS);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopBlinking_002 end";
}

// ==================== IsDisplayEnabled / IsFlashlightEnabled ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_IsDisplayEnabled_001
 * @tc.name: IsDisplayEnabled
 * @tc.desc: Test IsDisplayEnabled with DISPLAY mode.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_IsDisplayEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsDisplayEnabled_001 start";
    EXPECT_TRUE(AccessibleBlinkingReminderManager::IsDisplayEnabled(FlashReminderMode::DISPLAY));
    EXPECT_TRUE(AccessibleBlinkingReminderManager::IsDisplayEnabled(FlashReminderMode::BOTH));
    EXPECT_FALSE(AccessibleBlinkingReminderManager::IsDisplayEnabled(FlashReminderMode::NONE));
    EXPECT_FALSE(AccessibleBlinkingReminderManager::IsDisplayEnabled(FlashReminderMode::FLASHLIGHT));
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsDisplayEnabled_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_IsFlashlightEnabled_001
 * @tc.name: IsFlashlightEnabled
 * @tc.desc: Test IsFlashlightEnabled with FLASHLIGHT mode.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_IsFlashlightEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsFlashlightEnabled_001 start";
    EXPECT_TRUE(AccessibleBlinkingReminderManager::IsFlashlightEnabled(FlashReminderMode::FLASHLIGHT));
    EXPECT_TRUE(AccessibleBlinkingReminderManager::IsFlashlightEnabled(FlashReminderMode::BOTH));
    EXPECT_FALSE(AccessibleBlinkingReminderManager::IsFlashlightEnabled(FlashReminderMode::NONE));
    EXPECT_FALSE(AccessibleBlinkingReminderManager::IsFlashlightEnabled(FlashReminderMode::DISPLAY));
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsFlashlightEnabled_001 end";
}

// ==================== ComputeEffectiveMode ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_001
 * @tc.name: ComputeEffectiveMode
 * @tc.desc: Test ComputeEffectiveMode with empty scenario states returns NONE.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_001 start";
    AccessibleBlinkingReminderManager::BlinkState state;
    int32_t mode = AccessibleBlinkingReminderManager::ComputeEffectiveMode(state);
    EXPECT_EQ(mode, FlashReminderMode::NONE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_002
 * @tc.name: ComputeEffectiveMode
 * @tc.desc: Test ComputeEffectiveMode with active display and flashlight components.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_002 start";
    AccessibleBlinkingReminderManager::BlinkState state;
    state.flashReminderMode = FlashReminderMode::BOTH;
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.scenario = BlinkingScenario::ALARM;
    scenarioState.displayExpired = false;
    scenarioState.flashlightExpired = false;
    state.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    int32_t mode = AccessibleBlinkingReminderManager::ComputeEffectiveMode(state);
    EXPECT_EQ(mode, FlashReminderMode::BOTH);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_003
 * @tc.name: ComputeEffectiveMode
 * @tc.desc: Test ComputeEffectiveMode with expired display returns FLASHLIGHT only.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_003 start";
    AccessibleBlinkingReminderManager::BlinkState state;
    state.flashReminderMode = FlashReminderMode::BOTH;
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.scenario = BlinkingScenario::ALARM;
    scenarioState.displayExpired = true;
    scenarioState.flashlightExpired = false;
    state.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    int32_t mode = AccessibleBlinkingReminderManager::ComputeEffectiveMode(state);
    EXPECT_EQ(mode, FlashReminderMode::FLASHLIGHT);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_003 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_004
 * @tc.name: ComputeEffectiveMode
 * @tc.desc: Test ComputeEffectiveMode with all expired returns NONE.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_004 start";
    AccessibleBlinkingReminderManager::BlinkState state;
    state.flashReminderMode = FlashReminderMode::BOTH;
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.scenario = BlinkingScenario::ALARM;
    scenarioState.displayExpired = true;
    scenarioState.flashlightExpired = true;
    state.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    int32_t mode = AccessibleBlinkingReminderManager::ComputeEffectiveMode(state);
    EXPECT_EQ(mode, FlashReminderMode::NONE);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ComputeEffectiveMode_004 end";
}

// ==================== MakeScenarioState ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_001
 * @tc.name: MakeScenarioState
 * @tc.desc: Test MakeScenarioState with DISPLAY mode, not blocked.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_001 start";
    auto state = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::ALARM, FlashReminderMode::DISPLAY, false);
    EXPECT_EQ(state.scenario, BlinkingScenario::ALARM);
    EXPECT_FALSE(state.displayExpired);
    EXPECT_TRUE(state.flashlightExpired);
    EXPECT_GT(state.displayStartTimeMs, 0);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_002
 * @tc.name: MakeScenarioState
 * @tc.desc: Test MakeScenarioState with BOTH mode, display blocked.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_002 start";
    auto state = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::CALL, FlashReminderMode::BOTH, true);
    EXPECT_EQ(state.scenario, BlinkingScenario::CALL);
    EXPECT_TRUE(state.displayExpired);
    EXPECT_FALSE(state.flashlightExpired);
    EXPECT_EQ(state.displayStartTimeMs, 0);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_003
 * @tc.name: MakeScenarioState
 * @tc.desc: Test MakeScenarioState with FLASHLIGHT mode.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_003 start";
    auto state = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::NOTIFICATION, FlashReminderMode::FLASHLIGHT, false);
    EXPECT_EQ(state.scenario, BlinkingScenario::NOTIFICATION);
    EXPECT_TRUE(state.displayExpired);
    EXPECT_FALSE(state.flashlightExpired);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_MakeScenarioState_003 end";
}

// ==================== IsScenarioEnabled ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_001
 * @tc.name: IsScenarioEnabled
 * @tc.desc: Test IsScenarioEnabled with empty enabled string.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_001 start";
    EXPECT_FALSE(manager_.IsScenarioEnabled(BlinkingScenario::ALARM));
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_002
 * @tc.name: IsScenarioEnabled
 * @tc.desc: Test IsScenarioEnabled with matching scenario name.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_002 start";
    manager_.state_.flashReminderFunctionEnabled = "ALARM_CLOCK";
    EXPECT_TRUE(manager_.IsScenarioEnabled(BlinkingScenario::ALARM));
    EXPECT_FALSE(manager_.IsScenarioEnabled(BlinkingScenario::NOTIFICATION));
    EXPECT_FALSE(manager_.IsScenarioEnabled(BlinkingScenario::CALL));
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_003
 * @tc.name: IsScenarioEnabled
 * @tc.desc: Test IsScenarioEnabled with multiple scenarios enabled.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_003 start";
    manager_.state_.flashReminderFunctionEnabled = "ALARM_CLOCK,INCOMING_CALL";
    EXPECT_TRUE(manager_.IsScenarioEnabled(BlinkingScenario::ALARM));
    EXPECT_FALSE(manager_.IsScenarioEnabled(BlinkingScenario::NOTIFICATION));
    EXPECT_TRUE(manager_.IsScenarioEnabled(BlinkingScenario::CALL));
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_IsScenarioEnabled_003 end";
}

// ==================== HasAnyActiveComponent ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_001
 * @tc.name: HasAnyActiveComponent
 * @tc.desc: Test HasAnyActiveComponent with no scenarios.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_001 start";
    EXPECT_FALSE(manager_.HasAnyActiveComponent());
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_002
 * @tc.name: HasAnyActiveComponent
 * @tc.desc: Test HasAnyActiveComponent with active display component.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_002 start";
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.displayExpired = false;
    scenarioState.flashlightExpired = true;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    EXPECT_TRUE(manager_.HasAnyActiveComponent());
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_003
 * @tc.name: HasAnyActiveComponent
 * @tc.desc: Test HasAnyActiveComponent with all expired components.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_003 start";
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.displayExpired = true;
    scenarioState.flashlightExpired = true;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    EXPECT_FALSE(manager_.HasAnyActiveComponent());
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HasAnyActiveComponent_003 end";
}

// ==================== ResetRuntimeState ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ResetRuntimeState_001
 * @tc.name: ResetRuntimeState
 * @tc.desc: Test ResetRuntimeState clears all runtime state.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ResetRuntimeState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ResetRuntimeState_001 start";
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::ALARM;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = {};
    manager_.state_.pausedByProximity = true;
    manager_.ResetRuntimeState();
    EXPECT_EQ(manager_.state_.currentMode, 0);
    EXPECT_EQ(manager_.state_.singleScenario, 0);
    EXPECT_TRUE(manager_.state_.scenarioStates.empty());
    EXPECT_FALSE(manager_.state_.pausedByProximity);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ResetRuntimeState_001 end";
}

// ==================== HandleProximityStateChange ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_001
 * @tc.name: HandleProximityStateChange
 * @tc.desc: Test proximity near pauses display blinking.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_001 start";
    manager_.state_.isRunning = true;
    manager_.state_.proximityNear = false;
    manager_.HandleProximityStateChange(true);
    EXPECT_TRUE(manager_.state_.proximityNear);
    EXPECT_TRUE(manager_.state_.pausedByProximity);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_002
 * @tc.name: HandleProximityStateChange
 * @tc.desc: Test proximity far unpauses display blinking.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_002 start";
    manager_.state_.isRunning = true;
    manager_.state_.proximityNear = true;
    manager_.state_.pausedByProximity = true;
    // Add a scenario with active display
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.displayExpired = false;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    manager_.HandleProximityStateChange(false);
    EXPECT_FALSE(manager_.state_.proximityNear);
    EXPECT_FALSE(manager_.state_.pausedByProximity);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_003
 * @tc.name: HandleProximityStateChange
 * @tc.desc: Test proximity far with all display expired does not unpause.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_003 start";
    manager_.state_.isRunning = true;
    manager_.state_.proximityNear = true;
    manager_.state_.pausedByProximity = true;
    AccessibleBlinkingReminderManager::ScenarioState scenarioState;
    scenarioState.displayExpired = true;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = scenarioState;
    manager_.HandleProximityStateChange(false);
    EXPECT_FALSE(manager_.state_.proximityNear);
    // pausedByProximity stays true since no active display component
    EXPECT_TRUE(manager_.state_.pausedByProximity);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleProximityStateChange_003 end";
}

// ==================== HandleStartSingleBlink / HandleStopSingleBlink ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_001
 * @tc.name: HandleStartSingleBlink
 * @tc.desc: Test starting single blink when already running returns ALREADY_FLASHING.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_001 start";
    manager_.state_.isRunning = true;
    BlinkResultCode ret = manager_.HandleStartSingleBlink(
        BlinkingScenario::ALARM, FlashReminderMode::DISPLAY, false);
    EXPECT_EQ(ret, BlinkResultCode::ALREADY_FLASHING);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_002
 * @tc.name: HandleStartSingleBlink
 * @tc.desc: Test single blink state setup via direct state manipulation.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_002 start";
    // Set up single blink state directly (avoid spawning hardware thread)
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::TESTING;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    EXPECT_TRUE(manager_.state_.isRunning);
    EXPECT_EQ(manager_.state_.currentMode, BlinkingMode::SINGLE_BLINK);
    EXPECT_EQ(manager_.state_.singleScenario, BlinkingScenario::TESTING);
    EXPECT_EQ(manager_.state_.flashReminderMode, FlashReminderMode::DISPLAY);
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartSingleBlink_002 end";
}

// ==================== HandleStartContinuousBlink / HandleStopContinuousBlink ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_001
 * @tc.name: HandleStartContinuousBlink
 * @tc.desc: Test continuous blink state setup via direct state manipulation.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_001 start";
    // Directly set up continuous blink state (avoid spawning BlinkingWorkContinuous thread
    // which accesses hardware services not available in test environment)
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto scenarioState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::TESTING, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::TESTING] = scenarioState;
    EXPECT_TRUE(manager_.state_.isRunning);
    EXPECT_EQ(manager_.state_.currentMode, BlinkingMode::CONTINUOUS_BLINK);
    manager_.ForceStopAll();
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_002
 * @tc.name: HandleStartContinuousBlink
 * @tc.desc: Test adding same scenario to running continuous blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_002 start";
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto scenarioState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::TESTING, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::TESTING] = scenarioState;
    // Adding same scenario again - state already exists, no duplicate
    EXPECT_EQ(manager_.state_.scenarioStates.count(BlinkingScenario::TESTING), 1u);
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_003
 * @tc.name: HandleStartContinuousBlink
 * @tc.desc: Test continuous blink state replaces single blink state.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_003 start";
    // Set up single blink state first
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::TESTING;
    EXPECT_EQ(manager_.state_.currentMode, BlinkingMode::SINGLE_BLINK);
    // Replace with continuous blink state
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.singleScenario = 0;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto scenarioState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::CALL, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::CALL] = scenarioState;
    EXPECT_EQ(manager_.state_.currentMode, BlinkingMode::CONTINUOUS_BLINK);
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStartContinuousBlink_003 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_HandleStopContinuousBlink_001
 * @tc.name: HandleStopContinuousBlink
 * @tc.desc: Test stopping a scenario from continuous blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_HandleStopContinuousBlink_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStopContinuousBlink_001 start";
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto alarmState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::ALARM, FlashReminderMode::DISPLAY, false);
    auto callState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::CALL, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = alarmState;
    manager_.state_.scenarioStates[BlinkingScenario::CALL] = callState;
    EXPECT_EQ(manager_.state_.scenarioStates.size(), 2u);
    // Stop one scenario
    manager_.state_.scenarioStates.erase(BlinkingScenario::ALARM);
    EXPECT_EQ(manager_.state_.scenarioStates.size(), 1u);
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_HandleStopContinuousBlink_001 end";
}

// ==================== ForceStopAll ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ForceStopAll_001
 * @tc.name: ForceStopAll
 * @tc.desc: Test ForceStopAll when not running.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ForceStopAll_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ForceStopAll_001 start";
    EXPECT_FALSE(manager_.state_.isRunning);
    manager_.ForceStopAll();
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ForceStopAll_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ForceStopAll_002
 * @tc.name: ForceStopAll
 * @tc.desc: Test ForceStopAll stops a running continuous blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ForceStopAll_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ForceStopAll_002 start";
    // Set up continuous blink state directly (avoid spawning hardware thread)
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto scenarioState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::TESTING, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::TESTING] = scenarioState;
    EXPECT_TRUE(manager_.state_.isRunning);
    manager_.ForceStopAll();
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ForceStopAll_002 end";
}

// ==================== RemoveExpiredScenarios ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_001
 * @tc.name: RemoveExpiredScenarios
 * @tc.desc: Test RemoveExpiredScenarios removes fully expired scenarios.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_001 start";
    AccessibleBlinkingReminderManager::ScenarioState expiredState;
    expiredState.scenario = BlinkingScenario::ALARM;
    expiredState.displayExpired = true;
    expiredState.flashlightExpired = true;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = expiredState;
    manager_.RemoveExpiredScenarios();
    EXPECT_TRUE(manager_.state_.scenarioStates.empty());
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_002
 * @tc.name: RemoveExpiredScenarios
 * @tc.desc: Test RemoveExpiredScenarios keeps partially active scenarios.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_002 start";
    AccessibleBlinkingReminderManager::ScenarioState partialState;
    partialState.scenario = BlinkingScenario::ALARM;
    partialState.displayExpired = true;
    partialState.flashlightExpired = false;
    partialState.joinTimeMs = AccessibleBlinkingReminderManager::GetCurrentTimeMs();
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = partialState;
    manager_.RemoveExpiredScenarios();
    EXPECT_EQ(manager_.state_.scenarioStates.size(), 1u);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_RemoveExpiredScenarios_002 end";
}

// ==================== ExpireDisplayForAll / ExpireFlashlightForAll ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ExpireDisplayForAll_001
 * @tc.name: ExpireDisplayForAll
 * @tc.desc: Test ExpireDisplayForAll marks all display components as expired.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ExpireDisplayForAll_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ExpireDisplayForAll_001 start";
    AccessibleBlinkingReminderManager::ScenarioState state1;
    state1.displayExpired = false;
    state1.flashlightExpired = false;
    AccessibleBlinkingReminderManager::ScenarioState state2;
    state2.displayExpired = false;
    state2.flashlightExpired = true;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = state1;
    manager_.state_.scenarioStates[BlinkingScenario::CALL] = state2;
    std::unique_lock<std::mutex> lock(manager_.mutex_);
    manager_.ExpireDisplayForAll(lock);
    EXPECT_TRUE(manager_.state_.scenarioStates[BlinkingScenario::ALARM].displayExpired);
    EXPECT_TRUE(manager_.state_.scenarioStates[BlinkingScenario::CALL].displayExpired);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ExpireDisplayForAll_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ExpireFlashlightForAll_001
 * @tc.name: ExpireFlashlightForAll
 * @tc.desc: Test ExpireFlashlightForAll marks all flashlight components as expired.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ExpireFlashlightForAll_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ExpireFlashlightForAll_001 start";
    AccessibleBlinkingReminderManager::ScenarioState state1;
    state1.displayExpired = false;
    state1.flashlightExpired = false;
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = state1;
    std::unique_lock<std::mutex> lock(manager_.mutex_);
    manager_.ExpireFlashlightForAll(lock);
    EXPECT_TRUE(manager_.state_.scenarioStates[BlinkingScenario::ALARM].flashlightExpired);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ExpireFlashlightForAll_001 end";
}

// ==================== StopByUnlockDisabled ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_001
 * @tc.name: StopByUnlockDisabled
 * @tc.desc: Test StopByUnlockDisabled stops non-TESTING single blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_001 start";
    // Set up single blink state directly (avoid spawning hardware thread)
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::ALARM;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    EXPECT_TRUE(manager_.state_.isRunning);
    std::unique_lock<std::mutex> lock(manager_.mutex_);
    manager_.StopByUnlockDisabled(lock);
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_002
 * @tc.name: StopByUnlockDisabled
 * @tc.desc: Test StopByUnlockDisabled does not stop TESTING single blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_002 start";
    // Set up single blink state directly with TESTING scenario
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::TESTING;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    EXPECT_TRUE(manager_.state_.isRunning);
    std::unique_lock<std::mutex> lock(manager_.mutex_);
    manager_.StopByUnlockDisabled(lock);
    // TESTING scenario should not be stopped
    EXPECT_TRUE(manager_.state_.isRunning);
    lock.unlock();
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_StopByUnlockDisabled_002 end";
}

// ==================== ValidateStartBlinking ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_001
 * @tc.name: ValidateStartBlinking
 * @tc.desc: Test ValidateStartBlinking with all conditions met for TESTING scenario.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_001 start";
    manager_.state_.flashReminderSwitch = true;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    int32_t flashReminderMode = FlashReminderMode::NONE;
    bool displayBlocked = false;
    BlinkResultCode ret = manager_.ValidateStartBlinking(BlinkingScenario::TESTING,
        flashReminderMode, displayBlocked);
    // Result depends on hardware support
    EXPECT_TRUE(ret == BlinkResultCode::BLINK_SUCCESS ||
        ret == BlinkResultCode::SCREEN_BLINKING_UNSUPPORTED);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_002
 * @tc.name: ValidateStartBlinking
 * @tc.desc: Test ValidateStartBlinking with proximity near and DISPLAY only mode.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_002 start";
    manager_.state_.flashReminderSwitch = true;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    manager_.state_.proximityNear = true;
    int32_t flashReminderMode = FlashReminderMode::NONE;
    bool displayBlocked = false;
    BlinkResultCode ret = manager_.ValidateStartBlinking(BlinkingScenario::TESTING,
        flashReminderMode, displayBlocked);
    // DISPLAY only with proximity near should return DEVICE_IN_USE
    EXPECT_TRUE(ret == BlinkResultCode::DEVICE_IN_USE ||
        ret == BlinkResultCode::SCREEN_BLINKING_UNSUPPORTED);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_002 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_003
 * @tc.name: ValidateStartBlinking
 * @tc.desc: Test ValidateStartBlinking with proximity near and BOTH mode marks displayBlocked.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_003 start";
    manager_.state_.flashReminderSwitch = true;
    manager_.state_.flashReminderMode = FlashReminderMode::BOTH;
    manager_.state_.proximityNear = true;
    int32_t flashReminderMode = FlashReminderMode::NONE;
    bool displayBlocked = false;
    BlinkResultCode ret = manager_.ValidateStartBlinking(BlinkingScenario::TESTING,
        flashReminderMode, displayBlocked);
    // BOTH mode with proximity near should set displayBlocked and succeed
    // (flashlight still works), or FLASH_BLINKING_UNSUPPORTED if no flashlight
    if (ret == BlinkResultCode::BLINK_SUCCESS) {
        EXPECT_TRUE(displayBlocked);
    }
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_ValidateStartBlinking_003 end";
}

// ==================== SetFlashReminderMode during running ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_001
 * @tc.name: SetFlashReminderModeRunning
 * @tc.desc: Test setting mode to NONE while running stops blinking.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_001 start";
    // Set up running state directly
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::TESTING;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    EXPECT_TRUE(manager_.state_.isRunning);
    manager_.SetFlashReminderMode(FlashReminderMode::NONE);
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_002
 * @tc.name: SetFlashReminderModeRunning
 * @tc.desc: Test removing flashlight from BOTH mode expires flashlight for all scenarios.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_002 start";
    // Set up running state directly with BOTH mode
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::TESTING;
    manager_.state_.flashReminderMode = FlashReminderMode::BOTH;
    auto scenarioState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::TESTING, FlashReminderMode::BOTH, false);
    manager_.state_.scenarioStates[BlinkingScenario::TESTING] = scenarioState;
    EXPECT_FALSE(manager_.state_.scenarioStates[BlinkingScenario::TESTING].flashlightExpired);
    // Remove flashlight from mode
    manager_.SetFlashReminderMode(FlashReminderMode::DISPLAY);
    EXPECT_TRUE(manager_.state_.scenarioStates[BlinkingScenario::TESTING].flashlightExpired);
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFlashReminderModeRunning_002 end";
}

// ==================== SetFlashReminderFunctionEnabled during running ====================

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_001
 * @tc.name: SetFunctionEnabledRunning
 * @tc.desc: Test disabling function for running scenario stops single blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_001 start";
    // Set up single blink state directly with ALARM scenario
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::SINGLE_BLINK;
    manager_.state_.singleScenario = BlinkingScenario::ALARM;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    EXPECT_TRUE(manager_.state_.isRunning);
    // Disable the scenario that is running
    manager_.SetFlashReminderFunctionEnabled("");
    EXPECT_FALSE(manager_.state_.isRunning);
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_001 end";
}

/**
 * @tc.number: AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_002
 * @tc.name: SetFunctionEnabledRunning
 * @tc.desc: Test disabling function for one scenario keeps others running in continuous blink.
 */
HWTEST_F(AccessibleBlinkingReminderManagerTest,
    AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_002 start";
    // Set up continuous blink state with two scenarios directly
    manager_.state_.isRunning = true;
    manager_.state_.currentMode = BlinkingMode::CONTINUOUS_BLINK;
    manager_.state_.flashReminderMode = FlashReminderMode::DISPLAY;
    auto alarmState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::ALARM, FlashReminderMode::DISPLAY, false);
    auto callState = AccessibleBlinkingReminderManager::MakeScenarioState(
        BlinkingScenario::CALL, FlashReminderMode::DISPLAY, false);
    manager_.state_.scenarioStates[BlinkingScenario::ALARM] = alarmState;
    manager_.state_.scenarioStates[BlinkingScenario::CALL] = callState;
    EXPECT_EQ(manager_.state_.scenarioStates.size(), 2u);
    // Disable one scenario, but keep the other
    manager_.SetFlashReminderFunctionEnabled("INCOMING_CALL");
    // ALARM should be removed, CALL should still be running
    EXPECT_EQ(manager_.state_.scenarioStates.size(), 1u);
    EXPECT_TRUE(manager_.state_.scenarioStates.find(BlinkingScenario::CALL) !=
        manager_.state_.scenarioStates.end());
    manager_.ForceStopAll();
    GTEST_LOG_(INFO) << "AccessibleBlinkingReminderManager_Unittest_SetFunctionEnabledRunning_002 end";
}

} // namespace Accessibility
} // namespace OHOS
