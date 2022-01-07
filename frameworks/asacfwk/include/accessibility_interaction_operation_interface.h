/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_INTERACTION_OPERATION_INTERFACE_H
#define ACCESSIBILITY_INTERACTION_OPERATION_INTERFACE_H

#include <cstdint>
#include <list>
#include <string>
#include <map>

#include "accessibility_interaction_operation_callback_interface.h"

#include "iremote_broker.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface for UI to implement.
* It triggered by ABMS when AA to request the accessibility information.
*/
class IAccessibilityInteractionOperation : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibilityInteractionOperation");

    /**
     * @brief Make the node information by accessibility ID and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only. 
     * @return -
     * @sysCap Accessibility
     */
    virtual void SearchElementInfoByAccessibilityId(const long elementId,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode) = 0;

    /**
     * @brief Make the child node information by accessibility ID and filtered by text and set the result by callback.
     * @param elementId: The unique id of the component ID. 
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param text  Filter for the child components to matched with the text 
     * @return 
     */
    virtual void SearchElementInfosByText(const long elementId, const std::string &text,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) = 0;

    /**
     * @brief Make the node information of the component focused by the focus type specified.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param focusType FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     * @return 
     */
    virtual void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback) = 0;

    /**
     * @brief Make the node info by current focus move direction.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     * @return -
     */
    virtual void FocusMoveSearch(const long elementId, const int direction, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback) = 0;

    /**
     * @brief To return the result of perform action.
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
     * @return 
     */
    virtual void PerformAction(const long elementId, const int action,
        const std::map<std::string, std::string> actionArguments,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) = 0;

    /**
    * @brief The function is called while accessibility System check the id of window is not equal
    * to the id of active window when sendAccessibility.
    * @param -
    * @return 
    */
    virtual void ClearFocus() = 0;

    /**
    * @brief the low layser is notified by the function called while accessibility system execute 
    * the function of performAction from AS to check the all low windows cared the outside event.
    * Example: PopupWindow receive the OUTSIDE_EVENT to close itself. 
    * @param -
    * @return 
    */
    virtual void OutsideTouch() = 0;

    enum class Message {
        SEARCH_BY_ACCESSIBILITY_ID = 0,
        SEARCH_BY_TEXT,
        FIND_FOCUSED_INFO,
        FOCUS_FIND,
        PERFORM_ACTION,
        CLEAR_FOCUS,
        OUTSIDE_TOUCH,
    };
};

}   // namespace Accessibility
}   // namespace OHOS
#endif