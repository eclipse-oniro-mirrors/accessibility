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

#ifndef ACCESSIBILITY_ABILITY_INFO_PARCEL_H
#define ACCESSIBILITY_ABILITY_INFO_PARCEL_H

#include "accessibility_ability_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityAbilityInfoParcel : public AccessibilityAbilityInfo, public Parcelable {
public:
    AccessibilityAbilityInfoParcel() = default;
    AccessibilityAbilityInfoParcel(const AccessibilityAbilityInfo &accessibilityAbilityInfo);

    /**
     * @brief read this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return true if read successfully, else return false.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this sequenceable object into a Parcel.
     * @param parcel Indicates the Parcel object to which the sequenceable object will be marshaled.
     * @return Return true if Marshal successfully, else return false.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return a sequenceable object of AccessibilityAbilityInfo.
     */
    static sptr<AccessibilityAbilityInfoParcel> Unmarshalling(Parcel &parcel);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_INFO_PARCEL_H