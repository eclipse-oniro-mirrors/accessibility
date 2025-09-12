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
    constexpr int32_t SA_CONNECT_TIMEOUT = 6 * 1000; // ms
    constexpr uint32_t DISPLAY_DALTONIZER_GREEN = 12;
    constexpr uint32_t DISPLAY_DALTONIZER_RED = 11;
    constexpr uint32_t DISPLAY_DALTONIZER_BLUE = 13;
    constexpr int32_t REPORTER_THRESHOLD_VALUE = 10;
}

AccessibilityConfig::Impl::Impl()
{}

AccessibilityConfig::Impl::~Impl()
{
    if (captionObserver_ != nullptr) {
        captionObserver_->OnclientDeleted();
    }

    if (configObserver_ != nullptr) {
        configObserver_->OnclientDeleted();
    }
    
    if (enableAbilityListsObserver_ != nullptr) {
        enableAbilityListsObserver_->OnclientDeleted();
    }
}

bool AccessibilityConfig::Impl::InitializeContext()
{
    HILOG_DEBUG();
    Utils::UniqueWriteGuard<Utils::RWLock> wLock(rwLock_);
    if (isInitialized_) {
        HILOG_DEBUG("Context has initialized");
        return true;
    }
    isInitialized_ = ConnectToService();
    return isInitialized_;
}

void AccessibilityConfig::Impl::UnInitializeContext()
{
    HILOG_DEBUG();
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Context UnInitializeContext failed");
        return;
    }
    sptr<IRemoteObject> object = serviceProxy_->AsObject();
    ResetService(object);
}

void AccessibilityConfig::Impl::OnParameterChanged(const char *key, const char *value, void *context)
{
    if (key == nullptr || std::strcmp(key, SYSTEM_PARAMETER_AAMS_NAME.c_str())) {
        return;
    }
    if (value == nullptr || std::strcmp(value, "true")) {
        return;
    }
    if (context == nullptr) {
        return;
    }
    Impl* implPtr = static_cast<Impl*>(context);
    if (implPtr->ConnectToServiceAsync() == true) {
        HILOG_INFO("ConnectToServiceAsync success.");
    } else {
        HILOG_ERROR("ConnectToServiceAsync failed.");
    }
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
    HILOG_DEBUG("ConnectToServiceAsync start.");
    Utils::UniqueWriteGuard<Utils::RWLock> wLock(rwLock_);
    if (InitAccessibilityServiceProxy()) {
        (void)RegisterToService();
        InitConfigValues();
        isInitialized_ = true;
        return true;
    } else {
        HILOG_ERROR("ConnectToServiceAsync fail");
        return false;
    }
}

bool AccessibilityConfig::Impl::InitAccessibilityServiceProxy()
{
    HILOG_DEBUG();
    if (serviceProxy_ != nullptr) {
        return true;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return false;
    }
    auto object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (object != nullptr) {
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
            if (deathRecipient_ == nullptr) {
                HILOG_ERROR("create deathRecipient_ fail");
                return false;
            }
        }

        if (object->IsProxyObject() && !object->AddDeathRecipient(deathRecipient_)) {
            HILOG_ERROR("Failed to add death recipient");
            return false;
        }

        serviceProxy_ = iface_cast<Accessibility::IAccessibleAbilityManagerService>(object);
        if (serviceProxy_ == nullptr) {
            HILOG_ERROR("IAccessibleAbilityManagerService iface_cast failed");
            return false;
        }
        HILOG_DEBUG("InitAccessibilityServiceProxy success");
        return true;
    } else {
        if (LoadAccessibilityService() == false) {
            HILOG_ERROR("LoadSystemAbilityService failed.");
            return false;
        } else {
            return true;
        }
    }
    return true;
}

bool AccessibilityConfig::Impl::LoadAccessibilityService()
{
    std::unique_lock<ffrt::mutex> lock(conVarMutex_);
    sptr<AccessibilityLoadCallback> loadCallback = new AccessibilityLoadCallback(this);
    if (loadCallback == nullptr) {
        return false;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return false;
    }
    int32_t ret = samgr->LoadSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID, loadCallback);
    if (ret != 0) {
        return false;
    }
    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(SA_CONNECT_TIMEOUT),
        [this]() { return serviceProxy_ != nullptr; });
    if (!waitStatus) {
        return false;
    }
    (void)RegisterToService();
    InitConfigValues();
    return true;
}

