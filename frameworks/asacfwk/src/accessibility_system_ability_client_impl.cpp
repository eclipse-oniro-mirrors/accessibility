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

#include <cinttypes>
#include "accessibility_system_ability_client_impl.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "api_reporter_helper.h"
#include "rules/rules_checker.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t CONFIG_PARAMETER_VALUE_SIZE = 10;
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    constexpr int32_t SA_CONNECT_TIMEOUT = 500; // ms
    constexpr int32_t ABILITY_SIZE_MAX = 10000;
} // namespaces

static ffrt::mutex g_Mutex;
static std::shared_ptr<AccessibilitySystemAbilityClientImpl> g_Instance = nullptr;

static bool CheckRulesCheckerIsInit(ReadableRulesChecker& rulesChecker,
    sptr<IAccessibleAbilityManagerService>& serviceProxy)
{
    if (!serviceProxy) {
        return false;
    }
    if (rulesChecker.IsCheckedRules())  {
        return rulesChecker.IsInited();
    }
    std::string readableRules;
    auto result = static_cast<RetError>(serviceProxy->GetReadableRules(readableRules));
    if (result != RET_OK) {
        return false;
    }
    return rulesChecker.CheckInit(readableRules);
}

std::shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(g_Mutex);
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

    stateHandler_.Reset();
    char value[CONFIG_PARAMETER_VALUE_SIZE] = "default";
    int retSysParam = GetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false", value, CONFIG_PARAMETER_VALUE_SIZE);
    if (retSysParam >= 0 && !std::strcmp(value, "true")) {
        HILOG_ERROR("accessibility service is ready");
        if (!ConnectToService()) {
            HILOG_ERROR("Failed to connect to aams service");
            return;
        }
        Init();
    }

    retSysParam = WatchParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), &OnParameterChanged, this);
    if (retSysParam) {
        HILOG_ERROR("Watch parameter failed, error = %{public}d", retSysParam);
    }
}

AccessibilitySystemAbilityClientImpl::~AccessibilitySystemAbilityClientImpl()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (serviceProxy_ != nullptr) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            HILOG_INFO("serviceProxy_ = nullptr");
        }
    }
    if (stateObserver_ != nullptr) {
        stateObserver_->OnClientDeleted();
    }
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
    if (object == nullptr) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return false;
    }

    if (deathRecipient_ == nullptr) {
        deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
        if (deathRecipient_ == nullptr) {
            HILOG_ERROR("Failed to create deathRecipient.");
            return false;
        }
    }
    if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
        HILOG_ERROR("Failed to add death recipient");
    }
    HILOG_DEBUG("Get remote object ok");
    serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(object);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }

    return true;
}

void AccessibilitySystemAbilityClientImpl::OnParameterChanged(const char *key, const char *value, void *context)
{
    HILOG_DEBUG("Parameter key = [%{public}s] value = [%{public}s]", key, value);

    if (!key || std::strcmp(key, SYSTEM_PARAMETER_AAMS_NAME.c_str())) {
        HILOG_WARN("not accessibility.config.ready callback");
        return;
    }

    if (!value || std::strcmp(value, "true")) {
        HILOG_WARN("accessibility.config.ready value not true");
        return;
    }

    if (!context) {
        HILOG_WARN("accessibility.config.ready context NULL");
        return;
    }

    AccessibilitySystemAbilityClientImpl* implPtr = static_cast<AccessibilitySystemAbilityClientImpl*>(context);
    {
        HILOG_DEBUG("ConnectToService start.");
        std::lock_guard<ffrt::mutex> lock(implPtr->mutex_);
        if (implPtr->serviceProxy_) {
            HILOG_DEBUG("service is already started.");
            return;
        }
        if (!implPtr->ConnectToService()) {
            HILOG_ERROR("Failed to connect to aams service");
            return;
        }
        implPtr->Init();
    }
}

bool AccessibilitySystemAbilityClientImpl::LoadAccessibilityService()
{
    std::unique_lock<ffrt::mutex> lock(conVarMutex_);
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
        [this]() { return serviceProxy_ != nullptr; });
    if (!waitStatus) {
        return false;
    }
    return true;
}

