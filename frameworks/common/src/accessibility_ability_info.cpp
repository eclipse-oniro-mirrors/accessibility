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

#include "accessibility_ability_info.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityAbilityInfo::AccessibilityAbilityInfo(const AccessibilityAbilityInitParams &initParams)
{
    HILOG_DEBUG();

    name_ = initParams.name;
    rationale_ = initParams.rationale;
    bundleName_ = initParams.bundleName;
    moduleName_ = initParams.moduleName;
    description_ = initParams.description;
    staticCapabilities_ = initParams.staticCapabilities;
    settingsAbility_ = initParams.settingsAbility;
    abilityTypes_ = initParams.abilityTypes;
    isImportant_ = initParams.isImportant;

    HILOG_DEBUG("ability name:[%{public}s], bundle name:[%{public}s], module name:[%{public}s],"
        "capabilities:[%{public}d], rationale:[%{public}s], settingsAbility:[%{public}s],"
        "abilityTypes:[%{public}d], isImportant:[%{public}d]",
        name_.c_str(), bundleName_.c_str(), moduleName_.c_str(), capabilities_,
        rationale_.c_str(), settingsAbility_.c_str(), abilityTypes_, isImportant_);
}

uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
    HILOG_DEBUG("types(%{public}d)start.", abilityTypes_);
    return abilityTypes_;
}

uint32_t AccessibilityAbilityInfo::GetCapabilityValues() const
{
    HILOG_DEBUG();
    return capabilities_;
}

uint32_t AccessibilityAbilityInfo::GetStaticCapabilityValues() const
{
    HILOG_DEBUG("staticCapabilities(%{public}d).", staticCapabilities_);
    return staticCapabilities_;
}

const std::string &AccessibilityAbilityInfo::GetDescription() const
{
    HILOG_DEBUG();
    return description_;
}

uint32_t AccessibilityAbilityInfo::GetEventTypes()
{
    HILOG_DEBUG();
    return eventTypes_;
}

std::string AccessibilityAbilityInfo::GetId() const
{
    HILOG_DEBUG();
    return bundleName_ + "/" + name_;
}

const std::string &AccessibilityAbilityInfo::GetName() const
{
    HILOG_DEBUG();
    return name_;
}

const std::string &AccessibilityAbilityInfo::GetPackageName() const
{
    HILOG_DEBUG();
    return bundleName_;
}

const std::string &AccessibilityAbilityInfo::GetModuleName() const
{
    HILOG_DEBUG();
    return moduleName_;
}

void AccessibilityAbilityInfo::SetPackageName(const std::string &bundleName)
{
    HILOG_DEBUG();
    bundleName_ = bundleName;
}

const std::vector<std::string> &AccessibilityAbilityInfo::GetFilterBundleNames() const
{
    HILOG_DEBUG();
    return targetBundleNames_;
}

const std::string &AccessibilityAbilityInfo::GetSettingsAbility() const
{
    HILOG_DEBUG();
    return settingsAbility_;
}

bool AccessibilityAbilityInfo::IsImportant() const
{
    HILOG_DEBUG();
    return isImportant_;
}
} // namespace Accessibility
} // namespace OHOS