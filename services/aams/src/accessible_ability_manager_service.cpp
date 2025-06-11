/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#include <algorithm>
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
#include "parameters.h"
#include "system_ability_definition.h"
#include "utils.h"
#include "xcollie_helper.h"
#include <ipc_skeleton.h>
#include "transaction/rs_interfaces.h"
#include "resource_manager.h"
#include "res_config.h"
#include "bundle_info.h"
#include "locale_config.h"
#include "locale_info.h"
#include "accesstoken_kit.h"
#include "tokenid_kit.h"
#include "accessibility_caption.h"
#include "msdp_manager.h"
#include "security_component_manager.h"

using namespace std;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::AccessibilityConfig;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
    const std::string AAMS_ACTION_RUNNER_NAME = "AamsActionRunner";
    const std::string AAMS_SEND_EVENT_RUNNER_NAME = "AamsSendEventRunner";
    const std::string AAMS_CHANNEL_RUNNER_NAME = "AamsChannelRunner";
    const std::string AAMS_INPUT_MANAGER_RUNNER_NAME = "AamsInputManagerRunner";
    const std::string AAMS_GESTURE_RUNNER_NAME = "AamsGestureRunner";
    const std::string AAMS_HOVER_ENTER_RUNNER_NAME = "AamsHoverEnterRunner";
    const std::string AAMS_REGISTER_RUNNER_NAME = "AamsRegisterRunner";
    const std::string UI_TEST_BUNDLE_NAME = "ohos.uitest";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
    const std::string SCREEN_MAGNIFICATION_MODE = "accessibility_magnification_mode";
    const std::string SCREEN_MAGNIFICATION_SCALE = "accessibility_display_magnification_scale";
    const std::string VOICE_RECOGNITION_KEY = "accessibility_sound_recognition_switch";
    const std::string VOICE_RECOGNITION_TYPES = "accessibility_sound_recognition_enabled";
    const std::string DELAY_UNLOAD_TASK = "TASK_UNLOAD_ACCESSIBILITY_SA";
    const std::string USER_SETUP_COMPLETED = "user_setup_complete";
    const std::string ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const std::string SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
    constexpr int32_t INVALID_SHORTCUT_STATE = 2;
    constexpr int32_t QUERY_USER_ID_RETRY_COUNT = 600;
    constexpr int32_t QUERY_USER_ID_SLEEP_TIME = 50;
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

    static const std::map<std::string, int32_t> AccessibilityConfigTable = {
        {"HIGH_CONTRAST_TEXT", HIGH_CONTRAST_TEXT},
        {"INVERT_COLOR", INVERT_COLOR},
        {"DALTONIZATION_COLOR_FILTER", DALTONIZATION_COLOR_FILTER},
        {"CONTENT_TIMEOUT", CONTENT_TIMEOUT},
        {"ANIMATION_OFF", ANIMATION_OFF},
        {"BRIGHTNESS_DISCOUNT", BRIGHTNESS_DISCOUNT},
        {"AUDIO_MONO", AUDIO_MONO},
        {"AUDIO_BALANCE", AUDIO_BALANCE},
        {"MOUSE_KEY", MOUSE_KEY},
        {"CAPTION_STATE", CAPTION_STATE},
        {"CAPTION_STYLE", CAPTION_STYLE},
        {"SCREEN_MAGNIFICATION", SCREEN_MAGNIFICATION},
        {"MOUSE_AUTOCLICK", MOUSE_AUTOCLICK}
    };
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

    accessibilitySettings_ = std::make_shared<AccessibilitySettings>();
    accessibilityShortKey_ = std::make_shared<AccessibilityShortKey>();
    magnificationManager_ = std::make_shared<MagnificationManager>();
}

AccessibleAbilityManagerService::~AccessibleAbilityManagerService()
{
    HILOG_INFO("AccessibleAbilityManagerService::~AccessibleAbilityManagerService");

    inputInterceptor_ = nullptr;
    touchEventInjector_ = nullptr;
    keyEventFilter_ = nullptr;
    a11yAccountsData_.Clear();
}

void AccessibleAbilityManagerService::InitHandler()
{
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
}

void AccessibleAbilityManagerService::InitActionHandler()
{
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
}

void AccessibleAbilityManagerService::InitSendEventHandler()
{
    if (!sendEventRunner_) {
        sendEventRunner_ = AppExecFwk::EventRunner::Create(AAMS_SEND_EVENT_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!sendEventRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS sendEvent runner failed");
            return;
        }
    }

    if (!sendEventHandler_) {
        sendEventHandler_ = std::make_shared<AAMSEventHandler>(sendEventRunner_);
        if (!sendEventHandler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS sendEvent handler failed");
            return;
        }
    }
}

void AccessibleAbilityManagerService::InitChannelHandler()
{
    if (!channelRunner_) {
        channelRunner_ = AppExecFwk::EventRunner::Create(AAMS_CHANNEL_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!channelRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS channel runner failed");
            return;
        }
    }

    if (!channelHandler_) {
        channelHandler_ = std::make_shared<AAMSEventHandler>(channelRunner_);
        if (!channelHandler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS channel handler failed");
            return;
        }
    }
}

void AccessibleAbilityManagerService::InitInputManagerHandler()
{
    if (!inputManagerRunner_) {
#ifdef ACCESSIBILITY_WATCH_FEATURE
        inputManagerRunner_ = AppExecFwk::EventRunner::Create(AAMS_INPUT_MANAGER_RUNNER_NAME);
#else
        inputManagerRunner_ = AppExecFwk::EventRunner::Create(AAMS_INPUT_MANAGER_RUNNER_NAME,
            AppExecFwk::ThreadMode::FFRT);
#endif
        if (!inputManagerRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS input manager runner failed");
            return;
        }
    }
}

void AccessibleAbilityManagerService::InitGestureHandler()
{
    if (!gestureRunner_) {
        gestureRunner_ = AppExecFwk::EventRunner::Create(AAMS_GESTURE_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!gestureRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS gesture runner failed");
            return;
        }
    }
}

void AccessibleAbilityManagerService::InitHoverEnterHandler()
{
    if (!hoverEnterRunner_) {
        hoverEnterRunner_ = AppExecFwk::EventRunner::Create(AAMS_HOVER_ENTER_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!hoverEnterRunner_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS hoverEnter runner failed");
            return;
        }
    }

    if (!hoverEnterHandler_) {
        hoverEnterHandler_ = std::make_shared<AAMSEventHandler>(hoverEnterRunner_);
        if (!hoverEnterHandler_) {
            HILOG_ERROR("AccessibleAbilityManagerService::OnStart failed:create AAMS hoverEnter handler failed");
            return;
        }
    }
}

void AccessibleAbilityManagerService::OnStart()
{
    HILOG_INFO("AccessibleAbilityManagerService::OnStart start");

    InitHandler();
    InitActionHandler();
    InitSendEventHandler();
    InitChannelHandler();
    InitInputManagerHandler();
    InitGestureHandler();
    InitHoverEnterHandler();

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

        Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
        Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
        Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();
        if (Utils::IsWideFold() || Utils::IsBigFold()) {
            Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayModeListener();
        }

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
        RegisterVoiceRecognitionState();
        if (accessibilitySettings_) {
            accessibilitySettings_->RegisterParamWatcher();
        }
        }, "OnAddSystemAbility");
}

void AccessibleAbilityManagerService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_INFO("systemAbilityId:%{public}d removed!", systemAbilityId);
    Utils::RecordOnRemoveSystemAbility(systemAbilityId);
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

        if (systemAbilityId == DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID) {
            sptr<AccessibilityAccountData> accountData =
                Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
            if (accountData == nullptr) {
                HILOG_ERROR("get accountData failed");
                return;
            }
            auto config = accountData->GetConfig();
            if (config == nullptr) {
                HILOG_ERROR("config is nullptr");
                return;
            }
            config->SetInitializeState(false);
        }
        }, "OnRemoveSystemAbility");
}

int AccessibleAbilityManagerService::Dump(int fd, const std::vector<std::u16string>& args)
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
        HILOG_ERROR("tokenId error!");
        return RET_ERR_TOKEN_ID;
    }

    return RET_OK;
}

