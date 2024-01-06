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

#ifndef MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_IMPL_H
#define MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_IMPL_H

#include <memory>
#include <unordered_map>
#include "accessibility_element_operator.h"
#include "accessibility_element_operator_callback.h"
#include "accessibility_element_operator_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface for UI to implement.
 * It is triggered by ABMS when AA to request the accessibility information.
 */
class MockAccessibilityElementOperatorImpl : public AccessibilityElementOperatorStub {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    explicit MockAccessibilityElementOperatorImpl(int32_t windowId,
        const std::shared_ptr<AccessibilityElementOperator> &operation,
        AccessibilityElementOperatorCallback &callback);

    /**
     * @brief destruct function
     */
    ~MockAccessibilityElementOperatorImpl();

    /**
     * @brief Make the node information by accessibility ID and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     */
    void SearchElementInfoByAccessibilityId(const int32_t elementId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode) override;

    /**
     * @brief Make the child node information by accessibility ID and filtered by text and set the result by callback.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param text  Filter for the child components to matched with the text
     */
    void SearchElementInfosByText(const int32_t elementId, const std::string& text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback) override;

    /**
     * @brief Make the node information of the component focused by the focus type specified.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param focusType FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     */
    void FindFocusedElementInfo(const int32_t elementId, const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback) override;

    /**
     * @brief Make the node info by current focus move direction.
     * @param elementId: The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     */
    void FocusMoveSearch(const int32_t elementId, const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback) override;

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
     */
    void ExecuteAction(const int32_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback) override;

    /**
     * @brief The function is called while accessibility System check the id of window is not equal
     * to the id of active window when sendAccessibility.
     */

    void ClearFocus() override;

    /**
     * @brief the low layer is notified by the function called while accessibility system execute
     * the function of executeAction from AS to check the all low windows cared the outside event.
     * Example: PopupWindow receive the OUTSIDE_EVENT to close itself.
     */
    void OutsideTouch() override;

    /**
     * @brief Get the window id related with operator object
     * @param windowId: The unique id of the window related with operator object.
     */
    int32_t GetWindowId();

    /**
     * @brief Set the element information by accessibility id to AA.
     * @param infos The element info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    void SetSearchElementInfoByAccessibilityIdResult(const std::list<AccessibilityElementInfo> &infos,
        const int32_t requestId);

    /**
     * @brief Set the element information matched with text to AA.
     * @param infos The element information searched matched with text.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    void SetSearchElementInfoByTextResult(const std::list<AccessibilityElementInfo> &infos,
        const int32_t requestId);

    /**
     * @brief Set the element information matched with focus type to AA.
     * @param info The element information searched matched with focus type.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int32_t requestId);

    /**
     * @brief Set the element information by focus direction to AA.
     * @param info The element information searched by focus direction.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId);

    /**
     * @brief Set the result of action executed to AA.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    void SetExecuteActionResult(const bool succeeded, const int32_t requestId);

private:
    int32_t AddRequest(int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback);

    std::mutex mutex_;
    int32_t windowId_ = 0;
    AccessibilityElementOperatorCallback &operatorCallback_;
    std::shared_ptr<AccessibilityElementOperator> operator_ = nullptr;
    std::unordered_map<int32_t, sptr<IAccessibilityElementOperatorCallback>> requests_;
};

template <class T>
std::vector<T> TranslateListToVector(const std::list<T>& originList)
{
    std::size_t len = originList.size();
    std::vector<T> destVector(len);
    std::copy(originList.begin(), originList.end(), destVector.begin());
    return destVector;
}

template <class T>
std::list<T> TranslateVectorToList(const std::vector<T>& originVector)
{
    size_t len = originVector.length();
    std::list<T> destList(len);
    std::copy(originVector.begin(), originVector.end(), destList.begin());
    return destList;
}
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ELEMENT_OPERATOR_IMPL_H