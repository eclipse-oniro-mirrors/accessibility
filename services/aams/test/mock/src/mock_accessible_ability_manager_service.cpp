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
#include "accessibility_element_info.h"
#include "accessibility_display_manager.h"
#include "accessibility_event_info.h"
#include "accessibility_ut_helper.h"
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
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::OnStart";
    runner_ = AppExecFwk::EventRunner::Create("AccessibleAbilityManagerService", AppExecFwk::ThreadMode::FFRT);
    handler_ = std::make_shared<AAMSEventHandler>(runner_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(handler_);
    channelRunner_ = AppExecFwk::EventRunner::Create("AamsChannelRunner", AppExecFwk::ThreadMode::FFRT);
}

void AccessibleAbilityManagerService::OnStop()
{
    Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
    Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();
    runner_.reset();
    handler_.reset();
    channelRunner_.reset();
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

RetError AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfo& uiEvent, const int32_t flag)
{
    HILOG_INFO("AccessibleAbilityManagerService::SendEvent successfully");
    EventType uTeventType = uiEvent.GetEventType();
    AccessibilityAbilityHelper::GetInstance().SetGestureId(uiEvent.GetGestureType());
    AccessibilityAbilityHelper::GetInstance().SetEventTypeVector(uTeventType);
    AccessibilityAbilityHelper::GetInstance().SetEventWindowId(uiEvent.GetWindowId());
    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(uiEvent.GetWindowChangeTypes());
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent GetGestureType="
                     << (int32_t)uiEvent.GetGestureType();
    GTEST_LOG_(INFO) << "###AccessibleAbilityManagerService::SendEvent uTeventType=0x" << std::hex
                     << (int32_t)uTeventType;

    handler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("start");
        AccessibilityAbilityHelper::GetInstance().AddSendEventTimes();
        }), "TASK_SEND_EVENT");
    return RET_OK;
}

uint32_t AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    (void)callback;
    return 0;
}

RetError AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    (void)abilityTypes;
    (void)stateType;
    (void)infos;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& operation, bool isApp)
{
    (void)windowId;
    (void)operation;
    (void)isApp;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::RegisterElementOperator(Registration parameter,
    const sptr<IAccessibilityElementOperator> &operation, bool isApp)
{
    (void)parameter;
    (void)operation;
    (void)isApp;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId)
{
    (void)windowId;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return RET_OK;
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    bool needNullFlag = AccessibilityAbilityHelper::GetInstance().GetNeedAccountDataNullFlag();
    if (needNullFlag) {
        return nullptr;
    }

    auto accountData = a11yAccountsData_.GetCurrentAccountData(ACCOUNT_ID);
    accountData->Init();
    return accountData;
}

sptr<AccessibilityAccountData> AccessibleAbilityManagerService::GetAccountData(int32_t accountId)
{
    return a11yAccountsData_.GetAccountData(accountId);
}

RetError AccessibleAbilityManagerService::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    (void)caption;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    (void)caption;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    (void)state;
    return RET_OK;
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    return true;
}

