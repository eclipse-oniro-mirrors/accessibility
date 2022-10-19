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
#include "accessibility_config.h"
#include "parameter.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AccessibilityConfig {
namespace {
    constexpr float BRIGHTNESS_DISCOUNT_VALUE = 0.2f;
} // namespace

class AccessibilityConfigImplTest : public ::testing::Test {
public:
    AccessibilityConfigImplTest()
    {}
    ~AccessibilityConfigImplTest()
    {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityConfigImplTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityConfigImplTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityConfigImplTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityConfigImplTest TearDown()";
    }
};

class MockAccessibilityConfigObserverImpl : public OHOS::AccessibilityConfig::AccessibilityConfigObserver {
public:
    MockAccessibilityConfigObserverImpl() = default;
    void OnConfigChanged(const OHOS::AccessibilityConfig::CONFIG_ID id,
        const OHOS::AccessibilityConfig::ConfigValue& value) override
    {}
};

class MockAccessibilityEnableAbilityListsObserverImpl :
        public OHOS::AccessibilityConfig::AccessibilityEnableAbilityListsObserver {
public:
    MockAccessibilityEnableAbilityListsObserverImpl() = default;
    void OnEnableAbilityListsStateChanged() override
    {}
};

/**
 * @tc.number: SetCaptionProperty_002
 * @tc.name: SetCaptionProperty_002
 * @tc.desc: Test function SetCaptionsProperty GetCaptionsProperty
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionProperty_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionProperty_002 start";

    CaptionProperty caption;
    CaptionProperty value;
    caption.SetFontFamily("sansSerif");

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetCaptionsProperty(caption);
    instance.GetCaptionsProperty(value);
    EXPECT_STREQ(value.GetFontFamily().c_str(), "default");
    GTEST_LOG_(INFO) << "SetCaptionProperty_002 end";
}

/**
 * @tc.number: SetScreenMagnificationState_002
 * @tc.name: SetScreenMagnificationState_002
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilityConfigImplTest, SetScreenMagnificationState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetScreenMagnificationState(state);
    instance.GetScreenMagnificationState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_002 end";
}

/**
 * @tc.number: SetShortKeyState_002
 * @tc.name: SetShortKeyState_002
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortKeyState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetShortKeyState(state);
    instance.GetShortKeyState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetShortKeyState_002 end";
}

/**
 * @tc.number: SetMouseKeyState_002
 * @tc.name: SetMouseKeyState_002
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseKeyState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseKeyState_002 start";
    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetMouseKeyState(state);
    instance.GetMouseKeyState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetMouseKeyState_002 end";
}

/**
 * @tc.number: SetCaptionsState_002
 * @tc.name: SetCaptionsState_002
 * @tc.desc: Test function SetCaptionsState GetCaptionsState
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionsState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionsState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetCaptionsState(state);
    instance.GetCaptionsState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetCaptionsState_002 end";
}

/**
 * @tc.number: SetMouseAutoClick_002
 * @tc.name: SetMouseAutoClick_002
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseAutoClick_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseAutoClick_002 start";

    int32_t time = 10;
    int32_t value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetMouseAutoClick(time);
    instance.GetMouseAutoClick(value);
    EXPECT_EQ(0, value);
    GTEST_LOG_(INFO) << "SetMouseAutoClick_002 end";
}

/**
 * @tc.number: SetShortkeyTarget_002
 * @tc.name: SetShortkeyTarget_002
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortkeyTarget_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortkeyTarget_002 start";

    std::string name = "test";
    std::string value = "";
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetShortkeyTarget(name);
    instance.GetShortkeyTarget(value);
    EXPECT_STREQ(value.c_str(), "");
    GTEST_LOG_(INFO) << "SetShortkeyTarget_002 end";
}

/**
 * @tc.number: SetHighContrastTextState_002
 * @tc.name: SetHighContrastTextState_002
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilityConfigImplTest, SetHighContrastTextState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHighContrastTextState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetHighContrastTextState(state);
    instance.GetHighContrastTextState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetHighContrastTextState_002 end";
}

/**
 * @tc.number: SetInvertColorState_002
 * @tc.name: SetInvertColorState_002
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilityConfigImplTest, SetInvertColorState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInvertColorState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetInvertColorState(state);
    instance.GetInvertColorState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetInvertColorState_002 end";
}

/**
 * @tc.number: SetDaltonizationColorFilter_002
 * @tc.name: SetDaltonizationColorFilter_002
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilityConfigImplTest, SetDaltonizationColorFilter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_002 start";

    DALTONIZATION_TYPE type = Protanomaly;
    DALTONIZATION_TYPE value = Normal;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetDaltonizationColorFilter(type);
    instance.GetDaltonizationColorFilter(value);
    EXPECT_EQ(0, static_cast<uint32_t>(value));
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_002 end";
}

/**
 * @tc.number: SetContentTimeout_002
 * @tc.name: SetContentTimeout_002
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilityConfigImplTest, SetContentTimeout_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentTimeout_002 start";

    uint32_t timer = 10;
    uint32_t value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetContentTimeout(timer);
    instance.GetContentTimeout(value);
    EXPECT_EQ(0, value);
    GTEST_LOG_(INFO) << "SetContentTimeout_002 end";
}

/**
 * @tc.number: SetAnimationOffState_002
 * @tc.name: SetAnimationOffState_002
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilityConfigImplTest, SetAnimationOffState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAnimationOffState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetAnimationOffState(state);
    instance.GetAnimationOffState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetAnimationOffState_002 end";
}

/**
 * @tc.number: SetBrightnessDiscount_002
 * @tc.name: SetBrightnessDiscount_002
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilityConfigImplTest, SetBrightnessDiscount_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_002 start";

    float brightness = BRIGHTNESS_DISCOUNT_VALUE;
    float value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetBrightnessDiscount(brightness);
    instance.GetBrightnessDiscount(value);
    EXPECT_FLOAT_EQ(0, value);
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_002 end";
}

/**
 * @tc.number: SetAudioMonoState_002
 * @tc.name: SetAudioMonoState_002
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioMonoState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioMonoState_002 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetAudioMonoState(state);
    instance.GetAudioMonoState(value);
    EXPECT_FALSE(value);
    GTEST_LOG_(INFO) << "SetAudioMonoState_002 end";
}

/**
 * @tc.number: SetAudioBalance_002
 * @tc.name: SetAudioBalance_002
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioBalance_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioBalance_002 start";
    float balance = -1.0;
    float value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SetAudioBalance(balance);
    instance.GetAudioBalance(value);
    EXPECT_FLOAT_EQ(0, value);
    GTEST_LOG_(INFO) << "SetAudioBalance_002 end";
}

/**
 * @tc.number: ConnectToService_001
 * @tc.name: ConnectToService_001
 * @tc.desc: Test function ConnectToService
 */
