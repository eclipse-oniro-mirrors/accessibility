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

#ifndef ACCESSIBILITY_INTERACTION_CLIENT_H
#define ACCESSIBILITY_INTERACTION_CLIENT_H

#include <cstdint>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "accessible_ability_channel_interface.h"
#include "accessible_ability_channel_proxy.h"
#include "accessibility_element_info.h"
#include "accessibility_element_operator_async_mng.h"
#include "accessibility_element_operator_callback_interface.h"
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_window_info.h"
#include "gesture_simulation.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the api called by AA to get the accessibility info of UI.
* It call the API of AAMS and the request will be proccessed by
* UI and then set the result by callback(AA-->this class(with Callback)--> AAMS
* --> UI process and set Callback).
* It supply one instance for each thread to use this class.
*/
class AccessibilityOperator : public AccessibilityElementOperatorCallbackStub
{
public:
    static const int NONE_ID = -1;
    static const int MAX_INSTANCE = 10;
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    AccessibilityOperator();

	/**
     * @brief destruct function
     * @param object The object of IPC
     * @return
     */
    virtual ~AccessibilityOperator();

    /**
     * @brief Get the instance of AccessibilityOperator
     * @param
     * @return The instance of AccessibilityOperator
     */
    static AccessibilityOperator &GetInstance();

    /**
     * @brief Inner function: Get the object connected from AA to AAMS by channel id.
     * @param channelId The id matched the object connected from AA to AAMS
     * @return The object connected from AA to AAMS.
     */
    static sptr<IAccessibleAbilityChannel> GetChannel(int channelId);

    /**
     * @brief Set the object connected from AA to AAMS and channel id matched.
     * @param channelId The id matched the object connected from AA to AAMS
     * @param channel The object connected from AA to AAMS.
     * @return
     */
    static void AddChannel(const int channelId, const sptr<IAccessibleAbilityChannel> &channel);

    /**
     * @brief Remove the object connected from AA to AAMS by channel id.
     * @param channelId The id matched the object connected from AA to AAMS
     * @return
     */
    static void RemoveChannel(int channelId);

    /**
     * @brief Get the root component information
     * @param channelId The id matched the object connected from AA to AAMS
     * @param elementInfo[out] The root component information
     * @return
     */
    bool GetRoot(int channelId, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get all windows of accessibility.
     * @param channelId The id matched the object connected from AA to AAMS
     * @return All windows information of accessibility.
     */
    std::vector<AccessibilityWindowInfo> GetWindows(int channelId);

    /**
     * @brief Find the node information by accessibility ID.
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param elementId: The unique id of the component ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return -
     */
    bool SearchElementInfosByAccessibilityId(int channelId,
            int accessibilityWindowId, int elementId,
            int mode, std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Find the node information filtered by text.
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param elementId: The unique id of the component ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param text The filter text.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return -
     */
    bool SearchElementInfosByText(int channelId, int accessibilityWindowId, int elementId,
        const std::string &text, std::vector<AccessibilityElementInfo>& elementInfos);

    /**
     * @brief Find the focus node information.
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param elementId: The unique id of the component ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param focusType The type of focus.
     *                  FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     * @param elementInfos[out] The components information matched conditions searched.
     * @return -
     */
    bool FindFocusedElementInfo(int channelId, int accessibilityWindowId,
            int elementId, int focusType, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Find the node information by focus move direction.
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param elementId: The unique id of the component ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param direction The direction of focus move direction.
     *                  Refer to FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD).
     * @param elementInfos[out] The components information matched conditions searched.
     * @return -
     */
    bool FocusMoveSearch(int channelId, int accessibilityWindowId,
            int elementId, int direction, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Perform the action on the component.
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param elementId: The unique id of the component ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param action The action triggered on component.
     *               Refer to [AccessibilityElementInfo.ActionType]
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
     * @return true: Perform action successfully; otherwise is not.
     */
    bool ExecuteAction(int channelId, int accessibilityWindowId,
            int elementId, int action,  std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Save the elements information searched in ACE side
     * @param infos The elements info searched by accessibility id.
     * @param sequenceNum The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int sequenceNum) override;

    /**
     * @brief Save the elements information searched in ACE side
     * @param infos The elements info searched by accessibility id.
     * @param sequenceNum The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int sequenceNum) override;

    /**
     * @brief Save the element information searched in ACE side
     * @param infos The element info searched by accessibility id.
     * @param sequenceNum The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int sequenceNum) override;

    /**
     * @brief Save the element information searched in ACE side
     * @param infos The element info searched by accessibility id.
     * @param sequenceNum The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int sequenceNum) override;

    /**
     * @brief Save the result of action executed in ACE.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param sequenceNum The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetExecuteActionResult(const bool succeeded, const int sequenceNum) override;

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param action: The common action
     * @return true: Perform successfully; otherwise is not.
     */
    bool ExecuteCommonAction(const int channelId, const int action);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param handled True: That is handled; otherwise is not.
     * @param sequence  The number of sequence.
     * @return -
     */
    void SetOnKeyPressEventResult(const int channelId, const bool handled, const int sequence);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @return -
     */
    float GetDisplayResizeScale(const int channelId, const int displayId);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @return -
     */
    float GetDisplayResizeCenterX(const int channelId, const int displayId);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @return -
     */
    float GetDisplayResizeCenterY(const int channelId, const int displayId);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @return The range displayed.
     */
    Rect GetDisplayResizeRect(const int channelId, const int displayId);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @param animate true: It is animation; otherwise is not.
     * @return true: Reset successfully; otherwise is not.
     */
    bool ResetDisplayResize(const int channelId, const int displayId, const bool animate);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param displayId The display id
     * @param scale The value of scaled.
     * @param animate true: It is animation; otherwise is not.
     * @param centerX coordinate X.
     * @param centerY coordinate Y.
     * @return true: Resize successfully; otherwise is not.
     */
    bool SetDisplayResizeScaleAndCenter(const int channelId, const int displayId, const float scale,
        const float centerX, const float centerY, const bool animate);

    /**
     * @brief Wrap the function of function expect for ACE related
     * @param channelId The id matched the object connected from AA to AAMS.
     * @param sequenceNum  The number of sequence.
     * @param gestureSteps The value of gesture.
     * @return -
     */
    void SendSimulateGesture(const int channelId, const int sequenceNum,
        const std::vector<GesturePathDefine> &gestureSteps);
private:
    static const uint32_t MAX_REQUEST = 0x7FFFFFFF;
    static std::vector<sptr<AccessibilityOperator>> instances_;
    static std::map<int, sptr<IAccessibleAbilityChannel>> channels_;
    AccessibilityElementAsyncOperatorMng asyncElementOperatorMng_ = {};
    AccessibilityElementInfo accessibilityInfoResult_ = {};
    std::vector<AccessibilityElementInfo> accessibilityInfosResult_ = {};
    std::vector<AccessibilityWindowInfo> windows_ = {};
    bool executeActionResult_ = false;
    static std::recursive_mutex mutex_;
};

} // namespace Accessibility
} // namespace OHOS
#endif