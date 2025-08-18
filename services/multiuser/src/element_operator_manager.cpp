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

#include "element_operator_manager.h"

#include <cinttypes>
#include <future>
#include <chrono>
#include "accessible_ability_manager_service.h"
#include "accessibility_constants.h"
#include "accessibility_element_info.h"
#include "accessibility_window_manager.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "singleton.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t ELEMENT_MOVE_BIT = 40;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
    constexpr int32_t WINDOW_ID_INVALID = -1;
    constexpr uint32_t ELEMENT_TIMEOUT_MS = 5000;
    constexpr int32_t REQUEST_ID_MAX = 0x0000FFFF;
    constexpr int32_t REQUEST_ID_MIN = 1;
} // namespace

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

const sptr<AccessibilityWindowConnection> ElementOperatorManager::GetAccessibilityWindowConnection(
    const int32_t windowId)
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

void ElementOperatorManager::DeleteConnectionAndDeathRecipient(int32_t windowId,
    const sptr<AccessibilityWindowConnection>& connection)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("connection or proxy is null");
        return;
    }
    RemoveAccessibilityWindowConnection(windowId);
    auto object = connection->GetProxy()->AsObject();
    if (object) {
        std::lock_guard lock(deathRecipientMutex_);
        auto iter = interactionOperationDeathRecipients_.find(windowId);
        if (iter != interactionOperationDeathRecipients_.end()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient = iter->second;
            bool result = object->RemoveDeathRecipient(deathRecipient);
            HILOG_DEBUG("The result of deleting connection's death recipient is %{public}d", result);
            interactionOperationDeathRecipients_.erase(iter);
        }
    }
}

RetError ElementOperatorManager::VerifyingToKenId(const uint32_t tokenId, const int32_t windowId,
    const int64_t elementId)
{
    int32_t treeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
    HILOG_DEBUG("VerifyingToKenId: treeId[%{public}d], windowId[%{public}d], elementId[%{public}" PRId64 "]",
        treeId, windowId, elementId);
    if (elementId == ELEMENT_ID_INVALID || windowId == WINDOW_ID_INVALID) {
        HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
        return RET_OK;
    }

    HILOG_DEBUG("treeId %{public}d, windowId %{public}d", treeId, windowId);
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId,
        FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(realId);
    if (connection == nullptr) {
        HILOG_ERROR("connection is empty.");
        return RET_ERR_REGISTER_EXIST;
    }
    uint32_t expectTokenId = connection->GetTokenIdMap(treeId);
    if (tokenId != expectTokenId) {
        HILOG_ERROR("tokenId error!");
        return RET_ERR_TOKEN_ID;
    }

    return RET_OK;
}

ErrCode ElementOperatorManager::GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId)
{
    HILOG_INFO("aa search treeParent from aams,  windowId: %{public}d, treeId: %{public}d", windowId, treeId);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operator of windowId[%{public}d] has not been registered.", windowId);
        return RET_ERR_NO_CONNECTION;
    }
    connection->GetRootParentId(treeId, parentId);
    return RET_OK;
}

bool ElementOperatorManager::GetParentElementRecursively(int32_t windowId, int64_t elementId,
    std::vector<AccessibilityElementInfo>& infos)
{
    HILOG_DEBUG("windowId(%{public}d), elementId(%{public}" PRId64 ")", windowId, elementId);

    int32_t treeId = 0;
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
        return false;
    }

    if (elementId > 0) {
        treeId = GetTreeIdBySplitElementId(elementId);
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy();
    }

    if (elementOperator == nullptr) {
        HILOG_DEBUG("elementOperator failed elementId: %{public}" PRId64 " winId: %{public}d treeId: %{public}d",
            elementId, windowId, treeId);
        return false;
    }

    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl();
    if (callBack == nullptr) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }

    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    int32_t requestId = GenerateRequestId();
    AddRequestId(windowId, treeId, requestId, callBack);
    elementOperator->SearchElementInfoByAccessibilityId(elementId, requestId, callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(ELEMENT_TIMEOUT_MS));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        RemoveRequestId(requestId);
        return false;
    }

    for (const auto& info : callBack->elementInfosResult_) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfoByAccessibilityId elementInfo from ace is wrong");
            return false;
        }
    }

    infos = callBack->elementInfosResult_;
    HILOG_DEBUG("Get parent element success, size %{public}zu", infos.size());
    return true;
}