HWTEST_F(AccessibilityConfigImplTest, ConnectToService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ConnectToService_001 start";

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    WaitParameter(NULL, NULL, 1);
    instance.InitializeContext();
    WaitParameter(NULL, NULL, 0);
    GTEST_LOG_(INFO) << "ConnectToService_001 end";
}

/**
 * @tc.number: SetCaptionProperty_001
 * @tc.name: SetCaptionProperty_001
 * @tc.desc: Test function SetCaptionsProperty GetCaptionsProperty
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 start";

    CaptionProperty caption;
    caption.SetFontFamily("sansSerif");
    CaptionProperty value;

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetCaptionsProperty(caption);
    instance.GetCaptionsProperty(value);
    EXPECT_STREQ(caption.GetFontFamily().c_str(), value.GetFontFamily().c_str());
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 end";
}

/**
 * @tc.number: SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState_001
 * @tc.desc: Test function SetScreenMagnificationState GetScreenMagnificationState
 */
HWTEST_F(AccessibilityConfigImplTest, SetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetScreenMagnificationState(state);
    instance.GetScreenMagnificationState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: SetShortKeyState_001
 * @tc.name: SetShortKeyState_001
 * @tc.desc: Test function SetShortKeyState GetShortKeyState
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortKeyState_001 start";
    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetShortKeyState(state);
    instance.GetShortKeyState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetShortKeyState_001 end";
}

/**
 * @tc.number: SetMouseKeyState_001
 * @tc.name: SetMouseKeyState_001
 * @tc.desc: Test function SetMouseKeyState GetMouseKeyState
 * @tc.require: issueI5NTXA
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseKeyState_001 start";
    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetMouseKeyState(state);
    instance.GetMouseKeyState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetMouseKeyState_001 end";
}

/**
 * @tc.number: SetCaptionsState_001
 * @tc.name: SetCaptionsState_001
 * @tc.desc: Test function SetCaptionsState GetCaptionsState
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionsState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionsState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetCaptionsState(state);
    instance.GetCaptionsState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetCaptionsState_001 end";
}

/**
 * @tc.number: SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick_001
 * @tc.desc: Test function SetMouseAutoClick GetMouseAutoClick
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseAutoClick_001 start";

    int32_t time = 10;
    int32_t value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetMouseAutoClick(time);
    instance.GetMouseAutoClick(value);
    EXPECT_EQ(time, value);
    GTEST_LOG_(INFO) << "SetMouseAutoClick_001 end";
}

/**
 * @tc.number: SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget_001
 * @tc.desc: Test function SetShortkeyTarget GetShortkeyTarget
 * @tc.require: issueI5NTXH
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortkeyTarget_001 start";

    std::string name = "test";
    std::string value = "";
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetShortkeyTarget(name);
    instance.GetShortkeyTarget(value);
    EXPECT_STREQ(name.c_str(), value.c_str());
    GTEST_LOG_(INFO) << "SetShortkeyTarget_001 end";
}

/**
 * @tc.number: SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState_001
 * @tc.desc: Test function SetHighContrastTextState GetHighContrastTextState
 * @tc.require: issueI5NTX9
 */