ErrCode AccessibleAbilityManagerService::SendEvent(const AccessibilityEventInfoParcel &eventInfoParcel,
    const int32_t flag)
{
    AccessibilityEventInfo uiEvent = static_cast<AccessibilityEventInfo>(eventInfoParcel);
    EventType eventType = uiEvent.GetEventType();
    HILOG_DEBUG("eventType[%{public}d] gestureId[%{public}d] windowId[%{public}d] compnentId: %{public}" PRId64 " "
        "elementId: %{public}" PRId64 " winId: %{public}d innerWinId: %{public}d treeId: %{public}d",
        uiEvent.GetEventType(), uiEvent.GetGestureType(), uiEvent.GetWindowId(), uiEvent.GetAccessibilityId(),
        uiEvent.GetElementInfo().GetAccessibilityId(), uiEvent.GetElementInfo().GetWindowId(),
        uiEvent.GetElementInfo().GetInnerWindowId(), uiEvent.GetElementInfo().GetBelongTreeId());
    if (!sendEventHandler_ || !hoverEnterHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return RET_ERR_NULLPTR;
    }
    if (flag) {
        if (VerifyingToKenId(uiEvent.GetElementInfo().GetWindowId(),
            uiEvent.GetElementInfo().GetAccessibilityId()) == RET_OK) {
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            return RET_ERR_CONNECTION_EXIST;
        }
    }

    RetError res = GetResourceBundleInfo(const_cast<AccessibilityEventInfo&>(uiEvent));
    if (res != RET_OK) {
        HILOG_ERROR("Get Resource BundleInfo failed! RetError is %{public}d", res);
    }

    auto sendEventTask = [this, uiEvent]() {
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
    };

    if (eventType == TYPE_VIEW_HOVER_ENTER_EVENT) {
        hoverEnterHandler_->PostTask(sendEventTask, "TASK_SEND_EVENT");
    } else {
        sendEventHandler_->PostTask(sendEventTask, "TASK_SEND_EVENT");
    }

    return RET_OK;
}

ErrCode AccessibleAbilityManagerService::RegisterStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver, uint32_t &state)
{
    HILOG_DEBUG();
    if (!stateObserver || !handler_) {
        HILOG_ERROR("parameters check failed!");
        return ERR_INVALID_DATA;
    }
    XCollieHelper timer(TIMER_REGISTER_STATE_OBSERVER, XCOLLIE_TIMEOUT);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!stateObserversDeathRecipient_) {
        stateObserversDeathRecipient_ = new(std::nothrow) StateCallbackDeathRecipient();
        if (!stateObserversDeathRecipient_) {
            HILOG_ERROR("stateObserversDeathRecipient_ is null");
            return ERR_INVALID_DATA;
        }
    }

    if (!stateObserver->AsObject()) {
        HILOG_ERROR("object is null");
        return ERR_INVALID_DATA;
    }

    stateObserver->AsObject()->AddDeathRecipient(stateObserversDeathRecipient_);
    stateObservers_.AddStateObserver(stateObserver);
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        return ERR_INVALID_DATA;
    }

    state = accountData->GetAccessibilityState();
    return ERR_OK;
}

ErrCode AccessibleAbilityManagerService::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return ERR_INVALID_DATA;
    }

    ffrt::promise<ErrCode> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<AccessibilityWindowManager>::GetInstance().GetRealWindowAndElementId(windowId, elementId);
        syncPromise.set_value(ERR_OK);
        }, "GET_REAL_WINDOW_AND_ELEMENT_ID");
    return syncFuture.get();
}

ErrCode AccessibleAbilityManagerService::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    HILOG_DEBUG("real windowId %{public}d", windowId);
    if (!handler_) {
        return ERR_INVALID_DATA;
    }

    ffrt::promise<ErrCode> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([&, this]() {
        Singleton<AccessibilityWindowManager>::GetInstance().GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        syncPromise.set_value(ERR_OK);
        }, "GET_SCENE_BOARD_INNER_WINDOW_ID");
    return syncFuture.get();
}

sptr<AccessibilityWindowConnection> AccessibleAbilityManagerService::GetRealIdConnection()
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

bool AccessibleAbilityManagerService::FindFocusedElementByConnection(sptr<AccessibilityWindowConnection> connection,
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

bool AccessibleAbilityManagerService::FindFocusedElement(AccessibilityElementInfo &elementInfo, uint32_t timeout)
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
    RETURN_FALSE_IF_NULL(accountData);
    int32_t realId =
        Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, FOCUS_TYPE_INVALID);
    connection = accountData->GetAccessibilityWindowConnection(realId);
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}" PRId64 "]", windowId, elementId);
    RETURN_FALSE_IF_NULL(connection);
    sptr<ElementOperatorCallbackImpl> callBack = new(std::nothrow) ElementOperatorCallbackImpl();
    RETURN_FALSE_IF_NULL(callBack);
    ffrt::future<void> promiseFuture = callBack->promise_.get_future();
    GetElementOperatorConnection(connection, elementId, elementOperator);
    RETURN_FALSE_IF_NULL(elementOperator);
    elementOperator->SearchElementInfoByAccessibilityId(elementId, GenerateRequestId(), callBack, 0);
    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(timeout));
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

void AccessibleAbilityManagerService::GetElementOperatorConnection(sptr<AccessibilityWindowConnection> &connection,
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

bool AccessibleAbilityManagerService::ExecuteActionOnAccessibilityFocused(const ActionType &action)
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
    AccessibilityElementInfo focusedElementInfo {};
    bool ret = Singleton<AccessibleAbilityManagerService>::GetInstance().FindFocusedElement(focusedElementInfo);
    if (ret) {
        actionArguments = SecurityComponentManager::GenerateActionArgumentsWithHMAC(action,
            focusedElementInfo.GetUniqueId(), focusedElementInfo.GetBundleName(), actionArguments);
    }
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

ErrCode AccessibleAbilityManagerService::RegisterCaptionObserver(
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

ErrCode AccessibleAbilityManagerService::RegisterEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG();
    if (!observer || !actionHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return ERR_INVALID_DATA;
    }
    XCollieHelper timer(TIMER_REGISTER_ENABLEABILITY_OBSERVER, XCOLLIE_TIMEOUT);
    std::shared_ptr<ffrt::promise<ErrCode>> syncPromisePtr = std::make_shared<ffrt::promise<ErrCode>>();
    ffrt::future syncFuture = syncPromisePtr->get_future();
    actionHandler_->PostTask([this, syncPromisePtr, observer]() {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            syncPromisePtr->set_value(ERR_INVALID_DATA);
            return;
        }
        if (!enableAbilityListsObserverDeathRecipient_) {
            enableAbilityListsObserverDeathRecipient_ = new(std::nothrow) EnableAbilityListsObserverDeathRecipient();
            if (!enableAbilityListsObserverDeathRecipient_) {
                HILOG_ERROR("enableAbilityListsObserverDeathRecipient_ is null");
                syncPromisePtr->set_value(ERR_INVALID_DATA);
                return;
            }
        }
        if (!observer->AsObject()) {
            HILOG_ERROR("object is null");
            syncPromisePtr->set_value(ERR_OK);
            return;
        }
        observer->AsObject()->AddDeathRecipient(enableAbilityListsObserverDeathRecipient_);
        accountData->AddEnableAbilityListsObserver(observer);
        syncPromisePtr->set_value(ERR_OK);
        }, "TASK_REGISTER_ENABLE_ABILITY_LISTS_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait RegisterEnableAbilityListsObserver result");
        return ERR_TRANSACTION_FAILED;
    }
    return syncFuture.get();
}

ErrCode AccessibleAbilityManagerService::GetAbilityList(uint32_t abilityTypes, int32_t stateType,
    std::vector<AccessibilityAbilityInfoParcel>& infos)
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
                AccessibilityAbilityInfoParcel info(ability);
                infos.push_back(info);
            }
        }
        HILOG_DEBUG("infos count is %{public}zu", infos.size());
        syncPromise.set_value(RET_OK);
        }, "TASK_GET_ABILITY_LIST");
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::IsApp() const
{
    HILOG_DEBUG();

    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_HAP) {
        HILOG_DEBUG("caller is an application");
        return true;
    }
    return false;
}

