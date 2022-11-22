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

#include "accessibility_config_impl.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AccessibilityConfig {
namespace {
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    constexpr int32_t CONFIG_PARAMETER_VALUE_SIZE = 10;
} // namespace

AccessibilityConfig::Impl::Impl()
{}

bool AccessibilityConfig::Impl::InitializeContext()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (isInitialized_) {
        HILOG_DEBUG("Context has initialized");
        return true;
    }
    isInitialized_ = ConnectToService();

    return isInitialized_;
}

void AccessibilityConfig::Impl::OnParameterChanged(const char *key, const char *value, void *context)
{
    HILOG_DEBUG("Parameter key = [%{public}s] value = [%{public}s]", key, value);

    if (!key || std::strcmp(key, SYSTEM_PARAMETER_AAMS_NAME.c_str())) {
        HILOG_WARN("not accessibility.config.ready callback");
        return;
    }

    if (!value || std::strcmp(value, "true")) {
        HILOG_WARN("accessibility.config.ready value not true");
        return;
    }

    if (!context) {
        HILOG_ERROR("accessibility.config.ready context NULL");
        return;
    }

    Impl* implPtr = static_cast<Impl*>(context);
    (void)implPtr->ConnectToServiceAsync();
}

bool AccessibilityConfig::Impl::ConnectToService()
{
    char value[CONFIG_PARAMETER_VALUE_SIZE] = "default";
    int retSysParam = GetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false", value, CONFIG_PARAMETER_VALUE_SIZE);
    if (retSysParam >= 0 && !std::strcmp(value, "true")) {
        // Accessibility service is ready
        if (!InitAccessibilityServiceProxy()) {
            return false;
        }

        if (!RegisterToService()) {
            return false;
        }

        InitConfigValues();
    } else {
        HILOG_DEBUG("Start watching accessibility service.");
        InitEventHandler();
        retSysParam = WatchParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), &OnParameterChanged, this);
        if (retSysParam) {
            HILOG_ERROR("Watch parameter failed, error = %{public}d", retSysParam);
            return false;
        }
    }

    return true;
}

bool AccessibilityConfig::Impl::ConnectToServiceAsync()
{
    if (handler_) {
        handler_->PostTask(std::bind([this]() {
            HILOG_DEBUG("ConnectToServiceAsync start.");
            std::lock_guard<std::mutex> lock(mutex_);
            if (InitAccessibilityServiceProxy()) {
                (void)RegisterToService();
                InitConfigValues();
                HILOG_DEBUG("ConnectToService Success");
            }
            }), "ConnectToServiceAsync");
        return true;
    } else {
        HILOG_ERROR("Event handler is nullptr");
        return false;
    }
}

bool AccessibilityConfig::Impl::RegisterToService()
{
    if (!serviceProxy_) {
        HILOG_ERROR("Service is not connected");
        return false;
    }

    if (captionObserver_ && enableAbilityListsObserver_ && configObserver_) {
        HILOG_DEBUG("Observers is registered");
        return true;
    }

    if (!captionObserver_) {
        captionObserver_ = new(std::nothrow) AccessibleAbilityManagerCaptionObserverImpl(*this);
        if (!captionObserver_) {
            HILOG_ERROR("Create captionObserver_ failed.");
            return false;
        }
        serviceProxy_->RegisterCaptionObserver(captionObserver_);
    }

    if (!enableAbilityListsObserver_) {
        enableAbilityListsObserver_ = new(std::nothrow) AccessibilityEnableAbilityListsObserverImpl(*this);
        if (!enableAbilityListsObserver_) {
            HILOG_ERROR("Create enableAbilityListsObserver_ failed.");
            return false;
        }
        serviceProxy_->RegisterEnableAbilityListsObserver(enableAbilityListsObserver_);
    }

    if (!configObserver_) {
        configObserver_ = new(std::nothrow) AccessibleAbilityManagerConfigObserverImpl(*this);
        if (!configObserver_) {
            HILOG_ERROR("Create configObserver_ failed.");
            return false;
        }
        serviceProxy_->RegisterConfigObserver(configObserver_);
    }

    HILOG_DEBUG("RegisterToService succeaddss");
    return true;
}

