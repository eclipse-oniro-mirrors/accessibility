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
#include "system_ability_definition.h"
#include "accessibility_constants.h"

namespace OHOS {
namespace AccessibilityConfig {
AccessibilityConfig::Impl::Impl()
{
    HILOG_DEBUG("start");
    if (!ConnectToService()) {
        HILOG_ERROR("Failed to connect to aams service");
        return;
    }
    captionObserver_ = new(std::nothrow) AccessibleAbilityManagerCaptionObserverImpl(*this);
    if (!captionObserver_) {
        HILOG_ERROR("Create captionObserver_ fail.");
        return;
    }
    serviceProxy_->RegisterCaptionObserver(captionObserver_);

    enableAbilityListsObserverStub_ = new(std::nothrow) AccessibilityEnableAbilityListsObserverStubImpl(*this);
    if (!enableAbilityListsObserverStub_) {
        HILOG_ERROR("Create enableAbilityListsObserverStub_ fail.");
        return;
    }
    serviceProxy_->RegisterEnableAbilityListsObserver(enableAbilityListsObserverStub_);

    configObserver_ = new(std::nothrow) AccessibleAbilityManagerConfigObserverImpl(*this);
    if (!configObserver_) {
        HILOG_ERROR("Create configObserver_ fail.");
        return;
    }
    serviceProxy_->RegisterConfigObserver(configObserver_);
}

bool AccessibilityConfig::Impl::ConnectToService()
{
    if (serviceProxy_) {
        HILOG_DEBUG("AAMS Service is connected");
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
    }

    HILOG_DEBUG("Get remote object ok");
    serviceProxy_ = iface_cast<Accessibility::IAccessibleAbilityManagerService>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
        return false;
    }

    return true;
}

void AccessibilityConfig::Impl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (serviceProxy_) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            HILOG_DEBUG("Reset OK");
        }
    }
}

bool AccessibilityConfig::Impl::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->EnableAbility(name, capabilities);
}

bool AccessibilityConfig::Impl::DisableAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DisableAbility(name);
}

bool AccessibilityConfig::Impl::GetCaptionState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetCaptionState();
    return true;
}

bool AccessibilityConfig::Impl::GetCaptionProperty(CaptionProperty &caption)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    caption = serviceProxy_->GetCaptionProperty();
    return true;
}

bool AccessibilityConfig::Impl::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->SetCaptionProperty(caption);
    return true;
}

bool AccessibilityConfig::Impl::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetCaptionState(state);
    return ret;
}

void AccessibilityConfig::Impl::NotifyCaptionStateChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.captionState = state;
            observer->OnConfigChanged(CONFIG_CAPTION_STATE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyCaptionChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const CaptionProperty &captionProperty)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("observers is null");
        return;
    }
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.captionStyle = captionProperty_;
            observer->OnConfigChanged(CONFIG_CAPTION_STYLE, configValue);
        } else {
            HILOG_ERROR("end observers is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::SubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver>& observer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(id);
    if (it != configObservers_.end()) {
        it->second.push_back(observer);
        HILOG_INFO("configObservers->second.size%{public}zu  configTest", it->second.size());
    } else {
        std::vector<std::shared_ptr<AccessibilityConfigObserver>> ob;
        ob.push_back(observer);
        configObservers_.insert(std::make_pair(id, ob));
        HILOG_INFO("configObservers->second.size%{public}zu  configTest", ob.size());
    }
}

void AccessibilityConfig::Impl::UnsubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver>& observer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(id);
    if (it != configObservers_.end()) {
        configObservers_.erase(it);
    } else {
        HILOG_DEBUG("%{public}d has not subscribed ", id);
    }
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty& property)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

bool AccessibilityConfig::Impl::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetScreenMagnificationState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetShortKeyState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetMouseKeyState(state);
    return ret;
}

bool AccessibilityConfig::Impl::GetScreenMagnificationState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetScreenMagnificationState();
    return true;
}

bool AccessibilityConfig::Impl::GetShortKeyState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetShortKeyState();
    return true;
}

bool AccessibilityConfig::Impl::GetMouseKeyState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetMouseKeyState();
    return true;
}


