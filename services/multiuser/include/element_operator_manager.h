/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ELEMENT_OPERATOR_MANAGER_H
#define ELEMENT_OPERATOR_MANAGER_H

#include <map>
#include <vector>
#include <set>
#include <atomic>
#include <future>
#include <chrono>

#include "accessibility_element_info.h"
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_window_connection.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
class ElementOperatorManager {
public:
    ElementOperatorManager() = default;
    ~ElementOperatorManager() = default;

    /**
     * @brief Add interface operation interactive connection.
     * @param windowId Interface operation interactive connection the
     * corresponding window id.
     * @param interactionConnection Interface interface operation
     * interactive connection.
     */
    void AddAccessibilityWindowConnection(
        const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection);

    /**
     * @brief Remove interface operation interactive connection.
     * @param windowId Interface operation interactive connection the corresponding window id.
     */
    void RemoveAccessibilityWindowConnection(const int32_t windowId);

    /**
     * @brief Query interface operation interactive connection through window id.
     * @param windowId Interface operation interactive connection the corresponding window id.
     * @return Interface operation interactive connection corresponding to window id.
     */
    const sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(const int32_t windowId);

    /**
     * @brief Delete connection and death recipient for a window.
     * @param windowId Window ID
     * @param connection Window connection object
     */
    void DeleteConnectionAndDeathRecipient(int32_t windowId, const sptr<AccessibilityWindowConnection>& connection);

    /**
     * @brief Verify token ID for element access.
     * @param tokenId Token ID to verify
     * @param windowId Window ID
     * @param elementId Element ID
     * @return Error code
     */
    RetError VerifyingToKenId(const uint32_t tokenId, const int32_t windowId, const int64_t elementId);

