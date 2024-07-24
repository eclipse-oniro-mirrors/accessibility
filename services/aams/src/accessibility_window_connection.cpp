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

#include "accessibility_window_connection.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
AccessibilityWindowConnection::AccessibilityWindowConnection(const int32_t windowId,
    const sptr<IAccessibilityElementOperator> &connection, const int32_t accountId)
{
    windowId_ = windowId;
    proxy_ = connection;
    accountId_ = accountId;
    cardProxy_[0] = connection;
}

AccessibilityWindowConnection::AccessibilityWindowConnection(const int32_t windowId, const int32_t treeId,
    const sptr<IAccessibilityElementOperator> &connection, const int32_t accountId)
{
    windowId_ = windowId;
    treeId_ = treeId;
    cardProxy_[treeId] = connection;
    accountId_ = accountId;
}

AccessibilityWindowConnection::~AccessibilityWindowConnection()
{
}

RetError AccessibilityWindowConnection::SetCardProxy(const int32_t treeId,
    sptr<IAccessibilityElementOperator> operation)
{
    if (!operation) {
        HILOG_DEBUG("SetCardProxy : operation is nullptr");
        return RET_ERR_FAILED;
    }
    cardProxy_[treeId] = operation;
    return RET_OK;
}

sptr<IAccessibilityElementOperator> AccessibilityWindowConnection::GetCardProxy(const int32_t treeId)
{
    auto iter = cardProxy_.find(treeId);
    if (iter != cardProxy_.end()) {
        HILOG_DEBUG("GetCardProxy : operation is ok");
        return cardProxy_[treeId];
    }
    HILOG_DEBUG("GetCardProxy : operation is no");
    return nullptr;
}

RetError AccessibilityWindowConnection::SetTokenIdMap(const int32_t treeId,
    const uint32_t tokenId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    tokenIdMap_[treeId] = tokenId;
    return RET_OK;
}

uint32_t AccessibilityWindowConnection::GetTokenIdMap(const int32_t treeId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    return tokenIdMap_[treeId];
}

void AccessibilityWindowConnection::GetAllTreeId(std::vector<int32_t> &treeIds)
{
    for (auto &treeId: cardProxy_) {
        treeIds.emplace_back(treeId.first);
    }
}

RetError AccessibilityWindowConnection::GetRootParentId(int32_t treeId, int64_t &elementId)
{
    auto iter = treeIdParentId_.find(treeId);
    if (iter != treeIdParentId_.end()) {
        elementId = iter->second;
        return RET_OK;
    }
    return RET_ERR_FAILED;
}

RetError AccessibilityWindowConnection::SetRootParentId(const int32_t treeId, const int64_t elementId)
{
    treeIdParentId_[treeId] = elementId;
    return RET_OK;
}

void AccessibilityWindowConnection::EraseProxy(const int32_t treeId)
{
    cardProxy_.erase(treeId);
}
} // namespace Accessibility
} // namespace OHOS
