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

#include "accessibility_ipc_interface_code.h"
#include "iaccessible_ability_channel.h"
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
    virtual RetError SearchElementInfoByAccessibilityId(const ElementBasicInfo elementBasicInfo,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter, bool systemApi = false) override;

    /**
     * @brief Searches elementInfo by window id and set the result by callback through the proxy object.
     * @param elementBasicInfo
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback To transfer the element info to ASAC.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent element info also.
     *             PREFETCH_SIBLINGS: Need to make the sister/brothers element info also.
     *             PREFETCH_CHILDREN: Need to make the child element info also.
     *             otherwise: Make the element information by elementId only.
     * @return Return RET_OK if search elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchDefaultFocusedByWindowId(const ElementBasicInfo elementBasicInfo,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter = false) override;

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
    virtual RetError SearchElementInfosByText(const int32_t accessibilityWindowId, const int64_t elementId,
        const std::string &text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi = false) override;

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
    virtual RetError FindFocusedElementInfo(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi = false) override;

    /**
     * @brief Make the element info by current focus move direction through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param direction Refer to AccessibilityElementInfo.FocusMoveDirection(UP/DOWN/LEFT/RIGHT/FORWARD/BACKWARD)
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the element info to ASAC and it defined by ASAC.
     * @return Return RET_OK if search elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi = false) override;

    /**
     * @brief To perform action through the proxy object.
     * @param accessibilityWindowId The id of accessibility window.
     * @param elementId: The unique id of the component ID.
     * @param action Refer to [AccessibilityElementInfo.ActionType]
     * @param actionArguments The parameter for action type.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @param rect The element's boundary rectangle info.
     * @return Return RET_OK if perform action successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, const Rect &rect) override;

    /**
     * @brief Get the result of Cursor Position through the proxy object.
     * @param accessibilityWindowId The target winid.
     * @param elementId The element Id.
     * @param requestId Matched the request and response. It needn't cared by ACE, transfer it by callback only.
     * @param callback  To transfer the node info to ASAC and it defined by ASAC.
     * @return Return RET_OK if Cursor Position successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override;

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
    virtual RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi = false) override;

    /**
     * @brief Package and transfer isenable to the data table
     * @param isEnable The information of status.
     * @return Return RET_OK if obtains screen curtain status successfully.
     */
    virtual RetError EnableScreenCurtain(bool isEnable) override;

    /**
     * @brief Hold running lock to prevent screen turning off automatically.
     * @return Return RET_OK if hold running lock successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError HoldRunningLock() override;
 
    /**
     * @brief Unhold running lock to prevent screen turning off automatically
     * @return Return RET_OK if Unhold running lock successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError UnholdRunningLock() override;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindowsByDisplayId(const uint64_t displayId,
        std::vector<AccessibilityWindowInfo> &windows, bool systemApi = false) override;

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

    /**
     * @brief Set is register disconnectCallback.
     * @param targetBundleNames The isRegister state
     * @return Return RET_OK if sets isRegister callback successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SetIsRegisterDisconnectCallback(bool isRegister) override;

    /**
     * @brief Notify disconnect.
     * @return Return RET_OK if notifyDisconnect successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError NotifyDisconnect() override;

    /**
     * @brief Search element infos by specific property.
     * @param elementBasicInfo The basic info of the element.
     * @param param The specific property parameters.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @param callback The callback to return the result.
     * @return Returns 0 on success, others on failure.
     */
    virtual void SearchElementInfoBySpecificProperty(const ElementBasicInfo elementBasicInfo,
        const SpecificPropertyParam& param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    /**
     * @brief Configure events.
     * @return Return RET_OK if ConfigureEvents successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError ConfigureEvents(const std::vector<uint32_t> needEvents) override;

    virtual RetError FocusMoveSearchWithCondition(const AccessibilityElementInfo &info,
        const AccessibilityFocusMoveParam& param,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t windowId) override;
    
    /**
     * @brief Update accessibility element info attributes through the proxy object.
     * @param elementId The element id to update.
     * @param windowId The window id of the element.
     * @param accessibilityVirtualNode The element attributes to update.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @param callback The callback to return the result.
     * @return Return RET_OK if update successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError UpdateCustomAccessibilityProperty(const int64_t elementId, const int32_t windowId,
        const AccessibilityVirtualNode& accessibilityVirtualNode, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;
 
    /**
     * @brief Add accessibility virtual nodes.
     * @param rootId The root element id.
     * @param windowId The window id.
     * @param nodes The virtual nodes to add.
     * @param requestId Matched the request and response.
     * @param callback To transfer the result to ASAC.
     * @return RetError: ERR_OK if success, otherwise error code.
     */
    virtual RetError AddAccessibilityVirtualNode(const int64_t rootId, const int32_t windowId,
        const std::vector<AccessibilityVirtualNode> &nodes, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;
 
    /**
     * @brief Remove accessibility virtual node.
     * @param id The virtual node id to remove.
     * @param windowId The window id.
     * @param requestId Matched the request and response.
     * @param callback To transfer the result to ASAC.
     * @return RetError: ERR_OK if success, otherwise error code.
     */
    virtual RetError RemoveAccessibilityVirtualNode(const int64_t id, const int32_t windowId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override;

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
    bool SendTransactCmd(AccessibilityInterfaceCode code, MessageParcel &data,
        MessageParcel &reply,  MessageOption &option);

    /**
     * @brief Write action arguments to message parcel.
     * @param data The message parcel to write to.
     * @param actionArguments The action arguments to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteActionArguments(MessageParcel &data,
        const std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Write accessibility virtual node properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteAccessibilityVirtualNode(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    /**
     * @brief Write virtual node basic properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteVirtualNodeBasicProperties(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    /**
     * @brief Write virtual node rectangle properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteVirtualNodeRectProperties(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    /**
     * @brief Write virtual node boolean properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteVirtualNodeBoolProperties(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    /**
     * @brief Write virtual node other properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteVirtualNodeOtherProperties(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    /**
     * @brief Write virtual node relation properties to message parcel.
     * @param data The message parcel to write to.
     * @param accessibilityVirtualNode The virtual node to write.
     * @return true: Write successfully; otherwise is not.
     */
    bool WriteVirtualNodeRelationProperties(MessageParcel &data,
        const AccessibilityVirtualNode& accessibilityVirtualNode);

    static inline BrokerDelegator<AccessibleAbilityChannelProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CHANNEL_PROXY_H