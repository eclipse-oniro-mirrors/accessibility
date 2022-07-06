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

#ifndef ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H
#define ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H

#include <map>
#include <vector>
#include "accessibility_ability_info.h"
#include "accessibility_element_operator.h"
#include "accessibility_event_info.h"
#include "accessibility_state_event.h"
#include "accessibility_window_info.h"

namespace OHOS {
namespace Accessibility {
enum AccessibilityControlType : int32_t {
    CONTENT_CONTROLS = 0x00000001,
    CONTENT_ICONS = 0x00000002,
    CONTENT_TEXT = 0x00000004,
};

/*
 * The class register the accessibility service observer to AAMS,and
 * dispatch the accessibility service status changed. such as Service Enable，
 * Accessibility Enable. It calls AAMS API to send the event to AA.
 * It supply sington instance for each process.
 */
class AccessibilitySystemAbilityClient {
public:
    /**
     * @brief Obtains the AccessibilitySystemAbilityClient instance.
     * @param -
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance();

    /**
     * @brief Deconstruct.
     * @param
     * @return
     */
    virtual ~AccessibilitySystemAbilityClient() = default;

    /**
     * @brief Register the interaction operation, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @return 0: Succeed ; otherwise is failed.
     */
    virtual int32_t RegisterElementOperator(const int32_t windowId,
        const std::shared_ptr<AccessibilityElementOperator> &operation) = 0;

    /**
     * @brief Deregister the interaction operation.
     * @param windowId Window ID
     * @return
     */
    virtual void DeregisterElementOperator(const int32_t windowId) = 0;

    /**
     * @brief Checks whether accessibility ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsEnabled() = 0;

    /**
     * @brief Checks whether touch exploration ability is enabled.
     * @param -
     * @return true: enabled; false: disabled
     */
    virtual bool IsTouchExplorationEnabled() = 0;

    /**
     * @brief Queries the list of accessibility abilities.
     * @param accessibilityAbilityTypes Indicates the accessibility type specified by
     *                                  AccessibilityAbilityInfo#ACCESSIBILITY_ABILITY_TYPE_SPOKEN.
     * @param stateType Indicates the accessibility ability status.
     *                  1 indicates that the ability is enabled;
     *                  2 indicates that the ability is disabled;
     *                  4 indicates that the ability has been installed.
     * @param infos accessibility ability infos by specified types.
     * @return Return true if get ability lists successfully, else return false.
     */
    virtual bool GetAbilityList(const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType,
        std::vector<AccessibilityAbilityInfo> &infos) = 0;

    /**
     * @brief Sends an accessibility event.
     * @param eventType  Identifies the accessibility event specified by AccessibilityEventInfo.
     * @param componentId Indicates the ID of the component to be associated with the event.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const EventType eventType, const int32_t componentId) = 0;

    /**
     * @brief Sends information about an accessibility event.
     * @param event Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SendEvent(const AccessibilityEventInfo &event) = 0;

    /**
     * @brief Subscribes to the specified type of accessibility status change events.
     * @param observer Indicates the observer for listening to status events, which is specified
     *              by AccessibilityStateObserver.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     *              #EVENT_SCREEN_MAGNIFIER_CHANGED and EVENT_AUTO_CLICK_CHANGED and #EVENT_SHORT_KEY_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEvent
     *              #EVENT_ACCESSIBILITY_STATE_CHANGED and AccessibilityStateEvent#EVENT_TOUCH_BROWSE_STATE_CHANGED
     *              #EVENT_SCREEN_MAGNIFIER_CHANGED and EVENT_AUTO_CLICK_CHANGED and #EVENT_SHORT_KEY_CHANGED
     * @return true: send ok; otherwise is refused.
     */
    virtual bool UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Get installed abilities.
     * @param installedAbilities The installed accessibility ability infos.
     * @return Return true if get installed abilities successfully, else return false.
     */
    virtual bool GetInstalledAbilities(std::vector<AccessibilityAbilityInfo> &installedAbilities) = 0;

    /**
     * @brief Get enabled abilities.
     * @param enabledAbilities The infos of enabled abilities.
     * @return Return true if get enabled abilities successfully, else return false.
     */
    virtual bool GetEnabledAbilities(std::vector<std::string> &enabledAbilities) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H