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

#ifndef ACCESSIBILITY_CAPTION_H
#define ACCESSIBILITY_CAPTION_H

#include "accessibility_def.h"

namespace OHOS {
namespace AccessibilityConfig {
class CaptionProperty {
public:
    CaptionProperty() = default;
    ~CaptionProperty() = default;

    bool CheckProperty(const std::string& property);

    void SetFontFamily(std::string family);
    std::string GetFontFamily() const;

    int32_t GetFontScale() const;
    void SetFontScale(int32_t scale);

    void SetFontColor(uint32_t color);
    uint32_t GetFontColor() const;

    void SetFontEdgeType(std::string type);
    std::string  GetFontEdgeType() const;

    void SetWindowColor(uint32_t color);
    uint32_t GetWindowColor() const;

    void SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;

protected:
    std::string fontFamily_ = "default";
    int32_t fontScale_ = 75;    // font size
    uint32_t fontColor_ = 0xff000000;
    std::string fontEdgeType_ = "none";
    uint32_t backgroundColor_ = 0xff000000;
    uint32_t windowColor_ = 0xff000000;

private:
    bool HasBackgroundColor();
    bool HasTextColor();
    bool HasEdgeType();
    bool HasEdgeColor();
    bool HasWindowColor();
};

enum CaptionObserverType : int32_t {
    CAPTION_ENABLE = 0,
    CAPTION_PROPERTY,
};
class CaptionObserver {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~CaptionObserver() = default;

    /**
     * @brief Called when the caption property changed.
     * @param caption current caption property.
     * @return
     */
    virtual void OnStateChanged(const bool& enable) = 0;
    virtual void OnPropertyChanged(const CaptionProperty& caption) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_CAPTION_H