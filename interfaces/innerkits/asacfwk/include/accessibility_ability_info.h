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

#ifndef ACCESSIBILITY_ABILILTY_INFO_H
#define ACCESSIBILITY_ABILILTY_INFO_H

#include <vector>
#include <string>
#include <stdint.h>

#include "accessibility_event_info.h"
#include "extension_ability_info.h"
#include "nlohmann/json.hpp"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {

    // The capability types of the accessible ability.
    enum Capability : uint32_t {
        CAPABILITY_RETRIEVE = 0x0001,
        CAPABILITY_TOUCH_GUIDE = 0x0002,
        CAPABILITY_KEY_EVENT_OBSERVER = 0x0008,
        CAPABILITY_ZOOM = 0x0010,
        CAPABILITY_GESTURE = 0x0020,
    };

    // The accessibility ability types for feedbacks.
    enum AccessibilityAbilityTypes : uint32_t {
        ACCESSIBILITY_ABILITY_TYPE_INVALID = 0x00000000,
        ACCESSIBILITY_ABILITY_TYPE_SPOKEN = 0x00000001,
        ACCESSIBILITY_ABILITY_TYPE_HAPTIC = 0x00000002,
        ACCESSIBILITY_ABILITY_TYPE_AUDIBLE = 0x00000004,
        ACCESSIBILITY_ABILITY_TYPE_VISUAL = 0x00000008,
        ACCESSIBILITY_ABILITY_TYPE_GENERIC = 0x00000010,
        ACCESSIBILITY_ABILITY_TYPE_ALL = 0xFFFFFFFF,
    };

class AccessibilityAbilityInfo : public Parcelable {
public:

    AccessibilityAbilityInfo() = default;
    ~AccessibilityAbilityInfo() = default;

    AccessibilityAbilityInfo(AppExecFwk::ExtensionAbilityInfo abilityInfo);

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

    void SetPackageName(std::string bundleName);

    /**
     * @brief Obtains the target bundles's name that you are listening on.
     * @param
     * @return Return the target bundles's name that you are listening on.
     */
    std::vector<std::string> GetFilterBundleNames();

    /**
     * @brief Obtains the setting ability of the accessible ability.
     * @param
     * @return Return the setting ability of the accessible ability.
     */
    std::string GetSettingsAbility();

    /**
     * @brief read this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return true if read successfully, else return false.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this sequenceable object into a Parcel.
     * @param parcel Indicates the Parcel object to which the sequenceable object will be marshaled.
     * @return Return true if Marshal successfully, else return false.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return a sequenceable object of AccessibilityAbilityInfo.
     */
    static AccessibilityAbilityInfo *Unmarshalling(Parcel &parcel);

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

private:
    /**
     * @brief Parse config files of the accessible ability.
     * @param
     * @return Return true if parses config files successfully, else return false.
     */
    bool ParseAAConfig(nlohmann::json sourceJson);

    std::string bundleName_;
    std::string moduleName_;
    std::string name_;
    std::string description_;

    uint32_t capabilities_ = 0;
    std::string rationale_ = "";

    uint32_t abilityTypes_ = ACCESSIBILITY_ABILITY_TYPE_INVALID;
    uint32_t eventTypes_ = EventType::TYPE_VIEW_INVALID;
    std::string settingsAbility_;
    std::vector<std::string> targetBundleNames_;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBILITY_ABILITY_INFO_H