ErrCode AccessibleAbilityManagerService::RegisterElementOperatorByWindowId(
    const int32_t windowId, const sptr<IAccessibilityElementOperator> &elementOperator)
{
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (CheckCallingUid() != RET_OK) {
        return RET_ERR_SAMGR;
    }
    bool isApp = IsApp();
    handler_->PostTask([=]() {
        HILOG_INFO("Register windowId[%{public}d]", windowId);
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
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
            new(std::nothrow) AccessibilityWindowConnection(windowId, elementOperator, currentAccountId_);
        if (!connection) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
            HILOG_ERROR("New  AccessibilityWindowConnection failed!!");
            return;
        }
        connection->SetTokenIdMap(SINGLE_TREE_ID, tokenId);
        accountData->AddAccessibilityWindowConnection(windowId, connection);

        IsCheckWindowIdEventExist(windowId);
        if (elementOperator && elementOperator->AsObject()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient =
                new(std::nothrow) InteractionOperationDeathRecipient(windowId, currentAccountId_);
            if (!deathRecipient) {
                Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                    A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
                HILOG_ERROR("Create interactionOperationDeathRecipient failed");
                return;
            }

            bool result = elementOperator->AsObject()->AddDeathRecipient(deathRecipient);
            interactionOperationDeathRecipients_[windowId] = deathRecipient;
            HILOG_DEBUG("The result of adding elementOperator's death recipient is %{public}d", result);
        }
        }, "TASK_REGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

void AccessibleAbilityManagerService::IsCheckWindowIdEventExist(const int32_t windowId)
{
    if (CheckWindowIdEventExist(windowId)) {
        AccessibilityEventInfoParcel eventInfoParcel(windowFocusEventMap_.ReadVal(windowId));
        SendEvent(eventInfoParcel, 0);
        windowFocusEventMap_.Erase(windowId);
    }
}

RetError AccessibleAbilityManagerService::RegisterElementOperatorChildWork(const RegistrationPara &parameter,
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

        auto cardOper = parentConnection->GetCardProxy(treeId);
        if (cardOper == nullptr) {
            parentConnection->SetCardProxy(treeId, operation);
            SetTokenIdMapAndRootParentId(parentConnection, treeId, nodeId, tokenId);
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

void AccessibleAbilityManagerService::SetTokenIdMapAndRootParentId(
    const sptr<AccessibilityWindowConnection> connection,
    const int32_t treeId, const int64_t nodeId, const uint32_t tokenId)
{
    connection->SetTokenIdMap(treeId, tokenId);
    connection->SetRootParentId(treeId, nodeId);
}

int32_t AccessibleAbilityManagerService::ApplyTreeId()
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

void AccessibleAbilityManagerService::RecycleTreeId(int32_t treeId)
{
    std::lock_guard<ffrt::mutex> lock(treeIdPoolMutex_);
    if ((treeId > 0) && (treeId <= TREE_ID_MAX)) {
        treeIdPool_.set(treeId - 1, false);
    }
}

ErrCode AccessibleAbilityManagerService::RegisterElementOperatorByParameter(const RegistrationPara& parameter,
    const sptr<IAccessibilityElementOperator>& elementOperator)
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
        "element[%{public}" PRId64 "]", treeIdSingle, parameter.elementId, nodeId);

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    bool isApp = IsApp();
    handler_->PostTask([=]() {
        HILOG_INFO("Register windowId[%{public}d]", parameter.windowId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
        HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "RegisterElementOperator");
#endif // OHOS_BUILD_ENABLE_HITRACE
        if (RET_OK != RegisterElementOperatorChildWork(parameter, treeIdSingle, nodeId, elementOperator,
            tokenId, isApp)) {
            return;
        }
        if (CheckWindowIdEventExist(parameter.windowId)) {
            AccessibilityEventInfoParcel eventInfoParcel(windowFocusEventMap_.ReadVal(parameter.windowId));
            SendEvent(eventInfoParcel, 0);
            windowFocusEventMap_.Erase(parameter.windowId);
        }
        if (elementOperator && elementOperator->AsObject()) {
            sptr<IRemoteObject::DeathRecipient> deathRecipient =
                new(std::nothrow) InteractionOperationDeathRecipient(parameter.windowId, treeIdSingle,
                    currentAccountId_);
            if (deathRecipient == nullptr) {
                Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                    A11yError::ERROR_CONNECT_TARGET_APPLICATION_FAILED);
                HILOG_ERROR("Create interactionOperationDeathRecipient failed");
                return;
            }
            bool result = elementOperator->AsObject()->AddDeathRecipient(deathRecipient);
            interactionOperationDeathMap_[parameter.windowId][treeIdSingle] = deathRecipient;
            HILOG_DEBUG("The result of adding elementOperator's death recipient is %{public}d", result);
        }
        }, "TASK_REGISTER_ELEMENT_OPERATOR");
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

ErrCode AccessibleAbilityManagerService::DeregisterElementOperatorByWindowId(int32_t windowId)
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
        StopCallbackWait(windowId);

        if (!connection->GetProxy()) {
            HILOG_ERROR("proxy is null");
            return;
        }

        auto object = connection->GetProxy()->AsObject();
        if (object) {
            auto iter = interactionOperationDeathRecipients_.find(windowId);
            if (iter != interactionOperationDeathRecipients_.end()) {
                HILOG_DEBUG("delete death recipient, window id = %{public}d", windowId);
                sptr<IRemoteObject::DeathRecipient> deathRecipient = iter->second;
                bool result = object->RemoveDeathRecipient(deathRecipient);
                HILOG_DEBUG("The result of deleting operation's death recipient is %{public}d", result);
                interactionOperationDeathRecipients_.erase(iter);
            } else {
                HILOG_INFO("cannot find remote object. windowId[%{public}d]", windowId);
            }
        }

        connection->SetProxy(nullptr);
        RemoveTreeDeathRecipient(windowId, 0, connection);

        std::vector<int32_t> treeIds {};
        connection->GetAllTreeId(treeIds);
        for (int32_t treeId : treeIds) {
            RecycleTreeId(treeId);
            StopCallbackWait(windowId, treeId);
            RemoveTreeDeathRecipient(windowId, treeId, connection);
        }
        accountData->RemoveAccessibilityWindowConnection(windowId);
        }, "TASK_DEREGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

ErrCode AccessibleAbilityManagerService::DeregisterElementOperatorByWindowIdAndTreeId(int32_t windowId,
    const int32_t treeId)
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

        RemoveTreeDeathRecipient(windowId, treeId, connection);
        // remove connection when all proxy and children tree proxy deregistered
        if (connection->GetProxy() == nullptr && connection->GetCardProxySize() == 0) {
            accountData->RemoveAccessibilityWindowConnection(windowId);
        }
        Singleton<AccessibilityWindowManager>::GetInstance().RemoveTreeIdWindowIdPair(treeId);
        }, "TASK_DEREGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}

void AccessibleAbilityManagerService::RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
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

bool AccessibleAbilityManagerService::IsSystemApp() const
{
    HILOG_DEBUG();

    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType != TOKEN_HAP) {
        HILOG_INFO("Caller is not a application.");
        return true;
    }
    uint64_t accessTokenId = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApplication = TokenIdKit::IsSystemAppByFullTokenID(accessTokenId);
    return isSystemApplication;
}

bool AccessibleAbilityManagerService::CheckPermission(const std::string &permission) const
{
    HILOG_DEBUG();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_INVALID) {
        HILOG_WARN("AccessToken type invalid!");
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(callerToken, permission);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID denied!");
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, callerToken, permission.c_str());
    return true;
}

ErrCode AccessibleAbilityManagerService::GetCaptionProperty(CaptionPropertyParcel &caption)
{
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetCaptionProperty(caption);
}

ErrCode AccessibleAbilityManagerService::SetCaptionProperty(const CaptionPropertyParcel &caption)
{
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetCaptionProperty(caption);
}

ErrCode AccessibleAbilityManagerService::SetCaptionState(const bool state)
{
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetCaptionState(state);
}

ErrCode AccessibleAbilityManagerService::GetCaptionState(bool &state)
{
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetCaptionState(state);
}

ErrCode AccessibleAbilityManagerService::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
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

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }

    bool state;
    switch (targetAbilityValue) {
        case HIGH_CONTRAST_TEXT:
            state = accountData->GetConfig()->GetHighContrastTextState();
            Utils::RecordEnableShortkeyAbilityEvent("HIGH_CONTRAST_TEXT", !state);
            return accessibilitySettings_->SetHighContrastTextState(!state) == RET_OK;
        case INVERT_COLOR:
            state = accountData->GetConfig()->GetInvertColorState();
            Utils::RecordEnableShortkeyAbilityEvent("INVERT_COLOR", !state);
            return accessibilitySettings_->SetInvertColorState(!state) == RET_OK;
        case ANIMATION_OFF:
            state = accountData->GetConfig()->GetAnimationOffState();
            Utils::RecordEnableShortkeyAbilityEvent("ANIMATION_OFF", !state);
            return accessibilitySettings_->SetAnimationOffState(!state) == RET_OK;
        case SCREEN_MAGNIFICATION:
            state = accountData->GetConfig()->GetScreenMagnificationState();
            Utils::RecordEnableShortkeyAbilityEvent("SCREEN_MAGNIFICATION", !state);
            return accessibilitySettings_->SetScreenMagnificationState(!state) == RET_OK;
        case AUDIO_MONO:
            state = accountData->GetConfig()->GetAudioMonoState();
            Utils::RecordEnableShortkeyAbilityEvent("AUDIO_MONO", !state);
            return accessibilitySettings_->SetAudioMonoState(!state) == RET_OK;
        case MOUSE_KEY:
            state = accountData->GetConfig()->GetMouseKeyState();
            Utils::RecordEnableShortkeyAbilityEvent("MOUSE_KEY", !state);
            return accessibilitySettings_->SetMouseKeyState(!state) == RET_OK;
        case CAPTION_STATE:
            state = accountData->GetConfig()->GetCaptionState();
            Utils::RecordEnableShortkeyAbilityEvent("CAPTION_STATE", !state);
            return accessibilitySettings_->SetCaptionState(!state) == RET_OK;
        default:
            return false;
    }
}

ErrCode AccessibleAbilityManagerService::GetScreenReaderState(bool &state)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return ERR_INVALID_DATA;
    }
    state = accountData->GetScreenReaderState();
    return ERR_OK;
}

RetError AccessibleAbilityManagerService::InnerEnableAbility(const std::string &name, const uint32_t capabilities)
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

ErrCode AccessibleAbilityManagerService::GetEnabledAbilities(std::vector<std::string> &enabledAbilities)
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

