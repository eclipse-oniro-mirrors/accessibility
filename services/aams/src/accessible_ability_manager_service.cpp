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
#include <string>
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
    constexpr uint32_t TIME_OUT_OPERATOR = 5000;
    constexpr int32_t REQUEST_ID_MAX = 0x0000FFFF;
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
        a11yAccountsData_.Clear();
        stateObservers_.Clear();
        bundleManager_ = nullptr;
        inputInterceptor_ = nullptr;
        touchEventInjector_ = nullptr;
        keyEventFilter_ = nullptr;
        stateObserversDeathRecipient_ = nullptr;
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
    HILOG_INFO("systemAbilityId:%{public}d removed!", systemAbilityId);
    if (!handler_) {
        HILOG_DEBUG("Event handler is nullptr.");
        return;
    }

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_INFO("Remove system ability start");
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
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    std::promise<int> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, fd, args]() -> void {
        if (!accessibilityDumper_) {
            accessibilityDumper_ = new(std::nothrow) AccessibilityDumper();
            if (!accessibilityDumper_) {
                HILOG_ERROR("accessibilityDumper_ is nullptr");
                syncPromise.set_value(-1);
                return;
            }
        }
        syncPromise.set_value(accessibilityDumper_->Dump(fd, args));
        }), "TASK_DUMP_INFO");
    return syncFuture.get();
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
    const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver)
{
    HILOG_DEBUG();
    if (!stateObserver || !handler_) {
        HILOG_ERROR("parameters check failed!");
        return 0;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!stateObserversDeathRecipient_) {
        stateObserversDeathRecipient_ = new(std::nothrow) StateCallbackDeathRecipient();
        if (!stateObserversDeathRecipient_) {
            HILOG_ERROR("stateObserversDeathRecipient_ is null");
            return 0;
        }
    }

    if (!stateObserver->AsObject()) {
        HILOG_ERROR("object is null");
        return 0;
    }

    stateObserver->AsObject()->AddDeathRecipient(stateObserversDeathRecipient_);
    stateObservers_.AddStateObserver(stateObserver);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        return 0;
    }

    return accountData->GetAccessibilityState();
}

void AccessibleAbilityManagerService::GetRealWindowAndElementId(int32_t& windowId, int32_t& elementId)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([&, this]() -> void {
        Singleton<AccessibilityWindowManager>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
        syncPromise.set_value();
        }), "GET_REAL_WINDOW_AND_ELEMENT_ID");
    return syncFuture.get();
}

void AccessibleAbilityManagerService::GetSceneBoardInnerWinId(int32_t windowId, int32_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    std::promise<void> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([&, this]() -> void {
        Singleton<AccessibilityWindowManager>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        syncPromise.set_value();
        }), "GET_SCENE_BOARD_INNER_WINDOW_ID");
    return syncFuture.get();
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed!");
        return ERR_INVALID_VALUE;
    }

    std::shared_ptr<std::promise<uint32_t>> syncPromise = std::make_shared<std::promise<uint32_t>>();
    std::future syncFuture = syncPromise->get_future();
    handler_->PostTask(std::bind([this, syncPromise, callback]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromise->set_value(ERR_INVALID_VALUE);
            return;
        }
        if (!captionPropertyCallbackDeathRecipient_) {
            captionPropertyCallbackDeathRecipient_ = new(std::nothrow) CaptionPropertyCallbackDeathRecipient();
            if (!captionPropertyCallbackDeathRecipient_) {
                HILOG_ERROR("captionPropertyCallbackDeathRecipient_ is null");
                syncPromise->set_value(ERR_INVALID_VALUE);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromise->set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
        accountData->AddCaptionPropertyCallback(callback);
        HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
            accountData->GetCaptionPropertyCallbacks().size());
        syncPromise->set_value(NO_ERROR);
        }), "TASK_REGISTER_CAPTION_OBSERVER");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterCaptionObserver result");
        return RET_ERR_TIME_OUT;
    }
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

    std::shared_ptr<std::promise<void>> syncPromisePtr = std::make_shared<std::promise<void>>();
    std::future syncFuture = syncPromisePtr->get_future();
    handler_->PostTask(std::bind([this, syncPromisePtr, observer]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromisePtr->set_value();
            return;
        }
        if (!enableAbilityListsObserverDeathRecipient_) {
            enableAbilityListsObserverDeathRecipient_ = new(std::nothrow) EnableAbilityListsObserverDeathRecipient();
            if (!enableAbilityListsObserverDeathRecipient_) {
                HILOG_ERROR("enableAbilityListsObserverDeathRecipient_ is null");
                syncPromisePtr->set_value();
                return;
            }
        }
        if (!observer->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromisePtr->set_value();
            return;
        }
        observer->AsObject()->AddDeathRecipient(enableAbilityListsObserverDeathRecipient_);
        accountData->AddEnableAbilityListsObserver(observer);
        syncPromisePtr->set_value();
        }), "TASK_REGISTER_ENABLE_ABILITY_LISTS_OBSERVER");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterEnableAbilityListsObserver result");
        return;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d", stateType);
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
        sptr<AccessibilityWindowConnection> oldConnection = accountData->GetAccessibilityWindowConnection(windowId);
        DeleteConnectionAndDeathRecipient(windowId, oldConnection);
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

