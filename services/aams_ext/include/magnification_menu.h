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

#ifndef MAGNIFICATION_MENU_H
#define MAGNIFICATION_MENU_H

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
#include "image_source.h"
#include "pixel_map.h"
#include "magnification_def.h"
#include "display_manager.h"
#include "ext_utils.h"

namespace OHOS {
namespace Accessibility {

class MagnificationMenu {
public:
    // magnification menu
    void ShowMenuWindow(uint32_t mode);
    void DisableMenuWindow();
    bool IsTapOnMenu(int32_t posX, int32_t posY);
    void MoveMenuWindow(int32_t deltaX, int32_t deltaY);
    void SetCurrentType(uint32_t type);
    void AttachToEdge();
    void RefreshWindowParam();
    uint32_t ChangeMode();
    bool IsMenuShown();

private:
    void GetWindowParam();
    void CreateMenuWindow();
    void LoadMenuBgImage(uint32_t mode);
    void AdjustMenuPosition();
    void FlushImplicitTransaction();
    std::shared_ptr<Media::PixelMap> DecodePixelMap(const std::string& pathName,
        const Media::AllocatorType& allocatorType);
    //menu window
    sptr<Rosen::Window> menuWindow_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_ = nullptr;
    std::shared_ptr<Rosen::RSUIContext> rsUIContext_ = nullptr;
    Media::AllocatorType allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap_ = nullptr;
    std::shared_ptr<Rosen::RSImage> rosenImage_ = nullptr;

    Rosen::Rect menuRect_ = {0, 0, 0, 0};
    Rosen::Rect screenRect_ = {0, 0, 0, 0};

    uint32_t menuSize_ = 150;
    int32_t margin_ = 50;
    uint64_t screenId_ = 0;
    uint32_t screenWidth_ = 0;
    uint32_t screenHeight_ = 0;

    uint32_t menuMode_ = 0;
    uint32_t currentType_ = 0;
    bool isMenuShown_ = false;
};

} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_MENU_H