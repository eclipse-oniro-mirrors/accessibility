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
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AccessibilityConfig {
namespace {
    constexpr uint32_t CONTENT_TIMEOUT_VALUE = 10;
    constexpr int32_t MOUSE_AUTO_CLICK_VALUE = 10;
    constexpr float BRIGHTNESS_DISCOUNT_VALUE = 0.3f;
}
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

/**
 * @tc.number: SetCaptionProperty_001
 * @tc.name: SetCaptionProperty_001
 * @tc.desc: Test function SetCaptionsProperty
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 start";

    CaptionProperty caption;
    caption.SetFontFamily("sansSerif");

    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetCaptionsProperty(caption);
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 end";
}


/**
 * @tc.number: SetScreenMagnificationState_001
 * @tc.name: SetScreenMagnificationState_001
 * @tc.desc: Test function SetScreenMagnificationState
 */
HWTEST_F(AccessibilityConfigImplTest, SetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetScreenMagnificationState(state);
    GTEST_LOG_(INFO) << "SetScreenMagnificationState_001 end";
}

/**
 * @tc.number: SetShortKeyState_001
 * @tc.name: SetShortKeyState_001
 * @tc.desc: Test function SetShortKeyState
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortKeyState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetShortKeyState(state);
    GTEST_LOG_(INFO) << "SetShortKeyState_001 end";
}

/**
 * @tc.number: SetMouseKeyState_001
 * @tc.name: SetMouseKeyState_001
 * @tc.desc: Test function SetMouseKeyState
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseKeyState_001 start";
    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetMouseKeyState(state);
    GTEST_LOG_(INFO) << "SetMouseKeyState_001 end";
}

/**
 * @tc.number: SetCaptionsState_001
 * @tc.name: SetCaptionsState_001
 * @tc.desc: Test function SetCaptionsState
 */
HWTEST_F(AccessibilityConfigImplTest, SetCaptionsState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionsState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetCaptionsState(state);
    GTEST_LOG_(INFO) << "SetCaptionsState_001 end";
}

/**
 * @tc.number: SetMouseAutoClick_001
 * @tc.name: SetMouseAutoClick_001
 * @tc.desc: Test function SetMouseAutoClick
 */
HWTEST_F(AccessibilityConfigImplTest, SetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMouseAutoClick_001 start";

    int32_t time = 10;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetMouseAutoClick(time);
    GTEST_LOG_(INFO) << "SetMouseAutoClick_001 end";
}

/**
 * @tc.number: SetShortkeyTarget_001
 * @tc.name: SetShortkeyTarget_001
 * @tc.desc: Test function SetShortkeyTarget
 */
HWTEST_F(AccessibilityConfigImplTest, SetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetShortkeyTarget_001 start";

    std::string name = "test";
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetShortkeyTarget(name);
    GTEST_LOG_(INFO) << "SetShortkeyTarget_001 end";
}

/**
 * @tc.number: SetHighContrastTextState_001
 * @tc.name: SetHighContrastTextState_001
 * @tc.desc: Test function SetHighContrastTextState
 */
HWTEST_F(AccessibilityConfigImplTest, SetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHighContrastTextState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetHighContrastTextState(state);
    GTEST_LOG_(INFO) << "SetHighContrastTextState_001 end";
}

/**
 * @tc.number: SetInvertColorState_001
 * @tc.name: SetInvertColorState_001
 * @tc.desc: Test function SetInvertColorState
 */
HWTEST_F(AccessibilityConfigImplTest, SetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInvertColorState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetInvertColorState(state);
    GTEST_LOG_(INFO) << "SetInvertColorState_001 end";
}

/**
 * @tc.number: SetDaltonizationColorFilter_001
 * @tc.name: SetDaltonizationColorFilter_001
 * @tc.desc: Test function SetDaltonizationColorFilter
 */
HWTEST_F(AccessibilityConfigImplTest, SetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_001 start";

    DALTONIZATION_TYPE type = Protanomaly;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetDaltonizationColorFilter(type);
    GTEST_LOG_(INFO) << "SetDaltonizationColorFilter_001 end";
}

/**
 * @tc.number: SetContentTimeout_001
 * @tc.name: SetContentTimeout_001
 * @tc.desc: Test function SetContentTimeout
 */
HWTEST_F(AccessibilityConfigImplTest, SetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentTimeout_001 start";

    uint32_t timer = 10;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetContentTimeout(timer);
    GTEST_LOG_(INFO) << "SetContentTimeout_001 end";
}

/**
 * @tc.number: SetAnimationOffState_001
 * @tc.name: SetAnimationOffState_001
 * @tc.desc: Test function SetAnimationOffState
 */
HWTEST_F(AccessibilityConfigImplTest, SetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAnimationOffState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetAnimationOffState(state);
    GTEST_LOG_(INFO) << "SetAnimationOffState_001 end";
}

/**
 * @tc.number: SetBrightnessDiscount_001
 * @tc.name: SetBrightnessDiscount_001
 * @tc.desc: Test function SetBrightnessDiscount
 */
HWTEST_F(AccessibilityConfigImplTest, SetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_001 start";

    float brightness = 0.2;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetBrightnessDiscount(brightness);
    GTEST_LOG_(INFO) << "SetBrightnessDiscount_001 end";
}

/**
 * @tc.number: SetAudioMonoState_001
 * @tc.name: SetAudioMonoState_001
 * @tc.desc: Test function SetAudioMonoState
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioMonoState_001 start";

    bool state = true;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetAudioMonoState(state);
    GTEST_LOG_(INFO) << "SetAudioMonoState_001 end";
}

/**
 * @tc.number: SetAudioBalance_001
 * @tc.name: SetAudioBalance_001
 * @tc.desc: Test function SetAudioBalance
 */
HWTEST_F(AccessibilityConfigImplTest, SetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAudioBalance_001 start";
    float balance = -1.0;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.SetAudioBalance(balance);
    GTEST_LOG_(INFO) << "SetAudioBalance_001 end";
}

/**
 * @tc.number: GetScreenMagnificationState_001
 * @tc.name: GetScreenMagnificationState_001
 * @tc.desc: Test function GetScreenMagnificationState
 */
HWTEST_F(AccessibilityConfigImplTest, GetScreenMagnificationState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetScreenMagnificationState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetScreenMagnificationState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetScreenMagnificationState_001 end";
}

/**
 * @tc.number: GetShortKeyState_001
 * @tc.name: GetShortKeyState_001
 * @tc.desc: Test function GetShortKeyState
 */
HWTEST_F(AccessibilityConfigImplTest, GetShortKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetShortKeyState_001 start";
    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetShortKeyState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetShortKeyState_001 end";
}

/**
 * @tc.number: GetMouseKeyState_001
 * @tc.name: GetMouseKeyState_001
 * @tc.desc: Test function GetMouseKeyState
 */
HWTEST_F(AccessibilityConfigImplTest, GetMouseKeyState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetMouseKeyState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetMouseKeyState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetMouseKeyState_001 end";
}

/**
 * @tc.number: GetCaptionsState_001
 * @tc.name: GetCaptionsState_001
 * @tc.desc: Test function GetCaptionsState
 */
HWTEST_F(AccessibilityConfigImplTest, GetCaptionsState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCaptionsState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetCaptionsState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetCaptionsState_001 end";
}

/**
 * @tc.number: GetCaptionsProperty_001
 * @tc.name: GetCaptionsProperty_001
 * @tc.desc: Test function GetCaptionsProperty
 */
HWTEST_F(AccessibilityConfigImplTest, GetCaptionsProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCaptionsProperty_001 start";


    CaptionProperty value;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetCaptionsProperty(value);
    GTEST_LOG_(INFO) << "GetCaptionsProperty_001 end";
}

/**
 * @tc.number: GetMouseAutoClick_001
 * @tc.name: GetMouseAutoClick_001
 * @tc.desc: Test function GetMouseAutoClick
 */
HWTEST_F(AccessibilityConfigImplTest, GetMouseAutoClick_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetMouseAutoClick_001 start";


    int32_t value = 0;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetMouseAutoClick(value);
    EXPECT_EQ(MOUSE_AUTO_CLICK_VALUE, value);
    GTEST_LOG_(INFO) << "GetMouseAutoClick_001 end";
}

/**
 * @tc.number: GetShortkeyTarget_001
 * @tc.name: GetShortkeyTarget_001
 * @tc.desc: Test function GetShortkeyTarget
 */
HWTEST_F(AccessibilityConfigImplTest, GetShortkeyTarget_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetShortkeyTarget_001 start";

    std::string value = "";
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetShortkeyTarget(value);
    EXPECT_STREQ(value.c_str(), "test");
    GTEST_LOG_(INFO) << "GetShortkeyTarget_001 end";
}

/**
 * @tc.number: GetInvertColorState_001
 * @tc.name: GetInvertColorState_001
 * @tc.desc: Test function GetInvertColorState
 */
HWTEST_F(AccessibilityConfigImplTest, GetInvertColorState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInvertColorState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetInvertColorState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetInvertColorState_001 end";
}

/**
 * @tc.number:GetHighContrastTextState _001
 * @tc.name: GetHighContrastTextState_001
 * @tc.desc: Test function GetHighContrastTextState
 */
HWTEST_F(AccessibilityConfigImplTest, GetHighContrastTextState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetHighContrastTextState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetHighContrastTextState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetHighContrastTextState_001 end";
}

/**
 * @tc.number: GetDaltonizationColorFilter_001
 * @tc.name: GetDaltonizationColorFilter_001
 * @tc.desc: Test function GetDaltonizationColorFilter
 */
HWTEST_F(AccessibilityConfigImplTest, GetDaltonizationColorFilter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "_001 start";

    DALTONIZATION_TYPE value = Normal;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetDaltonizationColorFilter(value);
    EXPECT_EQ(1, static_cast<uint32_t>(value));
    GTEST_LOG_(INFO) << "_001 end";
}

/**
 * @tc.number: GetContentTimeout_001
 * @tc.name: GetContentTimeout_001
 * @tc.desc: Test function GetContentTimeout
 */
HWTEST_F(AccessibilityConfigImplTest, GetContentTimeout_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetContentTimeout_001 start";

    uint32_t value = 0;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetContentTimeout(value);
    EXPECT_EQ(CONTENT_TIMEOUT_VALUE, value);
    GTEST_LOG_(INFO) << "GetContentTimeout_001 end";
}

/**
 * @tc.number: GetAnimationOffState_001
 * @tc.name: GetAnimationOffState_001
 * @tc.desc: Test function GetAnimationOffState
 */
HWTEST_F(AccessibilityConfigImplTest, GetAnimationOffState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnimationOffState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetAnimationOffState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetAnimationOffState_001 end";
}

/**
 * @tc.number: GetBrightnessDiscount_001
 * @tc.name: GetBrightnessDiscount_001
 * @tc.desc: Test function GetBrightnessDiscount
 */
HWTEST_F(AccessibilityConfigImplTest, GetBrightnessDiscount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetBrightnessDiscount_001 start";

    float value = 0;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetBrightnessDiscount(value);
    EXPECT_FLOAT_EQ(BRIGHTNESS_DISCOUNT_VALUE, value);
    GTEST_LOG_(INFO) << "GetBrightnessDiscount_001 end";
}

/**
 * @tc.number: GetAudioMonoState_001
 * @tc.name: GetAudioMonoState_001
 * @tc.desc: Test function GetAudioMonoState
 */
HWTEST_F(AccessibilityConfigImplTest, GetAudioMonoState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAudioMonoState_001 start";

    bool value = false;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetAudioMonoState(value);
    EXPECT_TRUE(value);
    GTEST_LOG_(INFO) << "GetAudioMonoState_001 end";
}

/**
 * @tc.number: GetAudioBalance_001
 * @tc.name: GetAudioBalance_001
 * @tc.desc: Test function GetAudioBalance
 */
HWTEST_F(AccessibilityConfigImplTest, GetAudioBalance_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAudioBalance_001 start";

    float value = 0;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.GetAudioBalance(value);
    EXPECT_FLOAT_EQ(-1, value);
    GTEST_LOG_(INFO) << "GetAudioBalance_001 end";
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
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
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
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
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

    std::shared_ptr<AccessibilityEnableAbilityListsObserver> observer = nullptr;
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
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
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
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
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.EnableAbility(name, 0);
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
    auto &instance = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
    instance.InitializeContext();
    instance.DisableAbility(name);
    GTEST_LOG_(INFO) << "DisableAbility_001 end";
}
} // namespace AccessibilityConfig
} // namespace OHOS