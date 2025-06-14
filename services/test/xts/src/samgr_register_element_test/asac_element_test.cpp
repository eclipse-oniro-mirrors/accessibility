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

#include <cinttypes>
#include <new>
#include <string>
#include <unistd.h>
#include <functional>
#ifdef OHOS_BUILD_ENABLE_HITRACE
#include <hitrace_meter.h>
#endif // OHOS_BUILD_ENABLE_HITRACE

#include "ability_info.h"
#include "a11yRegisterElement_event_info.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "a11yRegisterElement_power_manager.h"
#endif
#include "a11yRegisterElement_short_key_dialog.h"
#include "a11yRegisterElement_window_manager.h"
#include "hilog_wrapper.h"
#include "input_manager.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "parameter.h"
#include "system_ability_definition.h"
#include "utils.h"
#include "xcollie_helper.h"
#include <ipc_skeleton.h>
#include "transaction/rs_interfaces.h"

using namespace std;

namespace OHOS {
namespace A11yRegisterElement {
namespace {
    const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
    const std::string AAMS_ACTION_RUNNER_NAME = "AamsActionRunner";
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "a11yRegisterElement.config.ready";
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/A11yRegisterElementExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "a11yRegisterElement_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "a11yRegisterElement_magnification_capability";
    const std::string DELAY_UNLOAD_TASK = "TASK_UNLOAD_ACCESSIBILITY_SA";
    const std::string USER_SETUP_COMPLETED = "user_setup_complete";
    const std::string ACCESSIBILITY_CLONE_FLAG = "a11yRegisterElement_config_clone";
    const std::string SHORTCUT_ENABLED = "a11yRegisterElement_shortcut_enabled";
    constexpr int32_t INVALID_SHORTCUT_STATE = 2;
    constexpr int32_t QUERY_USER_ID_RETRY_COUNT = 600;
    constexpr int32_t QUERY_USER_ID_SLEEP_TIME = 50;
    constexpr uint32_t TIME_OUT_OPERATOR = 5000;
    constexpr int32_t REQUEST_ID_MAX = 0xFFFFFFFF;
    constexpr int32_t REQUEST_ID_MIN = 0x0000FFFF;
    constexpr int32_t DEFAULT_ACCOUNT_ID = 100;
    constexpr int32_t ROOT_UID = 0;
    constexpr int32_t UNLOAD_TASK_INTERNAL = 3 * 60 * 1000; // ms
    constexpr int32_t TREE_ID_INVALID = 0;
    constexpr uint32_t ELEMENT_MOVE_BIT = 40;
    constexpr int32_t SINGLE_TREE_ID = 0;
    constexpr int32_t TREE_ID_MAX = 0x00001FFF;
    constexpr int32_t SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    constexpr int32_t SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    constexpr int32_t WINDOW_ID_INVALID = -1;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
    enum SCREENREADER_STATE : int32_t {
        UNINIT = -1,
        OFF = 0,
        ON = 1,
    };
    const std::string TIMER_REGISTER_STATE_OBSERVER = "a11yRegisterElement:registerStateObServer";
    const std::string TIMER_REGISTER_CAPTION_OBSERVER = "a11yRegisterElement:registerCaptionObServer";
    const std::string TIMER_REGISTER_ENABLEABILITY_OBSERVER = "a11yRegisterElement:registerEnableAbilityObServer";
    const std::string TIMER_GET_ALL_CONFIG = "a11yRegisterElement:getAllConfig";
    const std::string TIMER_REGISTER_CONFIG_OBSERVER = "a11yRegisterElement:registerConfigObserver";
    constexpr int32_t XCOLLIE_TIMEOUT = 1; // s
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
    dependentServicesStatus_[DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID] = false;

    a11yRegisterElementSettings_ = std::make_shared<A11yRegisterElementSettings>();
    a11yRegisterElementShortKey_ = std::make_shared<A11yRegisterElementShortKey>();
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
    HILOG_INFO("AccessibleAbilityManagerService::~AccessibleAbilityManagerService");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    a11yAccountsData_.Clear();
}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME, AppExecFwk::ThreadMode::FFRT);
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

    if (!actionRunner_) {
        actionRunner_ = AppExecFwk::EventRunner::Create(AAMS_ACTION_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!actionRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS action runner failed");
            return;
        }
    }

    if (!actionHandler_) {
        actionHandler_ = std::make_shared<AAMSEventHandler>(actionRunner_);
        if (!actionHandler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS action handler failed");
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
    AddSystemAbilityListener(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);

    a11yRegisterElementSettings_->RegisterSettingsHandler(handler_);
}

void AccessibleAbilityManagerService::OnStop()
{
    HILOG_INFO("stop AccessibleAbilityManagerService");
    if (!handler_) {
        HILOG_ERROR("AccessibleAbilityManagerService::OnStop failed!");
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();

        Singleton<A11yRegisterElementCommonEvent>::GetInstance().UnSubscriberEvent();
        Singleton<A11yRegisterElementDisplayManager>::GetInstance().UnregisterDisplayListener();
        Singleton<A11yRegisterElementWindowManager>::GetInstance().DeregisterWindowListener();

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
        }, "TASK_ONSTOP");
    syncFuture.wait();

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

    handler_->PostTask([=]() {
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

        InitInnerResource();

        isReady_ = true;
        SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "true");
        HILOG_DEBUG("AAMS is ready!");
        RegisterShortKeyEvent();
        PostDelayUnloadTask();
        RegisterScreenMagnificationState();
        RegisterScreenMagnificationType();
        }, "OnAddSystemAbility");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_INFO("systemAbilityId:%{public}d removed!", systemAbilityId);
    if (!handler_) {
        HILOG_DEBUG("Event handler is nullptr.");
        return;
    }

