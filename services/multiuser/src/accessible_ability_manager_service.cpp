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
#include "accessibility_account_data.h"
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
#include "accessibility_permission.h"
#include "mem_mgr_client.h"
#include "mem_mgr_proxy.h"
#include "magnification_def.h"

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
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
    const std::string SCREEN_MAGNIFICATION_MODE = "accessibility_magnification_mode";
    const std::string SCREEN_MAGNIFICATION_SCALE = "accessibility_display_magnification_scale";
    const std::string MAGNIFICATION_PARAM = "const.accessibility.magnification";
    const std::string VOICE_RECOGNITION_KEY = "accessibility_sound_recognition_switch";
    const std::string VOICE_RECOGNITION_TYPES = "accessibility_sound_recognition_enabled";
    const std::string DELAY_UNLOAD_TASK = "TASK_UNLOAD_ACCESSIBILITY_SA";
    const std::string USER_SETUP_COMPLETED = "user_setup_complete";
    const std::string ACCESSIBILITY_CLONE_FLAG = "accessibility_config_clone";
    const std::string SHORTCUT_ENABLED = "accessibility_shortcut_enabled";
    const std::string HAP_PATH = "/system/app/Settings/Settings.hap";
    const std::string HAP_BUNDLE = "com.ohos.settings";
    const std::string UI_TEST_ABILITY_NAME = "uitestability";
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
    const std::string MAGNIFICATION_SCALE = "magnification_scale";
    const std::string MAGNIFICATION_DISABLE = "magnification_disabled";
    const std::string SWITCH_FULL_SCREEN = "switch_full_screen_magnification";
    const std::string SWITCH_WINDOW = "switch_window_magnification";
    constexpr int32_t XCOLLIE_TIMEOUT = 6; // s
    constexpr int QUANTITY = 2; // plural string

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

    static std::map<std::string, std::string> ResourceMap = {
        {MAGNIFICATION_SCALE, ""},
        {MAGNIFICATION_DISABLE, ""},
        {SWITCH_FULL_SCREEN, ""},
        {SWITCH_WINDOW, ""}
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
    AddSystemAbilityListener(MEMORY_MANAGER_SA_ID);

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
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
        Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
#endif
        Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();
        UnsubscribeOsAccount();

        currentAccountId_ = -1;
        a11yAccountsData_.Clear();
        stateObservers_.Clear();
        inputInterceptor_ = nullptr;
        touchEventInjector_ = nullptr;
        keyEventFilter_ = nullptr;
        stateObserversDeathRecipient_ = nullptr;

        syncPromise.set_value();
        }, "TASK_ONSTOP");
    syncFuture.wait();

    for (auto &iter : dependentServicesStatus_) {
        iter.second = false;
    }

    isReady_ = false;
    isPublished_ = false;
    SetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false");
    int pid = getpid();
    Memory::MemMgrClient::GetInstance().NotifyProcessStatus(pid, 1, 0, ACCESSIBILITY_MANAGER_SERVICE_ID);
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
        if (systemAbilityId == MEMORY_MANAGER_SA_ID) {
            int pid = getpid();
            Memory::MemMgrClient::GetInstance().NotifyProcessStatus(pid, 1, 1, ACCESSIBILITY_MANAGER_SERVICE_ID);
            return;
        }
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
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
            Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
#endif
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
    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get current account data failed!!");
        return RET_ERR_CONNECTION_EXIST;
    }

    return accountData->VerifyingToKenId(tokenId, windowId, elementId);
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
        OnFocusedEvent(uiEvent);
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
        actionArguments = AccessibilitySecurityComponentManager::GenerateActionArgumentsWithHMAC(action,
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
        sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Get current account data failed!!");
            return;
        }
        accountData->RegisterElementOperatorByWindowId(tokenId, windowId, elementOperator, isApp);
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

    sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("Get current account data failed!!");
        return RET_ERR_NULLPTR;
    }

    int32_t treeIdSingle = accountData->ApplyTreeId();
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
    handler_->PostTask([accountData, parameter, elementOperator, treeIdSingle, nodeId, tokenId, isApp]() {
        accountData->RegisterElementOperatorByParameter(parameter, elementOperator, treeIdSingle, nodeId, tokenId,
            isApp);
        }, "TASK_REGISTER_ELEMENT_OPERATOR");
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS