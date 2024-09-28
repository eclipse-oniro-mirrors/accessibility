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
#include "a11y_event_info.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "a11y_power_manager.h"
#endif
#include "a11y_short_key_dialog.h"
#include "a11y_window_manager.h"
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
namespace A11y {
namespace {
    const std::string AAMS_SERVICE_NAME = "SaPerformActionXTS";
    const std::string AAMS_ACTION_RUNNER_NAME = "AamsActionRunner";
    const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "a11y.config.ready";
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/A11yExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "a11y_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "a11y_magnification_capability";
    const std::string DELAY_UNLOAD_TASK = "TASK_UNLOAD_ACCESSIBILITY_SA";
    const std::string USER_SETUP_COMPLETED = "user_setup_complete";
    const std::string ACCESSIBILITY_CLONE_FLAG = "a11y_config_clone";
    const std::string SHORTCUT_ENABLED = "a11y_shortcut_enabled";
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
    const std::string TIMER_REGISTER_STATE_OBSERVER = "a11y:registerStateObServer";
    const std::string TIMER_REGISTER_CAPTION_OBSERVER = "a11y:registerCaptionObServer";
    const std::string TIMER_REGISTER_ENABLEABILITY_OBSERVER = "a11y:registerEnableAbilityObServer";
    const std::string TIMER_GET_ALL_CONFIG = "a11y:getAllConfig";
    const std::string TIMER_REGISTER_CONFIG_OBSERVER = "a11y:registerConfigObserver";
    constexpr int32_t XCOLLIE_TIMEOUT = 1; // s
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<SaPerformActionXTS>::GetInstance());

SaPerformActionXTS::SaPerformActionXTS()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
    HILOG_INFO("SaPerformActionXTS is constructed");
    dependentServicesStatus_[ABILITY_MGR_SERVICE_ID] = false;
    dependentServicesStatus_[BUNDLE_MGR_SERVICE_SYS_ABILITY_ID] = false;
    dependentServicesStatus_[COMMON_EVENT_SERVICE_ID] = false;
    dependentServicesStatus_[DISPLAY_MANAGER_SERVICE_SA_ID] = false;
    dependentServicesStatus_[SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN] = false;
    dependentServicesStatus_[WINDOW_MANAGER_SERVICE_ID] = false;
    dependentServicesStatus_[DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID] = false;

    a11ySettings_ = std::make_shared<A11ySettings>();
    a11yShortKey_ = std::make_shared<A11yShortKey>();
}

SaPerformActionXTS::~SaPerformActionXTS()
{
    HILOG_INFO("SaPerformActionXTS::~SaPerformActionXTS");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    a11yAccountsData_.Clear();
}

void SaPerformActionXTS::OnStart()
{
    HILOG_INFO("SaPerformActionXTS::OnStart start");
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!runner_) {
            HILOG_ERROR("SaPerformActionXTS::OnStart failed:create AAMS runner failed");
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AAMSEventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("SaPerformActionXTS::OnStart failed:create AAMS event handler failed");
            return;
        }
    }

    if (!actionRunner_) {
        actionRunner_ = AppExecFwk::EventRunner::Create(AAMS_ACTION_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!actionRunner_) {
            HILOG_ERROR("SaPerformActionXTS::OnStart failed:create AAMS action runner failed");
            return;
        }
    }

    if (!actionHandler_) {
        actionHandler_ = std::make_shared<AAMSEventHandler>(actionRunner_);
        if (!actionHandler_) {
            HILOG_ERROR("SaPerformActionXTS::OnStart failed:create AAMS action handler failed");
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

    a11ySettings_->RegisterSettingsHandler(handler_);
}

void SaPerformActionXTS::OnStop()
{
    HILOG_INFO("stop SaPerformActionXTS");
    if (!handler_) {
        HILOG_ERROR("SaPerformActionXTS::OnStop failed!");
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
        HILOG_DEBUG();

        Singleton<A11yCommonEvent>::GetInstance().UnSubscriberEvent();
        Singleton<A11yDisplayManager>::GetInstance().UnregisterDisplayListener();
        Singleton<A11yWindowManager>::GetInstance().DeregisterWindowListener();

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
    HILOG_INFO("SaPerformActionXTS::OnStop OK.");
}

void SaPerformActionXTS::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
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
            HILOG_ERROR("SaPerformActionXTS::Init failed!");
            return;
        }

        if (!isPublished_) {
            if (Publish(this) == false) {
                HILOG_ERROR("SaPerformActionXTS::Publish failed!");
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

void SaPerformActionXTS::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
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
            Singleton<A11yCommonEvent>::GetInstance().UnSubscriberEvent();
            Singleton<A11yDisplayManager>::GetInstance().UnregisterDisplayListener();
            Singleton<A11yWindowManager>::GetInstance().DeregisterWindowListener();
            Singleton<A11yWindowManager>::GetInstance().DeInit();

            isReady_ = false;
            SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
        }
        }, "OnRemoveSystemAbility");
}