ErrCode AccessibleAbilityManagerService::DisableAbility(const std::string &name)
{
    HILOG_INFO();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    std::string bundleName = "";
    if (!Utils::GetBundleNameByCallingUid(bundleName)) {
        return RET_ERR_FAILED;
    }
    auto &powerManager = Singleton<AccessibilityPowerManager>::GetInstance();
    if (!powerManager.UnholdRunningLock(bundleName)) {
        HILOG_ERROR("Failed to unhold running lock.");
        return RET_ERR_FAILED;
    }
#endif
    ffrt::promise<RetError> syncPromise;
    ffrt::future syncFuture = syncPromise.get_future();
    handler_->PostTask([this, &syncPromise, &name]() {
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

    if (!actionHandler_) {
        HILOG_ERROR("actionHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

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
        actionHandler_->PostTask([this]() {
            ExecuteActionOnAccessibilityFocused(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
            }, "TASK_CLEAR_FOCUS");
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

ErrCode AccessibleAbilityManagerService::SetMagnificationState(const bool state)
{
    HILOG_INFO("state = %{public}d", state);
    bool currentState = GetMagnificationState();
    uint32_t type = GetMagnificationType();
    uint32_t mode = GetMagnificationMode();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }

    if (state && !currentState) {
        HILOG_ERROR("magnificaiton is not enabled.");
        return RET_ERR_ENABLE_MAGNIFICATION;
    }

    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return RET_ERR_ENABLE_MAGNIFICATION;
    }

    if (state == magnificationManager_->GetMagnificationState()) {
        HILOG_ERROR("no need change state.");
        return RET_OK;
    }

    if (state) {
        magnificationManager_->TriggerMagnification(type, mode);
    } else {
        magnificationManager_->DisableMagnification();
    }
    return RET_OK;
}

ErrCode AccessibleAbilityManagerService::EnableUITestAbility(const sptr<IRemoteObject> &obj)
{
    HILOG_DEBUG();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
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

ErrCode AccessibleAbilityManagerService::DisableUITestAbility()
{
    HILOG_DEBUG();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
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

ErrCode AccessibleAbilityManagerService::GetActiveWindow(int32_t &windowId)
{
    HILOG_DEBUG();
    windowId = Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId();
    return ERR_OK;
}

bool AccessibleAbilityManagerService::Init()
{
    HILOG_DEBUG();
    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(handler_);
    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(handler_);
    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(handler_);
    if (Utils::IsWideFold() || Utils::IsBigFold()) {
        Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayModeListener();
    }
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
    sptr<AccessibilityAccountData> accountData =
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
        Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperatorByWindowIdAndTreeId(
            windowId_, treeId_);
    } else {
        Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperatorByWindowId(windowId_);
    }
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Current account data is null");
            return;
        }
        defaultConfigCallbacks_ = accountData->GetConfigCallbacks();
        screenReaderState = accountData->GetDefaultUserScreenReaderState() ?
            SCREENREADER_STATE::ON : SCREENREADER_STATE::OFF;
        if (screenReaderState == SCREENREADER_STATE::ON) {
            ExecuteActionOnAccessibilityFocused(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS);
        }
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
    UpdateVoiceRecognitionState();
    RegisterShortKeyEvent();
    RegisterScreenMagnificationState();
    RegisterScreenMagnificationType();
    RegisterVoiceRecognitionState();
}

void AccessibleAbilityManagerService::PackageRemoved(const std::string &bundleName)
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

void AccessibleAbilityManagerService::PackageAdd(const std::string &bundleName)
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

void AccessibleAbilityManagerService::PackageChanged(const std::string &bundleName)
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

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_ERROR("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_ERROR("VerifyingToKenId failed");
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

void AccessibleAbilityManagerService::ElementOperatorCallbackImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

bool AccessibleAbilityManagerService::GetParentElementRecursively(int32_t windowId, int64_t elementId,
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

void AccessibleAbilityManagerService::FindInnerWindowId(const AccessibilityEventInfo &event, int32_t& windowId)
{
    HILOG_DEBUG();
    auto mapTable = Singleton<AccessibilityWindowManager>::GetInstance().sceneBoardElementIdMap_.GetAllPairs();
    int64_t elementId = event.GetAccessibilityId();
    int tmpWindowId = Singleton<AccessibilityWindowManager>::GetInstance().
        FindTreeIdWindowIdPair(GetTreeIdBySplitElementId(elementId));
    if (tmpWindowId != 0) {
        windowId = tmpWindowId;
        return;
    }
    while (1) {
        auto iter = std::find_if(mapTable.begin(), mapTable.end(),
            [elementId] (const std::pair<int32_t, int64_t>& p) {
                return elementId == p.second;
            });
        if (iter != mapTable.end()) {
            windowId = iter->first;
            break;
        }
        if (event.GetWindowId() == 1 && elementId == 0) {
            HILOG_INFO("parent elementId is 0");
            break;
        }

        int32_t treeId = GetTreeIdBySplitElementId(elementId);
        // handle seprately because event send by UiExtension children tree may carry the root elemnt of children
        // tree, whose componentType is also root
        // deal other eventType like this may lead to performance problem
        if (treeId != 0) {
            // WindowScene
            //       \
            // UiExtensionComponent -> try to find the windowId of the event send by its children node
            //       \
            //       root -> node that send event, and it's a UiExtensionNode
            // when elementId is element that at the UiExtension tree, try to get the id of UiExtensionComponent
            // by GetRootParentId,
            elementId = GetRootParentId(windowId, treeId, elementId);
        } else {
            // keep find its parent node, until it's a root node or find its elementId in sceneBoardElementIdMap_
            // which saves mapping of windowId&root-elementId of the window.
            std::vector<AccessibilityElementInfo> infos = {};
            if (GetParentElementRecursively(event.GetWindowId(), elementId, infos) == false || infos.size() == 0) {
                HILOG_ERROR("find parent element failed");
                break;
            }

            if (infos[0].GetComponentType() == "root") {
                HILOG_ERROR("can not find parent element, has reach root node");
                break;
            }

            elementId = infos[0].GetParentNodeId();
        }
    }
    int originTreeId = GetTreeIdBySplitElementId(event.GetAccessibilityId());
    Singleton<AccessibilityWindowManager>::GetInstance().InsertTreeIdWindowIdPair(originTreeId, windowId);
}

void AccessibleAbilityManagerService::UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event)
{
    EventType evtType = event.GetEventType();
    HILOG_DEBUG("windowId is %{public}d", event.GetWindowId());
    int32_t windowId = event.GetWindowId();
    if (windowId == 1) {
        int32_t innerWindowId = event.GetElementInfo().GetInnerWindowId();
        if (innerWindowId > 0) {
            windowId = innerWindowId;
        } else {
            FindInnerWindowId(event, windowId);
        }
    }

    const_cast<AccessibilityEventInfo&>(event).SetElementMainWindowId(windowId);

    switch (evtType) {
        case TYPE_VIEW_HOVER_ENTER_EVENT:
            Singleton<AccessibilityWindowManager>::GetInstance().SetActiveWindow(windowId, false);
            Singleton<AccessibilityWindowManager>::GetInstance().SetAccessibilityFocusedWindow(windowId);
            break;
        case TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT:
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
    UpdateShortKeyRegister();
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
    return accessibilitySettings_->UpdateCaptionProperty();
}

void AccessibleAbilityManagerService::UpdateSettingsInAtoHosTask()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("UpdateSettingsInAtoHosTask: handler is nullptr!");
        return;
    }
    handler_->PostTask([=]() {
        UpdateSettingsInAtoHos();
        }, "UPDATE_SETTINGS_IN_ATOHOS_TASK");
}

void AccessibleAbilityManagerService::UpdateAutoStartAbilities()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("UpdateAutoStartAbilities: handler is nullptr.");
        return;
    }

    handler_->PostTask([=]() {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }
        for (auto &abilityId : removedAutoStartAbilities_) {
            if (accountData->GetAbilityAutoStartState(abilityId)) {
                accountData->SetAbilityAutoStartState(abilityId, false);
            }
        }
        accountData->UpdateAutoStartEnabledAbilities();
        accountData->UpdateAbilities();
        }, "UPDATE_AUTO_START_ABILITIES");
}

void AccessibleAbilityManagerService::UpdateSettingsInAtoHos()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }

    if (!accountData->GetConfig()->GetStartToHosState() || currentAccountId_ != DEFAULT_ACCOUNT_ID) {
        HILOG_INFO("Not first start from a to hos.");
        return;
    }
    accessibilitySettings_->UpdateSettingsInAtoHos();
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

void AccessibleAbilityManagerService::UpdateAllSetting()
{
    accessibilitySettings_->UpdateAllSetting();
}

void AccessibleAbilityManagerService::UpdateShortKeyRegister()
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("Account data is null!");
        return;
    }

    bool shortKeyState = accountData->GetConfig()->GetShortKeyState();
    if (shortKeyState) {
        accessibilityShortKey_->Unregister();
        accessibilityShortKey_->Register();
    } else {
        accessibilityShortKey_->Unregister();
    }
}

ErrCode AccessibleAbilityManagerService::SetScreenMagnificationState(const bool state)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetScreenMagnificationState(state);
}

ErrCode AccessibleAbilityManagerService::SetShortKeyState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetShortKeyState(state);
}

