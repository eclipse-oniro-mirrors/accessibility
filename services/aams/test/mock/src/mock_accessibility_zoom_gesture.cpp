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
#include "accessibility_event_info.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "accessibility_power_manager.h"
#endif
#include "accessibility_short_key_dialog.h"
#include "accessibility_window_manager.h"
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
namespace Accessibility {
namespace {
    const std::string AAMS_SERVICE_NAME = "MockAccessibilityZoomGesture";
    const std::string AAMS_ACTION_RUNNER_NAME = "AamsActionRunner";
    const std::string AAMS_SEND_EVENT_RUNNER_NAME = "AamsSendEventRunner";
    const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
    const std::string DELAY_UNLOAD_TASK = "TASK_UNLOAD_ACCESSIBILITY_SA";
    const std::string USER_SETUP_COMPLETED = "user_setup_complete";
    const std::string ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const std::string SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
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
    constexpr int32_t SHORT_KEY_TIMEOUT_BEFORE_USE = 3000; // ms
    constexpr int32_t SHORT_KEY_TIMEOUT_AFTER_USE = 1000; // ms
    constexpr int32_t WINDOW_ID_INVALID = -1;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
    enum SCREENREADER_STATE : int32_t {
        UNINIT = -1,
        OFF = 0,
        ON = 1,
    };
    const std::string TIMER_REGISTER_STATE_OBSERVER = "accessibility:registerStateObServer";
    const std::string TIMER_REGISTER_CAPTION_OBSERVER = "accessibility:registerCaptionObServer";
    const std::string TIMER_REGISTER_ENABLEABILITY_OBSERVER = "accessibility:registerEnableAbilityObServer";
    const std::string TIMER_GET_ALL_CONFIG = "accessibility:getAllConfig";
    const std::string TIMER_REGISTER_CONFIG_OBSERVER = "accessibility:registerConfigObserver";
    constexpr int32_t XCOLLIE_TIMEOUT = 6; // s
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&Singleton<MockAccessibilityZoomGesture>::GetInstance());

MockAccessibilityZoomGesture::MockAccessibilityZoomGesture()
    : SystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, true)
{
    HILOG_INFO("MockAccessibilityZoomGesture is constructed");
    dependentServicesStatus_[ABILITY_MGR_SERVICE_ID] = false;
    dependentServicesStatus_[BUNDLE_MGR_SERVICE_SYS_ABILITY_ID] = false;
    dependentServicesStatus_[COMMON_EVENT_SERVICE_ID] = false;
    dependentServicesStatus_[DISPLAY_MANAGER_SERVICE_SA_ID] = false;
    dependentServicesStatus_[SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN] = false;
    dependentServicesStatus_[WINDOW_MANAGER_SERVICE_ID] = false;
    dependentServicesStatus_[DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID] = false;

    accessibilitySettings_ = std::make_shared<AccessibilitySettings>();
    accessibilityShortKey_ = std::make_shared<AccessibilityShortKey>();
}

MockAccessibilityZoomGesture::~MockAccessibilityZoomGesture()
{
    HILOG_INFO("MockAccessibilityZoomGesture::~MockAccessibilityZoomGesture");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    a11yAccountsData_.Clear();
}

void MockAccessibilityZoomGesture::OnStart()
{
    HILOG_INFO("MockAccessibilityZoomGesture::OnStart start");
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AAMS_SERVICE_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!runner_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS runner failed");
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AAMSEventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS event handler failed");
            return;
        }
    }

    if (!actionRunner_) {
        actionRunner_ = AppExecFwk::EventRunner::Create(AAMS_ACTION_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!actionRunner_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS action runner failed");
            return;
        }
    }

    if (!actionHandler_) {
        actionHandler_ = std::make_shared<AAMSEventHandler>(actionRunner_);
        if (!actionHandler_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS action handler failed");
            return;
        }
    }

    if (!sendEventRunner_) {
        sendEventRunner_ = AppExecFwk::EventRunner::Create(AAMS_SEND_EVENT_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!sendEventRunner_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS sendEvent runner failed");
            return;
        }
    }

    if (!sendEventHandler_) {
        sendEventHandler_ = std::make_shared<AAMSEventHandler>(sendEventRunner_);
        if (!sendEventHandler_) {
            HILOG_ERROR("MockAccessibilityZoomGesture::OnStart failed:create AAMS sendEvent handler failed");
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

    accessibilitySettings_->RegisterSettingsHandler(handler_);
}

void MockAccessibilityZoomGesture::OnStop()
{
    HILOG_INFO("stop MockAccessibilityZoomGesture");
    if (!handler_) {
        HILOG_ERROR("MockAccessibilityZoomGesture::OnStop failed!");
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise]() {
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
        }, "TASK_ONSTOP");
    syncFuture.wait();

    for (auto &iter : dependentServicesStatus_) {
        iter.second = false;
    }

    isReady_ = false;
    isPublished_ = false;
    SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
    HILOG_INFO("MockAccessibilityZoomGesture::OnStop OK.");
}

