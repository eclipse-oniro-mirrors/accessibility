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

#ifndef MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H
#define MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H

#include <map>
#include "i_accessible_ability_manager_service.h"
#include "accessibility_config_impl.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class MockAccessibleAbilityManagerServiceStub : public IRemoteStub<IAccessibleAbilityManagerService> {
public:
    MockAccessibleAbilityManagerServiceStub();

    virtual ~MockAccessibleAbilityManagerServiceStub();

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    RetError SendEvent(const AccessibilityEventInfo &uiEvent) override;

    uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) override;

    uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) override;

    void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) override;

    RetError GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) override;

    RetError RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation) override;

    RetError DeregisterElementOperator(const int32_t windowId) override;

    RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption) override;
    RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) override;
    RetError SetCaptionState(const bool state) override;

    bool GetEnabledState() override;
    RetError GetCaptionState(bool &state) override;
    bool GetTouchGuideState() override;
    bool GetGestureState() override;
    bool GetKeyEventObserverState() override;

    RetError EnableAbility(const std::string &name, const uint32_t capabilities) override;
    RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) override;

    RetError DisableAbility(const std::string &name) override;
    int32_t GetActiveWindow() override;

    RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) override;
    RetError DisableUITestAbility() override;

    RetError SetScreenMagnificationState(const bool state) override;
    RetError SetShortKeyState(const bool state) override;
    RetError SetMouseKeyState(const bool state) override;
    RetError SetMouseAutoClick(const int32_t time) override;
    RetError SetShortkeyTarget(const std::string &name) override;
    RetError SetHighContrastTextState(const bool state) override;
    RetError SetInvertColorState(const bool state) override;
    RetError SetAnimationOffState(const bool state) override;
    RetError SetAudioMonoState(const bool state) override;
    RetError SetDaltonizationColorFilter(const uint32_t filter) override;
    RetError SetContentTimeout(const uint32_t time) override;
    RetError SetBrightnessDiscount(const float discount) override;
    RetError SetAudioBalance(const float balance) override;

    RetError GetScreenMagnificationState(bool &state) override;
    RetError GetShortKeyState(bool &state) override;
    RetError GetMouseKeyState(bool &state) override;
    RetError GetMouseAutoClick(int32_t &time) override;
    RetError GetShortkeyTarget(std::string &name) override;
    RetError GetHighContrastTextState(bool &state) override;
    RetError GetInvertColorState(bool &state) override;
    RetError GetAnimationOffState(bool &state) override;
    RetError GetAudioMonoState(bool &state) override;
    RetError GetDaltonizationColorFilter(uint32_t &type) override;
    RetError GetContentTimeout(uint32_t &timer) override;
    RetError GetBrightnessDiscount(float &brightness) override;
    RetError GetAudioBalance(float &balance) override;
    void GetAllConfigs(AccessibilityConfigData &configData) override;
    uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) override;

private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    sptr<IAccessibleAbilityManagerConfigObserver> observer_ = nullptr;
    sptr<IAccessibleAbilityManagerCaptionObserver> captionObserver_ = nullptr;
    sptr<IAccessibilityEnableAbilityListsObserver> abilityObserver_ = nullptr;

    bool shortkey_ = false;
    bool highContrastText_ = false;
    bool screenMagnifier_ = false;
    bool invertColor_ = false;
    bool captionState_ = false;
    bool animationOff_ = false;
    bool audioMono_ = false;
    bool mouseKey_ = false;
    int32_t mouseAutoClick_ = 0;
    uint32_t contentTimeout_ = 0;
    uint32_t daltonizationColorFilter_ = 0;
    float audioBalance_ = 0.0;
    float brightnessDiscount_ = 0.0;
    std::string shortkeyTarget_ = "";
    AccessibilityConfig::CaptionProperty captionProperty_ = {};
};
} // namespace Accessibility
} // namespace OHOS
#endif