ErrCode AccessibleAbilityManagerService::SetMouseKeyState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetMouseKeyState(state);
}

ErrCode AccessibleAbilityManagerService::SetMouseAutoClick(const int32_t time)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetMouseAutoClick(time);
}

ErrCode AccessibleAbilityManagerService::SetShortkeyTarget(const std::string &name)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetShortkeyTarget(name);
}

ErrCode AccessibleAbilityManagerService::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetShortkeyMultiTarget(name);
}

ErrCode AccessibleAbilityManagerService::SetHighContrastTextState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetHighContrastTextState(state);
}

ErrCode AccessibleAbilityManagerService::SetDaltonizationState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetDaltonizationState(state);
}

ErrCode AccessibleAbilityManagerService::SetInvertColorState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetInvertColorState(state);
}

ErrCode AccessibleAbilityManagerService::SetAnimationOffState(const bool state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetAnimationOffState(state);
}

ErrCode AccessibleAbilityManagerService::SetAudioMonoState(const bool state)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetAudioMonoState(state);
}

ErrCode AccessibleAbilityManagerService::SetDaltonizationColorFilter(const uint32_t filter)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetDaltonizationColorFilter(filter);
}

ErrCode AccessibleAbilityManagerService::SetContentTimeout(const uint32_t time)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetContentTimeout(time);
}

ErrCode AccessibleAbilityManagerService::SetBrightnessDiscount(const float discount)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetBrightnessDiscount(discount);
}

ErrCode AccessibleAbilityManagerService::SetAudioBalance(const float balance)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetAudioBalance(balance);
}

ErrCode AccessibleAbilityManagerService::SetClickResponseTime(const uint32_t time)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetClickResponseTime(time);
}

ErrCode AccessibleAbilityManagerService::SetIgnoreRepeatClickState(const bool state)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetIgnoreRepeatClickState(state);
}

ErrCode AccessibleAbilityManagerService::SetIgnoreRepeatClickTime(const uint32_t time)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->SetIgnoreRepeatClickTime(time);
}

ErrCode AccessibleAbilityManagerService::GetScreenMagnificationState(bool &state)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetScreenMagnificationState(state);
}

ErrCode AccessibleAbilityManagerService::GetShortKeyState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetShortKeyState(state);
}

ErrCode AccessibleAbilityManagerService::GetMouseKeyState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetMouseKeyState(state);
}

ErrCode AccessibleAbilityManagerService::GetMouseAutoClick(int32_t &time)
{
    PostDelayUnloadTask();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("SetCaptionProperty permission denied.");
        return RET_ERR_NO_PERMISSION;
    }
    return accessibilitySettings_->GetMouseAutoClick(time);
}

ErrCode AccessibleAbilityManagerService::GetShortkeyTarget(std::string &name)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetShortkeyTarget(name);
}

ErrCode AccessibleAbilityManagerService::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetShortkeyMultiTarget(name);
}

ErrCode AccessibleAbilityManagerService::GetHighContrastTextState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetHighContrastTextState(state);
}

ErrCode AccessibleAbilityManagerService::GetDaltonizationState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetDaltonizationState(state);
}

ErrCode AccessibleAbilityManagerService::GetInvertColorState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetInvertColorState(state);
}

ErrCode AccessibleAbilityManagerService::GetAnimationOffState(bool &state)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetAnimationOffState(state);
}

ErrCode AccessibleAbilityManagerService::GetAudioMonoState(bool &state)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetAudioMonoState(state);
}

ErrCode AccessibleAbilityManagerService::GetDaltonizationColorFilter(uint32_t &type)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetDaltonizationColorFilter(type);
}

ErrCode AccessibleAbilityManagerService::GetContentTimeout(uint32_t &timer)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetContentTimeout(timer);
}

ErrCode AccessibleAbilityManagerService::GetBrightnessDiscount(float &brightness)
{
    PostDelayUnloadTask();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }
    return accessibilitySettings_->GetBrightnessDiscount(brightness);
}

ErrCode AccessibleAbilityManagerService::GetAudioBalance(float &balance)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetAudioBalance(balance);
}

ErrCode AccessibleAbilityManagerService::GetClickResponseTime(uint32_t &time)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetClickResponseTime(time);
}

ErrCode AccessibleAbilityManagerService::GetIgnoreRepeatClickState(bool &state)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetIgnoreRepeatClickState(state);
}

ErrCode AccessibleAbilityManagerService::GetIgnoreRepeatClickTime(uint32_t &time)
{
    PostDelayUnloadTask();
    return accessibilitySettings_->GetIgnoreRepeatClickTime(time);
}

ErrCode AccessibleAbilityManagerService::GetAllConfigs(AccessibilityConfigData& configData,
    CaptionPropertyParcel& caption)
{
    HILOG_DEBUG();
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }

    XCollieHelper timer(TIMER_GET_ALL_CONFIG, XCOLLIE_TIMEOUT);
    std::shared_ptr<ffrt::promise<ErrCode>> syncPromise = std::make_shared<ffrt::promise<ErrCode>>();
    std::shared_ptr<AccessibilityConfigData> config = std::make_shared<AccessibilityConfigData>();
    std::shared_ptr<CaptionProperty> captionInfo = std::make_shared<CaptionProperty>(caption);
    if (syncPromise == nullptr || config == nullptr || captionInfo == nullptr) {
        HILOG_WARN("create syncPromise or config failed");
        return ERR_INVALID_DATA;
    }
    ffrt::future syncFuture = syncPromise->get_future();
    actionHandler_->PostTask([this, syncPromise, config, captionInfo]() {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise->set_value(ERR_INVALID_DATA);
            return;
        }

        config->highContrastText_ = accountData->GetConfig()->GetHighContrastTextState();
        config->daltonizationState_ = accountData->GetConfig()->GetDaltonizationState();
        config->invertColor_ = accountData->GetConfig()->GetInvertColorState();
        config->animationOff_ = accountData->GetConfig()->GetAnimationOffState();
        config->audioMono_ = accountData->GetConfig()->GetAudioMonoState();
        config->mouseKey_ = accountData->GetConfig()->GetMouseKeyState();
        config->captionState_ = accountData->GetConfig()->GetCaptionState();
        config->screenMagnifier_ = accountData->GetConfig()->GetScreenMagnificationState();
        config->shortkey_ = accountData->GetConfig()->GetShortKeyState();
        config->mouseAutoClick_ = accountData->GetConfig()->GetMouseAutoClick();
        config->daltonizationColorFilter_ = accountData->GetConfig()->GetDaltonizationColorFilter();
        config->contentTimeout_ = accountData->GetConfig()->GetContentTimeout();
        config->brightnessDiscount_ = accountData->GetConfig()->GetBrightnessDiscount();
        config->audioBalance_ = accountData->GetConfig()->GetAudioBalance();
        config->shortkeyTarget_ = accountData->GetConfig()->GetShortkeyTarget();
        config->shortkeyMultiTarget_ = accountData->GetConfig()->GetShortkeyMultiTarget();
        *captionInfo = accountData->GetConfig()->GetCaptionProperty();
        syncPromise->set_value(ERR_OK);
        }, "TASK_GET_ALL_CONFIGS");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait GetAllConfigs result");
        return ERR_TRANSACTION_FAILED;
    }
    configData = *config;
    return syncFuture.get();
}

bool AccessibleAbilityManagerService::EnableShortKeyTargetAbility(const std::string &name)
{
    HILOG_DEBUG();
    HILOG_INFO("EnableShortKeyTargetAbility name = %{public}s", name.c_str());
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "EnableShortKeyTargetAbility");
#endif // OHOS_BUILD_ENABLE_HITRACE

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }

    std::string targetAbility;
    if (name != "") {
        targetAbility = name;
    } else {
        targetAbility = accountData->GetConfig()->GetShortkeyTarget();
        HILOG_DEBUG("target ability is [%{public}s]", targetAbility.c_str());
        if (targetAbility == "") {
            HILOG_ERROR("target ability is null");
            return false;
        }
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
        Utils::RecordEnableShortkeyAbilityEvent(targetAbility, false);
        return InnerDisableAbility(targetAbility) == RET_OK;
    } else if (enableState == RET_OK) {
        Utils::RecordEnableShortkeyAbilityEvent(targetAbility, true);
        return true;
    } else {
        return false;
    }
}

void AccessibleAbilityManagerService::OnShortKeyProcess()
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    std::shared_ptr<AccessibilityShortkeyDialog> shortkeyDialog = std::make_shared<AccessibilityShortkeyDialog>();

    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    bool oobeState = false;
    bool userSetupState = false;
    service->GetBoolValue(DEVICE_PROVISIONED, oobeState, true);
    if (accountData->GetConfig()->GetDbHandle()) {
        userSetupState = accountData->GetConfig()->GetDbHandle()->GetBoolValue(USER_SETUP_COMPLETED, false, true);
    }
    if (oobeState && userSetupState) {
        int32_t shortKeyTimeout = accountData->GetConfig()->GetShortKeyTimeout();
        if (shortKeyTimeout == SHORT_KEY_TIMEOUT_BEFORE_USE) {
            HILOG_INFO("first use short cut key");
            accountData->GetConfig()->SetShortKeyTimeout(SHORT_KEY_TIMEOUT_AFTER_USE);
            shortkeyDialog->ConnectDialog(ShortKeyDialogType::RECONFIRM);
            return;
        }
    }

    std::vector<std::string> shortkeyMultiTarget = accountData->GetConfig()->GetShortkeyMultiTarget();
    if (shortkeyMultiTarget.size() == 0) {
        EnableShortKeyTargetAbility();
    } else if (shortkeyMultiTarget.size() == 1) {
        EnableShortKeyTargetAbility(shortkeyMultiTarget[0]);
    } else {
        // dialog
        if (shortkeyDialog->ConnectDialog(ShortKeyDialogType::FUNCTION_SELECT)) {
            HILOG_DEBUG("ready to build dialog");
        }
    }
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