    handler_->PostTask([=]() {
        HILOG_INFO("Remove system ability start");
        auto iter = dependentServicesStatus_.find(systemAbilityId);
        if (iter == dependentServicesStatus_.end()) {
            HILOG_ERROR("SystemAbilityId is not found!");
            return;
        }

        dependentServicesStatus_[systemAbilityId] = false;
        if (isReady_) {
            SwitchedUser(-1);
            Singleton<A11yRegisterElementCommonEvent>::GetInstance().UnSubscriberEvent();
            Singleton<A11yRegisterElementDisplayManager>::GetInstance().UnregisterDisplayListener();
            Singleton<A11yRegisterElementWindowManager>::GetInstance().DeregisterWindowListener();
            Singleton<A11yRegisterElementWindowManager>::GetInstance().DeInit();

            isReady_ = false;
            SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
        }
        }, "OnRemoveSystemAbility");
}

int AccessibleAbilityManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    HILOG_DEBUG("dump A11yRegisterElementManagerServiceInfo");
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    ffrt::promise<int> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, fd, args]() {
        if (!a11yRegisterElementDumper_) {
            a11yRegisterElementDumper_ = new(std::nothrow) A11yRegisterElementDumper();
            if (!a11yRegisterElementDumper_) {
                HILOG_ERROR("a11yRegisterElementDumper_ is nullptr");
                syncPromise.set_value(-1);
                return;
            }
        }
        syncPromise.set_value(a11yRegisterElementDumper_->Dump(fd, args));
        }, "TASK_DUMP_INFO");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::VerifyingToKenId(const int32_t windowId, const int64_t elementId)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t treeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
    HILOG_DEBUG("VerifyingToKenId: treeId[%{public}d], windowId[%{public}d], elementId[%{public}" PRId64 "]",
        treeId, windowId, elementId);
    if (elementId == ELEMENT_ID_INVALID || windowId == WINDOW_ID_INVALID) {
        HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
        return RET_OK;
    }
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return RET_ERR_CONNECTION_EXIST;
    }
    HILOG_DEBUG("treeId %{public}d, windowId %{public}d", treeId, windowId);
    int32_t realId =
        Singleton<A11yRegisterElementWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    sptr<A11yRegisterElementWindowConnection> connection = accountData->GetA11yRegisterElementWindowConnection(realId);
    if (connection == nullptr) {
        HILOG_ERROR("connection is empty.");
        return RET_ERR_REGISTER_EXIST;
    }
    uint32_t expectTokenId = connection->GetTokenIdMap(treeId);
    if (tokenId != expectTokenId) {
        HILOG_DEBUG("tokenId error!");
        return RET_ERR_TOKEN_ID;
    }

    return RET_OK;
}

RetError AccessibleAbilityManagerService::SendEvent(const A11yRegisterElementEventInfo &uiEvent, const int32_t flag)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d] windowId[%{public}d] compnentId: %{public}" PRId64 " "
        "elementId: %{public}" PRId64 " winId: %{public}d treeId: %{public}d",
        uiEvent.GetEventType(), uiEvent.GetGestureType(), uiEvent.GetWindowId(), uiEvent.GetA11yRegisterElementId(),
        uiEvent.GetElementInfo().GetA11yRegisterElementId(),
        uiEvent.GetElementInfo().GetWindowId(), uiEvent.GetElementInfo().GetBelongTreeId());
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    if (flag) {
        if (VerifyingToKenId(uiEvent.GetElementInfo().GetWindowId(),
            uiEvent.GetElementInfo().GetA11yRegisterElementId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_DEBUG("VerifyingToKenId failed");
            return RET_ERR_CONNECTION_EXIST;
        }
    }

    UpdateA11yRegisterElementWindowStateByEvent(uiEvent);
    handler_->PostTask([this, uiEvent]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }

        const_cast<A11yRegisterElementEventInfo&>(uiEvent).SetTimeStamp(Utils::GetSystemTime());
        map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
        for (auto &ability : abilities) {
            if (ability.second) {
                ability.second->OnA11yRegisterElementEvent(const_cast<A11yRegisterElementEventInfo&>(uiEvent));
            }
        }
        }, "TASK_SEND_EVENT");
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
    XCollieHelper timer(TIMER_REGISTER_STATE_OBSERVER, XCOLLIE_TIMEOUT);
    std::lock_guard<ffrt::mutex> lock(mutex_);
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
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        return 0;
    }

    return accountData->GetA11yRegisterElementState();
}

void AccessibleAbilityManagerService::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<A11yRegisterElementWindowManager>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
        syncPromise.set_value();
        }, "GET_REAL_WINDOW_AND_ELEMENT_ID");
    return syncFuture.get();
}

void AccessibleAbilityManagerService::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<A11yRegisterElementWindowManager>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        syncPromise.set_value();
        }, "GET_SCENE_BOARD_INNER_WINDOW_ID");
    return syncFuture.get();
}

sptr<A11yRegisterElementWindowConnection> AccessibleAbilityManagerService::GetRealIdConnection()
{
    HILOG_DEBUG();
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    int32_t realId = Singleton<A11yRegisterElementWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);

    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return sptr<A11yRegisterElementWindowConnection>();
    }
    return accountData->GetA11yRegisterElementWindowConnection(realId);
}