void AccessibilityConfig::Impl::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<ffrt::mutex> lock(conVarMutex_);
    char value[CONFIG_PARAMETER_VALUE_SIZE] = "default";
    int retSysParam = GetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false", value, CONFIG_PARAMETER_VALUE_SIZE);
    if (retSysParam >= 0 && std::strcmp(value, "true")) {
        do {
            auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (samgr == nullptr) {
                break;
            }
            auto object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
            if (object == nullptr) {
                break;
            }
            deathRecipient_ = new(std::nothrow) DeathRecipient(*this);
            if (deathRecipient_ == nullptr) {
                break;
            }
            if (object->IsProxyObject()) {
                object->AddDeathRecipient(deathRecipient_);
            }
            serviceProxy_ = iface_cast<Accessibility::IAccessibleAbilityManagerService>(object);
        } while (0);
    }
    proxyConVar_.notify_one();
}

void AccessibilityConfig::Impl::LoadSystemAbilityFail()
{
    std::lock_guard<ffrt::mutex> lock(conVarMutex_);
    HILOG_WARN("LoadSystemAbilityFail.");
    proxyConVar_.notify_one();
}

bool AccessibilityConfig::Impl::RegisterToService()
{
    if (serviceProxy_ == nullptr) {
        HILOG_ERROR("Service is not connected");
        return false;
    }

    if (captionObserver_ && enableAbilityListsObserver_ && configObserver_) {
        HILOG_DEBUG("Observers is registered");
        return true;
    }

    if (captionObserver_ == nullptr) {
        captionObserver_ = new(std::nothrow) AccessibleAbilityManagerCaptionObserverImpl(*this);
        if (captionObserver_ == nullptr) {
            HILOG_ERROR("Create captionObserver_ failed.");
            return false;
        }
        ErrCode ret = serviceProxy_->RegisterCaptionObserver(captionObserver_);
        if (ret != ERR_OK) {
            captionObserver_ = nullptr;
            HILOG_ERROR("Register captionObserver failed.");
            return false;
        }
    }

    if (!enableAbilityListsObserver_) {
        enableAbilityListsObserver_ = new(std::nothrow) AccessibilityEnableAbilityListsObserverImpl(*this);
        if (enableAbilityListsObserver_ == nullptr) {
            HILOG_ERROR("Create enableAbilityListsObserver_ failed.");
            return false;
        }
        serviceProxy_->RegisterEnableAbilityListsObserver(enableAbilityListsObserver_);
    }

    if (!configObserver_) {
        configObserver_ = new(std::nothrow) AccessibleAbilityManagerConfigObserverImpl(*this);
        if (configObserver_ == nullptr) {
            HILOG_ERROR("Create configObserver_ failed.");
            return false;
        }
        ErrCode ret = serviceProxy_->RegisterConfigObserver(configObserver_);
        if (ret != ERR_OK) {
            configObserver_ = nullptr;
            HILOG_ERROR("Register configObserver failed.");
            return false;
        }
    }

    HILOG_DEBUG("RegisterToService succeaddss");
    return true;
}

sptr<Accessibility::IAccessibleAbilityManagerService> AccessibilityConfig::Impl::GetServiceProxy()
{
    if (serviceProxy_ == nullptr) {
        LoadAccessibilityService();
    }
    return serviceProxy_;
}

void AccessibilityConfig::Impl::ResetService(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    Utils::UniqueWriteGuard<Utils::RWLock> wLock(rwLock_);
    if (serviceProxy_ != nullptr) {
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object != nullptr && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            serviceProxy_ = nullptr;
            captionObserver_ = nullptr;
            enableAbilityListsObserver_ = nullptr;
            configObserver_ = nullptr;
            isInitialized_ = false;
            HILOG_INFO("ResetService ok");
        }
    }
}

