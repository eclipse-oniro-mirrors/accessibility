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

#ifndef ACCESSIBILITY_CAPTION_H
#define ACCESSIBILITY_CAPTION_H

#include <string>
#include "parcel.h"

namespace OHOS {
namespace Accessibility {

enum CaptionMode : int {
    CAPTION_BLACK_WHITE = 0,  // CAPTION_background_foreground
    CAPTION_WHITE_BLACK,
    CAPTION_BLACK_YELLOW,
    CAPTION_BLUE_YELLOW,
    CAPTION_CUSTOM,
    CAPTION_MODE_MAX,
};

enum CaptionEdge : int {
    CAPTION_EDGE_NONEB = 0,
    CAPTION_EDGE_OUTLINE,
    CAPTION_EDGE_DROP_SHADOW,
    CAPTION_EDGE_RAISED,
    CAPTION_EDGE_DEPRESSED,
};

class CaptionProperty : public Parcelable {
public:
    CaptionProperty() = default;
    ~CaptionProperty() = default;
    bool CheckProperty(const std::string& property);

    void SetFontFamily(std::string family);
    std::string GetFontFamily() const;

    int GetFontScale() const;
    void SetFontScale(int scale);

    void SetFontColor(std::string color);
    std::string GetFontColor() const;

    void SetFontEdgeType(std::string  type);
    std::string  GetFontEdgeType() const;

    void SetWindowColor(std::string color);
    std::string GetWindowColor() const;

    void SetBackgroundColor(std::string color);
    std::string GetBackgroundColor() const;

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
    virtual bool Marshalling(Parcel& parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable
     * object has been marshaled.
     * @return Return a sequenceable object of CaptionProperty.
     */
    static CaptionProperty* Unmarshalling(Parcel& parcel);

private:
    bool HasBackgroundColor();
    bool HasTextColor();
    bool HasEdgeType();
    bool HasEdgeColor();
    bool HasWindowColor();

    std::string fontFamily_ = "default";
    int fontScale_ = 75;    // font size
    std::string fontColor_ = "";     //#0055AA
    std::string fontEdgeType_ = "none";
    std::string backgroundColor_ = "";
    std::string windowColor_ = "";

};

class CaptionObserver {
public:
    /**
     * @brief Called when the caption property changed.
     * @param caption current caption property.
     * @return
     */
    virtual void OnCaptionPropertyChanged(const CaptionProperty& caption) = 0;
};

}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_CAPTION_H