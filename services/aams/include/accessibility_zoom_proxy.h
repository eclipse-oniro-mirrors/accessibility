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

#ifndef ACCESSIBILITY_ZOOM_PROXY_H
#define ACCESSIBILITY_ZOOM_PROXY_H

#include <vector>
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
/**
 * @brief Magnification callback function, including zone change, rotation, user change.
 *
 * @since 1.0
 * @version 1.0
 */
class AccessibilityDisplayResizeListener {
public:
    /**
     * @brief
     */
    AccessibilityDisplayResizeListener() {}

    /**
     * @brief
     */
    virtual ~AccessibilityDisplayResizeListener() {}

    /**
     * @brief Callback function when zone change.
     *
     * @param aams  AccessibilityAbilityManagerServicethe reference.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnDisplayResized(Rect &outRegion) {}

    /**
     * @brief Callback function when rotation.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnRotationChanged() {}

    /**
     * @brief Callback function when user changed.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnUserContextChanged() {}
};

class AccessibilityZoomProxy : public RefBase {
public:
    /**
     * @brief AccessibilityZoomProxy instance.
     *
     * @since 1.0
     * @version 1.0
     */
    static AccessibilityZoomProxy &GetInstance()
    {
        static AccessibilityZoomProxy instance_;
        return instance_;
    }

    /**
     * @brief A constructor used to create a magnification controller instance.
     *
     * @since 1.0
     * @version 1.0
     */
    AccessibilityZoomProxy() {}

    /**
     * @brief A destructor used to delete the magnification controller instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~AccessibilityZoomProxy() {}

    /**
     * @brief Register the listener of magnification.
     *
     * @param listener  Reference to the magnification listener.
     * @since 1.0
     * @version 1.0
     */
    void AddListener(AccessibilityDisplayResizeListener &listener) {}

    /**
     * @brief Unregister the listener of magnification.
     *
     * @param listener  Reference to the magnification listener.
     * @since 1.0
     * @version 1.0
     */
    void DeleteListener(AccessibilityDisplayResizeListener &listener) {}

    void Register(int displayId) {}

    void Unregister(int displayId) {}

    bool IsRegistered(int displayId)
    {
        return false;
    }

    bool IsRunning(int displayId)
    {
        return false;
    }

    /**
     * @brief Gets the scale.
     *
     * @param displayId  The logical display id.
     * @return Returns scale value.
     * @since 1.0
     * @version 1.0
     */
    float GetScale(int displayId)
    {
        return 0.0;
    }

    /**
     * @brief Gets the screen-relative X coordinate that is centered.
     *
     * @param displayId  The logical display id.
     * @return Returns the X coordinate of focus.
     * @since 1.0
     * @version 1.0
     */
    float GetCenterX(int displayId)
    {
        return 0.0;
    }

    /**
     * @brief Gets the screen-relative Y coordinate that is centered.
     *
     * @param displayId  The logical display id.
     * @return Returns the Y coordinate of focus.
     * @since 1.0
     * @version 1.0
     */
    float GetCenterY(int displayId)
    {
        return 0.0;
    }

    /**
     * @brief Gets the current magnification area.
     *
     * @param displayId  The logical display id.
     * @return Returns the region of magnification.
     * @since 1.0
     * @version 1.0
     */
    Rect GetDisplayResizeRect(int displayId)
    {
        Rect rc {};
        return rc;
    }

    /**
     * @brief Reset magnification.
     *
     * @param displayId  The logical display id.
     * @return Returns the region of .
     * @since 1.0
     * @version 1.0
     */
    bool Reset(int displayId)
    {
        return false;
    }

    /**
     * @brief Set the magnification center and scale.
     *
     * @param displayId  The logical display id.
     * @param scale  the target scale.
     * @param centerX  The screen-relative X coordinate that is centered and scaled.
     * @param centerY  The screen-relative Y coordinate that is centered and scaled.
     * @since 1.0
     * @version 1.0
     */
    bool SetScaleAndCenter(int displayId, float scale, float centerX, float centerY)
    {
        return false;
    }

private:
    std::vector<AccessibilityDisplayResizeListener> mListeners_;

    /**
     * @brief Set the magnification scale.
     *
     * @param displayId  The logical display id.
     * @param scale  the target scale.
     * @return Returns the region of .
     * @since 1.0
     * @version 1.0
     */
    bool SetScale(int displayId, float scale, float pivotX, float pivotY, bool animate, int id)
    {
        return false;
    }

    /**
     * @brief Set the magnification center.
     *
     * @param displayId  The logical display id.
     * @param centerX  The screen-relative X coordinate that is centered and scaled.
     * @param centerY  The screen-relative Y coordinate that is centered and scaled.
     * @since 1.0
     * @version 1.0
     */
    bool SetCenter(int displayId, float centerX, float centerY, bool animate, int id)
    {
        return false;
    }

    /**
     * @brief Set the magnifying offset.
     *
     * @param displayId  corresponds to one screen.
     * @param offsetX  Offset the distance in the X direction, in current
     *                screen pixels.
     * @param offsetY  Offset the distance in the Y direction, in current
     *                screen pixels.
     * @since 1.0
     * @version 1.0
     */
    void SetDisplayResizeOffset(int displayId, float offsetX, float offsetY) {}
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_ZOOM_PROXY_H