void MockAccessibilityZoomGesture::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
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
            HILOG_ERROR("MockAccessibilityZoomGesture::Init failed!");
            return;
        }

        if (!isPublished_) {
            if (Publish(this) == false) {
                HILOG_ERROR("MockAccessibilityZoomGesture::Publish failed!");
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

void MockAccessibilityZoomGesture::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
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
            Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
            Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
            Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();
            Singleton<AccessibilityWindowManager>::GetInstance().DeInit();

            isReady_ = false;
            SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
        }
        }, "OnRemoveSystemAbility");
}

int MockAccessibilityZoomGesture::Dump(int fd, const std::vector<std::u16string>& args)
{
    HILOG_DEBUG("dump AccessibilityManagerServiceInfo");
    if (!handler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    ffrt::promise<int> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, fd, args]() {
        if (!accessibilityDumper_) {
            accessibilityDumper_ = new(std::nothrow) AccessibilityDumper();
            if (!accessibilityDumper_) {
                HILOG_ERROR("accessibilityDumper_ is nullptr");
                syncPromise.set_value(-1);
                return;
            }
        }
        syncPromise.set_value(accessibilityDumper_->Dump(fd, args));
        }, "TASK_DUMP_INFO");
    return syncFuture.get();
}

RetError MockAccessibilityZoomGesture::VerifyingToKenId(const int32_t windowId, const int64_t elementId)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t treeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
    HILOG_DEBUG("VerifyingToKenId: treeId[%{public}d], windowId[%{public}d], elementId[%{public}" PRId64 "]",
        treeId, windowId, elementId);
    if (elementId == ELEMENT_ID_INVALID || windowId == WINDOW_ID_INVALID) {
        HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
        return RET_OK;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return RET_ERR_CONNECTION_EXIST;
    }
    HILOG_DEBUG("treeId %{public}d, windowId %{public}d", treeId, windowId);
    int32_t realId =
        Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
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

RetError MockAccessibilityZoomGesture::SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t flag)
{
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d] windowId[%{public}d] compnentId: %{public}" PRId64 " "
        "elementId: %{public}" PRId64 " winId: %{public}d innerWinId: %{public}d treeId: %{public}d",
        uiEvent.GetEventType(), uiEvent.GetGestureType(), uiEvent.GetWindowId(), uiEvent.GetAccessibilityId(),
        uiEvent.GetElementInfo().GetAccessibilityId(), uiEvent.GetElementInfo().GetWindowId(),
        uiEvent.GetElementInfo().GetInnerWindowId(), uiEvent.GetElementInfo().GetBelongTreeId());
    if (!sendEventHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    if (flag) {
        if (VerifyingToKenId(uiEvent.GetElementInfo().GetWindowId(),
            uiEvent.GetElementInfo().GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_DEBUG("VerifyingToKenId failed");
            return RET_ERR_CONNECTION_EXIST;
        }
    }

    sendEventHandler_->PostTask([this, uiEvent]() {
        HILOG_DEBUG();
        UpdateAccessibilityWindowStateByEvent(uiEvent);
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }

        const_cast<AccessibilityEventInfo&>(uiEvent).SetTimeStamp(Utils::GetSystemTime());
        map<string, sptr<AccessibleAbilityConnection>> abilities = accountData->GetConnectedA11yAbilities();
        for (auto &ability : abilities) {
            if (ability.second) {
                ability.second->OnAccessibilityEvent(const_cast<AccessibilityEventInfo&>(uiEvent));
            }
        }
        }, "TASK_SEND_EVENT");
    return RET_OK;
}

uint32_t MockAccessibilityZoomGesture::RegisterStateObserver(
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
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        return 0;
    }

    return accountData->GetAccessibilityState();
}

void MockAccessibilityZoomGesture::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<AccessibilityWindowManager>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
        syncPromise.set_value();
        }, "GET_REAL_WINDOW_AND_ELEMENT_ID");
    return syncFuture.get();
}