void ElementOperatorManager::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

void ElementOperatorManager::AddDeathRecipient(int32_t windowId,
    const sptr<IRemoteObject::DeathRecipient>& deathRecipient)
{
    std::lock_guard lock(deathRecipientMutex_);
    interactionOperationDeathRecipients_[windowId] = deathRecipient;
}

void ElementOperatorManager::AddDeathRecipient(int32_t windowId, int32_t treeId,
    const sptr<IRemoteObject::DeathRecipient>& deathRecipient)
{
    std::lock_guard lock(deathRecipientMutex_);
    interactionOperationDeathMap_[windowId][treeId] = deathRecipient;
}

void ElementOperatorManager::RemoveDeathRecipient(int32_t windowId)
{
    std::lock_guard lock(deathRecipientMutex_);
    auto iter = interactionOperationDeathRecipients_.find(windowId);
    if (iter != interactionOperationDeathRecipients_.end()) {
        interactionOperationDeathRecipients_.erase(iter);
    }
}

void ElementOperatorManager::RemoveDeathRecipient(int32_t windowId, int32_t treeId)
{
    std::lock_guard lock(deathRecipientMutex_);
    auto iter = interactionOperationDeathMap_.find(windowId);
    if (iter != interactionOperationDeathMap_.end()) {
        auto iterTree = iter->second.find(treeId);
        if (iterTree != iter->second.end()) {
            iter->second.erase(iterTree);
        }
    }
}

void ElementOperatorManager::RemoveTreeDeathRecipient(int32_t windowId, int32_t treeId,
    const sptr<AccessibilityWindowConnection>& connection)
{
    if (connection == nullptr) {
        HILOG_ERROR("connection is null");
        return;
    }

    auto object = connection->GetCardProxy(treeId);
    if (object == nullptr) {
        HILOG_ERROR("GetCardProxy is null");
        return;
    }
    auto remoteObject = object->AsObject();
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is null");
        return;
    }
    connection->EraseProxy(treeId);

    std::lock_guard lock(deathRecipientMutex_);
    auto iter = interactionOperationDeathMap_.find(windowId);
    if (iter != interactionOperationDeathMap_.end()) {
        auto iterTree = iter->second.find(treeId);
        if (iterTree != iter->second.end()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient = iterTree->second;
            bool result = remoteObject->RemoveDeathRecipient(deathRecipient);
            HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
            iter->second.erase(iterTree);
        } else {
            HILOG_ERROR("cannot find remote object. treeId[%{public}d]", treeId);
        }
    } else {
        HILOG_ERROR("cannot find remote object. windowId[%{public}d]", windowId);
    }
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>>& ElementOperatorManager::GetAsacConnections() const
{
    return asacConnections_;
}

int32_t ElementOperatorManager::GetTreeIdBySplitElementId(const int64_t elementId)
{
    return static_cast<int32_t>(static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
}

int32_t ElementOperatorManager::GenerateRequestId()
{
    std::lock_guard lock(requestMutex_);
    int32_t requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    if (requestId == REQUEST_ID_MAX) {
        requestId_ = REQUEST_ID_MIN;
        requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    }
    return requestId;
}

void ElementOperatorManager::AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
    sptr<IAccessibilityElementOperatorCallback> callback)
{
    std::lock_guard lock(requestMutex_);
    HILOG_DEBUG("Add windowId: %{public}d treeId: %{public}d requestId: %{public}d", windowId, treeId, requestId);
    if (!windowRequestIdMap_.count(windowId)) {
        windowRequestIdMap_[windowId] = {};
    }
    if (!windowRequestIdMap_[windowId].count(treeId)) {
        windowRequestIdMap_[windowId][treeId] = {};
    }
    if (!windowRequestIdMap_[windowId][treeId].count(requestId)) {
        windowRequestIdMap_[windowId][treeId].insert(requestId);
        requestIdMap_[requestId] = callback;
    }
}

