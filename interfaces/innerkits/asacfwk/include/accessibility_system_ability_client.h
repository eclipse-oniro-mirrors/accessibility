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

constexpr int32_t ELEMENT_MOVE_BIT = 40;
constexpr int32_t CONT_SPLIT_ID = -1;
constexpr uint64_t MAX_ELEMENT_ID = 0xFFFFFFFFFF;

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
     * @return AccessibilitySystemAbilityClient instance
     */
    static std::shared_ptr<AccessibilitySystemAbilityClient> GetInstance();

    /**
     * @brief Deconstruct.
     */
    virtual ~AccessibilitySystemAbilityClient() = default;

    /**
     * @brief Register the element operator, so the AA can get node info from ACE.
     * @param windowId Window ID
     * @param operation The callback object.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterElementOperator(const int32_t windowId,
        const std::shared_ptr<AccessibilityElementOperator> &operation) = 0;

    /**
     * @brief Register the element operator, so the AA can get node info from ACE.
     * @param parameter The Register parameters.
     * @param operation The callback object.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterElementOperator(Registration parameter,
        const std::shared_ptr<AccessibilityElementOperator> &operation) = 0;

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

    /**
     * @brief Checks whether screenreader ability is enabled.
     * @param isEnabled true: enabled; false: disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError IsScreenReaderEnabled(bool &isEnabled)= 0;

    /**
     * @brief Checks whether accessibility ability is enabled.
     * @param isEnabled true: enabled; false: disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError IsEnabled(bool &isEnabled)= 0;

    /**
     * @brief Checks whether touch exploration ability is enabled.
     * @param isEnabled true: enabled; false: disabled
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError IsTouchExplorationEnabled(bool &isEnabled) = 0;

    /**
     * @brief Get touch exploration mode.
     * @param touchMode 'singleTouchMode': single click mode; 'doubleTouchMode': double click mode;
     *                  'none": touch exploration is not enabled.
     */
    virtual void GetTouchMode(std::string &touchMode) = 0;

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
    virtual RetError GetAbilityList(const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType,
        std::vector<AccessibilityAbilityInfo> &infos) = 0;

    /**
     * @brief Sends an accessibility event.
     * @param eventType  Identifies the accessibility event specified by AccessibilityEventInfo.
     * @param componentId Indicates the ID of the component to be associated with the event.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError SendEvent(const EventType eventType, const int64_t componentId) = 0;

    /**
     * @brief Sends information about an accessibility event.
     * @param event Indicates the accessibility event information specified by AccessibilityEventInfo.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError SendEvent(const AccessibilityEventInfo &event) = 0;

    /**
     * @brief Subscribes to the specified type of accessibility status change events.
     * @param observer Indicates the observer for listening to status events, which is specified
     *              by AccessibilityStateObserver.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEventType.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError SubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Unsubscribe the specified type of accessibility status change events.
     * @param observer Indicates the registered accessibility status event observer.
     * @param eventType Indicates the status type, which is specified by AccessibilityStateEventType.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError UnsubscribeStateObserver(const std::shared_ptr<AccessibilityStateObserver> &observer,
        const uint32_t eventType) = 0;

    /**
     * @brief Get enabled abilities.
     * @param enabledAbilities The infos of enabled abilities.
     * @return Returns RET_OK if successful, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) = 0;

    virtual uint32_t GetAccessibilityState() = 0;
    virtual void SetFindAccessibilityNodeInfoResult(const AccessibilityElementInfo elementInfo,
        const int32_t requestId, const int32_t requestCode) = 0;
    virtual void SetFindAccessibilityNodeInfosResult(const std::list<AccessibilityElementInfo> elementInfos,
        const int32_t requestId, const int32_t requestCode) = 0;
    virtual void SetPerformActionResult(const bool succeeded, const int32_t requestId) = 0;
    virtual RetError GetFocusedWindowId(int32_t &focusedWindowId) = 0;

    /**
    * @brief Splic ElementId and TreeId.
    * @param treeId: The tree Id.
    * @param elementId: The incoming before splicing elementId,Send out after splicing elementId.
    */
    static void SetSplicElementIdTreeId(const int32_t treeId, int64_t &elementId)
    {
        if (treeId == CONT_SPLIT_ID || elementId == CONT_SPLIT_ID) {
            elementId = CONT_SPLIT_ID;
            return;
        }
        if ((static_cast<uint64_t>(elementId) & MAX_ELEMENT_ID) != static_cast<uint64_t>(elementId)) {
            return;
        }
        uint64_t itemp = 0;
        itemp = treeId;
        itemp = (itemp << ELEMENT_MOVE_BIT);
        itemp |= static_cast<uint64_t>(elementId);
        elementId = static_cast<int64_t>(itemp);
    }

    /**
    * @brief Split ElementId.
    * @param elementId:Splic ElementId
    * @param splitElementId: The split ElementId.
    * @param splitTreeId: The split TreeId.
    */
    static void GetTreeIdAndElementIdBySplitElementId(const int64_t elementId, int64_t &splitElementId,
        int32_t &splitTreeId)
    {
        if (elementId <= CONT_SPLIT_ID) {
            splitTreeId = CONT_SPLIT_ID;
            splitElementId = CONT_SPLIT_ID;
            return;
        }
        splitTreeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
        splitElementId = (static_cast<uint64_t>(elementId) & MAX_ELEMENT_ID);
    }
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SYSTEM_ABILITY_CLIENT_H