bool AccessibleAbilityManagerService::FindFocusedElementByConnection(sptr<A11yRegisterElementWindowConnection> connection,
    A11yRegisterElementElementInfo &elementInfo)
{
    HILOG_DEBUG();
    int64_t elementId = -1;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("GetA11yRegisterElementWindowConnection failed");
        return false;
    }

    uint32_t timeOut = 5000;
    sptr<ElementOperatorCallbackImpl> focusCallback = new(std::nothrow) ElementOperatorCallbackImpl();
    if (!focusCallback) {
        HILOG_ERROR("Failed to create focusCallback.");
        return false;
    }
    ffrt::future<void> focusFuture = focusCallback->promise_.get_future();
    connection->GetProxy()->FindFocusedElementInfo(elementId, focusType, GenerateRequestId(), focusCallback);
    ffrt::future_status waitFocus = focusFuture.wait_for(std::chrono::milliseconds(timeOut));
    if (waitFocus != ffrt::future_status::ready) {
        HILOG_ERROR("FindFocusedElementInfo Failed to wait result");
        return false;
    }
    elementInfo = focusCallback->a11yRegisterElementInfoResult_;
    return true;
}

bool AccessibleAbilityManagerService::FindFocusedElement(A11yRegisterElementElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<A11yRegisterElementWindowConnection> connection = GetRealIdConnection();
    FindFocusedElementByConnection(connection, elementInfo);
    if (elementInfo.GetA11yRegisterElementId() >= 0) {
        HILOG_DEBUG("find focused element success.");
        return true;
    }
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    sptr<IA11yRegisterElementElementOperator> elementOperator = nullptr;
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    connection = accountData->GetA11yRegisterElementWindowConnection(windowId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
    if (connection == nullptr) {
        HILOG_ERROR("connection is nullptr");
        return false;
    }
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl();
    if (callBack == nullptr) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }
    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    GetElementOperatorConnection(connection, elementId, elementOperator);
    if (elementOperator == nullptr) {
        HILOG_ERROR("elementOperator is nullptr");
        return false;
    }
    elementOperator->SearchElementInfoByA11yRegisterElementId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        return false;
    }

    if (callBack->elementInfosResult_.size() <= 0) {
        HILOG_ERROR("SearchElementInfoByA11yRegisterElementId return null");
        return false;
    }
    elementInfo = callBack->elementInfosResult_[0];
    return true;
}

void AccessibleAbilityManagerService::GetElementOperatorConnection(sptr<A11yRegisterElementWindowConnection> &connection,
    const int64_t elementId, sptr<IA11yRegisterElementElementOperator> &elementOperator)
{
    int32_t treeId = 0;
    if (elementId > 0) {
        treeId = GetTreeIdBySplitElementId(elementId);
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy();
    }
    HILOG_DEBUG("elementId:%{public}" PRId64 " treeId:%{public}d", elementId, treeId);
}

bool AccessibleAbilityManagerService::ExecuteActionOnA11yRegisterElementFocused(const ActionType &action)
{
    HILOG_DEBUG();
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    uint32_t timeOut = 5000;
    int32_t treeId = GetTreeIdBySplitElementId(elementId);
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    sptr<A11yRegisterElementWindowConnection> connection = accountData->GetA11yRegisterElementWindowConnection(windowId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "], action[%{public}d", windowId, elementId,
        action);
    if (connection == nullptr) {
        HILOG_ERROR("connection is nullptr");
        return false;
    }
    std::map<std::string, std::string> actionArguments {};
    sptr<ElementOperatorCallbackImpl> actionCallback = new(std::nothrow) ElementOperatorCallbackImpl();
    if (actionCallback == nullptr) {
        HILOG_ERROR("Failed to create actionCallback.");
        return false;
    }
    ffrt::future<void> actionFuture = actionCallback->promise_.get_future();
    if (treeId > TREE_ID_INVALID) {
        if (connection->GetCardProxy(treeId) != nullptr) {
            connection->GetCardProxy(treeId)->ExecuteAction(elementId, action,
                actionArguments, GenerateRequestId(), actionCallback);
        } else {
            HILOG_ERROR("get operation is nullptr");
            return false;
        }
    } else {
        if (connection->GetProxy() != nullptr) {
            connection->GetProxy()->ExecuteAction(elementId, action, actionArguments, GenerateRequestId(),
                actionCallback);
        } else {
            HILOG_ERROR("get operation is nullptr");
            return false;
        }
    }
    ffrt::future_status waitAction = actionFuture.wait_for(std::chrono::milliseconds(timeOut));
    if (waitAction != ffrt::future_status::ready) {
        HILOG_ERROR("ExecuteAction Failed to wait result");
        return false;
    }

    return actionCallback->executeActionResult_;
}

void AccessibleAbilityManagerService::SetFocusWindowId(const int32_t focusWindowId)
{
    focusWindowId_ = focusWindowId;
}

int32_t AccessibleAbilityManagerService::GetFocusWindowId()
{
    return focusWindowId_;
}

void AccessibleAbilityManagerService::SetFocusElementId(const int64_t focusElementId)
{
    focusElementId_ = focusElementId;
}

int64_t AccessibleAbilityManagerService::GetFocusElementId()
{
    return focusElementId_;
}