ErrCode ElementOperatorManager::RemoveRequestId(int32_t requestId)
{
    std::lock_guard lock(requestMutex_);
    HILOG_DEBUG("RemoveRequestId requestId: %{public}d", requestId);
    for (auto &window : windowRequestIdMap_) {
        for (auto &tree : window.second) {
            auto it = tree.second.find(requestId);
            if (it != tree.second.end()) {
                HILOG_DEBUG("tree.second.erase requestId:%{public}d", requestId);
                tree.second.erase(it);
            }
            auto ite = requestIdMap_.find(requestId);
            if (ite != requestIdMap_.end()) {
                HILOG_DEBUG("requestIdMap_.erase requestId:%{public}d", requestId);
                requestIdMap_.erase(ite);
            }
        }
    }
    return ERR_OK;
}

void ElementOperatorManager::StopCallbackWait(int32_t windowId)
{
    HILOG_INFO("StopCallbackWait start windowId: %{public}d", windowId);
    std::lock_guard lock(requestMutex_);
    if (!windowRequestIdMap_.count(windowId)) {
        HILOG_DEBUG("windowId not exists");
        return;
    }
    for (auto iter = windowRequestIdMap_[windowId].begin(); iter != windowRequestIdMap_[windowId].end(); ++iter) {
        HILOG_DEBUG("stop callback wait windowId: %{public}d, treeId: %{public}d", windowId, iter->first);
        StopCallbackWait(windowId, iter->first);
    }
}

void ElementOperatorManager::StopCallbackWait(int32_t windowId, int32_t treeId)
{
    std::lock_guard lock(requestMutex_);
    HILOG_DEBUG("StopCallbackWait start windowId: %{public}d treeId: %{public}d", windowId, treeId);
    if (!windowRequestIdMap_.count(windowId)) {
        return;
    }
    if (!windowRequestIdMap_[windowId].count(treeId)) {
        return;
    }

    auto requestIds = windowRequestIdMap_[windowId][treeId];
    for (auto requestId = requestIds.begin(); requestId != requestIds.end();) {
        HILOG_DEBUG("stop callback wait windowId: %{public}d, requestId: %{public}d", windowId, *requestId);
        auto iter = requestIdMap_.find(*requestId);
        if (iter != requestIdMap_.end()) {
            HILOG_DEBUG("requestIdMap_ set callback and erase requestId:%{public}d", *requestId);
            sptr<IAccessibilityElementOperatorCallback> callback = requestIdMap_[*requestId];
            if (callback != nullptr) {
                callback->SetExecuteActionResult(false, *requestId);
            }
            requestIdMap_.erase(iter);
        }
        requestId = requestIds.erase(requestId);
    }
}

RetError ElementOperatorManager::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);

    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("No connection found for windowId(%{public}d)", windowId);
        return RET_ERR_NO_CONNECTION;
    }

    sptr<IAccessibilityElementOperator> elementOperator = connection->GetProxy();
    if (!elementOperator) {
        HILOG_WARN("No elementOperator found for windowId(%{public}d)", windowId);
        return RET_ERR_NO_CONNECTION;
    }
    elementOperator->ClearFocus();
    return RET_OK;
}

