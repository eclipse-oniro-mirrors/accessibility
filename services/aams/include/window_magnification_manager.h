/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef WINDOW_MAGNIFICATION_MANAGER_H
#define WINDOW_MAGNIFICATION_MANAGER_H

#include "window.h"
#include "window_option.h"
#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <transaction/rs_transaction.h>
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "recording/recording_canvas.h"
#include "hilog_wrapper.h"
#include "magnification_def.h"
#include "ffrt.h"

namespace OHOS {
namespace Accessibility {

struct RectBound {
    uint32_t width;
    uint32_t height;
};

class WindowMagnificationManager {
public:
    WindowMagnificationManager();
    ~WindowMagnificationManager() = default;

    void OnMagnificationModeChanged(uint32_t magnificationMode);

    inline bool IsMagnificationWindowShow()
    {
        return isMagnificationWindowShow_;
    }
    inline float GetScale()
    {
        return scale_;
    }
    
    void CreateMagnificationWindow(int32_t posX, int32_t posY);
    void DrawRuoundRectFrame();
    void EnableWindowMagnification(int32_t centerX, int32_t centerY);
    void ShowWindowMagnification();
    void DisableWindowMagnification(bool needClear = false);
    void SetScale(float deltaSpan);
    void MoveMagnificationWindow(int32_t deltaX, int32_t deltaY);
    void InitMagnificationParam();
    bool IsTapOnHotArea(int32_t posX, int32_t posY);
    bool IsTapOnMagnificationWindow(int32_t posX, int32_t posY);
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void FixSourceCenter(bool needFix);
    void PersistScale();
    void RefreshWindowParam();
    void FollowFocuseElement(int32_t centerX, int32_t centerY);

private:
    PointerPos ConvertCenterToTopLeft(int32_t centerX, int32_t centerY);
    RectBound GenRectBound(uint32_t width, uint32_t height);
    Rosen::Rect GetSourceRectFromPointer(int32_t centerX, int32_t centerY);
    Rosen::Rect GetWindowRectFromPointer(int32_t centerX, int32_t centerY);
    void AdjustMagnificationWindowPosition();
    void AdjustSourceWindowPosition();
    void CalculateAnchorOffset();
    void GetWindowParam();
    PointerPos GetRectCenter(Rosen::Rect rect);
    void UpdateRelativeRect();

    Rosen::Rect sourceRect_ = {0, 0, 0, 0};
    Rosen::Rect windowRect_ = {0, 0, 0, 0};
    Rosen::Rect screenRect_ = {0, 0, 0, 0};
    Rosen::Rect relativeRect_ = {0, 0, 0, 0}; //The relative position of sourceRect_ in windowRect_

    sptr<Rosen::Window> window_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_;
    bool isMagnificationWindowShow_ = false;

    uint32_t windowWidth_ = 940;
    uint32_t windowHeight_ = 550;
    uint32_t barMargin_ = 40;
    float scale_ = 2.0f;
    uint32_t hotAreaWidth_ = 60;
    int32_t anchorOffsetX_ = 0; // windowRect_'s centerX - sourceRect_'s centerX
    int32_t anchorOffsetY_ = 0; // windowRect_'s centerY - sourceRect_'s centerY

    uint32_t screenWidth_ = 0;
    uint32_t screenHeight_ = 0;
    float screenSpan_ = 0.0f;
    uint64_t screenId_ = 0;
    Rosen::DisplayOrientation orientation_ = Rosen::DisplayOrientation::UNKNOWN;
    bool isFixSourceCenter_ = false;
    PointerPos fixedSourceCenter_ = {0, 0};
    ffrt::mutex mutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // WINDOW_MAGNIFICATION_MANAGER_H