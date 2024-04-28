/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "accessible_ability_channel_client.h"
#include "accessible_ability_client.h"
#include "accessible_ability_client_stub.h"
#include "i_accessible_ability_manager_service.h"
#include "refbase.h"
#include "system_ability_load_callback_stub.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Accessibility {

constexpr int32_t SCENE_BOARD_WINDOW_ID = 1; // default scene board window id 1
constexpr int32_t INVALID_SCENE_BOARD_INNER_WINDOW_ID = -1; // invalid scene board window id -1
constexpr int64_t INVALID_SCENE_BOARD_ELEMENT_ID = -1; // invalid scene board element id -1
constexpr int32_t MAX_CACHE_WINDOW_SIZE = 5;

class AccessibleAbilityClientImpl : public AccessibleAbilityClient, public AccessibleAbilityClientStub {
public:
    /**
     * @brief The constructor of AccessibleAbilityClientImpl.
     */
    AccessibleAbilityClientImpl();

    /**
     * @brief The deconstructor of AccessibleAbilityClientImpl.
     */
    ~AccessibleAbilityClientImpl();

    /**
     * @brief Get the implement of accessibility ability client.
     */
    static sptr<AccessibleAbilityClientImpl> GetAbilityClientImplement();

    /**
     * @brief Gets remote object.
     * @return Remote object.
     */
    virtual sptr<IRemoteObject> GetRemoteObject() override;

