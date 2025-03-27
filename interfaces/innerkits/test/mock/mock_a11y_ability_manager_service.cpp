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
#include "a11y_element_info.h"
#include "a11y_display_manager.h"
#include "a11y_event_info.h"
#include "a11y_ut_helper.h"
#include "a11y_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "utils.h"

namespace OHOS {
namespace A11y {
namespace {
    constexpr int32_t ACCOUNT_ID = 100;
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<A11yManagerService>::GetInstance());

A11yManagerService::A11yManagerService()
    : SystemAbility(A11y_MANAGER_SERVICE_ID, true)
{
}

A11yManagerService::~A11yManagerService()
{
}

void A11yManagerService::OnStart()
{
    GTEST_LOG_(INFO) << "###A11yManagerService::OnStart";
    runner_ = AppExecFwk::EventRunner::Create("A11yManagerService", AppExecFwk::ThreadMode::FFRT);
    handler_ = std::make_shared<AAMSEventHandler>(runner_);
    Singleton<A11yWindowManager>::GetInstance().RegisterWindowListener(handler_);
    Singleton<A11yCommonEvent>::GetInstance().SubscriberEvent(handler_);
}

void A11yManagerService::OnStop()
{
    Singleton<A11yCommonEvent>::GetInstance().UnSubscriberEvent();
    Singleton<A11yWindowManager>::GetInstance().DeregisterWindowListener();
    runner_.reset();
    handler_.reset();
}

void A11yManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

void A11yManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

int A11yManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    (void)fd;
    (void)args;
    return 0;
}

RetError A11yManagerService::SendEvent(const A11yEventInfo& uiEvent, const int32_t flag)
{
    HILOG_INFO("A11yManagerService::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    A11yAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    A11yAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    A11yAbilityHelper::GetInstance().SetEventWindowId(uiEvent.GetWindowId());
    A11yAbilityHelper::GetInstance().SetEventWindowChangeType(uiEvent.GetWindowChangeTypes());
    GTEST_LOG_(INFO) << "###A11yManagerService::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###A11yManagerService::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        A11yAbilityHelper::GetInstance().AddSendEventTimes();
        }), "TASK_SEND_EVENT");
    return RET_OK;
}

uint32_t A11yManagerService::RegisterStateObserver(
    const sptr<IA11yManagerStateObserver>& callback)
{
    (void)callback;
    return 0;
}

RetError A11yManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<A11yAbilityInfo> &infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return RET_OK;
}

RetError A11yManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IA11yElementOperator>& operation, bool isApp)
{
    (void)windowId;
    (void)operation;
    (void)isApp;
    return RET_OK;
}

RetError A11yManagerService::RegisterElementOperator(Registration parameter,
    const sptr<IA11yElementOperator> &operation, bool isApp)
{
    (void)parameter;
    (void)operation;
    (void)isApp;
    return RET_OK;
}

RetError A11yManagerService::DeregisterElementOperator(int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

RetError A11yManagerService::DeregisterElementOperator(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return RET_OK;
}

sptr<A11yAccountData> A11yManagerService::GetCurrentAccountData()
{
    bool needNullFlag = A11yAbilityHelper::GetInstance().GetNeedAccountDataNullFlag();
    if (needNullFlag) {
        return nullptr;
    }

    auto accountData = a11yAccountsData_.GetCurrentAccountData(ACCOUNT_ID);
    accountData->Init();
    return accountData;
}

sptr<A11yAccountData> A11yManagerService::GetAccountData(int32_t accountId)
{
    return a11yAccountsData_.GetAccountData(accountId);
}

RetError A11yManagerService::GetCaptionProperty(A11yConfig::CaptionProperty &caption)
{
    (void)caption;
    return RET_OK;
}

RetError A11yManagerService::SetCaptionProperty(const A11yConfig::CaptionProperty& caption)
{
    (void)caption;
    return RET_OK;
}

RetError A11yManagerService::SetCaptionState(const bool state)
{
    (void)state;
    return RET_OK;
}

bool A11yManagerService::GetEnabledState()
{
    return true;
}

RetError A11yManagerService::GetCaptionState(bool &state)
{
    state = true;
    return RET_OK;
}

bool A11yManagerService::GetTouchGuideState()
{
    return true;
}

bool A11yManagerService::GetGestureState()
{
    return true;
}

bool A11yManagerService::GetKeyEventObserverState()
{
    return true;
}

RetError A11yManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    return RET_OK;
}

RetError A11yManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return RET_OK;
}

uint32_t A11yManagerService::RegisterCaptionObserver(
    const sptr<IA11yManagerCaptionObserver>& callback)
{
    (void)callback;
    return NO_ERROR;
}

