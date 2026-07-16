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

#ifndef ACCESSIBILITY_VIRTUAL_NODE_PARCEL_H
#define ACCESSIBILITY_VIRTUAL_NODE_PARCEL_H

#include "accessibility_element_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityVirtualNodeParcel : public AccessibilityVirtualNode, public Parcelable {
public:
    AccessibilityVirtualNodeParcel() = default;

    explicit AccessibilityVirtualNodeParcel(const AccessibilityVirtualNode &virtualNode);

    bool ReadFromParcel(Parcel &parcel);

    bool Marshalling(Parcel &parcel) const override;

    static AccessibilityVirtualNodeParcel *Unmarshalling(Parcel &parcel);

private:
    bool ReadBasicInfoFromParcel(Parcel &parcel);
    bool ReadBoolPropertiesFromParcel(Parcel &parcel);
    bool ReadExtraInfoFromParcel(Parcel &parcel);
    bool ReadChildNodesFromParcel(Parcel &parcel);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_VIRTUAL_NODE_PARCEL_H