bool AccessibilityConfig::Impl::CheckSaStatus()
{
    std::vector<int> dependentSa = {
        ABILITY_MGR_SERVICE_ID,
        BUNDLE_MGR_SERVICE_SYS_ABILITY_ID,
        COMMON_EVENT_SERVICE_ID,
        DISPLAY_MANAGER_SERVICE_SA_ID,
        SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN,
        WINDOW_MANAGER_SERVICE_ID
    };
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return false;
    }

    if (std::any_of(dependentSa.begin(), dependentSa.end(), [&](int saId) {
        return samgr->CheckSystemAbility(saId) == nullptr;
    })) {
        return false;
    }
    return true;
}

Accessibility::RetError AccessibilityConfig::Impl::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_INFO("name = [%{private}s] capabilities = [%{private}u]", name.c_str(), capabilities);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->EnableAbility(name,
        capabilities));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::DisableAbility(const std::string &name)
{
    HILOG_INFO("name = [%{private}s]", name.c_str());
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->DisableAbility(name));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetMagnificationState(const bool state)
{
    HILOG_INFO("state = [%{public}d]", state);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(
        GetServiceProxy()->SetMagnificationState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetCaptionsState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetCaptionState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetCaptionsProperty(CaptionProperty &caption)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    CaptionPropertyParcel captionParcel(caption);
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(
        GetServiceProxy()->GetCaptionProperty(captionParcel));
    caption = static_cast<CaptionProperty>(captionParcel);
    HILOG_INFO();
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetCaptionsProperty(const CaptionProperty& caption)
{
    HILOG_INFO();
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    CaptionPropertyParcel captionParcel(caption);
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetCaptionProperty(
        captionParcel));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetCaptionsState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetCaptionState(state));
    return ret;
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
    HILOG_DEBUG("id = [%{public}d]", static_cast<int32_t>(id));
    std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
    std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetScreenMagnificationState(
        state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetShortKeyState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetMouseKeyState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetScreenMagnificationState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetScreenMagnificationState(
        state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetShortKeyState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetShortKeyState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetMouseKeyState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetMouseKeyState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

void AccessibilityConfig::Impl::UpdateCaptionEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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

void AccessibilityConfig::Impl::UpdateDaltonizationStateEnabled(const bool enabled)
{
    HILOG_INFO("enabled = [%{public}s]", enabled ? "True" : "False");
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        if (daltonizationState_ == enabled) {
            return;
        }
        daltonizationState_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyDaltonizationStateChanged(observers, enabled);
    if (!enabled) {
        HILOG_DEBUG();
        NotifyDaltonizationColorFilterChanged(observers, Normal);
    } else {
        HILOG_DEBUG();
        NotifyDaltonizationColorFilterChanged(observers, daltonizationColorFilter_);
    }
}

void AccessibilityConfig::Impl::UpdateIgnoreRepeatClickStateEnabled(const bool enabled)
{
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        if (ignoreRepeatClickState_ == enabled) {
            return;
        }
        ignoreRepeatClickState_ = enabled;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_IGNORE_REPEAT_CLICK_STATE);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }
    NotifyIgnoreRepeatClickStateChanged(observers, enabled);
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

void AccessibilityConfig::Impl::NotifyDaltonizationStateChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.daltonizationState = state;
            observer->OnConfigChanged(CONFIG_DALTONIZATION_STATE, configValue);
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
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetMouseAutoClick(time));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetShortkeyTarget(const std::string& name)
{
    HILOG_INFO("name = [%{public}s]", name.c_str());
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetShortkeyTarget(name));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetShortkeyMultiTarget(const std::vector<std::string>& name)
{
    HILOG_INFO("start");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetShortkeyMultiTarget(name));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetMouseAutoClick(int32_t &time)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetMouseAutoClick(time));
    HILOG_INFO("time = [%{public}d]", time);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetShortkeyTarget(std::string &name)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetShortkeyTarget(name));
    HILOG_INFO("name = [%{public}s]", name.c_str());
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetShortkeyMultiTarget(name));
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