void MockAccessibilityZoomGesture::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return;
    }

    ffrt::promise<void> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<AccessibilityWindowManager>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        syncPromise.set_value();
        }, "GET_SCENE_BOARD_INNER_WINDOW_ID");
    return syncFuture.get();
}

sptr<AccessibilityWindowConnection> MockAccessibilityZoomGesture::GetRealIdConnection()
{
    HILOG_DEBUG();
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return sptr<AccessibilityWindowConnection>();
    }
    return accountData->GetAccessibilityWindowConnection(realId);
}

bool MockAccessibilityZoomGesture::FindFocusedElementByConnection(sptr<AccessibilityWindowConnection> connection,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    int64_t elementId = -1;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
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
    elementInfo = focusCallback->accessibilityInfoResult_;
    return true;
}

bool MockAccessibilityZoomGesture::FindFocusedElement(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetRealIdConnection();
    FindFocusedElementByConnection(connection, elementInfo);
    if (elementInfo.GetAccessibilityId() >= 0) {
        HILOG_DEBUG("find focused element success.");
        return true;
    }
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    int32_t realId =
        Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    connection = accountData->GetAccessibilityWindowConnection(realId);
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
    elementOperator->SearchElementInfoByAccessibilityId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
        return false;
    }

    if (callBack->elementInfosResult_.size() <= 0) {
        HILOG_ERROR("SearchElementInfoByAccessibilityId return null");
        return false;
    }
    elementInfo = callBack->elementInfosResult_[0];
    return true;
}

void MockAccessibilityZoomGesture::GetElementOperatorConnection(sptr<AccessibilityWindowConnection> &connection,
    const int64_t elementId, sptr<IAccessibilityElementOperator> &elementOperator)
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

bool MockAccessibilityZoomGesture::ExecuteActionOnAccessibilityFocused(const ActionType &action)
{
    int32_t windowId = GetFocusWindowId();
    int64_t elementId = GetFocusElementId();
    uint32_t timeOut = 5000;
    int32_t treeId = GetTreeIdBySplitElementId(elementId);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }
    int32_t realId =
        Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
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
    HILOG_INFO("windowId[%{public}d], elementId[%{public}" PRId64 "], action[%{public}d, result: %{public}d",
        windowId, elementId, action, actionCallback->executeActionResult_);
    return actionCallback->executeActionResult_;
}

void MockAccessibilityZoomGesture::SetFocusWindowId(const int32_t focusWindowId)
{
    focusWindowId_ = focusWindowId;
}

int32_t MockAccessibilityZoomGesture::GetFocusWindowId()
{
    return focusWindowId_;
}

void MockAccessibilityZoomGesture::SetFocusElementId(const int64_t focusElementId)
{
    focusElementId_ = focusElementId;
}

int64_t MockAccessibilityZoomGesture::GetFocusElementId()
{
    return focusElementId_;
}

uint32_t MockAccessibilityZoomGesture::RegisterCaptionObserver(
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
        }, "TASK_REGISTER_CAPTION_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterCaptionObserver result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void MockAccessibilityZoomGesture::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
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
        }, "TASK_REGISTER_ENABLE_ABILITY_LISTS_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterEnableAbilityListsObserver result");
        return;
    }
    return syncFuture.get();
}

RetError MockAccessibilityZoomGesture::GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
    std::vector<AccessibilityAbilityInfo> &infos)
{
    HILOG_DEBUG("abilityTypes(%{public}d) stateType(%{public}d)", abilityTypes, stateType);
    if (!handler_ || (stateType > ABILITY_STATE_INSTALLED) || (stateType < ABILITY_STATE_ENABLE)) {
        HILOG_ERROR("Parameters check failed! stateType:%{public}d", stateType);
        return RET_ERR_INVALID_PARAM;
    }

    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &infos, abilityTypes, stateType]() {
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
        }, "TASK_GET_ABILITY_LIST");
    return syncFuture.get();
}

RetError MockAccessibilityZoomGesture::RegisterElementOperator(
    const int32_t windowId, const sptr<IAccessibilityElementOperator> &operation, bool isApp)
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("Get current account data failed!!");
            return;
        }
        sptr<AccessibilityWindowConnection> oldConnection = accountData->GetAccessibilityWindowConnection(windowId);
        if (isApp && oldConnection) {
            HILOG_WARN("no need to register again.");
            return;
        }
        DeleteConnectionAndDeathRecipient(windowId, oldConnection);
        sptr<AccessibilityWindowConnection> connection =
            new(std::nothrow) AccessibilityWindowConnection(windowId, operation, currentAccountId_);
        if (!connection) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
            return;
        }
        connection->SetTokenIdMap(SINGLE_TREE_ID, tokenId);
        accountData->AddAccessibilityWindowConnection(windowId, connection);

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

