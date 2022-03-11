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
#include "ability_info.h"
#include "parcel.h"
#include "dummy.h"
#include "parcel_util.h"

using namespace OHOS::AppExecFwk;
using namespace std;

namespace OHOS {
namespace Accessibility {
bool AccessibilityAbilityInfo::Marshalling(Parcel &parcel) const
{
    return true;
}
uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
    abilityTypes_ = 0x00000001;
    return abilityTypes_;
}

string AccessibilityAbilityInfo::GetId()
{
    return bundleName_ + "/" + name_;
}

string AccessibilityAbilityInfo::GetName()
{
    return name_;
}

string AccessibilityAbilityInfo::GetPackageName()
{
    return bundleName_;
}

void AccessibilityAbilityInfo::SetPackageName(string bundleName)
{
    bundleName_ = bundleName;
}

AccessibilityAbilityInfo::AccessibilityAbilityInfo(const ExtensionAbilityInfo &abilityInfo)
{
    // CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_TOUCH_GUIDE |
    // CAPABILITY_ZOOM | CAPABILITY_RETRIEVE | CAPABILITY_GESTURE
    uint32_t capabilities = 0x0008 | 0x0002 | 0x0001 | 0x0020;
    capabilities_ = capabilities;
}

uint32_t AccessibilityAbilityInfo::GetCapabilityValues()
{
    return capabilities_;
}

uint32_t AccessibilityAbilityInfo::GetEventTypes()
{
    return eventTypes_;
}

vector<string> AccessibilityAbilityInfo::GetFilterBundleNames()
{
    return targetBundleNames_;
}

bool AccessibilityAbilityInfo::ParseAAConfig(std::string &config)
{
    return true;
}
} // namespace Accessibility
} // namespace OHOS