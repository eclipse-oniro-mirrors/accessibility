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

#include <gtest/gtest.h>
#include "ability_info.h"
#include "accessibility_ability_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<AccessibleAbilityManagerService>::GetInstance());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true), bundleManager_(nullptr)
{
    HILOG_INFO("AccessibleAbilityManagerService is constructed");
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");

    if (!runner_) {
        const std::string serviceName = "AccessibleAbilityManagerService";
        runner_ = AppExecFwk::EventRunner::Create(serviceName);
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

    (void)Init();
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    if (Publish(&Singleton<AccessibleAbilityManagerService>::GetInstance()) == false) {
        HILOG_ERROR("AccessibleAbilityManagerService::Publish failed!");
        return;
    }
    HILOG_INFO("OnAddSystemAbility AccessibleAbilityManagerService");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
    HILOG_INFO("OnRemoveSystemAbility AccessibleAbilityManagerService");
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG("start");
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
    Singleton<AccessibilityWindowManager>::GetInstance().Init();

    return true;
}

void AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent)
{
    HILOG_INFO("AccessibleAbilityManagerService::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    AccessibilityAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    AccessibilityAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return 0;
    }

    if (!stateCallbackDeathRecipient_) {
        stateCallbackDeathRecipient_ = new StateCallbackDeathRecipient();
    }
    if (callback != nullptr) {
        callback->AsObject()->AddDeathRecipient(stateCallbackDeathRecipient_);
        accountData->AddStateCallback(callback);
    }

    HILOG_INFO("AccessibleAbilityManagerService::RegisterStateCallback successfully");

    return accountData->GetAccessibilityState();
}

bool AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    if ((stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("stateType is out of range!!");
        return false;
    }

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    if (!accountData) {
        HILOG_ERROR("Get current account data failed!!");
        return false;
    }

    AbilityStateType state = static_cast<AbilityStateType>(stateType);
    std::vector<AccessibilityAbilityInfo> abilities;
    accountData->GetAbilitiesByState(state, abilities);
    for (auto& ability : abilities) {
        if (ability.GetAccessibilityAbilityType() & static_cast<uint32_t>(abilityTypes)) {
            infos.push_back(ability);
        }
    }
    return true;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& operation)
{
    sptr<AccessibilityWindowConnection> connection = new AccessibilityWindowConnection(
        windowId, operation, GetOsAccountId());
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr && connection != nullptr) {
        accountData->AddAccessibilityWindowConnection(windowId, connection);
    }
    HILOG_INFO("AccessibleAbilityManagerService::RegisterElementOperator successfully");
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData != nullptr) {
        accountData->RemoveAccessibilityWindowConnection(windowId);
        HILOG_INFO("AccessibleAbilityManagerService::DeregisterElementOperator successfully");
    }
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    auto iter = a11yAccountsData_.find(currentAccountId_);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(currentAccountId_);
    a11yAccountsData_.insert(std::make_pair(currentAccountId_, accountData));
    return accountData;
}

void AccessibleAbilityManagerService::NotifyDisplayResizeStateChanged(
    int32_t displayId, Rect& rect, float scale, float centerX, float centerY)
{
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    return bundleManager_;
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetAccessibilityWindowConnection(int32_t windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetAccessibilityWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int32_t windowId)
{
    (void)windowId;
}

void AccessibleAbilityManagerService::OutsideTouch(int32_t windowId)
{
    (void)windowId;
}

void AccessibleAbilityManagerService::UpdateAbilities()
{
    HILOG_DEBUG("start.");
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService()
{
    HILOG_DEBUG("start.");
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
    for (auto& callback : accountData->GetStateCallbacks()) {
        callback->OnStateChanged(state);
    }
}

void AccessibleAbilityManagerService::UpdateInputFilter()
{
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector>& touchEventInjector)
{
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter>& keyEventFilter)
{
    keyEventFilter_ = keyEventFilter;
}

void AccessibleAbilityManagerService::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Current account data is null");
        return;
    }
    accountData->RemoveStateCallback(remote);
}

void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    (void)accountId;
}

void AccessibleAbilityManagerService::PackageChanged(const std::string& bundleName)
{
    (void)bundleName;
}
void AccessibleAbilityManagerService::PackageRemoved(const std::string& bundleName)
{
    (void)bundleName;
}

AccessibilityConfig::CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    HILOG_DEBUG("start");
    AccessibilityConfig::CaptionProperty cp;
    return cp;
}

bool AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    (void)caption;
    return true;
}

