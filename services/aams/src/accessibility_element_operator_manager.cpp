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
#include "accessibility_account_data.h"
#include "element_operator_callback_impl.h"
#include "accessible_extend_manager_service_proxy.h"
#include "accessibility_window_manager.h"
#include "accessibility_security_component_manager.h"
#include "input_manager.h"
#include "pointer_event.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t SINGLE_TREE_ID = 0;
    constexpr int32_t DIVISOR_TWO = 2;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
    constexpr int32_t WINDOW_ID_INVALID = -1;
    constexpr uint32_t TIME_OUT_OPERATOR = 5000;
}
ElementOperatorManager::~ElementOperatorManager()
{
    Clear();
}

void ElementOperatorManager::SetAccountData(int32_t accountId, const wptr<AccessibilityAccountData>& accountData)
{
    accountId_ = accountId;
    accountData_ = accountData;
}
 
int32_t ElementOperatorManager::GenerateRequestId()
{
    int32_t requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    if (requestId == REQUEST_ID_MAX) {
        requestId_ = REQUEST_ID_MIN;
        requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    }
    return requestId;
}
 
int32_t ElementOperatorManager::ApplyTreeId()
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    int32_t curTreeId = preTreeId_ + 1;
    for (int32_t index = 0; index < TREE_ID_MAX; index++) {
        if (curTreeId == TREE_ID_MAX) {
            curTreeId = 0;
        }
        if (!treeIdPool_.test(curTreeId)) {
            treeIdPool_.set(curTreeId, true);
            preTreeId_ = curTreeId;
            return curTreeId + 1;
        }
        curTreeId++;
    }
    preTreeId_ = TREE_ID_MAX - 1;
    return 0;
}
 
void ElementOperatorManager::RecycleTreeId(int32_t treeId)
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    if ((treeId > 0) && (treeId <= TREE_ID_MAX)) {
        treeIdPool_.set(treeId - 1, false);
    }
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
    if (connection) {
        connection->ClearFocus();
    }
}

void ElementOperatorManager::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection) {
        connection->OutsideTouch();
    }
}

ErrCode ElementOperatorManager::GetRootParentId(int32_t windowId, int32_t treeId, int64_t& parentId)
{
    uint64_t displayId = 0;
    sptr<AccessibilityWindowConnection> connection = GetRealIdWindowConnection(windowId, FOCUS_TYPE_INVALID, displayId);
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

RetError ElementOperatorManager::RegisterElementOperatorByWindowId(
    int32_t windowId, const sptr<IAccessibilityElementOperator> &elementOperator, uint32_t tokenId, bool isBroker, uint64_t displayId)
{
    HILOG_INFO("RegisterElementOperatorByWindowId %{public}d %{public}llu", windowId, displayId);
    sptr<AccessibilityWindowConnection> oldConnection = GetAccessibilityWindowConnection(windowId);
    if (!isBroker && oldConnection && oldConnection->GetRawProxy(displayId)) {
        HILOG_WARN("no need to register again.");
        return RET_ERR_REGISTER_EXIST;
    }
    sptr<AccessibilityWindowConnection> connection = oldConnection;
    if (connection == nullptr) {
        connection = new (std::nothrow) AccessibilityWindowConnection(windowId, accountId_);
        AddAccessibilityWindowConnection(windowId, connection);
    }
    if (!connection) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
        return RET_ERR_FAILED;
    }
    if (!isBroker) {
        connection->SetProxy(displayId, elementOperator);
    } else {
        connection->SetBrokerProxy(elementOperator);
        connection->SetUseBrokerFlag(true);
        connection->SetAncoFlag(isBroker);
    }
    connection->SetTokenIdMap(SINGLE_TREE_ID, tokenId);
    return RET_OK;
}