uint32_t AccessibleAbilityManagerService::RegisterCaptionObserver(
    const sptr<IAccessibleAbilityManagerCaptionObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !actionHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return ERR_INVALID_VALUE;
    }

    XCollieHelper timer(TIMER_REGISTER_CAPTION_OBSERVER, XCOLLIE_TIMEOUT);
    std::shared_ptr<ffrt::promise<uint32_t>> syncPromise = std::make_shared<ffrt::promise<uint32_t>>();
    ffrt::future syncFuture = syncPromise->get_future();
    actionHandler_->PostTask([this, syncPromise, callback]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
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
        }, "TASK_REGISTER_CAPTION_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterCaptionObserver result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IA11yRegisterElementEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    if (!observer || !actionHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return;
    }
    XCollieHelper timer(TIMER_REGISTER_ENABLEABILITY_OBSERVER, XCOLLIE_TIMEOUT);
    std::shared_ptr<ffrt::promise<void>> syncPromisePtr = std::make_shared<ffrt::promise<void>>();
    ffrt::future syncFuture = syncPromisePtr->get_future();
    actionHandler_->PostTask([this, syncPromisePtr, observer]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
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
        }, "TASK_REGISTER_ENABLE_ABILITY_LISTS_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterEnableAbilityListsObserver result");
        return;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<A11yRegisterElementAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d", stateType);
        return RET_ERR_INVALID_PARAM;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &infos, abilityTypes, stateType]() {
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Get current account data failed!!");
            syncPromise.set_value(RET_ERR_FAILED);
            return;
        }

        vector<A11yRegisterElementAbilityInfo> abilities;
        accountData->GetAbilitiesByState(static_cast<AbilityStateType>(stateType), abilities);
        HILOG_DEBUG("abilityes count is %{public}zu", abilities.size());
        for (auto &ability : abilities) {
            if (abilityTypes == A11yRegisterElementAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL ||
                (ability.GetA11yRegisterElementAbilityType() & abilityTypes)) {
                infos.push_back(ability);
            }
        }
        HILOG_DEBUG("infos count is %{public}zu", infos.size());
        syncPromise.set_value(RET_OK);
        }, "TASK_GET_ABILITY_LIST");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::RegisterElementOperator(
    const int32_t windowId, const sptr<IA11yRegisterElementElementOperator> &operation, bool isApp)
{
    if (!handler_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (CheckCallingUid() != RET_OK) {
        return RET_ERR_SAMGR;
    }
    handler_->PostTask([=]() {
        HILOG_INFO("Register windowId[%{public}d]", windowId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
        HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "RegisterElementOperator");
#endif // OHOS_BUILD_ENABLE_HITRACE
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return;
        }
        sptr<A11yRegisterElementWindowConnection> oldConnection = accountData->GetA11yRegisterElementWindowConnection(windowId);
        if (isApp && oldConnection) {
            HILOG_WARN("no need to register again.");
            return;
        }
        DeleteConnectionAndDeathRecipient(windowId, oldConnection);
        sptr<A11yRegisterElementWindowConnection> connection =
            new(std::nothrow) A11yRegisterElementWindowConnection(windowId, operation, currentAccountId_);
        if (!connection) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("New  A11yRegisterElementWindowConnection failed!!");
            return;
        }
        connection->SetTokenIdMap(SINGLE_TREE_ID, tokenId);
        accountData->AddA11yRegisterElementWindowConnection(windowId, connection);

        IsCheckWindowIdEventExist(windowId);
        if (operation && operation->AsObject()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient =
                new(std::nothrow) InteractionOperationDeathRecipient(windowId, currentAccountId_);
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
        }, "TASK_REGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

void AccessibleAbilityManagerService::IsCheckWindowIdEventExist(const int32_t windowId)
{
    if (CheckWindowIdEventExist(windowId)) {
        SendEvent(windowFocusEventMap_[windowId]);
        windowFocusEventMap_.erase(windowId);
    }
}

RetError AccessibleAbilityManagerService::RegisterElementOperatorChildWork(const RegistrationPara &parameter,
    const int32_t treeId, const int64_t nodeId, const sptr<IA11yRegisterElementElementOperator> &operation,
    const uint32_t tokenId, bool isApp)
{
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return RET_ERR_REGISTER_EXIST;
    }

    sptr<A11yRegisterElementWindowConnection> parentConnection =
        accountData->GetA11yRegisterElementWindowConnection(parameter.parentWindowId);
    if (isApp && parentConnection) {
        sptr<IA11yRegisterElementElementOperator> parentAamsOper =
            parentConnection->GetCardProxy(parameter.parentTreeId);
        if (parentAamsOper != nullptr) {
            parentAamsOper->SetChildTreeIdAndWinId(nodeId, treeId, parameter.windowId);
        } else {
            HILOG_DEBUG("parentAamsOper is nullptr");
        }
    } else {
        return RET_ERR_NO_CONNECTION;
    }

    operation->SetBelongTreeId(treeId);
    operation->SetParentWindowId(parameter.parentWindowId);
    sptr<A11yRegisterElementWindowConnection> oldConnection =
        accountData->GetA11yRegisterElementWindowConnection(parameter.windowId);
    if (isApp && oldConnection) {
        if (oldConnection->GetCardProxy(treeId) != nullptr) {
            HILOG_WARN("no need to register again.");
            return RET_ERR_REGISTER_EXIST;
        } else {
            oldConnection->SetCardProxy(treeId, operation);
            SetTokenIdMapAndRootParentId(oldConnection, treeId, nodeId, tokenId);
        }
    }
    return RET_OK;
}

void AccessibleAbilityManagerService::SetTokenIdMapAndRootParentId(
    const sptr<A11yRegisterElementWindowConnection> connection,
    const int32_t treeId, const int64_t nodeId, const uint32_t tokenId)
{
    connection->SetTokenIdMap(treeId, tokenId);
    connection->SetRootParentId(treeId, nodeId);
}

RetError AccessibleAbilityManagerService::RegisterElementOperator(RegistrationPara parameter,
    const sptr<IA11yRegisterElementElementOperator> &operation, bool isApp)
{
    if (CheckCallingUid() != RET_OK) {
        return RET_ERR_SAMGR;
    }
    static std::atomic<int32_t> treeId(1);
    int32_t treeIdSingle = treeId.fetch_add(1, std::memory_order_relaxed);
    if (treeIdSingle > TREE_ID_MAX) {
        HILOG_ERROR("TreeId more than 13.");
        return RET_ERR_TREE_TOO_BIG;
    }
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int64_t nodeId = parameter.elementId;
    HILOG_INFO("get treeId element and treeid - treeId: %{public}d parameter.elementId[%{public}" PRId64 "]"
        "element[%{public}" PRId64 "]",
        treeIdSingle, parameter.elementId, nodeId);

    if (!handler_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    handler_->PostTask([=]() {
        HILOG_INFO("Register windowId[%{public}d]", parameter.windowId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
        HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "RegisterElementOperator");
#endif // OHOS_BUILD_ENABLE_HITRACE
        if (RET_OK != RegisterElementOperatorChildWork(parameter, treeIdSingle, nodeId, operation, tokenId, isApp)) {
            return;
        }
        if (CheckWindowIdEventExist(parameter.windowId)) {
            SendEvent(windowFocusEventMap_[parameter.windowId]);
            windowFocusEventMap_.erase(parameter.windowId);
        }
        if (operation && operation->AsObject()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient =
                new(std::nothrow) InteractionOperationDeathRecipient(parameter.windowId, treeIdSingle,
                    currentAccountId_);
            if (deathRecipient == nullptr) {
                Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                    A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
                HILOG_ERROR("Create interactionOperationDeathRecipient failed");
                return;
            }
            bool result = operation->AsObject()->AddDeathRecipient(deathRecipient);
            interactionOperationDeathMap_[parameter.windowId][treeIdSingle] = deathRecipient;
            HILOG_DEBUG("The result of adding operation's death recipient is %{public}d", result);
        }
        }, "TASK_REGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

void AccessibleAbilityManagerService::DeleteConnectionAndDeathRecipient(
    const int32_t windowId, const sptr<A11yRegisterElementWindowConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return;
    }

    accountData->RemoveA11yRegisterElementWindowConnection(windowId);
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

    handler_->PostTask([=]() {
        HILOG_INFO("Deregister windowId[%{public}d]", windowId);
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<A11yRegisterElementWindowConnection> connection = accountData->GetA11yRegisterElementWindowConnection(windowId);
        if (!connection) {
            HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
            return;
        }
        accountData->RemoveA11yRegisterElementWindowConnection(windowId);
        StopCallbackWait(windowId);

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
            } else {
                HILOG_INFO("cannot find remote object. windowId[%{public}d]", windowId);
            }
        }
        }, "TASK_DEREGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DeregisterElementOperator(int32_t windowId, const int32_t treeId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask([=]() {
        HILOG_INFO("Deregister windowId[%{public}d], treeId[%{public}d] start", windowId, treeId);
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<A11yRegisterElementWindowConnection> connection = accountData->GetA11yRegisterElementWindowConnection(windowId);
        if (connection == nullptr) {
            HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
            return;
        }
        StopCallbackWait(windowId, treeId);

        if (!connection->GetCardProxy(treeId)) {
            HILOG_ERROR("proxy is null");
            return;
        }

        auto object = connection->GetCardProxy(treeId)->AsObject();
        if (object) {
            RemoveTreeDeathRecipient(windowId, treeId, connection);
        }
        }, "TASK_DEREGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

void AccessibleAbilityManagerService::RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
    const sptr<A11yRegisterElementWindowConnection> connection)
{
    auto object = connection->GetCardProxy(treeId)->AsObject();
    connection->EraseProxy(treeId);
    auto iter = interactionOperationDeathMap_.find(windowId);
    if (iter != interactionOperationDeathMap_.end()) {
        auto iterTree = iter->second.find(treeId);
        if (iterTree != iter->second.end()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient = iterTree->second;
            bool result = object->RemoveDeathRecipient(deathRecipient);
            HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
            iter->second.erase(iterTree);
        } else {
            HILOG_ERROR("cannot find remote object. treeId[%{public}d]", treeId);
        }
    } else {
        HILOG_ERROR("cannot find remote object. windowId[%{public}d]", windowId);
    }
}

RetError AccessibleAbilityManagerService::GetCaptionProperty(A11yRegisterElementConfig::CaptionProperty &caption)
{
    return a11yRegisterElementSettings_->GetCaptionProperty(caption);
}

RetError AccessibleAbilityManagerService::SetCaptionProperty(const A11yRegisterElementConfig::CaptionProperty &caption)
{
    return a11yRegisterElementSettings_->SetCaptionProperty(caption);
}

RetError AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    return a11yRegisterElementSettings_->SetCaptionState(state);
}