ErrCode AccessibleAbilityManagerService::RegisterConfigObserver(
    const sptr<IAccessibleAbilityManagerConfigObserver> &callback)
{
    HILOG_DEBUG();
    if (!callback || !actionHandler_) {
        HILOG_ERROR("Parameters check failed!");
        return ERR_INVALID_VALUE;
    }
    XCollieHelper timer(TIMER_REGISTER_CONFIG_OBSERVER, XCOLLIE_TIMEOUT);
    std::shared_ptr<ffrt::promise<uint32_t>> syncPromisePtr = std::make_shared<ffrt::promise<uint32_t>>();
    ffrt::future syncFuture = syncPromisePtr->get_future();
    actionHandler_->PostTask([this, syncPromisePtr, callback]() {
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
        }, "TASK_REGISTER_CONFIG_OBSERVER");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
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
    return accessibilitySettings_->UpdateConfigState();
}

void AccessibleAbilityManagerService::UpdateShortkeyTarget()
{
    return accessibilitySettings_->UpdateShortkeyTarget();
}

void AccessibleAbilityManagerService::UpdateShortkeyMultiTarget()
{
    return accessibilitySettings_->UpdateShortkeyMultiTarget();
}

void AccessibleAbilityManagerService::RemoveCallback(CallBackID callback,
    const sptr<DeathRecipient> &recipient, const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("remove callback[%{public}d]", callback);
    if (!handler_) {
        HILOG_ERROR("handler is nullptr");
        return;
    }
    handler_->PostTask([=]() {
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
        }, "RemoveCallback");
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

    handler_->PostTask([=]() {
        if (!remote.GetRefPtr() || !bundleManager_) {
            HILOG_ERROR("remote is null");
            return;
        }

        bundleManager_->AsObject()->RemoveDeathRecipient(bundleManagerDeathRecipient_);
        bundleManager_ = nullptr;
        }, "OnBundleManagerDied");
}

void AccessibleAbilityManagerService::StateObservers::AddStateObserver(
    const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver)
{
    std::lock_guard<ffrt::mutex> lock(stateObserversMutex_);
    auto iter = std::find(observersList_.begin(), observersList_.end(), stateObserver);
    if (iter == observersList_.end()) {
        observersList_.push_back(stateObserver);
        HILOG_DEBUG("register state observer successfully");
        return;
    }

    HILOG_INFO("state observer is existed");
}

void AccessibleAbilityManagerService::StateObservers::OnStateObservers(uint32_t state)
{
    std::lock_guard<ffrt::mutex> lock(stateObserversMutex_);
    for (auto& stateObserver : observersList_) {
        if (stateObserver) {
            stateObserver->OnStateChanged(state);
        }
    }
}

void AccessibleAbilityManagerService::StateObservers::RemoveStateObserver(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<ffrt::mutex> lock(stateObserversMutex_);
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
    std::lock_guard<ffrt::mutex> lock(stateObserversMutex_);
    observersList_.clear();
}

ErrCode AccessibleAbilityManagerService::GetFocusedWindowId(int32_t &focusedWindowId)
{
    HILOG_DEBUG();
    return Singleton<AccessibilityWindowManager>::GetInstance().GetFocusedWindowId(focusedWindowId);
}

void AccessibleAbilityManagerService::InsertWindowIdEventPair(int32_t windowId, const AccessibilityEventInfo &event)
{
    HILOG_DEBUG("insert event, windowId: %{public}d", windowId);
    windowFocusEventMap_.EnsureInsert(windowId, event);
}

bool AccessibleAbilityManagerService::CheckWindowIdEventExist(int32_t windowId)
{
    AccessibilityEventInfo eventInfo;
    return windowFocusEventMap_.Find(windowId, eventInfo);
}

bool AccessibleAbilityManagerService::CheckWindowRegister(int32_t windowId)
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return false;
    }
    return accountData->GetAccessibilityWindowConnection(windowId) != nullptr;
}

void AccessibleAbilityManagerService::OnDeviceProvisioned()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    service->UnregisterObserver(DEVICE_PROVISIONED);
    if (accountData->GetConfig()->GetDbHandle()) {
        accountData->GetConfig()->GetDbHandle()->UnregisterObserver(USER_SETUP_COMPLETED);
    }
    if (accountData->GetDefaultUserScreenReaderState()) {
        HILOG_INFO("Modify shortKeyTimeout and shortKeyOnLockScreenState");
        accountData->GetConfig()->SetShortKeyTimeout(SHORT_KEY_TIMEOUT_AFTER_USE);
        accountData->GetConfig()->SetShortKeyOnLockScreenState(true);
        UpdateConfigState();
    }
}

void AccessibleAbilityManagerService::InitializeShortKeyState()
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    bool shortKeyFlag = false;
    if (accountData->GetAccountId() != DEFAULT_ACCOUNT_ID && accountData->GetConfig()->GetDbHandle() != nullptr) {
        if (accountData->GetConfig()->GetDbHandle()->GetIntValue(SHORTCUT_ENABLED, INVALID_SHORTCUT_STATE) ==
            INVALID_SHORTCUT_STATE) {
            HILOG_INFO("Initialize the shortcut key state of PrivateSpace");
            shortKeyFlag = true;
        }
    } else if (accountData->GetAccountId() == DEFAULT_ACCOUNT_ID) {
        HILOG_INFO("Initialize the shortcut key state of MainSpace");
        shortKeyFlag = true;
    }

    if (shortKeyFlag) {
        accountData->GetConfig()->SetShortKeyState(true);
        std::vector<std::string> tmpVec { SCREEN_READER_BUNDLE_ABILITY_NAME };
        accountData->GetConfig()->SetShortkeyMultiTarget(tmpVec);
        UpdateConfigState();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateShortKeyRegister();
    }
}

void AccessibleAbilityManagerService::RegisterProvisionCallback()
{
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    AccessibilitySettingObserver::UpdateFunc func = [ = ](const std::string &state) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnDeviceProvisioned();
    };
    service->RegisterObserver(DEVICE_PROVISIONED, func);
    if (accountData->GetConfig()->GetDbHandle() != nullptr) {
        accountData->GetConfig()->GetDbHandle()->RegisterObserver(USER_SETUP_COMPLETED, func);
    }
}

void AccessibleAbilityManagerService::RegisterShortKeyEvent()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    handler_->PostTask([=]() {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            return;
        }
        std::shared_ptr<AccessibilitySettingProvider> service =
            AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
        if (service == nullptr) {
            HILOG_ERROR("service is nullptr");
            return;
        }
        bool oobeState = false;
        bool userSetupState = false;
        service->GetBoolValue(DEVICE_PROVISIONED, oobeState, true);
        if (accountData->GetConfig()->GetDbHandle() != nullptr) {
            userSetupState = accountData->GetConfig()->GetDbHandle()->GetBoolValue(USER_SETUP_COMPLETED, false, true);
        }
        if (accountData->GetAccountId() == DEFAULT_ACCOUNT_ID && (oobeState == false || userSetupState == false)) {
            InitializeShortKeyState();
            RegisterProvisionCallback();
        } else if (accountData->GetAccountId() != DEFAULT_ACCOUNT_ID) {
            InitializeShortKeyState();
        }
        }, "REGISTER_SHORTKEY_OBSERVER");
}

void AccessibleAbilityManagerService::OffZoomGesture()
{
    HILOG_INFO();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return;
    }
    magnificationManager_->DisableMagnification();
}

void AccessibleAbilityManagerService::OnScreenMagnificationStateChanged()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    std::shared_ptr<AccessibilitySettingsConfig> config = accountData->GetConfig();
    if (config == nullptr) {
        HILOG_ERROR("config is nullptr");
        return;
    }

    if (config->GetDbHandle() == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return;
    }

    bool screenMagnificationEnabled = false;
    screenMagnificationEnabled = config->GetDbHandle()->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false);
    config->SetScreenMagnificationState(screenMagnificationEnabled);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
    if (!screenMagnificationEnabled) {
        OffZoomGesture();
    }
}

