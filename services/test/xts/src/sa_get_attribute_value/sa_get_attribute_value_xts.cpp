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
    SystemAbility::MakeAndRegisterAbility(&Singleton<SaGetAttributeValueXts>::GetInstance());

SaGetAttributeValueXts::SaGetAttributeValueXts()
    : SystemAbility(A11y_MANAGER_SERVICE_ID, true)
{
}

SaGetAttributeValueXts::~SaGetAttributeValueXts()
{
}

void SaGetAttributeValueXts::OnStart()
{
    GTEST_LOG_(INFO) << "###SaGetAttributeValueXts::OnStart";
    runner_ = AppExecFwk::EventRunner::Create("SaGetAttributeValueXts", AppExecFwk::ThreadMode::FFRT);
    handler_ = std::make_shared<AAMSEventHandler>(runner_);
    Singleton<A11yWindowManager>::GetInstance().RegisterWindowListener(handler_);
    Singleton<A11yCommonEvent>::GetInstance().SubscriberEvent(handler_);
}

void SaGetAttributeValueXts::OnStop()
{
    Singleton<A11yCommonEvent>::GetInstance().UnSubscriberEvent();
    Singleton<A11yWindowManager>::GetInstance().DeregisterWindowListener();
    runner_.reset();
    handler_.reset();
}

void SaGetAttributeValueXts::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

void SaGetAttributeValueXts::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

int SaGetAttributeValueXts::Dump(int fd, const std::vector<std::u16string>& args)
{
    (void)fd;
    (void)args;
    return 0;
}

RetError SaGetAttributeValueXts::SendEvent(const A11yEventInfo& uiEvent, const int32_t flag)
{
    HILOG_INFO("SaGetAttributeValueXts::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    A11yAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    A11yAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    A11yAbilityHelper::GetInstance().SetEventWindowId(uiEvent.GetWindowId());
    A11yAbilityHelper::GetInstance().SetEventWindowChangeType(uiEvent.GetWindowChangeTypes());
    GTEST_LOG_(INFO) << "###SaGetAttributeValueXts::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###SaGetAttributeValueXts::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        A11yAbilityHelper::GetInstance().AddSendEventTimes();
        }), "TASK_SEND_EVENT");
    return RET_OK;
}

RetError SaGetAttributeValueXts::RegisterElementOperator(Registration parameter,
    const sptr<IA11yElementOperator> &operation, bool isApp)
{
    (void)parameter;
    (void)operation;
    (void)isApp;
    return RET_OK;
}