RetError ElementOperatorManager::RegisterElementOperatorByParameter(const RegistrationPara &parameter,
    const sptr<IAccessibilityElementOperator> &elementOperator, uint32_t tokenId, bool isApp)
{
    int32_t treeId = ApplyTreeId();
    if (treeId == 0) {
        HILOG_ERROR("TreeId is used up.");
        return RET_ERR_TREE_TOO_BIG;
    }
    HILOG_INFO("get treeId element and treeid - treeId: %{public}d parameter.elementId[%{public}" PRId64 "]",
        treeId,
        parameter.elementId);
    uint64_t displayId = parameter.displayId;
    sptr<AccessibilityWindowConnection> parentConnection = GetAccessibilityWindowConnection(parameter.parentWindowId);
    if (isApp && parentConnection) {
        sptr<IAccessibilityElementOperator> parentAamsOper =
            parentConnection->GetCardProxy(parameter.parentTreeId);
        if (parentAamsOper != nullptr) {
            parentAamsOper->SetChildTreeIdAndWinId(parameter.elementId, treeId, parameter.windowId);
        } else {
            HILOG_DEBUG("parentAamsOper is nullptr");
        }
 
        auto cardOper = parentConnection->GetCardProxy(treeId);
        if (cardOper == nullptr) {
            parentConnection->SetCardProxy(treeId, elementOperator);
            parentConnection->SetTokenIdMap(treeId, tokenId);
            parentConnection->SetRootParentId(treeId, parameter.elementId);
        }
    } else {
        return RET_ERR_NO_CONNECTION;
    }
 
    elementOperator->SetBelongTreeId(treeId);
    elementOperator->SetParentWindowId(parameter.parentWindowId);
    sptr<AccessibilityWindowConnection> oldConnection = GetAccessibilityWindowConnection(parameter.windowId);
    if (parameter.parentWindowId != parameter.windowId && isApp && oldConnection) {
        if (oldConnection->GetCardProxy(treeId) != nullptr) {
            HILOG_WARN("no need to register again.");
            return RET_ERR_REGISTER_EXIST;
        } else {
            oldConnection->SetCardProxy(treeId, elementOperator);
            oldConnection->SetTokenIdMap(treeId, tokenId);
            oldConnection->SetRootParentId(treeId, parameter.elementId);
        }
        oldConnection->AddTreeDeathRecipient(parameter.windowId, treeId, accountId_, displayId);
    }
 
    return RET_OK;
}

RetError ElementOperatorManager::DeregisterElementOperatorByWindowId(int32_t windowId, uint64_t displayId, bool isBroker)
{
    HILOG_INFO("DeregisterElementOperatorByWindowId %{public}d %{public}llu", windowId, displayId);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
        return RET_ERR_FAILED;
    }
    if (isBroker && connection->IsAnco()) {
        connection->ResetBrokerProxy();
        return RET_ERR_FAILED;
    }
    StopCallbackWait(windowId);
    connection->ResetProxy();
    connection->ResetBrokerProxy();
    std::vector<int32_t> treeIds {};
    connection->GetAllTreeId(treeIds);
    for (int32_t treeId : treeIds) {
        RecycleTreeId(treeId);
        StopCallbackWait(windowId, treeId);
        RemoveTreeDeathRecipient(windowId, treeId, connection);
    }
    RemoveAccessibilityWindowConnection(windowId);
    return RET_OK;
}

RetError ElementOperatorManager::DeregisterElementOperatorByWindowIdAndTreeId(
    int32_t windowId, int32_t treeId, uint64_t displayId)
{
    RecycleTreeId(treeId);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection == nullptr) {
        HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
        return RET_ERR_FAILED;
    }
    StopCallbackWait(windowId, treeId);
    RemoveTreeDeathRecipient(windowId, treeId, connection);
    // remove connection when all proxy and children tree proxy deregistered
    if (connection->GetProxy(displayId) == nullptr && connection->GetCardProxySize() == 0) {
        RemoveAccessibilityWindowConnection(windowId);
    }
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        return RET_ERR_NULLPTR;
    }
    accountData->GetWindowManager().RemoveTreeIdWindowIdPair(treeId);
    return RET_OK;
}
 