void AccessibleAbilityManagerService::RegisterScreenMagnificationState()
{
    HILOG_DEBUG();
    if (handler_ == nullptr) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    handler_->PostTask([=]() {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (accountData == nullptr) {
            HILOG_ERROR("accountData is nullptr");
            return;
        }

        AccessibilitySettingObserver::UpdateFunc func = [ = ](const std::string &state) {
            Singleton<AccessibleAbilityManagerService>::GetInstance().OnScreenMagnificationStateChanged();
        };
        if (accountData->GetConfig()->GetDbHandle()) {
            accountData->GetConfig()->GetDbHandle()->RegisterObserver(SCREEN_MAGNIFICATION_KEY, func);
        }
        }, "REGISTER_SCREEN_ZOOM_OBSERVER");
}

void AccessibleAbilityManagerService::OnScreenMagnificationTypeChanged()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    std::shared_ptr<AccessibilitySettingsConfig> config = accountData->GetConfig();
    if (config == nullptr) {
        HILOG_ERROR("config is nullptr");
        return;
    }

    if (config->GetDbHandle() == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return;
    }

    uint32_t screenMagnificationType = 0;
    screenMagnificationType =
        static_cast<uint32_t>(config->GetDbHandle()->GetIntValue(SCREEN_MAGNIFICATION_TYPE, 0));
    config->SetScreenMagnificationType(screenMagnificationType);
    if (magnificationManager_ != nullptr) {
        magnificationManager_->OnMagnificationTypeChanged(screenMagnificationType);
    }
}

void AccessibleAbilityManagerService::RegisterScreenMagnificationType()
{
    HILOG_DEBUG();
    if (handler_ == nullptr) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    handler_->PostTask([=]() {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (accountData == nullptr) {
            HILOG_ERROR("accountData is nullptr");
            return;
        }

        AccessibilitySettingObserver::UpdateFunc func = [ = ](const std::string &state) {
            Singleton<AccessibleAbilityManagerService>::GetInstance().OnScreenMagnificationTypeChanged();
        };
        if (accountData->GetConfig()->GetDbHandle()) {
            accountData->GetConfig()->GetDbHandle()->RegisterObserver(SCREEN_MAGNIFICATION_TYPE, func);
        }
        }, "REGISTER_SCREEN_ZOOM_TYPE_OBSERVER");
}

void AccessibleAbilityManagerService::UpdateVoiceRecognitionState()
{
    HILOG_INFO();
    {
        std::lock_guard<ffrt::mutex> lock(subscribeMSDPMutex_);
        if (isSubscribeMSDPCallback_) {
            MsdpManager::GetInstance().UnSubscribeVoiceRecognition();
            isSubscribeMSDPCallback_ = false;
            HILOG_INFO("userstatusClient.Unsubscribe");
        }
    }
    OnVoiceRecognitionChanged();
}

void AccessibleAbilityManagerService::OnVoiceRecognitionChanged()
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }

    std::shared_ptr<AccessibilitySettingsConfig> config = accountData->GetConfig();
    if (config == nullptr) {
        HILOG_ERROR("config is nullptr");
        return;
    }

    if (config->GetDbHandle() == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return;
    }

    bool voiceRecognitionEnabled = config->GetDbHandle()->GetBoolValue(VOICE_RECOGNITION_KEY, false);
    std::string voiceRecognitionTypes = config->GetDbHandle()->GetStringValue(VOICE_RECOGNITION_TYPES, "");

    std::lock_guard<ffrt::mutex> lock(subscribeMSDPMutex_);
    if (!isSubscribeMSDPCallback_ && voiceRecognitionEnabled && !voiceRecognitionTypes.empty()) {
        int32_t ret = MsdpManager::GetInstance().SubscribeVoiceRecognition();
        isSubscribeMSDPCallback_ = true;
        HILOG_INFO("SubscribeVoiceRecognition ret: %{public}d", ret);
        return;
    }

    if (isSubscribeMSDPCallback_ && (!voiceRecognitionEnabled || voiceRecognitionTypes.empty())) {
        MsdpManager::GetInstance().UnSubscribeVoiceRecognition();
        isSubscribeMSDPCallback_ = false;
        return;
    }
}

void AccessibleAbilityManagerService::RegisterVoiceRecognitionState()
{
    HILOG_INFO();
    if (handler_ == nullptr) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    handler_->PostTask([=]() {
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (accountData == nullptr) {
            HILOG_ERROR("accountData is nullptr");
            return;
        }

        AccessibilitySettingObserver::UpdateFunc func = [ = ](const std::string &state) {
            Singleton<AccessibleAbilityManagerService>::GetInstance().OnVoiceRecognitionChanged();
        };

        if (accountData->GetConfig()->GetDbHandle()) {
            accountData->GetConfig()->GetDbHandle()->RegisterObserver(VOICE_RECOGNITION_KEY, func);
            accountData->GetConfig()->GetDbHandle()->RegisterObserver(VOICE_RECOGNITION_TYPES, func);
        }
        }, "REGISTER_VOICE_RECOGNITION");
}

void AccessibleAbilityManagerService::PostDelayUnloadTask()
{
#ifdef ACCESSIBILITY_WATCH_FEATURE
    auto task = [=]() {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            HILOG_ERROR("failed to get system ability mgr");
            return;
        }
        if (!IsNeedUnload()) {
            Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
            return;
        }
        int32_t ret = systemAbilityManager->UnloadSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
        if (ret != ERR_OK) {
            HILOG_ERROR("unload system ability failed");
            return;
        }
        SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
    };
    handler_->RemoveTask(DELAY_UNLOAD_TASK);
    handler_->PostTask(task, DELAY_UNLOAD_TASK, UNLOAD_TASK_INTERNAL);
#endif
}

bool AccessibleAbilityManagerService::IsNeedUnload()
{
    HILOG_DEBUG();
#ifndef ACCESSIBILITY_WATCH_FEATURE
    // always return true to avoid stablity problem
    return false;
#else // for watch
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return true;
    }

    // do not unload when any extension is enabled
    std::vector<std::string> enableAbilityList = accountData->GetEnabledAbilities();
    if (enableAbilityList.size() != 0) {
        return false;
    }
    return true;
#endif // ACCESSIBILITY_WATCH_FEATURE
}

int32_t AccessibleAbilityManagerService::GetTreeIdBySplitElementId(const int64_t elementId)
{
    if (elementId < 0) {
        HILOG_DEBUG("The elementId is -1");
        return elementId;
    }
    int32_t treeId = (static_cast<uint64_t>(elementId) >> ELEMENT_MOVE_BIT);
    return treeId;
}

void AccessibleAbilityManagerService::AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
    sptr<IAccessibilityElementOperatorCallback> callback)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("Add windowId: %{public}d treeId: %{public}d requestId: %{public}d", windowId, treeId, requestId);
    if (!windowRequestIdMap_.count(windowId)) {
        windowRequestIdMap_[windowId] = {};
    }
    if (!windowRequestIdMap_[windowId].count(treeId)) {
        windowRequestIdMap_[windowId][treeId] = {};
    }
    if (!windowRequestIdMap_[windowId][treeId].count(requestId)) {
        windowRequestIdMap_[windowId][treeId].insert(requestId);
        requestIdMap_[requestId] = callback;
    }
}

ErrCode AccessibleAbilityManagerService::RemoveRequestId(int32_t requestId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("RemoveRequestId requestId: %{public}d", requestId);
    for (auto &window : windowRequestIdMap_) {
        for (auto &tree : window.second) {
            auto it = tree.second.find(requestId);
            if (it != tree.second.end()) {
                HILOG_DEBUG("tree.second.erase requestId:%{public}d", requestId);
                tree.second.erase(it);
            }
            auto ite = requestIdMap_.find(requestId);
            if (ite != requestIdMap_.end()) {
                HILOG_DEBUG("requestIdMap_.erase requestId:%{public}d", requestId);
                requestIdMap_.erase(ite);
            }
        }
    }
    return ERR_OK;
}

void AccessibleAbilityManagerService::StopCallbackWait(int32_t windowId)
{
    HILOG_INFO("StopCallbackWait start windowId: %{public}d", windowId);
    if (!windowRequestIdMap_.count(windowId)) {
        HILOG_DEBUG("windowId not exists");
        return;
    }
    for (auto iter = windowRequestIdMap_[windowId].begin(); iter != windowRequestIdMap_[windowId].end(); ++iter) {
        HILOG_DEBUG("stop callback wait windowId: %{public}d, treeId: %{public}d", windowId, iter->first);
        StopCallbackWait(windowId, iter->first);
    }
}

void AccessibleAbilityManagerService::StopCallbackWait(int32_t windowId, int32_t treeId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG("StopCallbackWait start windowId: %{public}d treeId: %{public}d", windowId, treeId);
    if (!windowRequestIdMap_.count(windowId)) {
        return;
    }
    if (!windowRequestIdMap_[windowId].count(treeId)) {
        return;
    }
    auto requestIds = windowRequestIdMap_[windowId][treeId];
    for (auto requestId = requestIds.begin(); requestId != requestIds.end();) {
        HILOG_DEBUG("stop callback wait windowId: %{public}d, requestId: %{public}d", windowId, *requestId);
        auto iter = requestIdMap_.find(*requestId);
        if (iter != requestIdMap_.end()) {
            HILOG_DEBUG("requestIdMap_ set callback and erase requestId:%{public}d", *requestId);
            sptr<IAccessibilityElementOperatorCallback> callback = requestIdMap_[*requestId];
            if (callback != nullptr) {
                callback->SetExecuteActionResult(false, *requestId);
            }
            requestIdMap_.erase(iter);
        }
        requestId = requestIds.erase(requestId);
    }
}