bool AccessibleAbilityManagerService::GetEnabledState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    ffrt::promise<bool> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetEnabledState();
        syncPromise.set_value(result);
        }, "TASK_GET_ENABLE_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::GetCaptionState(bool &state)
{
    return a11yRegisterElementSettings_->GetCaptionState(state);
}

bool AccessibleAbilityManagerService::GetTouchGuideState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    ffrt::promise<bool> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetTouchGuideState();
        syncPromise.set_value(result);
        }, "TASK_GET_TOUCH_GUIDE_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetGestureState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    ffrt::promise<bool> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetGestureState();
        syncPromise.set_value(result);
        }, "TASK_GET_GESTURE_STATE");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::GetKeyEventObserverState()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return false;
    }

    ffrt::promise<bool> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(false);
            return;
        }
        bool result = accountData->GetConfig()->GetKeyEventObserverState();
        syncPromise.set_value(result);
        }, "TASK_GET_KEY_EVENT_OBSERVER_STATE");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &name, &capabilities]() {
        HILOG_DEBUG();
        RetError result = InnerEnableAbility(name, capabilities);
        syncPromise.set_value(result);
        }, "TASK_ENABLE_ABILITIES");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::SetTargetAbility(const int32_t targetAbilityValue)
{
    HILOG_DEBUG();

    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
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
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    auto iter = removedAutoStartAbilities_.begin();
    for (; iter != removedAutoStartAbilities_.end(); ++iter) {
        if (*iter == name) {
            removedAutoStartAbilities_.erase(iter);
            break;
        }
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

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &enabledAbilities]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        enabledAbilities = accountData->GetEnabledAbilities();
        syncPromise.set_value(RET_OK);
        }, "TASK_GET_ENABLE_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::SetCurtainScreenUsingStatus(bool isEnable)
{
    HILOG_DEBUG();
    auto rsInterfaces = &(Rosen::RSInterfaces::GetInstance());
    if (rsInterfaces == nullptr) {
        HILOG_ERROR("rsInterfaces is nullptr.");
        return RET_ERR_NULLPTR;
    }
    HILOG_INFO("SetCurtainScreenUsingStatus: status = %{public}d", isEnable);
    rsInterfaces->SetCurtainScreenUsingStatus(isEnable);
    return RET_OK;
}