void ElementOperatorManager::AddRequestId(
    int32_t windowId, int32_t treeId, int32_t requestId, sptr<IAccessibilityElementOperatorCallback> callback)
{
    std::lock_guard<ffrt::mutex> lock(requestIdmutex_);
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

void ElementOperatorManager::RemoveRequestId(int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(requestIdmutex_);
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
}

void ElementOperatorManager::StopCallbackWait(int32_t windowId)
{
    HILOG_INFO("StopCallbackWait start windowId: %{public}d", windowId);
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
    std::lock_guard<ffrt::mutex> lock(requestIdmutex_);
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

bool ElementOperatorManager::GetParentElementRecursively(
    int32_t windowId, int64_t elementId, std::vector<AccessibilityElementInfo> &infos)
{
    int32_t treeId = 0;
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
        return false;
    }
 
    if (elementId > 0) {
        treeId = Utils::GetTreeIdBySplitElementId(elementId);
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy(0);
    }
    if (elementOperator == nullptr) {
        HILOG_DEBUG("elementOperator failed elementId: %{public}" PRId64 " winId: %{public}d treeId: %{public}d",
            elementId, windowId, treeId);
        return false;
    }
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl(accountId_);
    if (callBack == nullptr) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }
 
    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    int32_t requestId = GenerateRequestId();
    AddRequestId(windowId, treeId, requestId, callBack);
    elementOperator->SearchElementInfoByAccessibilityId(elementId, requestId, callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        return false;
    }
 
    for (auto& info : callBack->elementInfosResult_) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfoByAccessibilityId elementInfo from ace is wrong");
            return false;
        }
    }
 
    infos = callBack->elementInfosResult_;
    HILOG_DEBUG("Get parent element success, size %{public}zu", infos.size());
    return true;
}

RetError ElementOperatorManager::VerifyingToKenId(const int32_t windowId, const int64_t elementId, uint32_t tokenId)
{
    int32_t treeId = Utils::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("VerifyingToKenId: treeId[%{public}d], windowId[%{public}d], elementId[%{public}" PRId64 "]",
        treeId, windowId, elementId);
    if (elementId == ELEMENT_ID_INVALID || windowId == WINDOW_ID_INVALID) {
        HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
        return RET_OK;
    }
    uint64_t displayId = 0;
    sptr<AccessibilityWindowConnection> connection = GetRealIdWindowConnection(windowId, FOCUS_TYPE_INVALID, displayId);
    HILOG_DEBUG("treeId %{public}d, windowId %{public}d displayId %{public}llu", treeId, windowId, displayId);
    if (connection == nullptr) {
        HILOG_ERROR("connection is empty.");
        return RET_ERR_REGISTER_EXIST;
    }
    if (connection->IsAnco()) {
        return RET_OK;
    }
    if (windowId == SCENE_BOARD_WINDOW_ID) {
        return connection->CheckScbTokenIdMap(tokenId) ? RET_OK : RET_ERR_TOKEN_ID;
    }
    uint32_t expectTokenId = connection->GetTokenIdMap(treeId);
    if (tokenId != expectTokenId) {
        HILOG_ERROR("tokenId error!");
        return RET_ERR_TOKEN_ID;
    }
    return RET_OK;
}

bool ElementOperatorManager::InnerGetElementOperator(
    int32_t windowId, int64_t elementId, sptr<IAccessibilityElementOperator> &elementOperator)
{
    uint64_t displayId = 0;
    sptr<AccessibilityWindowConnection> connection = GetRealIdWindowConnection(windowId, FOCUS_TYPE_INVALID, displayId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
    RETURN_FALSE_IF_NULL(connection);
    int32_t treeId = 0;
    if (elementId > 0) {
        treeId = Utils::GetTreeIdBySplitElementId(elementId);
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy(displayId);
    }
    RETURN_FALSE_IF_NULL(elementOperator);
    return true;
}
 
void ElementOperatorManager::OnFocusedEvent(const AccessibilityEventInfo &eventInfo)
{
    if (!GetMagnificationState()) {
        return;
    }
    if (eventInfo.GetEventType() != TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT) {
        return;
    }
    Rect rect = eventInfo.GetElementInfo().GetRectInScreen();
    int32_t centerX = static_cast<int32_t>((rect.GetLeftTopXScreenPostion() +
        rect.GetRightBottomXScreenPostion()) / static_cast<float>(DIVISOR_TWO));
    int32_t centerY = static_cast<int32_t>((rect.GetLeftTopYScreenPostion() +
        rect.GetRightBottomYScreenPostion()) / static_cast<float>(DIVISOR_TWO));
 
    if (Singleton<ExtendManagerServiceProxy>::GetInstance().CheckExtProxyStatus()) {
        if (Singleton<ExtendManagerServiceProxy>::GetInstance().LoadExtProxy()) {
            Singleton<ExtendManagerServiceProxy>::GetInstance().FollowFocuseElement(centerX, centerY);
        }
    }
}

bool ElementOperatorManager::GetMagnificationState()
{
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        return false;
    }
    if (accountData->GetConfig()) {
        return accountData->GetConfig()->GetScreenMagnificationState();
    }
    return false;
}
 