void AccessibilityConfig::Impl::UpdateCaptionEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateScreenMagnificationEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateShortKeyEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateMouseKeyEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateAudioMonoEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateAnimationOffEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateInvertColorEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::UpdateHighContrastTextEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (highContrastText_ == enabled) {
            return;
        }
        highContrastText_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_HIGH_CONTRASTE_TEXT);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyHighContrastTextChanged(observers, enabled);
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyScreenMagnificationChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.screenMagnifier = state;
            observer->OnConfigChanged(CONFIG_SCREEN_MAGNIFICATION, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyShortKeyChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey = state;
            observer->OnConfigChanged(CONFIG_SHORT_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyMouseKeyChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.mouseKey = state;
            observer->OnConfigChanged(CONFIG_MOUSE_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyInvertColorChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.invertColor = state;
            observer->OnConfigChanged(CONFIG_INVERT_COLOR, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyHighContrastTextChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.highContrastText = state;
            observer->OnConfigChanged(CONFIG_HIGH_CONTRASTE_TEXT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyAudioMonoChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.audioMono = state;
            observer->OnConfigChanged(CONFIG_AUDIO_MONO, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyAnimationOffChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.animationOff = state;
            observer->OnConfigChanged(CONFIG_ANIMATION_OFF, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilityConfig::Impl::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetMouseAutoClick(time);
    return ret;
}

bool AccessibilityConfig::Impl::SetShortkeyTarget(const std::string& name)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetShortkeyTarget(name);
    return ret;
}

bool AccessibilityConfig::Impl::GetMouseAutoClick(int32_t &time)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return 0;
    }

    time = serviceProxy_->GetMouseAutoClick();
    return true;
}

bool AccessibilityConfig::Impl::GetShortkeyTarget(std::string &name)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    name = serviceProxy_->GetShortkeyTarget();
    return true;
}

void AccessibilityConfig::Impl::NotifyShortkeyTargetChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const std::string &shortkey_target)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey_target = shortkeyTarget_;
            observer->OnConfigChanged(CONFIG_SHORT_KEY_TARGET, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyMouseAutoClickChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t mouseAutoClick)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.mouseAutoClick = mouseAutoClick_;
            observer->OnConfigChanged(CONFIG_MOUSE_AUTOCLICK, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyAudioBalanceChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const float audioBalance)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.audioBalance = audioBalance;
            observer->OnConfigChanged(CONFIG_AUDIO_BALANCE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyBrightnessDiscountChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const float brightnessDiscount)
{
    HILOG_DEBUG("start");
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.brightnessDiscount = brightnessDiscount;
            observer->OnConfigChanged(CONFIG_BRIGHTNESS_DISCOUNT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyContentTimeoutChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t contentTimeout)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.contentTimeout = contentTimeout;
            observer->OnConfigChanged(CONFIG_CONTENT_TIMEOUT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::NotifyDaltonizationColorFilterChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t daltonizationColorFilter)
{
    HILOG_DEBUG("start");
    if (!observers.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.daltonizationColorFilter = static_cast<DALTONIZATION_TYPE>(daltonizationColorFilter_);
            observer->OnConfigChanged(CONFIG_DALTONIZATION_COLOR_FILTER, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilityConfig::Impl::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetHighContrastTextState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetInvertColorState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetDaltonizationColorFilter(type);
    return ret;
}

bool AccessibilityConfig::Impl::SetContentTimeout(const uint32_t timer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetContentTimeout(timer);
    return ret;
}

bool AccessibilityConfig::Impl::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAnimationOffState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetBrightnessDiscount(const float brightness)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetBrightnessDiscount(brightness);
    return ret;
}

bool AccessibilityConfig::Impl::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAudioMonoState(state);
    return ret;
}

bool AccessibilityConfig::Impl::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAudioBalance(balance);
    return ret;
}

bool AccessibilityConfig::Impl::GetInvertColorState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetInvertColorState();
    return true;
}

bool AccessibilityConfig::Impl::GetHighContrastTextState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetHighContrastTextState();
    return true;
}

bool AccessibilityConfig::Impl::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    type = static_cast<DALTONIZATION_TYPE>(serviceProxy_->GetDaltonizationColorFilter());
    return true;
}

bool AccessibilityConfig::Impl::GetContentTimeout(uint32_t &timer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    timer = serviceProxy_->GetContentTimeout();
    return true;
}

bool AccessibilityConfig::Impl::GetAnimationOffState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetAnimationOffState();
    return true;
}

bool AccessibilityConfig::Impl::GetBrightnessDiscount(float &brightness)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    brightness = serviceProxy_->GetBrightnessDiscount();
    return true;
}

bool AccessibilityConfig::Impl::GetAudioMonoState(bool &state)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    state = serviceProxy_->GetAudioMonoState();
    return true;
}

bool AccessibilityConfig::Impl::GetAudioBalance(float &balance)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    balance = serviceProxy_->GetAudioBalance();
    return true;
}

void AccessibilityConfig::Impl::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &enableAbilityListsObserver : enableAbilityListsObservers_) {
        if (enableAbilityListsObserver == observer) {
            HILOG_ERROR("the observer is exist");
            return;
        }
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void AccessibilityConfig::Impl::UnsubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        if (*iter == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(iter);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return;
        }
    }
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

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerCongfigStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("start stateType[%{public}d}", stateType);
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
    HILOG_DEBUG("start audioBalance[%{public}f}", audioBalance);
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerBrightnessDiscountChanged(const float brightnessDiscount)
{
    HILOG_DEBUG("start brightnessDiscount[%{public}f}", brightnessDiscount);

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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerContentTimeoutChanged(const uint32_t contentTimeout)
{
    HILOG_DEBUG("start contentTimeout[%{public}d}", contentTimeout);
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerDaltonizationColorFilterChanged(const uint32_t filterType)
{
    HILOG_DEBUG("start filterType[%{public}u}", filterType);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (daltonizationColorFilter_ == filterType) {
            HILOG_DEBUG("filterType[%{public}u    configTest}", daltonizationColorFilter_);
            return;
        }
        daltonizationColorFilter_ = filterType;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER);
        if (it == configObservers_.end()) {
            HILOG_INFO("it == configObservers_.end()   configTest");
            return;
        }
        observers = it->second;
    }

    NotifyDaltonizationColorFilterChanged(observers, filterType);
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick)
{
    HILOG_DEBUG("start mouseAutoClick[%{public}d}", mouseAutoClick);
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
    HILOG_DEBUG("end");
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerShortkeyTargetChanged(const std::string &shortkeyTarget)
{
    HILOG_DEBUG("start shortkeyTarget[%{public}s}", shortkeyTarget.c_str());
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!std::strcmp(shortkeyTarget_.c_str(), shortkeyTarget.c_str())) {
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
    HILOG_DEBUG("end");
}
} // namespace AccessibilityConfig
} // namespace OHOS