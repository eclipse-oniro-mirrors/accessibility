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
    /**
     * @brief The constructor of CaptionProperty
     */
    CaptionProperty() = default;

    /**
     * @brief The deconstructor of CaptionProperty
     */
    ~CaptionProperty() = default;

    /**
     * @brief Check property
     * @param property The value of property
     */
    bool CheckProperty(const std::string &property);

    /**
     * @brief Set font family
     * @param family Font family
     */
    void SetFontFamily(const std::string &family);

    /**
     * @brief Get font family
     * @return Font family
     */
    const std::string &GetFontFamily() const;

    /**
     * @brief Set font scale
     * @param scale Font scale
     */
    void SetFontScale(int32_t scale);

    /**
     * @brief Get font scale
     * @return Font scale
     */
    int32_t GetFontScale() const;

    /**
     * @brief Set font color
     * @param color Font color
     */
    void SetFontColor(uint32_t color);

    /**
     * @brief Get font color
     * @return Font color
     */
    uint32_t GetFontColor() const;

    /**
     * @brief Set font edge type
     * @param type The type of font edge
     */
    void SetFontEdgeType(const std::string &type);

    /**
     * @brief Get font edge type
     * @return The type of font edge
     */
    const std::string &GetFontEdgeType() const;

    /**
     * @brief Set window color
     * @param color The color of window
     */
    void SetWindowColor(uint32_t color);

    /**
     * @brief Get window color
     * @return The color of window
     */
    uint32_t GetWindowColor() const;

    /**
     * @brief Set background color
     * @param color The color of background
     */
    void SetBackgroundColor(uint32_t color);

    /**
     * @brief Get background color
     * @return The color of background
     */
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
     * @brief The deconstructor of CaptionObserver
     */
    virtual ~CaptionObserver() = default;

    /**
     * @brief Called when the accessibility state changed.
     * @param enable true:the accessibility state is enabled;false:the accessibility state is disabled
     */
    virtual void OnStateChanged(const bool& enable) = 0;

    /**
     * @brief Called when the caption property changed.
     * @param caption current caption property.
     */
    virtual void OnPropertyChanged(const CaptionProperty& caption) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_CAPTION_H