void AccessibilitySystemAbilityClientImpl::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<ffrt::mutex> lock(conVarMutex_);
    if (serviceProxy_ != nullptr) {
        HILOG_INFO("serviceProxy_ isn't nullptr");
        proxyConVar_.notify_one();
        return;
    }
    if (remoteObject != nullptr) {
        serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(remoteObject);
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
            if (deathRecipient_ == nullptr) {
                HILOG_ERROR("create deathRecipient_ fail.");
            }
        }
        if (deathRecipient_ && remoteObject->IsProxyObject() && remoteObject->AddDeathRecipient(deathRecipient_)) {
            HILOG_INFO("successed to add death recipient");
        }
    } else {
        HILOG_WARN("remoteObject is nullptr.");
    }
    proxyConVar_.notify_one();
}

void AccessibilitySystemAbilityClientImpl::LoadSystemAbilityFail()
{
    std::lock_guard<ffrt::mutex> lock(conVarMutex_);
    HILOG_WARN("LoadSystemAbilityFail.");
    proxyConVar_.notify_one();
}

void AccessibilitySystemAbilityClientImpl::Init()
{
    HILOG_DEBUG();
    stateHandler_.Reset();
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
    uint32_t stateType = 0;
    serviceProxy_->RegisterStateObserver(stateObserver_, stateType);
    SetAccessibilityState(stateType);
    if (stateType & STATE_ACCESSIBILITY_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED, true);
    }
    if (stateType & STATE_EXPLORATION_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED, true);
    }
    if (stateType & STATE_KEYEVENT_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED, true);
    }
    if (stateType & STATE_GESTURE_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED, true);
    }
    if (stateType & STATE_SCREENREADER_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED, true);
    }
    if (stateType & STATE_SINGLE_CLICK_MODE_ENABLED) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_TOUCH_MODE_CHANGED, true);
    }
    if (stateType & STATE_CONFIG_EVENT_CHANGE) {
        stateHandler_.SetState(AccessibilityStateEventType::EVENT_CONFIG_EVENT_CHANGED, true);
    }
}

void AccessibilitySystemAbilityClientImpl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    {
        // there is lock(mutex_) in OnStateChanged, so make sure no deadlock
        std::lock_guard<ffrt::mutex> lock(mutex_);
        if (serviceProxy_ != nullptr) {
            sptr<IRemoteObject> object = serviceProxy_->AsObject();
            if (object && (remote == object)) {
                object->RemoveDeathRecipient(deathRecipient_);
                serviceProxy_ = nullptr;
                HILOG_INFO("ResetService OK");
            }
        }
    }
    // notify observer when SA died
    OnAccessibleAbilityManagerStateChanged(0);
    stateHandler_.Reset();
}

RetError AccessibilitySystemAbilityClientImpl::RegisterElementOperator(
    const int32_t windowId, const std::shared_ptr<AccessibilityElementOperator> &operation)
{
    HILOG_INFO("Register windowId[%{public}d] start", windowId);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!operation) {
        HILOG_ERROR("Input operation is null");
        return RET_ERR_INVALID_PARAM;
    }
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    auto iter = elementOperators_.find(windowId);
    if (iter != elementOperators_.end()) {
        HILOG_ERROR("windowID[%{public}d] is exited", windowId);
        return RET_ERR_CONNECTION_EXIST;
    }

    sptr<AccessibilityElementOperatorImpl> aamsInteractionOperator =
        new(std::nothrow) AccessibilityElementOperatorImpl(windowId, operation, *this);
    if (aamsInteractionOperator == nullptr) {
        HILOG_ERROR("Failed to create aamsInteractionOperator.");
        return RET_ERR_NULLPTR;
    }
    elementOperators_[windowId] = aamsInteractionOperator;
    return static_cast<RetError>(serviceProxy_->RegisterElementOperatorByWindowId(windowId, aamsInteractionOperator));
}

RetError AccessibilitySystemAbilityClientImpl::RegisterElementOperator(Registration parameter,
    const std::shared_ptr<AccessibilityElementOperator> &operation)
{
    HILOG_DEBUG("parentWindowId:%{public}d, parentTreeId:%{public}d, windowId:%{public}d,nodeId:%{public}" PRId64 "",
        parameter.parentWindowId, parameter.parentTreeId, parameter.windowId, parameter.elementId);

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (parameter.windowId < 0 || parameter.elementId < 0 ||
        parameter.parentTreeId < 0 || parameter.parentWindowId < 0) {
        return RET_ERR_INVALID_PARAM;
    }

    if (!operation) {
        HILOG_ERROR("Input operation is null");
        return RET_ERR_INVALID_PARAM;
    }

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    sptr<AccessibilityElementOperatorImpl> aamsInteractionOperator =
        new(std::nothrow) AccessibilityElementOperatorImpl(parameter.windowId, operation, *this);
    if (aamsInteractionOperator == nullptr) {
        HILOG_ERROR("Failed to create aamsInteractionOperator.");
        return RET_ERR_NULLPTR;
    }
    elementOperators_[parameter.windowId] = aamsInteractionOperator;
    RegistrationPara registrationPara {
        .windowId = parameter.windowId,
        .parentWindowId = parameter.parentWindowId,
        .parentTreeId = parameter.parentTreeId,
        .elementId = parameter.elementId,
    };
    return static_cast<RetError>(serviceProxy_->RegisterElementOperatorByParameter(registrationPara,
        aamsInteractionOperator));
}

