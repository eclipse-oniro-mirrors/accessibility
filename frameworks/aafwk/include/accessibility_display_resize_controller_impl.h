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

#ifndef ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_IMPL_H
#define ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_IMPL_H

#include <vector>
#include "accessibility_display_resize_controller.h"
#include "accessible_ability_channel_client.h"

namespace OHOS {
namespace Accessibility {
class DisplayResizeControllerImpl : public DisplayResizeController {
public:
    /**
     * @brief The constructor of DisplayResizeController.
     * @param displayId The id of display.
     * @param client The client of channel.
     */
    explicit DisplayResizeControllerImpl(const int32_t displayId,
        const std::shared_ptr<AccessibleAbilityChannelClient> &client);

    /**
     * @brief The desconstructor of DisplayResizeController.
     */
    ~DisplayResizeControllerImpl() = default;

    /**
     * @brief Adds a displayresize listener to observe display resizing operations.
     * @param listener Indicates the listener to add.
     * @return
     */
    virtual void AddListener(std::shared_ptr<DisplayResizeListener> &listener) override;

    /**
     * @brief Removes the specified displayresize listener.
     * @param listener Indicates the listener to remove.
     * @return
     */
    virtual void DeleteListener(std::shared_ptr<DisplayResizeListener> &listener) override;

    /**
     * @brief Obtains the X coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the X coordinate of the center.
     */
    virtual float GetCenterX() override;

    /**
     * @brief Obtains the Y coordinate of the center in this display resizing rectangle.
     * @param
     * @return Returns the Y coordinate of the center.
     */
    virtual float GetCenterY() override;

    /**
     * @brief Obtains the display resizing rectangle.
     * @param
     * @return Returns the display resizing rectangle.
     */
    virtual Rect GetDisplayResizeRect() override;

    /**
     * @brief Obtains the resizing scale of this display resizing rectangle.
     * @param
     * @return Returns the resizing scale.
     */
    virtual float GetScale() override;

    /**
     * @brief Resets the display to its initial size.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the display is successfully reset; returns false otherwise.
     */
    virtual bool Reset(bool isShouldAnimate) override;

    /**
     * @brief Sets the center coordinates for the display resizing rectangle.
     * @param centerX Indicates the X coordinate of the center for resizing the display.
     * @param centerY Indicates the Y coordinate of the center for resizing the display.
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the center coordinates are successfully set; returns false otherwise.
     */
    virtual bool SetCenter(float centerX, float centerY, bool isShouldAnimate) override;

    /**
     * @brief Sets the display resizing scale.
     * @param scale Indicates the scale for resizing the display
     * @param isShouldAnimate Specifies whether animation is required.
     * @return Returns true if the resizing scale is successfully set; returns false otherwise.
     */
    virtual bool SetScale(float scale, bool isShouldAnimate) override;

    /**
     * @brief Dispatch displayresize changes to listeners who has been registered.
     * @param rect The rectangle of displayresize which needs to dispatch.
     * @param scale The scale of displayresize which needs to dispatch.
     * @param centerX The centerX of displayresize which needs to dispatch.
     * @param centerY The centerY of displayresize which needs to dispatch.
     * @return
     */
    void DispatchOnDisplayResized(const Rect& rect, const float scale, const float centerX, const float centerY);

private:
    int32_t displayId_ = 0;
    std::shared_ptr<AccessibleAbilityChannelClient> channelClient_ = nullptr;
    std::vector<std::shared_ptr<DisplayResizeListener>> displayResizeListeners_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_DISPLAY_RESIZE_CONTROLLER_IMPL_H