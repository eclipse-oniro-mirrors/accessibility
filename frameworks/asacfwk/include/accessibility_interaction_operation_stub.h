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

#ifndef ACCESSIBILITY_INTERACTION_OPERATION_STUB_H
#define ACCESSIBILITY_INTERACTION_OPERATION_STUB_H

#include <cstdint>
#include <list>
#include <vector>
#include <string>

#include "accessibility_interaction_operation_interface.h"
#include "accessibility_interaction_operation_callback.h"
#include "accessibility_errorcode.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface for UI to implement.
* It is triggered by ABMS when AA to request the accessibility information.
*/
class AccessibilityInteractionOperationStub : public IRemoteStub<IAccessibilityInteractionOperation>{
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    AccessibilityInteractionOperationStub();

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibilityInteractionOperationStub();

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @param option The option paramter of IPC,such as: async,sync
     * @return
     */
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

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
     */
    virtual void SearchElementInfoByAccessibilityId(const long elementId, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode) override;

    /**
     * @brief Make the child node information by accessibility ID and filtered by text and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param text  Filter for the child components to matched with the text
     * @return
     */
    virtual void SearchElementInfosByText(const long elementId, const std::string &text,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

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
        const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    /**
     * @brief Make the node info by current focus move direction.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     * @return -
     */
    virtual void FocusMoveSearch(const long elementId, const int direction, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

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
        int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    /**
     * @brief The function is called while accessibility System check the id of window is not equal
     * to the id of active window when sendAccessibility.
     * @param -
     * @return
     */

    virtual void ClearFocus() override;

    /**
     * @brief the low layser is notified by the function called while accessibility system execute
     * the function of performAction from AS to check the all low windows cared the outside event.
     * Example: PopupWindow receive the OUTSIDE_EVENT to close itself.
     * @param -
     * @return
     */
    virtual void OutsideTouch() override;

    /**
     * @brief Save the window id related with operator object
     * @param windowId: The unique id of the window related with operator object.
     * @return -
     */
    void SetWindowId(int windowId);

    /**
     * @brief Get the window id related with operator object
     * @param windowId: The unique id of the window related with operator object.
     * @return -
     */
    int GetWindowId();

    /**
     * Implement the function to set the result to AA when ACE complete the request.
     */
    class CallbackImpl : public AccessibilityInteractionOperationCallback {
    public:
        enum CALL_API_NUM : uint32_t {
            CALLBACK_BY_ACCESSIBILITY_ID = 0x00000001,
            CALLBACK_BY_TEXT = 0x00000002,
            CALLBACK_FIND_FOCUS = 0x00000004,
            CALLBACK_BY_FOCUS_MOVE = 0x00000008,
            CALLBACK_PERFORM_ACTION = 0x00000010,
        };

        /**
         * @brief construct function
         * @param object The object of IPC
         * @return
         */
        CallbackImpl();

        /**
         * @brief construct function
         * @param requestId The request id from AA, it is used to match with request and response.
         * @param callNum The id is matched handle function of IPC.
         * @return
         */
        CallbackImpl(const int requestId, CALL_API_NUM callNum);

        /**
         * @brief Set the element information by accessibility id to AA.
         * @param infos The element info searched by accessibility id.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        virtual void SetSearchElementInfoByAccessibilityIdResult(const std::list<AccessibilityElementInfo> &infos,
            const int requestId) override;

        /**
         * @brief Set the element informations matched with text to AA.
         * @param infos The element informations searched matched with text.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        virtual void SetSearchElementInfoByTextResult(const std::list<AccessibilityElementInfo> &infos,
            const int requestId) override;

        /**
         * @brief Set the element informations matched with focus type to AA.
         * @param info The element informations searched matched with focus type.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
            const int requestId) override;

        /**
         * @brief Set the element informations by focus direction to AA.
         * @param info The element informations searched by focus direction.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId) override;

        /**
         * @brief Set the result of action executed to AA.
         * @param succeeded True: The action is executed successfully; otherwise is false.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        virtual void SetPerformActionResult(const bool succeeded, const int requestId) override;

        /**
         * @brief Get the callback object of AA.
         * @param
         * @return The callback object of AA and the request id matched.
         */
        std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>> GetAACallbackList();

        /**
         * @brief Remove the callback object of AA after send the result to AA.
         * @param requestId The request id from AA, it is used to match with request and response.
         * @return
         */
        void RemoveAACallbackList(int requestId);
    private:
        int requestId_ = 0;
        CALL_API_NUM callNum_ = CALLBACK_BY_ACCESSIBILITY_ID;
    };
private:
    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByAccessibilityIdResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSearchElementInfoByAccessibilityId(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByTextResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSearchElementInfosByText(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFindFocusedElementInfoResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFocusMoveSearchResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleFocusFind(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetPerformActionResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandlePerformAction(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:ClearFocus.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleClearFocus(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:OutsideTouch.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleOutsideTouch(MessageParcel &data, MessageParcel &reply);

    using AccessibilityInteractionOperationFunc = ErrCode (AccessibilityInteractionOperationStub::*)
    (MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AccessibilityInteractionOperationFunc> memberFuncMap_ {};
    static std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>>  aaCallbacks_;
    int windowId_ = 0;
    DISALLOW_COPY_AND_MOVE(AccessibilityInteractionOperationStub);
};

}   // namespace Accessibility
}   // namespace OHOS
#endif