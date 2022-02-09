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
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_interaction_operation.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_interaction_operation_stub.h"
#include "dummy.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
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

        void OnRemoteDied(const wptr<IRemoteObject>& remote) {
            AccessibilitySystemAbilityClient::GetInstance()->ResetService(remote);
        }
    };
    sptr<IRemoteObject::DeathRecipient> deathRecipient_{};
    sptr<AccessibleAbilityManagerServiceStateStub> stateCallback_{};
    sptr<AccessibleAbilityManagerServiceClientProxy> serviceProxy_{};
    sptr<IAccessibleAbilityManagerServiceClient> GetService() {
        if (serviceProxy_ != nullptr) {
            return serviceProxy_;
        }

        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            HILOG_ERROR("[%s] Failed to get ISystemAbilityManager", __func__);
            return nullptr;
        }

        sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBLE_ABILITY_MANAGER_SERVICE_ID);
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

AccessibilitySystemAbilityClient::AccessibilitySystemAbilityClient(const Context &context,
    int accountId) : pimpl(std::make_unique<Impl>())
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

}

void AccessibilitySystemAbilityClient::Interrupt()   //Remained: target app called
{
    HILOG_DEBUG("AccessibilitySystemAbilityClient Interrupt");

    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return;
    }
    proxyService->Interrupt(accountId_);

    HILOG_DEBUG("AccessibilitySystemAbilityClient Interrupt OK");
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

int AccessibilitySystemAbilityClient::RegisterInteractionOperation(const int windowId,
        const shared_ptr<AccessibilityInteractionOperation> &operation, int user)
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
    interactionOperators_.insert(pair<int, shared_ptr<AccessibilityInteractionOperation>>(windowId, operation));
    if (operation != nullptr) {
        interactionOperator_ = operation;
        sptr<AccessibilityInteractionOperationStub> aamsInteractionOperator =
            new AccessibilityInteractionOperationStub();
        aamsInteractionOperator->SetWindowId(windowId);
        auto proxyService = pimpl->GetService();
        if (proxyService == nullptr) {
            HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
            return -1;
        }
        proxyService->RegisterInteractionOperation(windowId, aamsInteractionOperator, user);
    } else {
        HILOG_DEBUG("AccessibilitySystemAbilityClient had register AccessibilityInteractionOperation.");
    }

    return result;
}

void AccessibilitySystemAbilityClient::DeregisterInteractionOperation(const int windowId)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return;
    }
    proxyService->DeregisterInteractionOperation(windowId);
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

shared_ptr<AccessibilityInteractionOperation> AccessibilitySystemAbilityClient::GetInteractionObject(int windowId)
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
    const Context &abilityContext)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (instance_ == nullptr) {
        int accountId = 0;
        instance_ = std::make_shared<AccessibilitySystemAbilityClient>(abilityContext, accountId);
    } else {
        HILOG_DEBUG("IAccessibleAbilityManagerServiceClient had construct instance");
    }

    return instance_;
}

shared_ptr<AccessibilitySystemAbilityClient> AccessibilitySystemAbilityClient::GetInstance()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    AbilityContext abilityContext {};
    if (instance_ == nullptr) {
        int accountId = 0;
        instance_ = std::make_shared<AccessibilitySystemAbilityClient>(abilityContext, accountId);
    } else {
        HILOG_DEBUG("IAccessibleAbilityManagerServiceClient had construct instance");
    }

    return instance_;
}

int AccessibilitySystemAbilityClient::GetSuggestedInterval(const int timeout, const int contentType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    int interval = 0;
    int64_t timeSuggested = 0;
    auto proxyService = pimpl->GetService();
    if (proxyService == nullptr) {
        HILOG_ERROR("[%{public}s] Failed to get aams service", __func__);
        return -1;
    }
    timeSuggested = proxyService->GetSuggestedInterval();
    if (contentType == CONTENT_CONTROLS) {
        interval = timeSuggested;
    } else {
        interval = timeSuggested >> NUM_INT32;
    }

    if (interval < timeout) {
        interval = timeout;
    }
    return interval;
}

CaptionProperties AccessibilitySystemAbilityClient::GetAccessibilityCaptionProperties() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return captionProperties_;
}

bool AccessibilitySystemAbilityClient::IsAccessibilityCaptionEnabled() const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    return true;
}

