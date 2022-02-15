/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "parcel.h"
#include "parcel_util.h"
#include "dummy.h"

using namespace OHOS::Accessibility;
using namespace OHOS::AppExecFwk;
using namespace std;

bool AccessibilityAbilityInfo::Marshalling(Parcel &parcel) const
{
    return true;
}
uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
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

AccessibilityAbilityInfo::AccessibilityAbilityInfo(ExtensionAbilityInfo abilityInfo)
{
    AccessibilityAbilityInfoDummy accessibilityAbilityInfoDummydata;
    // CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM | CAPABILITY_RETRIEVE | CAPABILITY_GESTURE
    uint32_t capabilities = 0x0008 | 0x0002 | 0x0010 | 0x0001 | 0x0020;
    capabilities_ = capabilities;
    targetBundleNames_ = accessibilityAbilityInfoDummydata.filterBundleNames;
    eventTypes_ = accessibilityAbilityInfoDummydata.accessibilityEventTypes;
    name_ = abilityInfo.name;
    bundleName_ = abilityInfo.bundleName;
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