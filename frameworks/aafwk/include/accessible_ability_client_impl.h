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

#ifndef ACCESSIBLE_ABILITY_CLIENT_IMPL_H
#define ACCESSIBLE_ABILITY_CLIENT_IMPL_H

#include <memory>
#include "accessible_ability_channel_client.h"
#include "accessible_ability_client.h"
#include "accessible_ability_client_stub.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_display_resize_controller_impl.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityClientImpl : public AccessibleAbilityClient, public AccessibleAbilityClientStub {
public:
    /**
     * @brief The constructor of AccessibleAbilityClientImpl.
     */
    AccessibleAbilityClientImpl();

    /**
     * @brief The desconstructor of AccessibleAbilityClientImpl.
     */
    ~AccessibleAbilityClientImpl() = default;

    /**
     * @brief Gets remote object.
     * @param
     * @return Remote object.
     */
    virtual sptr<IRemoteObject> GetRemoteObject() override;

    /**
     * @brief Register ability listener.
     * @param listener The listener to add.
     * @return Return true if registers listener successfully, else return false.
     */
    virtual bool RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener) override;

    /**
     * @brief Init accessible ability.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId) override;

    /**
     * @brief Disconnect accessible ability.
     * @param channelId The id of channel.
     * @return
     */
    virtual void Disconnect(const int32_t channelId) override;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     * @return
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     * @return
     */
    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence) override;

    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed.
     * @param displayId The id of display.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    virtual void OnDisplayResized(const int32_t displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY) override;

    /**
     * @brief Called when need to notify the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param completedSuccessfully The result of gesture completion.
     * @return
     */
    virtual void OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully) override;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains elementInfo of focus.
     * @param sourceInfo The source info to get focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo, const int32_t focusType,
        AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Sends simulate gestures to the screen.
     * @param sequence The sequence of gesture.
     * @param gesturePathList The gesture which need to send.
     * @param listener The listener of the gesture.
     * @return Return true if the gesture sends successfully, else return false.
     */
    virtual bool GestureInject(const uint32_t sequence, const std::vector<AccessibilityGesturePath>& gesturePathList,
        const std::shared_ptr<AccessibilityGestureResultListener>& listener) override;

    /**
     * @brief Obtains the default displayResize controller.
     * @param
     * @return Return the default displayResize controller.
     */
    virtual std::shared_ptr<DisplayResizeController> GetDisplayResizeController() override;

    /**
     * @brief Obtains the specified displayResize controller by displayId.
     * @param displayId The id of display.
     * @return Return the specified displayResize controller.
     */
    virtual std::shared_ptr<DisplayResizeController> GetDisplayResizeController(const int32_t displayId) override;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetRoot(AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param windowInfo The source window info to get root.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
        AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param
     * @return The information of windows.
     */
    virtual std::vector<AccessibilityWindowInfo> GetWindows() override;

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::GlobalAction.
     * @return Return true if executes action successfully, else return false.
     */
    virtual bool ExecuteCommonAction(const GlobalAction action) override;

    /**
     * @brief Gets the next focused node in the specified direction of the currently focused node.
     * @param elementInfo The source info to get next info.
     * @param direction Indicates the direction to obtain the next focused node. Refer to FocusMoveDirection
     * @param nextElementInfo The info of next element.
     * @return Return true if gets next elementInfo successfully, else return false.
     */
    virtual bool GetNext(const AccessibilityElementInfo &elementInfo, const FocusMoveDirection direction,
        AccessibilityElementInfo &nextElementInfo) override;

    /**
     * @brief Obtains information about the accessibility child window at a specified index.
     * @param index The index of the child.
     * @param parent The parent info to get child.
     * @param child The window info of child.
     * @return Return true if gets child windowInfo successfully, else return false.
     */
    virtual bool GetChildWindowInfo(const int32_t index, const AccessibilityWindowInfo &parent,
        AccessibilityWindowInfo &child) override;

    /**
     * @brief Get the child node information by childId
     * @param index The index of the child.
     * @param parent The parent info to get child.
     * @param child The element info of child.
     * @return Return true if gets child elementInfo successfully, else return false.
     */
    virtual bool GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
        AccessibilityElementInfo &child) override;

    /**
     * @brief Searches for node information based on the specified content.
     * @param elementInfo The source info.
     * @param text specified content
     * @param elementInfos The element infos of specified content.
     * @return Return true if gets elementInfos successfully, else return false.
     */
    virtual bool GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
        std::vector<AccessibilityElementInfo> &elementInfos) override;

    /**
     * @brief Obtains an anchor accessibility node that anchors this window to another window.
     * @param windowInfo The source window info to get anchor.
     * @param elementInfo The anchor element info.
     * @return Return true if gets elementInfos successfully, else return false.
     */
    virtual bool GetAnchor(const AccessibilityWindowInfo &windowInfo, AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Get the node information related with the event
     * @param eventInfo The source info to get source.
     * @param elementInfo The element info of source.
     * @return Return true if gets elementInfos successfully, else return false.
     */
    virtual bool GetSource(const AccessibilityEventInfo &eventInfo, AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Get the parent window
     * @param child The child window info to get parent.
     * @param parent The parent window info.
     * @return Return true if gets info successfully, else return false.
     */
    virtual bool GetParentWindowInfo(const AccessibilityWindowInfo &child, AccessibilityWindowInfo &parent) override;

    /**
     * @brief Get Parent node information
     * @param child The child element info to get parent.
     * @param parent The parent element info.
     * @return Return true if gets info successfully, else return false.
     */
    virtual bool GetParentElementInfo(const AccessibilityElementInfo &child, AccessibilityElementInfo &parent) override;

    /**
     * @brief Get the node information labeled component
     * @param elementInfo The source info to get labeled.
     * @param labeledElementInfo The labeled element info.
     * @return Return true if gets infos successfully, else return false.
     */
    virtual bool GetLabeled(const AccessibilityElementInfo &elementInfo,
        AccessibilityElementInfo &labeledElementInfo) override;

    /**
     * @brief Executes a specified action.
     * @param elementInfo The source info to execute action.
     * @param action: the action type
     * @param actionArguments: The parameter for action type. such as:
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
     * @return true Perform action succeed, otherwise is not.
     */
    virtual bool ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
        const std::map<std::string, std::string> &actionArguments) override;

    /**
     * @brief Clean data.
     * @param remote The object access to AAMS.
     * @return
     */
    void ResetAAClient(const wptr<IRemoteObject> &remote);
private:
    /**
     * @brief Dispatch the result of simulate gesture.
     * @param sequence The sequence of gesture.
     * @param result The result of gesture completion.
     * @return
     */
    void DispatchGestureInjectResult(uint32_t sequence, bool result);

    class AccessibleAbilityDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityDeathRecipient(AccessibleAbilityClientImpl &client) : client_(client) {}
        ~AccessibleAbilityDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote);
    private:
        AccessibleAbilityClientImpl &client_;
    };

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<AccessibleAbilityManagerServiceProxy> serviceProxy_ = nullptr;
    std::shared_ptr<AccessibleAbilityListener> listener_ = nullptr;
    std::shared_ptr<AccessibleAbilityChannelClient> channelClient_ = nullptr;

    std::map<uint32_t, std::shared_ptr<DisplayResizeControllerImpl>> displayResizeControllers_;
    std::map<uint32_t, std::shared_ptr<AccessibilityGestureResultListener>> gestureResultListenerInfos_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CLIENT_IMPL_H