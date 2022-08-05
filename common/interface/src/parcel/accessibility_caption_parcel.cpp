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

#include "accessibility_caption_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
CaptionPropertyParcel::CaptionPropertyParcel(const CaptionProperty &property)
{
    HILOG_DEBUG();

    AccessibilityConfig::CaptionProperty *self = this;
    *self = property;
}

bool CaptionPropertyParcel::ReadFromParcel(Parcel& parcel)
{
    HILOG_DEBUG();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontFamily_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, fontScale_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, fontColor_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontEdgeType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, backgroundColor_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowColor_);

    return true;
}

bool CaptionPropertyParcel::Marshalling(Parcel& parcel) const
{
    HILOG_DEBUG();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontFamily_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, fontScale_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, fontColor_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontEdgeType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, backgroundColor_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, windowColor_);

    return true;
}

sptr<CaptionPropertyParcel> CaptionPropertyParcel::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG();
    sptr<CaptionPropertyParcel> captionProperty = new(std::nothrow) CaptionPropertyParcel();
    if (!captionProperty) {
        HILOG_ERROR("Failed to create captionProperty.");
        return nullptr;
    }
    if (!captionProperty->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return captionProperty;
}
} // namespace Accessibility
} // namespace OHOS