void AccessibilitySystemAbilityClientImpl::ReregisterElementOperator()
{
    HILOG_DEBUG();

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is null.");
        return;
    }
    for (auto iter = elementOperators_.begin(); iter != elementOperators_.end(); iter++) {
        serviceProxy_->RegisterElementOperatorByWindowId(iter->first, iter->second);
    }
}

RetError AccessibilitySystemAbilityClientImpl::DeregisterElementOperator(const int32_t windowId)
{
    HILOG_INFO("Deregister windowId[%{public}d] start", windowId);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = elementOperators_.find(windowId);
    if (iter != elementOperators_.end()) {
        HILOG_DEBUG("windowID[%{public}d] is erase", windowId);
        elementOperators_.erase(iter);
    } else {
        HILOG_WARN("Not find windowID[%{public}d]", windowId);
        return RET_ERR_NO_REGISTER;
    }
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return static_cast<RetError>(serviceProxy_->DeregisterElementOperatorByWindowId(windowId));
}

RetError AccessibilitySystemAbilityClientImpl::DeregisterElementOperator(const int32_t windowId, const int32_t treeId)
{
    HILOG_INFO("Deregister windowId[%{public}d] treeId[%{public}d] start", windowId, treeId);
    std::lock_guard<ffrt::mutex> lock(mutex_);

    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    return static_cast<RetError>(serviceProxy_->DeregisterElementOperatorByWindowIdAndTreeId(windowId, treeId));
}

RetError AccessibilitySystemAbilityClientImpl::IsScreenReaderEnabled(bool &isEnabled)
{
    HILOG_DEBUG();
#ifdef ACCESSIBILITY_EMULATOR_DEFINED
    ApiReportHelper reporter("AccessibilitySystemAbilityClientImpl.IsScreenReaderEnabled");
#endif // ACCESSIBILITY_EMULATOR_DEFINED
    std::lock_guard<ffrt::mutex> lock(mutex_);
    
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    serviceProxy_->GetScreenReaderState(isEnabled);
    return RET_OK;
}

RetError AccessibilitySystemAbilityClientImpl::IsEnabled(bool &isEnabled)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isEnabled = stateHandler_.GetState(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
    return RET_OK;
}

RetError AccessibilitySystemAbilityClientImpl::IsTouchExplorationEnabled(bool &isEnabled)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isEnabled = stateHandler_.GetState(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED);
    return RET_OK;
}

void AccessibilitySystemAbilityClientImpl::GetTouchMode(std::string &touchMode)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    bool isTouchExploration = stateHandler_.GetState(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED);
    if (!isTouchExploration) {
        touchMode = "none";
        return;
    }

    bool isSingleClickMode = stateHandler_.GetState(AccessibilityStateEventType::EVENT_TOUCH_MODE_CHANGED);
    if (isSingleClickMode) {
        touchMode = "singleTouchMode";
    } else {
        touchMode = "doubleTouchMode";
    }
}

RetError AccessibilitySystemAbilityClientImpl::GetAbilityList(const uint32_t accessibilityAbilityTypes,
    const AbilityStateType stateType, std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
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
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    std::vector<AccessibilityAbilityInfoParcel> infosParcel = {};
    RetError ret = static_cast<RetError>(serviceProxy_->GetAbilityList(accessibilityAbilityTypes, stateType,
        infosParcel));
    if (infosParcel.size() < 0 || infosParcel.size() > ABILITY_SIZE_MAX) {
        HILOG_ERROR("abilityInfoSize is invalid");
        return RET_ERR_INVALID_PARAM;
    }
    for (auto& infoParcel : infosParcel) {
        infos.push_back(infoParcel);
    }
    return ret;
}

