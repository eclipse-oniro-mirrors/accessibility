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

#include "accessibility_circle_drawing_manager.h"
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "hilog_wrapper.h"
#include "pipeline/rs_recording_canvas.h"
#include "recording/recording_canvas.h"

namespace OHOS {
namespace Accessibility {

constexpr int32_t DEFAULT_VALUE = -1;

constexpr int32_t OUT_CIRCLE_RADIUS = 49; // outer circle
constexpr int32_t PROGRESS_RADIUS = 46; // progress circle background
constexpr int32_t CENTER_CIRCLE_RADIUS = 6; // center circle
constexpr int32_t PROGRESS_STROKE_WIDTH = 2; // progress stroke

constexpr int32_t OUT_CIRCLE_BACKGROUND_ALPHA = 64;
constexpr int32_t PROGRESS_BACKGROUND_ALPHA = 153;
constexpr int32_t CENTER_CIRCLE_BACKGROUND_ALPHA = 230;
constexpr int32_t PROGRESS_BAR_BACKGROUND_ALPHA = 230;

const std::string PROGRESS_BACKGROUND_COLOR_TYPE = "#0A59F7"; // progress circle background
const std::string PROGRESS_COLOR_TYPE = "#FFFFFF"; // center circle
const std::string OUT_CIRCLE_BACKGROUND_COLOR_TYPE = "#1756C4"; // outer circle

constexpr int32_t DEFAULT_WIDTH = 500;
constexpr int32_t DEFAULT_HEIGHT = 500;
constexpr int32_t DEFAULT_HALF = 2;

constexpr int32_t DEFAULT_PIXEL_DENSITY = 160;

constexpr int32_t HEXADECIMAL = 16;
constexpr int32_t DECIMAL = 10;
constexpr char CHAR_0 = '0';
constexpr char CHAR_9 = '9';
constexpr char CHAR_A = 'A';
constexpr char CHAR_B = 'B';
constexpr char CHAR_C = 'C';
constexpr char CHAR_D = 'D';
constexpr char CHAR_E = 'E';
constexpr char CHAR_F = 'F';

constexpr int32_t COLOR_STRING_LENGTH = 2;
constexpr int32_t RED_START_INDEX = 1;
constexpr int32_t GREEN_START_INDEX = 3;
constexpr int32_t BLUE_START_INDEX = 5;

constexpr int32_t NUMBER_0 = 0;
constexpr int32_t NUMBER_1 = 1;
constexpr int32_t NUMBER_2 = 2;

constexpr int32_t START_ANGLE = -90; // start angle
constexpr int32_t INVALID_NODE_ID = -1;

std::shared_ptr<AccessibilityCircleDrawingManager> AccessibilityCircleDrawingManager::pointDrawMgr_ = nullptr;

std::shared_ptr<AccessibilityCircleDrawingManager> AccessibilityCircleDrawingManager::GetInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        pointDrawMgr_ = std::make_shared<AccessibilityCircleDrawingManager>();
    }
    return pointDrawMgr_;
}

void AccessibilityCircleDrawingManager::DeleteInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }
    pointDrawMgr_ = nullptr;
}

AccessibilityCircleDrawingManager::AccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
    imageWidth_ = DEFAULT_WIDTH;
    imageHeight_ = DEFAULT_HEIGHT;
    half_ = DEFAULT_WIDTH / DEFAULT_HALF;

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    screenId_ = displayMgr.GetDefaultDisplayId(); // default screenId 0
    auto dpi = displayMgr.GetDefaultDisplayDpi();
    dispalyDensity_ = static_cast<float>(dpi) / DEFAULT_PIXEL_DENSITY;
#else
    HILOG_DEBUG("not support display manager");
    screenId_ = 0;
    dispalyDensity_ = 1;
#endif
}

