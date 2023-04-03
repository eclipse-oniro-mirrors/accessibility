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

#include "accessible_ability_manager_service.h"

#include <new>
#include <unistd.h>
#include <functional>
#include <hitrace_meter.h>

#include "ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "display_power_mgr_client.h"
#include "hilog_wrapper.h"
#include "input_manager.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "utils.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
    const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    constexpr int32_t QUERY_USER_ID_RETRY_COUNT = 600;
    constexpr int32_t QUERY_USER_ID_SLEEP_TIME = 50;
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<AccessibleAbilityManagerService>::GetInstance());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
    dependentServicesStatus_[ABILITY_MGR_SERVICE_ID] = false;
    dependentServicesStatus_[BUNDLE_MGR_SERVICE_SYS_ABILITY_ID] = false;
    dependentServicesStatus_[COMMON_EVENT_SERVICE_ID] = false;
    dependentServicesStatus_[DISPLAY_MANAGER_SERVICE_SA_ID] = false;
    dependentServicesStatus_[SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN] = false;
    dependentServicesStatus_[WINDOW_MANAGER_SERVICE_ID] = false;
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
    HILOG_INFO("AccessibleAbilityManagerService::~AccessibleAbilityManagerService");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME);
        if (!runner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS runner failed");
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AAMSEventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS event handler failed");
            return;
        }
    }
    SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");

    HILOG_DEBUG("AddAbilityListener!");
    AddSystemAbilityListener(ABILITY_MGR_SERVICE_ID);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    AddSystemAbilityListener(DISPLAY_MANAGER_SERVICE_SA_ID);
    AddSystemAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN);
    AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");
    if (!handler_) {
        HILOG_ERROR("AccessibleAbilityManagerService::OnStop failed!");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();

        Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
        Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
        Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();

        currentAccountId_ = -1;
        a11yAccountsData_.clear();
        stateCallbacks_.clear();
        bundleManager_ = nullptr;
        inputInterceptor_ = nullptr;
        touchEventInjector_ = nullptr;
        keyEventFilter_ = nullptr;
        stateCallbackDeathRecipient_ = nullptr;
        bundleManagerDeathRecipient_ = nullptr;

        syncPromise.set_value();
        }), "TASK_ONSTOP");
    syncFuture.wait();

    runner_.reset();
    handler_.reset();
    for (auto &iter : dependentServicesStatus_) {
        iter.second = false;
    }

    isReady_ = false;
    isPublished_ = false;
    SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
    HILOG_INFO("AccessibleAbilityManagerService::OnStop OK.");
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d added!", systemAbilityId);
    if (!handler_) {
        HILOG_DEBUG("Event handler is nullptr.");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        auto iter = dependentServicesStatus_.find(systemAbilityId);
        if (iter == dependentServicesStatus_.end()) {
            HILOG_ERROR("SystemAbilityId is not found!");
            return;
        }

        dependentServicesStatus_[systemAbilityId] = true;
        if (std::any_of(dependentServicesStatus_.begin(), dependentServicesStatus_.end(),
            [](const std::map<int32_t, bool>::value_type &status) { return !status.second; })) {
            HILOG_DEBUG("Not all the dependence is ready!");
            return;
        }

        if (Init() == false) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed!");
            return;
        }

        if (!isPublished_) {
            if (Publish(this) == false) {
                HILOG_ERROR("AccessibleAbilityManagerService::Publish failed!");
                return;
            }
            isPublished_ = true;
        }

        isReady_ = true;
        SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "true");
        HILOG_DEBUG("AAMS is ready!");
        }), "OnAddSystemAbility");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d removed!", systemAbilityId);
    if (!handler_) {
        HILOG_DEBUG("Event handler is nullptr.");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        auto iter = dependentServicesStatus_.find(systemAbilityId);
        if (iter == dependentServicesStatus_.end()) {
            HILOG_ERROR("SystemAbilityId is not found!");
            return;
        }

        dependentServicesStatus_[systemAbilityId] = false;
        if (isReady_) {
            SwitchedUser(-1);
            Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
            Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
            Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();
            Singleton<AccessibilityWindowManager>::GetInstance().DeInit();

            isReady_ = false;
            SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
        }
        }), "OnRemoveSystemAbility");
}

int AccessibleAbilityManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    HILOG_DEBUG("dump AccessibilityManagerServiceInfo");
    if (!accessibilityDumper_) {
        accessibilityDumper_ = new(std::nothrow) AccessibilityDumper();
        if (!accessibilityDumper_) {
            HILOG_ERROR("accessibilityDumper_ is nullptr");
            return -1;
        }
    }
    return accessibilityDumper_->Dump(fd, args);
}

RetError AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d]", uiEvent.GetEventType(), uiEvent.GetGestureType());
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }

    UpdateAccessibilityWindowStateByEvent(uiEvent);
    handler_->PostTask(std::bind([this](AccessibilityEventInfo &event) -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }

        event.SetTimeStamp(Utils::GetSystemTime());
        map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
        for (auto &ability : abilities) {
            if (ability.second) {
                ability.second->OnAccessibilityEvent(event);
            }
        }
        }, uiEvent), "TASK_SEND_EVENT");
    return RET_OK;
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed!");
        return 0;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG();
        if (!stateCallbackDeathRecipient_) {
            stateCallbackDeathRecipient_ = new(std::nothrow) StateCallbackDeathRecipient();
            if (!stateCallbackDeathRecipient_) {
                HILOG_ERROR("stateCallbackDeathRecipient_ is null");
                syncPromise.set_value(0);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise.set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(stateCallbackDeathRecipient_);
        auto iter = std::find(stateCallbacks_.begin(), stateCallbacks_.end(), callback);
        if (iter == stateCallbacks_.end()) {
            stateCallbacks_.push_back(callback);
            HILOG_INFO("RegisterStateObserver successfully");
        }

        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value(0);
            return;
        }
        uint32_t state = accountData->GetAccessibilityState();
        syncPromise.set_value(state);
        }), "TASK_REGISTER_STATE_OBSERVER");

    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed! callback:%{public}p, handler:%{public}p",
            callback.GetRefPtr(), handler_.get());
        return ERR_INVALID_VALUE;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value(ERR_INVALID_VALUE);
            return;
        }
        if (!captionPropertyCallbackDeathRecipient_) {
            captionPropertyCallbackDeathRecipient_ = new(std::nothrow) CaptionPropertyCallbackDeathRecipient();
            if (!captionPropertyCallbackDeathRecipient_) {
                HILOG_ERROR("captionPropertyCallbackDeathRecipient_ is null");
                syncPromise.set_value(ERR_INVALID_VALUE);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise.set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
        accountData->AddCaptionPropertyCallback(callback);
        HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
            accountData->GetCaptionPropertyCallbacks().size());
        syncPromise.set_value(NO_ERROR);
        }), "TASK_REGISTER_CAPTION_OBSERVER");
    return syncFuture.get();
}

void AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    if (!observer || !handler_) {
        HILOG_ERROR("Parameters check failed!");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, observer]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value();
            return;
        }
        if (!enableAbilityListsObserverDeathRecipient_) {
            enableAbilityListsObserverDeathRecipient_ = new(std::nothrow) EnableAbilityListsObserverDeathRecipient();
            if (!enableAbilityListsObserverDeathRecipient_) {
                HILOG_ERROR("enableAbilityListsObserverDeathRecipient_ is null");
                syncPromise.set_value();
                return;
            }
        }
        if (!observer->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise.set_value();
            return;
        }
        observer->AsObject()->AddDeathRecipient(enableAbilityListsObserverDeathRecipient_);
        accountData->AddEnableAbilityListsObserver(observer);
        syncPromise.set_value();
        }), "TASK_REGISTER_ENABLE_ABILITY_LISTS_OBSERVER");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d, handler:%{public}p", stateType, handler_.get());
        return RET_ERR_INVALID_PARAM;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &infos, abilityTypes, stateType]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Get current account data failed!!");
            syncPromise.set_value(RET_ERR_FAILED);
            return;
        }

        vector<AccessibilityAbilityInfo> abilities;
        accountData->GetAbilitiesByState(static_cast<AbilityStateType>(stateType), abilities);
        HILOG_DEBUG("abilityes count is %{public}zu", abilities.size());
        for (auto &ability : abilities) {
            if (abilityTypes == AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL ||
                (ability.GetAccessibilityAbilityType() & abilityTypes)) {
                infos.push_back(ability);
            }
        }
        HILOG_DEBUG("infos count is %{public}zu", infos.size());
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_ABILITY_LIST");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation)
{
    if (!handler_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_INFO("Register windowId[%{public}d]", windowId);
        HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "RegisterElementOperator");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return;
        }
        if (accountData->GetAccessibilityWindowConnection(windowId)) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_WARN("This operation already exists, do not register twice!!");
            return;
        }

        sptr<AccessibilityWindowConnection> connection =
            new(std::nothrow) AccessibilityWindowConnection(windowId, operation, currentAccountId_);
        if (!connection) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
            return;
        }
        accountData->AddAccessibilityWindowConnection(windowId, connection);

        if (operation && operation->AsObject()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient =
                new(std::nothrow) InteractionOperationDeathRecipient(windowId);
            if (!deathRecipient) {
                Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                    A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
                HILOG_ERROR("Create interactionOperationDeathRecipient failed");
                return;
            }

            bool result = operation->AsObject()->AddDeathRecipient(deathRecipient);
            interactionOperationDeathRecipients_[windowId] = deathRecipient;
            HILOG_DEBUG("The result of adding operation's death recipient is %{public}d", result);
        }
        }), "TASK_REGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_INFO("Deregister windowId[%{public}d]", windowId);
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(windowId);
        if (!connection) {
            HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
            return;
        }
        accountData->RemoveAccessibilityWindowConnection(windowId);

        if (!connection->GetProxy()) {
            HILOG_ERROR("proxy is null");
            return;
        }

        auto object = connection->GetProxy()->AsObject();
        if (object) {
            auto iter = interactionOperationDeathRecipients_.find(windowId);
            if (iter != interactionOperationDeathRecipients_.end()) {
                sptr<IRemoteObject::DeathRecipient> deathRecipient = iter->second;
                bool result = object->RemoveDeathRecipient(deathRecipient);
                HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
                interactionOperationDeathRecipients_.erase(iter);
            }
        }
        }), "TASK_DEREGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &caption]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        caption = accountData->GetConfig()->GetCaptionProperty();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CAPTION_PROPERTY");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &caption]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetCaptionProperty(caption);
        syncPromise.set_value(ret);
        UpdateCaptionProperty();
        }), "TASK_SET_CAPTION_PROPERTY");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetCaptionState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        }), "TASK_SET_CAPTION_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetEnabledState();
        syncPromise.set_value(result);
        }), "TASK_GET_ENABLE_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetCaptionState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetCaptionState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CAPTION_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetTouchGuideState();
        syncPromise.set_value(result);
        }), "TASK_GET_TOUCH_GUIDE_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetGestureState();
        syncPromise.set_value(result);
        }), "TASK_GET_GESTURE_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetKeyEventObserverState();
        syncPromise.set_value(result);
        }), "TASK_GET_KEY_EVENT_OBSERVER_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name, &capabilities]() -> void {
        HILOG_DEBUG();
        RetError result = InnerEnableAbility(name, capabilities);
        syncPromise.set_value(result);
        }), "TASK_ENABLE_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::InnerEnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    return accountData->EnableAbility(name, capabilities);
}

RetError AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &enabledAbilities]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        enabledAbilities = accountData->GetEnabledAbilities();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_ENABLE_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG();
        RetError result = InnerDisableAbility(name);
        syncPromise.set_value(result);
        }), "TASK_DISABLE_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::InnerDisableAbility(const std::string &name)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->RemoveEnabledAbility(name);
    if (ret != RET_OK) {
        HILOG_ERROR("RemoveEnabledAbility failed");
        return ret;
    }
    accountData->RemoveConnectingA11yAbility(name);
    accountData->UpdateAbilities();
    return RET_OK;
}

