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

#ifndef MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_PROXY_H
#define MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_PROXY_H

#include <gmock/gmock.h>
#include "accessible_ability_manager_caption_observer_proxy.h"
#include "accessible_ability_manager_config_observer_proxy.h"
#include "accessibility_enable_ability_lists_observer_proxy.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityManagerCaptionObserverProxy : public AccessibleAbilityManagerCaptionObserverProxy {
public:
    MockAccessibleAbilityManagerCaptionObserverProxy(const sptr<IRemoteObject>& object);
    virtual ~MockAccessibleAbilityManagerCaptionObserverProxy() override;
    MOCK_METHOD1(OnPropertyChanged, void(const AccessibilityConfig::CaptionProperty &property));
};

class MockAccessibleAbilityManagerConfigObserverProxy : public AccessibleAbilityManagerConfigObserverProxy {
public:
    MockAccessibleAbilityManagerConfigObserverProxy(const sptr<IRemoteObject>& object);
    virtual ~MockAccessibleAbilityManagerConfigObserverProxy() override;
    MOCK_METHOD1(OnConfigStateChanged, void(const uint32_t stateType));
    MOCK_METHOD1(OnAudioBalanceChanged, void(const float audioBalance));
    MOCK_METHOD1(OnBrightnessDiscountChanged, void(const float brightnessDiscount));
    MOCK_METHOD1(OnContentTimeoutChanged, void(const uint32_t contentTimeout));
    MOCK_METHOD1(OnDaltonizationColorFilterChanged, void(const uint32_t filterType));
    MOCK_METHOD1(OnMouseAutoClickChanged, void(const int32_t mouseAutoClick));
    MOCK_METHOD1(OnShortkeyTargetChanged, void(const std::string &shortkeyTarget));
};

class MockAccessibilityEnableAbilityListsObserverProxy : public AccessibilityEnableAbilityListsObserverProxy {
public:
    MockAccessibilityEnableAbilityListsObserverProxy(const sptr<IRemoteObject>& object);
    virtual ~MockAccessibilityEnableAbilityListsObserverProxy() override;
    MOCK_METHOD0(OnAccessibilityEnableAbilityListsChanged, void());
};

MockAccessibleAbilityManagerCaptionObserverProxy::MockAccessibleAbilityManagerCaptionObserverProxy(
    const sptr<IRemoteObject>& object)
    : AccessibleAbilityManagerCaptionObserverProxy(object)
{
    (void)object;
}
MockAccessibleAbilityManagerCaptionObserverProxy::~MockAccessibleAbilityManagerCaptionObserverProxy()
{}

MockAccessibleAbilityManagerConfigObserverProxy::MockAccessibleAbilityManagerConfigObserverProxy(
    const sptr<IRemoteObject>& object)
    : AccessibleAbilityManagerConfigObserverProxy(object)
{
    (void)object;
}
MockAccessibleAbilityManagerConfigObserverProxy::~MockAccessibleAbilityManagerConfigObserverProxy()
{}

MockAccessibilityEnableAbilityListsObserverProxy::MockAccessibilityEnableAbilityListsObserverProxy(
    const sptr<IRemoteObject>& object)
    : AccessibilityEnableAbilityListsObserverProxy(object)
{
    (void)object;
}

MockAccessibilityEnableAbilityListsObserverProxy::~MockAccessibilityEnableAbilityListsObserverProxy()
{}
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_PROXY_H