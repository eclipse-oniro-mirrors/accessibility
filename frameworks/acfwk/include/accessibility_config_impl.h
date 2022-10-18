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

#ifndef ACCESSIBILITY_CONFIG_IMPL_H
#define ACCESSIBILITY_CONFIG_IMPL_H

#include <mutex>
#include "accessibility_config.h"
#include "accessibility_enable_ability_lists_observer_stub.h"
#include "accessible_ability_manager_caption_observer_stub.h"
#include "accessible_ability_manager_config_observer_stub.h"
#include "accessible_ability_manager_service_proxy.h"
#include "event_handler.h"

namespace OHOS {
namespace AccessibilityConfig {
class AccessibilityConfig::Impl {
public:
    Impl();
    ~Impl() = default;

    bool InitializeContext();

    Accessibility::RetError SubscribeConfigObserver(const CONFIG_ID id,
        const std::shared_ptr<AccessibilityConfigObserver> &observer, const bool retFlag);
    Accessibility::RetError UnsubscribeConfigObserver(const CONFIG_ID id,
        const std::shared_ptr<AccessibilityConfigObserver> &observer);

    Accessibility::RetError SubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);
    Accessibility::RetError UnsubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer);

    Accessibility::RetError EnableAbility(const std::string &name, const uint32_t capabilities);
    Accessibility::RetError DisableAbility(const std::string &name);

    Accessibility::RetError SetScreenMagnificationState(const bool state);
    Accessibility::RetError SetShortKeyState(const bool state);
    Accessibility::RetError SetMouseKeyState(const bool state);
    Accessibility::RetError SetCaptionsState(const bool state);
    Accessibility::RetError SetCaptionsProperty(const CaptionProperty &caption);
    Accessibility::RetError SetMouseAutoClick(const int32_t time);
    Accessibility::RetError SetShortkeyTarget(const std::string &name);
    Accessibility::RetError SetHighContrastTextState(bool state);
    Accessibility::RetError SetInvertColorState(const bool state);
    Accessibility::RetError SetDaltonizationColorFilter(const DALTONIZATION_TYPE type);
    Accessibility::RetError SetContentTimeout(const uint32_t timer);
    Accessibility::RetError SetAnimationOffState(const bool state);
    Accessibility::RetError SetBrightnessDiscount(const float brightness);
    Accessibility::RetError SetAudioMonoState(const bool state);
    Accessibility::RetError SetAudioBalance(const float balance);

    Accessibility::RetError GetScreenMagnificationState(bool &state);
    Accessibility::RetError GetShortKeyState(bool &state);
    Accessibility::RetError GetMouseKeyState(bool &state);
    Accessibility::RetError GetCaptionsState(bool &state);
    Accessibility::RetError GetCaptionsProperty(CaptionProperty &caption);
    Accessibility::RetError GetMouseAutoClick(int32_t &time);
    Accessibility::RetError GetShortkeyTarget(std::string &name);
    Accessibility::RetError GetInvertColorState(bool &state);
    Accessibility::RetError GetHighContrastTextState(bool &state);
    Accessibility::RetError GetDaltonizationColorFilter(DALTONIZATION_TYPE &type);
    Accessibility::RetError GetContentTimeout(uint32_t &timer);
    Accessibility::RetError GetAnimationOffState(bool &state);
    Accessibility::RetError GetBrightnessDiscount(float &brightness);
    Accessibility::RetError GetAudioMonoState(bool &state);
    Accessibility::RetError GetAudioBalance(float &balance);

    void ResetService(const wptr<IRemoteObject> &remote);
    void OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty &property);
    void OnAccessibleAbilityManagerConfigStateChanged(const uint32_t stateType);
    void OnAccessibleAbilityManagerAudioBalanceChanged(const float audioBalance);
    void OnAccessibleAbilityManagerBrightnessDiscountChanged(const float brightnessDiscount);
    void OnAccessibleAbilityManagerContentTimeoutChanged(const uint32_t contentTimeout);
    void OnAccessibleAbilityManagerDaltonizationColorFilterChanged(const uint32_t filterType);
    void OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick);
    void OnAccessibleAbilityManagerShortkeyTargetChanged(const std::string &shortkeyTarget);
    void OnAccessibilityEnableAbilityListsChanged();