void ElementOperatorManager::ClearAllConnections()
{
    std::lock_guard lock(asacConnectionsMutex_);
    asacConnections_.clear();

    std::lock_guard deathLock(deathRecipientMutex_);
    interactionOperationDeathRecipients_.clear();
    interactionOperationDeathMap_.clear();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    std::vector<AccessibilityElementInfo> validatedInfos;
    for (const auto& info : infos) {
        auto& service = Singleton<AccessibleAbilityManagerService>::GetInstance();
        auto accountData = service.GetCurrentAccountData();
        if (accountData) {
            uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
            RetError ret = accountData->VerifyingToKenId(tokenId, info.GetWindowId(), info.GetAccessibilityId());
            if (ret == RET_OK) {
                validatedInfos.push_back(info);
                HILOG_DEBUG("Element info validated, windowId: %{public}d, elementId: %{public}" PRId64 "",
                    info.GetWindowId(), info.GetAccessibilityId());
            } else {
                HILOG_WARN("Element info validation failed, windowId: %{public}d, elementId: %{public}" PRId64 "",
                    info.GetWindowId(), info.GetAccessibilityId());
            }
        }
    }
    elementInfosResult_ = validatedInfos;

    auto& service = Singleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = service.GetCurrentAccountData();
    if (accountData) {
        accountData->GetOperatorManager().RemoveRequestId(requestId);
    }
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetCursorPositionResult(
    const int32_t cursorPosition, const int32_t requestId)
{
    HILOG_DEBUG("cursorPosition [result:%{public}d, requestId:%{public}d]", cursorPosition, requestId);
    callCursorPosition_ = cursorPosition;
    promise_.set_value();
}

void ElementOperatorManager::ElementOperatorCallbackImpl::SetSearchElementInfoBySpecificPropertyResult(
    const std::list<AccessibilityElementInfo> &infos, const std::list<AccessibilityElementInfo> &treeInfos,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (!infos.empty()) {
        elementInfosResult_.assign(infos.begin(), infos.end());
    } else if (!treeInfos.empty()) {
        elementInfosResult_.assign(treeInfos.begin(), treeInfos.end());
    }
    promise_.set_value();
}

void ElementOperatorManager::SetElementOperatorDeathRecipient(int32_t windowId, const sptr<IRemoteObject>& object,
    int32_t accountId)
{
    HILOG_DEBUG("windowId(%{public}d), accountId(%{public}d)", windowId, accountId);
    if (!object) {
        HILOG_ERROR("object is null");
        return;
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) ElementOperatorDeathRecipient(windowId, accountId);
    if (!deathRecipient) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Create ElementOperatorDeathRecipient failed");
        return;
    }

    bool result = object->AddDeathRecipient(deathRecipient);
    if (result) {
        std::lock_guard lock(deathRecipientMutex_);
        interactionOperationDeathRecipients_[windowId] = deathRecipient;
        HILOG_DEBUG("Successfully added death recipient for windowId(%{public}d)", windowId);
    } else {
        HILOG_ERROR("Failed to add death recipient for windowId(%{public}d)", windowId);
    }
}

void ElementOperatorManager::SetElementOperatorDeathRecipient(int32_t windowId, int32_t treeId,
    const sptr<IRemoteObject>& object, int32_t accountId)
{
    HILOG_DEBUG("windowId(%{public}d), treeId(%{public}d), accountId(%{public}d)", windowId, treeId, accountId);

    if (!object) {
        HILOG_ERROR("object is null");
        return;
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new(std::nothrow) ElementOperatorDeathRecipient(windowId, treeId, accountId);
    if (!deathRecipient) {
        HILOG_ERROR("Create ElementOperatorDeathRecipient failed");
        return;
    }

    bool result = object->AddDeathRecipient(deathRecipient);
    if (result) {
        std::lock_guard lock(deathRecipientMutex_);
        interactionOperationDeathMap_[windowId][treeId] = deathRecipient;
        HILOG_DEBUG("Successfully added death recipient for windowId(%{public}d), treeId(%{public}d)", windowId,
            treeId);
    } else {
        HILOG_ERROR("Failed to add death recipient for windowId(%{public}d), treeId(%{public}d)", windowId, treeId);
    }
}

void ElementOperatorManager::RemoveElementOperatorDeathRecipient(int32_t windowId, const sptr<IRemoteObject>& object)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);

    if (!object) {
        HILOG_ERROR("object is null");
        return;
    }

    std::lock_guard lock(deathRecipientMutex_);
    auto iter = interactionOperationDeathRecipients_.find(windowId);
    if (iter != interactionOperationDeathRecipients_.end()) {
        HILOG_DEBUG("delete death recipient, window id = %{public}d", windowId);
        sptr<IRemoteObject::DeathRecipient> deathRecipient = iter->second;
        bool result = object->RemoveDeathRecipient(deathRecipient);
        HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
        interactionOperationDeathRecipients_.erase(iter);
    } else {
        HILOG_INFO("cannot find remote object. windowId[%{public}d]", windowId);
    }
}

void ElementOperatorManager::ElementOperatorDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("ElementOperatorDeathRecipient OnRemoteDied windowId[%{public}d], treeId[%{public}d],"
        " accountId[%{public}d]", windowId_, treeId_, accountId_);

    auto& service = Singleton<AccessibleAbilityManagerService>::GetInstance();
    int32_t currentAccountId = service.GetCurrentAccountId();
    if (currentAccountId != accountId_) {
        HILOG_ERROR("check accountId failed");
        return;
    }

    if (treeId_ > 0) {
        service.DeregisterElementOperatorByWindowIdAndTreeId(windowId_, treeId_);
    } else {
        service.DeregisterElementOperatorByWindowId(windowId_);
    }
}
} // namespace Accessibility
} // namespace OHOS