void AccessibilityConfig::Impl::NotifyShortkeyMultiTargetChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
    const std::vector<std::string> &shortkeyMultiTarget)
{
    HILOG_DEBUG("start");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.shortkeyMultiTarget = shortkeyMultiTarget;
            observer->OnConfigChanged(CONFIG_SHORT_KEY_MULTI_TARGET, configValue);
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
    HILOG_INFO("daltonizationColorFilter = [%{public}u], daltonizationState_ = [%{public}d]", daltonizationColorFilter,
        daltonizationState_);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            if (!daltonizationState_) {
                HILOG_DEBUG();
                configValue.daltonizationColorFilter = Normal;
                observer->OnConfigChanged(CONFIG_DALTONIZATION_COLOR_FILTER, configValue);
            } else {
                configValue.daltonizationColorFilter = static_cast<DALTONIZATION_TYPE>(daltonizationColorFilter);
                observer->OnConfigChanged(CONFIG_DALTONIZATION_COLOR_FILTER, configValue);
            }
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyClickResponseTimeChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t clickResponseTime)
{
    HILOG_INFO("daltonizationColorFilter = [%{public}u]", clickResponseTime);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.clickResponseTime = static_cast<CLICK_RESPONSE_TIME>(clickResponseTime);
            observer->OnConfigChanged(CONIFG_CLICK_RESPONSE_TIME, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyIgnoreRepeatClickTimeChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const uint32_t time)
{
    HILOG_INFO("daltonizationColorFilter = [%{public}u]", time);
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.ignoreRepeatClickTime = static_cast<IGNORE_REPEAT_CLICK_TIME>(time);
            observer->OnConfigChanged(CONFIG_IGNORE_REPEAT_CLICK_TIME, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

void AccessibilityConfig::Impl::NotifyIgnoreRepeatClickStateChanged(
    const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers, const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    for (auto &observer : observers) {
        if (observer) {
            ConfigValue configValue;
            configValue.ignoreRepeatClickState = state;
            observer->OnConfigChanged(CONFIG_IGNORE_REPEAT_CLICK_STATE, configValue);
        } else {
            HILOG_ERROR("end configObservers_ is null");
        }
    }
}

Accessibility::RetError AccessibilityConfig::Impl::SetHighContrastTextState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetHighContrastTextState(
        state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetInvertColorState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetDaltonizationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetDaltonizationState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    HILOG_INFO("type = [%{public}u]", static_cast<uint32_t>(type));
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetDaltonizationColorFilter(
        type));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetContentTimeout(const uint32_t timer)
{
    HILOG_INFO("timer = [%{public}u]", timer);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetContentTimeout(timer));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetAnimationOffState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetBrightnessDiscount(const float brightness)
{
    HILOG_INFO("brightness = [%{public}f]", brightness);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetBrightnessDiscount(
        brightness));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetAudioMonoState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetAudioMonoState(state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetAudioBalance(const float balance)
{
    HILOG_INFO("balance = [%{public}f]", balance);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetAudioBalance(balance));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetClickResponseTime(const CLICK_RESPONSE_TIME time)
{
    HILOG_INFO("click response time = [%{public}u]", time);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetClickResponseTime(time));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetIgnoreRepeatClickState(
        state));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SetIgnoreRepeatClickTime(const IGNORE_REPEAT_CLICK_TIME time)
{
    HILOG_INFO("ignore repeat click time = [%{public}u]", time);
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->SetIgnoreRepeatClickTime(
        time));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetInvertColorState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetInvertColorState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetHighContrastTextState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetHighContrastTextState(
        state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetDaltonizationState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetDaltonizationState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    uint32_t filterType = 0;
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetDaltonizationColorFilter(
        filterType));
    type = static_cast<DALTONIZATION_TYPE>(filterType);
    HILOG_INFO("type = [%{public}u]", static_cast<uint32_t>(type));
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetContentTimeout(uint32_t &timer)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetContentTimeout(timer));
    HILOG_INFO("timer = [%{public}u]", timer);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAnimationOffState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetAnimationOffState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetBrightnessDiscount(float &brightness)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetBrightnessDiscount(
        brightness));
    HILOG_INFO("brightness = [%{public}f]", brightness);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAudioMonoState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetAudioMonoState(state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetAudioBalance(float &balance)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetAudioBalance(balance));
    HILOG_INFO("balance = [%{public}f]", balance);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetClickResponseTime(CLICK_RESPONSE_TIME &time)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    uint32_t responseTime = 0;
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetClickResponseTime(
        responseTime));
    time = static_cast<CLICK_RESPONSE_TIME>(responseTime);
    HILOG_INFO("click response time = [%{public}u]", time);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetIgnoreRepeatClickState(bool &state)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetIgnoreRepeatClickState(
        state));
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::GetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_TIME &time)
{
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }

    uint32_t ignoreRepeatClickTime = 0;
    Accessibility::RetError ret = static_cast<Accessibility::RetError>(GetServiceProxy()->GetIgnoreRepeatClickTime(
        ignoreRepeatClickTime));
    time = static_cast<IGNORE_REPEAT_CLICK_TIME>(ignoreRepeatClickTime);
    HILOG_INFO("ignore repeat click time = [%{public}u]", time);
    return ret;
}