bool AccessibilityConfig::Impl::InitAccessibilityServiceProxy()
{
    if (serviceProxy_) {
        HILOG_DEBUG("Accessibility Service is connected");
        return true;
    }

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return false;
    }

    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
        if (!deathRecipient_) {
            HILOG_ERROR("Create deathRecipient_ fail.");
            return false;
        }
    }

    if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
        HILOG_ERROR("Failed to add death recipient");
        return false;
    }

    serviceProxy_ = iface_cast<Accessibility::IAccessibleAbilityManagerService>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }

    HILOG_DEBUG("InitAccessibilityServiceProxy success");
    return true;
}

void AccessibilityConfig::Impl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (serviceProxy_) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            captionObserver_ = nullptr;
            enableAbilityListsObserver_ = nullptr;
            configObserver_ = nullptr;
            HILOG_DEBUG("Reset OK");
        }
    }
}

Accessibility::RetError AccessibilityConfig::Impl::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_INFO("name = [%{private}s] capabilities = [%{private}u]", name.c_str(), capabilities);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->EnableAbility(name, capabilities);
}

Accessibility::RetError AccessibilityConfig::Impl::DisableAbility(const std::string &name)
{
    HILOG_INFO("name = [%{private}s]", name.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->DisableAbility(name);
}

Accessibility::RetError AccessibilityConfig::Impl::GetCaptionsState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = serviceProxy_->GetCaptionState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetCaptionsProperty(CaptionProperty &caption)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = serviceProxy_->GetCaptionProperty(caption);
    HILOG_INFO();
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetCaptionsProperty(const CaptionProperty& caption)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetCaptionProperty(caption);
}

Accessibility::RetError AccessibilityConfig::Impl::SetCaptionsState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetCaptionState(state);
}

void AccessibilityConfig::Impl::NotifyCaptionStateChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.captionState = state;
            observer->OnConfigChanged(CONFIG_CAPTION_STATE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyCaptionChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const CaptionProperty &captionProperty)
{
    HILOG_INFO();
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.captionStyle = captionProperty_;
            observer->OnConfigChanged(CONFIG_CAPTION_STYLE, configValue);
        } else {
            HILOG_ERROR("end observers is null");
        }
    }
}

Accessibility::RetError AccessibilityConfig::Impl::SubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer, const bool retFlag)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(id));
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(id);
    if (it != configObservers_.end()) {
        it->second.push_back(observer);
        HILOG_DEBUG("configObservers->second.size%{public}zu", it->second.size());
    } else {
        std::vector<std::shared_ptr<AccessibilityConfigObserver>> ob;
        ob.push_back(observer);
        configObservers_.insert(std::make_pair(id, ob));
        HILOG_DEBUG("configObservers->second.size%{public}zu", ob.size());
    }

    if (retFlag && observer) {
        NotifyImmediately(id, observer);
    }
    return Accessibility::RET_OK;
}

Accessibility::RetError AccessibilityConfig::Impl::UnsubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(id));
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(id);
    if (it != configObservers_.end()) {
        for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
            if (*iter == observer) {
                HILOG_DEBUG("erase observer");
                it->second.erase(iter);
                HILOG_DEBUG("observer's size is %{public}zu", it->second.size());
                return Accessibility::RET_OK;
            }
        }
    } else {
        HILOG_DEBUG("%{public}d has not subscribed ", id);
    }
    return Accessibility::RET_OK;
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty& property)
{
    HILOG_DEBUG();
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (captionProperty_.GetFontScale() == property.GetFontScale() &&
            captionProperty_.GetFontColor() == property.GetFontColor() &&
            !strcmp(captionProperty_.GetFontFamily().c_str(), property.GetFontFamily().c_str()) &&
            !strcmp(captionProperty_.GetFontEdgeType().c_str(), property.GetFontEdgeType().c_str()) &&
            captionProperty_.GetBackgroundColor() == property.GetBackgroundColor() &&
            captionProperty_.GetWindowColor() == property.GetWindowColor()) {
            return;
        }
        captionProperty_ = property;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_CAPTION_STYLE);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyCaptionChanged(observers, property);
}

Accessibility::RetError AccessibilityConfig::Impl::SetScreenMagnificationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetScreenMagnificationState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetShortKeyState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetMouseKeyState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::GetScreenMagnificationState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetScreenMagnificationState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetShortKeyState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetShortKeyState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetMouseKeyState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetMouseKeyState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

