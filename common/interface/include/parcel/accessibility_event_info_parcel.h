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

#ifndef ACCESSIBILITY_EVENT_INFO_PARCEL_H
#define ACCESSIBILITY_EVENT_INFO_PARCEL_H

#include "accessibility_event_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
class ExtraEventInfoParcel : public ExtraEventInfo, public Parcelable {
public:
    /**
     * @brief Construct
     * @sysCap Accessibility
     */
    ExtraEventInfoParcel() = default;
 
    /**
     * @brief Construct
     * @param extraEventInfo The object of ExtraEventInfo.
     * @sysCap Accessibility
     */
    explicit ExtraEventInfoParcel(const ExtraEventInfo &extraEventInfo);
 
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);
 
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;
 
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @sysCap Accessibility
     */
    static ExtraEventInfoParcel *Unmarshalling(Parcel &parcel);
};

class AccessibilityEventInfoParcel : public AccessibilityEventInfo, public Parcelable {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfoParcel() {}

    /**
     * @brief Construct
     * @param eventInfo The object of AccessibilityEventInfo.
     * @since 3
     * @sysCap Accessibility
     */
    explicit AccessibilityEventInfoParcel(const AccessibilityEventInfo &eventInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @sysCap Accessibility
     */
    bool MarshallingFirstPart(Parcel &parcel) const;
 
    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @sysCap Accessibility
     */
    bool MarshallingSecondPart(Parcel &parcel) const;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibilityEventInfoParcel from parcel data
     * @since 3
     * @sysCap Accessibility
     */
    static AccessibilityEventInfoParcel *Unmarshalling(Parcel &parcel);

private:
    /**
     * @brief Used for IPC communication first part
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     * @sysCap Accessibility
     */
    bool ReadFromParcelFirstPart(Parcel &parcel);

    /**
     * @brief Used for IPC communication second part
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     * @sysCap Accessibility
     */
    bool ReadFromParcelSecondPart(Parcel &parcel);

    /**
     * @brief Used for IPC communication third part
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     * @sysCap Accessibility
     */
    bool ReadFromParcelThirdPart(Parcel &parcel);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EVENT_INFO_PARCEL_H
