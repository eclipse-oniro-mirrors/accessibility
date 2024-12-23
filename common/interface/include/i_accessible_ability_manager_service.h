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

#ifndef INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H
#define INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H

#include <map>
#include <vector>
#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_event_info.h"
#include "i_accessibility_element_operator.h"
#include "i_accessibility_enable_ability_lists_observer.h"
#include "i_accessible_ability_manager_caption_observer.h"
#include "i_accessible_ability_manager_config_observer.h"
#include "i_accessible_ability_manager_state_observer.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
struct AccessibilityConfigData {
    bool highContrastText_ = false;
    bool invertColor_ = false;
    bool animationOff_ = false;
    bool audioMono_ = false;
    bool mouseKey_ = false;
    bool captionState_ = false;
    bool screenMagnifier_ = false;
    bool shortkey_ = false;
    int32_t mouseAutoClick_ = 0;
    bool daltonizationState_ = false;
    uint32_t daltonizationColorFilter_ = 0;
    uint32_t contentTimeout_ = 0;
    float brightnessDiscount_ = 0.0;
    float audioBalance_ = 0.0;
    std::string shortkeyTarget_ = "";
    uint32_t clickResponseTime_ = 0;
    bool ignoreRepeatClickState_ = false;
    uint32_t ignoreRepeatClickTime_ = 0;
    AccessibilityConfig::CaptionProperty captionProperty_ = {};
    std::vector<std::string> shortkeyMultiTarget_ = {};
};

class IAccessibleAbilityManagerService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerService");

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t flag = 0) = 0;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) = 0;

    virtual uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) = 0;

    virtual void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by AccessibilityAbilityTypes.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  3 indicates that the ability has been installed.
     * @param infos accessibility ability infos by specified types.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) = 0;

    /**
     * @brief Register the element operator, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param isApp Whether an application.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation, bool isApp = true) = 0;

    /**
     * @brief Register the element operator, so the AA can get node info from ACE.
     * @param parameter The Register parameters.
     * @param operation The callback object.
     * @param isApp whether an application.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterElementOperator(Registration parameter,
        const sptr<IAccessibilityElementOperator> &operation, bool isApp = false) = 0;

    /**
     * @brief Deregister the element operator.
     * @param windowId Window ID
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError DeregisterElementOperator(const int32_t windowId) = 0;

    /**
     * @brief Deregister the element operator.
     * @param windowId Window ID
     * @param treeId Tree ID
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError DeregisterElementOperator(const int32_t windowId, const int32_t treeId) = 0;

    virtual RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption) = 0;
    virtual bool GetEnabledState() = 0;
    virtual RetError GetCaptionState(bool &state) = 0;
    virtual bool GetTouchGuideState() = 0;
    virtual bool GetGestureState() = 0;
    virtual bool GetKeyEventObserverState() = 0;

    virtual RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) = 0;
    virtual RetError SetCaptionState(const bool state) = 0;

    virtual RetError EnableAbility(const std::string &name, const uint32_t capabilities) = 0;
    virtual RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) = 0;
    virtual RetError DisableAbility(const std::string &name) = 0;
    virtual int32_t GetActiveWindow() = 0;

    virtual RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) = 0;
    virtual RetError DisableUITestAbility() = 0;

    virtual RetError SetScreenMagnificationState(const bool state) = 0;
    virtual RetError SetShortKeyState(const bool state) = 0;
    virtual RetError SetMouseKeyState(const bool state) = 0;
    virtual RetError SetMouseAutoClick(const int32_t time) = 0;
    virtual RetError SetShortkeyTarget(const std::string &name) = 0;
    virtual RetError SetShortkeyMultiTarget(const std::vector<std::string> &name) = 0;
    virtual RetError SetHighContrastTextState(const bool state) = 0;
    virtual RetError SetInvertColorState(const bool state) = 0;
    virtual RetError SetAnimationOffState(const bool state) = 0;
    virtual RetError SetAudioMonoState(const bool state) = 0;
    virtual RetError SetDaltonizationState(const bool state) = 0;
    virtual RetError SetDaltonizationColorFilter(const uint32_t filter) = 0;
    virtual RetError SetContentTimeout(const uint32_t time) = 0;
    virtual RetError SetBrightnessDiscount(const float discount) = 0;
    virtual RetError SetAudioBalance(const float balance) = 0;
    virtual RetError SetClickResponseTime(const uint32_t time) = 0;
    virtual RetError SetIgnoreRepeatClickState(const bool state) = 0;
    virtual RetError SetIgnoreRepeatClickTime(const uint32_t time) = 0;

    virtual RetError GetScreenMagnificationState(bool &state) = 0;
    virtual RetError GetShortKeyState(bool &state) = 0;
    virtual RetError GetMouseKeyState(bool &state) = 0;
    virtual RetError GetMouseAutoClick(int32_t &time) = 0;
    virtual RetError GetShortkeyTarget(std::string &name) = 0;
    virtual RetError GetShortkeyMultiTarget(std::vector<std::string> &name) = 0;
    virtual RetError GetHighContrastTextState(bool &state) = 0;
    virtual RetError GetInvertColorState(bool &state) = 0;
    virtual RetError GetAnimationOffState(bool &state) = 0;
    virtual RetError GetAudioMonoState(bool &state) = 0;
    virtual RetError GetDaltonizationState(bool &state) = 0;
    virtual RetError GetDaltonizationColorFilter(uint32_t &type) = 0;
    virtual RetError GetContentTimeout(uint32_t &timer) = 0;
    virtual RetError GetBrightnessDiscount(float &brightness) = 0;
    virtual RetError GetAudioBalance(float &balance) = 0;
    virtual RetError GetClickResponseTime(uint32_t& time) = 0;
    virtual RetError GetIgnoreRepeatClickState(bool& state) = 0;
    virtual RetError GetIgnoreRepeatClickTime(uint32_t& time) = 0;
    virtual void GetAllConfigs(AccessibilityConfigData& configData) = 0;
    virtual void GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId) = 0;
    virtual void GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid) = 0;
    virtual RetError GetFocusedWindowId(int32_t &focusedWindowId) = 0;

    virtual uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) = 0;
    virtual void RemoveRequestId(int32_t requestId) = 0;

    virtual int64_t GetRootParentId(int32_t windowsId, int32_t treeId) = 0;
    virtual RetError GetAllTreeId(int32_t windowId, std::vector<int32_t> &treeIds) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H