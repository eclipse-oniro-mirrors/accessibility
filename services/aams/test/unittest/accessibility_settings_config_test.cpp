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
#include "accessibility_settings_config.h"
#include "mock_preferences.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t CONTENT_TIMEOUT_VALUE = 10;
    constexpr int32_t MOUSE_AUTO_CLICK_VALUE = 2;
    constexpr float BRIGHTNESS_DISCOUNT_VALUE = 0.3f;
    constexpr float AUDIO_BALANCE_VALUE = 0.1f;
    const std::string CONFIG_ACCESSIBILITY = "accessible";
    const std::string CONFIG_TOUCHGUIDE = "touchGuide";
    const std::string CONFIG_GESTURE = "gesture";
    const std::string CONFIG_CAPTION = "CaptionState";
    const std::string CONFIG_KEYEVENT = "keyEventObserver";
    const std::string CONFIG_SCREENMAGNIFIER = "ScreenMagnification";
    const std::string CONFIG_SHORTKEY = "ShortKey";
    const std::string CONFIG_MOUSEKEY = "MouseKey";
    const std::string CONFIG_HIGHCONTRASTTEXT = "highContrastText";
    const std::string CONFIG_DALTONIZATIONSTATE = "daltonizationState";
    const std::string CONFIG_INVERTCOLOR = "invertColor";
    const std::string CONFIG_ANIMATIONOFF = "animationOff";
    const std::string CONFIG_AUDIOMONO = "audioMono";
    const std::string CONFIG_IGNOREREPEATCLICKSTATE = "ignoreRepeatClickState";
} // namespace

class AccessibilitySettingsConfigTest : public testing::Test {
public:
    AccessibilitySettingsConfigTest()
    {}
    ~AccessibilitySettingsConfigTest()
    {}

    std::shared_ptr<AccessibilitySettingsConfig> settingConfig_ = nullptr;
    std::shared_ptr<AccessibilityDatashareHelper> datashare_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilitySettingsConfigTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfigTest SetUpTestCase";
}

void AccessibilitySettingsConfigTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfigTest TearDownTestCase";
}

void AccessibilitySettingsConfigTest::SetUp()
{
    int32_t accountId = 1;
    settingConfig_ = std::make_shared<AccessibilitySettingsConfig>(accountId);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfigTest SetUp";
}