bool AccessibilitySystemAbilityClient::CheckEventType(EventType eventType)
{
    if ((eventType & EventType::TYPE_VIEW_CLICKED_EVENT) != EventType::TYPE_VIEW_CLICKED_EVENT &&
        (eventType & EventType::TYPE_VIEW_LONG_CLICKED_EVENT) != EventType::TYPE_VIEW_LONG_CLICKED_EVENT &&
        (eventType & EventType::TYPE_VIEW_SELECTED_EVENT) != EventType::TYPE_VIEW_SELECTED_EVENT &&
        (eventType & EventType::TYPE_VIEW_FOCUSED_EVENT) != EventType::TYPE_VIEW_FOCUSED_EVENT &&
        (eventType & EventType::TYPE_VIEW_TEXT_UPDATE_EVENT) != EventType::TYPE_VIEW_TEXT_UPDATE_EVENT &&
        (eventType & EventType::TYPE_PAGE_STATE_UPDATE) != EventType::TYPE_PAGE_STATE_UPDATE &&
        (eventType & EventType::TYPE_NOTIFICATION_UPDATE_EVENT) != EventType::TYPE_NOTIFICATION_UPDATE_EVENT &&
        (eventType & EventType::TYPE_VIEW_HOVER_ENTER_EVENT) != EventType::TYPE_VIEW_HOVER_ENTER_EVENT &&
        (eventType & EventType::TYPE_VIEW_HOVER_EXIT_EVENT) != EventType::TYPE_VIEW_HOVER_EXIT_EVENT &&
        (eventType & EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN) != EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN &&
        (eventType & EventType::TYPE_TOUCH_GUIDE_GESTURE_END) != EventType::TYPE_TOUCH_GUIDE_GESTURE_END &&
        (eventType & EventType::TYPE_PAGE_CONTENT_UPDATE) != EventType::TYPE_PAGE_CONTENT_UPDATE &&
        (eventType & EventType::TYPE_VIEW_SCROLLED_EVENT) != EventType::TYPE_VIEW_SCROLLED_EVENT &&
        (eventType & EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT) !=
            EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT &&
        (eventType & EventType::TYPE_PUBLIC_NOTICE_EVENT) != EventType::TYPE_PUBLIC_NOTICE_EVENT &&
        (eventType & EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT) !=
            EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT &&
        (eventType & EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT) !=
            EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT &&
        (eventType & EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT) != EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT &&
        (eventType & EventType::TYPE_TOUCH_GUIDE_BEGIN) != EventType::TYPE_TOUCH_GUIDE_BEGIN &&
        (eventType & EventType::TYPE_TOUCH_GUIDE_END) != EventType::TYPE_TOUCH_GUIDE_END &&
        (eventType & EventType::TYPE_TOUCH_BEGIN) != EventType::TYPE_TOUCH_BEGIN &&
        (eventType & EventType::TYPE_TOUCH_END) != EventType::TYPE_TOUCH_END &&
        (eventType & EventType::TYPE_WINDOW_UPDATE) != EventType::TYPE_WINDOW_UPDATE &&
        (eventType & EventType::TYPES_ALL_MASK) != EventType::TYPES_ALL_MASK) {
        HILOG_ERROR("[%{public}s] event type is invalid", __func__);
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

bool AccessibilitySystemAbilityClient::SendEvent(const AccessibilityEventInfo &event)
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

bool AccessibilitySystemAbilityClient::SubscribeStateObserver(const shared_ptr<AccessibilityStateObserver> &observer,
    const int eventType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (eventType != AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED) {
        HILOG_ERROR("[%{public}s] Input eventType is out of scope", __func__);
        return false;
    }
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    AccessibilityStateEventType et = AccessibilityStateEventType(
                                                        *(const_cast<int*>(&eventType)));
    shared_ptr<AccessibilityStateObserver> ob = const_cast<shared_ptr<AccessibilityStateObserver> &>(observer);
    if (et == AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED) {
        observersAceessibilityState_.push_back(ob);
    } else {
        observersTouchState_.push_back(ob);
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
    return true;
}

bool AccessibilitySystemAbilityClient::UnsubscribeStateObserver(const shared_ptr<AccessibilityStateObserver> &observer,
    const int eventType)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (eventType != AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED &&
        eventType != AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED) {
        HILOG_ERROR("[%{public}s] Input eventType is out of scope", __func__);
        return false;
    }
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    AccessibilityStateEventType et = AccessibilityStateEventType(*(const_cast<int*>(&eventType)));
    if (et == AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED) {
        for (auto it = observersAceessibilityState_.begin(); it != observersAceessibilityState_.end(); it++) {
            if (*it == observer) {
                observersAceessibilityState_.erase(it);
                return true;
            }
        }
    } else {
        for (auto it = observersTouchState_.begin(); it != observersTouchState_.end(); it++) {
            if (*it == observer) {
                observersTouchState_.erase(it);
                return true;
            }
        }
    }
    HILOG_DEBUG("%{public}s Not find eventType[%{public}d]" , __func__, eventType);
    return false;
}

bool AccessibilitySystemAbilityClient::UnsubscribeStateObserver(const shared_ptr<AccessibilityStateObserver> &observer)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    if (!observer) {
        HILOG_ERROR("[%{public}s] Input observer is null", __func__);
        return false;
    }
    bool result = false;
    for (auto it = observersAceessibilityState_.begin(); it != observersAceessibilityState_.end(); it++) {
        if (*it == observer) {
            observersAceessibilityState_.erase(it);
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
    return result;
}

void AccessibilitySystemAbilityClient::SetEnabled(const bool enabled)
{
    HILOG_DEBUG("%{public}s" , __func__);
    isEnabled_ = enabled;
    NotifyAccessibilityStateChanged();
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::SetTouchExplorationEnabled(const bool enabled)
{
    HILOG_DEBUG("%{public}s" , __func__);
    isTouchExplorationEnabled_ = enabled;
    NotifyTouchExplorationStateChanged();
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyAccessibilityStateChanged()
{
    HILOG_DEBUG("%{public}s" , __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersAceessibilityState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersAceessibilityState_ is null" , __func__);
        return;
    }
    for (auto it = observersAceessibilityState_.begin(); it != observersAceessibilityState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_ACCESSIBILITY_STATE_CHANGED);
        stateEvent.SetEventResult(isEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersAceessibilityState_ is null" , __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

void AccessibilitySystemAbilityClient::NotifyTouchExplorationStateChanged()
{
    HILOG_DEBUG("[%{public}s]", __func__);
    std::lock_guard<std::recursive_mutex> lock(asacProxyLock_);
    if (observersTouchState_.size() == 0) {
        HILOG_DEBUG("%{public}s observersTouchState_ is null" , __func__);
        return;
    }
    for (auto it = observersTouchState_.begin(); it != observersTouchState_.end(); it++) {
        AccessibilityStateEvent stateEvent;
        stateEvent.SetEventType(EVENT_TOUCH_GUIDE_STATE_CHANGED);
        stateEvent.SetEventResult(isTouchExplorationEnabled_);
        if (*it != nullptr && it->get() != nullptr) {
            it->get()->OnStateChanged(stateEvent);
        } else {
            HILOG_ERROR("%{public}s end observersTouchState_ is null" , __func__);
        }
    }
    HILOG_DEBUG("[%{public}s] end", __func__);
}

bool CaptionProperties::CheckProperty(const std::string &property)
{
    bool result = true;
    return result;
}

int CaptionProperties::GetBackgroundColor() const
{
    return backgroundColor_;
}

int CaptionProperties::GetForegroundColor() const
{
    return foregroundColor_;
}

int CaptionProperties::GetEdgeType() const
{
    return edgeType_;
}

int CaptionProperties::GetEdgeColor() const
{
    return edgeColor_;
}

int CaptionProperties::GetWindowColor() const
{
    return windowColor_;
}

bool CaptionProperties::HasBackgroundColor()
{
    return hasBackgroundColor_;
}

bool CaptionProperties::HasForegroundColor()
{
    return hasForegroundColor_;
}

bool CaptionProperties::HasEdgeType()
{
    return hasEdgeType_;
}

bool CaptionProperties::HasEdgeColor()
{
    return hasEdgeColor_;
}

bool CaptionProperties::HasWindowColor()
{
    return hasWindowColor_;
}

} //namespace Accessibility
} //namespace OHOS