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

#ifndef ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_H
#define ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_H

#include <vector>
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {
class DisplayResizeListener {
public:
    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    virtual void OnDisplayResized(const Rect &rect, const float scale, const float centerX, const float centerY) = 0;
};

class DisplayResizeController {
public:
    /**
     * @brief The desconstructor of DisplayResizeController.
     */
    virtual ~DisplayResizeController() = default;

    /**
     * @brief Adds a displayresize listener to observe display resizing operations.
     * @param listener Indicates the listener to add.
     * @return
     */
    virtual void AddListener(std::shared_ptr<DisplayResizeListener> &listener) = 0;

    /**
     * @brief Removes the specified displayresize listener.
     * @param listener Indicates the listener to remove.
     * @return
     */
    virtual void DeleteListener(std::shared_ptr<DisplayResizeListener> &listener) = 0;

    /**
     * @brief Obtains the X coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the X coordinate of the center.
     */
    virtual float GetCenterX() = 0;

    /**
     * @brief Obtains the Y coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the Y coordinate of the center.
     */
    virtual float GetCenterY() = 0;

    /**
     * @brief Obtains the display resizing rectangle.
     * @param
     * @return Returns the display resizing rectangle.
     */
    virtual Rect GetDisplayResizeRect() = 0;

    /**
     * @brief Obtains the resizing scale of this display resizing rectangle.
     * @param
     * @return Returns the resizing scale.
     */
    virtual float GetScale() = 0;

    /**
     * @brief Resets the display to its initial size.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the display is successfully reset; returns false otherwise.
     */
    virtual bool Reset(bool isShouldAnimate) = 0;

    /**
     * @brief Sets the center coordinates for the display resizing rectangle.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the center coordinates are successfully set; returns false otherwise.
     */
    virtual bool SetCenter(float centerX, float centerY, bool isShouldAnimate) = 0;

    /**
     * @brief Sets the display resizing scale.
     * @param scale Indicates the scale for resizing the display
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the resizing scale is successfully set; returns false otherwise.
     */
    virtual bool SetScale(float scale, bool isShouldAnimate) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_H