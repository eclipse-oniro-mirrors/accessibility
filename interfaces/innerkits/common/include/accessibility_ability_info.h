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

#ifndef ACCESSIBILITY_ABILITY_INFO_H
#define ACCESSIBILITY_ABILITY_INFO_H

#include <vector>
#include "accessibility_def.h"

namespace OHOS {
namespace Accessibility {
struct AccessibilityAbilityInitParams {
    std::string bundleName = "";
    std::string description = "";
    std::string moduleName = "";
    std::string name = "";
    std::string rationale = "";
    std::string settingsAbility = "";
    uint32_t capabilities = 0;
    uint32_t abilityTypes = ACCESSIBILITY_ABILITY_TYPE_INVALID;
};

class AccessibilityAbilityInfo {
public:
    /**
     * @brief The constructor of AccessibilityAbilityInfo.
     */
    AccessibilityAbilityInfo() = default;

    /**
     * @brief The desconstructor of AccessibilityAbilityInfo.
     */
    ~AccessibilityAbilityInfo() = default;

    /**
     * @brief The constructor of AccessibilityAbilityInfo.
     * @param initParams The params to init AccessibilityAbilityInfo.
     */
    AccessibilityAbilityInfo(const AccessibilityAbilityInitParams &initParams);

    /**
     * @brief Obtains the types of the accessible ability.
     * @param
     * @return Return the type of the accessible ability.
     */
    uint32_t GetAccessibilityAbilityType();

    /**
     * @brief Obtains the types of the capabilities.
     * @param
     * @return Return the types of the capabilities.
     */
    uint32_t GetCapabilityValues();

    /**
     * @brief Obtains the description of the accessible ability.
     * @param
     * @return Return the description of the accessible ability.
     */
    std::string GetDescription();

    /**
     * @brief Obtains the type of the accessible events.
     * @param
     * @return Return the type of the accessible events.
     */
    uint32_t GetEventTypes();

    /**
     * @brief Obtains the id of the accessible ability.
     * @param
     * @return Return the id of the accessible ability.
     */
    std::string GetId();

    /**
     * @brief Obtains the name of the accessible ability.
     * @param
     * @return Return the name of the accessible ability.
     */
    std::string GetName();

    /**
     * @brief Obtains the package name of the accessible ability.
     * @param
     * @return Return the package name of the accessible ability.
     */
    std::string GetPackageName();

    /**
     * @brief Obtains the package name of the accessible ability.
     * @param bundleName the package name of the accessible ability
     * @return -
     */
    void SetPackageName(const std::string &bundleName);

    /**
     * @brief Obtains the target bundles's name that you are listening on.
     * @param
     * @return Return the target bundles's name that you are listening on.
     */
    const std::vector<std::string> &GetFilterBundleNames() const;

    /**
     * @brief Obtains the setting ability of the accessible ability.
     * @param
     * @return Return the setting ability of the accessible ability.
     */
    std::string GetSettingsAbility();

    /**
     * @brief Set the target bundles's name that you want to listening on.
     * @param targetBundleNames the target bundle name to set.
     * @return
     */
    inline void SetFilterBundleNames(const std::vector<std::string> &targetBundleNames)
    {
        targetBundleNames_ = targetBundleNames;
    }

    /**
     * @brief Set the types of the capabilities.
     * @param capabilities the capabilities to set.
     * @return
     */
    inline void SetCapabilityValues(uint32_t capabilities)
    {
        capabilities_ = capabilities;
    }

    /**
     * @brief Set the types of the ability.
     * @param abilityTypes the ability to set.
     * @return
     */
    inline void SetAccessibilityAbilityType(uint32_t abilityTypes)
    {
        abilityTypes_ = abilityTypes;
    }

    /**
     * @brief Set the types of the event.
     * @param eventTypes the event to set.
     * @return
     */
    inline void SetEventTypes(uint32_t eventTypes)
    {
        eventTypes_ = eventTypes;
    }

protected:
    std::string bundleName_;
    std::string moduleName_;
    std::string name_;
    std::string description_;

    uint32_t capabilities_ = 0;
    std::string rationale_ = "";
    std::string settingsAbility_;

    uint32_t abilityTypes_ = ACCESSIBILITY_ABILITY_TYPE_INVALID;
    uint32_t eventTypes_ = EventType::TYPES_ALL_MASK;

    std::vector<std::string> targetBundleNames_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_INFO_H