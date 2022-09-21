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

#ifndef MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_STUB_H
#define MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_STUB_H

#include <gmock/gmock.h>
#include "accessible_ability_manager_config_observer_stub.h"
#include "accessible_ability_manager_caption_observer_stub.h"
#include "accessibility_enable_ability_lists_observer_stub.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityManagerConfigObserverStub : public AccessibleAbilityManagerConfigObserverStub {
public:
    MockAccessibleAbilityManagerConfigObserverStub();
    virtual ~MockAccessibleAbilityManagerConfigObserverStub() = default;

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD1(OnConfigStateChanged, void(const uint32_t stateType));
    MOCK_METHOD1(OnAudioBalanceChanged, void(const float audioBalance));
    MOCK_METHOD1(OnBrightnessDiscountChanged, void(const float brightnessDiscount));
    MOCK_METHOD1(OnContentTimeoutChanged, void(const uint32_t contentTimeout));
    MOCK_METHOD1(OnDaltonizationColorFilterChanged, void(const uint32_t filterType));
    MOCK_METHOD1(OnMouseAutoClickChanged, void(const int32_t mouseAutoClick));
    MOCK_METHOD1(OnShortkeyTargetChanged, void(const std::string &shortkeyTarget));
};

class MockAccessibleAbilityManagerCaptionObserverStub : public AccessibleAbilityManagerCaptionObserverStub {
public:
    MockAccessibleAbilityManagerCaptionObserverStub();
    virtual ~MockAccessibleAbilityManagerCaptionObserverStub() = default;

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD1(OnPropertyChanged, void(const AccessibilityConfig::CaptionProperty &property));
};

class MockAccessibilityEnableAbilityListsObserverStub : public AccessibilityEnableAbilityListsObserverStub {
public:
    MockAccessibilityEnableAbilityListsObserverStub();
    virtual ~MockAccessibilityEnableAbilityListsObserverStub() = default;

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD0(OnAccessibilityEnableAbilityListsChanged, void());
};


MockAccessibleAbilityManagerConfigObserverStub::MockAccessibleAbilityManagerConfigObserverStub()
{}

MockAccessibleAbilityManagerCaptionObserverStub::MockAccessibleAbilityManagerCaptionObserverStub()
{}

MockAccessibilityEnableAbilityListsObserverStub::MockAccessibilityEnableAbilityListsObserverStub()
{}
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CONFIG_OBSERVER_STUB_H