void AccessibleAbilityManagerService::DeleteConnectionAndDeathRecipient(
    const int32_t windowId, const sptr<AccessibilityWindowConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return;
    }

    accountData->RemoveAccessibilityWindowConnection(windowId);
    if (!connection->GetProxy()) {
        HILOG_WARN("proxy is null");
        return;
    }
    auto object = connection->GetProxy()->AsObject();
    if (object) {
        auto iter = interactionOperationDeathRecipients_.find(windowId);
        if (iter != interactionOperationDeathRecipients_.end()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient = iter->second;
            bool result = object->RemoveDeathRecipient(deathRecipient);
            HILOG_DEBUG("The result of deleting connection's death recipient is %{public}d", result);
            interactionOperationDeathRecipients_.erase(iter);
        }
    }
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
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetCaptionState(state);
    UpdateConfigState();
    return ret;
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

bool AccessibleAbilityManagerService::SetTargetAbility(const int32_t targetAbilityValue)
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }

    bool state;
    switch (targetAbilityValue) {
        case HIGH_CONTRAST_TEXT:
            state = accountData->GetConfig()->GetHighContrastTextState();
            return SetHighContrastTextState(!state) == RET_OK;
        case INVERT_COLOR:
            state = accountData->GetConfig()->GetInvertColorState();
            return SetInvertColorState(!state) == RET_OK;
        case ANIMATION_OFF:
            state = accountData->GetConfig()->GetAnimationOffState();
            return SetAnimationOffState(!state) == RET_OK;
        case SCREEN_MAGNIFICATION:
            state = accountData->GetConfig()->GetScreenMagnificationState();
            return SetScreenMagnificationState(!state) == RET_OK;
        case AUDIO_MONO:
            state = accountData->GetConfig()->GetAudioMonoState();
            return SetAudioMonoState(!state) == RET_OK;
        case MOUSE_KEY:
            state = accountData->GetConfig()->GetMouseKeyState();
            return SetMouseKeyState(!state) == RET_OK;
        case CAPTION_STATE:
            state = accountData->GetConfig()->GetCaptionState();
            return SetCaptionState(!state) == RET_OK;
        default:
            return false;
    }
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "InnerDisableAbility:" + name);

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
    return Singleton<AccessibilityWindowManager>::GetInstance().activeWindowId_;
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
    HILOG_DEBUG();
    if (currentAccountId_ == -1) {
        HILOG_ERROR("current account id is wrong");
        return nullptr;
    }

    return a11yAccountsData_.GetCurrentAccountData(currentAccountId_);
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetAccountData(int32_t accountId)
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAccountData(accountId);
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
    HILOG_DEBUG();
    auto accountData = a11yAccountsData_.AddAccountData(accountId);
    if (accountData) {
        accountData->Init();
    }
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG();
    if (accountId == currentAccountId_) {
        HILOG_ERROR("Remove user failed, this account is current account.");
        return;
    }

    auto accountData = a11yAccountsData_.RemoveAccountData(accountId);
    if (accountData) {
        accountData->GetConfig()->ClearData();
        return;
    }

    HILOG_ERROR("accountId is not exist");
}

void AccessibleAbilityManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG();

    if (accountId == currentAccountId_) {
        HILOG_WARN("The account is current account id.");
        return;
    }

    std::map<std::string, uint32_t> importantEnabledAbilities;
    // Clear last account's data
    if (currentAccountId_ != -1) {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        defaultConfigCallbacks_ = accountData->GetConfigCallbacks();
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
    accountData->SetConfigCallbacks(defaultConfigCallbacks_);
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

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    elementInfosResult_ = infos;
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    accessibilityInfoResult_ = info;
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

bool AccessibleAbilityManagerService::GetParentElementRecursively(int32_t windowId, int32_t elementId,
    std::vector<AccessibilityElementInfo>& infos)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(windowId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
        return false;
    }

    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl();
    if (!callBack) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }

    std::future<void> promiseFuture = callBack->promise_.get_future();
    connection->GetProxy()->SearchElementInfoByAccessibilityId(elementId, REQUEST_ID_MAX, callBack, 0);
    std::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
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

