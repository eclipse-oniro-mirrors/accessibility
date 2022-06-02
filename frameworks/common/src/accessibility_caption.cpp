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

#include "accessibility_caption.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AccessibilityConfig {
bool CaptionProperty::CheckProperty(const std::string &property)
{
    bool result = true;
    return result;
}

uint32_t CaptionProperty::GetBackgroundColor() const
{
    return backgroundColor_;
}

void CaptionProperty::SetBackgroundColor(uint32_t color)
{
    backgroundColor_ = color;
}

int32_t CaptionProperty::GetFontScale() const
{
    return fontScale_;
}

void CaptionProperty::SetFontScale(int32_t scale)
{
    fontScale_ = scale;
}

uint32_t CaptionProperty::GetFontColor() const
{
    return fontColor_;
}

void CaptionProperty::SetFontColor(uint32_t color)
{
    fontColor_ = color;
}

const std::string &CaptionProperty::GetFontEdgeType() const
{
    return fontEdgeType_;
}

void CaptionProperty::SetFontEdgeType(const std::string &type)
{
    fontEdgeType_ = type;
}

uint32_t CaptionProperty::GetWindowColor() const
{
    return windowColor_;
}

void CaptionProperty::SetWindowColor(uint32_t color)
{
    windowColor_ = color;
}

void CaptionProperty::SetFontFamily(const std::string &family)
{
    fontFamily_ = family;
}

const std::string &CaptionProperty::GetFontFamily() const
{
    return fontFamily_;
}
} // namespace Accessibility
} // namespace OHOS