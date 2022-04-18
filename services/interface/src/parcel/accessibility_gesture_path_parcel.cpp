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

#include "accessibility_gesture_path_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityGesturePathParcel::AccessibilityGesturePathParcel(const AccessibilityGesturePath &gesturePath)
{
    HILOG_DEBUG("start.");
    AccessibilityGesturePath *self = this;
    *self = gesturePath;
}

bool AccessibilityGesturePathParcel::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("start.");
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, startPosition_.positionX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, startPosition_.positionY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, endPosition_.positionX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, endPosition_.positionY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, durationTime_);
    return true;
}

bool AccessibilityGesturePathParcel::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("start.");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, startPosition_.positionX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, startPosition_.positionY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, endPosition_.positionX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, endPosition_.positionY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, durationTime_);
    return true;
}

sptr<AccessibilityGesturePathParcel> AccessibilityGesturePathParcel::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG("start.");
    sptr<AccessibilityGesturePathParcel> path = new AccessibilityGesturePathParcel();
    if (!path || !path->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel AccessibilityGesturePathParcel failed.");
        path = nullptr;
        return nullptr;
    }
    return path;
}
} // namespace Accessibility
} // namespace OHOS
