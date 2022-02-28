/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_system_ability_client.h"
#include "ability_context.h"
#include "accessibility_element_operator.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_window_info.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessible_ability_manager_service_caption_property_stub.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace OHOS::AppExecFwk;
using namespace std;

namespace OHOS {
namespace Accessibility {
shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::instance_ = nullptr;

struct AccessibilitySystemAbilityClient::Impl {
    class DeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DeathRecipient() = default;
        ~DeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(DeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote)
        {
            AccessibilitySystemAbilityClient::GetInstance()->ResetService(remote);
        }
    };
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<AccessibleAbilityManagerServiceStateStub> stateCallback_ = nullptr;
    sptr<AccessibleAbilityManagerServiceClientProxy> serviceProxy_ = nullptr;
    sptr<AccessibleAbilityManagerServiceCaptionPropertyStub> captionPropertyCallback_ = nullptr;
    sptr<IAccessibleAbilityManagerServiceClient> GetService()
    {
        if (serviceProxy_ != nullptr) {
            return serviceProxy_;
        }

        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            HILOG_ERROR("[%s] Failed to get ISystemAbilityManager", __func__);
            return nullptr;
        }

        sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
        if (object == nullptr) {
            HILOG_ERROR("[%s]Get IAccessibleAbilityManagerServiceClient object from samgr failed", __func__);
            return nullptr;
        }

        if (deathRecipient_ == nullptr) {
            deathRecipient_ = new DeathRecipient();
        }

        if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
            HILOG_ERROR("Failed to add death recipient");
        }

        HILOG_DEBUG("get remote object ok");
        serviceProxy_ = iface_cast<AccessibleAbilityManagerServiceClientProxy>(object);
        if (serviceProxy_ == nullptr) {
            HILOG_ERROR("IAccessibleAbilityManagerServiceClient iface_cast failed");
        }
        return serviceProxy_;
    }
};

AccessibilitySystemAbilityClient::AccessibilitySystemAbilityClient(const Context& context, int accountId)
    : pimpl(std::make_unique<Impl>())
{
    interactionOperator_ = nullptr;
    accountId_ = accountId;
    pimpl->stateCallback_ = new AccessibleAbilityManagerServiceStateStub();
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return;
    }
    uint32_t stateType = proxyService->RegisterStateCallback(pimpl->stateCallback_, accountId_);
    if (stateType & AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED) {
        isEnabled_ = true;
    } else {
        isEnabled_ = false;
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED) {
        isTouchExplorationEnabled_ = true;
    } else {
        isTouchExplorationEnabled_ = false;
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_CAPTION_ENABLED) {
        isCaptionEnabled_ = true;
    } else {
        isCaptionEnabled_ = false;
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_KEYEVENT_ENABLED) {
        isFilteringKeyEventsEnabled_ = true;
    } else {
        isFilteringKeyEventsEnabled_ = false;
    }

    if (stateType & AccessibilitySystemAbilityClient::STATE_GESTURE_ENABLED) {
        isGesturesSimulationEnabled_ = true;
    } else {
        isGesturesSimulationEnabled_ = false;
    }

    pimpl->captionPropertyCallback_ = new AccessibleAbilityManagerServiceCaptionPropertyStub();
    proxyService->RegisterCaptionPropertyCallback(pimpl->captionPropertyCallback_, accountId_);
}

void AccessibilitySystemAbilityClient::ResetService(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (pimpl->serviceProxy_ != nullptr) {
        sptr<IRemoteObject> object = pimpl->serviceProxy_->GetObject();
        if ((object != nullptr) && (remote == object)) {
            object->RemoveDeathRecipient(pimpl->deathRecipient_);
            pimpl->serviceProxy_ = nullptr;
            HILOG_DEBUG("[%{public}s] Reset OK", __func__);
        }
    }
}

int AccessibilitySystemAbilityClient::RegisterElementOperator(
    const int windowId, const shared_ptr<AccessibilityElementOperator>& operation, int user)
{
    HILOG_INFO("[%{public}s]", __func__);
    if (!operation) {
        HILOG_ERROR("[%{public}s] Input operation is null", __func__);
        return -1;
    }
    int result = 0;
    connectionWindowId_ = windowId;
    for (auto iter = interactionOperators_.begin(); iter != interactionOperators_.end(); iter++) {
        if (iter->first == windowId) {
            HILOG_ERROR("[%{public}s] windowID[%{public}d] is exited", __func__, windowId);
            return result;
        }
    }
    interactionOperators_.insert(pair<int, shared_ptr<AccessibilityElementOperator>>(windowId, operation));
    if (operation != nullptr) {
        interactionOperator_ = operation;
        sptr<AccessibilityElementOperatorStub> aamsInteractionOperator = new AccessibilityElementOperatorStub();
        aamsInteractionOperator->SetWindowId(windowId);
        auto proxyService = pimpl->GetService();
        if (proxyService == nullptr) {
            HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
            return -1;
        }
        proxyService->RegisterElementOperator(windowId, aamsInteractionOperator, user);
    } else {
        HILOG_DEBUG("AccessibilitySystemAbilityClient had register AccessibilityElementOperator.");
    }

    return result;
}