void AccessibilityConfig::Impl::UpdateCaptionEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (captionState_ == enabled) {
            return;
        }
        captionState_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_CAPTION_STATE);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyCaptionStateChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateScreenMagnificationEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (screenMagnifier_ == enabled) {
            return;
        }
        screenMagnifier_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_SCREEN_MAGNIFICATION);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyScreenMagnificationChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateShortKeyEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shortkey_ == enabled) {
            return;
        }
        shortkey_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_SHORT_KEY);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyShortKeyChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateMouseKeyEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (mouseKey_ == enabled) {
            return;
        }
        mouseKey_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_MOUSE_KEY);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyMouseKeyChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateAudioMonoEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (audioMono_ == enabled) {
            return;
        }
        audioMono_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_AUDIO_MONO);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyAudioMonoChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateAnimationOffEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (animationOff_ == enabled) {
            return;
        }
        animationOff_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_ANIMATION_OFF);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyAnimationOffChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateInvertColorEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (invertColor_ == enabled) {
            return;
        }
        invertColor_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_INVERT_COLOR);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyInvertColorChanged(observers, enabled);
}

void AccessibilityConfig::Impl::UpdateHighContrastTextEnabled(const bool enabled)
{
    HILOG_INFO("enabled = [%{public}s]", enabled ? "True" : "False");
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (highContrastText_ == enabled) {
            return;
        }
        highContrastText_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_HIGH_CONTRAST_TEXT);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyHighContrastTextChanged(observers, enabled);
}

