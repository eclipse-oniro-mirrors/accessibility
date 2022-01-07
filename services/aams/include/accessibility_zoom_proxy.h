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

#ifndef ACCESSIBILITY_ZOOM_PROXY_H
#define ACCESSIBILITY_ZOOM_PROXY_H

// #include "accessibility_ability_manager_service.h"
#include <vector>
#include "accessibility_zoom_mock.h"
#include "refbase.h"
// #include "gfx_utils/rect.h"

namespace OHOS {
namespace Accessibility {
/**
 * @brief Magnification callback function, including zone change, rotation, user change.
 *
 * @since 1.0
 * @version 1.0
 */
// WMS需要定义的接口, 目前WMS还没有这些功能和接口.
// 需要包括下面的几种情况：区域改变，旋转，用户变更.
class AccessibilityDisplayResizeListener /*: public WMS***.DisplayResizeCallbacks*/ {
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
    virtual void OnDisplayResizeChanged(Rect &outRegion) {}

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
        static AccessibilityZoomProxy instance_ {};
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

    // 假定鸿蒙也有display、windows，activity的对应关系.
    // android: 一个activity对应一个window，一个display对应一个屏幕，一个display可以有多个windows.
    // 放大的效果是整个屏幕放大,不是某个window. 如果进行应用切换，即windows的切换，放大要保持不变.
    // android: 当放大时会启动一个放大图层，处于最外层。和别的图层最终合成一个屏幕.
    //
    // 下面内容不确定，display不确定，待定.
    // -----------------------------------------
    void Register(int displayId) {}
    void Unregister(int displayId) {}
    bool IsRegistered(int displayId) { return false; }
    bool IsRunning(int displayId) { return false; }
    // -----------------------------------------


    /**
     * @brief Gets the scale.
     *
     * @param displayId  The logical display id.
     * @return Returns scale value.
     * @since 1.0
     * @version 1.0
     */
    float GetScale(int displayId) { return 0.0; }

    /**
     * @brief Gets the screen-relative X coordinate that is centered.
     *
     * @param displayId  The logical display id.
     * @return Returns the X coordinate of focus.
     * @since 1.0
     * @version 1.0
     */
    float GetCenterX(int displayId) { return 0.0; }

    /**
     * @brief Gets the screen-relative Y coordinate that is centered.
     *
     * @param displayId  The logical display id.
     * @return Returns the Y coordinate of focus.
     * @since 1.0
     * @version 1.0
     */
    float GetCenterY(int displayId) { return 0.0; }

    /**
     * @brief Gets the current magnification area.
     *
     * @param displayId  The logical display id.
     * @return Returns the region of magnification.
     * @since 1.0
     * @version 1.0
     */
    Rect GetDisplayResizeRect(int displayId) { Rect rc {}; return rc; }

    /**
     * @brief Reset magnification.
     *
     * @param displayId  The logical display id.
     * @return Returns the region of .
     * @since 1.0
     * @version 1.0
     */
    bool Reset(int displayId) { return false; }

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
    bool SetScaleAndCenter(int displayId, float scale, float centerX, float centerY) { return false; }

private:
    std::vector<AccessibilityDisplayResizeListener> mListeners_ {};

    /**
     * @brief Set the magnification scale.
     *
     * @param displayId  The logical display id.
     * @param scale  the target scale.
     * @return Returns the region of .
     * @since 1.0
     * @version 1.0
     */
    // 下面两个函数参数需要参考display屏幕设置的参数，需要屏幕生效，需要确认鸿蒙：屏幕、window的关系和api.
    // 还需要确认鸿蒙下，动画的API使用，动画与WMS之间的关系，是否有关联.
    // 动画与放大需要联动.
    // 参数：需要不需要带x，y坐标，动画是否执行，需要依照WMS的实现来定。目前WMS尚不支持该功能.
    bool SetScale(int displayId, float scale, float pivotX, float pivotY,
        bool animate, int id) 
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
    bool SetCenter(int displayId, float centerX, float centerY, bool animate, int id) { return false; }

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

private:
    // 动画使用什么控件？是否需要 继承动画事件？
/*
    class AccessibilityAnimationListener : public ValueAnimator.AnimatorUpdateListener {
    private:
        explicit AnimationListener(int displayId);
        virtual ~AnimationListener();
        void onAnimationUpdate(ValueAnimator animation);
    };
*/
};
}  // namespace accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_ZOOM_PROXY_H