void AccessibilitySystemAbilityClient::DeregisterElementOperator(const int windowId)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return;
    }
    proxyService->DeregisterElementOperator(windowId);
    for (auto iter = interactionOperators_.begin(); iter != interactionOperators_.end(); iter++) {
        if (iter->first == windowId) {
            HILOG_DEBUG("[%{public}s] windowID[%{public}d] is erase", __func__, windowId);
            interactionOperator_ = nullptr;
            connectionWindowId_ = -1;
            interactionOperators_.erase(iter);
            return;
        }
    }
    HILOG_DEBUG("[%{public}s] Not find windowID[%{public}d]", __func__, windowId);
}

shared_ptr<AccessibilityElementOperator> AccessibilitySystemAbilityClient::GetOperatorObject(int windowId)
{
    HILOG_DEBUG("[%{public}s] windowId[%{public}d]", __func__, windowId);
    for (auto it = interactionOperators_.begin(); it != interactionOperators_.end(); it++) {
        if (it->second != nullptr && it->first == windowId) {
            HILOG_DEBUG("[%{public}s] find interaction object windowId[%{public}d]", __func__, windowId);
            return it->second;
        }
    }
    HILOG_DEBUG("[%{public}s] Failed to get interaction", __func__);
    return nullptr;
}

bool AccessibilitySystemAbilityClient::IsEnabled()
{
    HILOG_DEBUG("[%{public}s] isEnabled_[%{public}d]", __func__, isEnabled_);
    return isEnabled_;
}

bool AccessibilitySystemAbilityClient::IsTouchExplorationEnabled()
{
    HILOG_DEBUG("[%{public}s] isEnabled_[%{public}d]", __func__, isTouchExplorationEnabled_);
    return isTouchExplorationEnabled_;
}

bool AccessibilitySystemAbilityClient::IsCaptionEnabled()
{
    HILOG_DEBUG("[%{public}s] isEnabled_[%{public}d]", __func__, isCaptionEnabled_);
    return isCaptionEnabled_;
}

std::vector<AccessibilityAbilityInfo> AccessibilitySystemAbilityClient::GetAbilityList(
    const int accessibilityAbilityTypes, const AbilityStateType stateType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
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
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr || !check) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        std::vector<AccessibilityAbilityInfo> infos;
        return infos;
    }
    return (proxyService->GetAbilityList(accessibilityAbilityTypes, stateType));
}

shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance(
    const Context& abilityContext)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (instance_ == nullptr) {
        int accountId = 100;    // temp deal
        instance_ = std::make_shared<AccessibilitySystemAbilityClient>(abilityContext, accountId);
    } else {
        HILOG_DEBUG("IAccessibleAbilityManagerServiceClient had construct instance");
    }

    return instance_;
}

shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    AbilityContext abilityContext = {};
    if (instance_ == nullptr) {
        int accountId = 100;    // temp deal
        instance_ = std::make_shared<AccessibilitySystemAbilityClient>(abilityContext, accountId);
    } else {
        HILOG_DEBUG("IAccessibleAbilityManagerServiceClient had construct instance");
    }

    return instance_;
}

CaptionProperty AccessibilitySystemAbilityClient::GetCaptionProperty() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    CaptionProperty cp;
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return cp;
    }
    return proxyService->GetCaptionProperty();
}