RetError AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, obj]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        std::string uiTestUri = Utils::GetUri(UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
        if (connection) {
            HILOG_ERROR("connection is existed!!");
            syncPromise.set_value(RET_ERR_CONNECTION_EXIST);
            return;
        }

        std::function<void()> addUITestClientFunc = std::bind(&AccessibilityAccountData::AddUITestClient, accountData,
            obj, UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        handler_->PostTask(addUITestClientFunc, "AddUITestClient");
        syncPromise.set_value(RET_OK);
        }), "TASK_ENABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::DisableUITestAbility()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        std::string uiTestUri = Utils::GetUri(UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
        if (!connection) {
            HILOG_ERROR("connection is not existed!!");
            syncPromise.set_value(RET_ERR_NO_CONNECTION);
            return;
        }
        std::function<void()> removeUITestClientFunc =
            std::bind(&AccessibilityAccountData::RemoveUITestClient, accountData, connection, UI_TEST_BUNDLE_NAME);
        handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
        syncPromise.set_value(RET_OK);
        }), "TASK_DISABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return -1;
    }

    std::promise<int32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        syncPromise.set_value(Singleton<AccessibilityWindowManager>::GetInstance().activeWindowId_);
        }), "TASK_GET_ACTIVE_WINDOW");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG();
    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
    bool result = Singleton<AccessibilityWindowManager>::GetInstance().Init();
    HILOG_DEBUG("wms init result is %{public}d", result);

    int32_t retry = QUERY_USER_ID_RETRY_COUNT;
    int32_t sleepTime = QUERY_USER_ID_SLEEP_TIME;
    std::vector<int32_t> accountIds;
    ErrCode ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(accountIds);
    while (ret != ERR_OK || accountIds.size() == 0) {
        HILOG_DEBUG("Query account information failed, left retry count:%{public}d", retry);
        if (retry == 0) {
            HILOG_ERROR("Query account information failed!!!");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(accountIds);
        retry--;
    }

    if (accountIds.size() > 0) {
        HILOG_DEBUG("Query account information success, account id:%{public}d", accountIds[0]);
        SwitchedUser(accountIds[0]);
    }

    return true;
}

void AccessibleAbilityManagerService::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY);
    HILOG_INFO();
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(windowId_);
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    HILOG_DEBUG("currentAccoutId is %{public}d ", currentAccountId_);
    if (currentAccountId_ == -1) {
        HILOG_ERROR("current account id is wrong");
        return nullptr;
    }
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(currentAccountId_);
    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return nullptr;
    }
    a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    return accountData;
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetAccountData(int32_t accountId) const
{
    HILOG_DEBUG("accountId is %{public}d ", accountId);
    auto iter = a11yAccountsData_.find(accountId);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    HILOG_ERROR("There is no account[%{public}d]", accountId);
    return nullptr;
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    HILOG_DEBUG();
    if (bundleManager_) {
        return bundleManager_;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        HILOG_ERROR("failed:fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        HILOG_ERROR("failed:fail to get bundle manager proxy.");
        return nullptr;
    }

    bundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (!bundleManager_) {
        HILOG_ERROR("fail to new bundle manager.");
        return nullptr;
    }

    if (!bundleManagerDeathRecipient_) {
        bundleManagerDeathRecipient_ = new(std::nothrow) BundleManagerDeathRecipient();
        if (!bundleManagerDeathRecipient_) {
            HILOG_ERROR("bundleManagerDeathRecipient_ is null");
            return nullptr;
        }
    }

    bundleManager_->AsObject()->AddDeathRecipient(bundleManagerDeathRecipient_);
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(
    int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void AccessibleAbilityManagerService::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveCallback(STATE_CALLBACK, this, remote);
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveCallback(CAPTION_PROPERTY_CALLBACK, this, remote);
}

void AccessibleAbilityManagerService::EnableAbilityListsObserverDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveCallback(
        ENABLE_ABILITY_LISTS_CALLBACK, this, remote);
}

void AccessibleAbilityManagerService::AddedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);

    // Add this account in map
    auto iter = a11yAccountsData_.find(accountId);
    if (iter != a11yAccountsData_.end()) {
        HILOG_DEBUG("The account[%{public}d] is already exist.", accountId);
    }
    sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return;
    }
    a11yAccountsData_.insert(make_pair(accountId, accountData));

    // Initialize data of this account
    a11yAccountsData_[accountId]->Init();
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);
    if (accountId == currentAccountId_) {
        HILOG_ERROR("Remove user failed, this account is current account.");
        return;
    }

    auto iter = a11yAccountsData_.find(accountId);
    if (iter != a11yAccountsData_.end()) {
        iter->second->GetConfig()->ClearData();
        a11yAccountsData_.erase(iter);
        return;
    }
    HILOG_ERROR("The account[%{public}d] is not exist.", accountId);
}

void AccessibleAbilityManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);

    if (accountId == currentAccountId_) {
        HILOG_WARN("The account id(%{public}d) is current account id.", accountId);
        return;
    }

    std::map<std::string, uint32_t> importantEnabledAbilities;
    std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> tmpObserver;
    // Clear last account's data
    if (currentAccountId_ != -1) {
        HILOG_DEBUG("current account id: %{public}d", currentAccountId_);
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        tmpObserver = accountData->GetConfigCallbacks();
        accountData->GetImportantEnabledAbilities(importantEnabledAbilities);
        accountData->OnAccountSwitched();
        UpdateAccessibilityManagerService();
    }

    // Switch account id
    currentAccountId_ = accountId;

    // Initialize data for current account
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    accountData->Init();
    accountData->SetConfigCallbacks(tmpObserver);
    float discount = accountData->GetConfig()->GetBrightnessDiscount();
    auto& displayPowerMgrClient = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!displayPowerMgrClient.DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
    }
    if (accountData->GetInstalledAbilitiesFromBMS()) {
        accountData->UpdateImportantEnabledAbilities(importantEnabledAbilities);
        accountData->UpdateAbilities();
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageRemoved(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr.");
        return;
    }

    if (packageAccount->RemoveAbility(bundleName)) {
        HILOG_DEBUG("ability%{public}s removed!", bundleName.c_str());
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageAdd(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    packageAccount->AddAbility(bundleName);
}

void AccessibleAbilityManagerService::PackageChanged(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    packageAccount->ChangeAbility(bundleName);
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("windowId is %{public}d", event.GetWindowId());
    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
            Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(event.GetWindowId());
            Singleton<AccessibilityWindowManager>::GetInstance().SetAccessibilityFocusedWindow(event.GetWindowId());
            break;
        default:
            break;
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    accountData->UpdateAccountCapabilities();
    UpdateInputFilter();
    UpdateAccessibilityState();
}

void AccessibleAbilityManagerService::UpdateAccessibilityState()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }
    uint32_t state = accountData->GetAccessibilityState();
    if (!(state & STATE_ACCESSIBILITY_ENABLED)) {
        Singleton<AccessibilityWindowManager>::GetInstance().ClearAccessibilityFocused();
    }
    for (auto &callback : stateCallbacks_) {
        if (callback) {
            callback->OnStateChanged(state);
        }
    }
}

void AccessibleAbilityManagerService::UpdateCaptionProperty()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_DEBUG("UpdateCaptionProperty.");

        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        AccessibilityConfig::CaptionProperty caption = accountData->GetConfig()->GetCaptionProperty();
        for (auto &callback : accountData->GetCaptionPropertyCallbacks()) {
            if (callback) {
                callback->OnPropertyChanged(caption);
            }
        }
        }), "UpdateCaptionProperty");
}

void AccessibleAbilityManagerService::UpdateInputFilter()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    uint32_t flag = accountData->GetInputFilterFlag();
    HILOG_DEBUG("InputInterceptor flag is %{public}d", flag);

    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
    if (!inputInterceptor_) {
        HILOG_ERROR("inputInterceptor_ is null.");
        return;
    }
    inputInterceptor_->SetAvailableFunctions(flag);
    Utils::RecordStartingA11yEvent(flag);
}

