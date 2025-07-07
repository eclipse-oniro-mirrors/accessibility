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

#ifndef FULL_SCREEN_MAGNIFICATION_MANAGER_H
#define FULL_SCREEN_MAGNIFICATION_MANAGER_H

#include "window.h"
#include "window_option.h"
#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <transaction/rs_transaction.h>
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "recording/recording_canvas.h"
#include "hilog_wrapper.h"
#include "magnification_def.h"
#include "ffrt.h"

namespace OHOS {
namespace Accessibility {

class FullScreenMagnificationManager {
public:
    FullScreenMagnificationManager();
    ~FullScreenMagnificationManager() = default;

    void CreateMagnificationWindow();
    void DrawRuoundRectFrame();
    void EnableMagnification(int32_t centerX, int32_t centerY);
    void ShowMagnification();
    void ShowMagnificationWithPosition(PointerPos pos);
    void DisableMagnification(bool needClear = false);
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void RefreshWindowParam();
    void SetScale(float scaleSpan);
    void MoveMagnification(int32_t deltaX, int32_t deltaY);
    void PersistScale();
    PointerPos ConvertGesture(uint32_t type, PointerPos coordinates);
    uint32_t CheckTapOnHotArea(int32_t posX, int32_t posY);
    void FollowFocuseElement(int32_t centerX, int32_t centerY);
    PointerPos GetSourceCenter();
    inline bool IsMagnificationWindowShow()
    {
        return isMagnificationWindowShow_;
    }
private:
    void GetWindowParam();
    void InitMagnificationParam();
    Rosen::Rect GetSourceRectFromPointer(int32_t centerX, int32_t centerY);
    void UpdateAnchor();
    PointerPos GetRectCenter(Rosen::Rect rect);
    void FlushImplicitTransaction();
    sptr<Rosen::Window> window_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_ = nullptr;
    std::shared_ptr<Rosen::RSUIContext> rsUIContext_ = nullptr;
    bool isMagnificationWindowShow_ = false;

    Rosen::Rect sourceRect_ = {0, 0, 0, 0};
    Rosen::Rect windowRect_ = {0, 0, 0, 0};
    Rosen::Rect screenRect_ = {0, 0, 0, 0};
    uint32_t screenWidth_ = 0;
    uint32_t screenHeight_ = 0;
    float screenSpan_ = 0.0f;
    uint64_t screenId_ = 0;
    Rosen::DisplayOrientation orientation_ = Rosen::DisplayOrientation::UNKNOWN;
    int32_t centerX_ = 0;
    int32_t centerY_ = 0;
    float scale_ = 2.0f;
    ffrt::mutex mutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // FULL_SCREEN_MAGNIFICATION_MANAGER_H