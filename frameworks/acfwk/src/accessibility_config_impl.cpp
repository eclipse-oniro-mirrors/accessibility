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
static std::mutex g_Mutex;
static std::shared_ptr<AccessibilityConfigImpl> g_Instance = nullptr;
std::shared_ptr<AccessibilityConfig> AccessibilityConfig::GetInstance()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        g_Instance = std::make_shared<AccessibilityConfigImpl>();
    } else {
        HILOG_DEBUG("AccessibilityConfig had construct instance");
    }

    return g_Instance;
}

bool AccessibilityConfigImpl::ConnectToService()
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
        deathRecipient_ = new DeathRecipient(*this);
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

AccessibilityConfigImpl::AccessibilityConfigImpl()
{
    HILOG_DEBUG("start");
    if (!ConnectToService()) {
        HILOG_ERROR("Failed to connect to aams service");
        return;
    }
    captionObserver_ = new AccessibleAbilityManagerCaptionObserverImpl(*this);
    serviceProxy_->RegisterCaptionObserver(captionObserver_);

    enableAbilityListsObserverStub_ = new AccessibilityEnableAbilityListsObserverStubImpl(*this);
    serviceProxy_->RegisterEnableAbilityListsObserver(enableAbilityListsObserverStub_);
    configObserver_ = new AccessibleAbilityManagerConfigObserverImpl(*this);
    serviceProxy_->RegisterConfigObserver(configObserver_);
}

AccessibilityConfigImpl::~AccessibilityConfigImpl()
{
    HILOG_DEBUG("start");
}

void AccessibilityConfigImpl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start");
    if (serviceProxy_) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            HILOG_DEBUG("Reset OK");
        }
    }
}

bool AccessibilityConfigImpl::EnableAbilities(const std::string name, const uint32_t capabilities)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->EnableAbilities(name, capabilities);
}

bool AccessibilityConfigImpl::DisableAbilities(const std::string name)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DisableAbilities(name);
}

bool AccessibilityConfigImpl::GetCaptionState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetCaptionState();
    return true;
}

bool AccessibilityConfigImpl::GetCaptionProperty(CaptionProperty &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    value = serviceProxy_->GetCaptionProperty();
    return true;
}

bool AccessibilityConfigImpl::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    serviceProxy_->SetCaptionProperty(caption);
    return true;
}

bool AccessibilityConfigImpl::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetCaptionState(state);
    return ret;
}

void AccessibilityConfigImpl::NotifyCaptionStateChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }
    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.captionState = captionState_;
            observer->OnConfigChanged(CONFIG_CAPTION_STATE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::NotifyCaptionChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("captionStyleObservers_ is null");
        return;
    }
    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue captionStyle_;
            captionStyle_.captionStyle = captionProperty_;
            observer->OnConfigChanged(CONFIG_CAPTION_STYLE, captionStyle_);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::SubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer)
{
    HILOG_DEBUG("start");
    if (!configObservers_.size()) {
        configObservers_.push_back(observer);
    }
}

void AccessibilityConfigImpl::UnsubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer)
{
    HILOG_DEBUG("start");
    for (auto enable = configObservers_.begin(); enable != configObservers_.end(); ++enable) {
        if (*enable == observer) {
            configObservers_.erase(enable);
        }
    }
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty& property)
{
    HILOG_DEBUG("start");
    captionProperty_ = property;
    NotifyCaptionChanged();
    HILOG_DEBUG("end");
}

bool AccessibilityConfigImpl::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetScreenMagnificationState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetShortKeyState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetMouseKeyState(state);
    return ret;
}

bool AccessibilityConfigImpl::GetScreenMagnificationState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetScreenMagnificationState();
    return true;
}

bool AccessibilityConfigImpl::GetShortKeyState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetShortKeyState();
    return true;
}

bool AccessibilityConfigImpl::GetMouseKeyState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetMouseKeyState();
    return true;
}


void AccessibilityConfigImpl::UpdateCaptionEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (captionState_ != enabled) {
        captionState_ = enabled;
        NotifyCaptionStateChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::UpdateScreenMagnificationEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (screenMagnifier_ != enabled) {
        screenMagnifier_ = enabled;
        NotifyScreenMagnificationChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::UpdateShortKeyEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (shortkey_ != enabled) {
        shortkey_ = enabled;
        NotifyShortKeyChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::UpdateMouseKeyEnabled(const bool enabled)
{
    HILOG_DEBUG("start");
    if (mouseKey_ != enabled) {
        mouseKey_ = enabled;
        NotifyMouseKeyChanged();
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::NotifyScreenMagnificationChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.screenMagnifier = screenMagnifier_;
            observer->OnConfigChanged(CONFIG_SCREEN_MAGNIFICATION, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::NotifyShortKeyChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }
    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey = shortkey_;
            observer->OnConfigChanged(CONFIG_SHORT_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::NotifyMouseKeyChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.mouseKey = mouseKey_;
            observer->OnConfigChanged(CONFIG_MOUSE_KEY, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilityConfigImpl::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetMouseAutoClick(time);
    return ret;
}

bool AccessibilityConfigImpl::SetShortkeyTarget(const std::string& name)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetShortkeyTarget(name);
    return ret;
}

bool AccessibilityConfigImpl::GetMouseAutoClick(int32_t &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return 0;
    }

    value = serviceProxy_->GetMouseAutoClick();
    return true;
}

bool AccessibilityConfigImpl::GetShortkeyTarget(std::string &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetShortkeyTarget();
    return true;
}

void AccessibilityConfigImpl::NotifyShortkeyTargetChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkey_target = shortkeyTarget_;
            HILOG_ERROR("NotifyShortkeyTargetChanged configValue.shortkey_target is [%{public}s]",
                configValue.shortkey_target.c_str());
            observer->OnConfigChanged(CONFIG_SHORT_KEY_TARGET, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::NotifyMouseAutoClickChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
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

void AccessibilityConfigImpl::NotifyAudioBalanceChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.audioBalance = audioBalance_;
            observer->OnConfigChanged(CONFIG_AUDIO_BALANCE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}
void AccessibilityConfigImpl::NotifyBrightnessDiscountChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.brightnessDiscount = brightnessDiscount_;
            observer->OnConfigChanged(CONFIG_BRIGHTNESS_DISCOUNT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}
void AccessibilityConfigImpl::NotifyContentTimeoutChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.contentTimeout = contentTimeout_;
            observer->OnConfigChanged(CONFIG_CONTENT_TIMEOUT, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}
void AccessibilityConfigImpl::NotifyDaltonizationColorFilterChanged()
{
    HILOG_DEBUG("start");
    std::lock_guard<std::recursive_mutex> lock(acProxyLock_);
    if (!configObservers_.size()) {
        HILOG_DEBUG("There is no observers");
        return;
    }

    for (auto observer : configObservers_) {
        if (observer) {
            ConfigValue configValue;
            configValue.daltonizationColorFilter = (DALTONIZATION_TYPE)daltonizationColorFilter_;
            observer->OnConfigChanged(CONFIG_DALTONIZATION_COLOR_FILTER, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
    HILOG_DEBUG("end");
}

bool AccessibilityConfigImpl::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetHighContrastTextState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetInvertColorState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetDaltonizationColorFilter(type);
    return ret;
}

bool AccessibilityConfigImpl::SetContentTimeout(const uint32_t timer)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetContentTimeout(timer);
    return ret;
}

bool AccessibilityConfigImpl::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAnimationOffState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetBrightnessDiscount(const float brightness)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetBrightnessDiscount(brightness);
    return ret;
}

bool AccessibilityConfigImpl::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAudioMonoState(state);
    return ret;
}

bool AccessibilityConfigImpl::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start");
    bool ret = false;
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    ret = serviceProxy_->SetAudioBalance(balance);
    return ret;
}

bool AccessibilityConfigImpl::GetInvertColorState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetInvertColorState();
    return true;
}

bool AccessibilityConfigImpl::GetHighContrastTextState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetHighContrastTextState();
    return true;
}

bool AccessibilityConfigImpl::GetDaltonizationColorFilter(DALTONIZATION_TYPE &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = (DALTONIZATION_TYPE)serviceProxy_->GetDaltonizationColorFilter();
    return true;
}

bool AccessibilityConfigImpl::GetContentTimeout(uint32_t &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetContentTimeout();
    return true;
}

bool AccessibilityConfigImpl::GetAnimationOffState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetAnimationOffState();
    return true;
}

bool AccessibilityConfigImpl::GetBrightnessDiscount(float &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetBrightnessDiscount();
    return true;
}

bool AccessibilityConfigImpl::GetAudioMonoState(bool &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetAudioMonoState();
    return true;
}

bool AccessibilityConfigImpl::GetAudioBalance(float &value)
{
    HILOG_DEBUG("start");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }

    value = serviceProxy_->GetAudioBalance();
    return true;
}

void AccessibilityConfigImpl::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    for (auto &enableAbilityListsObserver : enableAbilityListsObservers_) {
        if (enableAbilityListsObserver == observer) {
            HILOG_ERROR("the observer is exist");
            return;
        }
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void AccessibilityConfigImpl::UnsubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_DEBUG("start");
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        if (*iter == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(iter);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return;
        }
    }
}

void AccessibilityConfigImpl::OnAccessibilityEnableAbilityListsChanged()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &enableAbilityListsObserver : enableAbilityListsObservers_) {
        enableAbilityListsObserver->OnEnableAbilityListsStateChanged();
    }
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerCongfigStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("stateType[%{public}d}", stateType);
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

    if (stateType & Accessibility::STATE_MOUSEKEY_ENABLED) {
        UpdateMouseKeyEnabled(true);
    } else {
        UpdateMouseKeyEnabled(false);
    }
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerAudioBalanceChanged(const float audioBalance)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("audioBalance[%{public}f}", audioBalance);
    audioBalance_ = audioBalance;
    NotifyAudioBalanceChanged();
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerBrightnessDiscountChanged(const float brightnessDiscount)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("brightnessDiscount[%{public}f}", brightnessDiscount);
    brightnessDiscount_ = brightnessDiscount;
    NotifyBrightnessDiscountChanged();
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerContentTimeoutChanged(const uint32_t contentTimeout)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("contentTimeout[%{public}d}", contentTimeout);
    contentTimeout_ = contentTimeout;
    NotifyContentTimeoutChanged();
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerDaltonizationColorFilterChanged(
    const DALTONIZATION_TYPE filterType)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("filterType[%{public}d}", (uint32_t)filterType);
    daltonizationColorFilter_ = filterType;
    NotifyDaltonizationColorFilterChanged();
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("mouseAutoClick[%{public}d}", mouseAutoClick);
    mouseAutoClick_ = mouseAutoClick;
    NotifyMouseAutoClickChanged();
    HILOG_DEBUG("end");
}

void AccessibilityConfigImpl::OnAccessibleAbilityManagerShortkeyTargetChanged(const std::string &shortkeyTarget)
{
    HILOG_DEBUG("start");
    HILOG_DEBUG("shortkeyTarget[%{public}s}", shortkeyTarget.c_str());
    shortkeyTarget_ = shortkeyTarget;
    NotifyShortkeyTargetChanged();
    HILOG_DEBUG("end");
}
} // namespace AccessibilityConfig
} // namespace OHOS