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

#ifndef ACCESSIBLE_ABILITY_CLIENT_H
#define ACCESSIBLE_ABILITY_CLIENT_H

#include <map>
#include <memory>
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_gesture_inject_path.h"
#include "accessibility_window_info.h"
#include "accessible_ability_listener.h"
#include "iremote_object.h"
#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Accessibility {
struct DisconnectCallback {
    DisconnectCallback(napi_env env, napi_ref ref) : handlerRef_(ref), env_(env) {};
    ~DisconnectCallback() {};
    static napi_handle_scope TmpOpenScope(napi_env env)
    {
        napi_handle_scope scope = nullptr;
        NAPI_CALL(env, napi_open_handle_scope(env, &scope));
        return scope;
    }

    void NotifyJS()
    {
        HILOG_INFO();
        auto task = [this]() {
            napi_env tmpEnv = env_;
            auto closeScope = [tmpEnv](napi_handle_scope scope) {
                napi_close_handle_scope(tmpEnv, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(TmpOpenScope(env_), closeScope);
            napi_value handler = nullptr;
            napi_value callResult = nullptr;
            napi_value jsEvent = nullptr;
            napi_get_reference_value(env_, handlerRef_, &handler);
            napi_value undefined = nullptr;
            napi_get_undefined(env_, &undefined);
            napi_call_function(env_, undefined, handler, 0, &jsEvent, &callResult);
        };
        if (napi_send_event(env_, task, napi_eprio_high) != napi_status::napi_ok) {
            HILOG_ERROR("failed to send event");
        }
    }

    bool operator==(const DisconnectCallback& otherCallback) const
    {
        if (env_ != otherCallback.env_) {
            return false;
        }
        napi_value item = nullptr;
        napi_get_reference_value(env_, handlerRef_, &item);
        napi_value otherItem = nullptr;
        napi_get_reference_value(otherCallback.env_, otherCallback.handlerRef_, &otherItem);
        bool equalFlag = false;
        napi_status status = napi_strict_equals(env_, item, otherItem, &equalFlag);
        if (status == napi_ok && equalFlag) {
            return true;
        }
        return false;
    }

    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
};

class AccessibleAbilityClient : public virtual RefBase {
public:
    /**
     * @brief Destruct
     */
    virtual ~AccessibleAbilityClient() = default;

    /**
     * @brief Gets an instance of AccessibleAbilityClient.
     * @return Return an instance of AccessibleAbilityClient.
     */
    static sptr<AccessibleAbilityClient> GetInstance();

    /**
     * @brief Gets remote object.
     * @return Remote object.
     */
    virtual sptr<IRemoteObject> GetRemoteObject() = 0;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetFocus(
        const int32_t focusType, AccessibilityElementInfo& elementInfo, bool systemApi = false) = 0;

    /**
     * @brief Obtains elementInfo of focus.
     * @param sourceInfo The source info to get focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo, const int32_t focusType,
        AccessibilityElementInfo &elementInfo) = 0;

    /**
     * @brief Sends simulate gestures to the screen.
     * @param gesturePath The gesture which need to send.
     * @return Return RET_OK if the gesture sends successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath) = 0;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRoot(AccessibilityElementInfo &elementInfo, bool systemApi = false) = 0;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param windowInfo The source window info to get root.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
        AccessibilityElementInfo &elementInfo, bool systemApi = false) = 0;

    /**
     * @brief Obtains elementInfos of the accessible root node in batchs.
     * @param elementInfos ElementInfos of the accessible root node and its recursive subnodes.
     * @return Return RET_OK if obtains elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootBatch(std::vector<AccessibilityElementInfo>& elementInfos) = 0;

    /**
     * @brief Obtains elementInfos of the accessible root node in batchs.
     * @param windowInfo The source window info to get root.
     * @param elementInfos ElementInfos of the accessible root node and its recursive subnodes.
     * @param isFilter Indicates whether to filter nodes.
     * @param needCut Indicates whether to remove invisible nodes.
     * @return Return RET_OK if obtains elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootByWindowBatch(const AccessibilityWindowInfo &windowInfo,
        std::vector<AccessibilityElementInfo>& elementInfos, bool isFilter = false, bool needCut = false) = 0;

    /**
     * @brief Get the window information related with the event
     * @param windowId The window id.
     * @param windowInfo The window information.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo) = 0;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi = false) = 0;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @param windows The information of windows.
     * @return Return RET_OK if obtains windowInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetWindows(
        const uint64_t displayId, std::vector<AccessibilityWindowInfo>& windows, bool systemApi = false) = 0;

    /**
     * @brief Gets the next focused node in the specified direction of the currently focused node.
     * @param elementInfo The source info to get next info.
     * @param direction Indicates the direction to obtain the next focused node. Refer to FocusMoveDirection
     * @param nextElementInfo The info of next element.
     * @return Return RET_OK if gets next elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetNext(const AccessibilityElementInfo &elementInfo, const FocusMoveDirection direction,
        AccessibilityElementInfo &nextElementInfo, bool systemApi = false) = 0;

    /**
     * @brief Get the child node information by child index.
     * @param index The index of the child.
     * @param parent The parent info to get child.
     * @param child The element info of child.
     * @return Return RET_OK if gets child elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
        AccessibilityElementInfo &child) = 0;

    /**
     * @brief Get the children node information
     * @param parent The parent info to get children.
     * @param children The element info of children.
     * @return Return RET_OK if gets children elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetChildren(const AccessibilityElementInfo &parent,
        std::vector<AccessibilityElementInfo> &children, bool systemApi = false) = 0;

    /**
     * @brief Searches for node information based on the specified content.
     * @param elementInfo The source info.
     * @param text specified content
     * @param elementInfos The element infos of specified content.
     * @return Return RET_OK if gets elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
        std::vector<AccessibilityElementInfo> &elementInfos, bool systemApi = false) = 0;

    /**
     * @brief Get the node information related with the event
     * @param eventInfo The source info to get source.
     * @param elementInfo The element info of source.
     * @return Return RET_OK if gets elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetSource(const AccessibilityEventInfo &eventInfo, AccessibilityElementInfo &elementInfo) = 0;

    /**
     * @brief Get Parent node information
     * @param child The child element info to get parent.
     * @param parent The parent element info.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetParentElementInfo(
        const AccessibilityElementInfo& child, AccessibilityElementInfo& parent, bool systemApi = false) = 0;

    /**
     * @brief Get node information based on element id in active window.
     * @param elementId The target element id.
     * @param windowId The target window id.
     * @param targetElementInfo The element info of specified content.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetByElementId(const int64_t elementId, const int32_t windowId,
        AccessibilityElementInfo &targetElementInfo, bool systemApi = false) = 0;

    /**
     * @brief Get node information based on inspectorKey in active window.
     * @param inspectorKey The target inspectorKey.
     * @param elementInfo The element info of specified content.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchElementInfoByInspectorKey(const std::string &inspectorKey,
        AccessibilityElementInfo &elementInfo) = 0;

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
     * @return Return RET_OK if performs action succeed, otherwise refer to the RetError for the failure.
     */
    virtual RetError ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
        const std::map<std::string, std::string> &actionArguments) = 0;

    /**
     * @brief Enable status value.
     * @param isEnable flag Screen Curtain.
     * @return Return RET_OK if registers listener successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError EnableScreenCurtain(bool isEnable) = 0;

    /**
    * @brief To return the result of cursor position.
    * @param elementInfo The source info to cursor position.
    * @param position: The position of the cursor to get.
    */
    virtual RetError GetCursorPosition(const AccessibilityElementInfo &elementInfo, int32_t &position) = 0;

    /**
     * @brief Check the permission of the extension ability.
     * @return Return RET_OK if the ability has the permission.
     */
    virtual RetError CheckExtensionAbilityPermission() = 0;

    /**
     * @brief Register ability listener.
     * @param listener The listener to add.
     * @return Return RET_OK if registers listener successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener) = 0;

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return RET_OK if sets target bundle names successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) = 0;

    /**
     * @brief Set cache mode.
     *        The mode is used for functions: GetRoot, GetRootByWindow, GetChildElementInfo,
     *        GetChildren, GetSource, GetParentElementInfo.
     * @param cacheMode The cache mode. It includes:
     *             PREFETCH_PREDECESSORS: cache the parent node info also.
     *             PREFETCH_SIBLINGS: cache the sister/brothers node info also.
     *             PREFETCH_CHILDREN: cache the child node info also.
     *             otherwise: no cache.
     * @return Return RET_OK if sets cache mode successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SetCacheMode(const int32_t cacheMode) = 0;

    /**
     * @brief Find the node information by accessibility ID.
     * @param accessibilityWindowId The window id that the component belongs to.
     * @param elementId: The unique id of the component ID.
     * @param mode PREFETCH_PREDECESSORS: Need to make the parent node info also.
     *              PREFETCH_SIBLINGS: Need to make the sister/brothers node info also.
     *              PREFETCH_CHILDREN: Need to make the child node info also.
     *              otherwise: Make the node information by elementId only.
     * @param info[out] The components information matched conditions searched.
     * @return Return RET_OK if search element info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchElementInfoByAccessibilityId(const int32_t windowId, const int64_t elementId,
        const uint32_t mode, AccessibilityElementInfo &info, bool isFilter = false) = 0;

    /**
     * @brief Search all child nodes
     * @param windowId The target window id.
     * @param elementId The target element id.
     * @param elementInfos The element infos of specified content.
     * @return Return RET_OK if gets all child nodes successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetElements(const int32_t windowId, const int64_t elementId,
        std::vector<AccessibilityElementInfo> &elementInfos) = 0;

    /**
     * @brief Search Default Focused ElementId
     * @param windowId The target window id.
     * @param elementInfos The element infos of specified content.
     * @return Return RET_OK if gets all Default Focused successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetDefaultFocusedElementIds(const int32_t windowId,
        std::vector<AccessibilityElementInfo> &elementInfos) = 0;

    /**
     * @brief Hold running lock to prevent screen turning off automatically.
     * @param null.
     * @return Return RET_OK if hold running lock successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError HoldRunningLock() = 0;
 
    /**
     * @brief Unhold running lock to prevent screen turning off automatically
     * @param null.
     * @return Return RET_OK if Unhold running lock successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError UnholdRunningLock() = 0;

    /**
     * @brief Register Disconnect Callback.
     * @param callback The callback.
     * @return Return RET_OK if Register Callback successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterDisconnectCallback(std::shared_ptr<DisconnectCallback> &callback) = 0;

    /**
     * @brief UnRegister Disconnect Callback.
     * @return Return RET_OK if unRegister callback successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError UnRegisterDisconnectCallback(std::shared_ptr<DisconnectCallback> &callback) = 0;

    /**
     * @brief Notify disconnect.
     * @return Return RET_OK if notifyDisconnect successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError NotifyDisconnect() = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CLIENT_H