int SaPerformActionXTS::Dump(int fd, const std::vector<std::u16string>& args)
{
    HILOG_DEBUG("dump A11yManagerServiceInfo");
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    ffrt::promise<int> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, fd, args]() {
        if (!a11yDumper_) {
            a11yDumper_ = new(std::nothrow) A11yDumper();
            if (!a11yDumper_) {
                HILOG_ERROR("a11yDumper_ is nullptr");
                syncPromise.set_value(-1);
                return;
            }
        }
        syncPromise.set_value(a11yDumper_->Dump(fd, args));
        }, "TASK_DUMP_INFO");
    return syncFuture.get();
}

RetError SaPerformActionXTS::VerifyingToKenId(const int32_t windowId, const int64_t elementId)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t treeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
    HILOG_DEBUG("VerifyingToKenId: treeId[%{public}d], windowId[%{public}d], elementId[%{public}" PRId64 "]",
        treeId, windowId, elementId);
    if (elementId == ELEMENT_ID_INVALID || windowId == WINDOW_ID_INVALID) {
        HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
        return RET_OK;
    }
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return RET_ERR_CONNECTION_EXIST;
    }
    HILOG_DEBUG("treeId %{public}d, windowId %{public}d", treeId, windowId);
    int32_t realId =
        Singleton<A11yWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    sptr<A11yWindowConnection> connection = accountData->GetA11yWindowConnection(realId);
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

RetError SaPerformActionXTS::SendEvent(const A11yEventInfo &uiEvent, const int32_t flag)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d] windowId[%{public}d] compnentId: %{public}" PRId64 " "
        "elementId: %{public}" PRId64 " winId: %{public}d treeId: %{public}d",
        uiEvent.GetEventType(), uiEvent.GetGestureType(), uiEvent.GetWindowId(), uiEvent.GetA11yId(),
        uiEvent.GetElementInfo().GetA11yId(),
        uiEvent.GetElementInfo().GetWindowId(), uiEvent.GetElementInfo().GetBelongTreeId());
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    if (flag) {
        if (VerifyingToKenId(uiEvent.GetElementInfo().GetWindowId(),
            uiEvent.GetElementInfo().GetA11yId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_DEBUG("VerifyingToKenId failed");
            return RET_ERR_CONNECTION_EXIST;
        }
    }

    UpdateA11yWindowStateByEvent(uiEvent);
    handler_->PostTask([this, uiEvent]() {
        HILOG_DEBUG();
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }

        const_cast<A11yEventInfo&>(uiEvent).SetTimeStamp(Utils::GetSystemTime());
        map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
        for (auto &ability : abilities) {
            if (ability.second) {
                ability.second->OnA11yEvent(const_cast<A11yEventInfo&>(uiEvent));
            }
        }
        }, "TASK_SEND_EVENT");
    return RET_OK;
}

uint32_t SaPerformActionXTS::RegisterStateObserver(
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
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        return 0;
    }

    return accountData->GetA11yState();
}

void SaPerformActionXTS::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<A11yWindowManager>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
        syncPromise.set_value();
        }, "GET_REAL_WINDOW_AND_ELEMENT_ID");
    return syncFuture.get();
}

void SaPerformActionXTS::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<A11yWindowManager>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        syncPromise.set_value();
        }, "GET_SCENE_BOARD_INNER_WINDOW_ID");
    return syncFuture.get();
}

