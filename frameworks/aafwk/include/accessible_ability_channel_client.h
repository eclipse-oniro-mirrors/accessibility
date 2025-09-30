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

#ifndef ACCESSIBLE_ABILITY_CHANNEL_CLIENT_H
#define ACCESSIBLE_ABILITY_CHANNEL_CLIENT_H

#include <atomic>
#include "iaccessible_ability_channel.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityChannelClient {
public:
    /**
     * @brief The constructor of AccessibleAbilityChannelClient.
     * @param channelId The id of channel.
     * @param channel The client of channel.
     */
    explicit AccessibleAbilityChannelClient(const int32_t channelId,
        const sptr<IAccessibleAbilityChannel> &channel) : channelId_(channelId), proxy_(channel) {}

    /**
     * @brief The deconstructor of AccessibleAbilityChannelClient.
     */
    virtual ~AccessibleAbilityChannelClient() = default;

    /**
     * @brief Get remote object of IAccessibleAbilityChannel
     * @return Return remote object.
     */
    sptr<IRemoteObject> GetRemote();

    /**
     * @brief Set the result of key press event.
     * @param handled The result of OnKeyPressEvent.
     * @param sequence The sequence of key press event.
     */
    void SetOnKeyPressEventResult(const bool handled, const int32_t sequence);

    /**
     * @brief Find the focus element information.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param focusType The type of focus.
     *                  FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     * @param elementInfo[out] The components information matched conditions searched.
     * @return Return RET_OK if finds focus element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError FindFocusedElementInfo(int32_t accessibilityWindowId, int64_t elementId,
        int32_t focusType, AccessibilityElementInfo &elementInfo, bool systemApi = false);

    /**
     * @brief Send simulate gesture to aams.
     * @param gesturePath The path of gesture.
     * @return Return RET_OK if gesture injection is successfully, otherwise refer to the RetError for the failure.
     */
    RetError SendSimulateGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath);

    /**
     * @brief execute the action on the component.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
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
     * @return RET_OK: execute action successfully; otherwise refer to the RetError for the failure.
     */
    RetError ExecuteAction(int32_t accessibilityWindowId,
        int64_t elementId, int32_t action, const std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Package status to send server data.
     * @param isEnable status info.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError EnableScreenCurtain(bool isEnable);

    /**
     * @brief Hold running lock to prevent screen turning off automatically.
     * @param null.
     * @return Return RET_OK if hold running lock successfully, otherwise refer to the RetError for the failure.
     */
    RetError HoldRunningLock();
 
    /**
     * @brief Unhold running lock to prevent screen turning off automatically
     * @param null.
     * @return Return RET_OK if Unhold running lock successfully, otherwise refer to the RetError for the failure.
     */
    RetError UnholdRunningLock();

    /**
     * @brief To return the result of cursor position.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param position: The position of the cursor to get.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError GetCursorPosition(int32_t accessibilityWindowId, int64_t elementId, int32_t &position);

    /**
     * @brief Find the node information by accessibility ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError SearchElementInfosByAccessibilityId(int32_t accessibilityWindowId, int64_t elementId, int32_t mode,
        std::vector<AccessibilityElementInfo>& elementInfos, int32_t treeId, bool isFilter = false,
        bool systemApi = false);

    /**
     * @brief Find the node information by window ID.
     * @param windowId: The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError SearchDefaultFocusedByWindowId(const int32_t windowId, int64_t elementId,
        int32_t mode, std::vector<AccessibilityElementInfo> &elementInfos, int32_t treeId, bool isFilter = false);

    /**
     * @brief Get the window information related with the event
     * @param windowId The window id.
     * @param windowInfo The window information.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi = false);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    RetError GetWindows(
        const uint64_t displayId, std::vector<AccessibilityWindowInfo>& windows, bool systemApi = false) const;

    /**
     * @brief Find the node information filtered by text.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param text The filter text.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError SearchElementInfosByText(int32_t accessibilityWindowId, int64_t elementId,
        const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos, bool systemApi = false);

    /**
     * @brief Find the node information by focus move direction.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param direction The direction of focus move direction.
     *                  Refer to FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD).
     * @param elementInfo[out] The components information matched conditions searched.
     * @return Return RET_OK if find element info successfully, otherwise refer to the RetError for the failure.
     */
    RetError FocusMoveSearch(int32_t accessibilityWindowId,
        int64_t elementId, int32_t direction, AccessibilityElementInfo &elementInfo, bool systemApi = false);

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return RET_OK if sets target bundle names successfully, otherwise refer to the RetError for the failure.
     */
    RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames);

    /**
     * @brief Set is register disconnectCallback.
     * @param isRegister The isRegister state
     * @return Return RET_OK if sets isRegister callback successfully, otherwise refer to the RetError for the failure.
     */
    RetError SetIsRegisterDisconnectCallback(bool isRegister);

    /**
     * @brief Notify disconnect.
     * @return Return RET_OK if notifyDisconnect successfully, otherwise refer to the RetError for the failure.
     */
    RetError NotifyDisconnect();

    /**
     * @brief Config need events.
     * @param needEvents The need events.
     * @return Return RET_OK if config need events successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError ConfigureEvents(const std::vector<uint32_t> needEvents);

    /*
     * @brief Search element infos by specific property.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId  The unique id of the component ID.
     * @param param The specific property parameters.
     * @param infos The element infos searched by specific property.
     * @param elementInfos The element infos searched by specific property.
     * @param treeId The tree id of the component.
     * @return Returns 0 on success, others on failure.
     */
    RetError SearchElementInfosBySpecificProperty(int32_t accessibilityWindowId, int64_t elementId,
        const SpecificPropertyParam& param, std::vector<AccessibilityElementInfo> &infos,
        std::vector<AccessibilityElementInfo> &elementInfos,
        int32_t treeId);
    
    RetError FocusMoveSearchWithCondition(int64_t elementId, AccessibilityFocusMoveParam param,
        std::vector<AccessibilityElementInfo> &infos, int32_t windowId);

private:
    int32_t GenerateRequestId();

    /**
     * @brief Validate and process element infos with main window ID setting
     * @param sourceInfos The source element infos to validate and process
     * @param targetInfos The target vector to store processed infos
     * @param clearInfos The vector to clear
     * @param accessibilityWindowId The window ID to set for all elements
     * @param logType The type name for logging
     * @return Returns RET_OK on success, RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE on validation failure
     */
    RetError ValidateAndProcessElementInfos(const std::vector<AccessibilityElementInfo>& sourceInfos,
        std::vector<AccessibilityElementInfo>& targetInfos,
        std::vector<AccessibilityElementInfo>& clearInfos,
        int32_t accessibilityWindowId,
        const std::string& logType);

    int32_t channelId_ = INVALID_CHANNEL_ID;
    int32_t accessibilityFocusedWindowId_ = INVALID_WINDOW_ID;
    int64_t accessibilityFocusedElementId_ = INVALID_WINDOW_ID;
    sptr<IAccessibleAbilityChannel> proxy_ = nullptr;
    std::atomic<int> requestId_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CHANNEL_CLIENT_H