void AccessibleAbilityManagerService::FindInnerWindowId(const AccessibilityEventInfo &event, int32_t& windowId)
{
    HILOG_DEBUG();
    auto mapTable = Singleton<AccessibilityWindowManager>::GetInstance().sceneBoardElementIdMap_.GetAllPairs();
    int32_t elementId = event.GetAccessibilityId();
    while (1) {
        for (auto iter = mapTable.begin(); iter != mapTable.end(); iter++) {
            if (elementId == iter->second) {
                windowId = iter->first;
                HILOG_DEBUG("inner windowId %{public}d", windowId);
                return;
            }
        }

        std::vector<AccessibilityElementInfo> infos = {};
        if (GetParentElementRecursively(event.GetWindowId(), elementId, infos) == false) {
            HILOG_ERROR("find parent element failed");
            return;
        }

        if (infos[0].GetComponentType() == "root") {
            HILOG_ERROR("can not find parent element, has reach root node");
            return;
        }

        elementId = infos[0].GetParentNodeId();
    }
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("windowId is %{public}d", event.GetWindowId());
    int32_t windowId = event.GetWindowId();
    if (windowId == 1 && (evtType == TYPE_VIEW_HOVER_ENTER_EVENT || evtType == TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT)) {
        FindInnerWindowId(event, windowId);
    }

    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
            Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(windowId);
            Singleton<AccessibilityWindowManager>::GetInstance().SetAccessibilityFocusedWindow(windowId);
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

    stateObservers_.OnStateObservers(state);
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetScreenMagnificationState:" + to_string(state));

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetScreenMagnificationState(state);
    UpdateConfigState();
    UpdateInputFilter();
    return ret;
}

RetError AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetShortKeyState:" + to_string(state));

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
        }), "TASK_SET_SHORTKEY_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetMouseKeyState(state);
    UpdateConfigState();
    UpdateInputFilter();
    return ret;
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetShortkeyTarget:" + name);

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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetHighContrastTextState:" + to_string(state));

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetHighContrastTextState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibleAbilityManagerService::SetDaltonizationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetDaltonizationState:" + to_string(state));

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetDaltonizationState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetInvertColorState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetAnimationOffState(state);
    UpdateConfigState();
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetAudioMonoState:" + to_string(state));

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetAudioMonoState(state);
    UpdateConfigState();
    return ret;
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetAudioBalance:" + to_string(balance));

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

RetError AccessibleAbilityManagerService::SetClickResponseTime(const uint32_t time)
{
    HILOG_INFO("click response time = [%{public}u]", time);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetClickResponseTime:" + to_string(time));

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
        RetError ret = accountData->GetConfig()->SetClickResponseTime(time);
        syncPromise.set_value(ret);
        UpdateClickResponseTime();
        UpdateInputFilter();
        }), "TASK_SET_CLICK_RESPONSE_TIME");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_INFO("ignore repeat click state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetIgnoreRepeatClickState:" + to_string(state));

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
        RetError ret = accountData->GetConfig()->SetIgnoreRepeatClickState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        UpdateInputFilter();
        }), "TASK_SET_IGNORE_REPEAT_CLICK_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_INFO("click response time = [%{public}u]", time);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetIgnoreRepeatClickTime:" + to_string(time));

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
        RetError ret = accountData->GetConfig()->SetIgnoreRepeatClickTime(time);
        syncPromise.set_value(ret);
        UpdateIgnoreRepeatClickTime();
        UpdateInputFilter();
        }), "TASK_SET_IGNORE_REPEAT_CLICK_TIME");
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

RetError AccessibleAbilityManagerService::GetDaltonizationState(bool &state)
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
        state = accountData->GetConfig()->GetDaltonizationState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_DALTONIZATIONSTATE_STATE");

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

