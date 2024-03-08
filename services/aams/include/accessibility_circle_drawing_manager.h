/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_CIRCLE_DRAWING_MANAGER_H
#define ACCESSIBILITY_CIRCLE_DRAWING_MANAGER_H

#include <string>
#include "draw/canvas.h"
#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <ui/rs_ui_director.h>
#include <ui/rs_root_node.h>
#include <transaction/rs_transaction.h>

#include "accessibility_def.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityCircleDrawingManager {
public:
    static std::shared_ptr<AccessibilityCircleDrawingManager> pointDrawMgr_;
    static std::shared_ptr<AccessibilityCircleDrawingManager> GetInstance();
    static void DeleteInstance();

    AccessibilityCircleDrawingManager();
    DISALLOW_COPY_AND_MOVE(AccessibilityCircleDrawingManager);
    ~AccessibilityCircleDrawingManager();

    void UpdatePointerVisible(bool state);
    void CreatePointerWindow(int32_t physicalX, int32_t physicalY, uint64_t screenId);
    void DrawPointer(int32_t physicalX, int32_t physicalY, int32_t angle, uint64_t screenId);
    void SetPointerLocation(int32_t physicalX, int32_t physicalY, uint64_t screenId);
    void DrawingProgress(int32_t physicalX, int32_t physicalY, int32_t angle);

    int32_t ParseStringToInteger(const std::string& color);
    std::vector<int32_t> ParseColorString(const std::string& color);

#ifndef USE_ROSEN_DRAWING
    void DrawingProgressByOpenSource(int32_t physicalX, int32_t physicalY, int32_t angle);
#else
    void DrawingProgressByRosenDrawing(int32_t physicalX, int32_t physicalY, int32_t angle);
#endif

    std::shared_ptr<Rosen::RSUIDirector> rsUiDirector_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_ = nullptr;
    std::shared_ptr<Rosen::RSNode> rootNode_ = nullptr;
    uint64_t screenId_;
    int32_t imageWidth_;
    int32_t imageHeight_;
    int32_t half_;
    float dispalyDensity_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_CIRCLE_DRAWING_MANAGER_H