    /**
     * @brief Register ability listener.
     * @param listener The listener to add.
     * @return Return RET_OK if registers listener successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener) override;

    /**
     * @brief Init accessible ability.
     * @param channel The object of IAccessibleAbilityChannel.
     * @param channelId The id of channel.
     */
    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId) override;

    /**
     * @brief Disconnect accessible ability.
     * @param channelId The id of channel.
     */
    virtual void Disconnect(const int32_t channelId) override;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     */
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @param sequence The sequence of the key event.
     */
    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence) override;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains elementInfo of focus.
     * @param sourceInfo The source info to get focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo, const int32_t focusType,
        AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Sends simulate gestures to the screen.
     * @param gesturePath The gesture which need to send.
     * @return Return RET_OK if the gesture sends successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath) override;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRoot(AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param windowInfo The source window info to get root.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return RET_OK if obtains elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
        AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Obtains elementInfos of the accessible root node in batchs.
     * @param elementInfos ElementInfos of the accessible root node and its recursive subnodes.
     * @return Return RET_OK if obtains elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootBatch(std::vector<AccessibilityElementInfo>& elementInfos) override;

    /**
     * @brief Obtains elementInfos of the accessible root node in batchs.
     * @param windowInfo The source window info to get root.
     * @param elementInfos ElementInfos of the accessible root node and its recursive subnodes.
     * @return Return RET_OK if obtains elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetRootByWindowBatch(const AccessibilityWindowInfo &windowInfo,
        std::vector<AccessibilityElementInfo>& elementInfos, bool isFilter) override;

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
    virtual RetError GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows) override;

    /**
     * @brief Gets the next focused node in the specified direction of the currently focused node.
     * @param elementInfo The source info to get next info.
     * @param direction Indicates the direction to obtain the next focused node. Refer to FocusMoveDirection
     * @param nextElementInfo The info of next element.
     * @return Return RET_OK if gets next elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetNext(const AccessibilityElementInfo &elementInfo, const FocusMoveDirection direction,
        AccessibilityElementInfo &nextElementInfo) override;

    /**
     * @brief Get the child node information by childId
     * @param index The index of the child.
     * @param parent The parent info to get child.
     * @param child The element info of child.
     * @return Return RET_OK if gets child elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
        AccessibilityElementInfo &child) override;

    /**
     * @brief Get the child node information
     * @param parent The parent info to get child.
     * @param children The element info of children.
     * @return Return RET_OK if gets child elementInfo successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetChildren(const AccessibilityElementInfo &parent,
        std::vector<AccessibilityElementInfo> &children) override;

    /**
     * @brief Searches for node information based on the specified content.
     * @param elementInfo The source info.
     * @param text specified content
     * @param elementInfos The element infos of specified content.
     * @return Return RET_OK if gets elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
        std::vector<AccessibilityElementInfo> &elementInfos) override;

    /**
     * @brief Get the node information related with the event
     * @param eventInfo The source info to get source.
     * @param elementInfo The element info of source.
     * @return Return RET_OK if gets elementInfos successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetSource(const AccessibilityEventInfo &eventInfo,
        AccessibilityElementInfo &elementInfo) override;

    /**
     * @brief Get Parent node information
     * @param child The child element info to get parent.
     * @param parent The parent element info.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetParentElementInfo(const AccessibilityElementInfo &child,
        AccessibilityElementInfo &parent) override;

    /**
     * @brief Get node information based on element id in active window.
     * @param elementId The target element id.
     * @param targetElementInfo The element info of specified content.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetByElementId(const int64_t elementId, AccessibilityElementInfo &targetElementInfo) override;

    /**
     * @brief Get node information based on inspectorKey in active window.
     * @param inspectorKey The target inspectorKey.
     * @param elementInfo The element info of specified content.
     * @return Return RET_OK if gets info successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SearchElementInfoByInspectorKey(const std::string &inspectorKey,
        AccessibilityElementInfo &elementInfo) override;

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
        const std::map<std::string, std::string> &actionArguments) override;

    /**
     * @brief Curtain screen ability.
     * @param isEnable Flag bits for opening or closing.
     */
    virtual RetError EnableScreenCurtain(bool isEnable) override;

    /**
     * @brief To return the result of cursor position.
     * @param elementInfo The source info to cursor position.
     * @param position: The position of the cursor to get.
     * @return Return RET_OK if performs action succeed, otherwise refer to the RetError for the failure.
     */
    virtual RetError GetCursorPosition(const AccessibilityElementInfo &elementInfo, int32_t &position) override;

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return RET_OK if sets target bundle names successfully, otherwise refer to the RetError for the failure.
     */
    virtual RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) override;

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
    virtual RetError SetCacheMode(const int32_t cacheMode) override;

    /**
     * @brief Clean data.
     * @param remote The object access to AAMS.
     */
    void ResetAAClient(const wptr<IRemoteObject> &remote);

    /**
     * @brief Notify AA client and clean data when service is died.
     * @param remote The object access to AAMS.
     */
    void NotifyServiceDied(const wptr<IRemoteObject> &remote);

    /**
     * @brief Connect to AAMS. For UI test.
     * @return Return RET_OK if the command of connection is sent successfully,
     *         otherwise refer to the RetError for the failure.
     */
    RetError Connect();

    /**
     * @brief disconnect to AAMS. For UI test.
     * @return Return RET_OK if the command of disconnect is sent successfully,
     *         otherwise refer to the RetError for the failure.
     */
    RetError Disconnect();

    /**
     * @brief Set connection state.
     * @param state Connnection state.
     */
    void SetConnectionState(bool state);

    void AddWindowElementMapByWMS(int32_t windowId, int64_t elementId);
    void AddWindowElementMapByAce(int32_t windowId, int64_t elementId);
    RetError GetElementInfoFromCache(int32_t windowId, int64_t elementId,
        std::vector<AccessibilityElementInfo> &elementInfos);
    RetError SearchElementInfoRecursive(int32_t windowId, int64_t elementId, int mode,
        std::vector<AccessibilityElementInfo> &elementInfos, bool isFilter = false);
    void RemoveCacheData(const AccessibilityEventInfo &eventInfo);
    void AddCacheByWMS(int32_t windowId, int64_t elementId, std::vector<AccessibilityElementInfo>& elementInfos);
    void AddCacheByAce(int32_t windowId, int64_t elementId, std::vector<AccessibilityElementInfo>& elementInfos);
    void SortElementInfosIfNecessary(std::vector<AccessibilityElementInfo> &elementInfos);

    bool LoadAccessibilityService();
    void LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject);
    void LoadSystemAbilityFail();