void MockAccessibilityZoomGesture::IsCheckWindowIdEventExist(const int32_t windowId)
{
    if (CheckWindowIdEventExist(windowId)) {
        SendEvent(windowFocusEventMap_[windowId]);
        windowFocusEventMap_.erase(windowId);
    }
}

RetError MockAccessibilityZoomGesture::RegisterElementOperatorChildWork(const Registration &parameter,
    const int32_t treeId, const int64_t nodeId, const sptr<IAccessibilityElementOperator> &operation,
    const uint32_t tokenId, bool isApp)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
        HILOG_ERROR("Get current account data failed!!");
        return RET_ERR_REGISTER_EXIST;
    }

    sptr<AccessibilityWindowConnection> parentConnection =
        accountData->GetAccessibilityWindowConnection(parameter.parentWindowId);
    if (isApp && parentConnection) {
        sptr<IAccessibilityElementOperator> parentAamsOper =
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
    sptr<AccessibilityWindowConnection> oldConnection =
        accountData->GetAccessibilityWindowConnection(parameter.windowId);
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

void MockAccessibilityZoomGesture::SetTokenIdMapAndRootParentId(
    const sptr<AccessibilityWindowConnection> connection,
    const int32_t treeId, const int64_t nodeId, const uint32_t tokenId)
{
    connection->SetTokenIdMap(treeId, tokenId);
    connection->SetRootParentId(treeId, nodeId);
}

int32_t MockAccessibilityZoomGesture::ApplyTreeId()
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    int32_t curTreeId = preTreeId_ + 1;
    for (int32_t index = 0; index < TREE_ID_MAX; index++) {
        if (curTreeId == TREE_ID_MAX) {
            curTreeId = 0;
        }
        if (!treeIdPool_.test(curTreeId)) {
            treeIdPool_.set(curTreeId, true);
            preTreeId_ = curTreeId;
            return curTreeId + 1;
        }
        curTreeId++;
    }
    preTreeId_ = TREE_ID_MAX - 1;
    return 0;
}

void MockAccessibilityZoomGesture::RecycleTreeId(int32_t treeId)
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    if ((treeId > 0) && (treeId <= TREE_ID_MAX)) {
        treeIdPool_.set(treeId - 1, false);
    }
}

