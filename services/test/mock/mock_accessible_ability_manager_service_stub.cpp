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
        runner_ = AppExecFwk::EventRunner::Create("Accessibility.Config.Unittest", AppExecFwk::ThreadMode::FFRT);
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

ErrCode MockAccessibleAbilityManagerServiceStub::SendEvent(const AccessibilityEventInfoParcel& eventInfoParcel,
    int32_t flag)
{
    (void)eventInfoParcel;
    (void)flag;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetCaptionProperty(
    const CaptionPropertyParcel &caption)
{
    captionProperty_ = caption;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetCaptionState(bool state)
{
    captionState_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& observer, uint32_t& state)
{
    (void)observer;
    (void)state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfoParcel>& infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterElementOperatorByWindowId(int32_t windowId,
    const sptr<IAccessibilityElementOperator> &elementOperator)
{
    (void)windowId;
    (void)elementOperator;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterElementOperatorByParameter(const RegistrationPara& parameter,
    const sptr<IAccessibilityElementOperator>& elementOperator)
{
    (void)parameter;
    (void)elementOperator;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DeregisterElementOperatorByWindowId(int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DeregisterElementOperatorByWindowIdAndTreeId(int32_t windowId,
    int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetCaptionProperty(CaptionPropertyParcel& caption)
{
    (void)caption;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &observer)
{
    captionObserver_ = observer;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetCaptionState(bool &state)
{
    state = captionState_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetScreenReaderState(bool &state)
{
    (void)state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::EnableAbility(const std::string &name, const uint32_t capabilities)
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

ErrCode MockAccessibleAbilityManagerServiceStub::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DisableAbility(const std::string &name)
{
    (void)name;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetMagnificationState(const bool state)
{
    (void)state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetActiveWindow(int32_t& windowId)
{
    (void)windowId;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetActiveWindow(int32_t& windowId, bool systemApi)
{
    (void)windowId;
    (void)systemApi;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::CheckExtensionAbilityPermission(std::string& processName)
{
    (void)processName;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    (void)obj;
    return RET_ERR_IPC_FAILED;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DisableUITestAbility()
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetScreenMagnificationState(bool state)
{
    screenMagnifier_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetShortKeyState(bool state)
{
    shortkey_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetMouseKeyState(bool state)
{
    mouseKey_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetMouseAutoClick(int32_t time)
{
    mouseAutoClick_ = time;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetShortkeyTarget(const std::string &name)
{
    shortkeyTarget_ = name;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    shortkeyMultiTarget_ = name;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetHighContrastTextState(bool state)
{
    highContrastText_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetInvertColorState(bool state)
{
    invertColor_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetAnimationOffState(bool state)
{
    animationOff_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetAudioMonoState(bool state)
{
    audioMono_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetDaltonizationState(bool state)
{
    daltonizationState_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetDaltonizationColorFilter(uint32_t filter)
{
    daltonizationColorFilter_ = filter;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetContentTimeout(uint32_t time)
{
    contentTimeout_ = time;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetBrightnessDiscount(float discount)
{
    brightnessDiscount_ = discount;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetAudioBalance(float balance)
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

ErrCode MockAccessibleAbilityManagerServiceStub::SetClickResponseTime(uint32_t time)
{
    clickResponseTime_ = time;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetIgnoreRepeatClickState(bool state)
{
    ignoreRepeatClickState_ = state;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SetIgnoreRepeatClickTime(uint32_t time)
{
    ignoreRepeatClickTime_ = time;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetScreenMagnificationState(bool &state)
{
    state = screenMagnifier_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetShortKeyState(bool &state)
{
    state = shortkey_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetMouseKeyState(bool &state)
{
    state = mouseKey_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetMouseAutoClick(int32_t &time)
{
    time = mouseAutoClick_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetShortkeyTarget(std::string &name)
{
    name = shortkeyTarget_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    name = shortkeyMultiTarget_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetHighContrastTextState(bool &state)
{
    state = highContrastText_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetInvertColorState(bool &state)
{
    state = invertColor_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetAnimationOffState(bool &state)
{
    state = animationOff_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetAudioMonoState(bool &state)
{
    state = audioMono_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetDaltonizationState(bool &state)
{
    state = daltonizationState_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetDaltonizationColorFilter(uint32_t &type)
{
    type = daltonizationColorFilter_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetContentTimeout(uint32_t &timer)
{
    timer = contentTimeout_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetBrightnessDiscount(float &brightness)
{
    brightness = brightnessDiscount_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetAudioBalance(float &balance)
{
    balance = audioBalance_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetClickResponseTime(uint32_t &time)
{
    time = clickResponseTime_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetIgnoreRepeatClickState(bool &state)
{
    state = ignoreRepeatClickState_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetIgnoreRepeatClickTime(uint32_t &time)
{
    time = ignoreRepeatClickTime_;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetAllConfigs(AccessibilityConfigData& configData,
    CaptionPropertyParcel& caption)
{
    configData.highContrastText_ = true;
    configData.invertColor_ = true;
    configData.animationOff_ = true;
    configData.audioMono_ = true;
    configData.mouseKey_ = true;
    configData.captionState_ = true;
    configData.screenMagnifier_ = true;
    configData.shortkey_ = true;
    configData.daltonizationState_ = true;
    configData.mouseAutoClick_ = MOUSE_AUTO_CLICK_VALUE;
    configData.daltonizationColorFilter_ = 1;
    configData.contentTimeout_ = CONTENT_TIMEOUT_VALUE;
    configData.brightnessDiscount_ = BRIGHTNESS_DISCOUNT_VALUE;
    configData.audioBalance_ = -1;
    configData.shortkeyTarget_ = "test";
    configData.shortkeyMultiTarget_ = {"test"};
    configData.clickResponseTime_ = 1;
    configData.ignoreRepeatClickState_ = false;
    configData.ignoreRepeatClickTime_ = 1;
    (void)caption;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    abilityObserver_ = observer;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &observer)
{
    observer_ = observer;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetFocusedWindowId(int32_t &focusedWindowId)
{
    focusedWindowId = 1;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::RemoveRequestId(int32_t requestId)
{
    (void)requestId;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetRootParentId(int32_t windowId, int32_t treeId,
    int64_t& parentId)
{
    (void)windowId;
    (void)treeId;
    (void)parentId;
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetRootParentId(int32_t windowId, int32_t treeId,
    int64_t& parentId, bool systemApi)
{
    (void)windowId;
    (void)treeId;
    (void)parentId;
    (void)systemApi;
    return RET_OK;
}

int32_t MockAccessibleAbilityManagerServiceStub::SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData)
{
    return 0;
}

ErrCode MockAccessibleAbilityManagerServiceStub::SearchNeedEvents(std::vector<uint32_t> &needEvents)
{
    return RET_OK;
}

RetError MockAccessibleAbilityManagerServiceStub::UpdateUITestConfigureEvents(std::vector<uint32_t> needEvents)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::GetReadableRules(std::string &name)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DeRegisterCaptionObserver(
    const sptr<IRemoteObject>& obj)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DeRegisterEnableAbilityListsObserver(
    const sptr<IRemoteObject>& obj)
{
    return RET_OK;
}

ErrCode MockAccessibleAbilityManagerServiceStub::DeRegisterConfigObserver(
    const sptr<IRemoteObject>& obj)
{
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS