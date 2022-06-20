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

#include "ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "input_manager.h"
#include "os_account_info.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"
#include "utils.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
    const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
    constexpr int32_t AUTOCLICK_DELAY_TIME_MIN = 1000; // ms
    constexpr int32_t AUTOCLICK_DELAY_TIME_MAX = 5000; // ms
    constexpr int32_t OS_ACCOUNT_ID = 100;
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<AccessibleAbilityManagerService>::GetInstance());
std::mutex AccessibleAbilityManagerService::mutex_;

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
    dependentServicesStatus_[ABILITY_MGR_SERVICE_ID] = false;
    dependentServicesStatus_[SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN] = false;
    dependentServicesStatus_[BUNDLE_MGR_SERVICE_SYS_ABILITY_ID] = false;
    dependentServicesStatus_[COMMON_EVENT_SERVICE_ID] = false;
    dependentServicesStatus_[DISPLAY_MANAGER_SERVICE_SA_ID] = false;
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

    if (isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already start.");
        return;
    }

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

    HILOG_INFO("AddAbilityListener!");
    AddSystemAbilityListener(ABILITY_MGR_SERVICE_ID);
    AddSystemAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    AddSystemAbilityListener(DISPLAY_MANAGER_SERVICE_SA_ID);
    AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);

    isRunning_ = true;
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");

    if (!isRunning_) {
        HILOG_DEBUG("AccessibleAbilityManagerService is already stop.");
        return;
    }

    a11yAccountsData_.clear();
    bundleManager_ = nullptr;
    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    stateCallbackDeathRecipient_ = nullptr;
    runner_.reset();
    handler_.reset();
    for (auto &iter : dependentServicesStatus_) {
        iter.second = false;
    }

    HILOG_INFO("AccessibleAbilityManagerService::OnStop OK.");
    isRunning_ = false;
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
        for (auto &iter : dependentServicesStatus_) {
            if (iter.second == false) {
                HILOG_DEBUG("Not all the dependence is ready!");
                return;
            }
        }

        if (Init() == false) {
            HILOG_ERROR("AccessibleAbilityManagerService::Init failed!");
            return;
        }

        if (Publish(this) == false) {
            HILOG_ERROR("AccessibleAbilityManagerService::Publish failed!");
            return;
        }
        HILOG_DEBUG("AAMS is ready!");
        }), "OnAddSystemAbility");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_DEBUG("systemAbilityId:%{public}d removed!", systemAbilityId);
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

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo &uiEvent)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d]", uiEvent.GetEventType(), uiEvent.GetGestureType());
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return;
    }

    if (currentAccountId_ != GetOsAccountId()) {
        HILOG_ERROR("account is wrong osAccount[%{public}d], currentAccount[%{public}d]!",
            GetOsAccountId(), currentAccountId_);
        return;
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
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed! callback:%{public}p, handler:%{public}p",
            callback.GetRefPtr(), handler_.get());
        return 0;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise.set_value(0);
            return;
        }

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
        accountData->AddStateCallback(callback);

        HILOG_INFO("AccessibleAbilityManagerService::RegisterStateObserver successfully");

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

bool AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d, handler:%{public}p", stateType, handler_.get());
        return false;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &infos, abilityTypes, stateType]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Get current account data failed!!");
            syncPromise.set_value();
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
        syncPromise.set_value();
        }), "TASK_GET_ABILITY_LIST");
    syncFuture.get();

    return true;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation)
{
    HILOG_INFO("Register windowId[%{public}d] start", windowId);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        int32_t osAccountId = GetOsAccountId();
        auto iter = a11yAccountsData_.find(osAccountId);
        if (iter == a11yAccountsData_.end()) {
            HILOG_DEBUG("There is no account[%{public}d] and new one", osAccountId);
            sptr<AccessibilityAccountData> accountData = new(std::nothrow) AccessibilityAccountData(osAccountId);
            if (!accountData) {
                HILOG_ERROR("accountData is null");
                return;
            }
            a11yAccountsData_.insert(make_pair(osAccountId, accountData));
        }

        sptr<AccessibilityAccountData> accountData = a11yAccountsData_[osAccountId];
        if (accountData->GetAccessibilityWindowConnection(windowId)) {
            HILOG_WARN("This operation already exists, do not register twice!!");
            return;
        }

        sptr<AccessibilityWindowConnection> connection =
            new(std::nothrow) AccessibilityWindowConnection(windowId, operation, osAccountId);
        if (!connection) {
            HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
            return;
        }
        accountData->AddAccessibilityWindowConnection(windowId, connection);

        if (!interactionOperationDeathRecipient_) {
            interactionOperationDeathRecipient_ = new(std::nothrow) InteractionOperationDeathRecipient(windowId);
            if (!interactionOperationDeathRecipient_) {
                HILOG_ERROR("interactionOperationDeathRecipient_ is null");
                return;
            }
        }

        if (connection->GetProxy()) {
            auto object = connection->GetProxy()->AsObject();
            if (object) {
                HILOG_DEBUG("Add death recipient of operation");
                bool result = object->AddDeathRecipient(interactionOperationDeathRecipient_);
                HILOG_DEBUG("The result of adding operation's death recipient is %{public}d", result);
            }
        }
        }), "TASK_REGISTER_ELEMENT_OPERATOR");
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    HILOG_INFO("Deregister windowId[%{public}d] start", windowId);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
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
        if (connection->GetProxy()) {
            auto object = connection->GetProxy()->AsObject();
            if (object) {
                HILOG_DEBUG("Delete death recipient of operation");
                bool result = object->RemoveDeathRecipient(interactionOperationDeathRecipient_);
                HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
            }
        }

        accountData->RemoveAccessibilityWindowConnection(windowId);
        }), "TASK_DEREGISTER_ELEMENT_OPERATOR");
}

AccessibilityConfig::CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG();
    AccessibilityConfig::CaptionProperty property {};
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return property;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &property]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        property = accountData->GetConfig()->GetCaptionProperty();
        syncPromise.set_value();
        }), "TASK_GET_CAPTION_PROPERTY");
    syncFuture.get();

    return property;
}

void AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &caption]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetCaptionProperty(caption);
        syncPromise.set_value();
        UpdateCaptionProperty();
        }), "TASK_SET_CAPTION_PROPERTY");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetCaptionState(state);
        syncPromise.set_value();
        UpdateConfigState();
        }), "TASK_SET_CAPTION_STATE");
    syncFuture.get();
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

bool AccessibleAbilityManagerService::GetCaptionState()
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
        bool result = accountData->GetConfig()->GetCaptionState();
        syncPromise.set_value(result);
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

bool AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name, &capabilities]() -> void {
        HILOG_DEBUG();
        bool result = InnerEnableAbility(name, capabilities);
        syncPromise.set_value(result);
        }), "TASK_ENABLE_ABILITIES");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::InnerEnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    bool result = accountData->EnableAbility(name, capabilities);
    UpdateAbilities();
    return result;
}

bool AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &enabledAbilities]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value();
            return;
        }
        enabledAbilities = accountData->GetEnabledAbilities();
        syncPromise.set_value();
        }), "TASK_GET_ENABLE_ABILITIES");
    syncFuture.get();

    return true;
}

bool AccessibleAbilityManagerService::GetInstalledAbilities(std::vector<AccessibilityAbilityInfo> &installedAbilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &installedAbilities]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value();
            return;
        }
        installedAbilities = accountData->GetInstalledAbilities();
        syncPromise.set_value();
        }), "TASK_GET_INSTALLED_ABILITIES");
    syncFuture.get();

    return true;
}

bool AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG();
        bool result = InnerDisableAbility(name);
        syncPromise.set_value(result);
        }), "TASK_DISABLE_ABILITIES");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::InnerDisableAbility(const std::string &name)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    accountData->RemoveEnabledAbility(name);
    UpdateAbilities();
    return true;
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

        std::function<void()> addUITestClientFunc =
            std::bind(&AccessibleAbilityManagerService::AddUITestClient, this, obj);
        handler_->PostTask(addUITestClientFunc, "AddUITestClient");
        syncPromise.set_value(RET_OK);
        }), "TASK_ENABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::DisableUITestAbility()
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
        std::string uiTestUri = Utils::GetUri(UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
        if (!connection) {
            HILOG_ERROR("connection is not existed!!");
            syncPromise.set_value(false);
            return;
        }
        std::function<void()> removeUITestClientFunc =
            std::bind(&AccessibleAbilityManagerService::RemoveUITestClient, this, connection);
        handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
        syncPromise.set_value(true);
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
    currentAccountId_ = GetOsAccountId();
    HILOG_DEBUG("current accountId %{public}d", currentAccountId_);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return false;
    }
    accountData->Init();
    // Get installed accessibility extension ability from BMS
    if (accountData->GetInstalledAbilitiesFromBMS()) {
        UpdateAbilities();
        UpdateAccessibilityManagerService();
    } else {
        HILOG_ERROR("Get installed ExtensionAbility failed");
        return false;
    }

    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
    bool result = Singleton<AccessibilityWindowManager>::GetInstance().Init();
    HILOG_DEBUG("wms init result is %{public}d", result);
    return true;
}