RetError AccessibleAbilityManagerService::GetCaptionState(bool &state)
{
    state = true;
    return RET_OK;
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

bool AccessibleAbilityManagerService::GetScreenReaderState()
{
    return true;
}

RetError AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    (void)name;
    (void)capabilities;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
{
    (void)enabledAbilities;
    return RET_OK;
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    (void)callback;
    return NO_ERROR;
}

RetError AccessibleAbilityManagerService::SetCurtainScreenUsingStatus(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject>& obj)
{
    (void)obj;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DisableUITestAbility()
{
    return RET_OK;
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    return 0;
}

RetError AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    (void)state;
    return RET_OK;
}

void AccessibleAbilityManagerService::UpdateAllSetting()
{
    return;
}

void AccessibleAbilityManagerService::UpdateInputFilter()
{
    return;
}

void AccessibleAbilityManagerService::UpdateShortKeyRegister()
{
    return;
}

RetError AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    (void)time;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    (void)name;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    (void)name;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetDaltonizationState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetDaltonizationColorFilter(const  uint32_t filter)
{
    (void)filter;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    (void)discount;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    (void)balance;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetClickResponseTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetIgnoreRepeatClickState(const bool state)
{
    (void)state;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::SetIgnoreRepeatClickTime(const uint32_t time)
{
    (void)time;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetScreenMagnificationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetShortKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetMouseKeyState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetMouseAutoClick(int32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetShortkeyTarget(std::string &name)
{
    name = "";
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    std::vector<std::string> vecName {};
    name = vecName;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetHighContrastTextState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetDaltonizationState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetInvertColorState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetAnimationOffState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetAudioMonoState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetDaltonizationColorFilter(uint32_t &type)
{
    type = 0;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetContentTimeout(uint32_t &timer)
{
    timer = 0;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetBrightnessDiscount(float &brightness)
{
    brightness = 0.0f;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetAudioBalance(float &balance)
{
    balance = 0.0f;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetClickResponseTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetIgnoreRepeatClickState(bool &state)
{
    state = true;
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetIgnoreRepeatClickTime(uint32_t &time)
{
    time = 0;
    return RET_OK;
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

bool AccessibleAbilityManagerService::EnableShortKeyTargetAbility(const std::string &name)
{
    AccessibilityAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
    return true;
}

void AccessibleAbilityManagerService::OnShortKeyProcess()
{
    AccessibilityAbilityHelper::GetInstance().SetShortKeyTargetAbilityState(true);
}

void AccessibleAbilityManagerService::AddedUser(int32_t accountId)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().AddUserId(accountId);
}
void AccessibleAbilityManagerService::RemovedUser(int32_t accountId)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().RemoveUserId(accountId);
}
void AccessibleAbilityManagerService::SwitchedUser(int32_t accountId)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().SetCurrentUserId(accountId);
}
void AccessibleAbilityManagerService::PackageChanged(const std::string &bundleName)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().ChangePackage(true);
}
void AccessibleAbilityManagerService::PackageRemoved(const std::string &bundleName)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().RemovePackage(bundleName);
}
void AccessibleAbilityManagerService::PackageAdd(const std::string &bundleName)
{
    HILOG_DEBUG();
    AccessibilityAbilityHelper::GetInstance().AddPackage(bundleName);
}

void AccessibleAbilityManagerService::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
}

void AccessibleAbilityManagerService::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid)
{
}

bool AccessibleAbilityManagerService::ExecuteActionOnAccessibilityFocused(const ActionType &action)
{
    return true;
}

bool AccessibleAbilityManagerService::FindFocusedElement(AccessibilityElementInfo &elementInfo, uint32_t timeout)
{
    return true;
}

RetError AccessibleAbilityManagerService::GetFocusedWindowId(int32_t &focusedWindowId)
{
    focusedWindowId = 1;
    return RET_OK;
}

void AccessibleAbilityManagerService::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void AccessibleAbilityManagerService::InsertWindowIdEventPair(int32_t windowId, const AccessibilityEventInfo &event)
{
    (void)windowId;
    (void)event;
}

bool AccessibleAbilityManagerService::CheckWindowRegister(int32_t windowId)
{
    (void)windowId;
    return true;
}

void AccessibleAbilityManagerService::PostDelayUnloadTask()
{
}

void AccessibleAbilityManagerService::SetFocusWindowId(const int32_t focusWindowId)
{
    (void)focusWindowId;
}

void AccessibleAbilityManagerService::SetFocusElementId(const int64_t focusElementId)
{
    (void)focusElementId;
}

int32_t AccessibleAbilityManagerService::GetTreeIdBySplitElementId(const int64_t elementId)
{
    (void)elementId;
    return 0;
}

void AccessibleAbilityManagerService::RemoveRequestId(int32_t requestId)
{
    (void)requestId;
}

void AccessibleAbilityManagerService::AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
    sptr<IAccessibilityElementOperatorCallback> callback)
{
    (void)windowId;
    (void)treeId;
    (void)requestId;
    (void)callback;
}

int64_t AccessibleAbilityManagerService::GetRootParentId(int32_t windowId, int32_t treeId)
{
    (void)windowId;
    (void)treeId;
    return 0;
}

RetError AccessibleAbilityManagerService::GetAllTreeId(int32_t windowId, std::vector<int32_t> &treeIds)
{
    (void)windowId;
    (void)treeIds;
    return RET_OK;
}

void AccessibleAbilityManagerService::OnDataClone()
{
}
} // namespace Accessibility
} // namespace OHOS