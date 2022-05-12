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
#include "i_accessible_ability_channel.h"

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
     * @brief The desconstructor of AccessibleAbilityChannelClient.
     */
    ~AccessibleAbilityChannelClient() = default;

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
     * @return Return true if finds focus element info successfully, else return false.
     */
    bool FindFocusedElementInfo(int32_t accessibilityWindowId, int32_t elementId,
        int32_t focusType, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Send simulate gesture to aams.
     * @param sequenceNum  The sequence of gesture.
     * @param gesturePath The path of gesture.
     */
    void SendSimulateGesture(const int32_t sequenceNum,
        const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);

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
     * @return true: execute action successfully; otherwise is not.
     */
    bool ExecuteAction(int32_t accessibilityWindowId,
        int32_t elementId, int32_t action,  std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Find the node information by accessibility ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return Return true if search element info successfully, else return false.
     */
    bool SearchElementInfosByAccessibilityId(int32_t accessibilityWindowId, int32_t elementId,
        int32_t mode, std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Get all windows of accessibility.
     * @param displayId the id of display
     * @return All windows information of accessibility.
     */
    std::vector<AccessibilityWindowInfo> GetWindows(const uint64_t displayId);

    /**
     * @brief Find the node information filtered by text.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param text The filter text.
     * @param elementInfos[out] The components information matched conditions searched.
     * @return Return true if search element info successfully, else return false.
     */
    bool SearchElementInfosByText(int32_t accessibilityWindowId, int32_t elementId,
        const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Find the node information by focus move direction.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param direction The direction of focus move direction.
     *                  Refer to FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD).
     * @param elementInfo[out] The components information matched conditions searched.
     * @return Return true if find element info successfully, else return false.
     */
    bool FocusMoveSearch(int32_t accessibilityWindowId,
        int32_t elementId, int32_t direction, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Execute common action.
     * @param action The common action
     * @return true: Perform successfully; otherwise is not.
     */
    bool ExecuteCommonAction(const int32_t action);

    /**
     * @brief Set event types to filter.
     * @param eventTypes The event types which you want.
     * @return Return true if sets event types successfully, else return false.
     */
    bool SetEventTypeFilter(const uint32_t eventTypes);

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return true if sets target bundle names successfully, else return false.
     */
    bool SetTargetBundleName(const std::vector<std::string> targetBundleNames);
private:
    int32_t GenrateRequestId();

    uint32_t channelId_ = INVALID_CHANNEL_ID;
    sptr<IAccessibleAbilityChannel> proxy_ = nullptr;
    std::atomic<int> requestId_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CHANNEL_CLIENT_H