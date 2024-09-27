/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <vector>
#include "accessibility_def.h"
#include "accessibility_ability_info.h"

namespace OHOS {
namespace Accessibility {
struct MockAccessibilityAbilityInitParams {
    std::string bundleName = "";
    std::string description = "";
    std::string moduleName = "";
    std::string name = "";
    std::string rationale = "";
    std::string settingsAbility = "";
    std::string label = "";
    uint32_t staticCapabilities = 0;
    uint32_t abilityTypes = 0;
    bool isImportant = false;
    bool needHide = false;
};

class MockAccessibilityAbilityInfo : public AccessibilityAbilityInfo {
public:
    MockAccessibilityAbilityInfo() = default;

    ~MockAccessibilityAbilityInfo() = default;

    MockAccessibilityAbilityInfo(const AccessibilityAbilityInitParams &initParams);

    /**
     * @brief Obtains the types of the accessible ability.
     * @return Return the types of the accessible ability.
     */
    uint32_t GetAccessibilityAbilityType();

    /**
     * @brief Obtains the types of the capabilities.
     * @return Return the types of the capabilities.
     */
    uint32_t GetCapabilityValues() const;

    /**
     * @brief Obtains the description of the accessible ability.
     * @return Return the description of the accessible ability.
     */
    const std::string &GetDescription() const;

    /**
     * @brief Obtains the types of the accessible events.
     * @return Return the types of the accessible events.
     */
    uint32_t GetEventTypes();

    /**
     * @brief Obtains the id of the accessible ability.
     * @return Return the id of the accessible ability.
     */
    std::string GetId() const;

    /**
     * @brief Obtains the name of the accessible ability.
     * @return Return the name of the accessible ability.
     */
    const std::string &GetName() const;

    /**
     * @brief Obtains the package name of the accessible ability.
     * @return Return the package name of the accessible ability.
     */
    const std::string &GetPackageName() const;

    /**
     * @brief Obtains the module name of the accessible ability.
     * @return Return the module name of the accessible ability.
     */
    const std::string &GetModuleName() const;

    /**
     * @brief Set the package name of the accessible ability.
     * @param bundleName the package name of the accessible ability
     */
    void SetPackageName(const std::string &bundleName);

    /**
     * @brief Obtains the target bundles's name that you are listening on.
     * @return Return the target bundles's name that you are listening on.
     */
    const std::vector<std::string> &GetFilterBundleNames() const;

    /**
     * @brief Obtains the setting ability of the accessible ability.
     * @return Return the setting ability of the accessible ability.
     */
    const std::string &GetSettingsAbility() const;

    /**
     * @brief Set the target bundles's name that you want to listening on.
     * @param targetBundleNames the target bundle name to set.
     */
    inline void SetFilterBundleNames(const std::vector<std::string> &targetBundleNames)
    {
        targetBundleNames_ = targetBundleNames;
    }

    /**
     * @brief Set the types of the capabilities.
     * @param capabilities the capabilities to set.
     */
    inline void SetCapabilityValues(uint32_t capabilities)
    {
        capabilities_ = capabilities;
    }

    /**
     * @brief Set the types of the ability.
     * @param abilityTypes the ability types to set.
     */
    inline void SetAccessibilityAbilityType(uint32_t abilityTypes)
    {
        abilityTypes_ = abilityTypes;
    }

    /**
     * @brief Set the types of the event.
     * @param eventTypes the event to set.
     */
    inline void SetEventTypes(uint32_t eventTypes)
    {
        eventTypes_ = eventTypes;
    }

    /**
     * @brief Obtains if the ability is important.
     * @return Return if the ability is important.
     */
    bool IsImportant() const;

    /**
     * @brief Obtains the capability types of static configuration.
     * @return Return the capability types of static configuration.
     */
    uint32_t GetStaticCapabilityValues() const;

    /**
     * @brief Obtains if the ability is need to hide.
     * @return Return true means hide the ability, return false means show the ability.
     */
    bool NeedHide() const;

    /**
     * @brief Obtains the label of the accessible ability.
     * @return Return the label of the accessible ability.
     */
    const std::string &GetLabel() const;

protected:
    std::string bundleName_;
    std::string moduleName_;
    std::string name_;
    std::string description_;
    std::string label_;

    uint32_t staticCapabilities_ = 0;
    uint32_t capabilities_ = 0;
    std::string rationale_ = "";
    std::string settingsAbility_;

    uint32_t abilityTypes_ = ACCESSIBILITY_ABILITY_TYPE_INVALID;
    uint32_t eventTypes_ = 0;

    std::vector<std::string> targetBundleNames_;
    bool isImportant_ = false;
    bool needHide_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_INFO_H