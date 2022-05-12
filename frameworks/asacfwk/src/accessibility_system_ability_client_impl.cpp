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
        g_Instance = std::make_shared<AccessibilitySystemAbilityClientImpl>();
    } else {
        HILOG_DEBUG("AccessibilitySystemAbilityClient had construct instance");
    }

    return g_Instance;
}

AccessibilitySystemAbilityClientImpl::AccessibilitySystemAbilityClientImpl()
{
    if (!ConnectToService()) {
        HILOG_ERROR("Failed to connect to aams service");
        return;
    }

    stateArray_.fill(false);
    stateObserver_ = new AccessibleAbilityManagerStateObserverImpl(*this);
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
    if (stateType & STATE_CAPTION_ENABLED) {
        isCaptionEnabled_ = true;
    }

    captionObserver_ = new AccessibleAbilityManagerCaptionObserverImpl(*this);
    serviceProxy_->RegisterCaptionObserver(captionObserver_);
}

AccessibilitySystemAbilityClientImpl::~AccessibilitySystemAbilityClientImpl()
{
    HILOG_DEBUG("start");
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
    serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }

    return true;
}

void AccessibilitySystemAbilityClientImpl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
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

int32_t AccessibilitySystemAbilityClientImpl::RegisterElementOperator(
    const int32_t windowId, const std::shared_ptr<AccessibilityElementOperator> &operation, int32_t accountId)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!operation) {
        HILOG_ERROR("Input operation is null");
        return -1;
    }
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return -1;
    }

    auto iter = interactionOperators_.find(windowId);
    if (iter != interactionOperators_.end()) {
        HILOG_ERROR("windowID[%{public}d] is exited", windowId);
        return 0;
    }

    sptr<AccessibilityElementOperatorImpl> aamsInteractionOperator =
        new AccessibilityElementOperatorImpl(windowId, operation);
    interactionOperators_[windowId] = aamsInteractionOperator;
    serviceProxy_->RegisterElementOperator(windowId, aamsInteractionOperator);

    return 0;
}

void AccessibilitySystemAbilityClientImpl::DeregisterElementOperator(const int32_t windowId)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);

    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return;
    }
    serviceProxy_->DeregisterElementOperator(windowId);
    auto iter = interactionOperators_.find(windowId);
    if (iter != interactionOperators_.end()) {
        HILOG_DEBUG("windowID[%{public}d] is erase", windowId);
        interactionOperators_.erase(iter);
        return;
    }
    HILOG_DEBUG("Not find windowID[%{public}d]", windowId);
}

bool AccessibilitySystemAbilityClientImpl::IsEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return stateArray_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED];
}

bool AccessibilitySystemAbilityClientImpl::IsTouchExplorationEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return stateArray_[AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED];
}

bool AccessibilitySystemAbilityClientImpl::IsCaptionEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOG_DEBUG("IsCaptionEnabled[%{public}d]", isCaptionEnabled_);
    return isCaptionEnabled_;
}

std::vector<AccessibilityAbilityInfo> AccessibilitySystemAbilityClientImpl::GetAbilityList(
    const uint32_t accessibilityAbilityTypes, const AbilityStateType stateType)
{
    HILOG_DEBUG("start");
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
    if (!serviceProxy_ || !check) {
        HILOG_ERROR("Failed to get aams service");
        std::vector<AccessibilityAbilityInfo> infos;
        return infos;
    }
    return (serviceProxy_->GetAbilityList(accessibilityAbilityTypes, stateType));
}

AccessibilityConfig::CaptionProperty AccessibilitySystemAbilityClientImpl::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    AccessibilityConfig::CaptionProperty cp;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return cp;
    }
    return serviceProxy_->GetCaptionProperty();
}

bool AccessibilitySystemAbilityClientImpl::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
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

bool AccessibilitySystemAbilityClientImpl::SetCaptionPropertyTojson(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
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
    serviceProxy_->SendEvent(event);
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SendEvent(const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("EventType[%{public}d]", event.GetEventType());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckEventType(event.GetEventType())) {
        return false;
    }
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->SendEvent(event);
    return true;
}

bool AccessibilitySystemAbilityClientImpl::SubscribeStateObserver(
    const std::shared_ptr<AccessibilityStateObserver> &observer, const uint32_t eventType)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
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

void AccessibilitySystemAbilityClientImpl::UpdateCaptionEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (isCaptionEnabled_ != enabled) {
        isCaptionEnabled_ = enabled;
        NotifyCaptionStateChanged();
    }
    HILOG_DEBUG("end");
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
    for (auto observer : observers) {
        if (observer) {
            observer->OnStateChanged(value);
        } else {
            HILOG_ERROR("end stateObserversArray[%{public}d] is null", eventType);
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilitySystemAbilityClientImpl::NotifyCaptionStateChanged()
{
    HILOG_DEBUG("start");
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

bool AccessibilitySystemAbilityClientImpl::AddCaptionListener(const std::shared_ptr<AccessibilityConfig::CaptionObserver> &ob,
    const int32_t type)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = true;
    if (type == AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE) {
        if (!observersCaptionEnable_.size()) {
            observersCaptionEnable_.push_back(ob);
        }
    } else if (type == AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY) {
        if (!observersCaptionProperty_.size()) {
            observersCaptionProperty_.push_back(ob);
        }
    } else {
        result = false;
        HILOG_ERROR("Type Error ");
    }

    return result;
}

bool AccessibilitySystemAbilityClientImpl::DeleteCaptionListener(const std::shared_ptr<AccessibilityConfig::CaptionObserver> &ob,
    const int32_t type)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = false;
    if (type == AccessibilityConfig::CaptionObserverType::CAPTION_ENABLE) {
        for (auto enable = observersCaptionEnable_.begin(); enable != observersCaptionEnable_.end(); ++enable) {
            if (*enable == ob) {
                observersCaptionEnable_.erase(enable);
                result = true;
                break;
            }
        }
    } else if (type == AccessibilityConfig::CaptionObserverType::CAPTION_PROPERTY) {
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetEnabledState();
}

bool AccessibilitySystemAbilityClientImpl::GetCaptionState()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetTouchGuideState();
}

bool AccessibilitySystemAbilityClientImpl::GetGestureState()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetGestureState();
}

bool AccessibilitySystemAbilityClientImpl::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->GetKeyEventObserverState();
}

std::vector<std::string> AccessibilitySystemAbilityClientImpl::GetEnabledAbilities()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        std::vector<AccessibilityAbilityInfo> it;
        return it;
    }
    return serviceProxy_->GetInstalledAbilities();
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType[%{public}d}", stateType);
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

    if (stateType & STATE_CAPTION_ENABLED) {
        UpdateCaptionEnabled(true);
    } else {
        UpdateCaptionEnabled(false);
    }
}

void AccessibilitySystemAbilityClientImpl::OnAccessibleAbilityManagerCaptionPropertyChanged(
    const AccessibilityConfig::CaptionProperty &property)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    captionProperty_ = property;
    NotifyCaptionChanged();
    HILOG_DEBUG("end");
}
} // namespace Accessibility
} // namespace OHOS