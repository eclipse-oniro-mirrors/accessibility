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

#include <cstdint>
#include <mutex>
#include "singleton.h"
#include "accessibility_config.h"
#include "accessibility_enable_ability_lists_observer_stub.h"
#include "accessible_ability_manager_caption_observer_stub.h"
#include "accessible_ability_manager_config_observer_stub.h"
#include "accessible_ability_manager_service_proxy.h"

namespace OHOS {
namespace AccessibilityConfig {
class AccessibilityConfigImpl : public AccessibilityConfig {
public:
    explicit AccessibilityConfigImpl();
    ~AccessibilityConfigImpl();

    /**
     * @brief Subscribes to accessibility config value.
     * @param ids  the ids which are observed.
     * @param observer Indicates the observer for listening to accessibility
     * config.
     * @return true: send ok; otherwise is refused.
     */
    virtual void SubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer) override;
    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param ids  the ids which are observed
     * @param observer Indicates the registered accessibility config observer.
     * @return true is succeed otherwise is failed.
     */
    virtual void UnsubscribeConfigObserver(const std::shared_ptr<AccessibilityConfigObserver>& observer) override;

    /**
     * @brief Subscribes the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    virtual void SubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer) override;

    /**
     * @brief Unsubscribe the observer of enable Ability lists
     * @param observer Indicates the observer for listening to enable Ability lists
     */
    virtual void UnsubscribeEnableAbilityListsObserver(
        const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer) override;

    void ResetService(const wptr<IRemoteObject> &remote);

    /**
     * @brief Enabled specified abilities
     * @param name The strings formated by 'bundleName/abilityName'.
     * @param capabilities The capabilities you permit.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool EnableAbilities(const std::string name, const uint32_t capabilities) override;

    /**
     * @brief Disabled specified abilities
     * @param name The strings formated by 'bundleName/abilityName'.
     * @return Return true if the command is sent successfully, else return false.
     */
    virtual bool DisableAbilities(const std::string name) override;

    virtual bool SetScreenMagnificationState(const bool state) override;
    virtual bool SetShortKeyState(const bool state) override;
    virtual bool SetMouseKeyState(const bool state) override;
    virtual bool SetCaptionState(const bool state) override;
    virtual bool SetCaptionProperty(const CaptionProperty &caption) override;
    virtual bool SetMouseAutoClick(const int32_t time) override;
    virtual bool SetShortkeyTarget(const std::string &name) override;
    virtual bool SetHighContrastTextState(bool state) override;
    virtual bool SetInvertColorState(const bool state) override;
    virtual bool SetDaltonizationColorFilter(const DALTONIZATION_TYPE type) override;
    virtual bool SetContentTimeout(const uint32_t timer) override;
    virtual bool SetAnimationOffState(const bool state) override;
    virtual bool SetBrightnessDiscount(const float brightness) override;
    virtual bool SetAudioMonoState(const bool state) override;
    virtual bool SetAudioBalance(const float balance) override;

    virtual bool GetScreenMagnificationState(bool &value) override;
    virtual bool GetShortKeyState(bool &value) override;
    virtual bool GetMouseKeyState(bool &value) override;
    virtual bool GetCaptionState(bool &value) override;
    virtual bool GetCaptionProperty(CaptionProperty &value) override;
    virtual bool GetMouseAutoClick(int32_t &value) override;
    virtual bool GetShortkeyTarget(std::string &value) override;
    virtual bool GetInvertColorState(bool &value) override;
    virtual bool GetHighContrastTextState(bool &value) override;
    virtual bool GetDaltonizationColorFilter(DALTONIZATION_TYPE &value) override;
    virtual bool GetContentTimeout(uint32_t &value)override;
    virtual bool GetAnimationOffState(bool &value) override;
    virtual bool GetBrightnessDiscount(float &value) override;
    virtual bool GetAudioMonoState(bool &value)override;
    virtual bool GetAudioBalance(float &value) override;

    virtual void OnAccessibleAbilityManagerCaptionPropertyChanged(const CaptionProperty &property);
    virtual void OnAccessibleAbilityManagerCongfigStateChanged(const uint32_t stateType);
    virtual void OnAccessibleAbilityManagerAudioBalanceChanged(const float audioBalance);
    virtual void OnAccessibleAbilityManagerBrightnessDiscountChanged(const float brightnessDiscount);
    virtual void OnAccessibleAbilityManagerContentTimeoutChanged(const uint32_t contentTimeout);
    virtual void OnAccessibleAbilityManagerDaltonizationColorFilterChanged(const DALTONIZATION_TYPE filterType);
    virtual void OnAccessibleAbilityManagerMouseAutoClickChanged(const int32_t mouseAutoClick);
    virtual void OnAccessibleAbilityManagerShortkeyTargetChanged(const std::string &shortkeyTarget);

