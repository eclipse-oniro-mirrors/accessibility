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
#include "i_accessible_ability_manager_caption_observer.h"
#include "i_accessible_ability_manager_state_observer.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class IAccessibleAbilityManagerService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerService");

    /**
     * @brief Sends information about an accessibility event.
     * @param uiEvent Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @param accountId The user id.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t accountId) = 0;

    /**
     * @brief Register the state observer of AAMS.
     * @param callback state observer
     * @param accountId The user id.
     * @return 0: Register ok; otherwise is refused.
     */
    virtual uint32_t RegisterStateObserver(
        const sptr<IAccessibleAbilityManagerStateObserver> &callback, const int32_t accountId) = 0;

    virtual uint32_t RegisterCaptionObserver(
        const sptr<IAccessibleAbilityManagerCaptionObserver> &callback, const int32_t accountId) = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status. 1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled; 4 indicates that the ability has been installed.
     * @return
     */
    virtual std::vector<AccessibilityAbilityInfo> GetAbilityList(const uint32_t abilityTypes,
        const int32_t stateType) = 0;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @param accountId User ID
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual void RegisterElementOperator(
        const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation, const int32_t accountId) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) = 0;

    virtual CaptionProperty GetCaptionProperty() = 0;
    virtual bool GetEnabledState() = 0;
    virtual bool GetCaptionState() = 0;
    virtual bool GetTouchGuideState() = 0;
    virtual bool GetGestureState() = 0;
    virtual bool GetKeyEventObserverState() = 0;

    virtual bool SetCaptionProperty(const CaptionProperty &caption) = 0;
    virtual bool SetCaptionState(const bool state) = 0;

    virtual bool EnableAbilities(std::vector<std::string> &abilities) = 0;
    virtual std::vector<std::string> GetEnabledAbilities() = 0;
    virtual std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() = 0;

    virtual bool DisableAbilities(std::vector<std::string> &abilities) =0;
    virtual int32_t GetActiveWindow() = 0;

    virtual bool EnableUITestAbility(const sptr<IRemoteObject> &obj) = 0;
    virtual bool DisableUITestAbility() = 0;

    enum class Message {
        SEND_EVENT = 0,
        REGISTER_STATE_CALLBACK,
        GET_ABILITYLIST,
        REGISTER_INTERACTION_CONNECTION,
        DEREGISTER_INTERACTION_CONNECTION,
        GET_CAPTION_PROPERTY,
        SET_CAPTION_PROPERTY,
        SET_CAPTION_STATE,
        REGISTER_CAPTION_PROPERTY_CALLBACK,
        GET_ENABLED,
        GET_CAPTION_STATE,
        GET_TOUCH_GUIDE_STATE,
        GET_GESTURE_STATE,
        GET_KEY_EVENT_OBSERVE_STATE,
        ENABLE_ABILITIES,
        GET_ENABLED_OBJECT,
        GET_INSTALLED,
        DISABLE_ABILITIES,
        ENABLE_UI_TEST_ABILITY,
        DISABLE_UI_TEST_ABILITY,
        GET_ACTIVE_WINDOW,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_H