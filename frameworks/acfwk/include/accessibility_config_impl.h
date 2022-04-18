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
#include "singleton.h"

#include "accessibility_config.h"

namespace OHOS {
namespace AccessibilityConfig {
class AccessibilityConfigImpl : public AccessibilityConfig {
    DECLARE_DELAYED_SINGLETON(AccessibilityConfig)
public:
    /**
     * @brief Subscribes to accessibility config value.
     * @param ids  the ids which are observed.
     * @param observer Indicates the observer for listening to accessibility
     * config.
     * @return true: send ok; otherwise is refused.
     */
    bool SubscribeConfigObserver(const std::vector<CONFIGID> ids,
            const std::shared_ptr<AccessibilityConfigObserver>& observer) override;

    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param ids  the ids which are observed
     * @param observer Indicates the registered accessibility config observer.
     * @return true is succeed otherwise is failed.
     */
    bool UnsubscribeConfigObserver(const std::vector<CONFIGID> ids,
            const std::shared_ptr<AccessibilityConfigObserver>& observer) override;

    bool SetHighContrastTextState(bool state);
    bool GetHighContrastTextState();

    bool SetInvertColorState(const bool state);
    bool GetInvertColorState();

    bool SetDaltonizationColorFilter(const DALTONIZATION_TYPE type);
    DALTONIZATION_TYPE GetDaltonizationColorFilter();

    bool SetContentTimeout(const int32_t timer);
    int32_t GetContentTimeout();

    bool SetAnimationOffState(const bool state);
    bool GetAnimationOffState();

    bool SetBrightnessDiscount(const int32_t brightness);
    int32_t GetBrightnessDiscount();

    bool SetAudioMonoState(const bool state);
    bool GetAudioMonoState();

    bool SetAudioBalance(const AudioBalance balance);
    AudioBalance GetAudioBalance();

    bool SetMouseKeyState(const bool);
    bool GetMouseKeyState();

private:
    bool highContrastText;
    bool invertColor;
    DALTONIZATION_TYPE daltonizationColorFilter;
    int32_t contentTimeout;
    bool animationOff;
    int32_t brightnessDiscount;
    bool audioMono;
    AudioBalance audioBalance;
    bool mouseKey;

    std::vector<std::shared_ptr<AccessibilityConfigObserver>> highContrastTextObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> invertColorObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> daltonizationColorFilterObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> animationOffObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> brightnessDiscountObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> audioMonoObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> audioBalanceObservers_;
    std::vector<std::shared_ptr<AccessibilityConfigObserver>> mouseKeyObservers_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_IMPL_H