RetError AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    HILOG_INFO();
    if (!actionHandler_) {
        HILOG_ERROR("actionHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    actionHandler_->PostTask([this, &syncPromise, &name]() {
        HILOG_DEBUG();
        RetError result = InnerDisableAbility(name);
        syncPromise.set_value(result);
        }, "TASK_DISABLE_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::InnerDisableAbility(const std::string &name)
{
    HILOG_INFO();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "InnerDisableAbility:" + name);
#endif // OHOS_BUILD_ENABLE_HITRACE

    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    if (accountData->GetConnectingA11yAbility(name) != nullptr) {
        HILOG_WARN("refuse to disconnect ability %{public}s when connecting", name.c_str());
        return RET_OK;
    }
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME) {
        ExecuteActionOnA11yRegisterElementFocused(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
        SetCurtainScreenUsingStatus(false);
    }
    RetError ret = accountData->RemoveEnabledAbility(name);
    if (ret != RET_OK) {
        HILOG_ERROR("RemoveEnabledAbility failed");
        return ret;
    }
    accountData->SetAbilityAutoStartState(name, false);
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

    if (!obj) {
        HILOG_ERROR("obj is nullptr.");
        return RET_ERR_NULLPTR;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, obj]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
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

        std::function<void()> addUITestClientFunc = std::bind(&A11yRegisterElementAccountData::AddUITestClient, accountData,
            obj, UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        handler_->PostTask(addUITestClientFunc, "AddUITestClient");
        syncPromise.set_value(RET_OK);
        }, "TASK_ENABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

RetError AccessibleAbilityManagerService::DisableUITestAbility()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    handler_->PostTask([this, syncPromise]() {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise->set_value(RET_ERR_NULLPTR);
            return;
        }
        std::string uiTestUri = Utils::GetUri(UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        sptr<AccessibleAbilityConnection> connection = accountData->GetAccessibleAbilityConnection(uiTestUri);
        if (!connection) {
            HILOG_ERROR("connection is not existed!!");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }
        std::function<void()> removeUITestClientFunc =
            std::bind(&A11yRegisterElementAccountData::RemoveUITestClient, accountData, connection, UI_TEST_BUNDLE_NAME);
        handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
        syncPromise->set_value(RET_OK);
        }, "TASK_DISABLE_UI_TEST_ABILITIES");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait DisableUITestAbility result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

int32_t AccessibleAbilityManagerService::GetActiveWindow()
{
    HILOG_DEBUG();
    return Singleton<A11yRegisterElementWindowManager>::GetInstance().GetActiveWindowId();
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG();
    Singleton<A11yRegisterElementCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<A11yRegisterElementDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<A11yRegisterElementWindowManager>::GetInstance().RegisterWindowListener(handler_);
    bool result = Singleton<A11yRegisterElementWindowManager>::GetInstance().Init();
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

void AccessibleAbilityManagerService::InitInnerResource()
{
    UpdateSettingsInAtoHosTask();
}

void AccessibleAbilityManagerService::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY);
    HILOG_INFO();
    sptr<A11yRegisterElementAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("get accountData failed");
        return;
    }
    int32_t currentAccountId = accountData->GetAccountId();
    if (currentAccountId != accountId_) {
        HILOG_ERROR("check accountId failed");
        return;
    }

    if (treeId_ > 0) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(windowId_, treeId_);
    } else {
        Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(windowId_);
    }
}

