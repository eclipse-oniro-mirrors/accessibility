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

#include <cinttypes>
#include "accessibility_system_ability_client_impl.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t REQUEST_WINDOW_ID_MASK_BIT = 16;
    constexpr int32_t SA_CONNECT_TIMEOUT = 500; // ms
} // namespaces

static std::mutex g_Mutex;
static std::shared_ptr<AccessibilitySystemAbilityClientImpl> g_Instance = nullptr;
std::shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        g_Instance = std::make_shared<AccessibilitySystemAbilityClientImpl>();
    } else {
        HILOG_DEBUG("AccessibilitySystemAbilityClient had construct instance");
    }

    return g_Instance;
}

AccessibilitySystemAbilityClientImpl::AccessibilitySystemAbilityClientImpl()
{
    HILOG_DEBUG();

    stateArray_.fill(false);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return;
    }
    sptr<AccessibilitySaStatusChange> statusChange = new AccessibilitySaStatusChange();
    int32_t ret = samgr->SubscribeSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, statusChange);
    if (ret != 0) {
        HILOG_ERROR("subscribe accessibility failed, error = %{public}d", ret);
    }
    if (ConnectToService()) {
        Init();
    }
}

AccessibilitySystemAbilityClientImpl::~AccessibilitySystemAbilityClientImpl()
{
    HILOG_DEBUG();
}

bool AccessibilitySystemAbilityClientImpl::ConnectToService()
{
    HILOG_DEBUG();

    if (serviceProxy_) {
        HILOG_DEBUG("AAMS Service is connected");
        return true;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (object == nullptr && LoadAccessibilityService() == false) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return false;
    }

    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
        if (!deathRecipient_) {
            HILOG_ERROR("Failed to create deathRecipient.");
            return false;
        }
    }

    if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
        HILOG_ERROR("Failed to add death recipient");
    }

    HILOG_DEBUG("Get remote object ok");
    serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }
    return true;
}

bool AccessibilitySystemAbilityClientImpl::LoadAccessibilityService()
{
    std::unique_lock<std::mutex> lock(conVarMutex_);
    sptr<AccessibilityLoadCallback> loadCallback = new AccessibilityLoadCallback();
    if (loadCallback == nullptr) {
        return false;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return false;
    }
    int32_t ret = samgr->LoadSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, loadCallback);
    if (ret != 0) {
        return false;
    }
    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(SA_CONNECT_TIMEOUT),
        [=]() { return serviceProxy_ != nullptr; });
    if (!waitStatus) {
        return false;
    }
    return true;
}

void AccessibilitySystemAbilityClientImpl::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::mutex> lock(conVarMutex_);
    if (remoteObject != nullptr) {
        serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(remoteObject);
    } else {
        serviceProxy_ = nullptr;
    }
    proxyConVar_.notify_one();
}

void AccessibilitySystemAbilityClientImpl::LoadSystemAbilityFail()
{
    std::lock_guard<std::mutex> lock(conVarMutex_);
    serviceProxy_ = nullptr;
    proxyConVar_.notify_one();
}

void AccessibilitySystemAbilityClientImpl::Init()
{
    HILOG_DEBUG();
    stateArray_.fill(false);
    if (!stateObserver_) {
        stateObserver_ = new(std::nothrow) AccessibleAbilityManagerStateObserverImpl(*this);
        if (!stateObserver_) {
            HILOG_ERROR("Failed to create stateObserver.");
            return;
        }
    }
    if (serviceProxy_ == nullptr) {
        return;
    }
    uint32_t stateType = serviceProxy_->RegisterStateObserver(stateObserver_);
    if (stateType & STATE_ACCESSIBILITY_ENABLED) {
        stateArray_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED] = true;
    }
    if (stateType & STATE_EXPLORATION_ENABLED) {
        stateArray_[AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED] = true;
    }
    if (stateType & STATE_KEYEVENT_ENABLED) {
        stateArray_[AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED] = true;
    }
    if (stateType & STATE_GESTURE_ENABLED) {
        stateArray_[AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED] = true;
    }
}