bool AccessibilitySystemAbilityClient::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (captionProperty_.GetFontScale() != caption.GetFontScale() ||
        (strcmp(captionProperty_.GetFontColor().c_str(), caption.GetFontColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontColor().c_str(), caption.GetFontColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontFamily().c_str(), caption.GetFontFamily().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontEdgeType().c_str(), caption.GetFontEdgeType().c_str()) != 0) ||
        (strcmp(captionProperty_.GetBackgroundColor().c_str(), caption.GetBackgroundColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetWindowColor().c_str(), caption.GetWindowColor().c_str()) != 0)) {
        captionProperty_ = caption;
        NotifyCaptionChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetCaptionPropertyTojson(const CaptionProperty& caption)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (captionProperty_.GetFontScale() != caption.GetFontScale() ||
        (strcmp(captionProperty_.GetFontColor().c_str(), caption.GetFontColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontColor().c_str(), caption.GetFontColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontFamily().c_str(), caption.GetFontFamily().c_str()) != 0) ||
        (strcmp(captionProperty_.GetFontEdgeType().c_str(), caption.GetFontEdgeType().c_str()) != 0) ||
        (strcmp(captionProperty_.GetBackgroundColor().c_str(), caption.GetBackgroundColor().c_str()) != 0) ||
        (strcmp(captionProperty_.GetWindowColor().c_str(), caption.GetWindowColor().c_str()) != 0)) {
        auto proxyService = pimpl->GetService();
        if (proxyService == nullptr) {
            HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
            return false;
        }
        proxyService->SetCaptionProperty(caption);
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetCaptionState(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (isCaptionEnabled_ != state) {
        isCaptionEnabled_ = state;
        NotifyCaptionStateChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetCaptionStateTojson(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    bool ret = false;
    if (isCaptionEnabled_ != state) {
        auto proxyService = pimpl->GetService();
        if (proxyService == nullptr) {
            HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
            return false;
        }
        ret = proxyService->SetCaptionState(state);
    }

    return ret;
}

bool AccessibilitySystemAbilityClient::SetEnabled(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (isEnabled_ != state) {
        isEnabled_ = state;
        NotifyAccessibilityStateChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::IsAccessibilityCaptionEnabled() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return true;
}

bool AccessibilitySystemAbilityClient::CheckEventType(EventType eventType)
{
    if ((eventType < EventType::TYPE_VIEW_CLICKED_EVENT) ||
        ((eventType >= EventType::TYPE_MAX_NUM) && (eventType != EventType::TYPES_ALL_MASK))) {
        HILOG_ERROR("[%{public}s] event type is invalid", __func__);
        return false;
    } else {
        return true;
    }
}

bool AccessibilitySystemAbilityClient::CheckActionType(ActionType actionType)
{
    if ((actionType < ActionType::ACCESSIBILITY_ACTION_FOCUS) ||
        ((actionType > ActionType::ACCESSIBILITY_ACTION_DELETED) &&
         (actionType != ActionType::ACCESSIBILITY_ACTION_TYPE_MASK))) {
        HILOG_ERROR("[%{public}s] action type is invalid", __func__);
        return false;
    } else {
        return true;
    }
}

bool AccessibilitySystemAbilityClient::SendEvent(const EventType eventType, const int componentId)
{
    HILOG_DEBUG("[%{public}s] componentId[%{public}d], eventType[%{public}d]", __func__, componentId, eventType);
    if (!CheckEventType(eventType)) {
        return false;
    }
    AccessibilityEventInfo event;
    event.SetEventType(eventType);
    event.SetSource(componentId);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    proxyService->SendEvent(event, accountId_);
    return true;
}

bool AccessibilitySystemAbilityClient::SendEvent(const AccessibilityEventInfo& event)
{
    HILOG_DEBUG("[%{public}s] EventType[%{public}d]", __func__, event.GetEventType());
    if (!CheckEventType(event.GetEventType())) {
        return false;
    }
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    proxyService->SendEvent(event, accountId_);
    return true;
}

bool AccessibilitySystemAbilityClient::SubscribeStateObserver(
    const shared_ptr<AccessibilityStateObserver>& observer, const int eventType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (eventType != AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_CAPTION_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED) {
        HILOG_ERROR("[%{public}s] Input eventType is out of scope", __func__);
        return false;
    }
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    AccessibilityStateEventType et = AccessibilityStateEventType(*(const_cast<int*>(&eventType)));
    shared_ptr<AccessibilityStateObserver> ob = const_cast<shared_ptr<AccessibilityStateObserver>&>(observer);
    switch (et) {
        case AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED:
            observersAccessibilityState_.push_back(ob);
            break;
        case AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED:
            observersTouchState_.push_back(ob);
            break;
        case AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED:
            observersKeyEventState_.push_back(ob);
            break;
        case AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED:
            observersGestureState_.push_back(ob);
            break;
        default:
            HILOG_ERROR("the EventType observed is not supported");
            return false;
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
    return true;
}

bool AccessibilitySystemAbilityClient::UnsubscribeStateObserver(
    const shared_ptr<AccessibilityStateObserver>& observer, const int eventType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (eventType != AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_CAPTION_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED) {
        HILOG_ERROR("[%{public}s] Input eventType is out of scope", __func__);
        return false;
    }
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    std::vector<std::shared_ptr<AccessibilityStateObserver>> observerTable;
    AccessibilityStateEventType et = AccessibilityStateEventType(*(const_cast<int*>(&eventType)));
    switch (et) {
        case AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED:
            observerTable = observersAccessibilityState_;
            break;
        case AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED:
            observerTable = observersTouchState_;
            break;
        case AccessibilityStateEventType::EVENT_KEVEVENT_STATE_CHANGED:
            observerTable = observersKeyEventState_;
            break;
        case AccessibilityStateEventType::EVENT_GESTURE_STATE_CHANGED:
            observerTable = observersGestureState_;
            break;
        default:
            HILOG_ERROR("the EventType observed is not supported");
            return false;
    }
    for (auto it = observerTable.begin(); it != observerTable.end(); it++) {
        if (*it == observer) {
            observerTable.erase(it);
            return true;
        }
    }
    HILOG_DEBUG("%{public}s Not find eventType[%{public}d]", __func__, eventType);
    return false;
}

bool AccessibilitySystemAbilityClient::UnsubscribeStateObserver(const shared_ptr<AccessibilityStateObserver>& observer)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    bool result = false;
    for (auto it = observersAccessibilityState_.begin(); it != observersAccessibilityState_.end(); it++) {
        if (*it == observer) {
            observersAccessibilityState_.erase(it);
            result = true;
            break;
        }
    }

    for (auto it = observersTouchState_.begin(); it != observersTouchState_.end(); it++) {
        if (*it == observer) {
            observersTouchState_.erase(it);
            result = true;
            break;
        }
    }

    for (auto it = observersKeyEventState_.begin(); it != observersKeyEventState_.end(); it++) {
        if (*it == observer) {
            observersKeyEventState_.erase(it);
            result = true;
            break;
        }
    }

    for (auto it = observersGestureState_.begin(); it != observersGestureState_.end(); it++) {
        if (*it == observer) {
            observersGestureState_.erase(it);
            result = true;
            break;
        }
    }

    return result;
}

void AccessibilitySystemAbilityClient::UpdateEnabled(const bool enabled)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (isEnabled_ != enabled) {
        isEnabled_ = enabled;
        NotifyAccessibilityStateChanged();
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::UpdateTouchExplorationEnabled(const bool enabled)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (isTouchExplorationEnabled_ != enabled) {
        isTouchExplorationEnabled_ = enabled;
        NotifyTouchExplorationStateChanged();
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::SetCaptionEnabled(const bool enabled)
{
    HILOG_DEBUG("%{public}s", __func__);
    if (isCaptionEnabled_ != enabled) {
        isCaptionEnabled_ = enabled;
        NotifyCaptionStateChanged();
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyAccessibilityStateChanged()
{
    HILOG_DEBUG("%{public}s", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersAccessibilityState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersAccessibilityState_ is null", __func__);
        return;
    }
    for (auto it = observersAccessibilityState_.begin(); it != observersAccessibilityState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_ACCESSIBILITY_STATE_CHANGED);
        stateEvent.SetEventResult(isEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersAccessibilityState_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyTouchExplorationStateChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersTouchState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersTouchState_ is null", __func__);
        return;
    }
    for (auto it = observersTouchState_.begin(); it != observersTouchState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_TOUCH_GUIDE_STATE_CHANGED);
        stateEvent.SetEventResult(isTouchExplorationEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersTouchState_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyCaptionStateChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersCaptionEnable_.size() == 0) {
        HILOG_DEBUG("%{public}s observersCaptionEnable_ is null", __func__);
        return;
    }
    for (auto it = observersCaptionEnable_.begin(); it != observersCaptionEnable_.end(); it++) {
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnCaptionStateChanged(isCaptionEnabled_);
        } else {
            HILOG_ERROR("%{public}s end observersCaptionEnable_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

bool AccessibilitySystemAbilityClient::AddCaptionListener(const std::shared_ptr<CaptionObserver>& ob, const int type)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    bool result = true;
    if (type == CaptionObserverType::CAPTION_ENABLE) {
        if (observersCaptionEnable_.size() == 0) {
            observersCaptionEnable_.push_back(ob);
        }
    } else if (type == CaptionObserverType::CAPTION_PROPERTY) {
        if (observersCaptionProperty_.size() == 0) {
            observersCaptionProperty_.push_back(ob);
        }
    } else {
        result = false;
        HILOG_ERROR("%{public}s Type Error ", __func__);
    }

    return result;
}

bool AccessibilitySystemAbilityClient::DeleteCaptionListener(
    const std::shared_ptr<CaptionObserver>& ob, const int type)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    bool result = false;
    if (type == CaptionObserverType::CAPTION_ENABLE) {
        for (auto it = observersCaptionEnable_.begin(); it != observersCaptionEnable_.end(); ++it) {
            if (*it == ob) {
                observersCaptionEnable_.erase(it);
                result = true;
                break;
            }
        }
    } else if (type == CaptionObserverType::CAPTION_PROPERTY) {
        for (auto it = observersCaptionProperty_.begin(); it != observersCaptionProperty_.end(); ++it) {
            if (*it == ob) {
                observersCaptionProperty_.erase(it);
                result = true;
                break;
            }
        }
    } else {
        HILOG_ERROR("%{public}s Type Error ", __func__);
    }

    return result;
}

void AccessibilitySystemAbilityClient::UpdatecaptionProperty(const CaptionProperty& property)
{
    HILOG_DEBUG("%{public}s", __func__);
    captionProperty_ = property;
    NotifyCaptionChanged();
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyCaptionChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersCaptionProperty_.size() == 0) {
        HILOG_DEBUG("%{public}s observersCaptionProperty_ is null", __func__);
        return;
    }
    for (auto it = observersCaptionProperty_.begin(); it != observersCaptionProperty_.end(); ++it) {
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnCaptionPropertyChanged(captionProperty_);
        } else {
            HILOG_ERROR("%{public}s end observersCaptionProperty_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

bool AccessibilitySystemAbilityClient::GetEnabledState()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    return proxyService->GetEnabledState();
}

bool AccessibilitySystemAbilityClient::GetCaptionState()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }

    isCaptionEnabled_ = proxyService->GetCaptionState();
    return isCaptionEnabled_;
}

bool AccessibilitySystemAbilityClient::GetTouchGuideState()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    return proxyService->GetTouchGuideState();
}

bool AccessibilitySystemAbilityClient::GetGestureState()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    return proxyService->GetGestureState();
}

bool AccessibilitySystemAbilityClient::GetKeyEventObserverState()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    return proxyService->GetKeyEventObserverState();
}

bool AccessibilitySystemAbilityClient::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (isTouchExplorationEnabled_ != state) {
        isTouchExplorationEnabled_ = state;
        NotifyTouchExplorationStateChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetGestureState(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (isGesturesSimulationEnabled_ != state) {
        isGesturesSimulationEnabled_ = state;
        NotifyGestureStateChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (isFilteringKeyEventsEnabled_ != state) {
        isFilteringKeyEventsEnabled_ = state;
        NotifyKeyEventStateChanged();
    }
    return true;
}

bool AccessibilitySystemAbilityClient::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    proxyService->SetEnabledObj(it);
    return true;
}

std::map<std::string, AppExecFwk::ElementName> AccessibilitySystemAbilityClient::GetEnabledAbilities()
{
    HILOG_DEBUG("[%{public}s]", __func__);

    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        std::map<std::string, AppExecFwk::ElementName> it;
        return it;
    }
    return proxyService->GetEnabledAbilities();
}

std::vector<AccessibilityAbilityInfo> AccessibilitySystemAbilityClient::GetInstalledAbilities()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        std::vector<AccessibilityAbilityInfo> it;
        return it;
    }
    return proxyService->GetInstalledAbilities();
}

void AccessibilitySystemAbilityClient::NotifyGestureStateChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersGestureState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersGestureState_ is null", __func__);
        return;
    }
    for (auto it = observersGestureState_.begin(); it != observersGestureState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_GESTURE_STATE_CHANGED);
        stateEvent.SetEventResult(isGesturesSimulationEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersGestureState_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyKeyEventStateChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersKeyEventState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersKeyEventState_ is null", __func__);
        return;
    }
    for (auto it = observersKeyEventState_.begin(); it != observersKeyEventState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_KEVEVENT_STATE_CHANGED);
        stateEvent.SetEventResult(isFilteringKeyEventsEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersKeyEventState_ is null", __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

bool AccessibilitySystemAbilityClient::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return false;
    }
    proxyService->DisableAbilities(it);

    return true;
}

int AccessibilitySystemAbilityClient::GetActiveWindow()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return INVALID_WINDOW_ID;
    }

    return proxyService->GetActiveWindow();
}
}  // namespace Accessibility
}  // namespace OHOS