HWTEST_F(AccessibilityConfigImplTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHighContrastTextState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetHighContrastTextState(state);
    instance.GetHighContrastTextState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetHighContrastTextState_001 end";
}

/**
 * @tc.number: SetInvertColorState_001
 * @tc.name: SetInvertColorState_001
 * @tc.desc: Test function SetInvertColorState GetInvertColorState
 * @tc.require: issueI5NTX7
 */
HWTEST_F(AccessibilityConfigImplTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInvertColorState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetInvertColorState(state);
    instance.GetInvertColorState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetInvertColorState_001 end";
}

/**
 * @tc.number: SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter_001
 * @tc.desc: Test function SetDaltonizationColorFilter GetDaltonizationColorFilter
 * @tc.require: issueI5NTX8
 */
HWTEST_F(AccessibilityConfigImplTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_001 start";

    DALTONIZATION_TYPE type = Protanomaly;
    DALTONIZATION_TYPE value = Normal;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetDaltonizationColorFilter(type);
    instance.GetDaltonizationColorFilter(value);
    EXPECT_EQ(static_cast<uint32_t>(type), static_cast<uint32_t>(value));
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: SetContentTimeout_001
 * @tc.name: SetContentTimeout_001
 * @tc.desc: Test function SetContentTimeout GetContentTimeout
 * @tc.require: issueI5NTXF
 */
HWTEST_F(AccessibilityConfigImplTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentTimeout_001 start";

    uint32_t timer = 10;
    uint32_t value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetContentTimeout(timer);
    instance.GetContentTimeout(value);
    EXPECT_EQ(timer, value);
    GTEST_LOG_(INFO) << "SetContentTimeout_001 end";
}

/**
 * @tc.number: SetAnimationOffState_001
 * @tc.name: SetAnimationOffState_001
 * @tc.desc: Test function SetAnimationOffState GetAnimationOffState
 * @tc.require: issueI5NTXG
 */
HWTEST_F(AccessibilityConfigImplTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAnimationOffState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetAnimationOffState(state);
    instance.GetAnimationOffState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetAnimationOffState_001 end";
}

/**
 * @tc.number: SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount_001
 * @tc.desc: Test function SetBrightnessDiscount GetBrightnessDiscount
 * @tc.require: issueI5NTXE
 */
HWTEST_F(AccessibilityConfigImplTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_001 start";

    float brightness = 0;
    float value = BRIGHTNESS_DISCOUNT_VALUE;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetBrightnessDiscount(brightness);
    instance.GetBrightnessDiscount(value);
    EXPECT_FLOAT_EQ(brightness, value);
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: SetAudioMonoState_001
 * @tc.name: SetAudioMonoState_001
 * @tc.desc: Test function SetAudioMonoState GetAudioMonoState
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioMonoState_001 start";

    bool state = true;
    bool value = false;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetAudioMonoState(state);
    instance.GetAudioMonoState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "SetAudioMonoState_001 end";
}

/**
 * @tc.number: SetAudioBalance_001
 * @tc.name: SetAudioBalance_001
 * @tc.desc: Test function SetAudioBalance GetAudioBalance
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioBalance_001 start";
    float balance = 0;
    float value = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SetAudioBalance(balance);
    instance.GetAudioBalance(value);
    EXPECT_FLOAT_EQ(balance, value);
    sleep(1);
    GTEST_LOG_(INFO) << "SetAudioBalance_001 end";
}

/**
 * @tc.number: ConfigNotify_001
 * @tc.name: ConfigNotify_001
 * @tc.desc: Test function OnConfigStateChanged
 */
HWTEST_F(AccessibilityConfigImplTest, ConfigNotify_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ConfigNotify_001 start";
    float balance = -1.0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::shared_ptr<MockAccessibilityConfigObserverImpl> observer =
        std::make_shared<MockAccessibilityConfigObserverImpl>();
    instance.InitializeContext();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID_MAX); index ++) {
        instance.SubscribeConfigObserver(static_cast<CONFIG_ID>(index), observer, false);
    }
    instance.SetAudioBalance(balance);
    sleep(1);
    instance.SetAudioBalance(balance);
    sleep(1);
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID_MAX); index ++) {
        instance.UnsubscribeConfigObserver(static_cast<CONFIG_ID>(index), observer);
    }
    GTEST_LOG_(INFO) << "ConfigNotify_001 end";
}

/**
 * @tc.number: ConfigNotify_002
 * @tc.name: ConfigNotify_002
 * @tc.desc: Test function OnConfigStateChanged
 */
