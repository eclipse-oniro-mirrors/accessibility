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

#include "accessibility_gesture_inject_path_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityGestureInjectPathParcel::AccessibilityGestureInjectPathParcel(
    const AccessibilityGestureInjectPath &gesturePath)
{
    HILOG_DEBUG();
    AccessibilityGestureInjectPath *self = this;
    *self = gesturePath;
}

bool AccessibilityGestureInjectPathParcel::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG();

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, durationTime_);
    int32_t positionSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, positionSize);
    for (auto i = 0; i < positionSize; i++) {
        AccessibilityGesturePosition position;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, position.positionX_);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, position.positionY_);
        AddPosition(position);
    }
    return true;
}

bool AccessibilityGestureInjectPathParcel::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG();

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, durationTime_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, positions_.size());
    for (auto &position : positions_) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, position.positionX_);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, position.positionY_);
    }

    return true;
}

sptr<AccessibilityGestureInjectPathParcel> AccessibilityGestureInjectPathParcel::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG();
    sptr<AccessibilityGestureInjectPathParcel> path = new(std::nothrow) AccessibilityGestureInjectPathParcel();
    if (!path) {
        HILOG_ERROR("Failed to create path.");
        return nullptr;
    }
    if (!path->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel AccessibilityGestureInjectPathParcel failed.");
        path = nullptr;
        return nullptr;
    }
    return path;
}
} // namespace Accessibility
} // namespace OHOS