bool AccessibilitySystemAbilityClientImpl::CheckEventType(EventType eventType)
{
#ifdef ACCESSIBILITY_EMULATOR_DEFINED
    ApiReportHelper reporter("AccessibilitySystemAbilityClientImpl.CheckEventType");
#endif // ACCESSIBILITY_EMULATOR_DEFINED
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
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!CheckEventType(eventType)) {
        return RET_ERR_INVALID_PARAM;
    }
    AccessibilityEventInfo event;
    event.SetEventType(eventType);
    event.SetSource(componentId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    AccessibilityEventInfoParcel eventInfoParcel(event);
    return static_cast<RetError>(serviceProxy_->SendEvent(eventInfoParcel, 1));
}

RetError AccessibilitySystemAbilityClientImpl::SendEvent(const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("EventType[%{public}d]", event.GetEventType());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!CheckEventType(event.GetEventType())) {
        return RET_ERR_INVALID_PARAM;
    }
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    AccessibilityEventInfoParcel eventInfoParcel(event);
    return static_cast<RetError>(serviceProxy_->SendEvent(eventInfoParcel, 1));
}

RetError AccessibilitySystemAbilityClientImpl::SubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
#ifdef ACCESSIBILITY_EMULATOR_DEFINED
    ApiReportHelper reporter("AccessibilitySystemAbilityClientImpl.SubscribeStateObserver");
#endif // ACCESSIBILITY_EMULATOR_DEFINED
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
    std::lock_guard<ffrt::mutex> lock(mutex_);
#ifdef ACCESSIBILITY_EMULATOR_DEFINED
    ApiReportHelper reporter("AccessibilitySystemAbilityClientImpl.UnsubscribeStateObserver");
#endif // ACCESSIBILITY_EMULATOR_DEFINED
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

    if (eventType != EVENT_CONFIG_EVENT_CHANGED) {
        if (stateHandler_.GetState(static_cast<AccessibilityStateEventType>(eventType)) == value) {
            HILOG_DEBUG("State value is not changed");
            return;
        }
    }

    stateHandler_.SetState(static_cast<AccessibilityStateEventType>(eventType), value);
    StateObserverVector &observers = stateObserversArray_[eventType];
    HILOG_INFO("observers size is %{public}zu", observers.size());
    for (auto &observer : observers) {
        if (observer) {
            observer->OnStateChanged(value);
        } else {
            HILOG_ERROR("end stateObserversArray[%{public}d] is null", eventType);
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyTouchModeChanged(bool touchExplorationState, bool isSingleTouchMode)
{
    HILOG_DEBUG("touchExplorationState = %{public}d, isSingleTouchMode = %{public}d", touchExplorationState,
        isSingleTouchMode);
    
    bool originalTouchMode = stateHandler_.GetState(EVENT_TOUCH_MODE_CHANGED);
    stateHandler_.SetState(EVENT_TOUCH_MODE_CHANGED, isSingleTouchMode);

    if (!touchExplorationState) {
        HILOG_DEBUG("touch guide state is false");
        return;
    }

    if ((originalTouchMode == isSingleTouchMode) &&
        (stateHandler_.GetState(EVENT_TOUCH_GUIDE_STATE_CHANGED) == touchExplorationState)) {
        HILOG_DEBUG("touch mode is not changed");
        return;
    }

    StateObserverVector &observers = stateObserversArray_[EVENT_TOUCH_MODE_CHANGED];
    for (auto &observer : observers) {
        if (observer) {
            observer->OnStateChanged(isSingleTouchMode);
        } else {
            HILOG_ERROR("touch mode observer is null!");
        }
    }
}

RetError AccessibilitySystemAbilityClientImpl::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    RetError ret = static_cast<RetError>(serviceProxy_->GetEnabledAbilities(enabledAbilities));
    if (enabledAbilities.size() < 0 || enabledAbilities.size() > ABILITY_SIZE_MAX) {
        HILOG_ERROR("dev_num is invalid");
        return RET_ERR_INVALID_PARAM;
    }
    return ret;
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType[%{public}d}", stateType);
    SetAccessibilityState(stateType);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    // the notification of the single click mode must be earlier than the notification of the touch exploration state;
    NotifyTouchModeChanged(!!(stateType & STATE_EXPLORATION_ENABLED), !!(stateType & STATE_SINGLE_CLICK_MODE_ENABLED));

    NotifyStateChanged(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED,
        !!(stateType & STATE_ACCESSIBILITY_ENABLED));
    
    NotifyStateChanged(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED,
        !!(stateType & STATE_EXPLORATION_ENABLED));

    NotifyStateChanged(AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED,
        !!(stateType & STATE_KEYEVENT_ENABLED));

    NotifyStateChanged(AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED,
        !!(stateType & STATE_GESTURE_ENABLED));

    NotifyStateChanged(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED,
        !!(stateType & STATE_SCREENREADER_ENABLED));

    NotifyStateChanged(AccessibilityStateEventType::EVENT_CONFIG_EVENT_CHANGED,
        !!(stateType & STATE_CONFIG_EVENT_CHANGE));
}

void AccessibilitySystemAbilityClientImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId < 0) {
        HILOG_ERROR("requestId is invalid");
        return;
    }
    if (callback != nullptr) {
        if (callback->GetFilter()) {
            AccessibilityElementOperatorImpl::SetFiltering(filterInfos);
        }
        serviceProxy_->RemoveRequestId(requestId);
        callback->SetSearchElementInfoByAccessibilityIdResult(filterInfos, requestId);
        AccessibilityElementOperatorImpl::EraseCallback(requestId);
    } else {
        HILOG_INFO("callback is nullptr");
    }
}

void AccessibilitySystemAbilityClientImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId < 0) {
        HILOG_ERROR("requestId is invalid");
        return;
    }
    if (callback != nullptr) {
        if (callback->GetFilter()) {
            AccessibilityElementOperatorImpl::SetFiltering(filterInfos);
        }
        serviceProxy_->RemoveRequestId(requestId);
        callback->SetSearchDefaultFocusByWindowIdResult(filterInfos, requestId);
        AccessibilityElementOperatorImpl::EraseCallback(requestId);
    } else {
        HILOG_ERROR("callback is nullptr");
    }
}

void AccessibilitySystemAbilityClientImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId >= 0) {
        if (callback != nullptr) {
            serviceProxy_->RemoveRequestId(requestId);
            callback->SetSearchElementInfoByTextResult(filterInfos, requestId);
            AccessibilityElementOperatorImpl::EraseCallback(requestId);
        } else {
            HILOG_INFO("callback is nullptr");
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId >= 0) {
        if (callback != nullptr) {
            serviceProxy_->RemoveRequestId(requestId);
            callback->SetFindFocusedElementInfoResult(info, requestId);
            AccessibilityElementOperatorImpl::EraseCallback(requestId);
        } else {
            HILOG_INFO("callback is nullptr");
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId >= 0) {
        if (callback != nullptr) {
            serviceProxy_->RemoveRequestId(requestId);
            callback->SetFocusMoveSearchResult(info, requestId);
            AccessibilityElementOperatorImpl::EraseCallback(requestId);
        } else {
            HILOG_INFO("callback is nullptr");
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetExecuteActionResult(
    const bool succeeded, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId >= 0) {
        if (callback != nullptr) {
            serviceProxy_->RemoveRequestId(requestId);
            callback->SetExecuteActionResult(succeeded, requestId);
            AccessibilityElementOperatorImpl::EraseCallback(requestId);
        } else {
            HILOG_INFO("callback is nullptr");
        }
    }
}

void AccessibilitySystemAbilityClientImpl::SetCursorPositionResult(
    const int32_t cursorPosition, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("requestId[%{public}d]  cursorPosition[%{public}d]", requestId, cursorPosition);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId >= 0) {
        if (callback != nullptr) {
            serviceProxy_->RemoveRequestId(requestId);
            callback->SetCursorPositionResult(cursorPosition, requestId);
            AccessibilityElementOperatorImpl::EraseCallback(requestId);
        } else {
            HILOG_INFO("callback is nullptr");
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
            SetSearchElementInfoByAccessibilityIdResult(elementInfos, requestId);
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
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return static_cast<RetError>(serviceProxy_->GetFocusedWindowId(focusedWindowId));
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

RetError AccessibilitySystemAbilityClientImpl::SearchNeedEvents(std::vector<uint32_t> &needEvents)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return static_cast<RetError>(serviceProxy_->SearchNeedEvents(needEvents));
}

void AccessibilitySystemAbilityClientImpl::SetSearchElementInfoBySpecificPropertyResult(
    const std::list<AccessibilityElementInfo> &infos, const std::list<AccessibilityElementInfo> &treeInfos,
    const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    std::vector<AccessibilityElementInfo> filterInfos(infos.begin(), infos.end());
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId < 0) {
        HILOG_ERROR("requestId is invalid");
        return;
    }
    if (callback != nullptr) {
        if (callback->GetFilter()) {
            AccessibilityElementOperatorImpl::SetFiltering(filterInfos);
        }
        serviceProxy_->RemoveRequestId(requestId);
        callback->SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
        AccessibilityElementOperatorImpl::EraseCallback(requestId);
    } else {
        HILOG_INFO("callback is nullptr");
    }
}

void AccessibilitySystemAbilityClientImpl::SetFocusMoveSearchWithConditionResult(
    const std::list<AccessibilityElementInfo> &infos, const FocusMoveResult& result, const int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId < 0) {
        HILOG_ERROR("requestId is invalid");
        return;
    }
    if (callback != nullptr) {
        serviceProxy_->RemoveRequestId(requestId);
        callback->SetFocusMoveSearchWithConditionResult(infos, result, requestId);
        AccessibilityElementOperatorImpl::EraseCallback(requestId);
    } else {
        HILOG_INFO("callback is nullptr");
    }
}

void AccessibilitySystemAbilityClientImpl::SetDetectElementInfoFocusableThroughAncestorResult(
    bool isFocusable, const int32_t requestId, const AccessibilityElementInfo &info)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("search element requestId[%{public}d]", requestId);
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("serviceProxy_ is nullptr");
        return;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        AccessibilityElementOperatorImpl::GetCallbackByRequestId(requestId);
    if (requestId < 0) {
        HILOG_ERROR("requestId is invalid");
        return;
    }
    if (callback != nullptr) {
        serviceProxy_->RemoveRequestId(requestId);
        callback->SetDetectElementInfoFocusableThroughAncestorResult(isFocusable, requestId, info);
        AccessibilityElementOperatorImpl::EraseCallback(requestId);
    } else {
        HILOG_INFO("callback is nullptr");
    }
}

AccessibilitySystemAbilityClientImpl::StateArrayHandler::StateArrayHandler()
{
    std::unique_lock<ffrt::shared_mutex> wLock(rwLock_);
    stateArray_.fill(false);
}

void AccessibilitySystemAbilityClientImpl::StateArrayHandler::SetState(AccessibilityStateEventType type, bool state)
{
    std::unique_lock<ffrt::shared_mutex> wLock(rwLock_);
    stateArray_[type] = state;
}

bool AccessibilitySystemAbilityClientImpl::StateArrayHandler::GetState(AccessibilityStateEventType type)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    return stateArray_[type];
}

