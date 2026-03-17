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

#include "accessibility_element_operator_manager.h"
#include "utils.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
ElementOperatorManager::~ElementOperatorManager()
{
    Clear();
}

void ElementOperatorManager::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_[windowId] = interactionConnection;
}

void ElementOperatorManager::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_INFO("windowId(%{public}d)", windowId);
    std::lock_guard lock(asacConnectionsMutex_);
    std::map<int32_t, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

sptr<AccessibilityWindowConnection> ElementOperatorManager::GetAccessibilityWindowConnection(const int32_t windowId)
{
    std::lock_guard lock(asacConnectionsMutex_);
    HILOG_DEBUG("window id[%{public}d] interactionOperators's size[%{public}zu]", windowId, asacConnections_.size());
    for (auto &asacConnection : asacConnections_) {
        HILOG_DEBUG("window id of asacConnection is %{public}d", asacConnection.first);
    }

    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

    return nullptr;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> ElementOperatorManager::GetAsacConnections()
{
    HILOG_DEBUG("GetAsacConnections start.");
    return asacConnections_;
}

void ElementOperatorManager::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void ElementOperatorManager::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

bool ElementOperatorManager::GetElementOperator(const int32_t windowId, const int64_t elementId,
    sptr<IAccessibilityElementOperator>& elementOperator)
{
    int32_t treeId = 0;
    if (elementId > 0) {
        treeId = Utils::GetTreeIdBySplitElementId(elementId);
    }
    
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
    if (!connection) {
        return false;
    }
    
    if (elementId > 0) {
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy();
    }
    
    return elementOperator != nullptr;
}

void ElementOperatorManager::GetElementOperatorConnection(sptr<AccessibilityWindowConnection>& connection,
    const int64_t elementId, sptr<IAccessibilityElementOperator>& elementOperator)
{
    int32_t treeId = 0;
    if (elementId > 0) {
        treeId = Utils::GetTreeIdBySplitElementId(elementId);
        if (!connection->GetUseBrokerFlag() && treeId > 0) {
            elementOperator = connection->GetCardProxy(treeId);
        } else if (connection->IsAnco() && connection->GetUseBrokerFlag() && treeId > 0
            && connection->GetCardProxy(treeId)) {
            elementOperator = connection->GetCardProxy(treeId);
        } else {
            elementOperator = connection->GetProxy();
        }
    } else {
        elementOperator = connection->GetProxy();
    }
    HILOG_DEBUG("elementId:%{public}" PRId64 " treeId:%{public}d", elementId, treeId);
}

void ElementOperatorManager::SetTokenIdMapAndRootParentId(const sptr<AccessibilityWindowConnection> connection,
    const int32_t treeId, const int64_t nodeId, const uint32_t tokenId)
{
    connection->SetTokenIdMap(treeId, tokenId);
    connection->SetRootParentId(treeId, nodeId);
}

ErrCode ElementOperatorManager::GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId)
{
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operator of windowId[%{public}d] has not been registered.", windowId);
        return RET_ERR_NO_CONNECTION;
    }
    connection->GetRootParentId(treeId, parentId);
    return ERR_OK;
}

void ElementOperatorManager::RemoveTreeDeathRecipient(int32_t windowId, int32_t treeId,
    const sptr<AccessibilityWindowConnection> connection)
{
    if (connection == nullptr) {
        HILOG_ERROR("connection is null");
        return;
    }
    connection->EraseProxy(treeId);
    connection->RemoveTreeDeathRecipient(treeId);
}

void ElementOperatorManager::Clear()
{
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_.clear();
}

size_t ElementOperatorManager::ElementOperatorManager::GetSize()
{
    std::lock_guard lock(asacConnectionsMutex_);
    return asacConnections_.size();
}

} // namespace Accessibility
} // namespace OHOS