Accessibility::RetError AccessibilityConfig::Impl::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(enableAbilityListsObserversMutex_);
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
    std::lock_guard<ffrt::mutex> lock(enableAbilityListsObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(enableAbilityListsObserversMutex_);
        observers = enableAbilityListsObservers_;
    }
    for (auto &enableAbilityListsObserver : observers) {
        enableAbilityListsObserver->OnEnableAbilityListsStateChanged();
    }
}

void AccessibilityConfig::Impl::OnAccessibilityInstallAbilityListsChanged()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    std::vector<std::shared_ptr<AccessibilityEnableAbilityListsObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(enableAbilityListsObserversMutex_);
        observers = enableAbilityListsObservers_;
    }
    for (auto &enableAbilityListsObserver : observers) {
        if (enableAbilityListsObserver != nullptr) {
            enableAbilityListsObserver->OnInstallAbilityListsStateChanged();
        } else {
            HILOG_ERROR("enableAbilityListsObserver is null");
        }
    }
}

void AccessibilityConfig::Impl::OnIgnoreRepeatClickStateChanged(const uint32_t stateType)
{
    if (stateType & Accessibility::STATE_IGNORE_REPEAT_CLICK_ENABLED) {
        UpdateIgnoreRepeatClickStateEnabled(true);
    } else {
        UpdateIgnoreRepeatClickStateEnabled(false);
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

    if (stateType & Accessibility::STATE_DALTONIZATION_STATE_ENABLED) {
        UpdateDaltonizationStateEnabled(true);
    } else {
        UpdateDaltonizationStateEnabled(false);
    }

    if (stateType & Accessibility::STATE_MOUSEKEY_ENABLED) {
        UpdateMouseKeyEnabled(true);
    } else {
        UpdateMouseKeyEnabled(false);
    }

    OnIgnoreRepeatClickStateChanged(stateType);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerAudioBalanceChanged(const float audioBalance)
{
    HILOG_DEBUG("audioBalance = [%{public}f}", audioBalance);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        if (daltonizationColorFilter_ == filterType) {
            HILOG_DEBUG("filterType[%{public}u]", daltonizationColorFilter_);
            return;
        }
        daltonizationColorFilter_ = InvertDaltonizationColorInAtoHos(filterType);
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyDaltonizationColorFilterChanged(observers, daltonizationColorFilter_);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick)
{
    HILOG_DEBUG("mouseAutoClick = [%{public}d}", mouseAutoClick);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
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

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerShortkeyMultiTargetChanged(
    const std::vector<std::string> &shortkeyMultiTarget)
{
    HILOG_DEBUG("start");
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        // need to add, if no change, do not inform
        shortkeyMultiTarget_ = shortkeyMultiTarget;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_SHORT_KEY_MULTI_TARGET);
        if (it == configObservers_.end()) {
            HILOG_DEBUG("Cannot find CONFIG_SHORT_KEY_MULTI_TARGET configObserver.");
            return;
        }
        observers = it->second;
    }

    NotifyShortkeyMultiTargetChanged(observers, shortkeyMultiTarget);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerClickResponseTimeChanged(const uint32_t clickResponseTime)
{
    HILOG_DEBUG("clickResponseTime = [%{public}u}", clickResponseTime);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        if (clickResponseTime_ == clickResponseTime) {
            return;
        }
        clickResponseTime_ = clickResponseTime;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONIFG_CLICK_RESPONSE_TIME);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyClickResponseTimeChanged(observers, clickResponseTime);
}

void AccessibilityConfig::Impl::OnAccessibleAbilityManagerIgnoreRepeatClickTimeChanged(const uint32_t time)
{
    HILOG_DEBUG("ignoreRepeatClickTime = [%{public}u}", time);
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> observers;
    {
        std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
        if (ignoreRepeatClickTime_ == time) {
            return;
        }
        ignoreRepeatClickTime_ = time;
        std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
            configObservers_.find(CONFIG_IGNORE_REPEAT_CLICK_TIME);
        if (it == configObservers_.end()) {
            return;
        }
        observers = it->second;
    }

    NotifyIgnoreRepeatClickTimeChanged(observers, time);
}

void AccessibilityConfig::Impl::NotifyImmediately(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    HILOG_DEBUG("NotifyImmediately start.");
    ConfigValue configValue;
    {
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
        configValue.daltonizationState = daltonizationState_;
        if (!configValue.daltonizationState) {
            configValue.daltonizationColorFilter = Normal;
        } else {
            configValue.daltonizationColorFilter = static_cast<DALTONIZATION_TYPE>(daltonizationColorFilter_);
        }
        configValue.shortkey_target = shortkeyTarget_;
        configValue.shortkeyMultiTarget = shortkeyMultiTarget_;
        configValue.captionStyle = captionProperty_;
        configValue.clickResponseTime = static_cast<CLICK_RESPONSE_TIME>(clickResponseTime_);
        configValue.ignoreRepeatClickState = ignoreRepeatClickState_;
        configValue.ignoreRepeatClickTime = static_cast<IGNORE_REPEAT_CLICK_TIME>(ignoreRepeatClickTime_);
    }
    observer->OnConfigChanged(id, configValue);
}

uint32_t AccessibilityConfig::Impl::InvertDaltonizationColorInAtoHos(uint32_t filter)
{
    if (filter == DISPLAY_DALTONIZER_GREEN) {
        return Deuteranomaly;
    }
    if (filter == DISPLAY_DALTONIZER_RED) {
        return Protanomaly;
    }
    if (filter == DISPLAY_DALTONIZER_BLUE) {
        return Tritanomaly;
    }
    return filter;
}

void AccessibilityConfig::Impl::InitConfigValues()
{
    Accessibility::AccessibilityConfigData configData;
    CaptionProperty caption = {};
    CaptionPropertyParcel captionParcel(caption);
    if (serviceProxy_ == nullptr) {
        return;
    }
    serviceProxy_->GetAllConfigs(configData, captionParcel);
    
    std::lock_guard<ffrt::mutex> lock(configObserversMutex_);
    highContrastText_ = configData.highContrastText_;
    invertColor_ = configData.invertColor_;
    animationOff_ = configData.animationOff_;
    audioMono_ = configData.audioMono_;
    mouseKey_ = configData.mouseKey_;
    captionState_ = configData.captionState_;
    screenMagnifier_ = configData.screenMagnifier_;
    shortkey_ = configData.shortkey_;
    mouseAutoClick_ = configData.mouseAutoClick_;
    daltonizationState_ = configData.daltonizationState_;
    daltonizationColorFilter_ = InvertDaltonizationColorInAtoHos(configData.daltonizationColorFilter_);
    contentTimeout_ = configData.contentTimeout_;
    brightnessDiscount_ = configData.brightnessDiscount_;
    audioBalance_ = configData.audioBalance_;
    shortkeyTarget_ = configData.shortkeyTarget_;
    shortkeyMultiTarget_ = configData.shortkeyMultiTarget_;
    clickResponseTime_ = configData.clickResponseTime_;
    ignoreRepeatClickTime_ = configData.ignoreRepeatClickTime_;
    ignoreRepeatClickState_ = configData.ignoreRepeatClickState_;
    captionProperty_ = static_cast<CaptionProperty>(captionParcel);
    NotifyDefaultConfigs();
    HILOG_DEBUG("ConnectToService Success");
}

void AccessibilityConfig::Impl::NotifyDefaultDaltonizationConfigs()
{
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(CONFIG_DALTONIZATION_STATE);
    if (it != configObservers_.end()) {
        NotifyDaltonizationStateChanged(it->second, daltonizationState_);
    }
    if ((it = configObservers_.find(CONFIG_DALTONIZATION_COLOR_FILTER)) != configObservers_.end()) {
        NotifyDaltonizationColorFilterChanged(it->second, daltonizationColorFilter_);
    }
}

void AccessibilityConfig::Impl::NotifyDefaultScreenTouchConfigs()
{
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(CONIFG_CLICK_RESPONSE_TIME);
    if (it != configObservers_.end()) {
        NotifyClickResponseTimeChanged(it->second, clickResponseTime_);
    }
    if ((it = configObservers_.find(CONFIG_IGNORE_REPEAT_CLICK_STATE)) != configObservers_.end()) {
        NotifyIgnoreRepeatClickTimeChanged(it->second, ignoreRepeatClickState_);
    }
    if ((it = configObservers_.find(CONFIG_IGNORE_REPEAT_CLICK_TIME)) != configObservers_.end()) {
        NotifyIgnoreRepeatClickStateChanged(it->second, ignoreRepeatClickTime_);
    }
}

void AccessibilityConfig::Impl::NotifyDefaultShortKeyConfigs()
{
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(CONFIG_SHORT_KEY);
    if (it != configObservers_.end()) {
        NotifyShortKeyChanged(it->second, shortkey_);
    }
    if ((it = configObservers_.find(CONFIG_SHORT_KEY_TARGET)) != configObservers_.end()) {
        NotifyShortkeyTargetChanged(it->second, shortkeyTarget_);
    }
}

void AccessibilityConfig::Impl::NotifyDefaultShortKeyMultiConfigs()
{
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>>::iterator it =
        configObservers_.find(CONFIG_SHORT_KEY);
    if (it != configObservers_.end()) {
        NotifyShortKeyChanged(it->second, shortkey_);
    }
    if ((it = configObservers_.find(CONFIG_SHORT_KEY_MULTI_TARGET)) != configObservers_.end()) {
        NotifyShortkeyMultiTargetChanged(it->second, shortkeyMultiTarget_);
    }
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
    if ((it = configObservers_.find(CONFIG_CAPTION_STATE)) != configObservers_.end()) {
        NotifyCaptionStateChanged(it->second, captionState_);
    }
    if ((it = configObservers_.find(CONFIG_CAPTION_STYLE)) != configObservers_.end()) {
        NotifyCaptionChanged(it->second, captionProperty_);
    }
    if ((it = configObservers_.find(CONFIG_SCREEN_MAGNIFICATION)) != configObservers_.end()) {
        NotifyScreenMagnificationChanged(it->second, screenMagnifier_);
    }
    if ((it = configObservers_.find(CONFIG_MOUSE_AUTOCLICK)) != configObservers_.end()) {
        NotifyMouseAutoClickChanged(it->second, mouseAutoClick_);
    }

    NotifyDefaultDaltonizationConfigs();
    NotifyDefaultScreenTouchConfigs();
    NotifyDefaultShortKeyConfigs();
    NotifyDefaultShortKeyMultiConfigs();
}

void AccessibilityConfig::Impl::AccessibilityLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    HILOG_DEBUG();
    if (config_) {
        config_->LoadSystemAbilitySuccess(remoteObject);
    }
}

void AccessibilityConfig::Impl::AccessibilityLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOG_DEBUG();
    if (config_) {
        config_->LoadSystemAbilityFail();
    }
}

Accessibility::RetError AccessibilityConfig::Impl::SetEnhanceConfig(uint8_t *cfg, uint32_t cfgLen)
{
    HILOG_INFO();
    Utils::UniqueReadGuard<Utils::RWLock> rLock(rwLock_);
    if (cfg == nullptr || cfgLen <= 0) {
        HILOG_ERROR("SecCompEnhance cfg info is empty");
        return Accessibility::RET_ERR_NULLPTR;
    }
 
    if (GetServiceProxy() == nullptr) {
        HILOG_ERROR("Failed to get accessibility service");
        return Accessibility::RET_ERR_SAMGR;
    }
    AccessibilitySecCompRawdata rawData;
    rawData.size = cfgLen;
    rawData.data = cfg;
    return static_cast<Accessibility::RetError>(GetServiceProxy()->SetEnhanceConfig(rawData));
}
} // namespace AccessibilityConfig
} // namespace OHOS