RetError AccessibleAbilityManagerService::GetClickResponseTime(uint32_t &time)
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
        time = accountData->GetConfig()->GetClickResponseTime();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CLICK_RESPONSE_TIME");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetIgnoreRepeatClickState(bool &state)
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
        state = accountData->GetConfig()->GetIgnoreRepeatClickState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_IGNORE_REPEAT_CLICK_STATE");

    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetIgnoreRepeatClickTime(uint32_t &time)
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
        time = accountData->GetConfig()->GetIgnoreRepeatClickTime();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_IGNORE_REPEAT_CLICK_TIME");

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
        configData.daltonizationState_ = accountData->GetConfig()->GetDaltonizationState();
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
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "EnableShortKeyTargetAbility");

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

    auto it = AccessibilityConfigTable.find(targetAbility);
    if (it != AccessibilityConfigTable.end()) {
        return SetTargetAbility(it->second);
    }

    uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
        CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
    RetError enableState = accountData->EnableAbility(targetAbility, capabilities);
    if (enableState == RET_ERR_CONNECTION_EXIST) {
        HILOG_DEBUG();
        return InnerDisableAbility(targetAbility) == RET_OK;
    }
    return enableState == RET_OK;
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
        HILOG_ERROR("Parameters check failed!");
        return ERR_INVALID_VALUE;
    }

    std::shared_ptr<std::promise<uint32_t>> syncPromisePtr = std::make_shared<std::promise<uint32_t>>();
    std::future syncFuture = syncPromisePtr->get_future();
    handler_->PostTask(std::bind([this, syncPromisePtr, callback]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromisePtr->set_value(ERR_INVALID_VALUE);
            return;
        }
        if (!configCallbackDeathRecipient_) {
            configCallbackDeathRecipient_ = new(std::nothrow) ConfigCallbackDeathRecipient();
            if (!configCallbackDeathRecipient_) {
                HILOG_ERROR("configCallbackDeathRecipient_ is null");
                syncPromisePtr->set_value(ERR_INVALID_VALUE);
                return;
            }
        }
        if (!callback->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromisePtr->set_value(0);
            return;
        }
        callback->AsObject()->AddDeathRecipient(configCallbackDeathRecipient_);
        accountData->AddConfigCallback(callback);
        HILOG_DEBUG("the size of caption property callbacks is %{public}zu",
            accountData->GetConfigCallbacks().size());
        syncPromisePtr->set_value(NO_ERROR);
        }), "TASK_REGISTER_CONFIG_OBSERVER");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterConfigObserver result");
        return RET_ERR_TIME_OUT;
    }
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

void AccessibleAbilityManagerService::UpdateClickResponseTime()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateClickResponseTime.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t time = accountData->GetConfig()->GetClickResponseTime();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnClickResponseTimeChanged(time);
            }
        }
        }), "UpdateClickResponseTime");
}

void AccessibleAbilityManagerService::UpdateIgnoreRepeatClickTime()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateIgnoreRepeatClickTime.");
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t time = accountData->GetConfig()->GetIgnoreRepeatClickTime();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnIgnoreRepeatClickTimeChanged(time);
            }
        }
        }), "UpdateIgnoreRepeatClickTime");
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

        if (callback == CONFIG_CALLBACK) {
            RemoveSavedConfigCallback(remote);
        }
        auto accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        switch (callback) {
            case STATE_CALLBACK:
                stateObservers_.RemoveStateObserver(remote);
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

void AccessibleAbilityManagerService::RemoveSavedConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("start.");
    for (auto itr = defaultConfigCallbacks_.begin(); itr != defaultConfigCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            defaultConfigCallbacks_.erase(itr);
            break;
        }
    }
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

void AccessibleAbilityManagerService::StateObservers::AddStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver)
{
    std::lock_guard<std::mutex> lock(stateObserversMutex_);
    auto iter = std::find(observersList_.begin(), observersList_.end(), stateObserver);
    if (iter == observersList_.end()) {
        observersList_.push_back(stateObserver);
        HILOG_INFO("register state observer successfully");
        return;
    }

    HILOG_INFO("state observer is existed");
}

void AccessibleAbilityManagerService::StateObservers::OnStateObservers(uint32_t state)
{
    std::lock_guard<std::mutex> lock(stateObserversMutex_);
    for (auto& stateObserver : observersList_) {
        if (stateObserver) {
            stateObserver->OnStateChanged(state);
        }
    }
}

void AccessibleAbilityManagerService::StateObservers::RemoveStateObserver(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> lock(stateObserversMutex_);
    auto iter = std::find_if(observersList_.begin(), observersList_.end(),
        [remote](const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver) {
            return stateObserver->AsObject() == remote;
        });
    if (iter != observersList_.end()) {
        observersList_.erase(iter);
    }
}

void AccessibleAbilityManagerService::StateObservers::Clear()
{
    std::lock_guard<std::mutex> lock(stateObserversMutex_);
    observersList_.clear();
}
} // namespace Accessibility
} // namespace OHOS