void AccessibleAbilityManagerService::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
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
    } else {
        HILOG_INFO("AccessibleAbilityManagerService::GetBundleMgrProxy OK");
    }
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
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveStateCallback(remote);
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveCaptionPropertyCallback(remote);
}

void AccessibleAbilityManagerService::EnableAbilityListsObserverDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveEnableAbilityListsObserver(remote);
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
        HILOG_ERROR("switch user failed, this account is current account.");
        return;
    }

    // Clear last account's data
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->OnAccountSwitched();
    UpdateAccessibilityManagerService();

    // Switch account id
    currentAccountId_ = accountId;

    // Initialize data for current account
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        HILOG_DEBUG("Find account[%{public}d].", currentAccountId_);
        accountData = iter->second;
    } else {
        accountData = new(std::nothrow) AccessibilityAccountData(currentAccountId_);
        a11yAccountsData_.insert(make_pair(currentAccountId_, accountData));
    }

    if (!accountData) {
        HILOG_ERROR("accountData is null");
        return;
    }
    accountData->Init();
    accountData->GetInstalledAbilitiesFromBMS();
    UpdateAbilities();
}

void AccessibleAbilityManagerService::PackageRemoved(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr.");
        return;
    }
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    packageAccount->RemoveInstalledAbility(bundleName);

    // Remove enabled ability, remove connecting ability if it is connecting.
    bool needUpdateAbility = false;
    std::vector<std::string> enabledAbilities = packageAccount->GetEnabledAbilities();
    if (enabledAbilities.empty()) {
        HILOG_DEBUG("There is no enabled abilities.");
        return;
    }
    for (auto& enableAbility : enabledAbilities) {
        if (enableAbility.substr(0, enableAbility.find("/")) == bundleName) {
            packageAccount->RemoveEnabledAbility(enableAbility);
            packageAccount->RemoveConnectingA11yAbility(enableAbility);
            needUpdateAbility = true;
        }
    }

    // Remove connected ability
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectedAbilities =
        packageAccount->GetConnectedA11yAbilities();
    if (connectedAbilities.empty()) {
        HILOG_DEBUG("There is no connected abilities.");
        return;
    }
    for (auto &connectedAbility : connectedAbilities) {
        std::size_t firstPos = connectedAbility.first.find_first_of('/') + 1;
        std::size_t endPos = connectedAbility.first.find_last_of('/');
        if (endPos <= firstPos) {
            HILOG_ERROR("it's a wrong ability and the uri is %{public}s", connectedAbility.first.c_str());
            continue;
        }
        std::string connectedBundleName = connectedAbility.first.substr(firstPos, endPos - firstPos);
        if (connectedBundleName == bundleName) {
            HILOG_DEBUG("Remove connected ability and it's bundle name is %{public}s", connectedBundleName.c_str());
            packageAccount->RemoveConnectedAbility(connectedAbility.second);
            needUpdateAbility = true;
        }
    }

    if (needUpdateAbility) {
        UpdateAbilities();
        UpdateAccessibilityManagerService();
    }
}

void AccessibleAbilityManagerService::PackageAdd(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    bool hasNewExtensionAbility = false;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        HILOG_ERROR("bundleMgrProxy is nullptr.");
        return;
    }
    bundleMgrProxy->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &newAbility : extensionInfos) {
        if (newAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package added is an extension ability and\
                extension ability's name is %{public}s", newAbility.name.c_str());
            AccessibilityAbilityInitParams initParams;
            Utils::Parse(newAbility, initParams);
            std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
                std::make_shared<AccessibilityAbilityInfo>(initParams);
            if (!accessibilityInfo) {
                HILOG_ERROR("accessibilityInfo is nullptr");
                return;
            }
            packageAccount->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("add new extension ability successfully and installed abilities's size is %{public}zu",
                packageAccount->GetInstalledAbilities().size());
            hasNewExtensionAbility = true;
            break;
        }
    }

    if (hasNewExtensionAbility) {
        HILOG_DEBUG("add new extension ability and update abilities.");
        UpdateAbilities();
    }
}

