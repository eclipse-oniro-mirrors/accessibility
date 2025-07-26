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

#ifndef MAGNIFICATION_WINDOW_H
#define MAGNIFICATION_WINDOW_H

#include "hilog_wrapper.h"
#include "wm/window.h"
#include "window_option.h"
#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <transaction/rs_transaction.h>
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "recording/recording_canvas.h"
#include "magnification_def.h"
#include "display_manager.h"
#include "ext_utils.h"

namespace OHOS {
namespace Accessibility {
class MagnificationWindow {
public:
    // common
    float GetScale();
    PointerPos GetSourceCenter();
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void InitMagnificationParam(float scale);

    // full magnification
    void EnableMagnificationFull(int32_t centerX, int32_t centerY);
    void DisableMagnificationFull(bool needClear = false);
    void SetScaleFull(float scaleSpan);
    void MoveMagnificationFull(int32_t deltaX, int32_t deltaY);
    PointerPos ConvertGesture(uint32_t type, PointerPos coordinates);
    uint32_t CheckTapOnHotArea(int32_t posX, int32_t posY);
    void FollowFocuseElementFull(int32_t centerX, int32_t centerY);
    void ShowMagnificationFull();
    void RefreshWindowParamFull();
    bool IsMagnificationShowFull();

    // window magnification
    void EnableMagnificationPart(int32_t centerX, int32_t centerY);
    void DisableMagnificationPart(bool needClear = false);
    void SetScalePart(float scaleSpan);
    void MoveMagnificationPart(int32_t deltaX, int32_t deltaY);
    bool IsTapOnHotArea(int32_t posX, int32_t posY);
    bool IsTapOnMagnificationWindow(int32_t posX, int32_t posY);
    void FixSourceCenter(bool needFix);
    void FollowFocuseElementPart(int32_t centerX, int32_t centerY);
    void ShowMagnificationPart();
    void RefreshWindowParamPart();
    bool IsMagnificationShowPart();

private:
    // common
    void CreateMagnificationWindow();
    void GetWindowParam();
    PointerPos GetRectCenter(Rosen::Rect rect);
    void FlushImplicitTransaction();
    void DisableMagnification(bool needClear = false);

    // full magnification
    Rosen::Rect GetSourceRectFromPointer(int32_t centerX, int32_t centerY);
    void UpdateAnchor();
    void DrawRuoundRectFrameFull();

    // window magnification
    Rosen::Rect GetWindowRectFromPointer(int32_t centerX, int32_t centerY);
    void DrawRuoundRectFramePart();
    void CalculateAnchorOffset();
    void UpdateRelativeRect();
    void AdjustMagnificationWindowPosition();
    void AdjustSourceWindowPosition();

    // common
    sptr<Rosen::Window> window_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_ = nullptr;
    std::shared_ptr<Rosen::RSUIContext> rsUIContext_ = nullptr;
    Rosen::Rect sourceRect_ = {0, 0, 0, 0};
    Rosen::Rect windowRect_ = {0, 0, 0, 0};
    Rosen::Rect screenRect_ = {0, 0, 0, 0};
    Rosen::Rect relativeRect_ = {0, 0, 0, 0}; //The relative position of sourceRect_ in windowRect_
    uint32_t screenWidth_ = 0;
    uint32_t screenHeight_ = 0;
    float screenSpan_ = 0.0f;
    uint64_t screenId_ = 0;
    int32_t centerX_ = 0;
    int32_t centerY_ = 0;
    float scale_ = DEFAULT_SCALE;

    uint32_t windowWidth_ = MAGNIFY_WINDOW_WIDTH;
    uint32_t windowHeight_ = MAGNIFY_WINDOW_HEIGHT;
    uint32_t barMargin_ = BAR_MARGIN;

    uint32_t hotAreaWidth_ = HOT_AREA_WIDTH;
    int32_t anchorOffsetX_ = 0; // windowRect_'s centerX - sourceRect_'s centerX
    int32_t anchorOffsetY_ = 0; // windowRect_'s centerY - sourceRect_'s centerY

    bool isFixSourceCenter_ = false;
    PointerPos fixedSourceCenter_ = {0, 0};
    bool isMagnificationShowPart_ = false;
    bool isMagnificationShowFull_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_WINDOW_H