void AccessibilitySystemAbilityClientImpl::StateArrayHandler::Reset()
{
    std::unique_lock<ffrt::shared_mutex> wLock(rwLock_);
    stateArray_.fill(false);
}

RetError AccessibilitySystemAbilityClientImpl::IsScreenReaderRulesEnabled(bool &isEnabled)
{
    auto& rulesChecker = ReadableRulesChecker::GetInstance();
    isEnabled = false;
    isEnabled = CheckRulesCheckerIsInit(rulesChecker, serviceProxy_);
    return RET_OK;
}
 
RetError AccessibilitySystemAbilityClientImpl::CheckNodeIsReadable(
    const std::shared_ptr<ReadableRulesNode>& node, bool& isReadable)
{
    auto& rulesChecker = ReadableRulesChecker::GetInstance();
    isReadable = false;
    if (!CheckRulesCheckerIsInit(rulesChecker, serviceProxy_)) {
        return RET_ERR_NOT_ENABLED;
    }
    isReadable = rulesChecker.IsReadable(node);
    return RET_OK;
}
 
RetError AccessibilitySystemAbilityClientImpl::CheckNodeIsSpecificType(
    const std::shared_ptr<ReadableRulesNode>& node, ReadableSpecificType specificType, bool& isHit)
{
    auto& rulesChecker = ReadableRulesChecker::GetInstance();
    if (!CheckRulesCheckerIsInit(rulesChecker, serviceProxy_)) {
        isHit = false;
        return RET_ERR_NOT_ENABLED;
    }
 
    switch (specificType) {
        case ReadableSpecificType::ROOT_TYPE:
            isHit = rulesChecker.IsRootType(node);
            break;
        case ReadableSpecificType::IGNORE_SCROLL_TYPE:
            isHit = rulesChecker.IsScrollIgnoreTypes(node);
            break;
        default:
            break;
    }
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS