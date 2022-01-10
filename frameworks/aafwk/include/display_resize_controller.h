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

#ifndef DISPLAY_RESIZE_CONTROLLER_H
#define DISPLAY_RESIZE_CONTROLLER_H

#include <vector>
#include <stdint.h>
#include "accessible_ability_client_stub_impl.h"
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {

class DisplayResizeListener;

class DisplayResizeController {
public:
    /**
     * @brief The constructor of DisplayResizeController.
     * @param channelId The id of channel.
     * @param displayId The id of display.
     */
    DisplayResizeController(uint32_t channelId, uint32_t displayId);

    /**
     * @brief Adds a displayresize listener to observe display resizing operations.
     * @param listener Indicates the listener to add.
     * @return
     */
    void AddListener(std::shared_ptr<DisplayResizeListener>& listener);

    /**
     * @brief Removes the specified displayresize listener.
     * @param listener Indicates the listener to remove.
     * @return Return true if removing listener successfully, else return false.
     */
    bool DeleteListener(std::shared_ptr<DisplayResizeListener>& listener);

    /**
     * @brief Obtains the X coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the X coordinate of the center.
     */
    float GetCenterX();

    /**
     * @brief Obtains the Y coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the Y coordinate of the center.
     */
    float GetCenterY();

    /**
     * @brief Obtains the display resizing rectangle.
     * @param
     * @return Returns the display resizing rectangle.
     */
    Rect GetDisplayResizeRect();

    /**
     * @brief Obtains the resizing scale of this display resizing rectangle.
     * @param
     * @return Returns the resizing scale.
     */
    float GetScale();

    /**
     * @brief Resets the display to its initial size.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the display is successfully reset; returns false otherwise.
     */
    bool Reset(bool isShouldAnimate);

    /**
     * @brief Sets the center coordinates for the display resizing rectangle.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the center coordinates are successfully set; returns false otherwise.
     */
    bool SetCenter(float centerX, float centerY, bool isShouldAnimate);

    /**
     * @brief Sets the display resizing scale.
     * @param scale Indicates the scale for resizing the display
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the resizing scale is successfully set; returns false otherwise.
     */
    bool SetScale(float scale, bool isShouldAnimate);

    /**
     * @brief Dispatch displayresize changes to listeners who has been registered.
     * @param rect The rectangle of displayresize which needs to dispatch.
     * @param scale The scale of displayresize which needs to dispatch.
     * @param centerX The centerX of displayresize which needs to dispatch.
     * @param centerY The centerY of displayresize which needs to dispatch.
     * @return
     */
    void DispatchOnDisplayResizeChanged(const Rect& rect, const float scale, const float centerX, const float centerY);

private:
    std::vector<std::shared_ptr<DisplayResizeListener>> displayResizeListeners_ {};
    uint32_t channelId_ = AccessibleAbilityClientStubImpl::INVALID_CHANNEL_ID;
    uint32_t displayId_;
};

class DisplayResizeListener {
public:
    /**
     * @brief Called when the rectangle, scale, or center coordinate for performing the resizing operations is changed.
     * @param controller Indicates the controller for display resizing operations.
     * @param rect Indicates the rectangle for resizing the display.
     * @param scale Indicates the scale for resizing the display.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @return
     */
    virtual void OnDisplayResizeChanged(const DisplayResizeController& controller, const Rect& rect, const float scale,
                                            const float centerX, const float centerY) = 0;
};

} // namespace Accessibility
} // namespace OHOS

#endif // DISPLAY_RESIZE_CONTROLLER_H