void AccessibilitySystemAbilityClientImpl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (serviceProxy_) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            HILOG_DEBUG("Reset OK");
        }
    }
}

RetError AccessibilitySystemAbilityClientImpl::RegisterElementOperator(
    const int32_t windowId, const std::shared_ptr<AccessibilityElementOperator> &operation)
{
    HILOG_INFO("Register windowId[%{public}d] start", windowId);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!operation) {
        HILOG_ERROR("Input operation is null");
        return RET_ERR_INVALID_PARAM;
    }
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    auto iter = elementOperators_.find(windowId);
    if (iter != elementOperators_.end()) {
        HILOG_ERROR("windowID[%{public}d] is exited", windowId);
        return RET_OK;
    }

    sptr<AccessibilityElementOperatorImpl> aamsInteractionOperator =
        new(std::nothrow) AccessibilityElementOperatorImpl(windowId, operation, *this);
    if (!aamsInteractionOperator) {
        HILOG_ERROR("Failed to create aamsInteractionOperator.");
        return RET_ERR_NULLPTR;
    }
    elementOperators_[windowId] = aamsInteractionOperator;
    return serviceProxy_->RegisterElementOperator(windowId, aamsInteractionOperator);
}

void AccessibilitySystemAbilityClientImpl::ReregisterElementOperator()
{
    HILOG_DEBUG();

    if (!serviceProxy_) {
        HILOG_ERROR("serviceProxy_ is null.");
        return;
    }
    for (auto iter = elementOperators_.begin(); iter != elementOperators_.end(); iter++) {
        serviceProxy_->RegisterElementOperator(iter->first, iter->second);
    }
}

RetError AccessibilitySystemAbilityClientImpl::DeregisterElementOperator(const int32_t windowId)
{
    HILOG_INFO("Deregister windowId[%{public}d] start", windowId);
    std::lock_guard<std::mutex> lock(mutex_);

    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    auto iter = elementOperators_.find(windowId);
    if (iter != elementOperators_.end()) {
        HILOG_DEBUG("windowID[%{public}d] is erase", windowId);
        elementOperators_.erase(iter);
    } else {
        HILOG_WARN("Not find windowID[%{public}d]", windowId);
        return RET_ERR_NO_REGISTER;
    }
    return serviceProxy_->DeregisterElementOperator(windowId);
}

RetError AccessibilitySystemAbilityClientImpl::IsEnabled(bool &isEnabled)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    isEnabled = stateArray_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED];
    return RET_OK;
}

RetError AccessibilitySystemAbilityClientImpl::IsTouchExplorationEnabled(bool &isEnabled)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    isEnabled = stateArray_[AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED];
    return RET_OK;
}

RetError AccessibilitySystemAbilityClientImpl::GetAbilityList(const uint32_t accessibilityAbilityTypes,
    const AbilityStateType stateType, std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    bool check = false;
    if ((accessibilityAbilityTypes & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN) ||
        (accessibilityAbilityTypes & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC) ||
        (accessibilityAbilityTypes & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE) ||
        (accessibilityAbilityTypes & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL) ||
        (accessibilityAbilityTypes & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC)) {
        check = true;
    }
    if (stateType == ABILITY_STATE_INVALID) {
        check = false;
    }
    if (!check) {
        HILOG_ERROR("Invalid params: accessibilityAbilityTypes[%{public}d] stateType[%{public}d]",
            accessibilityAbilityTypes, stateType);
        return RET_ERR_INVALID_PARAM;
    }
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->GetAbilityList(accessibilityAbilityTypes, stateType, infos);
}

bool AccessibilitySystemAbilityClientImpl::CheckEventType(EventType eventType)
{
    if ((eventType < EventType::TYPE_VIEW_CLICKED_EVENT) ||
        ((eventType >= EventType::TYPE_MAX_NUM) && (eventType != EventType::TYPES_ALL_MASK))) {
        HILOG_ERROR("event type is invalid");
        return false;
    } else {
        return true;
    }
}