void AccessibleAbilityManagerService::UpdateCaptionProperty()
{
    HILOG_DEBUG("start.");

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    AccessibilityConfig::CaptionProperty caption = accountData->GetCaptionProperty();
    for (auto& callback : accountData->GetCaptionPropertyCallbacks()) {
        callback->OnPropertyChanged(caption);
    }
}

bool AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return true;
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG("start");
    return true;
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG("start");
    return true;
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG("start");
    return true;
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG("start");
    return true;
}

bool AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    return false;
}

bool AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    HILOG_DEBUG("start");
    (void)enabledAbilities;
    return true;
}

bool AccessibleAbilityManagerService::GetInstalledAbilities(std::vector<AccessibilityAbilityInfo> &installedAbilities)
{
    HILOG_DEBUG("start");
    (void)installedAbilities;
    return true;
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_DEBUG("start");
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return ERR_INVALID_VALUE;
    }

    if (!captionPropertyCallbackDeathRecipient_) {
        captionPropertyCallbackDeathRecipient_ = new CaptionPropertyCallbackDeathRecipient();
    }
    if (callback != nullptr) {
        callback->AsObject()->AddDeathRecipient(captionPropertyCallbackDeathRecipient_);
        accountData->AddCaptionPropertyCallback(callback);
    }

    HILOG_INFO("AccessibleAbilityManagerService::RegisterCaptionPropertyCallback successfully");

    return NO_ERROR;
}

void AccessibleAbilityManagerService::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start");
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

bool AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    (void)name;
    return false;
}

RetError AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject>& obj)
{
    (void)obj;
    return RET_OK;
}

bool AccessibleAbilityManagerService::DisableUITestAbility()
{
    return true;
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    return 0;
}

void AccessibleAbilityManagerService::PackageAdd(const std::string& bundleName)
{
    (void)bundleName;
}

int32_t AccessibleAbilityManagerService::GetOsAccountId()
{
    constexpr int32_t TEMP_ACCOUNT_ID = 100;
    return TEMP_ACCOUNT_ID;
}

bool AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start");
    (void)time;
    return false;
}

bool AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start");
    (void)name;
    return false;
}

bool AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start");
    (void)state;
    return false;
}

bool AccessibleAbilityManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    HILOG_DEBUG("start");
    (void)filter;
    return false;
}

bool AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start");
    (void)time;
    return false;
}

bool AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start");
    (void)discount;
    return false;
}

bool AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start");
    (void)balance;
    return false;
}

bool AccessibleAbilityManagerService::GetScreenMagnificationState()
{
    HILOG_DEBUG("start");
    return false;
}

bool AccessibleAbilityManagerService::GetShortKeyState()
{
    HILOG_DEBUG("start");
    return false;
}

bool AccessibleAbilityManagerService::GetMouseKeyState()
{
    HILOG_DEBUG("start");
    return false;
}

int32_t AccessibleAbilityManagerService::GetMouseAutoClick()
{
    return 0 ;
}

std::string AccessibleAbilityManagerService::GetShortkeyTarget()
{
    HILOG_DEBUG("start");
    std::string a = "";
    return a ;
}

bool AccessibleAbilityManagerService::GetHighContrastTextState()
{
    HILOG_DEBUG("start");
    return true;
}

bool AccessibleAbilityManagerService::GetInvertColorState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetAnimationOffState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetAudioMonoState()
{
    HILOG_DEBUG("start");
    return true;
}

uint32_t AccessibleAbilityManagerService::GetDaltonizationColorFilter()
{
    return 0;
}

uint32_t AccessibleAbilityManagerService::GetContentTimeout()
{
    return 0;
}

float AccessibleAbilityManagerService::GetBrightnessDiscount()
{
    return 0;
}

float AccessibleAbilityManagerService::GetAudioBalance()
{
    return 0;
}

void AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    (void)observer;
}

uint32_t AccessibleAbilityManagerService::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    HILOG_DEBUG("start");
    if (!callback || !handler_) {
        HILOG_ERROR("Parameters check failed! callback:%{public}p, handler:%{public}p",
            callback.GetRefPtr(), handler_.get());
        return ERR_INVALID_VALUE;
    }

    std::promise<uint32_t> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, callback]() -> void {
        HILOG_DEBUG("start");
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
    HILOG_DEBUG("start");
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

void AccessibleAbilityManagerService::AddedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);
}

void AccessibleAbilityManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG("accountId(%{public}d)", accountId);
}
} // namespace Accessibility
} // namespace OHOS