ErrCode AccessibleAbilityManagerService::GetRootParentId(int32_t windowId, int32_t treeId, int64_t &parentId)
{
    HILOG_INFO("aa search treeParent from aams,  windowId: %{public}d, treeId: %{public}d", windowId, treeId);
    sptr<AccessibilityWindowConnection> connection = GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_WARN("The operator of windowId[%{public}d] has not been registered.", windowId);
        return RET_ERR_NO_CONNECTION;
    }
    connection->GetRootParentId(treeId, parentId);
    return ERR_OK;
}

int32_t AccessibleAbilityManagerService::GenerateRequestId()
{
    int32_t requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    if (requestId == REQUEST_ID_MAX) {
        requestId_ = REQUEST_ID_MIN;
        requestId = requestId_.fetch_add(1, std::memory_order_relaxed);
    }
    return requestId;
}

RetError AccessibleAbilityManagerService::CheckCallingUid()
{
    int32_t accountId = Utils::GetUserIdByCallingUid();
    if (accountId != currentAccountId_ && accountId != ROOT_UID) {
        HILOG_WARN("accountId is diff from currentAccountId_.");
        return RET_ERR_SAMGR;
    }
    return RET_OK;
}

void AccessibleAbilityManagerService::OnDataClone()
{
    std::shared_ptr<AccessibilitySettingProvider> service =
        AccessibilitySettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID);
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    bool cloneState = false;
    service->GetBoolValue(ACCESSIBILITY_CLONE_FLAG, cloneState);
    if (cloneState == false) {
        return;
    }
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_WARN("accountData is nullptr.");
        return;
    }
    if (accountData->GetConfig() != nullptr) {
        accountData->GetConfig()->OnDataClone();
        UpdateAllSetting();
        UpdateAutoStartAbilities();
        UpdateInputFilter();
        UpdateVoiceRecognitionState();
        HILOG_INFO("accessibility reload config.");
    } else {
        HILOG_WARN("config_ is nullptr");
    }
}

RetError AccessibleAbilityManagerService::GetResourceBundleInfo(AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG("BundleName is %{public}s, ModuleName is %{public}s, ResourceId is %{public}d",
        eventInfo.GetResourceBundleName().c_str(), eventInfo.GetResourceModuleName().c_str(),
        eventInfo.GetResourceId());
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("get accountData failed");
        return RET_ERR_NULLPTR;
    }
    int32_t userId = accountData->GetAccountId();

    AppExecFwk::BundleInfo bundleInfo;
    ErrCode ret =
        Singleton<AccessibilityResourceBundleManager>::GetInstance().GetBundleInfoV9(eventInfo.GetResourceBundleName(),
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE), bundleInfo, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("get BundleInfo failed!");
        return RET_ERR_FAILED;
    }

    std::string resourceValue;
    RetError res = GetResourceValue(eventInfo, bundleInfo, userId, resourceValue);
    if (res != RET_OK) {
        HILOG_ERROR("Get Resource Value failed");
        return res;
    }
    HILOG_DEBUG("resource value is %{public}s", resourceValue.c_str());
    eventInfo.SetTextAnnouncedForAccessibility(resourceValue);
    return RET_OK;
}

RetError AccessibleAbilityManagerService::GetResourceValue(AccessibilityEventInfo &eventInfo,
    AppExecFwk::BundleInfo bundleInfo, int32_t userId, std::string &result)
{
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (resConfig == nullptr) {
        HILOG_ERROR("create resConfig failed");
        return RET_ERR_NULLPTR;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(Global::I18n::LocaleConfig::GetSystemLanguage(), status);
    resConfig->SetLocaleInfo(locale.getLanguage(), locale.getScript(), locale.getCountry());

    std::string hapPath;
    std::vector<std::string> overlayPaths;
    int32_t appType = 0;
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager(
        eventInfo.GetResourceBundleName(), eventInfo.GetResourceModuleName(), hapPath, overlayPaths, *resConfig,
        appType, userId));
    if (resourceManager == nullptr) {
        HILOG_ERROR("create Resource manager failed");
        return RET_ERR_NULLPTR;
    }

    Global::Resource::RState state = resourceManager->UpdateResConfig(*resConfig);
    if (state != Global::Resource::RState::SUCCESS) {
        HILOG_ERROR("UpdateResConfig failed! errCode: %{public}d", state);
        return RET_ERR_FAILED;
    }

    for (const auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        std::string moduleResPath = hapModuleInfo.hapPath.empty() ? hapModuleInfo.resourcePath : hapModuleInfo.hapPath;
        HILOG_DEBUG("hapModuleInfo.hapPath is %{public}s", hapModuleInfo.hapPath.c_str());
        if (moduleResPath.empty()) {
            HILOG_ERROR("moduleResPath is empty");
            continue;
        }
        if (!resourceManager->AddResource(moduleResPath.c_str())) {
            HILOG_ERROR("AddResource is failed");
        }
    }

    Global::Resource::RState res = resourceManager->GetStringById(eventInfo.GetResourceId(), result);
    if (res != Global::Resource::RState::SUCCESS) {
        HILOG_ERROR("get resource value failed");
        return RET_ERR_FAILED;
    }
    return RET_OK;
}

std::shared_ptr<AccessibilityDatashareHelper> AccessibleAbilityManagerService::GetCurrentAcountDatashareHelper()
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }

    std::shared_ptr<AccessibilitySettingsConfig> config = accountData->GetConfig();
    if (config == nullptr) {
        HILOG_ERROR("config is nullptr");
        return nullptr;
    }

    return config->GetDbHandle();
}

bool AccessibleAbilityManagerService::GetMagnificationState()
{
    HILOG_DEBUG();
    bool magnificationState = false;
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return magnificationState;
    }

    magnificationState =
        static_cast<uint32_t>(helper->GetBoolValue(SCREEN_MAGNIFICATION_KEY, false));
    return magnificationState;
}

uint32_t AccessibleAbilityManagerService::GetMagnificationType()
{
    HILOG_DEBUG();
    uint32_t magnificationType = FULL_SCREEN_MAGNIFICATION;
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return magnificationType;
    }

    magnificationType =
        static_cast<uint32_t>(helper->GetIntValue(SCREEN_MAGNIFICATION_TYPE, FULL_SCREEN_MAGNIFICATION));
    return magnificationType;
}

uint32_t AccessibleAbilityManagerService::GetMagnificationMode()
{
    HILOG_DEBUG();
    uint32_t magnificationMode = FULL_SCREEN_MAGNIFICATION;
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return magnificationMode;
    }

    magnificationMode =
        static_cast<uint32_t>(helper->GetIntValue(SCREEN_MAGNIFICATION_MODE, FULL_SCREEN_MAGNIFICATION));
    return magnificationMode;
}

void AccessibleAbilityManagerService::SetMagnificationMode(int32_t mode)
{
    HILOG_DEBUG();
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return;
    }

    helper->PutIntValue(SCREEN_MAGNIFICATION_MODE, mode, false);
}

float AccessibleAbilityManagerService::GetMagnificationScale()
{
    HILOG_DEBUG();
    float magnificationScale = DEFAULT_SCALE;
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return magnificationScale;
    }

    magnificationScale =
        static_cast<uint32_t>(helper->GetFloatValue(SCREEN_MAGNIFICATION_SCALE, DEFAULT_SCALE));
    return magnificationScale;
}

void AccessibleAbilityManagerService::SetMagnificationScale(float scale)
{
    HILOG_DEBUG();
    shared_ptr<AccessibilityDatashareHelper> helper = GetCurrentAcountDatashareHelper();
    if (helper == nullptr) {
        HILOG_ERROR("datashareHelper is nullptr");
        return;
    }
    helper->PutFloatValue(SCREEN_MAGNIFICATION_SCALE, scale, false);
}

std::shared_ptr<MagnificationManager> AccessibleAbilityManagerService::GetMagnificationMgr()
{
    HILOG_DEBUG();
    return magnificationManager_;
}

std::shared_ptr<WindowMagnificationManager> AccessibleAbilityManagerService::GetWindowMagnificationManager()
{
    HILOG_DEBUG();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return nullptr;
    }
    return magnificationManager_->GetWindowMagnificationManager();
}

std::shared_ptr<FullScreenMagnificationManager> AccessibleAbilityManagerService::GetFullScreenMagnificationManager()
{
    HILOG_DEBUG();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return nullptr;
    }
    return magnificationManager_->GetFullScreenMagnificationManager();
}

int32_t AccessibleAbilityManagerService::SetEnhanceConfig(const char *cfg, uint32_t cfgLen)
{
    HILOG_INFO();
    int32_t result = SecurityComponentManager::SetEnhanceConfig(cfg, cfgLen);
    return result;
}
} // namespace Accessibility
} // namespace OHOS