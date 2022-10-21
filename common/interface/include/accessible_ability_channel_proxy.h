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

#ifndef ACCESSIBLE_ABILITY_CHANNEL_PROXY_H
#define ACCESSIBLE_ABILITY_CHANNEL_PROXY_H

#include "i_accessible_ability_channel.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityChannelProxy : public IRemoteProxy<IAccessibleAbilityChannel> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    explicit AccessibleAbilityChannelProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     */
    virtual ~AccessibleAbilityChannelProxy() = default;

    /**
     * @brief Searches elementInfo by accessibility id and set the result by callback through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId The unique id of the component ID.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback To transfer the element info to ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent element info also.
     *             PREFETCH_SIBLINGS: Need to make the sister/brothers element info also.
     *             PREFETCH_CHILDREN: Need to make the child element info also.
     *             otherwise: Make the element information by elementId only.
     * @return Return RET_OK if search elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
        const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode) override;

    /**
     * @brief Make the child element information by accessibility ID and filtered by text and
     *        set the result by callback through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param text  Filter for the child components to matched with the text
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return RET_OK if search elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchElementInfosByText(const int32_t accessibilityWindowId, const int32_t elementId,
        const std::string &text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    /**
     * @brief Make the element information of the component focused
     *        by the focus type specified through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param focusType FOCUS_TYPE_ACCESSIBILITY: accessibility focus
     *                  FOCUS_TYPE_INPUT: text input focus
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return RET_OK if find elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError FindFocusedElementInfo(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    /**
     * @brief Make the element info by current focus move direction through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return RET_OK if search elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    /**
     * @brief To perform action through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param action Refer to [AccessibilityElementInfo.ActionType]
     * @param actionArguments The parameter for action type.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @return Return RET_OK if perform action successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    /**
     * @brief Get the window information related with the event
     * @param windowId The window id.
     * @param windowInfo The window information.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo) override;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows) override;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindowsByDisplayId(const uint64_t displayId,
        std::vector<AccessibilityWindowInfo> &windows) override;

    /**
     * @brief Set the result of key press event through the proxy object.
     * @param handled The result of key press event, true if the event has been consumed, otherwise false.
     * @param sequence The sequence of key press event result.
     */
    virtual void SetOnKeyPressEventResult(const bool handled, const int32_t sequence) override;

    /**
     * @brief Send simulation gesture through the proxy object.
     * @param gesturePath The gesture path to send.
     * @return Return RET_OK if gesture injection is successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SendSimulateGesture(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath) override;

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return RET_OK if sets target bundle names successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) override;

private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);
    /**
     * @brief Send the command data from proxy to stub in IPC mechanism.
     * @param code The code matched the function called.
     * @param data Serializable data
     * @param reply The response of IPC
     * @param option The option parameter of IPC,such as: async,sync
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool SendTransactCmd(IAccessibleAbilityChannel::Message code, MessageParcel &data,
        MessageParcel &reply,  MessageOption &option);
    static inline BrokerDelegator<AccessibleAbilityChannelProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CHANNEL_PROXY_H