private:
    class ElementCacheInfo {
    public:
        ElementCacheInfo() = default;
        ~ElementCacheInfo() = default;
        void RemoveElementByWindowId(const int32_t windowId);
        bool GetElementByWindowId(const int32_t windowId, const int64_t elementId,
            std::vector<AccessibilityElementInfo>& elementInfos);
        bool GetElementByWindowIdBFS(const int64_t elementId, std::vector<AccessibilityElementInfo>& elementInfos,
            std::map<int32_t, std::shared_ptr<AccessibilityElementInfo>>& cache);
        void AddElementCache(int32_t windowId, const std::vector<AccessibilityElementInfo>& elementInfos);
        bool IsExistWindowId(int32_t windowId);
    private:
        std::map<int32_t, std::map<int32_t, std::shared_ptr<AccessibilityElementInfo>>> elementCache_;
        std::deque<int32_t> windowIdSet_;
        std::mutex elementCacheMutex_;
    };

    class SceneBoardWindowElementMap {
    public:
        SceneBoardWindowElementMap() = default;
        ~SceneBoardWindowElementMap() = default;
        bool IsExistWindowId(int32_t windowId);
        void AddWindowElementIdPair(int32_t windowId, int64_t elementId);
        std::vector<int32_t> GetWindowIdList();
        int32_t GetWindowIdByElementId(int64_t elementId);
        void RemovePairByWindowIdList(std::vector<int32_t>& windowIdList);
        void RemovePairByWindowId(int32_t windowId);
    private:
        std::map<int32_t, int64_t> windowElementMap_;
        std::mutex mapMutex_;
    };

    class AccessibleAbilityDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityDeathRecipient(AccessibleAbilityClientImpl &client) : client_(client) {}
        ~AccessibleAbilityDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote);
    private:
        AccessibleAbilityClientImpl &client_;
    };

    class AccessibilityServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibilityServiceDeathRecipient(AccessibleAbilityClientImpl &client) : client_(client) {}
        ~AccessibilityServiceDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibilityServiceDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote);
    private:
        AccessibleAbilityClientImpl &client_;
    };

    class AccessibilityLoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
            const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
    };

    class AccessibilitySaStatusChange : public SystemAbilityStatusChangeStub {
    public:
        void OnAddSystemAbility(int32_t saId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t saId, const std::string &deviceId) override;
    };

    bool GetCacheElementInfo(const int32_t windowId,
        const int64_t elementId, AccessibilityElementInfo &elementInfo) const;
    void SetCacheElementInfo(const int32_t windowId,
        const std::vector<OHOS::Accessibility::AccessibilityElementInfo> &elementInfos);
    RetError SearchElementInfoFromAce(const int32_t windowId, const int64_t elementId,
        const uint32_t mode, AccessibilityElementInfo &info);
    bool InitAccessibilityServiceProxy();
    sptr<Accessibility::IAccessibleAbilityManagerService> GetServiceProxy();

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> accessibilityServiceDeathRecipient_ = nullptr;
    sptr<IAccessibleAbilityManagerService> serviceProxy_ = nullptr;
    std::shared_ptr<AccessibleAbilityListener> listener_ = nullptr;
    std::shared_ptr<AccessibleAbilityChannelClient> channelClient_ = nullptr;
    uint32_t cacheMode_ = 0;
    int32_t cacheWindowId_ = -1;
    std::map<int32_t, AccessibilityElementInfo> cacheElementInfos_;
    std::mutex mutex_;
    std::atomic<bool> isConnected_ = false;
    // used for query element info in batch
    ElementCacheInfo elementCacheInfo_;
    SceneBoardWindowElementMap windowElementMap_;

    std::condition_variable proxyConVar_;
    std::mutex conVarMutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CLIENT_IMPL_H