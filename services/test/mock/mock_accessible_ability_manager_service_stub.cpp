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
        runner_ = AppExecFwk::EventRunner::Create("Accessibility.Config.Unittest");
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

RetError MockAccessibleAbilityManagerServiceStub::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    (void)uiEvent;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetCaptionProperty(
    const AccessibilityConfig::CaptionProperty &caption)
{
    captionProperty_ = caption;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetCaptionState(const bool state)
{
    captionState_ = state;
    return RET_OK;
}

uint32_t MockAccessibleAbilityManagerServiceStub::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &callback)
{
    (void)callback;
    return 0;
}

RetError MockAccessibleAbilityManagerServiceStub::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::RegisterElementOperator(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &operation)
{
    (void)windowId;
    (void)operation;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::DeregisterElementOperator(const int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    caption = captionProperty_;
    return RET_OK;
}

uint32_t MockAccessibleAbilityManagerServiceStub::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback)
{
    captionObserver_ = callback;
    return 0;
}

bool MockAccessibleAbilityManagerServiceStub::GetEnabledState()
{
    return true;
}

RetError MockAccessibleAbilityManagerServiceStub::GetCaptionState(bool &state)
{
    state = captionState_;
    return RET_OK;
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

RetError MockAccessibleAbilityManagerServiceStub::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    if (handler_) {
        handler_->PostTask(std::bind([this]() {
            abilityObserver_->OnAccessibilityEnableAbilityListsChanged();
            }), "NotifyEnableAbility");
    }
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::DisableAbility(const std::string &name)
{
    (void)name;
    return RET_OK;
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

RetError MockAccessibleAbilityManagerServiceStub::DisableUITestAbility()
{
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetScreenMagnificationState(const bool state)
{
    screenMagnifier_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetShortKeyState(const bool state)
{
    shortkey_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetMouseKeyState(const bool state)
{
    mouseKey_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetMouseAutoClick(const int32_t time)
{
    mouseAutoClick_ = time;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetShortkeyTarget(const std::string &name)
{
    shortkeyTarget_ = name;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetHighContrastTextState(const bool state)
{
    highContrastText_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetInvertColorState(const bool state)
{
    invertColor_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetAnimationOffState(const bool state)
{
    animationOff_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetAudioMonoState(const bool state)
{
    audioMono_ = state;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetDaltonizationColorFilter(const uint32_t filter)
{
    daltonizationColorFilter_ = filter;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetContentTimeout(const uint32_t time)
{
    contentTimeout_ = time;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetBrightnessDiscount(const float discount)
{
    brightnessDiscount_ = discount;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::SetAudioBalance(const float balance)
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
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetScreenMagnificationState(bool &state)
{
    state = screenMagnifier_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetShortKeyState(bool &state)
{
    state = shortkey_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetMouseKeyState(bool &state)
{
    state = mouseKey_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetMouseAutoClick(int32_t &time)
{
    time = mouseAutoClick_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetShortkeyTarget(std::string &name)
{
    name = shortkeyTarget_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetHighContrastTextState(bool &state)
{
    state = highContrastText_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetInvertColorState(bool &state)
{
    state = invertColor_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetAnimationOffState(bool &state)
{
    state = animationOff_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetAudioMonoState(bool &state)
{
    state = audioMono_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetDaltonizationColorFilter(uint32_t &type)
{
    type = daltonizationColorFilter_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetContentTimeout(uint32_t &timer)
{
    timer = contentTimeout_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetBrightnessDiscount(float &brightness)
{
    brightness = brightnessDiscount_;
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::GetAudioBalance(float &balance)
{
    balance = audioBalance_;
    return RET_OK;
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
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    observer_ = callback;
    return 0;
}
} // namespace Accessibility
} // namespace OHOS