/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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
#include "iaccessible_ability_manager_service.h"
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

    ErrCode SendEvent(const AccessibilityEventInfoParcel& eventInfoParcel, int32_t flag) override;

    ErrCode RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver>& observer,
        uint32_t& state) override;

    ErrCode RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver>& observer) override;

    ErrCode RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver>& observer) override;

    ErrCode GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfoParcel>& infos) override;

    ErrCode RegisterElementOperatorByWindowId(const int32_t windowId,
        const sptr<IAccessibilityElementOperator>& elementOperator) override;

    ErrCode RegisterElementOperatorByParameter(const RegistrationPara& parameter,
        const sptr<IAccessibilityElementOperator> &elementOperator) override;

    ErrCode DeregisterElementOperatorByWindowId(int32_t windowId) override;

    ErrCode DeregisterElementOperatorByWindowIdAndTreeId(int32_t windowId, int32_t treeId) override;

    ErrCode GetCaptionProperty(CaptionPropertyParcel& caption) override;
    ErrCode SetCaptionProperty(const CaptionPropertyParcel& caption) override;
    ErrCode SetCaptionState(const bool state) override;

    ErrCode GetCaptionState(bool& state) override;
    ErrCode GetScreenReaderState(bool& state) override;

    ErrCode EnableAbility(const std::string& name, uint32_t capabilities) override;
    ErrCode GetEnabledAbilities(std::vector<std::string>& enabledAbilities) override;

    ErrCode DisableAbility(const std::string& name) override;
    ErrCode SetMagnificationState(const bool state) override;
    ErrCode GetActiveWindow(int32_t& windowId) override;

    ErrCode CheckExtensionAbilityPermission(std::string& processName) override;
    ErrCode EnableUITestAbility(const sptr<IRemoteObject>& obj) override;
    ErrCode DisableUITestAbility() override;

    ErrCode SetScreenMagnificationState(bool state) override;
    ErrCode SetShortKeyState(bool state) override;
    ErrCode SetMouseKeyState(bool state) override;
    ErrCode SetMouseAutoClick(int32_t time) override;
    ErrCode SetShortkeyTarget(const std::string &name) override;
    ErrCode SetShortkeyMultiTarget(const std::vector<std::string> &name) override;
    ErrCode SetHighContrastTextState(bool state) override;
    ErrCode SetInvertColorState(bool state) override;
    ErrCode SetAnimationOffState(bool state) override;
    ErrCode SetAudioMonoState(bool state) override;
    ErrCode SetDaltonizationState(bool state) override;
    ErrCode SetDaltonizationColorFilter(uint32_t filter) override;
    ErrCode SetContentTimeout(uint32_t time) override;
    ErrCode SetBrightnessDiscount(float discount) override;
    ErrCode SetAudioBalance(float balance) override;
    ErrCode SetClickResponseTime(uint32_t time) override;
    ErrCode SetIgnoreRepeatClickState(bool state) override;
    ErrCode SetIgnoreRepeatClickTime(uint32_t time) override;

    ErrCode GetScreenMagnificationState(bool &state) override;
    ErrCode GetShortKeyState(bool &state) override;
    ErrCode GetMouseKeyState(bool &state) override;
    ErrCode GetMouseAutoClick(int32_t &time) override;
    ErrCode GetShortkeyTarget(std::string &name) override;
    ErrCode GetShortkeyMultiTarget(std::vector<std::string> &name) override;
    ErrCode GetHighContrastTextState(bool &state) override;
    ErrCode GetInvertColorState(bool &state) override;
    ErrCode GetAnimationOffState(bool &state) override;
    ErrCode GetAudioMonoState(bool &state) override;
    ErrCode GetDaltonizationState(bool &state) override;
    ErrCode GetDaltonizationColorFilter(uint32_t &type) override;
    ErrCode GetContentTimeout(uint32_t &timer) override;
    ErrCode GetBrightnessDiscount(float &brightness) override;
    ErrCode GetAudioBalance(float &balance) override;
    ErrCode GetClickResponseTime(uint32_t &time) override;
    ErrCode GetIgnoreRepeatClickState(bool &state) override;
    ErrCode GetIgnoreRepeatClickTime(uint32_t &time) override;
    ErrCode GetAllConfigs(AccessibilityConfigData& configData, CaptionPropertyParcel& caption) override;
    ErrCode RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver>& observer) override;
    ErrCode GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId) override;
    ErrCode GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid) override;
    ErrCode GetFocusedWindowId(int32_t &focusedWindowId) override;
    ErrCode RemoveRequestId(int32_t requestId) override;
    ErrCode GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId) override;
    int32_t SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData) override;
    ErrCode SearchNeedEvents(std::vector<uint32_t> &needEvents) override;
    RetError UpdateUITestConfigureEvents(std::vector<uint32_t> needEvents);

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
    bool daltonizationState_ = false;
    int32_t mouseAutoClick_ = 0;
    uint32_t contentTimeout_ = 0;
    uint32_t daltonizationColorFilter_ = 0;
    float audioBalance_ = 0.0;
    float brightnessDiscount_ = 0.0;
    std::string shortkeyTarget_ = "";
    std::vector<std::string> shortkeyMultiTarget_ {};
    uint32_t clickResponseTime_ = 0;
    bool ignoreRepeatClickState_ = false;
    uint32_t ignoreRepeatClickTime_ = 0;
    CaptionPropertyParcel captionProperty_ = {};
};
} // namespace Accessibility
} // namespace OHOS
#endif