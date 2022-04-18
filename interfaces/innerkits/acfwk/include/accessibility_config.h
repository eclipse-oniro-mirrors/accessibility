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

#ifndef ACCESSIBILITY_CONFIG_H
#define ACCESSIBILITY_CONFIG_H

#include <cstdint>
#include "singleton.h"

namespace OHOS {
namespace AccessibilityConfig {
enum CONFIGID : int32_t {
    CONFIG_HIGH_CONTRASTE_TEXT = 0,
    CONFIG_INVERT_COLOR,
    CONFIG_DALTONIZATION_COLOR_FILTER,
    CONFIG_CONTENT_TIMEOUT,
    CONFIG_ANIMATION_OFF,
    CONFIG_BRIGHTNESS_DISCOUNT,
    CONFIG_AUDIO_MONO,
    CONFIG_AUDIO_BALANCE,
    CONFIG_MOUSE_KEY,
    CONFIG_ID_MAX,
};

enum DALTONIZATION_TYPE : int32_t {
    Normal = 0,
    Protanomaly,
    Deuteranomaly,
    Tritanomaly,
};

struct AudioBalance {
    int32_t left;
    int32_t right;
};

typedef union {
    bool highContrastText;
    bool invertColor;
    DALTONIZATION_TYPE daltonizationColorFilter;
    int32_t contentTimeout;
    bool animationOff;
    int32_t brightnessDiscount;
    bool audioMono;
    AudioBalance audioBalance;
    bool mouseKey;
} ConfigValue;

class AccessibilityConfigObserver {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~ConfigObserver() = default;

    /**
     * @brief Called when the config value changed.
     * @param id the id of config.
     * @param value the value of config.
     * @return
     */
    virtual void OnConfigChanged(const CONFIGID id, const ConfigValue value) = 0;
};

class AccessibilityConfig {
    DECLARE_DELAYED_SINGLETON(AccessibilityConfig)
public:
    /**
     * @brief Subscribes to accessibility config value.
     * @param ids  the config ids which are observed.
     * @param observer Indicates the observer for listening to accessibility
     * config.
     * @return true: send ok; otherwise is refused.
     */
    virtual bool SubscribeConfigObserver(const std::vector<CONFIGID> ids,
                const std::shared_ptr<AccessibilityConfigObserver>& observer) = 0;

    /**
     * @brief Unsubscribe the accessibility config value observer.
     * @param ids  the ids which are observed.
     * @param observer Indicates the registered accessibility config observer.
     * @return true is succeed otherwise is failed.
     */
    virtual bool UnsubscribeConfigObserver(const int32_t ids,
                const std::shared_ptr<AccessibilityConfigObserver>& observer) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_CONFIG_H