void AccessibilityConfig::Impl::NotifyScreenMagnificationChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.screenMagnifier = state;
            observer->OnConfigChanged(CONFIG_SCREEN_MAGNIFICATION, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyShortKeyChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey = state;
            observer->OnConfigChanged(CONFIG_SHORT_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyMouseKeyChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.mouseKey = state;
            observer->OnConfigChanged(CONFIG_MOUSE_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyInvertColorChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.invertColor = state;
            observer->OnConfigChanged(CONFIG_INVERT_COLOR, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyHighContrastTextChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.highContrastText = state;
            observer->OnConfigChanged(CONFIG_HIGH_CONTRAST_TEXT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyAudioMonoChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.audioMono = state;
            observer->OnConfigChanged(CONFIG_AUDIO_MONO, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyAnimationOffChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.animationOff = state;
            observer->OnConfigChanged(CONFIG_ANIMATION_OFF, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

Accessibility::RetError AccessibilityConfig::Impl::SetMouseAutoClick(const int32_t time)
{
    HILOG_INFO("time = [%{public}d]", time);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetMouseAutoClick(time);
}

Accessibility::RetError AccessibilityConfig::Impl::SetShortkeyTarget(const std::string& name)
{
    HILOG_INFO("name = [%{public}s]", name.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetShortkeyTarget(name);
}

Accessibility::RetError AccessibilityConfig::Impl::GetMouseAutoClick(int32_t &time)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetMouseAutoClick(time);
    HILOG_INFO("time = [%{public}d]", time);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetShortkeyTarget(std::string &name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetShortkeyTarget(name);
    HILOG_INFO("name = [%{public}s]", name.c_str());
    return ret;
}

void AccessibilityConfig::Impl::NotifyShortkeyTargetChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const std::string &shortkey_target)
{
    HILOG_INFO("shortkey_target = [%{public}s]", shortkey_target.c_str());
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey_target = shortkeyTarget_;
            observer->OnConfigChanged(CONFIG_SHORT_KEY_TARGET, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyMouseAutoClickChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t mouseAutoClick)
{
    HILOG_INFO("mouseAutoClick = [%{public}u]", mouseAutoClick);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.mouseAutoClick = mouseAutoClick_;
            observer->OnConfigChanged(CONFIG_MOUSE_AUTOCLICK, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyAudioBalanceChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const float audioBalance)
{
    HILOG_INFO("audioBalance = [%{public}f]", audioBalance);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.audioBalance = audioBalance;
            observer->OnConfigChanged(CONFIG_AUDIO_BALANCE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyBrightnessDiscountChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const float brightnessDiscount)
{
    HILOG_INFO("brightnessDiscount = [%{public}f]", brightnessDiscount);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.brightnessDiscount = brightnessDiscount;
            observer->OnConfigChanged(CONFIG_BRIGHTNESS_DISCOUNT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyContentTimeoutChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t contentTimeout)
{
    HILOG_INFO("contentTimeout = [%{public}u]", contentTimeout);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.contentTimeout = contentTimeout;
            observer->OnConfigChanged(CONFIG_CONTENT_TIMEOUT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyDaltonizationColorFilterChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t daltonizationColorFilter)
{
    HILOG_INFO("daltonizationColorFilter = [%{public}u]", daltonizationColorFilter);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.daltonizationColorFilter = static_cast<DALTONIZATION_TYPE>(daltonizationColorFilter_);
            observer->OnConfigChanged(CONFIG_DALTONIZATION_COLOR_FILTER, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

Accessibility::RetError AccessibilityConfig::Impl::SetHighContrastTextState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetHighContrastTextState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetInvertColorState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    HILOG_INFO("type = [%{public}u]", static_cast<uint32_t>(type));
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetDaltonizationColorFilter(type);
}

Accessibility::RetError AccessibilityConfig::Impl::SetContentTimeout(const uint32_t timer)
{
    HILOG_INFO("timer = [%{public}u]", timer);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetContentTimeout(timer);
}

Accessibility::RetError AccessibilityConfig::Impl::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetAnimationOffState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetBrightnessDiscount(const float brightness)
{
    HILOG_INFO("brightness = [%{public}f]", brightness);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetBrightnessDiscount(brightness);
}

Accessibility::RetError AccessibilityConfig::Impl::SetAudioMonoState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetAudioMonoState(state);
}

Accessibility::RetError AccessibilityConfig::Impl::SetAudioBalance(const float balance)
{
    HILOG_INFO("balance = [%{public}f]", balance);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    return serviceProxy_->SetAudioBalance(balance);
}

Accessibility::RetError AccessibilityConfig::Impl::GetInvertColorState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetInvertColorState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetHighContrastTextState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetHighContrastTextState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    uint32_t filterType = 0;
    Accessibility::RetError ret = serviceProxy_->GetDaltonizationColorFilter(filterType);
    type = static_cast<DALTONIZATION_TYPE>(filterType);
    HILOG_INFO("type = [%{public}u]", static_cast<uint32_t>(type));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetContentTimeout(uint32_t &timer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetContentTimeout(timer);
    HILOG_INFO("timer = [%{public}u]", timer);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAnimationOffState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetAnimationOffState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetBrightnessDiscount(float &brightness)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetBrightnessDiscount(brightness);
    HILOG_INFO("brightness = [%{public}f]", brightness);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAudioMonoState(bool &state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetAudioMonoState(state);
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAudioBalance(float &balance)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = serviceProxy_->GetAudioBalance(balance);
    HILOG_INFO("balance = [%{public}f]", balance);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::any_of(enableAbilityListsObservers_.begin(), enableAbilityListsObservers_.end(),
        [&observer](const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &listObserver) {
            return listObserver == observer;
            })) {
        HILOG_ERROR("the observer is exist");
        return Accessibility::RET_OK;
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    return Accessibility::RET_OK;
}

Accessibility::RetError AccessibilityConfig::Impl::UnsubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_INFO();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        if (*iter == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(iter);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return Accessibility::RET_OK;
        }
    }
    return Accessibility::RET_OK;
}

void AccessibilityConfig::Impl::OnAccessibilityEnableAbilityListsChanged()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    std::vector<std::shared_ptr<AccessibilityEnableAbilityListsObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        observers = enableAbilityListsObservers_;
    }
    for (auto &enableAbilityListsObserver : observers) {
        enableAbilityListsObserver->OnEnableAbilityListsStateChanged();
    }
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerConfigStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("stateType = [%{public}u}", stateType);
    if (stateType & Accessibility::STATE_CAPTION_ENABLED) {
        UpdateCaptionEnabled(true);
    } else {
        UpdateCaptionEnabled(false);
    }

    if (stateType & Accessibility::STATE_SCREENMAGNIFIER_ENABLED) {
        UpdateScreenMagnificationEnabled(true);
    } else {
        UpdateScreenMagnificationEnabled(false);
    }

    if (stateType & Accessibility::STATE_SHORTKEY_ENABLED) {
        UpdateShortKeyEnabled(true);
    } else {
        UpdateShortKeyEnabled(false);
    }

    if (stateType & Accessibility::STATE_AUDIOMONO_ENABLED) {
        UpdateAudioMonoEnabled(true);
    } else {
        UpdateAudioMonoEnabled(false);
    }

    if (stateType & Accessibility::STATE_ANIMATIONOFF_ENABLED) {
        UpdateAnimationOffEnabled(true);
    } else {
        UpdateAnimationOffEnabled(false);
    }

    if (stateType & Accessibility::STATE_INVETRTCOLOR_ENABLED) {
        UpdateInvertColorEnabled(true);
    } else {
        UpdateInvertColorEnabled(false);
    }

    if (stateType & Accessibility::STATE_HIGHCONTRAST_ENABLED) {
        UpdateHighContrastTextEnabled(true);
    } else {
        UpdateHighContrastTextEnabled(false);
    }

    if (stateType & Accessibility::STATE_MOUSEKEY_ENABLED) {
        UpdateMouseKeyEnabled(true);
    } else {
        UpdateMouseKeyEnabled(false);
    }
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerAudioBalanceChanged(const float audioBalance)
{
    HILOG_DEBUG("audioBalance = [%{public}f}", audioBalance);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (audioBalance_ == audioBalance) {
            return;
        }
        audioBalance_ = audioBalance;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_AUDIO_BALANCE);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyAudioBalanceChanged(observers, audioBalance);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerBrightnessDiscountChanged(const float brightnessDiscount)
{
    HILOG_DEBUG("brightnessDiscount = [%{public}f}", brightnessDiscount);

    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (brightnessDiscount_ == brightnessDiscount) {
            return;
        }
        brightnessDiscount_ = brightnessDiscount;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_BRIGHTNESS_DISCOUNT);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyBrightnessDiscountChanged(observers, brightnessDiscount);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerContentTimeoutChanged(const uint32_t contentTimeout)
{
    HILOG_DEBUG("contentTimeout = [%{public}u}", contentTimeout);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (contentTimeout_ == contentTimeout) {
            return;
        }
        contentTimeout_ = contentTimeout;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_CONTENT_TIMEOUT);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyContentTimeoutChanged(observers, contentTimeout);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerDaltonizationColorFilterChanged(const uint32_t filterType)
{
    HILOG_DEBUG("filterType = [%{public}u}", filterType);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (daltonizationColorFilter_ == filterType) {
            HILOG_DEBUG("filterType[%{public}u]", daltonizationColorFilter_);
            return;
        }
        daltonizationColorFilter_ = filterType;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyDaltonizationColorFilterChanged(observers, filterType);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick)
{
    HILOG_DEBUG("mouseAutoClick = [%{public}d}", mouseAutoClick);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (mouseAutoClick_ == mouseAutoClick) {
            return;
        }
        mouseAutoClick_ = mouseAutoClick;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_MOUSE_AUTOCLICK);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyMouseAutoClickChanged(observers, mouseAutoClick);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerShortkeyTargetChanged(const std::string &shortkeyTarget)
{
    HILOG_DEBUG("shortkeyTarget = [%{public}s}", shortkeyTarget.c_str());
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shortkeyTarget_.length() > 0 && shortkeyTarget.length() > 0 &&
            !std::strcmp(shortkeyTarget_.c_str(), shortkeyTarget.c_str())) {
            return;
        }
        shortkeyTarget_ = shortkeyTarget;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_SHORT_KEY_TARGET);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyShortkeyTargetChanged(observers, shortkeyTarget);
}

void AccessibilityConfig::Impl::NotifyImmediately(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    if (handler_) {
        handler_->PostTask(std::bind([this, id, observer]() {
            HILOG_DEBUG("NotifyImmediately start.");
            ConfigValue configValue;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                configValue.highContrastText = highContrastText_;
                configValue.invertColor = invertColor_;
                configValue.animationOff = animationOff_;
                configValue.screenMagnifier = screenMagnifier_;
                configValue.audioMono = audioMono_;
                configValue.mouseKey = mouseKey_;
                configValue.shortkey = shortkey_;
                configValue.captionState = captionState_;
                configValue.contentTimeout = contentTimeout_;
                configValue.mouseAutoClick = mouseAutoClick_;
                configValue.audioBalance = audioBalance_;
                configValue.brightnessDiscount = brightnessDiscount_;
                configValue.daltonizationColorFilter = static_cast<DALTONIZATION_TYPE>(daltonizationColorFilter_);
                configValue.shortkey_target = shortkeyTarget_;
                configValue.captionStyle = captionProperty_;
            }
            observer->OnConfigChanged(id, configValue);
            }), "NotifyImmediately");
    } else {
        HILOG_ERROR("Event handler is nullptr");
    }
}

void AccessibilityConfig::Impl::InitConfigValues()
{
    Accessibility::AccessibilityConfigData configData;
    serviceProxy_->GetAllConfigs(configData);
    highContrastText_ = configData.highContrastText_;
    invertColor_ = configData.invertColor_;
    animationOff_ = configData.animationOff_;
    audioMono_ = configData.audioMono_;
    mouseKey_ = configData.mouseKey_;
    captionState_ = configData.captionState_;
    screenMagnifier_ = configData.screenMagnifier_;
    shortkey_ = configData.shortkey_;
    mouseAutoClick_ = configData.mouseAutoClick_;
    daltonizationColorFilter_ = configData.daltonizationColorFilter_;
    contentTimeout_ = configData.contentTimeout_;
    brightnessDiscount_ = configData.brightnessDiscount_;
    audioBalance_ = configData.audioBalance_;
    shortkeyTarget_ = configData.shortkeyTarget_;
    captionProperty_ = configData.captionProperty_;
    if (isInitialized_) {
        NotifyDefaultConfigs();
    }
    HILOG_DEBUG("ConnectToService Success");
}

void AccessibilityConfig::Impl::NotifyDefaultConfigs()
{
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(CONFIG_HIGH_CONTRAST_TEXT);
    if (it != configObservers_.end()) {
        NotifyHighContrastTextChanged(it->second, highContrastText_);
    }
    if ((it = configObservers_.find(CONFIG_INVERT_COLOR)) != configObservers_.end()) {
        NotifyInvertColorChanged(it->second, invertColor_);
    }
    if ((it = configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER)) != configObservers_.end()) {
        NotifyDaltonizationColorFilterChanged(it->second, daltonizationColorFilter_);
    }
    if ((it = configObservers_.find(CONFIG_CONTENT_TIMEOUT)) != configObservers_.end()) {
        NotifyContentTimeoutChanged(it->second, contentTimeout_);
    }
    if ((it = configObservers_.find(CONFIG_ANIMATION_OFF)) != configObservers_.end()) {
        NotifyAnimationOffChanged(it->second, animationOff_);
    }
    if ((it = configObservers_.find(CONFIG_BRIGHTNESS_DISCOUNT)) != configObservers_.end()) {
        NotifyBrightnessDiscountChanged(it->second, brightnessDiscount_);
    }
    if ((it = configObservers_.find(CONFIG_AUDIO_MONO)) != configObservers_.end()) {
        NotifyAudioMonoChanged(it->second, audioMono_);
    }
    if ((it = configObservers_.find(CONFIG_AUDIO_BALANCE)) != configObservers_.end()) {
        NotifyAudioBalanceChanged(it->second, audioBalance_);
    }
    if ((it = configObservers_.find(CONFIG_MOUSE_KEY)) != configObservers_.end()) {
        NotifyMouseKeyChanged(it->second, mouseKey_);
    }
    if ((it = configObservers_.find(CONFIG_SHORT_KEY)) != configObservers_.end()) {
        NotifyShortKeyChanged(it->second, shortkey_);
    }
    if ((it = configObservers_.find(CONFIG_CAPTION_STATE)) != configObservers_.end()) {
        NotifyCaptionStateChanged(it->second, captionState_);
    }
    if ((it = configObservers_.find(CONFIG_CAPTION_STYLE)) != configObservers_.end()) {
        NotifyCaptionChanged(it->second, captionProperty_);
    }
    if ((it = configObservers_.find(CONFIG_SCREEN_MAGNIFICATION)) != configObservers_.end()) {
        NotifyScreenMagnificationChanged(it->second, screenMagnifier_);
    }
    if ((it = configObservers_.find(CONFIG_SHORT_KEY_TARGET)) != configObservers_.end()) {
        NotifyShortkeyTargetChanged(it->second, shortkeyTarget_);
    }
    if ((it = configObservers_.find(CONFIG_MOUSE_AUTOCLICK)) != configObservers_.end()) {
        NotifyMouseAutoClickChanged(it->second, mouseAutoClick_);
    }
}

void AccessibilityConfig::Impl::InitEventHandler()
{
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create("Accessibility.Config");
        if (!runner_) {
            HILOG_ERROR("AccessibilityConfig::Impl::InitEventHandler create runner failed");
            return;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
        if (!handler_) {
            HILOG_ERROR("AccessibilityConfig::Impl::InitEventHandler create event handler failed");
            return;
        }
    }
}
} // namespace AccessibilityConfig
} // namespace OHOS