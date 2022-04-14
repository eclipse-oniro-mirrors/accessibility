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

#include "accessibility_system_ability_client_impl.h"
#include "ability_context.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
static std::mutex g_Mutex;
static std::shared_ptr<AccessibilitySystemAbilityClientImpl> g_Instance = nullptr;
std::shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        int32_t accountId = 100;    // Temp deal
        g_Instance = std::make_shared<AccessibilitySystemAbilityClientImpl>(accountId);
    } else {
        HILOG_DEBUG("AccessibilitySystemAbilityClient had construct instance");
    }

    return g_Instance;
}

bool AccessibilitySystemAbilityClientImpl::ConnectToService()
{
    if (serviceProxy_) {
        HILOG_DEBUG("AAMS Service is connected");
        return true;
    }

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return false;
    }

    if (!deathRecipient_) {
        deathRecipient_ = new DeathRecipient(*this);
    }

    if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
        HILOG_ERROR("Failed to add death recipient");
    }

    HILOG_DEBUG("Get remote object ok");
    serviceProxy_ = iface_cast<AccessibleAbilityManagerServiceProxy>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }

    return true;
}

AccessibilitySystemAbilityClientImpl::AccessibilitySystemAbilityClientImpl(int32_t accountId) : accountId_(accountId)
{
    if (!ConnectToService()) {
        HILOG_ERROR("Failed to connect to aams service");
        return;
    }

    stateObserver_ = new AccessibleAbilityManagerStateObserverImpl(*this);
    uint32_t stateType = serviceProxy_->RegisterStateObserver(stateObserver_, accountId_);
    if (stateType & STATE_ACCESSIBILITY_ENABLED) {
        isEnabled_ = true;
    } else {
        isEnabled_ = false;
    }

    if (stateType & STATE_EXPLORATION_ENABLED) {
        isTouchExplorationEnabled_ = true;
    } else {
        isTouchExplorationEnabled_ = false;
    }

    if (stateType & STATE_CAPTION_ENABLED) {
        isCaptionEnabled_ = true;
    } else {
        isCaptionEnabled_ = false;
    }

    if (stateType & STATE_KEYEVENT_ENABLED) {
        isFilteringKeyEventsEnabled_ = true;
    } else {
        isFilteringKeyEventsEnabled_ = false;
    }

    if (stateType & STATE_GESTURE_ENABLED) {
        isGesturesSimulationEnabled_ = true;
    } else {
        isGesturesSimulationEnabled_ = false;
    }

    captionObserver_ = new AccessibleAbilityManagerCaptionObserverImpl(*this);
    serviceProxy_->RegisterCaptionObserver(captionObserver_, accountId_);
}

AccessibilitySystemAbilityClientImpl::~AccessibilitySystemAbilityClientImpl()
{
    HILOG_DEBUG("start");
}

void AccessibilitySystemAbilityClientImpl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (serviceProxy_) {
        sptr<IRemoteObject> object = serviceProxy_->GetRemoteObject();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            HILOG_DEBUG("Reset OK");
        }
    }
}

int32_t AccessibilitySystemAbilityClientImpl::RegisterElementOperator(
    const int32_t windowId, const std::shared_ptr<AccessibilityElementOperator> &operation, int32_t accountId)
{
    HILOG_INFO();
    if (!operation) {
        HILOG_ERROR("Input operation is null");
        return -1;
    }

    auto iter = interactionOperators_.find(windowId);
    if (iter != interactionOperators_.end()) {
        HILOG_ERROR("windowID[%{public}d] is exited", windowId);
        return 0;
    }

    connectionWindowId_ = windowId;
    sptr<AccessibilityElementOperatorImpl> aamsInteractionOperator =
        new AccessibilityElementOperatorImpl(windowId, operation);
    interactionOperators_[windowId] = aamsInteractionOperator;

    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return -1;
    }
    serviceProxy_->RegisterElementOperator(windowId, aamsInteractionOperator, accountId);

    return 0;
}

void AccessibilitySystemAbilityClientImpl::DeregisterElementOperator(const int32_t windowId)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return;
    }
    serviceProxy_->DeregisterElementOperator(windowId);
    auto iter = interactionOperators_.find(windowId);
    if (iter != interactionOperators_.end()) {
        HILOG_DEBUG("windowID[%{public}d] is erase", windowId);
        connectionWindowId_ = -1;
        interactionOperators_.erase(iter);
        return;
    }
    HILOG_DEBUG("Not find windowID[%{public}d]", windowId);
}