RetError MockAccessibilityZoomGesture::RegisterElementOperator(Registration parameter,
    const sptr<IAccessibilityElementOperator> &operation, bool isApp)
{
    if (CheckCallingUid() != RET_OK) {
        return RET_ERR_SAMGR;
    }
    int32_t treeIdSingle = ApplyTreeId();
    if (treeIdSingle == 0) {
        HILOG_ERROR("TreeId is used up.");
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

void MockAccessibilityZoomGesture::DeleteConnectionAndDeathRecipient(
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

RetError MockAccessibilityZoomGesture::DeregisterElementOperator(int32_t windowId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask([=]() {
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

RetError MockAccessibilityZoomGesture::DeregisterElementOperator(int32_t windowId, const int32_t treeId)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    handler_->PostTask([=]() {
        HILOG_INFO("Deregister windowId[%{public}d], treeId[%{public}d] start", windowId, treeId);
        RecycleTreeId(treeId);
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            return;
        }
        sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(windowId);
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

void MockAccessibilityZoomGesture::RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
    const sptr<AccessibilityWindowConnection> connection)
{
    auto object = connection->GetCardProxy(treeId);
    if (object == nullptr) {
        HILOG_ERROR("GetCardProxy is null");
        return;
    }
    auto remoteObject = object->AsObject();
    connection->EraseProxy(treeId);
    auto iter = interactionOperationDeathMap_.find(windowId);
    if (iter != interactionOperationDeathMap_.end()) {
        auto iterTree = iter->second.find(treeId);
        if (iterTree != iter->second.end()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient = iterTree->second;
            bool result = remoteObject->RemoveDeathRecipient(deathRecipient);
            HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
            iter->second.erase(iterTree);
        } else {
            HILOG_ERROR("cannot find remote object. treeId[%{public}d]", treeId);
        }
    } else {
        HILOG_ERROR("cannot find remote object. windowId[%{public}d]", windowId);
    }
}

RetError MockAccessibilityZoomGesture::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    return accessibilitySettings_->GetCaptionProperty(caption);
}

RetError MockAccessibilityZoomGesture::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    return accessibilitySettings_->SetCaptionProperty(caption);
}

RetError MockAccessibilityZoomGesture::SetCaptionState(const bool state)
{
    return accessibilitySettings_->SetCaptionState(state);
}

bool MockAccessibilityZoomGesture::GetEnabledState()
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

RetError MockAccessibilityZoomGesture::GetCaptionState(bool &state)
{
    return accessibilitySettings_->GetCaptionState(state);
}

bool MockAccessibilityZoomGesture::GetTouchGuideState()
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

bool MockAccessibilityZoomGesture::GetGestureState()
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

bool MockAccessibilityZoomGesture::GetKeyEventObserverState()
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

RetError MockAccessibilityZoomGesture::EnableAbility(const std::string &name, const uint32_t capabilities)
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

bool MockAccessibilityZoomGesture::SetTargetAbility(const int32_t targetAbilityValue)
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

bool MockAccessibilityZoomGesture::GetScreenReaderState()
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    return accountData->GetScreenReaderState();
}

RetError MockAccessibilityZoomGesture::InnerEnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

RetError MockAccessibilityZoomGesture::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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

RetError MockAccessibilityZoomGesture::SetCurtainScreenUsingStatus(bool isEnable)
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

RetError MockAccessibilityZoomGesture::DisableAbility(const std::string &name)
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

RetError MockAccessibilityZoomGesture::InnerDisableAbility(const std::string &name)
{
    HILOG_INFO();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "InnerDisableAbility:" + name);
#endif // OHOS_BUILD_ENABLE_HITRACE

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    if (accountData->GetConnectingA11yAbility(name) != nullptr) {
        HILOG_WARN("refuse to disconnect ability %{public}s when connecting", name.c_str());
        return RET_OK;
    }
    if (name == SCREEN_READER_BUNDLE_ABILITY_NAME) {
        ExecuteActionOnAccessibilityFocused(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
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

RetError MockAccessibilityZoomGesture::EnableUITestAbility(const sptr<IRemoteObject> &obj)
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
        accountData->AddEnabledAbility(uiTestUri);
        syncPromise.set_value(RET_OK);
        }, "TASK_ENABLE_UI_TEST_ABILITIES");
    return syncFuture.get();
}

RetError MockAccessibilityZoomGesture::DisableUITestAbility()
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
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
            std::bind(&AccessibilityAccountData::RemoveUITestClient, accountData, connection, UI_TEST_BUNDLE_NAME);
        handler_->PostTask(removeUITestClientFunc, "RemoveUITestClient");
        accountData->RemoveEnabledAbility(uiTestUri);
        syncPromise->set_value(RET_OK);
        }, "TASK_DISABLE_UI_TEST_ABILITIES");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait DisableUITestAbility result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

int32_t MockAccessibilityZoomGesture::GetActiveWindow()
{
    HILOG_DEBUG();
    return Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
}

bool MockAccessibilityZoomGesture::Init()
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

void MockAccessibilityZoomGesture::InitInnerResource()
{
    UpdateSettingsInAtoHosTask();
}

void MockAccessibilityZoomGesture::InteractionOperationDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY);
    HILOG_INFO();
    sptr<AccessibilityAccountData> accountData =
        Singleton<MockAccessibilityZoomGesture>::GetInstance().GetCurrentAccountData();
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
        Singleton<MockAccessibilityZoomGesture>::GetInstance().DeregisterElementOperator(windowId_, treeId_);
    } else {
        Singleton<MockAccessibilityZoomGesture>::GetInstance().DeregisterElementOperator(windowId_);
    }
}

sptr<AccessibilityAccountData> MockAccessibilityZoomGesture::GetCurrentAccountData()
{
    HILOG_DEBUG();
    if (currentAccountId_ == -1) {
        HILOG_ERROR("current account id is wrong");
        return nullptr;
    }

    return a11yAccountsData_.GetCurrentAccountData(currentAccountId_);
}

sptr<AccessibilityAccountData> MockAccessibilityZoomGesture::GetAccountData(int32_t accountId)
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAccountData(accountId);
}

std::vector<int32_t> MockAccessibilityZoomGesture::GetAllAccountIds()
{
    HILOG_DEBUG();
    return a11yAccountsData_.GetAllAccountIds();
}

sptr<AppExecFwk::IBundleMgr> MockAccessibilityZoomGesture::GetBundleMgrProxy()
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