void AccessibilitySettingsConfigTest::TearDown()
{
    settingConfig_ = nullptr;
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfigTest TearDown";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_Init
 * @tc.name: AccessibilitySettingsConfig_Unittest_Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_Init, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_Init start";
    settingConfig_->Init();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_Init end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetEnabled_001
 * @tc.name: SetEnabled
 * @tc.desc: Test function SetEnabled GetEnabledState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetEnabled(state);
    EXPECT_TRUE(settingConfig_->GetEnabledState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetEnabled_002
 * @tc.name: SetEnabled
 * @tc.desc: Test function SetEnabled GetEnabledState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetEnabled_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled_002 start";
    bool state = true;
    settingConfig_->SetEnabled(state);
    EXPECT_TRUE(settingConfig_->GetEnabledState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetTouchGuideState_001
 * @tc.name: SetTouchGuideState
 * @tc.desc: Test function SetTouchGuideState GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetTouchGuideState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetTouchGuideState(state);
    EXPECT_TRUE(settingConfig_->GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetTouchGuideState_002
 * @tc.name: SetTouchGuideState
 * @tc.desc: Test function SetTouchGuideState GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetTouchGuideState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState_002 start";
    bool state = true;
    settingConfig_->SetTouchGuideState(state);
    EXPECT_TRUE(settingConfig_->GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetGestureState_001
 * @tc.name: SetGestureState
 * @tc.desc: Test function SetGestureState GetGestureState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetGestureState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetGestureState(state);
    EXPECT_TRUE(settingConfig_->GetGestureState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetGestureState_002
 * @tc.name: SetGestureState
 * @tc.desc: Test function SetGestureState GetGestureState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetGestureState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState_002 start";
    bool state = true;
    settingConfig_->SetGestureState(state);
    EXPECT_TRUE(settingConfig_->GetGestureState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_001
 * @tc.name: SetKeyEventObserverState
 * @tc.desc: Test function SetKeyEventObserverState GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetKeyEventObserverState(state);
    EXPECT_TRUE(settingConfig_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_002
 * @tc.name: SetKeyEventObserverState
 * @tc.desc: Test function SetKeyEventObserverState GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_002 start";
    bool state = true;
    settingConfig_->SetKeyEventObserverState(state);
    EXPECT_TRUE(settingConfig_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionProperty_001
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty_001 start";
    AccessibilityConfig::CaptionProperty caption;
    settingConfig_->Init();
    settingConfig_->SetCaptionProperty(caption);
    settingConfig_->GetCaptionProperty();
    EXPECT_NE(settingConfig_.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionProperty_002
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty_002 start";
    AccessibilityConfig::CaptionProperty caption;
    settingConfig_->SetCaptionProperty(caption);
    settingConfig_->GetCaptionProperty();
    EXPECT_NE(settingConfig_.get(), nullptr);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionState_001
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetCaptionState(state);
    EXPECT_TRUE(settingConfig_->GetCaptionState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionState_002
 * @tc.name: SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState_002 start";
    bool state = true;
    settingConfig_->SetCaptionState(state);
    EXPECT_TRUE(settingConfig_->GetCaptionState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetScreenMagnificationState(state);
    EXPECT_TRUE(settingConfig_->GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_002 start";
    bool state = true;
    settingConfig_->SetScreenMagnificationState(state);
    EXPECT_TRUE(settingConfig_->GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortKeyState_001
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetShortKeyState(state);
    EXPECT_TRUE(settingConfig_->GetShortKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortKeyState_002
 * @tc.name: SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState_002 start";
    bool state = true;
    settingConfig_->SetShortKeyState(state);
    EXPECT_TRUE(settingConfig_->GetShortKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseKeyState_001
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetMouseKeyState(state);
    EXPECT_TRUE(settingConfig_->GetMouseKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseKeyState_002
 * @tc.name: SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState_002 start";
    bool state = true;
    settingConfig_->SetMouseKeyState(state);
    EXPECT_TRUE(settingConfig_->GetMouseKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_001 start";
    settingConfig_->Init();
    settingConfig_->SetMouseAutoClick(MOUSE_AUTO_CLICK_VALUE);
    EXPECT_EQ(MOUSE_AUTO_CLICK_VALUE, settingConfig_->GetMouseAutoClick());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_002 start";
    settingConfig_->SetMouseAutoClick(MOUSE_AUTO_CLICK_VALUE);
    EXPECT_EQ(-1, settingConfig_->GetMouseAutoClick());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_001 start";
    std::string name = "TEST";
    settingConfig_->Init();
    settingConfig_->SetShortkeyTarget(name);
    EXPECT_STREQ("none", settingConfig_->GetShortkeyTarget().c_str());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_002 start";
    std::string name = "TEST";
    settingConfig_->SetShortkeyTarget(name);
    EXPECT_STREQ("", settingConfig_->GetShortkeyTarget().c_str());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetHighContrastTextState(state);
    EXPECT_TRUE(settingConfig_->GetHighContrastTextState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_002 start";
    bool state = true;
    settingConfig_->SetHighContrastTextState(state);
    EXPECT_TRUE(settingConfig_->GetHighContrastTextState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetDaltonizationState_001
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetDaltonizationState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetDaltonizationState(state);
    EXPECT_TRUE(settingConfig_->GetDaltonizationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetDaltonizationState_002
 * @tc.name: SetDaltonizationState
 * @tc.desc: Test function SetDaltonizationState GetDaltonizationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetDaltonizationState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationState_002 start";
    bool state = true;
    settingConfig_->SetDaltonizationState(state);
    EXPECT_TRUE(settingConfig_->GetDaltonizationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_DaltonizationState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetInvertColorState_001
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetInvertColorState(state);
    EXPECT_TRUE(settingConfig_->GetInvertColorState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetInvertColorState_002
 * @tc.name: SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState_002 start";
    bool state = true;
    settingConfig_->SetInvertColorState(state);
    EXPECT_TRUE(settingConfig_->GetInvertColorState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAnimationOffState_001
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAnimationOffState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetAnimationOffState(state);
    EXPECT_TRUE(settingConfig_->GetAnimationOffState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAnimationOffState_002
 * @tc.name: SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAnimationOffState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState_002 start";
    bool state = true;
    settingConfig_->SetAnimationOffState(state);
    EXPECT_TRUE(settingConfig_->GetAnimationOffState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioMonoState_001
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState_001 start";
    bool state = true;
    settingConfig_->Init();
    settingConfig_->SetAudioMonoState(state);
    EXPECT_TRUE(settingConfig_->GetAudioMonoState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioMonoState_002
 * @tc.name: SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState_002 start";
    bool state = true;
    settingConfig_->SetAudioMonoState(state);
    EXPECT_TRUE(settingConfig_->GetAudioMonoState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_001 start";
    uint32_t filter = 1;
    settingConfig_->Init();
    settingConfig_->SetDaltonizationColorFilter(filter);
    EXPECT_TRUE(settingConfig_->GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_002 start";
    uint32_t filter = 1;
    settingConfig_->SetDaltonizationColorFilter(filter);
    EXPECT_TRUE(settingConfig_->GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetContentTimeout_001
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout_001 start";
    settingConfig_->Init();
    settingConfig_->SetContentTimeout(CONTENT_TIMEOUT_VALUE);
    EXPECT_EQ(CONTENT_TIMEOUT_VALUE, settingConfig_->GetContentTimeout());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetContentTimeout_002
 * @tc.name: SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout_002 start";
    settingConfig_->SetContentTimeout(CONTENT_TIMEOUT_VALUE);
    EXPECT_EQ(CONTENT_TIMEOUT_VALUE, settingConfig_->GetContentTimeout());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_001 start";
    settingConfig_->Init();
    settingConfig_->SetBrightnessDiscount(BRIGHTNESS_DISCOUNT_VALUE);
    EXPECT_EQ(BRIGHTNESS_DISCOUNT_VALUE, settingConfig_->GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_002 start";
    settingConfig_->SetBrightnessDiscount(BRIGHTNESS_DISCOUNT_VALUE);
    EXPECT_EQ(BRIGHTNESS_DISCOUNT_VALUE, settingConfig_->GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioBalance_001
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance_001 start";
    settingConfig_->Init();
    settingConfig_->SetAudioBalance(AUDIO_BALANCE_VALUE);
    EXPECT_EQ(AUDIO_BALANCE_VALUE, settingConfig_->GetAudioBalance());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioBalance_002
 * @tc.name: SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance_002 start";
    settingConfig_->SetAudioBalance(AUDIO_BALANCE_VALUE);
    EXPECT_EQ(AUDIO_BALANCE_VALUE, settingConfig_->GetAudioBalance());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetClickResponseTime_001
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetClickResponseTime_001 start";
    settingConfig_->Init();
    settingConfig_->SetClickResponseTime(1);
    EXPECT_EQ(1, settingConfig_->GetClickResponseTime());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetClickResponseTime_002
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime GetClickResponseTime
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetClickResponseTime_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetClickResponseTime_002 start";
    settingConfig_->SetClickResponseTime(1);
    EXPECT_EQ(1, settingConfig_->GetClickResponseTime());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetClickResponseTime_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_001
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_001 start";
    settingConfig_->Init();
    settingConfig_->SetIgnoreRepeatClickState(true);
    EXPECT_EQ(true, settingConfig_->GetIgnoreRepeatClickState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_002
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState GetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_002 start";
    settingConfig_->SetIgnoreRepeatClickState(true);
    EXPECT_EQ(true, settingConfig_->GetIgnoreRepeatClickState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickState_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_001
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_001 start";
    settingConfig_->Init();
    settingConfig_->SetIgnoreRepeatClickTime(1);
    EXPECT_EQ(1, settingConfig_->GetIgnoreRepeatClickTime());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_002
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime GetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_002 start";
    settingConfig_->SetIgnoreRepeatClickTime(1);
    EXPECT_EQ(1, settingConfig_->GetIgnoreRepeatClickTime());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetIgnoreRepeatClickTime_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_001
 * @tc.name: UpdateEnabledAbilities
 * @tc.desc: Test function UpdateEnabledAbilities
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_001 start";
    std::vector<std::string> vecvalue {};
    settingConfig_->Init();
    settingConfig_->UpdateEnabledAbilities(vecvalue);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_002
 * @tc.name: UpdateEnabledAbilities
 * @tc.desc: Test function UpdateEnabledAbilities
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_002 start";
    std::vector<std::string> vecvalue { "test" };
    settingConfig_->UpdateEnabledAbilities(vecvalue);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities_002 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos_001
 * @tc.name: GetEnabledAbilityInfos
 * @tc.desc: Test function GetEnabledAbilityInfos
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos_001 start";
    settingConfig_->GetEnabledAbilityInfos();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_GetConfigState_001
 * @tc.name: GetConfigState
 * @tc.desc: Test function GetConfigState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_GetConfigState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetConfigState_001 start";
    uint32_t state = 0x00000000;
    EXPECT_EQ(state, settingConfig_->GetConfigState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetConfigState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_ClearData_001
 * @tc.name: ClearData
 * @tc.desc: Test function ClearData
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_ClearData_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_ClearData_001 start";
    settingConfig_->Init();
    settingConfig_->ClearData();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_ClearData_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001
 * @tc.name: SetShortkeyMultiTargetInPkgRemove
 * @tc.desc: Test function SetShortkeyMultiTargetInPkgRemove
 */
HWTEST_F(AccessibilitySettingsConfigTest, Unittest_SetShortkeyMultiTargetInPkgRemove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001 start";
    settingConfig_->Init();
    std::string name = "TEST";
    settingConfig_->SetShortkeyMultiTargetInPkgRemove(name);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetStartFromAtoHosState_001
 * @tc.name: SetStartFromAtoHosState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetStartFromAtoHosState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetStartFromAtoHosState_001 start";
    bool state = true;
    int32_t ret = settingConfig_->SetStartFromAtoHosState(state);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetStartFromAtoHosState_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTarget_001
 * @tc.name: SetShortkeyMultiTarget
 * @tc.desc: Test function SetShortkeyMultiTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTarget_001 start";
    const std::vector<std::string> name;
    int32_t ret = settingConfig_->SetShortkeyMultiTarget(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTarget_001 end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001
 * @tc.name: SetShortkeyMultiTargetInPkgRemove
 * @tc.desc: Test function SetShortkeyMultiTargetInPkgRemove
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest,
    AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001 start";
    const std::string name = "test";
    int32_t ret = settingConfig_->SetShortkeyMultiTargetInPkgRemove(name);
    EXPECT_EQ(ret, RET_ERR_NULLPTR);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyMultiTargetInPkgRemove_001 end";
}
} // namespace Accessibility
} // namespace OHOS