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

#ifndef INTERFACE_ACCESSIBLE_ABILITY_CHANNEL_H
#define INTERFACE_ACCESSIBLE_ABILITY_CHANNEL_H

#include <string>
#include <vector>
#include <map>

#include "accessibility_gesture_inject_path.h"
#include "accessibility_window_info.h"
#include "i_accessibility_element_operator_callback.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityChannel : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityChannel");

    /**
     * @brief Searches elementInfo by accessibility id and set the result by callback.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback To transfer the element info to ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent element info also.
     *             PREFETCH_SIBLINGS: Need to make the sister/brothers element info also.
     *             PREFETCH_CHILDREN: Need to make the child element info also.
     *             otherwise: Make the element information by elementId only.
     * @return Return true if search elementInfo successfully, else return false.
     */
    virtual bool SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
        const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode) = 0;

    /**
     * @brief Make the child element information by accessibility ID and filtered by text and
     *        set the result by callback.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param text  Filter for the child components to matched with the text
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return true if search elementInfo successfully, else return false.
     */
    virtual bool SearchElementInfosByText(const int32_t accessibilityWindowId, const int32_t elementId,
        const std::string &text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) = 0;

    /**
     * @brief Make the element information of the component focused by the focus type specified.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param focusType FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return true if find elementInfo successfully, else return false.
     */
    virtual bool FindFocusedElementInfo(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) = 0;

    /**
     * @brief Make the element info by current focus move direction.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return true if search elementInfo successfully, else return false.
     */
    virtual bool FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) = 0;

    /**
     * @brief To perform action.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param action Refer to [AccessibilityElementInfo.ActionType]
     * @param actionArguments The parameter for action type.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @return Return true if perform action successfully, else return false.
     */
    virtual bool ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t action,
        std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) = 0;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @return The information of windows.
     */
    virtual std::vector<AccessibilityWindowInfo> GetWindows(const uint64_t displayId) = 0;

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::GlobalAction.
     * @return Return true if performs action successfully, else return false.
     */
    virtual bool ExecuteCommonAction(const int32_t action) = 0;

    /**
     * @brief Set the result of key press event.
     * @param handled The result of key press event, true if the event has been consumed, otherwise false.
     * @param sequence The sequence of key press event result.
     * @return
     */
    virtual void SetOnKeyPressEventResult(const bool handled, const int32_t sequence) = 0;

    /**
     * @brief Send simulation gesture.
     * @param requestId The sequence of simulation gesture.
     * @param gesturePath The gesture path to send.
     * @return
     */
    virtual void SendSimulateGesture(const int32_t requestId,
        const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath) = 0;

    /**
     * @brief Set event types to filter.
     * @param eventTypes The event types which you want.
     * @return Return true if sets event types successfully, else return false.
     */
    virtual bool SetEventTypeFilter(const uint32_t eventTypes) = 0;

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return true if sets target bundle names successfully, else return false.
     */
    virtual bool SetTargetBundleName(const std::vector<std::string> targetBundleNames) = 0;

    enum class Message {
        SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID = 0,
        SEARCH_ELEMENTINFOS_BY_TEXT,
        FIND_FOCUSED_ELEMENTINFO,
        FOCUS_MOVE_SEARCH,
        PERFORM_ACTION,
        GET_WINDOWS,
        EXECUTE_COMMON_ACTION,
        DISABLE_ABILITY,
        SET_ON_KEY_PRESS_EVENT_RESULT,
        SEND_SIMULATE_GESTURE_PATH,
        SET_EVENT_TYPE_FILTER,
        SET_TARGET_BUNDLE_NAME,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBLE_ABILITY_CHANNEL_H