sptr<AccessibilityWindowConnection> MockAccessibilityZoomGesture::GetAccessibilityWindowConnection(
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

void MockAccessibilityZoomGesture::ClearFocus(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->ClearFocus();
    }
}

void MockAccessibilityZoomGesture::OutsideTouch(int32_t windowId)
{
    HILOG_DEBUG();
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (connection && connection->GetProxy()) {
        connection->GetProxy()->OutsideTouch();
    }
}

void MockAccessibilityZoomGesture::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void MockAccessibilityZoomGesture::SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter)
{
    HILOG_DEBUG();
    keyEventFilter_ = keyEventFilter;
}

void MockAccessibilityZoomGesture::StateCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    Singleton<MockAccessibilityZoomGesture>::GetInstance().RemoveCallback(STATE_CALLBACK, this, remote);
}

void MockAccessibilityZoomGesture::CaptionPropertyCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<MockAccessibilityZoomGesture>::GetInstance().RemoveCallback(CAPTION_PROPERTY_CALLBACK, this, remote);
}

void MockAccessibilityZoomGesture::EnableAbilityListsObserverDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<MockAccessibilityZoomGesture>::GetInstance().RemoveCallback(
        ENABLE_ABILITY_LISTS_CALLBACK, this, remote);
}

void MockAccessibilityZoomGesture::AddedUser(int32_t accountId)
{
    HILOG_DEBUG();
    a11yAccountsData_.AddAccountData(accountId);
}

void MockAccessibilityZoomGesture::RemovedUser(int32_t accountId)
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

void MockAccessibilityZoomGesture::SwitchedUser(int32_t accountId)
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        defaultConfigCallbacks_ = accountData->GetConfigCallbacks();
        screenReaderState = accountData->GetDefaultUserScreenReaderState() ?
            SCREENREADER_STATE::ON : SCREENREADER_STATE::OFF;
        accountData->GetImportantEnabledAbilities(importantEnabledAbilities);
        accountData->OnAccountSwitched();
        UpdateAccessibilityManagerService();
    }
    currentAccountId_ = accountId;
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    accountData->Init();
    accountData->SetConfigCallbacks(defaultConfigCallbacks_);
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    float discount = accountData->GetConfig()->GetBrightnessDiscount();
    if (!Singleton<AccessibilityPowerManager>::GetInstance().DiscountBrightness(discount)) {
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
        UpdateAccessibilityManagerService();
    }
    UpdateAllSetting();
    UpdateAutoStartAbilities();
    RegisterShortKeyEvent();
    RegisterScreenMagnificationState();
    RegisterScreenMagnificationType();
}

void MockAccessibilityZoomGesture::PackageRemoved(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
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
        UpdateAccessibilityManagerService();
    }
}

void MockAccessibilityZoomGesture::PackageAdd(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
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

void MockAccessibilityZoomGesture::PackageChanged(const std::string &bundleName)
{
    sptr<AccessibilityAccountData> packageAccount = GetCurrentAccountData();
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
    UpdateAccessibilityManagerService();

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

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<MockAccessibilityZoomGesture>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<MockAccessibilityZoomGesture>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
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

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<MockAccessibilityZoomGesture>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
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

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<MockAccessibilityZoomGesture>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_DEBUG("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

void MockAccessibilityZoomGesture::ElementOperatorCallbackImpl::SetCursorPositionResult(const int32_t cursorPosition,
    const int32_t requestId)
{
    HILOG_INFO("ElementOperatorCallbackImpl::SetCursorPositionResult [result:%{public}d]",
        cursorPosition);
    HILOG_DEBUG("cursorPosition [result:%{public}d, requestId:%{public}d]", cursorPosition, requestId);
    callCursorPosition_ = cursorPosition;
    promise_.set_value();
}

bool MockAccessibilityZoomGesture::GetParentElementRecursively(int32_t windowId, int64_t elementId,
    std::vector<AccessibilityElementInfo>& infos)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return false;
    }

    int32_t treeId = 0;
    sptr<IAccessibilityElementOperator> elementOperator = nullptr;
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("GetAccessibilityWindowConnection failed");
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
    int32_t requestId = GenerateRequestId();
    AddRequestId(windowId, treeId, requestId, callBack);
    elementOperator->SearchElementInfoByAccessibilityId(elementId, requestId, callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        ipcTimeoutNum_++;
        HILOG_ERROR("Failed to wait result, number %{public}" PRId64 "", ipcTimeoutNum_);
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
} // namespace Accessibility
} // namespace OHOS