sptr<A11yWindowConnection> SaPerformActionXTS::GetRealIdConnection()
{
    HILOG_DEBUG();
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    int32_t realId = Singleton<A11yWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);

    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return sptr<A11yWindowConnection>();
    }
    return accountData->GetA11yWindowConnection(realId);
}

bool SaPerformActionXTS::FindFocusedElementByConnection(sptr<A11yWindowConnection> connection,
    A11yElementInfo &elementInfo)
{
    HILOG_DEBUG();
    int64_t elementId = -1;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("GetA11yWindowConnection failed");
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
    elementInfo = focusCallback->a11yInfoResult_;
    return true;
}

bool SaPerformActionXTS::FindFocusedElement(A11yElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<A11yWindowConnection> connection = GetRealIdConnection();
    FindFocusedElementByConnection(connection, elementInfo);
    if (elementInfo.GetA11yId() >= 0) {
        HILOG_DEBUG("find focused element success.");
        return true;
    }
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    sptr<IA11yElementOperator> elementOperator = nullptr;
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    connection = accountData->GetA11yWindowConnection(windowId);
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
    elementOperator->SearchElementInfoByA11yId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        return false;
    }

    if (callBack->elementInfosResult_.size() <= 0) {
        HILOG_ERROR("SearchElementInfoByA11yId return null");
        return false;
    }
    elementInfo = callBack->elementInfosResult_[0];
    return true;
}

void SaPerformActionXTS::GetElementOperatorConnection(sptr<A11yWindowConnection> &connection,
    const int64_t elementId, sptr<IA11yElementOperator> &elementOperator)
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

bool SaPerformActionXTS::ExecuteActionOnA11yFocused(const ActionType &action)
{
    HILOG_DEBUG();
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    uint32_t timeOut = 5000;
    int32_t treeId = GetTreeIdBySplitElementId(elementId);
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    sptr<A11yWindowConnection> connection = accountData->GetA11yWindowConnection(windowId);
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

void SaPerformActionXTS::SetFocusWindowId(const int32_t focusWindowId)
{
    focusWindowId_ = focusWindowId;
}

int32_t SaPerformActionXTS::GetFocusWindowId()
{
    return focusWindowId_;
}

void SaPerformActionXTS::SetFocusElementId(const int64_t focusElementId)
{
    focusElementId_ = focusElementId;
}

int64_t SaPerformActionXTS::GetFocusElementId()
{
    return focusElementId_;
}

uint32_t SaPerformActionXTS::RegisterCaptionObserver(
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

void SaPerformActionXTS::RegisterEnableAbilityListsObserver(
    const sptr<IA11yEnableAbilityListsObserver> &observer)
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<A11yAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d", stateType);
        return RET_ERR_INVALID_PARAM;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &infos, abilityTypes, stateType]() {
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Get current account data failed!!");
            syncPromise.set_value(RET_ERR_FAILED);
            return;
        }

        vector<A11yAbilityInfo> abilities;
        accountData->GetAbilitiesByState(static_cast<AbilityStateType>(stateType), abilities);
        HILOG_DEBUG("abilityes count is %{public}zu", abilities.size());
        for (auto &ability : abilities) {
            if (abilityTypes == A11yAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL ||
                (ability.GetA11yAbilityType() & abilityTypes)) {
                infos.push_back(ability);
            }
        }
        HILOG_DEBUG("infos count is %{public}zu", infos.size());
        syncPromise.set_value(RET_OK);
        }, "TASK_GET_ABILITY_LIST");
    return syncFuture.get();
}

RetError SaPerformActionXTS::RegisterElementOperator(
    const int32_t windowId, const sptr<IA11yElementOperator> &operation, bool isApp)
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return;
        }
        sptr<A11yWindowConnection> oldConnection = accountData->GetA11yWindowConnection(windowId);
        if (isApp && oldConnection) {
            HILOG_WARN("no need to register again.");
            return;
        }
        DeleteConnectionAndDeathRecipient(windowId, oldConnection);
        sptr<A11yWindowConnection> connection =
            new(std::nothrow) A11yWindowConnection(windowId, operation, currentAccountId_);
        if (!connection) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("New  A11yWindowConnection failed!!");
            return;
        }
        connection->SetTokenIdMap(SINGLE_TREE_ID, tokenId);
        accountData->AddA11yWindowConnection(windowId, connection);

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