RetError AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetScreenMagnificationState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        UpdateInputFilter();
        }), "TASK_SET_SCREENMAGNIFICATION_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetShortKeyState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        UpdateInputFilter();

        // Disable shortkey ability
        if (!state) {
            DisableShortKeyTargetAbility();
        }
        }), "TASK_SET_SHORTKEY_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetMouseKeyState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        UpdateInputFilter();
        }), "TASK_SET_MOUSEKEY_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    HILOG_INFO("time = [%{public}d]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetMouseAutoClick(time);
        syncPromise.set_value(ret);
        UpdateMouseAutoClick();
        UpdateInputFilter();
        }), "TASK_SET_MOUSE_AUTOCLICK");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    HILOG_INFO("name = [%{public}s]", name.c_str());
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetShortkeyTarget(name);
        syncPromise.set_value(ret);
        UpdateShortkeyTarget();
        }), "TASK_SET_SHORTKEY_TARGET");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetHighContrastTextState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        }), "TASK_SET_HIGHCONTRASTTEXT_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetInvertColorState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        }), "TASK_SET_INVERTCOLOR_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetAnimationOffState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        }), "TASK_SET_ANIMATIONOFF_STATE");
    RetError ret = syncFuture.get();
    int setGraphicParamRes = -1;
    int setArkuiParamRes = -1;
    if (state) {
        setGraphicParamRes = SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "0");
        setArkuiParamRes = SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "0");
    } else {
        setGraphicParamRes = SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "1");
        setArkuiParamRes = SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "1");
    }
    HILOG_INFO("SetParameter results are %{public}d and %{public}d", setGraphicParamRes, setArkuiParamRes);
    return ret;
}

RetError AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetAudioMonoState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        }), "TASK_SET_AUDIOMONO_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_INFO("filter = [%{public}u]", filter);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, filter]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetDaltonizationColorFilter(filter);
        syncPromise.set_value(ret);
        UpdateDaltonizationColorFilter();
        }), "TASK_SET_DALTONIZATION_COLORFILTER");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    HILOG_INFO("time = [%{public}u]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetContentTimeout(time);
        syncPromise.set_value(ret);
        UpdateContentTimeout();
        }), "TASK_SET_CONTENT_TIMEOUT");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    HILOG_INFO("discount = [%{public}f]", discount);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    auto& displayPowerMgrClient = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!displayPowerMgrClient.DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
        return Accessibility::RET_ERR_FAILED;
    }
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, discount]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetBrightnessDiscount(discount);
        syncPromise.set_value(ret);
        UpdateBrightnessDiscount();
        }), "TASK_SET_BRIGHTNESS_DISCOUNT");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    HILOG_INFO("balance = [%{public}f]", balance);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, balance]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetAudioBalance(balance);
        syncPromise.set_value(ret);
        UpdateAudioBalance();
        }), "TASK_SET_AUDIO_BALANCE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetScreenMagnificationState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetScreenMagnificationState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SCREENMAGNIFIER_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetShortKeyState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetShortKeyState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SHORTKEY_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetMouseKeyState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetMouseKeyState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_MOUSEKEY_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetMouseAutoClick(int32_t &time)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &time]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        time = accountData->GetConfig()->GetMouseAutoClick();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_MOUSE_AUTOCLICK");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetShortkeyTarget(std::string &name)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        name = accountData->GetConfig()->GetShortkeyTarget();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SHORTKEY_TARGET");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetHighContrastTextState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetHighContrastTextState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_HIGHCONTRASTTEXT_STATE");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetInvertColorState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetInvertColorState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_INVERTCOLOR_STATE");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAnimationOffState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetAnimationOffState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_ANIMATIONOFF_STATE");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAudioMonoState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetAudioMonoState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_AUDIOMONO_STATE");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetDaltonizationColorFilter(uint32_t &type)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &type]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        type = accountData->GetConfig()->GetDaltonizationColorFilter();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_DALTONIZATION_COLORFILTER");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetContentTimeout(uint32_t &timer)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &timer]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        timer = accountData->GetConfig()->GetContentTimeout();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CONTENT_TIMEOUT");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetBrightnessDiscount(float &brightness)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &brightness]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        brightness = accountData->GetConfig()->GetBrightnessDiscount();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_BRIGHTNESS_DISCOUNT");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAudioBalance(float &balance)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &balance]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        balance = accountData->GetConfig()->GetAudioBalance();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_AUDIO_BALANCE");

    return syncFuture.get();
}