RetError AccessibilitySystemAbilityClientImpl::SendEvent(const EventType eventType, const int64_t componentId)
{
    HILOG_DEBUG("componentId[%{public}" PRId64 "], eventType[%{public}d]", componentId, eventType);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckEventType(eventType)) {
        return RET_ERR_INVALID_PARAM;
    }
    AccessibilityEventInfo event;
    event.SetEventType(eventType);
    event.SetSource(componentId);
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->SendEvent(event);
}

RetError AccessibilitySystemAbilityClientImpl::SendEvent(const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("EventType[%{public}d]", event.GetEventType());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckEventType(event.GetEventType())) {
        return RET_ERR_INVALID_PARAM;
    }
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->SendEvent(event);
}

RetError AccessibilitySystemAbilityClientImpl::SubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (eventType >= AccessibilityStateEventType::EVENT_TYPE_MAX) {
        HILOG_ERROR("Input eventType is out of scope");
        return RET_ERR_INVALID_PARAM;
    }
    if (!observer) {
        HILOG_ERROR("Input observer is null");
        return RET_ERR_INVALID_PARAM;
    }

    StateObserverVector &observerVector = stateObserversArray_[eventType];
    for (auto iter = observerVector.begin(); iter != observerVector.end(); ++iter) {
        if (*iter == observer) {
            HILOG_INFO("Observer has subscribed!");
            return RET_ERR_REGISTER_EXIST;
        }
    }
    observerVector.push_back(observer);
    return RET_OK;
}

RetError AccessibilitySystemAbilityClientImpl::UnsubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG("eventType is [%{public}d]", eventType);
    std::lock_guard<std::mutex> lock(mutex_);
    if (eventType >= AccessibilityStateEventType::EVENT_TYPE_MAX) {
        HILOG_ERROR("Input eventType is out of scope");
        return RET_ERR_INVALID_PARAM;
    }
    if (!observer) {
        HILOG_ERROR("Input observer is null");
        return RET_ERR_INVALID_PARAM;
    }

    StateObserverVector &observerVector = stateObserversArray_[eventType];
    for (auto iter = observerVector.begin(); iter != observerVector.end(); ++iter) {
        if (*iter == observer) {
            observerVector.erase(iter);
            return RET_OK;
        }
    }
    HILOG_ERROR("The observer has not subscribed.");
    return RET_ERR_NO_REGISTER;
}

void AccessibilitySystemAbilityClientImpl::NotifyStateChanged(uint32_t eventType, bool value)
{
    HILOG_DEBUG("EventType is %{public}d, value is %{public}d", eventType, value);
    if (eventType >= AccessibilityStateEventType::EVENT_TYPE_MAX) {
        HILOG_ERROR("EventType is invalid");
        return;
    }

    if (stateArray_[eventType] == value) {
        HILOG_DEBUG("State value is not changed");
        return;
    }

    stateArray_[eventType] = value;
    StateObserverVector &observers = stateObserversArray_[eventType];
    for (auto &observer : observers) {
        if (observer) {
            observer->OnStateChanged(value);
        } else {
            HILOG_ERROR("end stateObserversArray[%{public}d] is null", eventType);
        }
    }
    HILOG_DEBUG("end");
}

RetError AccessibilitySystemAbilityClientImpl::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->GetEnabledAbilities(enabledAbilities);
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType[%{public}d}", stateType);
    SetAccessibilityState(stateType);
    std::lock_guard<std::mutex> lock(mutex_);
    if (stateType & STATE_ACCESSIBILITY_ENABLED) {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED, true);
    } else {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED, false);
    }

    if (stateType & STATE_EXPLORATION_ENABLED) {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED, true);
    } else {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED, false);
    }

    if (stateType & STATE_KEYEVENT_ENABLED) {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED, true);
    } else {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED, false);
    }

    if (stateType & STATE_GESTURE_ENABLED) {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED, true);
    } else {
        NotifyStateChanged(AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED, false);
    }
}

void AccessibilitySystemAbilityClientImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetSearchElementInfoByTextResult(infos, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetFindFocusedElementInfoResult(info, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetFocusMoveSearchResult(info, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetExecuteActionResult(succeeded, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetCursorPositionResult(
    const int32_t cursorPosition, const int32_t requestId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]  cursorPosition[%{public}d]", requestId, cursorPosition);
    if (requestId >= 0) {
        auto iter = elementOperators_.find(static_cast<uint32_t>(requestId) >> REQUEST_WINDOW_ID_MASK_BIT);
        if (iter != elementOperators_.end()) {
            if (iter->second) {
                iter->second->SetCursorPositionResult(cursorPosition, requestId);
            }
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetAccessibilityState(const uint32_t stateType)
{
    HILOG_DEBUG();
    state_ = stateType;
}

uint32_t AccessibilitySystemAbilityClientImpl::GetAccessibilityState()
{
    HILOG_DEBUG();
    return state_;
}

void AccessibilitySystemAbilityClientImpl::SetFindAccessibilityNodeInfosResult(
    const std::list<AccessibilityElementInfo> elementInfos, const int32_t requestId, const int32_t requestCode)
{
    HILOG_DEBUG();
    switch (static_cast<SET_AA_CALLBACK_RESULT>(requestCode)) {
        case FIND_ACCESSIBILITY_NODE_BY_ACCESSIBILITY_ID:
            SetSearchElementInfoByAccessibilityIdResult(elementInfos, requestId);
            break;
        case FIND_ACCESSIBILITY_NODE_BY_TEXT:
            SetSearchElementInfoByTextResult(elementInfos, requestId);
            break;
        default:
            break;
    }
}

void AccessibilitySystemAbilityClientImpl::SetFindAccessibilityNodeInfoResult(
    const AccessibilityElementInfo elementInfo, const int32_t requestId, const int32_t requestCode)
{
    HILOG_DEBUG();
    switch (static_cast<SET_AA_CALLBACK_RESULT>(requestCode)) {
        case FIND_ACCESSIBILITY_NODE_BY_ACCESSIBILITY_ID:
            {
                std::list<AccessibilityElementInfo> elementInfos = {};
                elementInfos.push_back(elementInfo);
                SetSearchElementInfoByAccessibilityIdResult(elementInfos, requestId);
            }
            break;
        case FIND_FOCUS:
            SetFindFocusedElementInfoResult(elementInfo, requestId);
            break;
        case FIND_FOCUS_SEARCH:
            SetFocusMoveSearchResult(elementInfo, requestId);
            break;
        default:
            break;
    }
}

void AccessibilitySystemAbilityClientImpl::SetPerformActionResult(const bool succeeded, const int32_t requestId)
{
    HILOG_DEBUG();
    SetExecuteActionResult(succeeded, requestId);
}

RetError AccessibilitySystemAbilityClientImpl::GetFocusedWindowId(int32_t &focusedWindowId)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_ && !LoadAccessibilityService()) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->GetFocusedWindowId(focusedWindowId);
}

void AccessibilitySystemAbilityClientImpl::AccessibilityLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject)
{
    HILOG_DEBUG();
    if (g_Instance) {
        g_Instance->LoadSystemAbilitySuccess(remoteObject);
    }
}

void AccessibilitySystemAbilityClientImpl::AccessibilityLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOG_DEBUG();
    if (g_Instance) {
        g_Instance->LoadSystemAbilityFail();
    }
}

void AccessibilitySystemAbilityClientImpl::AccessibilitySaStatusChange::OnAddSystemAbility(int32_t saId,
    const std::string &deviceId)
{
    HILOG_DEBUG();
    if (!g_Instance) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_Instance->mutex_);
    if (g_Instance->serviceProxy_) {
        return;
    }
    if (!g_Instance->ConnectToService()) {
        return;
    }
    g_Instance->Init();
    g_Instance->ReregisterElementOperator();
}

void AccessibilitySystemAbilityClientImpl::AccessibilitySaStatusChange::OnRemoveSystemAbility(int32_t saId,
    const std::string &deviceId)
{
    HILOG_WARN("disconnect to accessibility service");
}
} // namespace Accessibility
} // namespace OHOS