bool ElementOperatorManager::InvalidHoverEnterEvent(AccessibilityEventInfo &event)
{
    if (event.GetEventType() != TYPE_VIEW_HOVER_ENTER_EVENT) {
        return false;
    }
    std::string readableRules;
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData || accountData->GetAccessibleAbilityManager().GetReadableRules(readableRules) != RET_OK ||
        readableRules.empty()) {
        HILOG_INFO("no readablerules");
        return false;
    }
    auto& originElementInfo = event.GetElementInfo();
    int32_t treeId = originElementInfo.GetBelongTreeId();
    if (treeId <= 0) {
        return false;
    }
    auto windowId = event.GetWindowId();
    int64_t parentId = -1;
    GetRootParentId(windowId, treeId, parentId);
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    if (!InnerGetElementOperator(windowId, parentId, elementOperator)) {
        return false;
    }
 
    AccessibilityFocusMoveParam param = {
        .direction = FocusMoveDirection::DETECT_FOCUSABLE_IN_HOVER,
        .condition = DetailCondition::BYPASS_SELF,
        .parentId = parentId,
        .detectParent = true,
    };
    int32_t requestId = GenerateRequestId();
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl(accountId_);
    if (callBack == nullptr) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }
    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    elementOperator->FocusMoveSearchWithCondition(originElementInfo, param, requestId, callBack);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait result, requestId: %{public}d", requestId);
        return false;
    }
    if (callBack->elementInfosResult_.size() <= 0) {
        HILOG_INFO("result size is 0");
        return false;
    }
    if (callBack->focusMoveResult_ == FocusMoveResultType::SEARCH_SUCCESS) {
        if (callBack->changeToNewInfo_ && callBack->elementInfosResult_.size() > 0) {
            auto newElement = callBack->elementInfosResult_[0];
            event.SetElementInfo(newElement);
            event.SetSource(newElement.GetAccessibilityId());
            return false;
        }
    } else {
        HILOG_INFO("hover enter event is invilid");
        return true;
    }
    return false;
}

void ElementOperatorManager::FindInnerWindowId(const AccessibilityEventInfo &event, int32_t& windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        return;
    }
    auto mapTable = accountData->GetWindowManager().sceneBoardElementIdMap_.GetAllPairs();
    int64_t elementId = event.GetAccessibilityId();
    int tmpWindowId = accountData->GetWindowManager().FindTreeIdWindowIdPair(Utils::GetTreeIdBySplitElementId(elementId));
    if (tmpWindowId != 0) {
        windowId = tmpWindowId;
        return;
    }
    while (1) {
        auto iter = std::find_if(mapTable.begin(), mapTable.end(),
            [elementId] (const std::pair<int32_t, int64_t>& p) {
                return elementId == p.second;
            });
        if (iter != mapTable.end()) {
            windowId = iter->first;
            break;
        }
        if (event.GetWindowId() == 1 && elementId == 0) {
            HILOG_INFO("parent elementId is 0");
            break;
        }
 
        int32_t treeId = Utils::GetTreeIdBySplitElementId(elementId);
        // handle seprately because event send by UiExtension children tree may carry the root elemnt of children
        // tree, whose componentType is also root
        // deal other eventType like this may lead to performance problem
        if (treeId != 0) {
            // WindowScene
            //       \
            // UiExtensionComponent -> try to find the windowId of the event send by its children node
            //       \
            //       root -> node that send event, and it's a UiExtensionNode
            // when elementId is element that at the UiExtension tree, try to get the id of UiExtensionComponent
            // by GetRootParentId,
            ErrCode ret = GetRootParentId(windowId, treeId, elementId);
            if (ret != ERR_OK) {
                break;
            }
        } else {
            // keep find its parent node, until it's a root node or find its elementId in sceneBoardElementIdMap_
            // which saves mapping of windowId&root-elementId of the window.
            std::vector<AccessibilityElementInfo> infos = {};
            if (GetParentElementRecursively(event.GetWindowId(), elementId, infos) == false || infos.size() == 0) {
                HILOG_ERROR("find parent element failed");
                break;
            }
 
            if (infos[0].GetComponentType() == "root") {
                HILOG_ERROR("can not find parent element, has reach root node");
                break;
            }
 
            elementId = infos[0].GetParentNodeId();
        }
    }
    int originTreeId = Utils::GetTreeIdBySplitElementId(event.GetAccessibilityId());
    accountData->GetWindowManager().InsertTreeIdWindowIdPair(originTreeId, windowId);
}

