/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "accessibility_notification_helper.h"
#include "accessible_ability_manager_service.h"
#include "timer_info.h"
#include "time_service_client.h"
 
#include "notification_helper.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"
#include "os_account_manager.h"
 
#include "hilog_wrapper.h"
#include "locale_config.h"
#include "locale_info.h"
 
#include <ctime>
 
#include "pixel_map.h"
#include "image_source.h"
 
namespace OHOS {
namespace Accessibility {
 
namespace {
constexpr uint64_t TWELVE_CLOCK = 12 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_2 = 2 * 24 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_5 = 5 * 24 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_8 = 8 * 24 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_15 = 15 * 24 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_22 = 22 * 24 * 60 * 60 * 1000;
constexpr uint64_t NOTIFICATION_DATA_30 = static_cast<uint64_t>(29) * static_cast<uint64_t>(24) *
                                          static_cast<uint64_t>(60) * static_cast<uint64_t>(60) *
                                          static_cast<uint64_t>(1000);
std::vector<uint64_t> NOTIFICATION_DATE{NOTIFICATION_DATA_2,
    NOTIFICATION_DATA_5,
    NOTIFICATION_DATA_8,
    NOTIFICATION_DATA_15,
    NOTIFICATION_DATA_22,
    NOTIFICATION_DATA_30};
 
const std::string SETTING_BUNDLE_NAME = "com.huawei.hmos.settings";
const std::string SETTING_ABILITY_NAME = "com.huawei.hmos.settings.MainAbility";
const std::string SERVICE_EXTENSION_ABILITY_NAME = "IgnoreRepeatClickExtService";
const std::string ACCESSIBILITY_SCREEN_TOUCH_URI = "accessibility_touchscreen_entry";
const std::string HAP_PATH = "/system/app/Settings/Settings.hap";
const std::string ICON_NAME = "ic_accessibility_notify";
const std::string IGNORE_REPEAT_CLICK_NOTIFICATION = "ignore_repeat_click_notification";
constexpr int32_t REQUEST_CODE = 8;
const std::string IGNORE_REPEAT_CLICK_NOTIFICATION_TITLE = "ignore_repeat_click_notification_title";
const std::string IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT = "ignore_repeat_click_notification_content";
const std::string IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_CANCEL =
    "ignore_repeat_click_notification_button_cancel";
const std::string IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_DISABLE =
    "ignore_repeat_click_notification_button_disable";
const std::string INGORE_REPEAT_CLICK_KEY = "ignore_repeat_click_switch";
constexpr int ACCESSIBILITY_SA_UID = 1103;
constexpr int ACCESSIBILITY_NOTIFICATION_UID = 1103801;
constexpr uint32_t NOTIFICATION_FLAG = 1 << 9;
constexpr float ICON_SIZE = 0.3;
 
std::map<std::string, std::string> notificationMap_ = {{IGNORE_REPEAT_CLICK_NOTIFICATION_TITLE, ""},
    {IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT, ""},
    {IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_CANCEL, ""},
    {IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_DISABLE, ""}};
 
std::vector<uint64_t> timersVec;
}  // namespace
 
static std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> GetWantAgent()
{
    std::shared_ptr<OHOS::AAFwk::Want> want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetElementName(SETTING_BUNDLE_NAME, SERVICE_EXTENSION_ABILITY_NAME);
    want->SetAction("disableIgnoreRepeatClick");
    std::vector<std::shared_ptr<OHOS::AAFwk::Want>> wants;
    wants.push_back(want);
    std::vector<OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    OHOS::AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(REQUEST_CODE,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags,
        wants,
        nullptr);
    return OHOS::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
}
 
static std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> GetCanceluttonWantAgent()
{
    std::shared_ptr<OHOS::AAFwk::Want> want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetAction("disableNotificaiton");
    want->SetElementName(SETTING_BUNDLE_NAME, SERVICE_EXTENSION_ABILITY_NAME);
 
    std::vector<std::shared_ptr<OHOS::AAFwk::Want>> wants;
    wants.push_back(want);
    std::vector<OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    OHOS::AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(REQUEST_CODE,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags,
        wants,
        nullptr);
    return OHOS::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
}
 
static std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> GetSettingsWantAgent()
{
    std::shared_ptr<OHOS::AAFwk::Want> want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetElementName(SETTING_BUNDLE_NAME, SETTING_ABILITY_NAME);
    want->SetUri(ACCESSIBILITY_SCREEN_TOUCH_URI);
    std::vector<std::shared_ptr<OHOS::AAFwk::Want>> wants;
    wants.push_back(want);
    std::vector<OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    OHOS::AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(REQUEST_CODE,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags,
        wants,
        nullptr);
    return OHOS::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
}
 
 
static std::shared_ptr<OHOS::Media::PixelMap> GetIcon()
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        HILOG_ERROR("resourceManager is null");
        return nullptr;
    }
    if (!resourceManager->AddResource(HAP_PATH.c_str())) {
        HILOG_ERROR("AddResource failed");
        return nullptr;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    resourceManager->GetMediaDataByName(ICON_NAME.c_str(), len, data);
    Media::SourceOptions opts;
    uint32_t errorCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(data.get(), len, opts, errorCode);
    Media::DecodeOptions decodeOpts;
    std::shared_ptr<OHOS::Media::PixelMap> icon;
    if (imageSource) {
        auto pixelMapPtr = imageSource->CreatePixelMap(decodeOpts, errorCode);
        icon = std::shared_ptr<Media::PixelMap>(pixelMapPtr.release());
    }
    if (errorCode != 0) {
        HILOG_ERROR("Failed to get icon");
        return nullptr;
    }
    if (icon == nullptr) {
        HILOG_ERROR("icon is null");
        return nullptr;
    }
    icon->scale(ICON_SIZE, ICON_SIZE);
    return icon;
}
 
static void GetReource()
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        HILOG_ERROR("resourceManager is null");
        return;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (resConfig == nullptr) {
        HILOG_ERROR("resConfig is null");
        return;
    }
    std::map<std::string, std::string> configs;
    OHOS::Global::I18n::LocaleInfo locale(Global::I18n::LocaleConfig::GetSystemLocale(), configs);
    std::string language = locale.GetLanguage();
    std::string script = locale.GetScript();
    std::string region = locale.GetRegion();
 
