void AccessibleAbilityManagerService::UpdateVoiceRecognitionState()
{
    
    HILOG_INFO();
    {
        std::lock_guard<ffrt::mutex> lock(subscribeMSDPMutex_);
        if (isSubscribeMSDPCallback_) {
            auto &userstatusClient = Msdp::UserStatusAwareness::UserStatusClient::GetInstance();
            userstatusClient.Unsubscribe(FEATURE_VOICE_RECOGNITION);
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
    auto &userstatusClient = Msdp::UserStatusAwareness::UserStatusClient::GetInstance();

    std::lock_guard<ffrt::mutex> lock(subscribeMSDPMutex_);
    if (!isSubscribeMSDPCallback_ && voiceRecognitionEnabled && !voiceRecognitionTypes.empty()) {
        std::function<void(int32_t, std::shared_ptr<Msdp::UserStatusAwareness::UserStatusData>)> func = [ = ](
            int32_t callbackId, std::shared_ptr<Msdp::UserStatusAwareness::UserStatusData> userStatusData) {
            HILOG_INFO("voice recognition callback, Id: %{public}d, GetFeature: %{public}d, GetResult: %{public}d",
                callbackId, userStatusData->GetFeature(), userStatusData->GetResult());
            // 通知到设置
            AccessibilitySettingsManager::GetInstance().VoiceRecognize(userStatusData->GetResult());
        };
        int32_t ret = userstatusClient.SubscribeCallback(FEATURE_VOICE_RECOGNITION, func);
        userstatusClient.Subscribe(FEATURE_VOICE_RECOGNITION, {});
        isSubscribeMSDPCallback_ = true;
        HILOG_INFO("userstatusClient.SubscribeCallback RET: %{public}d", ret);
        return;
    }

    if (isSubscribeMSDPCallback_ && (!voiceRecognitionEnabled || voiceRecognitionTypes.empty())) {
        userstatusClient.Unsubscribe(FEATURE_VOICE_RECOGNITION);
        isSubscribeMSDPCallback_ = false;
        HILOG_INFO("userstatusClient.Unsubscribe");
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
#include "accessibility_settings_connection.h"
#include "user_status_client.h"

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
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
    const std::string DEVICE_PROVISIONED = "device_provisioned";
    const std::string SCREEN_MAGNIFICATION_KEY = "accessibility_display_magnification_enabled";
    const std::string SCREEN_MAGNIFICATION_TYPE = "accessibility_magnification_capability";
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
    constexpr int32_t FEATURE_VOICE_RECOGNITION = 17;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
    enum SCREENREADER_STATE : int32_t {
        UNINIT = -1,
        OFF = 0,
        ON = 1,
    };