AccessibilityCircleDrawingManager::~AccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    surfaceNode_->ClearChildren();
    surfaceNode_->DetachToDisplay(screenId_);
    surfaceNode_ = nullptr;
    canvasNode_ = nullptr;
    if (rsUiDirector_ != nullptr) {
        rsUiDirector_->SetRoot(INVALID_NODE_ID);
    }

    rootNode_ = nullptr;
    rsUiDirector_ = nullptr;
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void AccessibilityCircleDrawingManager::UpdatePointerVisible(bool state)
{
    HILOG_DEBUG("state %{public}s", state ? "true" : "false");
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    surfaceNode_->SetVisible(state);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void AccessibilityCircleDrawingManager::CreatePointerWindow(int32_t physicalX, int32_t physicalY, uint64_t screenId)
{
    rsUiDirector_ = Rosen::RSUIDirector::Create();
    if (rsUiDirector_ == nullptr) {
        return;
    }

    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "screen touch progress";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("create surfaceNode_ fail");
        return;
    }

    rsUiDirector_->Init();
    surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode_->SetBounds(physicalX - half_, physicalY - half_, imageWidth_, imageHeight_);
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT); // USE_ROSEN_DRAWING
    screenId_ = screenId;
    surfaceNode_->AttachToDisplay(screenId);
    surfaceNode_->SetRotation(0);
    std::shared_ptr<Rosen::RectF> drawRect = std::make_shared<Rosen::RectF>(physicalX - half_, physicalY - half_,
        imageWidth_, imageHeight_);
    surfaceNode_->SetDrawRegion(drawRect);

    rootNode_ = Rosen::RSRootNode::Create();
    if (rootNode_ == nullptr) {
        return;
    }

    rootNode_->SetBounds(0, 0, imageWidth_, imageHeight_);
    rootNode_->SetFrame(0, 0, imageWidth_, imageHeight_);
    rootNode_->SetBackgroundColor(SK_ColorTRANSPARENT); // USE_ROSEN_DRAWING
    rootNode_->SetCornerRadius(1);
    rootNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    rootNode_->SetRotation(0);

    canvasNode_ = Rosen::RSCanvasNode::Create();
    if (canvasNode_ == nullptr) {
        HILOG_ERROR("create canvasNode_ fail");
        return;
    }

    rootNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    canvasNode_->SetBounds(0, 0, imageWidth_, imageHeight_);
    canvasNode_->SetFrame(0, 0, imageWidth_, imageHeight_);
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT); // USE_ROSEN_DRAWING
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);

    rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
    rsUiDirector_->SetRoot(rootNode_->GetId());
    Rosen::RSTransaction::FlushImplicitTransaction();
}

#ifndef USE_ROSEN_DRAWING
void AccessibilityCircleDrawingManager::DrawingProgressByOpenSource(int32_t physicalX, int32_t physicalY, int32_t angle)
{
    HILOG_DEBUG();
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));

    // outer circle
    SkPaint outCirclePaint;
    outCirclePaint.setAntiAlias(true);
    outCirclePaint.setAlphaf(OUT_CIRCLE_BACKGROUND_ALPHA);

    std::vector<int32_t> outCircleColor = ParseColorString(OUT_CIRCLE_BACKGROUND_COLOR_TYPE);
    outCirclePaint.setARGB(OUT_CIRCLE_BACKGROUND_ALPHA, outCircleColor[NUMBER_0], outCircleColor[NUMBER_1],
        outCircleColor[NUMBER_2]);
    outCirclePaint.setStyle(SkPaint::kFill_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * OUT_CIRCLE_RADIUS, outCirclePaint);

    // center circle
    SkPaint centerCirclePaint;
    centerCirclePaint.setAntiAlias(true);
    centerCirclePaint.setAlphaf(CENTER_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> centerCircleColor = ParseColorString(PROGRESS_COLOR_TYPE);
    centerCirclePaint.setARGB(CENTER_CIRCLE_BACKGROUND_ALPHA, centerCircleColor[NUMBER_0],
        centerCircleColor[NUMBER_1], centerCircleColor[NUMBER_2]);
    centerCirclePaint.setStyle(SkPaint::kFill_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * CENTER_CIRCLE_RADIUS, centerCirclePaint);

    // progress circle
    SkPaint progressCirclePaint;
    progressCirclePaint.setAntiAlias(true);
    progressCirclePaint.setAlphaf(PROGRESS_BACKGROUND_ALPHA);
    std::vector<int32_t> progressCircleColor = ParseColorString(PROGRESS_BACKGROUND_COLOR_TYPE);
    progressCirclePaint.setARGB(PROGRESS_BACKGROUND_ALPHA, progressCircleColor[NUMBER_0],
        progressCircleColor[NUMBER_1], progressCircleColor[NUMBER_2]);
    progressCirclePaint.setStrokeWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);
    progressCirclePaint.setStyle(SkPaint::kStroke_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * PROGRESS_RADIUS, progressCirclePaint);

    // progress arc
    SkPaint progressArcPaint;
    progressArcPaint.setAntiAlias(true);
    progressArcPaint.setAlphaf(PROGRESS_BAR_BACKGROUND_ALPHA);
    std::vector<int32_t> progressArcColor = ParseColorString(PROGRESS_COLOR_TYPE);
    progressArcPaint.setARGB(PROGRESS_BAR_BACKGROUND_ALPHA, progressArcColor[NUMBER_0],
        progressArcColor[NUMBER_1], progressArcColor[NUMBER_2]);
    progressArcPaint.setStrokeWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);
    progressArcPaint.setStyle(SkPaint::kStroke_Style);

    SkRect arcRect = SkRect::MakeLTRB(half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS);
    canvas->drawArc(arcRect, START_ANGLE, angle, false, progressArcPaint);

    canvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
}
#else
void AccessibilityCircleDrawingManager::DrawingProgressByRosenDrawing(int32_t physicalX, int32_t physicalY,
    int32_t angle)
{
    HILOG_DEBUG();
    auto canvas = canvasNode_->BeginRecording(imageWidth_, imageHeight_);

    // outer circle
    Rosen::Drawing::Brush outCircleBrush;
    outCircleBrush.SetAntiAlias(true);
    outCircleBrush.SetAlphaF(OUT_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> outCircleColor = ParseColorString(OUT_CIRCLE_BACKGROUND_COLOR_TYPE);
    outCircleBrush.SetARGB(OUT_CIRCLE_BACKGROUND_ALPHA, outCircleColor[NUMBER_0], outCircleColor[NUMBER_1],
        outCircleColor[NUMBER_2]);

    canvas->AttachBrush(outCircleBrush);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * OUT_CIRCLE_RADIUS);
    canvas->DetachBrush();

    // center circle
    Rosen::Drawing::Brush centerCircleBrush;
    centerCircleBrush.SetAntiAlias(true);
    centerCircleBrush.SetAlphaF(CENTER_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> centerCircleColor = ParseColorString(PROGRESS_COLOR_TYPE);
    centerCircleBrush.SetARGB(CENTER_CIRCLE_BACKGROUND_ALPHA, centerCircleColor[NUMBER_0],
        centerCircleColor[NUMBER_1], centerCircleColor[NUMBER_2]);

    canvas->AttachBrush(centerCircleBrush);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * CENTER_CIRCLE_RADIUS);
    canvas->DetachBrush();

    // progress circle
    Rosen::Drawing::Pen progressPen;
    progressPen.SetAntiAlias(true);
    progressPen.SetAlphaF(PROGRESS_BACKGROUND_ALPHA);
    std::vector<int32_t> progressCircleColor = ParseColorString(PROGRESS_BACKGROUND_COLOR_TYPE);
    progressPen.SetARGB(PROGRESS_BACKGROUND_ALPHA, progressCircleColor[NUMBER_0],
        progressCircleColor[NUMBER_1], progressCircleColor[NUMBER_2]);
    progressPen.SetWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);

    canvas->AttachPen(progressPen);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * PROGRESS_RADIUS);
    canvas->DetachPen();

    // progress arc
    Rosen::Drawing::Pen progressArcPen;
    progressArcPen.SetAntiAlias(true);
    progressArcPen.SetAlphaF(PROGRESS_BAR_BACKGROUND_ALPHA);
    std::vector<int32_t> progressArcColor = ParseColorString(PROGRESS_COLOR_TYPE);
    progressArcPen.SetARGB(PROGRESS_BAR_BACKGROUND_ALPHA, progressArcColor[NUMBER_0],
        progressArcColor[NUMBER_1], progressArcColor[NUMBER_2]);
    progressArcPen.SetWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);

    Rosen::Drawing::Rect rect(half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS);
    canvas->AttachPen(progressArcPen);
    canvas->DrawArc(rect, START_ANGLE, angle);
    canvas->DetachPen();

    canvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
}
#endif