HWTEST_F(AccessibilityConfigImplTest, ConfigNotify_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ConfigNotify_002 start";
    float balance = 0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::shared_ptr<MockAccessibilityConfigObserverImpl> observer =
        std::make_shared<MockAccessibilityConfigObserverImpl>();
    instance.InitializeContext();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID_MAX); index ++) {
        instance.SubscribeConfigObserver(static_cast<CONFIG_ID>(index), observer);
    }
    instance.SetAudioBalance(balance);
    sleep(1);
    GTEST_LOG_(INFO) << "ConfigNotify_002 end";
}

/**
 * @tc.number: ConfigNotify_003
 * @tc.name: ConfigNotify_003
 * @tc.desc: Test function OnConfigStateChanged
 */
HWTEST_F(AccessibilityConfigImplTest, ConfigNotify_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ConfigNotify_003 start";
    float balance = -1.0;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID_MAX); index ++) {
        instance.SubscribeConfigObserver(static_cast<CONFIG_ID>(index), nullptr);
    }
    instance.SetAudioBalance(balance);
    sleep(1);
    GTEST_LOG_(INFO) << "ConfigNotify_003 end";
}

/**
 * @tc.number: SubscribeConfigObserver_001
 * @tc.name: SubscribeConfigObserver_001
 * @tc.desc: Test function SubscribeConfigObserver
 */
HWTEST_F(AccessibilityConfigImplTest, SubscribeConfigObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SubscribeConfigObserver_001 start";

    std::shared_ptr<AccessibilityConfigObserver> observer = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SubscribeConfigObserver(CONFIG_HIGH_CONTRAST_TEXT, observer);
    GTEST_LOG_(INFO) << "SubscribeConfigObserver_001 end";
}

/**
 * @tc.number: UnsubscribeConfigObserver_001
 * @tc.name: UnsubscribeConfigObserver_001
 * @tc.desc: Test function UnsubscribeConfigObserver
 */
HWTEST_F(AccessibilityConfigImplTest, UnsubscribeConfigObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UnsubscribeConfigObserver_001 start";

    std::shared_ptr<AccessibilityConfigObserver> observer = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.UnsubscribeConfigObserver(CONFIG_HIGH_CONTRAST_TEXT, observer);
    GTEST_LOG_(INFO) << "UnsubscribeConfigObserver_001 end";
}

/**
 * @tc.number: SubscribeEnableAbilityListsObserver_001
 * @tc.name: SubscribeEnableAbilityListsObserver_001
 * @tc.desc: Test function SubscribeEnableAbilityListsObserver
 */
HWTEST_F(AccessibilityConfigImplTest, SubscribeEnableAbilityListsObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SubscribeEnableAbilityListsObserver_001 start";

    std::shared_ptr<AccessibilityEnableAbilityListsObserver> observer =
        std::make_shared<MockAccessibilityEnableAbilityListsObserverImpl>();
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SubscribeEnableAbilityListsObserver(observer);
    GTEST_LOG_(INFO) << "SubscribeEnableAbilityListsObserver_001 end";
}

/**
 * @tc.number: UnsubscribeEnableAbilityListsObserver_001
 * @tc.name: UnsubscribeEnableAbilityListsObserver_001
 * @tc.desc: Test function UnsubscribeEnableAbilityListsObserver
 */
HWTEST_F(AccessibilityConfigImplTest, UnsubscribeEnableAbilityListsObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UnsubscribeEnableAbilityListsObserver_001 start";

    std::shared_ptr<AccessibilityEnableAbilityListsObserver> observer = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    instance.SubscribeEnableAbilityListsObserver(observer);
    instance.UnsubscribeEnableAbilityListsObserver(observer);
    GTEST_LOG_(INFO) << "UnsubscribeEnableAbilityListsObserver_001 end";
}

/**
 * @tc.number: EnableAbility_001
 * @tc.name: EnableAbility_001
 * @tc.desc: Test function EnableAbility
 */
HWTEST_F(AccessibilityConfigImplTest, EnableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EnableAbility_001 start";

    std::string name = "test";
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    EXPECT_EQ(Accessibility::RET_OK, instance.EnableAbility(name, 0));
    sleep(1);
    GTEST_LOG_(INFO) << "EnableAbility_001 end";
}

/**
 * @tc.number: DisableAbility_001
 * @tc.name: DisableAbility_001
 * @tc.desc: Test function DisableAbility
 */
HWTEST_F(AccessibilityConfigImplTest, DisableAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DisableAbility_001 start";

    std::string name = "test";
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.InitializeContext();
    EXPECT_EQ(Accessibility::RET_OK, instance.DisableAbility(name));
    GTEST_LOG_(INFO) << "DisableAbility_001 end";
}
} // namespace AccessibilityConfig
} // namespace OHOS