void ElementOperatorManager::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("windowId is %{public}d", event.GetWindowId());
    int32_t windowId = event.GetWindowId();
    bool scbWindowFlag = false;
    if (windowId == 1) {
        scbWindowFlag = true;
        int32_t innerWindowId = event.GetElementInfo().GetInnerWindowId();
        if (innerWindowId > 0) {
            windowId = innerWindowId;
        } else {
            FindInnerWindowId(event, windowId);
        }
    }
 
    const_cast<AccessibilityEventInfo&>(event).SetElementMainWindowId(windowId);
 
    if (windowId < 1) {
        HILOG_INFO("windowId is invilid : %{public}d", windowId);
        return;
    }
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        return;
    }
    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
            if (accountData->GetWindowManager().NeedSetActive(windowId)) {
                accountData->GetWindowManager().SetActiveWindow(windowId, false);
            }
            accountData->GetWindowManager().SetAccessibilityFocusedWindow(windowId);
            if (scbWindowFlag) {
                HILOG_INFO("windowId set to subWindows_ is %{public}d", windowId);
                accountData->GetWindowManager().subWindows_.insert({windowId, 0});
            }
            break;
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
            accountData->GetWindowManager().SetAccessibilityFocusedWindow(windowId);
            break;
        default:
            break;
    }
}

RetError ElementOperatorManager::SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t flag, uint32_t tokenId)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d] windowId[%{public}d] compnentId: %{public}" PRId64 " "
        "elementId: %{public}" PRId64 " winId: %{public}d innerWinId: %{public}d treeId: %{public}d",
        uiEvent.GetEventType(), uiEvent.GetGestureType(), uiEvent.GetWindowId(), uiEvent.GetAccessibilityId(),
        uiEvent.GetElementInfo().GetAccessibilityId(), uiEvent.GetElementInfo().GetWindowId(),
        uiEvent.GetElementInfo().GetInnerWindowId(), uiEvent.GetElementInfo().GetBelongTreeId());
    EventType eventType = uiEvent.GetEventType();
    if (eventType == TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY && uiEvent.GetBundleName() == "com.ohos.shell_assistant") {
        sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(uiEvent.GetPageId());
        if (connection && uiEvent.GetTextAnnouncedForAccessibility() == "UIExtension begin") {
            connection->SetUseBrokerFlag(false);
        } else if (connection && uiEvent.GetTextAnnouncedForAccessibility() == "UIExtension end") {
            connection->SetUseBrokerFlag(true);
        }
        return RET_OK;
    }
    RetError res = Utils::GetResourceBundleInfo(const_cast<AccessibilityEventInfo&>(uiEvent), accountId_);
    if (res != RET_OK) {
        HILOG_ERROR("Get Resource BundleInfo failed! RetError is %{public}d", res);
    }
    std::string isAncoFlag = uiEvent.GetExtraEvent().GetExtraEventInfoValueByKey("isAnco");
    if (flag && isAncoFlag != "true" && VerifyingToKenId(uiEvent.GetElementInfo().GetWindowId(),
        uiEvent.GetElementInfo().GetAccessibilityId(), tokenId) != RET_OK) {
        HILOG_ERROR("VerifyingToKenId failed");
        return RET_ERR_TOKEN_ID;
    }
    if (isAncoFlag != "true" && InvalidHoverEnterEvent(const_cast<AccessibilityEventInfo&>(uiEvent))) {
        HILOG_ERROR("CheckNodeIsReadableOverChildTree failed");
        return RET_ERR_INVALID_PARAM;
    }
    OnFocusedEvent(uiEvent);
    UpdateAccessibilityWindowStateByEvent(uiEvent);
    const_cast<AccessibilityEventInfo&>(uiEvent).SetTimeStamp(Utils::GetSystemTime());
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (accountData) {
        accountData->isSendEvent(uiEvent);
    }
    return RET_OK;
}

