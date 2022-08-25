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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilitySettingsConfigTest : public testing::Test {
public:
    AccessibilitySettingsConfigTest()
    {}
    ~AccessibilitySettingsConfigTest()
    {}

    std::shared_ptr<AccessibilitySettingsConfig> settingConfig_ = nullptr;

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
    settingConfig_->Init();
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
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetEnabled
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetEnabled
 * @tc.desc: Test function SetEnabled GetEnabledState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetEnabled, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled start";
    bool state = true;
    settingConfig_->SetEnabled(state);
    EXPECT_TRUE(settingConfig_->GetEnabledState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetEnabled end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetTouchGuideState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetTouchGuideState
 * @tc.desc: Test function SetTouchGuideState GetTouchGuideState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetTouchGuideState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState start";
    bool state = true;
    settingConfig_->SetTouchGuideState(state);
    EXPECT_TRUE(settingConfig_->GetTouchGuideState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetTouchGuideState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetGestureState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetGestureState
 * @tc.desc: Test function SetGestureState GetGestureState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetGestureState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState start";
    bool state = true;
    settingConfig_->SetGestureState(state);
    EXPECT_TRUE(settingConfig_->GetGestureState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetGestureState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState
 * @tc.desc: Test function SetKeyEventObserverState GetKeyEventObserverState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState start";
    bool state = true;
    settingConfig_->SetKeyEventObserverState(state);
    EXPECT_TRUE(settingConfig_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetKeyEventObserverState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionProperty
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty GetCaptionProperty
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionProperty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty start";
    AccessibilityConfig::CaptionProperty caption;
    settingConfig_->SetCaptionProperty(caption);
    settingConfig_->GetCaptionProperty();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionProperty end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetCaptionState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetCaptionState
 * @tc.desc: Test function SetCaptionState GetCaptionState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetCaptionState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState start";
    bool state = true;
    settingConfig_->SetCaptionState(state);
    EXPECT_TRUE(settingConfig_->GetCaptionState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetCaptionState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState start";
    bool state = true;
    settingConfig_->SetScreenMagnificationState(state);
    EXPECT_TRUE(settingConfig_->GetScreenMagnificationState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetScreenMagnificationState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortKeyState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetShortKeyState
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortKeyState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState start";
    bool state = true;
    settingConfig_->SetShortKeyState(state);
    EXPECT_TRUE(settingConfig_->GetShortKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortKeyState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseKeyState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetMouseKeyState
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseKeyState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState start";
    bool state = true;
    settingConfig_->SetMouseKeyState(state);
    EXPECT_TRUE(settingConfig_->GetMouseKeyState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseKeyState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetMouseAutoClick
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetMouseAutoClick
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetMouseAutoClick, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick start";
    int32_t time = 2;
    settingConfig_->SetMouseAutoClick(time);
    EXPECT_EQ(time, settingConfig_->GetMouseAutoClick());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetMouseAutoClick end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetShortkeyTarget
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetShortkeyTarget
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetShortkeyTarget, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget start";
    std::string name = "TEST";
    settingConfig_->SetShortkeyTarget(name);
    EXPECT_STREQ("TEST", settingConfig_->GetShortkeyTarget().c_str());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetShortkeyTarget end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetHighContrastTextState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetHighContrastTextState
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetHighContrastTextState,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState start";
    bool state = true;
    settingConfig_->SetHighContrastTextState(state);
    EXPECT_TRUE(settingConfig_->GetHighContrastTextState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetHighContrastTextState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetInvertColorState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetInvertColorState
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetInvertColorState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState start";
    bool state = true;
    settingConfig_->SetInvertColorState(state);
    EXPECT_TRUE(settingConfig_->GetInvertColorState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetInvertColorState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAnimationOffState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetAnimationOffState
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAnimationOffState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState start";
    bool state = true;
    settingConfig_->SetAnimationOffState(state);
    EXPECT_TRUE(settingConfig_->GetAnimationOffState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAnimationOffState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioMonoState
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetAudioMonoState
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioMonoState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState start";
    bool state = true;
    settingConfig_->SetAudioMonoState(state);
    EXPECT_TRUE(settingConfig_->GetAudioMonoState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioMonoState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter start";
    uint32_t filter = 1;
    settingConfig_->SetDaltonizationColorFilter(filter);
    EXPECT_TRUE(settingConfig_->GetDaltonizationColorFilter());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetDaltonizationColorFilter end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetContentTimeout
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetContentTimeout
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetContentTimeout, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout start";
    uint32_t time = 10;
    settingConfig_->SetContentTimeout(time);
    EXPECT_EQ(time, settingConfig_->GetContentTimeout());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetContentTimeout end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount start";
    float discount = 0.1;
    settingConfig_->SetBrightnessDiscount(discount);
    EXPECT_EQ(discount, settingConfig_->GetBrightnessDiscount());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetBrightnessDiscount end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_SetAudioBalance
 * @tc.name: AccessibilitySettingsConfig_Unittest_SetAudioBalance
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_SetAudioBalance, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance start";
    float balance = 0.1;
    settingConfig_->SetAudioBalance(balance);
    EXPECT_EQ(balance, settingConfig_->GetAudioBalance());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_SetAudioBalance end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities
 * @tc.name: AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities
 * @tc.desc: Test function UpdateEnabledAbilities
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities start";
    std::vector<std::string> vecvalue {};
    settingConfig_->UpdateEnabledAbilities(vecvalue);
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_UpdateEnabledAbilities end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos
 * @tc.name: AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos
 * @tc.desc: Test function GetEnabledAbilityInfos
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos start";
    settingConfig_->GetEnabledAbilityInfos();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetEnabledAbilityInfos end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_GetConfigState
 * @tc.name: AccessibilitySettingsConfig_Unittest_GetConfigState
 * @tc.desc: Test function GetConfigState
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_GetConfigState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetConfigState start";
    uint32_t state = 0x00000000;
    EXPECT_EQ(state, settingConfig_->GetConfigState());
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_GetConfigState end";
}

/**
 * @tc.number: AccessibilitySettingsConfig_Unittest_ClearData
 * @tc.name: AccessibilitySettingsConfig_Unittest_ClearData
 * @tc.desc: Test function ClearData
 */
HWTEST_F(AccessibilitySettingsConfigTest, AccessibilitySettingsConfig_Unittest_ClearData, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_ClearData start";
    settingConfig_->ClearData();
    GTEST_LOG_(INFO) << "AccessibilitySettingsConfig_Unittest_ClearData end";
}
} // namespace Accessibility
} // namespace OHOS