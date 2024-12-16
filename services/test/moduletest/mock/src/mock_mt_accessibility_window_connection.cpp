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

namespace OHOS {
namespace Accessibility {
MockAccessibilityWindowConnection::MockAccessibilityWindowConnection(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& connection, const int32_t accountId)
{
    windowId_ = windowId;
    proxy_ = connection;
    accountId_ = accountId;
}

MockAccessibilityWindowConnection::MockAccessibilityWindowConnection(
    const int32_t windowId, const int32_t treeId,
    const sptr<IAccessibilityElementOperator>& connection, const int32_t accountId)
{
    windowId_ = windowId;
    treeId_ = treeId;
    proxy_ = connection;
    accountId_ = accountId;
}

MockAccessibilityWindowConnection::~MockAccessibilityWindowConnection()
{}

RetError MockAccessibilityWindowConnection::SetCardProxy(const int32_t treeId,
    sptr<IAccessibilityElementOperator> operation)
{
    if (!operation) {
        return RET_ERR_FAILED;
    }
    cardProxy_[treeId] = operation;
    return RET_OK;
}

sptr<IAccessibilityElementOperator> MockAccessibilityWindowConnection::GetCardProxy(const int32_t treeId)
{
    if (!operation) {
        HILOG_DEBUG("SetCardProxy : operation is nullptr");
        return RET_ERR_FAILED;
    }
    cardProxy_.EnsureInsert(treeId, operation);
    return RET_OK;
}

RetError AccessibilityWindowConnection::SetTokenIdMap(const int32_t treeId,
    const uint32_t tokenId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    tokenIdMap_.EnsureInsert(treeId, tokenId);
    return RET_OK;
}

uint32_t AccessibilityWindowConnection::GetTokenIdMap(const int32_t treeId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    return tokenIdMap_.ReadVal(treeId);
}

void MockAccessibilityWindowConnection::GetAllTreeId(std::vector<int32_t> &treeIds)
{
    auto getAllTreeIdFunc = [&treeIds](const int32_t treeId, sptr<IAccessibilityElementOperator> connection) {
        treeIds.emplace_back(treeId);
    };
    cardProxy_.Iterate(getAllTreeIdFunc);
}

RetError AccessibilityWindowConnection::GetRootParentId(int32_t treeId, int64_t &elementId)
{
    bool ret = treeIdParentId_.Find(treeId, elementId);
    return ret == true ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilityWindowConnection::SetRootParentId(const int32_t treeId, const int64_t elementId)
{
    treeIdParentId_.EnsureInsert(treeId, elementId);
    return RET_OK;
}
void MockAccessibilityWindowConnection::EraseProxy(const int32_t treeId)
{
    sptr<IAccessibilityElementOperator> connection = nullptr;
    bool ret = cardProxy_.Find(treeId, connection);
    if (ret) {
        cardProxy_.Erase(treeId);
    }
}
} // namespace Accessibility
} // namespace OHOS