bool ElementOperatorManager::FindFocusedElementByConnection(sptr<AccessibilityWindowConnection> connection,
    AccessibilityElementInfo &elementInfo, uint64_t displayId)
{
    HILOG_DEBUG();
    int64_t elementId = -1;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    if (!connection || !connection->GetProxy(displayId)) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
        return false;
    }
 
    sptr<ElementOperatorCallbackImpl> focusCallback = new(std::nothrow) ElementOperatorCallbackImpl(accountId_);
    if (!focusCallback) {
        HILOG_ERROR("Failed to create focusCallback.");
        return false;
    }
    ffrt::future<void> focusFuture = focusCallback->promise_.get_future();
    connection->GetProxy(displayId)->FindFocusedElementInfo(elementId, focusType, GenerateRequestId(), focusCallback);
    ffrt::future_status waitFocus = focusFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        HILOG_ERROR("FindFocusedElementInfo Failed to wait result");
        return false;
    }
    elementInfo = focusCallback->accessibilityInfoResult_;
    return true;
}

bool ElementOperatorManager::FindFocusedElement(AccessibilityElementInfo &elementInfo, uint32_t timeout)
{
    HILOG_DEBUG();
    if (timeout > TIME_OUT_OPERATOR) {
        HILOG_ERROR("invalid timeout value!");
        return false;
    }
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    uint64_t displayId = 0;
    sptr<AccessibilityWindowConnection> connection = GetRealIdWindowConnection(windowId, focusType, displayId);
    FindFocusedElementByConnection(connection, elementInfo, displayId);
    if (elementInfo.GetAccessibilityId() >= 0) {
        HILOG_DEBUG("find focused element success.");
        return true;
    }
    windowId = focusWindowId_.load();
    int64_t elementId = focusElementId_.load();
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    connection = GetRealIdWindowConnection(windowId, FOCUS_TYPE_INVALID, displayId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
    RETURN_FALSE_IF_NULL(connection);
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl(accountId_);
    RETURN_FALSE_IF_NULL(callBack);
    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    GetElementOperatorConnection(connection, elementId, elementOperator, displayId);
    RETURN_FALSE_IF_NULL(elementOperator);
    elementOperator->SearchElementInfoByAccessibilityId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(timeout));
    if (waitFocus != ffrt::future_status::ready) {
        return false;
    }
 
    if (callBack->elementInfosResult_.size() <= 0) {
        HILOG_ERROR("SearchElementInfoByAccessibilityId return null");
        return false;
    }
    elementInfo = callBack->elementInfosResult_[0];
    return true;
}

void ElementOperatorManager::CalculateClickPosition(const AccessibilityElementInfo &focusedElementInfo,
    int32_t &xPos, int32_t &yPos)
{
    Rect focusElement = focusedElementInfo.GetRectInScreen();
    int32_t displayWidth = 0;
    int32_t displayHeight = 0;
    if (Singleton<ExtendManagerServiceProxy>::GetInstance().CheckExtProxyStatus()) {
        Singleton<ExtendManagerServiceProxy>::GetInstance().GetClickPosition(xPos, yPos);
    }
    int32_t focusLeftTopXPos = focusElement.GetLeftTopXScreenPostion();
    int32_t focusRightBottomXPos = focusElement.GetRightBottomXScreenPostion();
    int32_t focusLeftTopYpos = focusElement.GetLeftTopYScreenPostion();
    int32_t focusRightBottomYPos = focusElement.GetRightBottomYScreenPostion();
 
    int32_t leftTopXPos = focusLeftTopXPos > 0 ? focusLeftTopXPos : 0;
    int32_t leftTopYPos = focusLeftTopYpos > 0 ? focusLeftTopYpos : 0;
    int32_t rightBottomXPos = displayWidth > 0 && displayWidth < focusRightBottomXPos ?
        displayWidth : focusRightBottomXPos;
    int32_t rightBottomYPos = displayHeight > 0 && displayHeight < focusRightBottomYPos ?
        displayHeight : focusRightBottomYPos;
    xPos = leftTopXPos + (rightBottomXPos - leftTopXPos) / DIVISOR_TWO;
    yPos = leftTopYPos + (rightBottomYPos - leftTopYPos) / DIVISOR_TWO;
}