    resConfig->SetLocaleInfo(language.c_str(), script.c_str(), region.c_str());
    resourceManager->UpdateResConfig(*resConfig);
    if (!resourceManager->AddResource(HAP_PATH.c_str())) {
        HILOG_ERROR("AddResource failed");
        return;
    }
    for (auto &iter : notificationMap_) {
        std::string outValue;
        resourceManager->GetStringByName(iter.first.c_str(), outValue);
        HILOG_ERROR("outValue = %{public}s", outValue.c_str());
        notificationMap_[iter.first] = outValue;
    }
}
 
void IgnoreRepeatClickNotification::CancelNotification()
{
    Notification::NotificationHelper::CancelNotification(ACCESSIBILITY_NOTIFICATION_UID);
}
 
bool IgnoreRepeatClickNotification::IsSendIgnoreRepeatClickNotification()
{
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return true;
    }
    bool notificationState = true;
    bool ignoreRepeatClickState = false;
    if (accountData->GetConfig() && accountData->GetConfig()->GetDbHandle()) {
        notificationState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(IGNORE_REPEAT_CLICK_NOTIFICATION, true);
        ignoreRepeatClickState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(INGORE_REPEAT_CLICK_KEY, false);
    }
    return ignoreRepeatClickState && notificationState;
}
 