sptr<A11yRegisterElementAccountData> AccessibleAbilityManagerService::GetCurrentAccountData()
{
    HILOG_DEBUG();
    if (currentAccountId_ == -1) {
        HILOG_ERROR("current account id is wrong");
        return nullptr;
    }

    return a11yAccountsData_.GetCurrentAccountData(currentAccountId_);
}

sptr<A11yRegisterElementAccountData> AccessibleAbilityManagerService::GetAccountData(int32_t accountId)
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAccountData(accountId);
}

std::vector<int32_t> AccessibleAbilityManagerService::GetAllAccountIds()
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAllAccountIds();
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

sptr<A11yRegisterElementWindowConnection> AccessibleAbilityManagerService::GetA11yRegisterElementWindowConnection(
    int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetA11yRegisterElementWindowConnection(windowId);
}

void AccessibleAbilityManagerService::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<A11yRegisterElementWindowConnection> connection = GetA11yRegisterElementWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void AccessibleAbilityManagerService::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<A11yRegisterElementWindowConnection> connection = GetA11yRegisterElementWindowConnection(windowId);
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
    a11yAccountsData_.AddAccountData(accountId);
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
    OffZoomGesture();

    std::map<std::string, uint32_t> importantEnabledAbilities;
    SCREENREADER_STATE screenReaderState = SCREENREADER_STATE::UNINIT;
    if (currentAccountId_ != -1) {
        HILOG_DEBUG();
        sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        defaultConfigCallbacks_ = accountData->GetConfigCallbacks();
        screenReaderState = accountData->GetDefaultUserScreenReaderState() ?
            SCREENREADER_STATE::ON : SCREENREADER_STATE::OFF;
        accountData->GetImportantEnabledAbilities(importantEnabledAbilities);
        accountData->OnAccountSwitched();
        UpdateA11yRegisterElementManagerService();
    }
    currentAccountId_ = accountId;
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    accountData->Init();
    accountData->SetConfigCallbacks(defaultConfigCallbacks_);
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    float discount = accountData->GetConfig()->GetBrightnessDiscount();
    if (!Singleton<A11yRegisterElementPowerManager>::GetInstance().DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
    }
#endif
    AccountSA::OsAccountType accountType = accountData->GetAccountType();
    if (screenReaderState != SCREENREADER_STATE::UNINIT &&
        (accountType == AccountSA::OsAccountType::PRIVATE || accountType == AccountSA::OsAccountType::ADMIN)) {
        bool state = (screenReaderState == SCREENREADER_STATE::ON) ? true : false;
        accountData->SetAbilityAutoStartState(SCREEN_READER_BUNDLE_ABILITY_NAME, state);
        HILOG_INFO("set screenreader auto-start state = %{public}d", true);
    }

    if (accountData->GetInstalledAbilitiesFromBMS()) {
        accountData->UpdateImportantEnabledAbilities(importantEnabledAbilities);
        accountData->UpdateAbilities();
        UpdateA11yRegisterElementManagerService();
    }
    UpdateAllSetting();
    UpdateAutoStartAbilities();
    RegisterShortKeyEvent();
    RegisterScreenMagnificationState();
    RegisterScreenMagnificationType();
}

void AccessibleAbilityManagerService::PackageRemoved(const std::string &bundleName)
{
    sptr<A11yRegisterElementAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr.");
        return;
    }

    packageAccount->DelAutoStartPrefKeyInRemovePkg(bundleName);
    std::vector<std::string> multiTarget = packageAccount->GetConfig()->GetShortkeyMultiTarget();
    std::string name = packageAccount->GetConfig()->GetShortkeyTarget();
    auto installedAbilities_ = packageAccount->GetInstalledAbilities();
    for (auto &installAbility : installedAbilities_) {
        std::string abilityId = installAbility.GetId();
        HILOG_DEBUG("abilityId%{public}s", abilityId.c_str());
        if (bundleName != installAbility.GetPackageName()) {
            continue;
        }
        if (std::find(removedAutoStartAbilities_.begin(), removedAutoStartAbilities_.end(), abilityId)
            == removedAutoStartAbilities_.end()) {
            removedAutoStartAbilities_.push_back(abilityId);
        }
        // no use later version
        if (abilityId == name) {
            std::string targetName = "";
            packageAccount->GetConfig()->SetShortkeyTarget(targetName);
            UpdateShortkeyTarget();
        }
        // multi
        for (const auto &target : multiTarget) {
            if (target == abilityId) {
                packageAccount->GetConfig()->SetShortkeyMultiTargetInPkgRemove(abilityId);
                UpdateShortkeyMultiTarget();
            }
        }
    }

    if (packageAccount->RemoveAbility(bundleName)) {
        HILOG_DEBUG("ability%{public}s removed!", bundleName.c_str());
        UpdateA11yRegisterElementManagerService();
    }
}

void AccessibleAbilityManagerService::PackageAdd(const std::string &bundleName)
{
    sptr<A11yRegisterElementAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }
    for (auto &abilityId : removedAutoStartAbilities_) {
        if (packageAccount->GetAbilityAutoStartState(abilityId)) {
            packageAccount->SetAbilityAutoStartState(abilityId, false);
        }
    }
    packageAccount->AddAbility(bundleName);
}

