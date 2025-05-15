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