void SaPerformActionXTS::IsCheckWindowIdEventExist(const int32_t windowId)
{
    if (CheckWindowIdEventExist(windowId)) {
        SendEvent(windowFocusEventMap_[windowId]);
        windowFocusEventMap_.erase(windowId);
    }
}

RetError SaPerformActionXTS::RegisterElementOperatorChildWork(const Registration &parameter,
    const int32_t treeId, const int64_t nodeId, const sptr<IA11yElementOperator> &operation,
    const uint32_t tokenId, bool isApp)
{
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return RET_ERR_REGISTER_EXIST;
    }

    sptr<A11yWindowConnection> parentConnection =
        accountData->GetA11yWindowConnection(parameter.parentWindowId);
    if (isApp && parentConnection) {
        sptr<IA11yElementOperator> parentAamsOper =
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
    sptr<A11yWindowConnection> oldConnection =
        accountData->GetA11yWindowConnection(parameter.windowId);
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

void SaPerformActionXTS::SetTokenIdMapAndRootParentId(
    const sptr<A11yWindowConnection> connection,
    const int32_t treeId, const int64_t nodeId, const uint32_t tokenId)
{
    connection->SetTokenIdMap(treeId, tokenId);
    connection->SetRootParentId(treeId, nodeId);
}

RetError SaPerformActionXTS::RegisterElementOperator(Registration parameter,
    const sptr<IA11yElementOperator> &operation, bool isApp)
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

void SaPerformActionXTS::DeleteConnectionAndDeathRecipient(
    const int32_t windowId, const sptr<A11yWindowConnection> &connection)
{
    HILOG_DEBUG();
    if (!connection) {
        HILOG_ERROR("connection is nullptr");
        return;
    }

    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return;
    }

    accountData->RemoveA11yWindowConnection(windowId);
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

RetError SaPerformActionXTS::DeregisterElementOperator(int32_t windowId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask([=]() {
        HILOG_INFO("Deregister windowId[%{public}d]", windowId);
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<A11yWindowConnection> connection = accountData->GetA11yWindowConnection(windowId);
        if (!connection) {
            HILOG_WARN("The operation of windowId[%{public}d] has not been registered.", windowId);
            return;
        }
        accountData->RemoveA11yWindowConnection(windowId);
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

RetError SaPerformActionXTS::DeregisterElementOperator(int32_t windowId, const int32_t treeId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask([=]() {
        HILOG_INFO("Deregister windowId[%{public}d], treeId[%{public}d] start", windowId, treeId);
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<A11yWindowConnection> connection = accountData->GetA11yWindowConnection(windowId);
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

void SaPerformActionXTS::RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
    const sptr<A11yWindowConnection> connection)
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

RetError SaPerformActionXTS::GetCaptionProperty(A11yConfig::CaptionProperty &caption)
{
    return a11ySettings_->GetCaptionProperty(caption);
}

RetError SaPerformActionXTS::SetCaptionProperty(const A11yConfig::CaptionProperty &caption)
{
    return a11ySettings_->SetCaptionProperty(caption);
}

RetError SaPerformActionXTS::SetCaptionState(const bool state)
{
    return a11ySettings_->SetCaptionState(state);
}

bool SaPerformActionXTS::GetEnabledState()
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::GetCaptionState(bool &state)
{
    return a11ySettings_->GetCaptionState(state);
}

bool SaPerformActionXTS::GetTouchGuideState()
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

bool SaPerformActionXTS::GetGestureState()
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

bool SaPerformActionXTS::GetKeyEventObserverState()
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::EnableAbility(const std::string &name, const uint32_t capabilities)
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

bool SaPerformActionXTS::SetTargetAbility(const int32_t targetAbilityValue)
{
    HILOG_DEBUG();

    sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::InnerEnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

RetError SaPerformActionXTS::SetCurtainScreenUsingStatus(bool isEnable)
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

RetError SaPerformActionXTS::DisableAbility(const std::string &name)
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

RetError SaPerformActionXTS::InnerDisableAbility(const std::string &name)
{
    HILOG_INFO();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "InnerDisableAbility:" + name);
#endif // OHOS_BUILD_ENABLE_HITRACE

    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    if (accountData->GetConnectingA11yAbility(name) != nullptr) {
        HILOG_WARN("refuse to disconnect ability %{public}s when connecting", name.c_str());
        return RET_OK;
    }
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME) {
        ExecuteActionOnA11yFocused(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
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

RetError SaPerformActionXTS::EnableUITestAbility(const sptr<IRemoteObject> &obj)
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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

        std::function<void()> addUITestClientFunc = std::bind(&A11yAccountData::AddUITestClient, accountData,
            obj, UI_TEST_BUNDLE_NAME, UI_TEST_ABILITY_NAME);
        handler_->PostTask(addUITestClientFunc, "AddUITestClient");
        syncPromise.set_value(RET_OK);
        }, "TASK_ENABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

RetError SaPerformActionXTS::DisableUITestAbility()
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
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
            std::bind(&A11yAccountData::RemoveUITestClient, accountData, connection, UI_TEST_BUNDLE_NAME);
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

int32_t SaPerformActionXTS::GetActiveWindow()
{
    HILOG_DEBUG();
    return Singleton<A11yWindowManager>::GetInstance().GetActiveWindowId();
}

bool SaPerformActionXTS::Init()
{
    HILOG_DEBUG();
    Singleton<A11yCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<A11yDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<A11yWindowManager>::GetInstance().RegisterWindowListener(handler_);
    bool result = Singleton<A11yWindowManager>::GetInstance().Init();
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

void SaPerformActionXTS::InitInnerResource()
{
    UpdateSettingsInAtoHosTask();
}

void SaPerformActionXTS::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY);
    HILOG_INFO();
    sptr<A11yAccountData> accountData =
        Singleton<SaPerformActionXTS>::GetInstance().GetCurrentAccountData();
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
        Singleton<SaPerformActionXTS>::GetInstance().DeregisterElementOperator(windowId_, treeId_);
    } else {
        Singleton<SaPerformActionXTS>::GetInstance().DeregisterElementOperator(windowId_);
    }
}

sptr<A11yAccountData> SaPerformActionXTS::GetCurrentAccountData()
{
    HILOG_DEBUG();
    if (currentAccountId_ == -1) {
        HILOG_ERROR("current account id is wrong");
        return nullptr;
    }

    return a11yAccountsData_.GetCurrentAccountData(currentAccountId_);
}

sptr<A11yAccountData> SaPerformActionXTS::GetAccountData(int32_t accountId)
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAccountData(accountId);
}

std::vector<int32_t> SaPerformActionXTS::GetAllAccountIds()
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAllAccountIds();
}

sptr<AppExecFwk::IBundleMgr> SaPerformActionXTS::GetBundleMgrProxy()
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

sptr<A11yWindowConnection> SaPerformActionXTS::GetA11yWindowConnection(
    int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get account data failed");
        return nullptr;
    }
    return accountData->GetA11yWindowConnection(windowId);
}

void SaPerformActionXTS::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<A11yWindowConnection> connection = GetA11yWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void SaPerformActionXTS::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<A11yWindowConnection> connection = GetA11yWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

void SaPerformActionXTS::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void SaPerformActionXTS::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

void SaPerformActionXTS::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    Singleton<SaPerformActionXTS>::GetInstance().RemoveCallback(STATE_CALLBACK, this, remote);
}

void SaPerformActionXTS::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<SaPerformActionXTS>::GetInstance().RemoveCallback(CAPTION_PROPERTY_CALLBACK, this, remote);
}