void AccessibleAbilityManagerService::GetAllConfigs(AccessibilityConfigData &configData)
{
    HILOG_DEBUG();
    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &configData]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value();
            return;
        }

        configData.highContrastText_ = accountData->GetConfig()->GetHighContrastTextState();
        configData.invertColor_ = accountData->GetConfig()->GetInvertColorState();
        configData.animationOff_ = accountData->GetConfig()->GetAnimationOffState();
        configData.audioMono_ = accountData->GetConfig()->GetAudioMonoState();
        configData.mouseKey_ = accountData->GetConfig()->GetMouseKeyState();
        configData.captionState_ = accountData->GetConfig()->GetCaptionState();
        configData.screenMagnifier_ = accountData->GetConfig()->GetScreenMagnificationState();
        configData.shortkey_ = accountData->GetConfig()->GetShortKeyState();
        configData.mouseAutoClick_ = accountData->GetConfig()->GetMouseAutoClick();
        configData.daltonizationColorFilter_ = accountData->GetConfig()->GetDaltonizationColorFilter();
        configData.contentTimeout_ = accountData->GetConfig()->GetContentTimeout();
        configData.brightnessDiscount_ = accountData->GetConfig()->GetBrightnessDiscount();
        configData.audioBalance_ = accountData->GetConfig()->GetAudioBalance();
        configData.shortkeyTarget_ = accountData->GetConfig()->GetShortkeyTarget();
        configData.captionProperty_ = accountData->GetConfig()->GetCaptionProperty();
        syncPromise.set_value();
        }), "TASK_GET_ALL_CONFIGS");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::EnableShortKeyTargetAbility()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }

    std::string targetAbility = accountData->GetConfig()->GetShortkeyTarget();
    HILOG_DEBUG("target ability is [%{public}s]", targetAbility.c_str());
    if (targetAbility == "") {
        HILOG_ERROR("target ability is null");
        return false;
    }
    uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
        CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
    return InnerEnableAbility(targetAbility, capabilities) == RET_OK;
}

bool AccessibleAbilityManagerService::DisableShortKeyTargetAbility()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }

    std::string targetAbility = accountData->GetConfig()->GetShortkeyTarget();
    HILOG_DEBUG("target ability is [%{public}s]", targetAbility.c_str());
    if (targetAbility == "") {
        HILOG_ERROR("target ability is null");
        return false;
    }
    return InnerDisableAbility(targetAbility) == RET_OK;
}

uint32_t AccessibleAbilityManagerService::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed! callback:%{public}p, handler:%{public}p",
            callback.GetRefPtr(), handler_.get());
        return ERR_INVALID_VALUE;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value(ERR_INVALID_VALUE);
            return;
        }
        if (!configCallbackDeathRecipient_) {
            configCallbackDeathRecipient_ = new(std::nothrow) ConfigCallbackDeathRecipient();
            if (!configCallbackDeathRecipient_) {
                HILOG_ERROR("configCallbackDeathRecipient_ is null");
                syncPromise.set_value(ERR_INVALID_VALUE);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise.set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(configCallbackDeathRecipient_);
        accountData->AddConfigCallback(callback);
        HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
            accountData->GetConfigCallbacks().size());
        syncPromise.set_value(NO_ERROR);
        }), "TASK_REGISTER_CONFIG_OBSERVER");
    return syncFuture.get();
}

void AccessibleAbilityManagerService::ConfigCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveCallback(CONFIG_CALLBACK, this, remote);
}

void AccessibleAbilityManagerService::BundleManagerDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnBundleManagerDied(remote);
}

