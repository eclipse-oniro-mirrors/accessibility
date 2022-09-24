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

#include "mock_accessible_ability_manager_service_stub.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t CONTENT_TIMEOUT_VALUE = 10;
    constexpr int32_t MOUSE_AUTO_CLICK_VALUE = 10;
    constexpr float BRIGHTNESS_DISCOUNT_VALUE = 0.3f;
} // namespace
MockAccessibleAbilityManagerServiceStub::MockAccessibleAbilityManagerServiceStub()
{
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create("accessibility.config.unittest");
        if (!runner_) {
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
        if (!handler_) {
            return;
        }
    }
}

MockAccessibleAbilityManagerServiceStub::~MockAccessibleAbilityManagerServiceStub()
{
}

int MockAccessibleAbilityManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return 0;
}

void MockAccessibleAbilityManagerServiceStub::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    (void)uiEvent;
}

void MockAccessibleAbilityManagerServiceStub::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    captionProperty_ = caption;
}

void MockAccessibleAbilityManagerServiceStub::SetCaptionState(const bool state)
{
    captionState_ = state;
}

uint32_t MockAccessibleAbilityManagerServiceStub::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &client)
{
    (void)client;
    return 0;
}

bool MockAccessibleAbilityManagerServiceStub::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return true;
}

void MockAccessibleAbilityManagerServiceStub::RegisterElementOperator(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &operation)
{
    (void)windowId;
    (void)operation;
}

void MockAccessibleAbilityManagerServiceStub::DeregisterElementOperator(const int32_t windowId)
{
    (void)windowId;
}

AccessibilityConfig::CaptionProperty MockAccessibleAbilityManagerServiceStub::GetCaptionProperty()
{
    return captionProperty_;
}

uint32_t MockAccessibleAbilityManagerServiceStub::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &client)
{
    captionObserver_ = client;
    return 0;
}

bool MockAccessibleAbilityManagerServiceStub::GetEnabledState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetCaptionState()
{
    return captionState_;
}

bool MockAccessibleAbilityManagerServiceStub::GetTouchGuideState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetGestureState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetKeyEventObserverState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    if (handler_) {
        handler_->PostTask(std::bind([this]() {
            abilityObserver_->OnAccessibilityEnableAbilityListsChanged();
            }), "NotifyEnableAbility");
    }
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetInstalledAbilities(
    std::vector<AccessibilityAbilityInfo> &installedAbilities)
{
    (void)installedAbilities;
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::DisableAbility(const std::string &name)
{
    (void)name;
    return true;
}

int32_t MockAccessibleAbilityManagerServiceStub::GetActiveWindow()
{
    return 0;
}

RetError MockAccessibleAbilityManagerServiceStub::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    (void)obj;
    return RET_ERR_IPC_FAILED;
}

bool MockAccessibleAbilityManagerServiceStub::DisableUITestAbility()
{
    return true;
}