void ElementOperatorManager::GetElementOperatorConnection(const sptr<AccessibilityWindowConnection> &connection,
    int64_t elementId, sptr<IAccessibilityElementOperator> &elementOperator, uint64_t displayId)
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
            elementOperator = connection->GetProxy(displayId);
        }
    } else {
        elementOperator = connection->GetProxy(displayId);
    }
    HILOG_DEBUG("elementId:%{public}" PRId64 " treeId:%{public}d", elementId, treeId);
}

bool ElementOperatorManager::ExecuteActionOnAccessibilityFocused(ActionType action)
{
    int32_t windowId = focusWindowId_.load();
    int64_t elementId = focusElementId_.load();
    int32_t treeId = Utils::GetTreeIdBySplitElementId(elementId);
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    uint64_t displayId = 0;
    sptr<AccessibilityWindowConnection> connection = GetRealIdWindowConnection(windowId, FOCUS_TYPE_INVALID, displayId);
    if (connection == nullptr) {
        HILOG_ERROR("connection is nullptr");
        return false;
    }
    bool isAnco = connection->IsAnco();
    if (isAnco && (action == ActionType::ACCESSIBILITY_ACTION_CLICK)) {
        accountData->GetWindowManager().SetAccessibilityFocusedWindow(windowId);
    }
    std::map<std::string, std::string> actionArguments {};
    AccessibilityElementInfo focusedElementInfo {};
 
    bool ret = FindFocusedElement(focusedElementInfo, TIME_OUT_OPERATOR);
    if (ret) {
        actionArguments = AccessibilitySecurityComponentManager::GenerateActionArgumentsWithHMAC(
            action, focusedElementInfo.GetUniqueId(), focusedElementInfo.GetBundleName(), actionArguments);
    }
 
    sptr<ElementOperatorCallbackImpl> actionCallback = new(std::nothrow) ElementOperatorCallbackImpl(accountId_);
    if (actionCallback == nullptr) {
        HILOG_ERROR("Failed to create actionCallback.");
        return false;
    }
    ffrt::future<void> actionFuture = actionCallback->promise_.get_future();
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    GetElementOperatorConnection(connection, elementId, elementOperator, displayId);
    RETURN_FALSE_IF_NULL(elementOperator);
    elementOperator->ExecuteAction(elementId, action, actionArguments, GenerateRequestId(), actionCallback);
    ffrt::future_status waitAction = actionFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitAction != ffrt::future_status::ready) {
        HILOG_ERROR("ExecuteAction Failed to wait result");
        return false;
    }
    HILOG_INFO("windowId[%{public}d], elementId[%{public}" PRId64 "], action[%{public}d, result: %{public}d",
        windowId, elementId, action, actionCallback->executeActionResult_);
 
    if (!actionCallback->executeActionResult_ && (action == ActionType::ACCESSIBILITY_ACTION_CLICK)) {
        int32_t xPos = 0;
        int32_t yPos = 0;
        CalculateClickPosition(focusedElementInfo, xPos, yPos);
        std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
        pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
        pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
        pointerEvent->AddFlag(MMI::InputEvent::EVENT_FLAG_ACCESSIBILITY);
        pointerEvent->AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
        MMI::PointerEvent::PointerItem item;
        item.SetDisplayX(xPos);
        item.SetDisplayY(yPos);
        item.SetRawDisplayX(xPos);
        item.SetRawDisplayY(yPos);
        item.SetPointerId(1);
        pointerEvent->AddPointerItem(item);
        pointerEvent->SetPointerId(1);
        MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
 
        pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
        MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
        return true;
    }
    return actionCallback->executeActionResult_;
}

void ElementOperatorManager::SetFocusWindowId(const int32_t focusWindowId)
{
    focusWindowId_ = focusWindowId;
}
 
void ElementOperatorManager::SetFocusElementId(const int64_t focusElementId)
{
    focusElementId_ = focusElementId;
}
 
sptr<AccessibilityWindowConnection> ElementOperatorManager::GetRealIdWindowConnection(int32_t windowId, int32_t focusType, uint64_t &displayId)
{
    sptr<AccessibilityAccountData> accountData = accountData_.promote();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }
    auto [realId, _displayId] = accountData->GetWindowManager().ConvertToRealWindowId(windowId, focusType);
    displayId = _displayId;
    return GetAccessibilityWindowConnection(realId);
}
} // namespace Accessibility
} // namespace OHOS