void SaPerformActionXTS::EnableAbilityListsObserverDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<SaPerformActionXTS>::GetInstance().RemoveCallback(
        ENABLE_ABILITY_LISTS_CALLBACK, this, remote);
}

void SaPerformActionXTS::AddedUser(int32_t accountId)
{
    HILOG_DEBUG();
    a11yAccountsData_.AddAccountData(accountId);
}

void SaPerformActionXTS::RemovedUser(int32_t accountId)
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

void SaPerformActionXTS::SwitchedUser(int32_t accountId)
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
        sptr<A11yAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        defaultConfigCallbacks_ = accountData->GetConfigCallbacks();
        screenReaderState = accountData->GetDefaultUserScreenReaderState() ?
            SCREENREADER_STATE::ON : SCREENREADER_STATE::OFF;
        accountData->GetImportantEnabledAbilities(importantEnabledAbilities);
        accountData->OnAccountSwitched();
        UpdateA11yManagerService();
    }
    currentAccountId_ = accountId;
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    accountData->Init();
    accountData->SetConfigCallbacks(defaultConfigCallbacks_);
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    float discount = accountData->GetConfig()->GetBrightnessDiscount();
    if (!Singleton<A11yPowerManager>::GetInstance().DiscountBrightness(discount)) {
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
        UpdateA11yManagerService();
    }
    UpdateAllSetting();
    UpdateAutoStartAbilities();
    RegisterShortKeyEvent();
    RegisterScreenMagnificationState();
    RegisterScreenMagnificationType();
}

