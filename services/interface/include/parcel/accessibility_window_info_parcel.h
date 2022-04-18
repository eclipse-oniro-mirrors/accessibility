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

#ifndef ACCESSIBILITY_WINDOW_INFO_PARCEL_H
#define ACCESSIBILITY_WINDOW_INFO_PARCEL_H

#include "accessibility_window_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowInfoParcel : public AccessibilityWindowInfo, public Parcelable {
public:
    AccessibilityWindowInfoParcel() = default;
    AccessibilityWindowInfoParcel(const AccessibilityWindowInfo &accessibilityWindowInfo);

    /**
     * @brief Used for IPC communication.
     * @param parcel Serializable data.
     * @return true: Read parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief  Used for IPC communication.
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief  Used for IPC communication.
     * @param parcel
     * @return The data of AccessibilityWindowInfoParcel
     * @since 3
     * @sysCap Accessibility
     */
    static sptr<AccessibilityWindowInfoParcel> Unmarshalling(Parcel &parcel);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_INFO_PARCEL_H