void MockAccessibleAbilityManagerServiceStub::SetScreenMagnificationState(const bool state)
{
    screenMagnifier_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetShortKeyState(const bool state)
{
    shortkey_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetMouseKeyState(const bool state)
{
    mouseKey_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetMouseAutoClick(const int32_t time)
{
    mouseAutoClick_ = time;
}

void MockAccessibleAbilityManagerServiceStub::SetShortkeyTarget(const std::string &name)
{
    shortkeyTarget_ = name;
}

void MockAccessibleAbilityManagerServiceStub::SetHighContrastTextState(const bool state)
{
    highContrastText_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetInvertColorState(const bool state)
{
    invertColor_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetAnimationOffState(const bool state)
{
    animationOff_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetAudioMonoState(const bool state)
{
    audioMono_ = state;
}

void MockAccessibleAbilityManagerServiceStub::SetDaltonizationColorFilter(const uint32_t filter)
{
    daltonizationColorFilter_ = filter;
}

void MockAccessibleAbilityManagerServiceStub::SetContentTimeout(const uint32_t time)
{
    contentTimeout_ = time;
}

void MockAccessibleAbilityManagerServiceStub::SetBrightnessDiscount(const float discount)
{
    brightnessDiscount_ = discount;
}

void MockAccessibleAbilityManagerServiceStub::SetAudioBalance(const float balance)
{
    audioBalance_ = balance;

    if (handler_) {
        handler_->PostTask(std::bind([this, balance]() {
            observer_->OnConfigStateChanged(balance == 0 ? 0 : 0xFFFF);
            observer_->OnAudioBalanceChanged(balance == 0 ? 0 : 1);
            observer_->OnBrightnessDiscountChanged(balance == 0 ? 0 : 1);
            observer_->OnContentTimeoutChanged(balance == 0 ? 0 : 1);
            observer_->OnMouseAutoClickChanged(balance == 0 ? 0 : 1);
            observer_->OnDaltonizationColorFilterChanged(balance == 0 ? 0 : 1);
            observer_->OnShortkeyTargetChanged(balance == 0 ? "test_target1" : "test_target2");
            AccessibilityConfig::CaptionProperty testProperty;
            captionObserver_->OnPropertyChanged(testProperty);
            }), "NotifyAll");
    }
}

bool MockAccessibleAbilityManagerServiceStub::GetScreenMagnificationState()
{
    return screenMagnifier_;
}

bool MockAccessibleAbilityManagerServiceStub::GetShortKeyState()
{
    return shortkey_;
}

bool MockAccessibleAbilityManagerServiceStub::GetMouseKeyState()
{
    return mouseKey_;
}

int32_t MockAccessibleAbilityManagerServiceStub::GetMouseAutoClick()
{
    return mouseAutoClick_;
}

std::string MockAccessibleAbilityManagerServiceStub::GetShortkeyTarget()
{
    return shortkeyTarget_;
}

bool MockAccessibleAbilityManagerServiceStub::GetHighContrastTextState()
{
    return highContrastText_;
}

bool MockAccessibleAbilityManagerServiceStub::GetInvertColorState()
{
    return invertColor_;
}

bool MockAccessibleAbilityManagerServiceStub::GetAnimationOffState()
{
    return animationOff_;
}

bool MockAccessibleAbilityManagerServiceStub::GetAudioMonoState()
{
    return audioMono_;
}

uint32_t MockAccessibleAbilityManagerServiceStub::GetDaltonizationColorFilter()
{
    return daltonizationColorFilter_;
}

uint32_t MockAccessibleAbilityManagerServiceStub::GetContentTimeout()
{
    return contentTimeout_;
}

float MockAccessibleAbilityManagerServiceStub::GetBrightnessDiscount()
{
    return brightnessDiscount_;
}

float MockAccessibleAbilityManagerServiceStub::GetAudioBalance()
{
    return audioBalance_;
}

void MockAccessibleAbilityManagerServiceStub::GetAllConfigs(AccessibilityConfigData &configData)
{
    configData.highContrastText_ = true;
    configData.invertColor_ = true;
    configData.animationOff_ = true;
    configData.audioMono_ = true;
    configData.mouseKey_ = true;
    configData.captionState_ = true;
    configData.screenMagnifier_ = true;
    configData.shortkey_ = true;
    configData.mouseAutoClick_ = MOUSE_AUTO_CLICK_VALUE;
    configData.daltonizationColorFilter_ = 1;
    configData.contentTimeout_ = CONTENT_TIMEOUT_VALUE;
    configData.brightnessDiscount_ = BRIGHTNESS_DISCOUNT_VALUE;
    configData.audioBalance_ = -1;
    configData.shortkeyTarget_ = "test";
}

void MockAccessibleAbilityManagerServiceStub::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    abilityObserver_ = observer;
    return;
}

uint32_t MockAccessibleAbilityManagerServiceStub::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &client)
{
    observer_ = client;
    return 0;
}
} // namespace Accessibility
} // namespace OHOS