RetError A11yManagerService::SetCurtainScreenUsingStatus(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

RetError A11yManagerService::DisableAbility(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError A11yManagerService::EnableUITestAbility(const sptr<IRemoteObject>& obj)
{
    (void)obj;
    return RET_OK;
}

RetError A11yManagerService::DisableUITestAbility()
{
    return RET_OK;
}

int32_t A11yManagerService::GetActiveWindow()
{
    return 0;
}

RetError A11yManagerService::SetScreenMagnificationState(const bool state)
{
    (void)state;
    return RET_OK;
}

void A11yManagerService::UpdateAllSetting()
{
    return;
}

void A11yManagerService::UpdateInputFilter()
{
    return;
}

void A11yManagerService::UpdateShortKeyRegister()
{
    return;
}

RetError A11yManagerService::SetShortKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetMouseKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetMouseAutoClick(const int32_t time)
{
    (void)time;
    return RET_OK;
}

RetError A11yManagerService::SetShortkeyTarget(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError A11yManagerService::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    (void)name;
    return RET_OK;
}

RetError A11yManagerService::SetHighContrastTextState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetDaltonizationState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetInvertColorState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetAnimationOffState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetAudioMonoState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    (void)filter;
    return RET_OK;
}

RetError A11yManagerService::SetContentTimeout(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError A11yManagerService::SetBrightnessDiscount(const float discount)
{
    (void)discount;
    return RET_OK;
}

RetError A11yManagerService::SetAudioBalance(const float balance)
{
    (void)balance;
    return RET_OK;
}

RetError A11yManagerService::SetClickResponseTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError A11yManagerService::SetIgnoreRepeatClickState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError A11yManagerService::SetIgnoreRepeatClickTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError A11yManagerService::GetScreenMagnificationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetShortKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetMouseKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetMouseAutoClick(int32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError A11yManagerService::GetShortkeyTarget(std::string &name)
{
    name = "";
    return RET_OK;
}

RetError A11yManagerService::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    std::vector<std::string> vecName {};
    name = vecName;
    return RET_OK;
}

RetError A11yManagerService::GetHighContrastTextState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetDaltonizationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetInvertColorState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetAnimationOffState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetAudioMonoState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetDaltonizationColorFilter(uint32_t &type)
{
    type = 0;
    return RET_OK;
}

RetError A11yManagerService::GetContentTimeout(uint32_t &timer)
{
    timer = 0;
    return RET_OK;
}

RetError A11yManagerService::GetBrightnessDiscount(float &brightness)
{
    brightness = 0.0f;
    return RET_OK;
}

RetError A11yManagerService::GetAudioBalance(float &balance)
{
    balance = 0.0f;
    return RET_OK;
}

RetError A11yManagerService::GetClickResponseTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError A11yManagerService::GetIgnoreRepeatClickState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError A11yManagerService::GetIgnoreRepeatClickTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
}

void A11yManagerService::GetAllConfigs(A11yConfigData &configData)
{
    (void)configData;
}

void A11yManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IA11yEnableAbilityListsObserver> &observer)
{
    (void)observer;
}

uint32_t A11yManagerService::RegisterConfigObserver(
    const sptr<IA11yManagerConfigObserver> &callback)
{
    (void)callback;
    return NO_ERROR;
}

void A11yManagerService::UpdateA11yManagerService()
{
}

sptr<AppExecFwk::IBundleMgr> A11yManagerService::GetBundleMgrProxy()
{
    return nullptr;
}

void A11yManagerService::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

bool A11yManagerService::EnableShortKeyTargetAbility(const std::string &name)
{
    A11yAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
    return true;
}

void A11yManagerService::OnShortKeyProcess()
{
    A11yAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
}

void A11yManagerService::AddedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().AddUserId(accountId);
}
void A11yManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().RemoveUserId(accountId);
}
void A11yManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().SetCurrentUserId(accountId);
}
void A11yManagerService::PackageChanged(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().ChangePackage(true);
}
void A11yManagerService::PackageRemoved(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().RemovePackage(bundleName);
}
void A11yManagerService::PackageAdd(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().AddPackage(bundleName);
}

void A11yManagerService::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
}

void A11yManagerService::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid)
{
}

bool A11yManagerService::ExecuteActionOnA11yFocused(const ActionType &action)
{
    return true;
}

bool A11yManagerService::FindFocusedElement(A11yElementInfo &elementInfo)
{
    return true;
}

RetError A11yManagerService::GetFocusedWindowId(int32_t &focusedWindowId)
{
    focusedWindowId = 1;
    return RET_OK;
}

void A11yManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void A11yManagerService::InsertWindowIdEventPair(int32_t windowId, const A11yEventInfo &event)
{
    (void)windowId;
    (void)event;
}

bool A11yManagerService::CheckWindowRegister(int32_t windowId)
{
    (void)windowId;
    return true;
}

void A11yManagerService::PostDelayUnloadTask()
{
}

void A11yManagerService::SetFocusWindowId(const int32_t focusWindowId)
{
    (void)focusWindowId;
}

void A11yManagerService::SetFocusElementId(const int64_t focusElementId)
{
    (void)focusElementId;
}

int32_t A11yManagerService::GetTreeIdBySplitElementId(const int64_t elementId)
{
    (void)elementId;
    return 0;
}

void A11yManagerService::RemoveRequestId(int32_t requestId)
{
    (void)requestId;
}

void A11yManagerService::AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
    sptr<IA11yElementOperatorCallback> callback)
{
    (void)windowId;
    (void)treeId;
    (void)requestId;
    (void)callback;
}

int64_t A11yManagerService::GetRootParentId(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return 0;
}

RetError A11yManagerService::GetAllTreeId(int32_t windowId, std::vector<int32_t> &treeIds)
{
    (void)windowId;
    (void)treeIds;
    return RET_OK;
}

void A11yManagerService::OnDataClone()
{
}
} // namespace A11y
} // namespace OHOS