RetError IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder()
{
    if (!IsSendIgnoreRepeatClickNotification()) {
        HILOG_ERROR("no need notificatino");
        return RET_OK;
    }
    GetReource();
    Notification::NotificationRequest request;
    std::shared_ptr<Notification::NotificationActionButton> cancelButtion =
        Notification::NotificationActionButton::Create(nullptr,
            notificationMap_[IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_CANCEL],
            GetCanceluttonWantAgent());
    request.AddActionButton(cancelButtion);
    std::shared_ptr<Notification::NotificationActionButton> closeButton =
        Notification::NotificationActionButton::Create(
            nullptr, notificationMap_[IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT_BUTTON_DISABLE], GetWantAgent());
    request.AddActionButton(closeButton);
 
    std::shared_ptr<Notification::NotificationLongTextContent> content =
        std::make_shared<Notification::NotificationLongTextContent>();
    content->SetTitle(notificationMap_[IGNORE_REPEAT_CLICK_NOTIFICATION_TITLE]);
    content->SetLongText(notificationMap_[IGNORE_REPEAT_CLICK_NOTIFICATION_CONTENT]);
    std::shared_ptr<Notification::NotificationContent> notificationContent =
        std::make_shared<Notification::NotificationContent>(content);
    request.SetContent(notificationContent);
    request.SetCreatorUid(ACCESSIBILITY_SA_UID);
    request.SetCreatorPid(getpid());
    int32_t userId;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(ACCESSIBILITY_SA_UID, userId);
 
    request.SetCreatorUserId(userId);
    request.SetCreatorBundleName("");
    request.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SERVICE_REMINDER);
    request.SetInProgress(true);
    request.SetDistributed(true);
    request.SetNotificationControlFlags(NOTIFICATION_FLAG);
    request.SetNotificationId(ACCESSIBILITY_NOTIFICATION_UID);
 
    auto icon = GetIcon();
    if (icon) {
        request.SetLittleIcon(icon);
    }
 
    request.SetWantAgent(GetSettingsWantAgent());
 
    ErrCode ret = Notification::NotificationHelper::PublishNotification(request);
    HILOG_ERROR("PublishReminder ret = %{public}d", ret);
    return RET_OK;
}
 
RetError IgnoreRepeatClickNotification::RegisterTimers(uint64_t beginTime)
{
    if (!IsSendIgnoreRepeatClickNotification()) {
        HILOG_ERROR("no need notificatino");
        return RET_OK;
    }
    uint64_t nowTime = MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
    uint64_t millisecondsToMidnight = CalculateTimeToMidnight(beginTime);
    for (const auto &interval : NOTIFICATION_DATE) {
        uint64_t intervalMs = millisecondsToMidnight + TWELVE_CLOCK + interval + beginTime;
        std::shared_ptr<TimerInfo> timer = std::make_shared<TimerInfo>();
        timer->SetCallbackInfo(TimerCallback);
        timer->SetDisposable(true);
        timer->SetType(timer->TIMER_TYPE_EXACT | timer->TIMER_TYPE_WAKEUP);
        uint64_t timerId = MiscServices::TimeServiceClient::GetInstance()->CreateTimer(timer);
        if (!timerId) {
            HILOG_ERROR("createTimerFailed");
            continue;
        }
        HILOG_INFO("timeId = %{public}llu", timerId);
        MiscServices::TimeServiceClient::GetInstance()->StartTimer(timerId, intervalMs);
        timersVec.emplace_back(timerId);
    }
    return RET_OK;
}
 
uint64_t IgnoreRepeatClickNotification::CalculateTimeToMidnight(uint64_t nowTime)
{
    time_t nowSec = nowTime / 1000;
    struct tm *utcTime = gmtime((const time_t *)&nowSec);
    if (!utcTime) {
        return 0;
    }
 
    utcTime->tm_hour = 0;
    utcTime->tm_min = 0;
    utcTime->tm_sec = 0;
    utcTime->tm_mday += 1;
    time_t midnight_seconds = mktime(utcTime);
    if (midnight_seconds < 0) {
        return 0;
    }
    uint64_t midnight_millis = (uint64_t)midnight_seconds * 1000;
    if (midnight_millis > nowTime) {
        uint64_t millisecondsToMidnight = midnight_millis - nowTime;
        return millisecondsToMidnight;
    } else {
        return 0;
    }
}
 
void IgnoreRepeatClickNotification::TimerCallback()
{
    IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
    HILOG_ERROR("IgnoreRepeatClickNotification::TimerCallback");
}
 
void IgnoreRepeatClickNotification::DestoryTimers()
{
    for (const auto &timerId : timersVec) {
        MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(timerId);
    }
    timersVec.clear();
}
}  // namespace Accessibility
}  // namespace OHOS