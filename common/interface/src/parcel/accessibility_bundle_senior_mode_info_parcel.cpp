/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "accessibility_bundle_senior_mode_info_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityBundleSeniorModeInfoParcel::AccessibilityBundleSeniorModeInfoParcel(
    const AccessibilityConfig::AccessibilityBundleSeniorModeInfo &info)
    : AccessibilityConfig::AccessibilityBundleSeniorModeInfo(info)
{
}

bool AccessibilityBundleSeniorModeInfoParcel::ReadFromParcel(Parcel& parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, seniorModeState_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, appIndex_);

    return true;
}

bool AccessibilityBundleSeniorModeInfoParcel::Marshalling(Parcel& parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, seniorModeState_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, appIndex_);

    return true;
}

AccessibilityBundleSeniorModeInfoParcel *AccessibilityBundleSeniorModeInfoParcel::Unmarshalling(
    Parcel& parcel)
{
    AccessibilityBundleSeniorModeInfoParcel *info = new(std::nothrow) AccessibilityBundleSeniorModeInfoParcel();
    if (info == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityBundleSeniorModeInfoParcel.");
        return nullptr;
    }
    if (!info->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete info;
        info = nullptr;
        return nullptr;
    }
    return info;
}
} // namespace Accessibility
} // namespace OHOS