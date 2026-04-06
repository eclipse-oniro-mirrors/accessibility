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
namespace {
    constexpr int32_t SCENE_BOARD_WINDOW_ID = 1;
}
AccessibilityWindowConnection::AccessibilityWindowConnection(
    const int32_t windowId, const int32_t accountId)
{
    windowId_ = windowId;
    accountId_ = accountId;
}

AccessibilityWindowConnection::AccessibilityWindowConnection(const int32_t windowId, const int32_t treeId,
    const sptr<IAccessibilityElementOperator> &elementOperator, const int32_t accountId)
{
    windowId_ = windowId;
    treeId_ = treeId;
    accountId_ = accountId;
    cardProxy_.EnsureInsert(treeId, elementOperator);
}

AccessibilityWindowConnection::~AccessibilityWindowConnection()
{}

RetError AccessibilityWindowConnection::SetCardProxy(const int32_t treeId,
    sptr<IAccessibilityElementOperator> operation)
{
    if (!operation) {
        HILOG_DEBUG("SetCardProxy : operation is nullptr");
        return RET_ERR_FAILED;
    }
    cardProxy_.EnsureInsert(treeId, operation);
    return RET_OK;
}

sptr<IAccessibilityElementOperator> AccessibilityWindowConnection::GetCardProxy(const int32_t treeId)
{
    sptr<IAccessibilityElementOperator> connection = nullptr;
    bool ret = cardProxy_.Find(treeId, connection);
    HILOG_DEBUG("GetCardProxy : operation is %{public}d", connection != nullptr);
    return connection;
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

void AccessibilityWindowConnection::GetAllTreeId(std::vector<int32_t> &treeIds)
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
void AccessibilityWindowConnection::EraseProxy(const int32_t treeId)
{
    sptr<IAccessibilityElementOperator> connection = nullptr;
    bool ret = cardProxy_.Find(treeId, connection);
    if (ret) {
        cardProxy_.Erase(treeId);
    }
}

void AccessibilityWindowConnection::SetProxy(uint64_t displayId, sptr<IAccessibilityElementOperator> proxy)
{
    AddDeathRecipient(proxy, false, displayId);
}
 
void AccessibilityWindowConnection::SetBrokerProxy(sptr<IAccessibilityElementOperator> proxy)
{
    AddDeathRecipient(proxy, true, 0);
}

void AccessibilityWindowConnection::AddDeathRecipient(sptr<IAccessibilityElementOperator> elementOperator, bool isBroker, uint64_t displayId)
{
    if (!elementOperator || !elementOperator->AsObject()) {
        return;
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) InteractionOperationDeathRecipient(windowId_, accountId_, displayId);
    if (!deathRecipient) {
        HILOG_ERROR("Create interactionOperationDeathRecipient failed");
        return;
    }
    if (elementOperator->AsObject()->AddDeathRecipient(deathRecipient)) {
        std::lock_guard<ffrt::mutex> lock(proxyMutex_);
        if (isBroker) {
            brokerProxy_ = elementOperator;
            brokerProxyDeathRecipient_ = deathRecipient;
        } else {
            proxyMap_.insert({displayId, {elementOperator, deathRecipient}});
        }
    }
}
 
void AccessibilityWindowConnection::AddTreeDeathRecipient(int32_t windowId, int32_t accountId, int32_t treeId, uint64_t displayId)
{
    sptr<IAccessibilityElementOperator> elementOperator = GetCardProxy(treeId);
    if (!elementOperator || !elementOperator->AsObject()) {
        return;
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) InteractionOperationDeathRecipient(windowId, treeId, accountId, displayId);
    if (!deathRecipient) {
        HILOG_ERROR("Create interactionOperationDeathRecipient failed");
    }
    if (elementOperator->AsObject()->AddDeathRecipient(deathRecipient)) {
        childTreeProxyDeathRecipient_.EnsureInsert(treeId, deathRecipient);
    }
}
 
void AccessibilityWindowConnection::RemoveTreeDeathRecipient(int32_t treeId)
{
    sptr<IAccessibilityElementOperator> elementOperator = GetCardProxy(treeId);
    if (!elementOperator || !elementOperator->AsObject()) {
        return;
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient = nullptr;
    childTreeProxyDeathRecipient_.Find(treeId, deathRecipient);
    if (!deathRecipient) {
        return;
    }
    elementOperator->AsObject()->RemoveDeathRecipient(deathRecipient);
    childTreeProxyDeathRecipient_.Erase(treeId);
}
 
void AccessibilityWindowConnection::InteractionOperationDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO();
}
 
void AccessibilityWindowConnection::ResetProxy()
{
    HILOG_INFO();
}
 
void AccessibilityWindowConnection::ResetBrokerProxy()
{
    HILOG_INFO();
}

bool AccessibilityWindowConnection::CheckScbTokenIdMap(uint32_t tokenId)
{
    return scbTokenMap_.count(tokenId) != 0;
}
 
sptr<IAccessibilityElementOperator> AccessibilityWindowConnection::GetProxy(uint64_t displayId)
{
    std::lock_guard<ffrt::mutex> lock(proxyMutex_);
    if (windowId_ != SCENE_BOARD_WINDOW_ID) {
        displayId = 0;
    }
    if (isUseBrokerProxy_) {
        return brokerProxy_;
    } else {
        return proxyMap_[displayId].first;
    }
}
} // namespace Accessibility
} // namespace OHOS