void AccessibleAbilityManagerService::UpdateConfigState()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateConfigState.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t state = accountData->GetConfig()->GetConfigState();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnConfigStateChanged(state);
            }
        }
        }), "UpdateConfigState");
}

void AccessibleAbilityManagerService::UpdateAudioBalance()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateAudioBalance.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        float audioBalance = accountData->GetConfig()->GetAudioBalance();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnAudioBalanceChanged(audioBalance);
            }
        }
        }), "UpdateAudioBalance");
}

void AccessibleAbilityManagerService::UpdateBrightnessDiscount()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateBrightnessDiscount.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        float brightnessDiscount = accountData->GetConfig()->GetBrightnessDiscount();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnBrightnessDiscountChanged(brightnessDiscount);
            }
        }
        }), "UpdateBrightnessDiscount");
}

void AccessibleAbilityManagerService::UpdateContentTimeout()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateContentTimeout.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t contentTimeout = accountData->GetConfig()->GetContentTimeout();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnContentTimeoutChanged(contentTimeout);
            }
        }
        }), "UpdateContentTimeout");
}

void AccessibleAbilityManagerService::UpdateDaltonizationColorFilter()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateDaltonizationColorFilter.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t daltonizationColorFilter = accountData->GetConfig()->GetDaltonizationColorFilter();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnDaltonizationColorFilterChanged(daltonizationColorFilter);
            }
        }
        }), "UpdateDaltonizationColorFilter");
}

void AccessibleAbilityManagerService::UpdateMouseAutoClick()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateMouseAutoClick.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        int32_t mouseAutoClick = accountData->GetConfig()->GetMouseAutoClick();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnMouseAutoClickChanged(mouseAutoClick);
            }
        }
        }), "UpdateMouseAutoClick");
}

void AccessibleAbilityManagerService::UpdateShortkeyTarget()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateShortkeyTarget.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        std::string shortkeyTarget = accountData->GetConfig()->GetShortkeyTarget();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnShortkeyTargetChanged(shortkeyTarget);
            }
        }
        }), "UpdateShortkeyTarget");
}

void AccessibleAbilityManagerService::RemoveCallback(CallBackID callback,
    const sptr<DeathRecipient> &recipient, const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("remove callback[%{public}d]", callback);
    if (!handler_) {
        HILOG_ERROR("handler is nullptr");
        return;
    }
    handler_->PostTask(std::bind([=]() -> void {
        if (!remote.GetRefPtr()) {
            HILOG_ERROR("remote is null");
            return;
        }
        remote->RemoveDeathRecipient(recipient);

        auto accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        switch (callback) {
            case STATE_CALLBACK:
                {
                    auto iter = std::find_if(stateCallbacks_.begin(), stateCallbacks_.end(),
                        [remote](const sptr<IAccessibleAbilityManagerStateObserver> &stateCallback) {
                            return stateCallback->AsObject() == remote;
                        });
                    if (iter != stateCallbacks_.end()) {
                        stateCallbacks_.erase(iter);
                    }
                }
                break;
            case CAPTION_PROPERTY_CALLBACK:
                accountData->RemoveCaptionPropertyCallback(remote);
                break;
            case ENABLE_ABILITY_LISTS_CALLBACK:
                accountData->RemoveEnableAbilityListsObserver(remote);
                break;
            case CONFIG_CALLBACK:
                accountData->RemoveConfigCallback(remote);
                break;
            default:
                break;
        }
        }), "RemoveCallback");
}

void AccessibleAbilityManagerService::OnBundleManagerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("OnBundleManagerDied ");
    if (!handler_) {
        HILOG_ERROR("handler is nullptr");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        if (!remote.GetRefPtr() || !bundleManager_) {
            HILOG_ERROR("remote is null");
            return;
        }

        bundleManager_->AsObject()->RemoveDeathRecipient(bundleManagerDeathRecipient_);
        bundleManager_ = nullptr;
        }), "OnBundleManagerDied");
}
} // namespace Accessibility
} // namespace OHOS