void AccessibilityCircleDrawingManager::DrawingProgress(int32_t physicalX, int32_t physicalY, int32_t angle)
{
#ifndef USE_ROSEN_DRAWING
    DrawingProgressByOpenSource(physicalX, physicalY, angle);
#else
    DrawingProgressByRosenDrawing(physicalX, physicalY, angle);
#endif
}

void AccessibilityCircleDrawingManager::SetPointerLocation(int32_t physicalX, int32_t physicalY, uint64_t screenId)
{
    HILOG_DEBUG("Pointer window move, x:%{public}d, y:%{public}d", physicalX, physicalY);
    if (surfaceNode_ != nullptr) {
        surfaceNode_->SetBounds(physicalX - half_,
            physicalY - half_,
            surfaceNode_->GetStagingProperties().GetBounds().z_,
            surfaceNode_->GetStagingProperties().GetBounds().w_);
        screenId_ = screenId;
        surfaceNode_->AttachToDisplay(screenId);
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
}

void AccessibilityCircleDrawingManager::DrawPointer(int32_t physicalX, int32_t physicalY, int32_t angle,
    uint64_t screenId)
{
    HILOG_DEBUG();
    if (surfaceNode_ != nullptr) {
        SetPointerLocation(physicalX, physicalY, screenId);
        DrawingProgress(physicalX, physicalY, angle);
        UpdatePointerVisible(true);
        HILOG_DEBUG("surfaceNode_ is existed");
        return;
    }

    CreatePointerWindow(physicalX, physicalY, screenId);
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    DrawingProgress(physicalX, physicalY, angle);
    UpdatePointerVisible(true);
}

int32_t AccessibilityCircleDrawingManager::ParseStringToInteger(const std::string& color)
{
    int32_t val = 0;
    int32_t base = 1;
    if (color.size() < 1) {
        return val;
    }
    for (int32_t index = static_cast<int32_t>(color.size() - 1); index >= 0; index--) {
        if (color[index] >= CHAR_0 && color[index] <= CHAR_9) {
            val += base * static_cast<int32_t>(color[index] - CHAR_0);
        }

        if (color[index] == CHAR_A) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_B) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_C) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_D) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_E) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_F) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        base = base * HEXADECIMAL;
    }

    return val;
}

std::vector<int32_t> AccessibilityCircleDrawingManager::ParseColorString(const std::string& color)
{
    std::string r = color.substr(RED_START_INDEX, COLOR_STRING_LENGTH);
    std::string g = color.substr(GREEN_START_INDEX, COLOR_STRING_LENGTH);
    std::string b = color.substr(BLUE_START_INDEX, COLOR_STRING_LENGTH);

    int32_t rValue = ParseStringToInteger(r);
    int32_t gValue = ParseStringToInteger(g);
    int32_t bValue = ParseStringToInteger(b);

    std::vector<int32_t> rgbValue = {rValue, gValue, bValue};
    return rgbValue;
}
} // namespace Accessibility
} // namespace OHOS