bool AccessibilitySystemAbilityClientImpl::IsEnabled()
{
    HILOG_DEBUG("isEnabled_[%{public}d]", isEnabled_);
    return isEnabled_;
}

bool AccessibilitySystemAbilityClientImpl::IsTouchExplorationEnabled()
{
    HILOG_DEBUG("isEnabled_[%{public}d]", isTouchExplorationEnabled_);
    return isTouchExplorationEnabled_;
}

bool AccessibilitySystemAbilityClientImpl::IsCaptionEnabled()
{
    HILOG_DEBUG("isEnabled_[%{public}d]", isCaptionEnabled_);
    return isCaptionEnabled_;
}

std::vector<AccessibilityAbilityInfo> AccessibilitySystemAbilityClientImpl::GetAbilityList(
    const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType)
{
    HILOG_DEBUG("start");
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
    if (!serviceProxy_ || !check) {
        HILOG_ERROR("Failed to get aams service");
        std::vector<AccessibilityAbilityInfo> infos;
        return infos;
    }
    return (serviceProxy_->GetAbilityList(accessibilityAbilityTypes, stateType));
}

CaptionProperty AccessibilitySystemAbilityClientImpl::GetCaptionProperty() const
{
    HILOG_DEBUG("start");
    CaptionProperty cp;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return cp;
    }
    return serviceProxy_->GetCaptionProperty();
}

