/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#include "accessibility_window_info_parcel.h"
#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityWindowInfoParcel::AccessibilityWindowInfoParcel(const AccessibilityWindowInfo &accessibilityWindowInfo)
    : AccessibilityWindowInfo(accessibilityWindowInfo)
{
}

// LCOV_EXCL_START
bool AccessibilityWindowInfoParcel::ReadFromParcel(Parcel &parcel)
{
    int32_t accessibilityWindowType = TYPE_WINDOW_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, accessibilityWindowType);
    accessibilityWindowType_ = static_cast<AccessibilityWindowType>(accessibilityWindowType);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowMode_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowLayer_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, active_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isDecorEnable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mainWindowId_);
    sptr<RectParcel> boundsInScreen = parcel.ReadStrongParcelable<RectParcel>();
    if (boundsInScreen == nullptr) {
        HILOG_ERROR("ReadStrongParcelable boundsInScreen failed.");
        return false;
    }
    boundsInScreen_ = *boundsInScreen;

    int32_t touchHotAreasSize = 0;
    static const int32_t touchHotAreasMaxSize = 1024;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, touchHotAreasSize);
    if (touchHotAreasSize > touchHotAreasMaxSize) {
        return false;
    }
    std::vector<Rect> touchHotAreas {};
    for (int i = 0; i < touchHotAreasSize; i++) {
        sptr<RectParcel> hotAreaRectParcel = parcel.ReadStrongParcelable<RectParcel>();
        if (hotAreaRectParcel == nullptr) {
            HILOG_ERROR("ReadStrongParcelable hotAreaRect failed.");
            return false;
        }
        touchHotAreas.push_back(*hotAreaRectParcel);
    }
    touchHotAreas_ = touchHotAreas;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint64, parcel, displayId_);

    return true;
}
// LCOV_EXCL_STOP

bool AccessibilityWindowInfoParcel::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(accessibilityWindowType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowMode_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowLayer_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, active_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isDecorEnable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, mainWindowId_);
    RectParcel rectParcel(boundsInScreen_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &rectParcel);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, touchHotAreas_.size());
    for (const auto &hotArea : touchHotAreas_) {
        RectParcel hotAreaRectParcel(hotArea);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &hotAreaRectParcel);
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint64, parcel, displayId_);

    return true;
};

// LCOV_EXCL_START
AccessibilityWindowInfoParcel *AccessibilityWindowInfoParcel::Unmarshalling(Parcel &parcel)
{
    AccessibilityWindowInfoParcel *info = new(std::nothrow) AccessibilityWindowInfoParcel();
    if (info == nullptr) {
        HILOG_ERROR("Failed to create info.");
        return nullptr;
    }
    if (!info->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel failed.");
        delete info;
        info = nullptr;
        return nullptr;
    }
    return info;
}
// LCOV_EXCL_STOP
} // namespace Accessibility
} // namespace OHOS