    /**
     * @brief Get root parent ID for a tree.
     * @param windowId Window ID
     * @param treeId Tree ID
     * @param parentId Output parent ID
     * @return Error code
     */
    ErrCode GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId);

    /**
     * @brief Get parent element recursively.
     * @param windowId Window ID
     * @param elementId Element ID
     * @param infos Output element info list
     * @return True if successful
     */
    bool GetParentElementRecursively(int32_t windowId, int64_t elementId, std::vector<AccessibilityElementInfo>& infos);

    /**
     * @brief Set death recipient for element operator.
     * @param windowId Window ID
     * @param object Remote object
     * @param accountId Account ID
     */
    void SetElementOperatorDeathRecipient(int32_t windowId, const sptr<IRemoteObject>& object, int32_t accountId);

    /**
     * @brief Set death recipient for element operator with tree ID.
     * @param windowId Window ID
     * @param treeId Tree ID
     * @param object Remote object
     * @param accountId Account ID
     */
    void SetElementOperatorDeathRecipient(int32_t windowId, int32_t treeId, const sptr<IRemoteObject>& object,
        int32_t accountId);

    /**
     * @brief Remove death recipient for element operator.
     * @param windowId Window ID
     * @param object Remote object
     */
    void RemoveElementOperatorDeathRecipient(int32_t windowId, const sptr<IRemoteObject>& object);

    /**
     * @brief Clear focus for a window.
     * @param windowId Window ID
     * @return Error code
     */
    RetError ClearFocus(int32_t windowId);

    /**
     * @brief Handle outside touch for a window.
     * @param windowId Window ID
     */
    void OutsideTouch(int32_t windowId);

    /**
     * @brief Add death recipient for element operator.
     * @param windowId Window ID
     * @param deathRecipient Death recipient object
     */
    void AddDeathRecipient(int32_t windowId, const sptr<IRemoteObject::DeathRecipient>& deathRecipient);

    /**
     * @brief Add death recipient for element operator with tree ID.
     * @param windowId Window ID
     * @param treeId Tree ID
     * @param deathRecipient Death recipient object
     */
    void AddDeathRecipient(int32_t windowId, int32_t treeId, const sptr<IRemoteObject::DeathRecipient>& deathRecipient);

    /**
     * @brief Remove death recipient for element operator.
     * @param windowId Window ID
     */
    void RemoveDeathRecipient(int32_t windowId);

    /**
     * @brief Remove death recipient for element operator with tree ID.
     * @param windowId Window ID
     * @param treeId Tree ID
     */
    void RemoveDeathRecipient(int32_t windowId, int32_t treeId);

    /**
     * @brief Remove tree death recipient and clean up proxy.
     * @param windowId Window ID
     * @param treeId Tree ID
     * @param connection Window connection object
     */
    void RemoveTreeDeathRecipient(int32_t windowId, int32_t treeId,
        const sptr<AccessibilityWindowConnection>& connection);

    /**
     * @brief Get all accessibility window connections (for backward compatibility).
     * @return Reference to the connections map
     */
    const std::map<int32_t, sptr<AccessibilityWindowConnection>>& GetAsacConnections() const;

    /**
     * @brief Clear all accessibility window connections.
     */
    void ClearAllConnections();

    /**
     * @brief Request management interfaces.
     */
    int32_t GenerateRequestId();
    void AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
        sptr<IAccessibilityElementOperatorCallback> callback);
    ErrCode RemoveRequestId(int32_t requestId);
    void StopCallbackWait(int32_t windowId);
    void StopCallbackWait(int32_t windowId, int32_t treeId);

    /**
     * @brief Utility methods.
     */
    static int32_t GetTreeIdBySplitElementId(const int64_t elementId);

    /**
     * @brief Callback implementation for element operations.
     */
    class ElementOperatorCallbackImpl : public AccessibilityElementOperatorCallbackStub {
    public:
        ElementOperatorCallbackImpl() = default;
        virtual ~ElementOperatorCallbackImpl() = default;

        virtual void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
            const int32_t requestId) override;
        virtual void SetSearchDefaultFocusByWindowIdResult(const std::vector<AccessibilityElementInfo> &infos,
            const int32_t requestId) override;
        virtual void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
            const int32_t requestId) override;
        virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
            const int32_t requestId) override;
        virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) override;
        virtual void SetExecuteActionResult(const bool succeeded, const int32_t requestId) override;
        virtual void SetCursorPositionResult(const int32_t cursorPosition, const int32_t requestId) override;
        virtual void SetSearchElementInfoBySpecificPropertyResult(const std::list<AccessibilityElementInfo> &infos,
            const std::list<AccessibilityElementInfo> &treeInfos, const int32_t requestId) override;

    private:
        ffrt::promise<void> promise_;
        bool executeActionResult_ = false;
        AccessibilityElementInfo accessibilityInfoResult_ = {};
        std::vector<AccessibilityElementInfo> elementInfosResult_;
        int32_t callCursorPosition_ = 0;

        friend class ElementOperatorManager;
    };

    class ElementOperatorDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        ElementOperatorDeathRecipient(int32_t windowId, int32_t accountId) : windowId_(windowId),
            accountId_(accountId) {};
        ElementOperatorDeathRecipient(int32_t windowId, int32_t treeId, int32_t accountId) : windowId_(windowId),
            treeId_(treeId), accountId_(accountId) {};
        ~ElementOperatorDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(ElementOperatorDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
        int32_t windowId_ = INVALID_WINDOW_ID;
        int32_t treeId_ = INVALID_TREE_ID;
        int32_t accountId_ = 0;
    };

private:
    std::map<int32_t, sptr<AccessibilityWindowConnection>> asacConnections_;
    ffrt::mutex asacConnectionsMutex_;
    std::map<int32_t, sptr<IRemoteObject::DeathRecipient>> interactionOperationDeathRecipients_{};
    std::map<int32_t, std::map<int32_t, sptr<IRemoteObject::DeathRecipient>>> interactionOperationDeathMap_{};
    ffrt::mutex deathRecipientMutex_;

    std::atomic<int> requestId_ = 65535; // ELEMENT_REQUEST_ID_INIT
    std::map<int32_t, std::map<int32_t, std::set<int32_t>>> windowRequestIdMap_; // windowId->treeId->requestId
    std::map<int32_t, sptr<IAccessibilityElementOperatorCallback>> requestIdMap_; // requestId->callback
    ffrt::mutex requestMutex_; // mutex for request management

    // count ipc timeout number
    int64_t ipcTimeoutNum_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ELEMENT_OPERATOR_MANAGER_H
