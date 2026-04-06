/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <bitset>

#include "accessibility_window_connection.h"
#include "iaccessibility_element_operator.h"
#include "accessibility_ipc_types.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t REQUEST_ID_MAX = 0xFFFFFFFF;
    constexpr int32_t REQUEST_ID_MIN = 0x0000FFFF;
    constexpr int32_t TREE_ID_MAX = 0x00001FFF;
}
class AccessibilityAccountData;

class ElementOperatorManager {
public:
    ElementOperatorManager() = default;
    ~ElementOperatorManager();

    void SetAccountData(int32_t accountId, const wptr<AccessibilityAccountData>& accountData);
    void AddAccessibilityWindowConnection(
        const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection);

    void RemoveAccessibilityWindowConnection(const int32_t windowId);

    sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(const int32_t windowId);

    const std::map<int32_t, sptr<AccessibilityWindowConnection>> GetAsacConnections();

    void ClearFocus(int32_t windowId);

    void OutsideTouch(int32_t windowId);

    ErrCode GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId);

    void RemoveTreeDeathRecipient(int32_t windowId, int32_t treeId,
        const sptr<AccessibilityWindowConnection> connection);

    void Clear();
    RetError RegisterElementOperatorByWindowId(int32_t windowId,
        const sptr<IAccessibilityElementOperator> &elementOperator, uint32_t tokenId, bool isBroker,
        uint64_t displayId);
    RetError RegisterElementOperatorByParameter(const RegistrationPara &parameter,
        const sptr<IAccessibilityElementOperator> &elementOperator, uint32_t tokenId, bool isApp);
    RetError DeregisterElementOperatorByWindowId(int32_t windowId, uint64_t displayId, bool isBroker);
    RetError DeregisterElementOperatorByWindowIdAndTreeId(int32_t windowId, int32_t treeId, uint64_t displayId);
    RetError SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t flag, uint32_t tokenId);

    void AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
        sptr<IAccessibilityElementOperatorCallback> callback);
    void RemoveRequestId(int32_t requestId);
    void StopCallbackWait(int32_t windowId);
    void StopCallbackWait(int32_t windowId, int32_t treeId);
    bool GetParentElementRecursively(int32_t windowId, int64_t elementId, std::vector<AccessibilityElementInfo> &infos);
    bool ExecuteActionOnAccessibilityFocused(ActionType action);
    bool FindFocusedElement(AccessibilityElementInfo &elementInfo, uint32_t timeout);
    void SetFocusWindowId(int32_t focusWindowId);
    void SetFocusElementId(int64_t focusElementId);
    RetError VerifyingToKenId(const int32_t windowId, const int64_t elementId, uint32_t tokenId);
private:
    bool InvalidHoverEnterEvent(AccessibilityEventInfo &event);
    bool InnerGetElementOperator(
        int32_t windowId, int64_t elementId, sptr<IAccessibilityElementOperator> &elementOperator);
    void OnFocusedEvent(const AccessibilityEventInfo &eventInfo);
    void UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event);
    // used for arkui windowId 1 map to WMS windowId
    void FindInnerWindowId(const AccessibilityEventInfo &event, int32_t& windowId);
    int32_t GenerateRequestId();
    void CalculateClickPosition(const AccessibilityElementInfo &focusedElementInfo, int32_t &xPos, int32_t &yPos);
    void GetElementOperatorConnection(const sptr<AccessibilityWindowConnection> &connection, int64_t elementId,
        sptr<IAccessibilityElementOperator> &elementOperator, uint64_t displayid);
    sptr<AccessibilityWindowConnection> GetRealIdWindowConnection(
        int32_t windowId, int32_t focusType, uint64_t &displayId);
    bool GetMagnificationState();
    bool FindFocusedElementByConnection(
        sptr<AccessibilityWindowConnection> connection, AccessibilityElementInfo &elementInfo, uint64_t displayId);

private:
    int32_t ApplyTreeId();
    void RecycleTreeId(int32_t treeId);
private:
    int32_t accountId_ = 0;
    std::map<int32_t, sptr<AccessibilityWindowConnection>> asacConnections_;
    ffrt::mutex asacConnectionsMutex_;
    ffrt::mutex requestIdmutex_; // current used for register state observer
    std::map<int32_t, std::map<int32_t, std::set<int32_t>>> windowRequestIdMap_ {}; // windowId->treeId->requestId
    std::map<int32_t, sptr<IAccessibilityElementOperatorCallback>> requestIdMap_ {}; // requestId->callback
    wptr<AccessibilityAccountData> accountData_;
    std::atomic<int32_t> requestId_ = REQUEST_ID_MIN;
    std::atomic<int32_t> focusWindowId_ = -1;
    std::atomic<int64_t> focusElementId_ = -1;
 
    std::bitset<TREE_ID_MAX> treeIdPool_;
    int32_t preTreeId_ = -1;
    ffrt::mutex treeIdPoolMutex_;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBILITY_ELEMENT_OPERATOR_MANAGER_H