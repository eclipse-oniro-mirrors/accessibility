/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MOCK_ACCESSIBILITY_ABILITY_INFO_H
#define MOCK_ACCESSIBILITY_ABILITY_INFO_H

#include <gmock/gmock.h>
#include "accessibility_ability_info.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityAbilityInfo : public AccessibilityAbilityInfo {
public:
    MockAccessibilityAbilityInfo() = default;
    ~MockAccessibilityAbilityInfo() = default;
    MockAccessibilityAbilityInfo(const AppExecFwk::ExtensionAbilityInfo& abilityInfo)
    {}
    MOCK_METHOD0(GetAccessibilityAbilityType, uint32_t());
    MOCK_METHOD0(GetCapabilityValues, uint32_t());
    MOCK_METHOD0(GetDescription, std::string());
    MOCK_METHOD0(GetEventTypes, uint32_t());
    MOCK_METHOD0(GetId, std::string());
    MOCK_METHOD0(GetName, std::string());
    MOCK_METHOD0(GetPackageName, std::string());
    MOCK_METHOD1(SetPackageName, void(const std::string &bundleName));
    MOCK_METHOD0(GetFilterBundleNames, const std::vector<std::string> &());
    MOCK_METHOD0(GetSettingsAbility, std::string());
    MOCK_METHOD1(ReadFromParcel, bool(Parcel& parcel));
    MOCK_CONST_METHOD1(Marshalling, bool(Parcel& parcel));
    MOCK_METHOD1(Unmarshalling, AccessibilityAbilityInfo*(Parcel& parcel));
    inline void SetCapabilityValues(uint32_t capabilities)
    {}

    inline void SetAccessibilityAbilityType(uint32_t abilityTypes)
    {}

    inline void SetEventTypes(uint32_t eventTypes)
    {}
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ABILITY_INFO_H