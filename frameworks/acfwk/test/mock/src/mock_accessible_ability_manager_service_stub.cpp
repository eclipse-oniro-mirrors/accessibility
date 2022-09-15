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
    (void)caption;
}

void MockAccessibleAbilityManagerServiceStub::SetCaptionState(const bool state)
{
    (void)state;
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
    AccessibilityConfig::CaptionProperty property = {};
    return property;
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
    return true;
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
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetShortKeyState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetMouseKeyState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetMouseAutoClick(const int32_t time)
{
    (void)time;
}

void MockAccessibleAbilityManagerServiceStub::SetShortkeyTarget(const std::string &name)
{
    (void)name;
}

void MockAccessibleAbilityManagerServiceStub::SetHighContrastTextState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetInvertColorState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetAnimationOffState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetAudioMonoState(const bool state)
{
    (void)state;
}

void MockAccessibleAbilityManagerServiceStub::SetDaltonizationColorFilter(const uint32_t filter)
{
    (void)filter;
}

void MockAccessibleAbilityManagerServiceStub::SetContentTimeout(const uint32_t time)
{
    (void)time;
}

void MockAccessibleAbilityManagerServiceStub::SetBrightnessDiscount(const float discount)
{
    (void)discount;
}

void MockAccessibleAbilityManagerServiceStub::SetAudioBalance(const float balance)
{
    (void)balance;
    if (handler_) {
        handler_->PostTask(std::bind([this]() {
            observer_->OnConfigStateChanged(0);
            observer_->OnAudioBalanceChanged(0);
            observer_->OnBrightnessDiscountChanged(0);
            observer_->OnContentTimeoutChanged(0);
            observer_->OnMouseAutoClickChanged(0);
            observer_->OnDaltonizationColorFilterChanged(0);
            observer_->OnShortkeyTargetChanged("test_target");
            AccessibilityConfig::CaptionProperty testProperty;
            captionObserver_->OnPropertyChanged(testProperty);
            }), "NotifyAll");
    }
}

bool MockAccessibleAbilityManagerServiceStub::GetScreenMagnificationState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetShortKeyState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetMouseKeyState()
{
    return true;
}

int32_t MockAccessibleAbilityManagerServiceStub::GetMouseAutoClick()
{
    return MOUSE_AUTO_CLICK_VALUE;
}

std::string MockAccessibleAbilityManagerServiceStub::GetShortkeyTarget()
{
    return "test";
}

bool MockAccessibleAbilityManagerServiceStub::GetHighContrastTextState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetInvertColorState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetAnimationOffState()
{
    return true;
}

bool MockAccessibleAbilityManagerServiceStub::GetAudioMonoState()
{
    return true;
}

uint32_t MockAccessibleAbilityManagerServiceStub::GetDaltonizationColorFilter()
{
    return 1;
}

uint32_t MockAccessibleAbilityManagerServiceStub::GetContentTimeout()
{
    return CONTENT_TIMEOUT_VALUE;
}

float MockAccessibleAbilityManagerServiceStub::GetBrightnessDiscount()
{
    return BRIGHTNESS_DISCOUNT_VALUE;
}

float MockAccessibleAbilityManagerServiceStub::GetAudioBalance()
{
    return -1;
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
    (void)observer;
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