void AccessibleAbilityManagerService::PackageChanged(const std::string &bundleName)
{
    HILOG_DEBUG("bundleName(%{public}s)", bundleName.c_str());

    bool hasChanged = false;
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    if (packageAccount->GetInstalledAbilities().empty()) {
        HILOG_DEBUG("There is no installed abilities.");
        return;
    }
    packageAccount->RemoveInstalledAbility(bundleName);

    // Add installed ability
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        HILOG_ERROR("bundleMgrProxy is nullptr.");
        return;
    }
    bundleMgrProxy->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, GetCurrentAccountId(), extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &changedAbility : extensionInfos) {
        if (changedAbility.bundleName == bundleName) {
            HILOG_DEBUG("The package changed is an extension ability and\
                extension ability's name is %{public}s", changedAbility.name.c_str());
            AccessibilityAbilityInitParams initParams;
            Utils::Parse(changedAbility, initParams);
            std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
                std::make_shared<AccessibilityAbilityInfo>(initParams);
            packageAccount->AddInstalledAbility(*accessibilityInfo);
            HILOG_DEBUG("update new extension ability successfully and installed abilities's size is %{public}zu",
                packageAccount->GetInstalledAbilities().size());
            hasChanged = true;
            break;
        }
    }

    if (hasChanged) {
        HILOG_DEBUG("update new extension ability and update abilities.");
        UpdateAbilities();
    }
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

void AccessibleAbilityManagerService::UpdateAbilities()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    vector<AccessibilityAbilityInfo> installedAbilities = accountData->GetInstalledAbilities();
    HILOG_DEBUG("installedAbilities is %{public}zu.", installedAbilities.size());
    for (auto &installAbility : installedAbilities) {
        std::string deviceId = "";
        std::string bundleName = installAbility.GetPackageName();
        std::string abilityName = installAbility.GetName();
        HILOG_DEBUG("installAbility's packageName is %{public}s and abilityName is %{public}s",
            bundleName.c_str(), abilityName.c_str());

        auto connectingAbilities = accountData->GetConnectingA11yAbilities();
        vector<string>::iterator iter = std::find(connectingAbilities.begin(),
            connectingAbilities.end(), Utils::GetUri(bundleName, abilityName));
        if (iter != connectingAbilities.end()) {
            HILOG_DEBUG("The ability(bundleName[%{public}s] abilityName[%{public}s]) is connecting.",
                bundleName.c_str(), abilityName.c_str());
            continue;
        }

        sptr<AccessibleAbilityConnection> connection =
            accountData->GetAccessibleAbilityConnection(Utils::GetUri(bundleName, abilityName));

        auto enabledAbilities = accountData->GetEnabledAbilities();
        iter = std::find(enabledAbilities.begin(), enabledAbilities.end(), Utils::GetUri(bundleName, abilityName));
        if (iter != enabledAbilities.end()) {
            if (!connection) {
                AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
                connection = new(std::nothrow) AccessibleAbilityConnection(accountData,
                    connectCounter_++, installAbility);
                connection->Connect(element);
            }
        } else {
            HILOG_DEBUG("not in enabledAbilites list .");
            if (connection) {
                connection->Disconnect();
            }
        }
    }
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
    for (auto &callback : accountData->GetStateCallbacks()) {
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

    uint32_t flag = 0;
    if (accountData->GetScreenMagnificationFlag() && accountData->GetConfig()->GetScreenMagnificationState()) {
        flag |= AccessibilityInputInterceptor::FEATURE_SCREEN_MAGNIFICATION;
    }
    if (accountData->GetEventTouchGuideStateFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_TOUCH_EXPLORATION;
    }
    if (accountData->GetFilteringKeyEventsFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_FILTER_KEY_EVENTS;
    }
    if (accountData->GetGesturesSimulationFlag()) {
        flag |= AccessibilityInputInterceptor::FEATURE_INJECT_TOUCH_EVENTS;
    }
    if (accountData->GetConfig()->GetShortKeyState()) {
        flag |= AccessibilityInputInterceptor::FEATURE_SHORT_KEY;
    }
    int32_t autoclickTime = accountData->GetConfig()->GetMouseAutoClick();
    if (autoclickTime >= AUTOCLICK_DELAY_TIME_MIN && autoclickTime <= AUTOCLICK_DELAY_TIME_MAX) {
        flag |= AccessibilityInputInterceptor::FEATURE_MOUSE_AUTOCLICK;
    }

    HILOG_DEBUG("InputInterceptor flag is %{public}d", flag);

    inputInterceptor_ = AccessibilityInputInterceptor::GetInstance();
    if (!inputInterceptor_) {
        HILOG_ERROR("inputInterceptor_ is null.");
        return;
    }
    inputInterceptor_->SetAvailableFunctions(flag);
}

