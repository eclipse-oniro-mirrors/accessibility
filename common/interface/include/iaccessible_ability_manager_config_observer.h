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

#ifndef INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CONFIG_OBSERVER_H
#define INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CONFIG_OBSERVER_H

#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityManagerConfigObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerconfigObserver");

    virtual void OnConfigStateChanged(const uint32_t stateType) = 0;
    virtual void OnAudioBalanceChanged(const float audioBalance) = 0;
    virtual void OnBrightnessDiscountChanged(const float brightnessDiscount) = 0;
    virtual void OnContentTimeoutChanged(const uint32_t contentTimeout) = 0;
    virtual void OnDaltonizationColorFilterChanged(const uint32_t filterType) = 0;
    virtual void OnMouseAutoClickChanged(const int32_t mouseAutoClick) = 0;
    virtual void OnShortkeyTargetChanged(const std::string &shortkeyTarget) = 0;
    virtual void OnShortkeyMultiTargetChanged(const std::vector<std::string> &shortkeyMultiTarget) = 0;
    virtual void OnClickResponseTimeChanged(const uint32_t clickResponseTime) = 0;
    virtual void OnIgnoreRepeatClickTimeChanged(const uint32_t ignoreRepeatClickTime) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CONFIG_OBSERVER_H