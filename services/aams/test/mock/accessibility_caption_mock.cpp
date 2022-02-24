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

#include "accessibility_caption.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
bool CaptionProperty::CheckProperty(const std::string& property)
{
    bool result = true;
    return result;
}

std::string CaptionProperty::GetBackgroundColor() const
{
    return backgroundColor_;
}

void CaptionProperty::SetBackgroundColor(std::string color)
{
    backgroundColor_ = color;
}

int CaptionProperty::GetFontScale() const
{
    return fontScale_;
}

void CaptionProperty::SetFontScale(int scale)
{
    fontScale_ = scale;
}

std::string CaptionProperty::GetFontColor() const
{
    return fontColor_;
}

void CaptionProperty::SetFontColor(std::string color)
{
    fontColor_ = color;
}

std::string CaptionProperty::GetFontEdgeType() const
{
    return fontEdgeType_;
}

void CaptionProperty::SetFontEdgeType(std::string type)
{
    fontEdgeType_ = type;
}

std::string  CaptionProperty::GetWindowColor() const
{
    return windowColor_;
}

void CaptionProperty::SetWindowColor(std::string  color)
{
    windowColor_ = color;
}

void CaptionProperty::SetFontFamily(std::string family)
{
    fontFamily_ = family;
}

std::string CaptionProperty::GetFontFamily() const
{
    return fontFamily_;
}

bool CaptionProperty::ReadFromParcel(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontFamily_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, fontScale_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontColor_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontEdgeType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundColor_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, windowColor_);
    return true;
}

bool CaptionProperty::Marshalling(Parcel& parcel) const
{
    HILOG_DEBUG("[%{public}s]", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontFamily_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, fontScale_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontColor_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fontEdgeType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, backgroundColor_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, windowColor_);
    return true;
}

CaptionProperty* CaptionProperty::Unmarshalling(Parcel& parcel)
{
    HILOG_DEBUG("[%{public}s]", __func__);
    CaptionProperty* captionProperty = new CaptionProperty();
    if (!captionProperty->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        delete captionProperty;
        captionProperty = nullptr;
    }
    return captionProperty;
}
}  // namespace Accessibility
}  // namespace OHOS