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

#ifndef ACCESSIBILITY_CAPTION_PARCEL_H
#define ACCESSIBILITY_CAPTION_PARCEL_H

#include "accessibility_caption.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
class CaptionPropertyParcel : public AccessibilityConfig::CaptionProperty, public Parcelable {
public:
    CaptionPropertyParcel() = default;
    explicit CaptionPropertyParcel(const AccessibilityConfig::CaptionProperty &property);
    /**
     * @brief read this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable
     * object has been marshaled.
     * @return Return true if read successfully, else return false.
     */
    bool ReadFromParcel(Parcel& parcel);

    /**
     * @brief Marshals this sequenceable object into a Parcel.
     * @param parcel Indicates the Parcel object to which the sequenceable
     * object will be marshaled.
     * @return Return true if Marshal successfully, else return false.
     */
    bool Marshalling(Parcel& parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable
     * object has been marshaled.
     * @return Return a sequenceable object of CaptionPropertyParcel.
     */
    static CaptionPropertyParcel *Unmarshalling(Parcel& parcel);
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_CAPTION_H