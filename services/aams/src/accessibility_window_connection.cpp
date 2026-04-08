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
#include "utils.h"
#include "accessibility_account_data.h"
#include "accessible_ability_manager_service.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
AccessibilityWindowConnection::AccessibilityWindowConnection(const int32_t windowId, const int32_t accountId)
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
{
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
    cardProxy_.Find(treeId, connection);
    HILOG_DEBUG("GetCardProxy : operation is %{public}d", connection != nullptr);
    return connection;
}

RetError AccessibilityWindowConnection::SetTokenIdMap(const int32_t treeId,
    const uint32_t tokenId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    if (windowId_ != SCENE_BOARD_WINDOW_ID) {
        tokenIdMap_.EnsureInsert(treeId, tokenId);
    } else {
        scbTokenMap_.insert(tokenId);
    }
    return RET_OK;
}

uint32_t AccessibilityWindowConnection::GetTokenIdMap(const int32_t treeId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    return tokenIdMap_.ReadVal(treeId);
}

bool AccessibilityWindowConnection::CheckScbTokenIdMap(uint32_t tokenId)
{
    return scbTokenMap_.count(tokenId) != 0;
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

void AccessibilityWindowConnection::AddDeathRecipient(
    sptr<IAccessibilityElementOperator> elementOperator, bool isBroker, uint64_t displayId)
{
    if (!elementOperator || !elementOperator->AsObject()) {
        return;
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) InteractionOperationDeathRecipient(windowId_, accountId_, displayId);
    if (!deathRecipient) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Create interactionOperationDeathRecipient failed");
        return;
    }
    if (elementOperator->AsObject()->AddDeathRecipient(deathRecipient)) {
        if (isBroker) {
            brokerProxy_ = elementOperator;
            brokerProxyDeathRecipient_ = deathRecipient;
        } else {
            proxyMap_.insert({displayId, {elementOperator, deathRecipient}});
        }
    }
}
 
void AccessibilityWindowConnection::ResetProxy()
{
    std::lock_guard<ffrt::mutex> lock(proxyMutex_);
    for (const auto &[displayId, value] : proxyMap_) {
        if (value.first && value.first->AsObject() && value.second) {
            value.first->AsObject()->RemoveDeathRecipient(value.second);
        }
    }
}
 
void AccessibilityWindowConnection::ResetBrokerProxy()
{
    std::lock_guard<ffrt::mutex> lock(proxyMutex_);
    if (brokerProxy_ && brokerProxy_->AsObject() && brokerProxyDeathRecipient_) {
        brokerProxy_->AsObject()->RemoveDeathRecipient(brokerProxyDeathRecipient_);
    }
    brokerProxy_ = nullptr;
}

void AccessibilityWindowConnection::AddTreeDeathRecipient(
    int32_t windowId, int32_t accountId, int32_t treeId, uint64_t displayId)
{
    sptr<IAccessibilityElementOperator> elementOperator = GetCardProxy(treeId);
    if (!elementOperator || !elementOperator->AsObject()) {
        return;
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) InteractionOperationDeathRecipient(windowId, treeId, accountId, displayId);
    if (!deathRecipient) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
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
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY);
    HILOG_INFO();
    if (treeId_ > 0) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().InnerDeregisterElementOperatorByWindowIdAndTreeId(
            windowId_, treeId_, accountId_, displayId_);
    } else {
        Singleton<AccessibleAbilityManagerService>::GetInstance().InnerDeregisterElementOperatorByWindowId(
            windowId_, accountId_, displayId_);
    }
}

void AccessibilityWindowConnection::ClearFocus()
{
    std::lock_guard<ffrt::mutex> lock(proxyMutex_);
    for (const auto& [displayId, value] : proxyMap_) {
        if (value.first) {
            value.first->ClearFocus();
        }
    }
}
 
void AccessibilityWindowConnection::OutsideTouch()
{
    std::lock_guard<ffrt::mutex> lock(proxyMutex_);
    for (const auto& [displayId, value] : proxyMap_) {
        if (value.first) {
            value.first->OutsideTouch();
        }
    }
}
} // namespace Accessibility
} // namespace OHOS