private:
    class AccessibilityEnableAbilityListsObserverImpl :
        public Accessibility::AccessibilityEnableAbilityListsObserverStub {
    public:
        explicit AccessibilityEnableAbilityListsObserverImpl(Impl &client)
            : client_(client) {}
        ~AccessibilityEnableAbilityListsObserverImpl() = default;

        virtual void OnAccessibilityEnableAbilityListsChanged() override
        {
            client_.OnAccessibilityEnableAbilityListsChanged();
        }
    private:
        Impl &client_;
    };

    class AccessibleAbilityManagerCaptionObserverImpl
        : public Accessibility::AccessibleAbilityManagerCaptionObserverStub {
    public:
        explicit AccessibleAbilityManagerCaptionObserverImpl(Impl &config_)
            : config_(config_) {}
        ~AccessibleAbilityManagerCaptionObserverImpl() = default;

        virtual void OnPropertyChanged(const CaptionProperty &property) override
        {
            config_.OnAccessibleAbilityManagerCaptionPropertyChanged(property);
        }
    private:
        Impl &config_;
    };

    class AccessibleAbilityManagerConfigObserverImpl
        : public Accessibility::AccessibleAbilityManagerConfigObserverStub {
    public:
        explicit AccessibleAbilityManagerConfigObserverImpl(Impl &config) : config_(config) {}
        ~AccessibleAbilityManagerConfigObserverImpl() = default;

        virtual void OnConfigStateChanged(const uint32_t stateType) override
        {
            config_.OnAccessibleAbilityManagerConfigStateChanged(stateType);
        }
        virtual void OnAudioBalanceChanged(const float audioBalance) override
        {
            config_.OnAccessibleAbilityManagerAudioBalanceChanged(audioBalance);
        }
        virtual void OnBrightnessDiscountChanged(const float brightnessDiscount) override
        {
            config_.OnAccessibleAbilityManagerBrightnessDiscountChanged(brightnessDiscount);
        }
        virtual void OnContentTimeoutChanged(const uint32_t contentTimeout) override
        {
            config_.OnAccessibleAbilityManagerContentTimeoutChanged(contentTimeout);
        }
        virtual void OnDaltonizationColorFilterChanged(const uint32_t filterType) override
        {
            config_.OnAccessibleAbilityManagerDaltonizationColorFilterChanged(filterType);
        }
        virtual void OnMouseAutoClickChanged(const int32_t mouseAutoClick) override
        {
            config_.OnAccessibleAbilityManagerMouseAutoClickChanged(mouseAutoClick);
        }
        virtual void OnShortkeyTargetChanged(const std::string &shortkeyTarget) override
        {
            config_.OnAccessibleAbilityManagerShortkeyTargetChanged(shortkeyTarget);
        }

    private:
        Impl &config_;
    };

    class DeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit DeathRecipient(Impl &config_) : config_(config_) {}
        ~DeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(DeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote)
        {
            config_.ResetService(remote);
        }
    private:
        Impl &config_;
    };

    bool ConnectToService();
    bool ConnectToServiceAsync();

    bool RegisterToService();
    bool InitAccessibilityServiceProxy();

    void NotifyCaptionStateChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyCaptionChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const CaptionProperty &captionProperty);
    void NotifyScreenMagnificationChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyShortKeyChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyMouseKeyChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyShortkeyTargetChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const std::string &shortkey_target);
    void NotifyMouseAutoClickChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const uint32_t mouseAutoClick);
    void NotifyAudioBalanceChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const float audioBalance);
    void NotifyBrightnessDiscountChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const float brightnessDiscount);
    void NotifyContentTimeoutChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const uint32_t contentTimeout);
    void NotifyDaltonizationColorFilterChanged(
        const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const uint32_t daltonizationColorFilter);
    void NotifyAudioMonoChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyAnimationOffChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyInvertColorChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);
    void NotifyHighContrastTextChanged(const std::vector<std::shared_ptr<AccessibilityConfigObserver>> &observers,
        const bool state);

    void UpdateScreenMagnificationEnabled(const bool enabled);
    void UpdateShortKeyEnabled(const bool enabled);
    void UpdateMouseKeyEnabled(const bool enabled);
    void UpdateCaptionEnabled(const bool enabled);
    void UpdateAudioMonoEnabled(const bool enabled);
    void UpdateAnimationOffEnabled(const bool enabled);
    void UpdateInvertColorEnabled(const bool enabled);
    void UpdateHighContrastTextEnabled(const bool enabled);
    void NotifyDefaultConfigs();
    void NotifyImmediately(const CONFIG_ID id, const std::shared_ptr<AccessibilityConfigObserver> &observer);
    void InitConfigValues();
    void InitEventHandler();
    static void OnParameterChanged(const char *key, const char *value, void *context);

    sptr<Accessibility::IAccessibleAbilityManagerService> serviceProxy_ = nullptr;
    sptr<AccessibleAbilityManagerCaptionObserverImpl> captionObserver_ = nullptr;
    sptr<AccessibleAbilityManagerConfigObserverImpl> configObserver_ = nullptr;
    sptr<AccessibilityEnableAbilityListsObserverImpl> enableAbilityListsObserver_ = nullptr;

    bool isInitialized_ = false;
    bool shortkey_ = false;
    bool highContrastText_ = false;
    bool screenMagnifier_ = false;
    bool invertColor_ = false;
    bool captionState_ = false;
    bool animationOff_ = false;
    bool audioMono_ = false;
    bool mouseKey_ = false;
    int32_t mouseAutoClick_ = 0;
    uint32_t contentTimeout_ = 0;
    uint32_t daltonizationColorFilter_ = 0;
    float audioBalance_ = 0.0;
    float brightnessDiscount_ = 0.0;
    std::string shortkeyTarget_ = "";
    CaptionProperty captionProperty_ = {};

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;

    std::vector<std::shared_ptr<AccessibilityEnableAbilityListsObserver>> enableAbilityListsObservers_;
    std::map<CONFIG_ID, std::vector<std::shared_ptr<AccessibilityConfigObserver>>> configObservers_;
    std::mutex mutex_;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
} // namespace AccessibilityConfig
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_IMPL_H
