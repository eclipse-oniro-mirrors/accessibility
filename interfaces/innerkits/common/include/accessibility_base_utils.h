/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_BASE_UTILS_H
#define ACCESSIBILITY_BASE_UTILS_H

#include "accessibility_caption.h"
#include "accessibility_def.h"

namespace OHOS {
namespace Accessibility {
    AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(const std::string &type);
    AbilityStateType ConvertStringToAbilityStateType(const std::string &type);
    bool CheckStateType(const std::string &stateType);
    std::vector<std::string> ParseAbilityTypesToVec(uint32_t abilityTypesValue);
    std::vector<std::string> ParseCapabilitiesToVec(uint32_t capabilitiesValue);
    std::vector<std::string> ParseEventTypesToVec(uint32_t eventTypesValue);
    bool CheckAbilityType(const std::string& abilityType);
    uint32_t ParseCapabilitiesFromVec(const std::vector<std::string> &capabilities);
    OHOS::AccessibilityConfig::DALTONIZATION_TYPE ConvertStringToDaltonizationTypes(std::string& type);
    std::string ConvertDaltonizationTypeToString(OHOS::AccessibilityConfig::DALTONIZATION_TYPE type);
    OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME ConvertStringToClickResponseTimeTypes(std::string& type);
    std::string ConvertClickResponseTimeTypeToString(OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME type);
    OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME ConvertStringToIgnoreRepeatClickTimeTypes(std::string& type);
    std::string ConvertIgnoreRepeatClickTimeTypeToString(OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME type);
    uint32_t ConvertColorStringToNumber(std::string colorStr);
    std::string ConvertColorToString(uint32_t color);
    bool ConvertColorStringToUint32(std::string &str, uint32_t &value);
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_BASE_UTILS_H