void AccessibleAbilityManagerService::PackageChanged(const std::string &bundleName)
{
    sptr<A11yRegisterElementAccountData> packageAccount = GetCurrentAccountData();
    if (!packageAccount) {
        HILOG_ERROR("packageAccount is nullptr");
        return;
    }

    bool isNeedUpdateShortKeyTarget = false;
    std::string target = packageAccount->GetConfig()->GetShortkeyTarget();
    if (target.substr(0, target.find("/")) == bundleName) {
        isNeedUpdateShortKeyTarget = true;
    }
    std::vector<std::string> multiTarget = packageAccount->GetConfig()->GetShortkeyMultiTarget();

    packageAccount->ChangeAbility(bundleName);
    UpdateA11yRegisterElementManagerService();

    std::vector<std::string> sameBundleTarget;
    auto installedAbilities_ = packageAccount->GetInstalledAbilities();
    for (auto &installAbility : installedAbilities_) {
        std::string abilityId = installAbility.GetId();
        if (bundleName != installAbility.GetPackageName()) {
            continue;
        }
        if (abilityId == target) {
            isNeedUpdateShortKeyTarget = false;
        }
        sameBundleTarget.push_back(abilityId);
    }

    if (isNeedUpdateShortKeyTarget) {
        packageAccount->GetConfig()->SetShortkeyTarget("");
        UpdateShortkeyTarget();
    }
    std::vector<std::string> tmpAbilities = multiTarget;
    bool isNeedUpdateShortKeyMultiTarget = false;
    Utils::SelectUsefulFromVecWithSameBundle(tmpAbilities, sameBundleTarget,
        isNeedUpdateShortKeyMultiTarget, bundleName);
    if (isNeedUpdateShortKeyMultiTarget) {
        packageAccount->GetConfig()->SetShortkeyMultiTarget(tmpAbilities);
        UpdateShortkeyMultiTarget();
    }
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const A11yRegisterElementElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetA11yRegisterElementId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        a11yRegisterElementInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<A11yRegisterElementElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetA11yRegisterElementId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_DEBUG("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByA11yRegisterElementIdResult(
    const std::vector<A11yRegisterElementElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetA11yRegisterElementId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_DEBUG("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const A11yRegisterElementElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetA11yRegisterElementId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        a11yRegisterElementInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetCursorPositionResult(const int32_t cursorPosition,
    const int32_t requestId)
{
    HILOG_INFO("ElementOperatorCallbackImpl::SetCursorPositionResult [result:%{public}d]",
        cursorPosition);
    HILOG_DEBUG("cursorPosition [result:%{public}d, requestId:%{public}d]", cursorPosition, requestId);
    callCursorPosition_ = cursorPosition;
    promise_.set_value();
}

bool AccessibleAbilityManagerService::GetParentElementRecursively(int32_t windowId, int64_t elementId,
    std::vector<A11yRegisterElementElementInfo>& infos)
{
    sptr<A11yRegisterElementAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }

    int32_t treeId = 0;
    sptr<IA11yRegisterElementElementOperator> elementOperator = nullptr;
    sptr<A11yRegisterElementWindowConnection> connection = accountData->GetA11yRegisterElementWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("GetA11yRegisterElementWindowConnection failed");
        return false;
    }

    if (elementId > 0) {
        treeId = GetTreeIdBySplitElementId(elementId);
        elementOperator = connection->GetCardProxy(treeId);
    } else {
        elementOperator = connection->GetProxy();
    }
    if (elementOperator == nullptr) {
        HILOG_DEBUG("elementOperator failed elementId: %{public}" PRId64 " winId: %{public}d treeId: %{public}d",
            elementId, windowId, treeId);
        return false;
    }
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl();
    if (callBack == nullptr) {
        HILOG_ERROR("Failed to create callBack.");
        return false;
    }

    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    elementOperator->SearchElementInfoByA11yRegisterElementId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        return false;
    }

    for (auto& info : callBack->elementInfosResult_) {
        if (info.GetA11yRegisterElementId() == A11yRegisterElementElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfoByA11yRegisterElementId elementInfo from ace is wrong");
            return false;
        }
    }

    infos = callBack->elementInfosResult_;
    HILOG_DEBUG("Get parent element success, size %{public}zu", infos.size());
    return true;
}

void AccessibleAbilityManagerService::FindInnerWindowId(const A11yRegisterElementEventInfo &event, int32_t& windowId)
{
    HILOG_DEBUG();
    auto mapTable = Singleton<A11yRegisterElementWindowManager>::GetInstance().sceneBoardElementIdMap_.GetAllPairs();
    int64_t elementId = event.GetA11yRegisterElementId();
    while (1) {
        for (auto iter = mapTable.begin(); iter != mapTable.end(); iter++) {
            if (elementId == iter->second) {
                windowId = iter->first;
                HILOG_DEBUG("inner windowId %{public}d", windowId);
                return;
            }
        }

        std::vector<A11yRegisterElementElementInfo> infos = {};
        if (GetParentElementRecursively(event.GetWindowId(), elementId, infos) == false || infos.size() == 0) {
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

void AccessibleAbilityManagerService::UpdateA11yRegisterElementWindowStateByEvent(const A11yRegisterElementEventInfo &event)
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
            Singleton<A11yRegisterElementWindowManager>::GetInstance().SetActiveWindow(windowId, false);
            Singleton<A11yRegisterElementWindowManager>::GetInstance().SetA11yRegisterElementFocusedWindow(windowId);
            break;
        default:
            break;
    }
}
} // namespace A11yRegisterElement
} // namespace OHOS