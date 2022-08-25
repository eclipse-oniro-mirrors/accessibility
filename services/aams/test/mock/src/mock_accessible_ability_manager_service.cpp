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
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t ACCOUNT_ID = 100;
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<AccessibleAbilityManagerService>::GetInstance());

AccessibleAbilityManagerService::AccessibleAbilityManagerService()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
}

void AccessibleAbilityManagerService::OnStart()
{
    runner_ = AppExecFwk::EventRunner::Create("AccessibleAbilityManagerService");
    handler_ = std::make_shared<AAMSEventHandler>(runner_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
}

void AccessibleAbilityManagerService::OnStop()
{
}

void AccessibleAbilityManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

int AccessibleAbilityManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    (void)fd;
    (void)args;
    return 0;
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

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        AccessibilityAbilityHelper::GetInstance().AddSendEventTimes();
        }), "TASK_SEND_EVENT");
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    (void)callback;
    return 0;
}

bool AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return true;
}

void AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& operation)
{
    (void)windowId;
    (void)operation;
}

void AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    (void)windowId;
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    auto iter = a11yAccountsData_.find(ACCOUNT_ID);
    if (iter != a11yAccountsData_.end()) {
        return iter->second;
    }
    sptr<AccessibilityAccountData> accountData = new AccessibilityAccountData(ACCOUNT_ID);
    accountData->Init();
    a11yAccountsData_[ACCOUNT_ID] = accountData;
    return accountData;
}

AccessibilityConfig::CaptionProperty AccessibleAbilityManagerService::GetCaptionProperty()
{
    AccessibilityConfig::CaptionProperty cp;
    return cp;
}

void AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    (void)caption;
}

void AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    (void)state;
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetCaptionState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    return true;
}

bool AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    return true;
}

bool AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return true;
}

bool AccessibleAbilityManagerService::GetInstalledAbilities(std::vector<AccessibilityAbilityInfo> &installedAbilities)
{
    (void)installedAbilities;
    return true;
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    (void)callback;
    return NO_ERROR;
}

bool AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    (void)name;
    return true;
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

void AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    (void)time;
}

void AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    (void)name;
}

void AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    (void)state;
}

void AccessibleAbilityManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    (void)filter;
}

void AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    (void)time;
}

void AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    (void)discount;
}

void AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    (void)balance;
}

bool AccessibleAbilityManagerService::GetScreenMagnificationState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetShortKeyState()
{
    return true;
}

bool AccessibleAbilityManagerService::GetMouseKeyState()
{
    return true;
}

int32_t AccessibleAbilityManagerService::GetMouseAutoClick()
{
    return 0;
}

std::string AccessibleAbilityManagerService::GetShortkeyTarget()
{
    return "";
}

bool AccessibleAbilityManagerService::GetHighContrastTextState()
{
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
    return 0.0f;
}

float AccessibleAbilityManagerService::GetAudioBalance()
{
    return 0.0f;
}

void AccessibleAbilityManagerService::GetAllConfigs(AccessibilityConfigData &configData)
{
    (void)configData;
}

void AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    (void)observer;
}

uint32_t AccessibleAbilityManagerService::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    (void)callback;
    return NO_ERROR;
}

void AccessibleAbilityManagerService::UpdateAccessibilityManagerService()
{
}

sptr<AppExecFwk::IBundleMgr> AccessibleAbilityManagerService::GetBundleMgrProxy()
{
    return nullptr;
}

void AccessibleAbilityManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

bool AccessibleAbilityManagerService::EnableShortKeyTargetAbility()
{
    AccessibilityAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
    return true;
}
} // namespace Accessibility
} // namespace OHOS