void AccessibleAbilityManagerService::AddUITestClient(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG();
    auto currentAccountData = GetCurrentAccountData();
    if (!currentAccountData) {
        HILOG_ERROR("currentAccountData is nullptr");
        return;
    }

    // Add installed ability
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>();
    if (!abilityInfo) {
        HILOG_ERROR("abilityInfo is null");
        return;
    }
    abilityInfo->SetPackageName(UI_TEST_BUNDLE_NAME);
    uint32_t capabilities = CAPABILITY_RETRIEVE | CAPABILITY_GESTURE;
    abilityInfo->SetCapabilityValues(capabilities);
    abilityInfo->SetAccessibilityAbilityType(ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetEventTypes(EventType::TYPES_ALL_MASK);
    currentAccountData->AddInstalledAbility(*abilityInfo);

    // Add connected ability
    sptr<AppExecFwk::ElementName> elementName = new(std::nothrow) AppExecFwk::ElementName();
    if (!elementName) {
        HILOG_ERROR("elementName is null");
        return;
    }
    elementName->SetBundleName(UI_TEST_BUNDLE_NAME);
    elementName->SetAbilityName(UI_TEST_ABILITY_NAME);
    sptr<AccessibleAbilityConnection> connection = new(std::nothrow) AccessibleAbilityConnection(
        currentAccountData, connectCounter_++, *abilityInfo);
    if (!connection) {
        HILOG_ERROR("connection is null");
        return;
    }
    connection->OnAbilityConnectDoneSync(*elementName, obj, 0);
}

void AccessibleAbilityManagerService::RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    accountData->RemoveInstalledAbility(UI_TEST_BUNDLE_NAME);
    connection->OnAbilityDisconnectDoneSync(connection->GetElementName(), 0);
}

int32_t AccessibleAbilityManagerService::GetOsAccountId()
{
    HILOG_DEBUG("start");
    // Temp deal
    return OS_ACCOUNT_ID;
}

void AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetScreenMagnificationState(state);
        syncPromise.set_value();
        UpdateConfigState();
        UpdateInputFilter();
        }), "TASK_SET_SCREENMAGNIFICATION_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetShortKeyState(state);
        syncPromise.set_value();
        UpdateConfigState();
        UpdateInputFilter();

        // Disable shortkey ability
        if (!state) {
            DisableShortKeyTargetAbility();
        }
        }), "TASK_SET_SHORTKEY_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetMouseKeyState(state);
        syncPromise.set_value();
        UpdateConfigState();
        UpdateInputFilter();
        }), "TASK_SET_MOUSEKEY_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    HILOG_INFO("time = [%{public}d]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetMouseAutoClick(time);
        syncPromise.set_value();
        UpdateMouseAutoClick();
        UpdateInputFilter();
        }), "TASK_SET_MOUSE_AUTOCLICK");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    HILOG_INFO("name = [%{public}s]", name.c_str());
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }
    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, name]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetShortkeyTarget(name);
        syncPromise.set_value();
        UpdateShortkeyTarget();
        }), "TASK_SET_SHORTKEY_TARGET");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetHighContrastTextState(state);
        syncPromise.set_value();
        UpdateConfigState();
        }), "TASK_SET_HIGHCONTRASTTEXT_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetInvertColorState(state);
        syncPromise.set_value();
        UpdateConfigState();
        }), "TASK_SET_INVERTCOLOR_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetAnimationOffState(state);
        syncPromise.set_value();
        UpdateConfigState();
        }), "TASK_SET_ANIMATIONOFF_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetAudioMonoState(state);
        syncPromise.set_value();
        UpdateConfigState();
        }), "TASK_SET_AUDIOMONO_STATE");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    HILOG_INFO("filter = [%{public}u]", filter);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, filter]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetDaltonizationColorFilter(filter);
        syncPromise.set_value();
        UpdateDaltonizationColorFilter();
        }), "TASK_SET_DALTONIZATION_COLORFILTER");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    HILOG_INFO("time = [%{public}u]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetContentTimeout(time);
        syncPromise.set_value();
        UpdateContentTimeout();
        }), "TASK_SET_CONTENT_TIMEOUT");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    HILOG_INFO("discount = [%{public}f]", discount);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, discount]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetBrightnessDiscount(discount);
        syncPromise.set_value();
        UpdateBrightnessDiscount();
        }), "TASK_SET_BRIGHTNESS_DISCOUNT");
    syncFuture.get();
}

void AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    HILOG_INFO("balance = [%{public}f]", balance);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, balance]() -> void {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value();
            return;
        }
        accountData->GetConfig()->SetAudioBalance(balance);
        syncPromise.set_value();
        UpdateAudioBalance();
        }), "TASK_SET_AUDIO_BALANCE");
    syncFuture.get();
}

bool AccessibleAbilityManagerService::GetScreenMagnificationState()
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
        bool result = accountData->GetConfig()->GetScreenMagnificationState();
        syncPromise.set_value(result);
        }), "TASK_GET_SCREENMAGNIFIER_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetShortKeyState()
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
        bool result = accountData->GetConfig()->GetShortKeyState();
        syncPromise.set_value(result);
        }), "TASK_GET_SHORTKEY_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetMouseKeyState()
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
        bool result = accountData->GetConfig()->GetMouseKeyState();
        syncPromise.set_value(result);
        }), "TASK_GET_MOUSEKEY_STATE");
    return syncFuture.get();
}

int32_t AccessibleAbilityManagerService::GetMouseAutoClick()
{
    HILOG_DEBUG();
    std::promise<int32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        int32_t result = accountData->GetConfig()->GetMouseAutoClick();
        syncPromise.set_value(result);
        }), "TASK_GET_MOUSE_AUTOCLICK");

    return syncFuture.get();
}

std::string AccessibleAbilityManagerService::GetShortkeyTarget()
{
    HILOG_DEBUG();
    std::promise<std::string> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value("");
            return;
        }
        std::string result = accountData->GetConfig()->GetShortkeyTarget();
        syncPromise.set_value(result);
        }), "TASK_GET_SHORTKEY_TARGET");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetHighContrastTextState()
{
    HILOG_DEBUG();
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
        bool result = accountData->GetConfig()->GetHighContrastTextState();
        syncPromise.set_value(result);
        }), "TASK_GET_HIGHCONTRASTTEXT_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetInvertColorState()
{
    HILOG_DEBUG();
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
        bool result = accountData->GetConfig()->GetInvertColorState();
        syncPromise.set_value(result);
        }), "TASK_GET_INVERTCOLOR_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetAnimationOffState()
{
    HILOG_DEBUG();
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
        bool result = accountData->GetConfig()->GetAnimationOffState();
        syncPromise.set_value(result);
        }), "TASK_GET_ANIMATIONOFF_STATE");

    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetAudioMonoState()
{
    HILOG_DEBUG();
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
        bool result = accountData->GetConfig()->GetAudioMonoState();
        syncPromise.set_value(result);
        }), "TASK_GET_AUDIOMONO_STATE");

    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::GetDaltonizationColorFilter()
{
    HILOG_DEBUG();
    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        uint32_t result = accountData->GetConfig()->GetDaltonizationColorFilter();
        syncPromise.set_value(result);
        }), "TASK_GET_DALTONIZATION_COLORFILTER");

    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::GetContentTimeout()
{
    HILOG_DEBUG();
    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        uint32_t result = accountData->GetConfig()->GetContentTimeout();
        syncPromise.set_value(result);
        }), "TASK_GET_CONTENT_TIMEOUT");

    return syncFuture.get();
}

float AccessibleAbilityManagerService::GetBrightnessDiscount()
{
    HILOG_DEBUG();
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        float result = accountData->GetConfig()->GetBrightnessDiscount();
        syncPromise.set_value(result);
        }), "TASK_GET_BRIGHTNESS_DISCOUNT");

    return syncFuture.get();
}

float AccessibleAbilityManagerService::GetAudioBalance()
{
    HILOG_DEBUG();
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        float result = accountData->GetConfig()->GetAudioBalance();
        syncPromise.set_value(result);
        }), "TASK_GET_AUDIO_BALANCE");

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
    bool result = InnerEnableAbility(targetAbility, capabilities);
    HILOG_DEBUG("result is %{public}d", result);
    return result;
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
    bool result = InnerDisableAbility(targetAbility);
    HILOG_DEBUG("result is %{public}d", result);
    return result;
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
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!remote.GetRefPtr()) {
        HILOG_ERROR("remote is null");
        return;
    }
    remote->RemoveDeathRecipient(this);

    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveConfigCallback(remote);
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
} // namespace Accessibility
} // namespace OHOS