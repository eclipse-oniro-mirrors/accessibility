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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_H

#include <map>
#include "accessibility_element_operator_callback.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface for UI to implement.
* It triggered by AAMS when AA to request the accessibility information.
*/
class AccessibilityElementOperator {
public:
    /**
     * @brief Destruct
     */
    virtual ~AccessibilityElementOperator() = default;

    /**
     * @brief Make the node information by accessibility ID and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *             PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *             PREFETCH_CHILDREN: Need to make the child node info also.
     *             otherwise: Make the node information by elementId only.
     */
    virtual void SearchElementInfoByAccessibilityId(const int32_t elementId,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback, const int32_t mode) = 0;

    /**
     * @brief Make the child node information by accessibility ID and filtered by text and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param text  Filter for the child components to matched with the text
     */
    virtual void SearchElementInfosByText(const int32_t elementId, const std::string &text,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) = 0;

    /**
     * @brief Make the node information of the component focused by the focus type specified.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param focusType FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     */
    virtual void FindFocusedElementInfo(const int32_t elementId, const int32_t focusType, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) = 0;

    /**
     * @brief Make the node info by current focus move direction.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     */
    virtual void FocusMoveSearch(const int32_t elementId, const int32_t direction, const int32_t requestId,
        AccessibilityElementOperatorCallback &callback) = 0;

    /**
     * @brief To return the result of execute action.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param action Refer to [AccessibilityElementInfo.ActionType]
     * @param actionArguments The parameter for action type. such as:
     *      action: ACCESSIBILITY_ACTION_NEXT_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_NEXT_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_SET_SELECTION,
     *                  actionArguments({ACTION_ARGU_SELECT_TEXT_START,"1"(start location)},
     *                                  {ACTION_ARGU_SELECT_TEXT_END,"10"(end location)})
     *      action: ACCESSIBILITY_ACTION_SET_TEXT,
     *                  actionArguments(ACTION_ARGU_SET_TEXT,"the text of setted")
     */
    virtual void ExecuteAction(const int32_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments,
        const int32_t requestId, AccessibilityElementOperatorCallback &callback) = 0;

    /**
    * @brief The function is called while accessibility System check the id of window is not equal
    *        to the id of active window when sendAccessibility.
    */
    virtual void ClearFocus() = 0;

    /**
    * @brief The low layer is notified by the function called while accessibility system execute
    *        the function of executeAction from AS to check the all low windows cared the outside event.
    *        Example: PopupWindow receive the OUTSIDE_EVENT to close itself.
    */
    virtual void OutsideTouch() = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ELEMENT_OPERATOR_H