RetError SaGetAttributeValueXts::DeregisterElementOperator(int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

RetError SaGetAttributeValueXts::DeregisterElementOperator(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return RET_OK;
}

sptr<A11yAccountData> SaGetAttributeValueXts::GetCurrentAccountData()
{
    bool needNullFlag = A11yAbilityHelper::GetInstance().GetNeedAccountDataNullFlag();
    if (needNullFlag) {
        return nullptr;
    }

    auto accountData = a11yAccountsData_.GetCurrentAccountData(ACCOUNT_ID);
    accountData->Init();
    return accountData;
}

sptr<A11yAccountData> SaGetAttributeValueXts::GetAccountData(int32_t accountId)
{
    return a11yAccountsData_.GetAccountData(accountId);
}

RetError SaGetAttributeValueXts::GetCaptionProperty(A11yConfig::CaptionProperty &caption)
{
    (void)caption;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetCaptionProperty(const A11yConfig::CaptionProperty& caption)
{
    (void)caption;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetCaptionState(const bool state)
{
    (void)state;
    return RET_OK;
}

bool SaGetAttributeValueXts::GetEnabledState()
{
    return true;
}

RetError SaGetAttributeValueXts::GetCaptionState(bool &state)
{
    state = true;
    return RET_OK;
}

bool SaGetAttributeValueXts::GetTouchGuideState()
{
    return true;
}

bool SaGetAttributeValueXts::GetGestureState()
{
    return true;
}

bool SaGetAttributeValueXts::GetKeyEventObserverState()
{
    return true;
}

RetError SaGetAttributeValueXts::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return RET_OK;
}

uint32_t SaGetAttributeValueXts::RegisterCaptionObserver(
    const sptr<IA11yManagerCaptionObserver>& callback)
{
    (void)callback;
    return NO_ERROR;
}

RetError SaGetAttributeValueXts::SetCurtainScreenUsingStatus(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

RetError SaGetAttributeValueXts::DisableAbility(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError SaGetAttributeValueXts::EnableUITestAbility(const sptr<IRemoteObject>& obj)
{
    (void)obj;
    return RET_OK;
}

RetError SaGetAttributeValueXts::DisableUITestAbility()
{
    return RET_OK;
}

int32_t SaGetAttributeValueXts::GetActiveWindow()
{
    return 0;
}

RetError SaGetAttributeValueXts::SetScreenMagnificationState(const bool state)
{
    (void)state;
    return RET_OK;
}

void SaGetAttributeValueXts::UpdateAllSetting()
{
    return;
}

void SaGetAttributeValueXts::UpdateInputFilter()
{
    return;
}

void SaGetAttributeValueXts::UpdateShortKeyRegister()
{
    return;
}

RetError SaGetAttributeValueXts::SetShortKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetMouseKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetMouseAutoClick(const int32_t time)
{
    (void)time;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetShortkeyTarget(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    (void)name;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetHighContrastTextState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetDaltonizationState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetInvertColorState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetAnimationOffState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetAudioMonoState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetDaltonizationColorFilter(const  uint32_t filter)
{
    (void)filter;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetContentTimeout(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetBrightnessDiscount(const float discount)
{
    (void)discount;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetAudioBalance(const float balance)
{
    (void)balance;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetClickResponseTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetIgnoreRepeatClickState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError SaGetAttributeValueXts::SetIgnoreRepeatClickTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetScreenMagnificationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetShortKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetMouseKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetMouseAutoClick(int32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetShortkeyTarget(std::string &name)
{
    name = "";
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    std::vector<std::string> vecName {};
    name = vecName;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetHighContrastTextState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetDaltonizationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetInvertColorState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetAnimationOffState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetAudioMonoState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetDaltonizationColorFilter(uint32_t &type)
{
    type = 0;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetContentTimeout(uint32_t &timer)
{
    timer = 0;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetBrightnessDiscount(float &brightness)
{
    brightness = 0.0f;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetAudioBalance(float &balance)
{
    balance = 0.0f;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetClickResponseTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetIgnoreRepeatClickState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError SaGetAttributeValueXts::GetIgnoreRepeatClickTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
}

void SaGetAttributeValueXts::GetAllConfigs(A11yConfigData &configData)
{
    (void)configData;
}

void SaGetAttributeValueXts::RegisterEnableAbilityListsObserver(
    const sptr<IA11yEnableAbilityListsObserver> &observer)
{
    (void)observer;
}

uint32_t SaGetAttributeValueXts::RegisterConfigObserver(
    const sptr<IA11yManagerConfigObserver> &callback)
{
    (void)callback;
    return NO_ERROR;
}

void SaGetAttributeValueXts::UpdateSaGetAttributeValueXts()
{
}

sptr<AppExecFwk::IBundleMgr> SaGetAttributeValueXts::GetBundleMgrProxy()
{
    return nullptr;
}

void SaGetAttributeValueXts::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

bool SaGetAttributeValueXts::EnableShortKeyTargetAbility(const std::string &name)
{
    A11yAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
    return true;
}

void SaGetAttributeValueXts::OnShortKeyProcess()
{
    A11yAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
}

void SaGetAttributeValueXts::AddedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().AddUserId(accountId);
}
void SaGetAttributeValueXts::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().RemoveUserId(accountId);
}
void SaGetAttributeValueXts::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().SetCurrentUserId(accountId);
}
void SaGetAttributeValueXts::PackageChanged(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().ChangePackage(true);
}
void SaGetAttributeValueXts::PackageRemoved(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().RemovePackage(bundleName);
}
void SaGetAttributeValueXts::PackageAdd(const std::string &bundleName)
{
    HILOG_DEBUG();
    A11yAbilityHelper::GetInstance().AddPackage(bundleName);
}

void SaGetAttributeValueXts::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
}

void SaGetAttributeValueXts::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid)
{
}

bool SaGetAttributeValueXts::ExecuteActionOnA11yFocused(const ActionType &action)
{
    return true;
}

bool SaGetAttributeValueXts::FindFocusedElement(A11yElementInfo &elementInfo)
{
    return true;
}

RetError SaGetAttributeValueXts::GetFocusedWindowId(int32_t &focusedWindowId)
{
    focusedWindowId = 1;
    return RET_OK;
}

void SaGetAttributeValueXts::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void SaGetAttributeValueXts::InsertWindowIdEventPair(int32_t windowId, const A11yEventInfo &event)
{
    (void)windowId;
    (void)event;
}

bool SaGetAttributeValueXts::CheckWindowRegister(int32_t windowId)
{
    (void)windowId;
    return true;
}

void SaGetAttributeValueXts::PostDelayUnloadTask()
{
}

void SaGetAttributeValueXts::SetFocusWindowId(const int32_t focusWindowId)
{
    (void)focusWindowId;
}

void SaGetAttributeValueXts::SetFocusElementId(const int64_t focusElementId)
{
    (void)focusElementId;
}

int32_t SaGetAttributeValueXts::GetTreeIdBySplitElementId(const int64_t elementId)
{
    (void)elementId;
    return 0;
}

void SaGetAttributeValueXts::RemoveRequestId(int32_t requestId)
{
    (void)requestId;
}

void SaGetAttributeValueXts::AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
    sptr<IA11yElementOperatorCallback> callback)
{
    (void)windowId;
    (void)treeId;
    (void)requestId;
    (void)callback;
}

int64_t SaGetAttributeValueXts::GetRootParentId(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return 0;
}

RetError SaGetAttributeValueXts::GetAllTreeId(int32_t windowId, std::vector<int32_t> &treeIds)
{
    (void)windowId;
    (void)treeIds;
    return RET_OK;
}

void SaGetAttributeValueXts::OnDataClone()
{
}
} // namespace A11y
} // namespace OHOS