bool AccessibilitySystemAbilityClientImpl::SetCaptionProperty(const CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    if (captionProperty_.GetFontScale() != caption.GetFontScale() ||
        captionProperty_.GetFontColor() != caption.GetFontColor() ||
        strcmp(captionProperty_.GetFontFamily().c_str(), caption.GetFontFamily().c_str()) ||
        strcmp(captionProperty_.GetFontEdgeType().c_str(), caption.GetFontEdgeType().c_str()) ||
        captionProperty_.GetBackgroundColor() != caption.GetBackgroundColor() ||
        captionProperty_.GetWindowColor() != caption.GetWindowColor()) {
        captionProperty_ = caption;
        NotifyCaptionChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SetCaptionPropertyTojson(const CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    if (captionProperty_.GetFontScale() != caption.GetFontScale() ||
        captionProperty_.GetFontColor() != caption.GetFontColor() ||
        strcmp(captionProperty_.GetFontFamily().c_str(), caption.GetFontFamily().c_str()) ||
        strcmp(captionProperty_.GetFontEdgeType().c_str(), caption.GetFontEdgeType().c_str()) ||
        captionProperty_.GetBackgroundColor() != caption.GetBackgroundColor() ||
        captionProperty_.GetWindowColor() != caption.GetWindowColor()) {
        if (!serviceProxy_) {
            HILOG_ERROR("Failed to get aams service");
            return false;
        }
        serviceProxy_->SetCaptionProperty(caption);
    }
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SetCaptionStateTojson(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (isCaptionEnabled_ != state) {
        if (!serviceProxy_) {
            HILOG_ERROR("Failed to get aams service");
            return false;
        }
        ret = serviceProxy_->SetCaptionState(state);
    }

    return ret;
}

bool AccessibilitySystemAbilityClientImpl::IsAccessibilityCaptionEnabled() const
{
    HILOG_DEBUG("start");
    return true;
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

bool AccessibilitySystemAbilityClientImpl::CheckActionType(ActionType actionType)
{
    if ((actionType < ActionType::ACCESSIBILITY_ACTION_FOCUS) ||
        ((actionType > ActionType::ACCESSIBILITY_ACTION_DELETED) &&
         (actionType != ActionType::ACCESSIBILITY_ACTION_TYPE_MASK))) {
        HILOG_ERROR("action type is invalid");
        return false;
    } else {
        return true;
    }
}

bool AccessibilitySystemAbilityClientImpl::SendEvent(const EventType eventType, const int32_t componentId)
{
    HILOG_DEBUG("componentId[%{public}d], eventType[%{public}d]", componentId, eventType);
    if (!CheckEventType(eventType)) {
        return false;
    }
    AccessibilityEventInfo event;
    event.SetEventType(eventType);
    event.SetSource(componentId);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->SendEvent(event, accountId_);
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SendEvent(const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("EventType[%{public}d]", event.GetEventType());
    if (!CheckEventType(event.GetEventType())) {
        return false;
    }
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->SendEvent(event, accountId_);
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG("start");
    if (eventType >= AccessibilityStateEventType::EVENT_TYPE_MAX) {
        HILOG_ERROR("Input eventType is out of scope");
        return false;
    }
    if (!observer) {
        HILOG_ERROR("Input observer is null");
        return false;
    }

    StateObserverVector &observerVector = stateObserversArray_[eventType];
    for (auto iter = observerVector.begin(); iter != observerVector.end(); ++iter) {
        if (*iter == observer) {
            HILOG_INFO("Observer has subscribed!");
            return true;
        }
    }
    observerVector.push_back(observer);
    HILOG_DEBUG("end");
    return true;
}

bool AccessibilitySystemAbilityClientImpl::UnsubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG("start. eventType is [%{public}d]", eventType);
    if (eventType >= AccessibilityStateEventType::EVENT_TYPE_MAX) {
        HILOG_ERROR("Input eventType is out of scope");
        return false;
    }
    if (!observer) {
        HILOG_ERROR("Input observer is null");
        return false;
    }

    StateObserverVector &observerVector = stateObserversArray_[eventType];
    for (auto iter = observerVector.begin(); iter != observerVector.end(); ++iter) {
        if (*iter == observer) {
            observerVector.erase(iter);
            return true;
        }
    }
    HILOG_ERROR("The observer has not subscribed.");
    return false;
}

void AccessibilitySystemAbilityClientImpl::UpdateEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (isEnabled_ != enabled) {
        isEnabled_ = enabled;
        NotifyAccessibilityStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::UpdateTouchExplorationEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (isTouchExplorationEnabled_ != enabled) {
        isTouchExplorationEnabled_ = enabled;
        NotifyTouchExplorationStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::UpdateGestureState(const bool state)
{
    HILOG_DEBUG("start");
    if (isGesturesSimulationEnabled_ != state) {
        isGesturesSimulationEnabled_ = state;
        NotifyGestureStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::UpdateKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start");
    if (isFilteringKeyEventsEnabled_ != state) {
        isFilteringKeyEventsEnabled_ = state;
        NotifyKeyEventStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::UpdateCaptionEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (isCaptionEnabled_ != enabled) {
        isCaptionEnabled_ = enabled;
        NotifyCaptionStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyAccessibilityStateChanged()
{
    HILOG_DEBUG("isEnabled_ is %{public}d", isEnabled_);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    StateObserverVector &observers =
        stateObserversArray_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED];

    for (auto observer : observers) {
        if (observer) {
            observer->OnStateChanged(isEnabled_);
        } else {
            HILOG_ERROR("end stateObserversArray[EVENT_ACCESSIBILITY_STATE_CHANGED] is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyTouchExplorationStateChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    StateObserverVector &observers =
        stateObserversArray_[AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED];

    for (auto observer : observers) {
        if (observer) {
            observer->OnStateChanged(isTouchExplorationEnabled_);
        } else {
            HILOG_ERROR("end stateObserversArray[EVENT_TOUCH_GUIDE_STATE_CHANGED] is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyKeyEventStateChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    StateObserverVector &observers = stateObserversArray_[AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED];

    for (auto observer : observers) {
        if (observer) {
            observer->OnStateChanged(isFilteringKeyEventsEnabled_);
        } else {
            HILOG_ERROR("end stateObserversArray[EVENT_KEVEVENT_STATE_CHANGED] is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyGestureStateChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    StateObserverVector &observers = stateObserversArray_[AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED];

    for (auto observer : observers) {
        if (observer) {
            observer->OnStateChanged(isGesturesSimulationEnabled_);
        } else {
            HILOG_ERROR("end stateObserversArray[EVENT_GESTURE_STATE_CHANGED] is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyCaptionStateChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (!observersCaptionEnable_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : observersCaptionEnable_) {
        if (observer) {
            observer->OnStateChanged(isCaptionEnabled_);
        } else {
            HILOG_ERROR("end observersCaptionEnable_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilitySystemAbilityClientImpl::AddCaptionListener(const std::shared_ptr<CaptionObserver> &ob,
    const int32_t type)
{
    HILOG_DEBUG("start");
    bool result = true;
    if (type == CaptionObserverType::CAPTION_ENABLE) {
        if (!observersCaptionEnable_.size()) {
            observersCaptionEnable_.push_back(ob);
        }
    } else if (type == CaptionObserverType::CAPTION_PROPERTY) {
        if (!observersCaptionProperty_.size()) {
            observersCaptionProperty_.push_back(ob);
        }
    } else {
        result = false;
        HILOG_ERROR("Type Error ");
    }

    return result;
}

bool AccessibilitySystemAbilityClientImpl::DeleteCaptionListener(const std::shared_ptr<CaptionObserver> &ob,
    const int32_t type)
{
    HILOG_DEBUG("start");
    bool result = false;
    if (type == CaptionObserverType::CAPTION_ENABLE) {
        for (auto enable = observersCaptionEnable_.begin(); enable != observersCaptionEnable_.end(); ++enable) {
            if (*enable == ob) {
                observersCaptionEnable_.erase(enable);
                result = true;
                break;
            }
        }
    } else if (type == CaptionObserverType::CAPTION_PROPERTY) {
        for (auto property = observersCaptionProperty_.begin(); property != observersCaptionProperty_.end();
            ++property) {
            if (*property == ob) {
                observersCaptionProperty_.erase(property);
                result = true;
                break;
            }
        }
    } else {
        HILOG_ERROR("Type Error ");
    }

    return result;
}

void AccessibilitySystemAbilityClientImpl::NotifyCaptionChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (!observersCaptionProperty_.size()) {
        HILOG_DEBUG("observersCaptionProperty_ is null");
        return;
    }
    for (auto it = observersCaptionProperty_.begin(); it != observersCaptionProperty_.end(); ++it) {
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnPropertyChanged(captionProperty_);
        } else {
            HILOG_ERROR("end observersCaptionProperty_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilitySystemAbilityClientImpl::GetEnabledState()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetEnabledState();
}

bool AccessibilitySystemAbilityClientImpl::GetCaptionState()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    isCaptionEnabled_ = serviceProxy_->GetCaptionState();
    return isCaptionEnabled_;
}

bool AccessibilitySystemAbilityClientImpl::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetTouchGuideState();
}

bool AccessibilitySystemAbilityClientImpl::GetGestureState()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetGestureState();
}

bool AccessibilitySystemAbilityClientImpl::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetKeyEventObserverState();
}

bool AccessibilitySystemAbilityClientImpl::EnableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->EnableAbilities(abilities);
    return true;
}

std::vector<std::string> AccessibilitySystemAbilityClientImpl::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        std::vector<std::string> abilities;
        return abilities;
    }
    return serviceProxy_->GetEnabledAbilities();
}

std::vector<AccessibilityAbilityInfo> AccessibilitySystemAbilityClientImpl::GetInstalledAbilities()
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        std::vector<AccessibilityAbilityInfo> it;
        return it;
    }
    return serviceProxy_->GetInstalledAbilities();
}

bool AccessibilitySystemAbilityClientImpl::DisableAbilities(std::vector<std::string> &abilities)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->DisableAbilities(abilities);

    return true;
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType[%{public}d}", stateType);
    if (stateType & STATE_ACCESSIBILITY_ENABLED) {
        UpdateEnabled(true);
    } else {
        UpdateEnabled(false);
    }

    if (stateType & STATE_EXPLORATION_ENABLED) {
        UpdateTouchExplorationEnabled(true);
    } else {
        UpdateTouchExplorationEnabled(false);
    }

    if (stateType & STATE_CAPTION_ENABLED) {
        UpdateCaptionEnabled(true);
    } else {
        UpdateCaptionEnabled(false);
    }

    if (stateType & STATE_KEYEVENT_ENABLED) {
        UpdateKeyEventObserverState(true);
    } else {
        UpdateKeyEventObserverState(false);
    }

    if (stateType & STATE_GESTURE_ENABLED) {
        UpdateGestureState(true);
    } else {
        UpdateGestureState(false);
    }
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerCaptionPropertyChanged(
    const CaptionProperty &property)
{
    HILOG_DEBUG("start");
    captionProperty_ = property;
    NotifyCaptionChanged();
    HILOG_DEBUG("end");
}
} // namespace Accessibility
} // namespace OHOS