    virtual void UpdateScreenMagnificationEnabled(const bool enabled) override;
    virtual void UpdateShortKeyEnabled(const bool enabled) override;
    virtual void UpdateMouseKeyEnabled(const bool enabled) override;
    virtual void UpdateCaptionEnabled(const bool enabled) override;

    void OnAccessibilityEnableAbilityListsChanged();

private:
    class AccessibilityEnableAbilityListsObserverStubImpl :
        public Accessibility::AccessibilityEnableAbilityListsObserverStub {
    public:
        explicit AccessibilityEnableAbilityListsObserverStubImpl(AccessibilityConfigImpl &client)
            : client_(client) {}
        ~AccessibilityEnableAbilityListsObserverStubImpl() = default;

        virtual void OnAccessibilityEnableAbilityListsChanged() override
        {
            client_.OnAccessibilityEnableAbilityListsChanged();
        }
    private:
        AccessibilityConfigImpl &client_;
    };

    class AccessibleAbilityManagerCaptionObserverImpl
        : public Accessibility::AccessibleAbilityManagerCaptionObserverStub {
    public:
        explicit AccessibleAbilityManagerCaptionObserverImpl(AccessibilityConfigImpl &config_)
            : config_(config_) {}
        ~AccessibleAbilityManagerCaptionObserverImpl() = default;

        virtual void OnPropertyChanged(const CaptionProperty &property) override
        {
            config_.OnAccessibleAbilityManagerCaptionPropertyChanged(property);
        }
    private:
        AccessibilityConfigImpl &config_;
    };

    class AccessibleAbilityManagerConfigObserverImpl
        : public Accessibility::AccessibleAbilityManagerConfigObserverStub {
    public:
        explicit AccessibleAbilityManagerConfigObserverImpl(AccessibilityConfigImpl &config_)
            : config_(config_) {}
        ~AccessibleAbilityManagerConfigObserverImpl() = default;

        virtual void OnConfigStateChanged(const uint32_t stateType) override
        {
            config_.OnAccessibleAbilityManagerCongfigStateChanged(stateType);
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
        virtual void OnDaltonizationColorFilterChanged(const DALTONIZATION_TYPE filterType) override
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
        AccessibilityConfigImpl &config_;
    };

    class DeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit DeathRecipient(AccessibilityConfigImpl &config_) : config_(config_) {}
        ~DeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(DeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote)
        {
            config_.ResetService(remote);
        }
    private:
        AccessibilityConfigImpl &config_;
    };

    bool ConnectToService();
    void NotifyCaptionStateChanged();
    void NotifyCaptionChanged();
    void NotifyScreenMagnificationChanged();
    void NotifyShortKeyChanged();
    void NotifyMouseKeyChanged();
    void NotifyShortkeyTargetChanged();
    void NotifyMouseAutoClickChanged();
    void NotifyAudioBalanceChanged();
    void NotifyBrightnessDiscountChanged();
    void NotifyContentTimeoutChanged();
    void NotifyDaltonizationColorFilterChanged();

    sptr<AccessibilityEnableAbilityListsObserverStubImpl> enableAbilityListsObserverStub_ = nullptr;
    sptr<Accessibility::IAccessibleAbilityManagerService> serviceProxy_ = nullptr;
    sptr<AccessibleAbilityManagerCaptionObserverImpl> captionObserver_ = nullptr;
    sptr<AccessibleAbilityManagerConfigObserverImpl> configObserver_;

    bool mouseKey_ = false;
    bool captionState_ = false;
    CaptionProperty captionProperty_;
    int32_t mouseAutoClick_;
    bool screenMagnifier_ = false;
    bool shortkey_ = false;
    std::string shortkeyTarget_ = "";
    uint32_t daltonizationColorFilter_ = 0;
    uint32_t contentTimeout_ = 0;
    float brightnessDiscount_ = 0.0;
    float audioBalance_ = 0.0;

    std::recursive_mutex acProxyLock_;

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> configObservers_;
    std::vector<std::shared_ptr<AccessibilityEnableAbilityListsObserver>> enableAbilityListsObservers_;
};
} // namespace AccessibilityConfig
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_IMPL_H