void SaPerformActionXTS::PackageRemoved(const std::string &bundleName)
{
    sptr<A11yAccountData> packageAccount = GetCurrentAccountData();
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
        UpdateA11yManagerService();
    }
}

void SaPerformActionXTS::PackageAdd(const std::string &bundleName)
{
    sptr<A11yAccountData> packageAccount = GetCurrentAccountData();
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

void SaPerformActionXTS::PackageChanged(const std::string &bundleName)
{
    sptr<A11yAccountData> packageAccount = GetCurrentAccountData();
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
    UpdateA11yManagerService();

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

void SaPerformActionXTS::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const A11yElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<SaPerformActionXTS>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetA11yId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        a11yInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void SaPerformActionXTS::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<A11yElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<SaPerformActionXTS>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetA11yId()) == RET_OK) {
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

void SaPerformActionXTS::ElementOperatorCallbackImpl::SetSearchElementInfoByA11yIdResult(
    const std::vector<A11yElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<SaPerformActionXTS>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetA11yId()) == RET_OK) {
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

void SaPerformActionXTS::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const A11yElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<SaPerformActionXTS>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetA11yId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        a11yInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void SaPerformActionXTS::ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

bool SaPerformActionXTS::GetParentElementRecursively(int32_t windowId, int64_t elementId,
    std::vector<A11yElementInfo>& infos)
{
    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }

    int32_t treeId = 0;
    sptr<IA11yElementOperator> elementOperator = nullptr;
    sptr<A11yWindowConnection> connection = accountData->GetA11yWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("GetA11yWindowConnection failed");
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
    elementOperator->SearchElementInfoByA11yId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        return false;
    }

    for (auto& info : callBack->elementInfosResult_) {
        if (info.GetA11yId() == A11yElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfoByA11yId elementInfo from ace is wrong");
            return false;
        }
    }

    infos = callBack->elementInfosResult_;
    HILOG_DEBUG("Get parent element success, size %{public}zu", infos.size());
    return true;
}

void SaPerformActionXTS::FindInnerWindowId(const A11yEventInfo &event, int32_t& windowId)
{
    HILOG_DEBUG();
    auto mapTable = Singleton<A11yWindowManager>::GetInstance().sceneBoardElementIdMap_.GetAllPairs();
    int64_t elementId = event.GetA11yId();
    while (1) {
        for (auto iter = mapTable.begin(); iter != mapTable.end(); iter++) {
            if (elementId == iter->second) {
                windowId = iter->first;
                HILOG_DEBUG("inner windowId %{public}d", windowId);
                return;
            }
        }

        std::vector<A11yElementInfo> infos = {};
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

void SaPerformActionXTS::UpdateA11yWindowStateByEvent(const A11yEventInfo &event)
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
            Singleton<A11yWindowManager>::GetInstance().SetActiveWindow(windowId, false);
            Singleton<A11yWindowManager>::GetInstance().SetA11yFocusedWindow(windowId);
            break;
        default:
            break;
    }
}

void SaPerformActionXTS::UpdateA11yManagerService()
{
    HILOG_DEBUG("start.");

    sptr<A11yAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Account data is null");
        return;
    }

    accountData->UpdateAccountCapabilities();
    UpdateInputFilter();
    UpdateA11yState();
    UpdateShortKeyRegister();
}
} // namespace A11y
} // namespace OHOS