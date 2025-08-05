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

// LCOV_EXCL_START
#include "magnification_window.h"
#include "visibility.h"

namespace OHOS {
namespace Accessibility {

namespace {
    const std::string WINDOW_NAME = "magnification_window";
}

static MagnificationWindow instance;

//common
PointerPos MagnificationWindow::GetSourceCenter()
{
    PointerPos point = {0, 0};
    point.posX = sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_ / DIVISOR_TWO);
    point.posY = sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_ / DIVISOR_TWO);
    return point;
}

PointerPos MagnificationWindow::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos sourcePoint = {0, 0};
    if ((abs(scale_) < EPS)) {
        return sourcePoint;
    }
    int32_t windowDeltaX = posX - windowRect_.posX_;
    int32_t windowDeltaY = posY - windowRect_.posY_;
    sourcePoint.posX = sourceRect_.posX_ + static_cast<int32_t>(windowDeltaX / scale_);
    sourcePoint.posY = sourceRect_.posY_ + static_cast<int32_t>(windowDeltaY / scale_);
    return sourcePoint;
}

void MagnificationWindow::CreateMagnificationWindow()
{
    HILOG_DEBUG();
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    if (windowOption == nullptr) {
        HILOG_ERROR("windowOption is null.");
        return;
    }
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_MAGNIFICATION);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetWindowRect(windowRect_);
    windowOption->SetFocusable(false);
    window_ = OHOS::Rosen::Window::Create(WINDOW_NAME, windowOption);
    if (window_ == nullptr) {
        HILOG_ERROR("window create failed.");
        ExtUtils::RecordMagnificationUnavailableEvent("Create window failed.");
        return;
    }
    window_->SetCornerRadius(CORNER_RADIUS);
    surfaceNode_ = window_->GetSurfaceNode();
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("get surfaceNode_ failed.");
        return;
    }
    rsUIContext_ = surfaceNode_->GetRSUIContext();
    canvasNode_ = Rosen::RSCanvasNode::Create(false, false, rsUIContext_);
    if (canvasNode_ == nullptr) {
        HILOG_ERROR("canvasNode_ is nullptr.");
        return;
    }
    canvasNode_->SetSkipCheckInMultiInstance(true);
}

void MagnificationWindow::GetWindowParam()
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    screenId_ = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(screenId_);
    if (display == nullptr) {
        HILOG_ERROR("display is nullptr.");
        return;
    }
    screenWidth_ = static_cast<uint32_t>(display->GetWidth());
    screenHeight_ = static_cast<uint32_t>(display->GetHeight());
    screenSpan_ = hypot(screenWidth_, screenHeight_);
    screenRect_ = {0, 0, screenWidth_, screenHeight_};
#else
    HILOG_INFO("not support");
#endif
}

void MagnificationWindow::FlushImplicitTransaction()
{
    if (rsUIContext_ != nullptr) {
        auto rsTransaction = rsUIContext_->GetRSTransaction();
        if (rsTransaction != nullptr) {
            rsTransaction->FlushImplicitTransaction();
        }
    } else {
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
}

PointerPos MagnificationWindow::GetRectCenter(Rosen::Rect rect)
{
    PointerPos point = {0, 0};
    point.posX = rect.posX_ + static_cast<int32_t>(rect.width_ / DIVISOR_TWO);
    point.posY = rect.posY_ + static_cast<int32_t>(rect.height_ / DIVISOR_TWO);
    return point;
}

Rosen::Rect MagnificationWindow::GetSourceRectFromPointer(int32_t centerX, int32_t centerY)
{
    Rosen::Rect sourceRect = {0, 0, 0, 0};
    if ((abs(scale_) < EPS)) {
        return sourceRect;
    }
    sourceRect.width_ = static_cast<uint32_t>(static_cast<float>(windowRect_.width_) / scale_);
    sourceRect.height_ = static_cast<uint32_t>(static_cast<float>(windowRect_.height_) / scale_);

    int32_t x = centerX - static_cast<int32_t>(sourceRect.width_ / DIVISOR_TWO);
    int32_t y = centerY - static_cast<int32_t>(sourceRect.height_ / DIVISOR_TWO);

    x = (x < 0) ? 0 : x;
    x = (x + static_cast<int32_t>(sourceRect.width_)) > static_cast<int32_t>(
        screenWidth_) ? static_cast<int32_t>(screenWidth_ - sourceRect.width_) : x;
 
    y = (y < 0) ? 0 : y;
    y = (y + static_cast<int32_t>(sourceRect.height_)) > static_cast<int32_t>(
        screenHeight_) ? static_cast<int32_t>(screenHeight_ - sourceRect.height_) : y;

    sourceRect.posX_ = x;
    sourceRect.posY_ = y;
    HILOG_WARN("sourceRect_ = %{public}s", sourceRect_.ToString().c_str());
    return sourceRect;
}

void MagnificationWindow::DisableMagnification(bool needClear)
{
    HILOG_INFO();
    if (needClear && surfaceNode_ != nullptr) {
        HILOG_DEBUG("claer surfaceNode");
        surfaceNode_->SetVisible(false);
        surfaceNode_->ClearChildren();
        FlushImplicitTransaction();
    }

    if (window_ != nullptr) {
        window_->Hide();
        window_->Destroy();
        window_ = nullptr;
    }
    surfaceNode_ = nullptr;
    canvasNode_ = nullptr;
    rsUIContext_ = nullptr;
}

// full magnification
PointerPos MagnificationWindow::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    int32_t posX = coordinates.posX;
    int32_t posY = coordinates.posY;
    PointerPos point = {posX, posY};

    if (type == BOTTOM_BACK_GESTURE) {
        int32_t offsetY = static_cast<int32_t>(screenHeight_) -
            (sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_));
        point.posY = posY + offsetY;
        return point;
    }
    if (type == LEFT_BACK_GESTURE) {
        int32_t offsetX = sourceRect_.posX_;
        point.posX = posX - offsetX;
        return point;
    }

    if (type == RIGHT_BACK_GESTURE) {
        int32_t offsetX = static_cast<int32_t>(screenWidth_) -
            (sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_));
        point.posX = posX + offsetX;
        return point;
    }

    return point;
}

uint32_t MagnificationWindow::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    bool isTapOnBottom = posY <= static_cast<int32_t>(screenHeight_) &&
        posY >= static_cast<int32_t>(screenHeight_) - GESTURE_OFFSET;
    if (isTapOnBottom) {
        HILOG_DEBUG("Tap On Bottom");
        return BOTTOM_BACK_GESTURE;
    }
    bool isTapOnLeftSide = posX >= 0 && posX <= GESTURE_OFFSET;
    if (isTapOnLeftSide) {
        HILOG_DEBUG("Tap On Left Side");
        return LEFT_BACK_GESTURE;
    }
    bool isTapOnRightSide = posX >= static_cast<int32_t>(screenWidth_) - GESTURE_OFFSET &&
        posX <= static_cast<int32_t>(screenWidth_);
    if (isTapOnRightSide) {
        HILOG_DEBUG("Tap On Right Side");
        return RIGHT_BACK_GESTURE;
    }
    return INVALID_GESTURE_TYPE;
}

void MagnificationWindow::FollowFocuseElementFull(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    window_->SetFrameRectForPartialZoomIn(sourceRect_);
    DrawRuoundRectFrameFull();
    FlushImplicitTransaction();
    UpdateAnchor();
}

void MagnificationWindow::UpdateAnchor()
{
    centerX_ = static_cast<int32_t>((sourceRect_.posX_ + sourceRect_.posX_ +
        static_cast<int32_t>(sourceRect_.width_)) / DIVISOR_TWO);
    centerY_ = static_cast<int32_t>((sourceRect_.posY_ + sourceRect_.posY_ +
        static_cast<int32_t>(sourceRect_.height_)) / DIVISOR_TWO);
}

void MagnificationWindow::DrawRuoundRectFrameFull()
{
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("get surfaceNode_ failed.");
        return;
    }

    if (canvasNode_ == nullptr) {
        HILOG_ERROR("create canvasNode_ fail");
        return;
    }
    surfaceNode_->SetAbilityBGAlpha(BG_ALPHA);
    surfaceNode_->AddChild(canvasNode_, -1);
    canvasNode_->SetBounds(0, 0, screenWidth_, screenHeight_);
    canvasNode_->SetFrame(0, 0, screenWidth_, screenHeight_);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);

    auto canvas = canvasNode_->BeginRecording(screenWidth_, screenHeight_);
    if (canvas == nullptr) {
        HILOG_ERROR("get canvas fail");
        return;
    }
    Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(ORANGE_COLOR);
    pen.SetWidth(PEN_WIDTH);
    canvas->AttachPen(pen);
    Rosen::Drawing::Path path;
    path.AddRoundRect({ROUND_RECT_MARGIN, ROUND_RECT_MARGIN,
        screenWidth_ - ROUND_RECT_MARGIN, screenHeight_ - ROUND_RECT_MARGIN},
        CORNER_RADIUS, CORNER_RADIUS, Rosen::Drawing::PathDirection::CW_DIRECTION);
    canvas->DrawPath(path);
    canvas->DetachPen();
    canvasNode_->FinishRecording();
}

void MagnificationWindow::EnableMagnificationFull(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    GetWindowParam();
    if (window_ == nullptr) {
        HILOG_DEBUG("window is null. need create.");
        windowRect_ = {0, 0, screenWidth_, screenHeight_};
        CreateMagnificationWindow();
    }

    if (window_ == nullptr) {
        HILOG_ERROR("create window failed.");
        return;
    }
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    UpdateAnchor();
    DrawRuoundRectFrameFull();
    Rosen::WMError ret = window_->SetFrameRectForPartialZoomIn(sourceRect_);
    if (ret != Rosen::WMError::WM_OK) {
        HILOG_ERROR("Trigger magnification failed.");
        ExtUtils::RecordMagnificationUnavailableEvent("Trigger magnification failed.");
    }
    window_->Show();
    FlushImplicitTransaction();
    isMagnificationShowFull_ = true;
}

void MagnificationWindow::DisableMagnificationFull(bool needClear)
{
    HILOG_INFO();
    DisableMagnification();
    isMagnificationShowFull_ = false;
}

void MagnificationWindow::SetScaleFull(float scaleSpan)
{
    HILOG_DEBUG();
    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }
    if (screenWidth_ == 0 || screenHeight_ == 0 || abs(screenSpan_) < EPS) {
        HILOG_ERROR("screen param invalid.");
        return;
    }
    
    float ratio = scaleSpan / screenSpan_;
    float tmpScale = scale_ + ratio * scale_;

    if (tmpScale > MAX_SCALE) {
        tmpScale = MAX_SCALE;
    }
    if (tmpScale < DEFAULT_SCALE) {
        tmpScale = DEFAULT_SCALE;
    }

    uint32_t newWidth = static_cast<uint32_t>(windowRect_.width_ / tmpScale);
    uint32_t newHeight = static_cast<uint32_t>(windowRect_.height_ / tmpScale);
    int32_t newPosX = centerX_ - static_cast<int32_t>(newWidth / DIVISOR_TWO);
    int32_t newPosY = centerY_ - static_cast<int32_t>(newHeight / DIVISOR_TWO);

    if (newPosX < 0) {
        centerX_ -= newPosX;
        newPosX = 0;
    }

    if (newPosY < 0) {
        centerY_ -= newPosY;
        newPosY = 0;
    }

    if (newPosX + static_cast<int32_t>(newWidth) > static_cast<int32_t>(screenWidth_)) {
        centerX_ -= (newPosX + static_cast<int32_t>(newWidth) - static_cast<int32_t>(screenWidth_));
        newPosX = static_cast<int32_t>(screenWidth_) - static_cast<int32_t>(newWidth);
    }

    if (newPosY + static_cast<int32_t>(newHeight) > static_cast<int32_t>(screenHeight_)) {
        centerY_ -= (newPosY + static_cast<int32_t>(newHeight) - static_cast<int32_t>(screenHeight_));
        newPosY = static_cast<int32_t>(screenHeight_) - static_cast<int32_t>(newHeight);
    }

    sourceRect_ = {newPosX, newPosY, newWidth, newHeight};
    scale_ = tmpScale;
    window_->SetFrameRectForPartialZoomIn(sourceRect_);
    DrawRuoundRectFrameFull();
    FlushImplicitTransaction();
    UpdateAnchor();
}

void MagnificationWindow::MoveMagnificationFull(int32_t deltaX, int32_t deltaY)
{
    HILOG_DEBUG();
    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }
    int32_t sourcePosX = sourceRect_.posX_ - deltaX;
    int32_t sourcePosY = sourceRect_.posY_ - deltaY;

    if (sourcePosX < 0) {
        sourcePosX = 0;
    }

    if (sourcePosY < 0) {
        sourcePosY = 0;
    }

    if (sourcePosX + static_cast<int32_t>(sourceRect_.width_) > static_cast<int32_t>(screenWidth_)) {
        sourcePosX = static_cast<int32_t>(screenWidth_) - static_cast<int32_t>(sourceRect_.width_);
    }

    if (sourcePosY + static_cast<int32_t>(sourceRect_.height_) > static_cast<int32_t>(screenHeight_)) {
        sourcePosY = static_cast<int32_t>(screenHeight_) - static_cast<int32_t>(sourceRect_.height_);
    }
    sourceRect_.posX_ = sourcePosX;
    sourceRect_.posY_ = sourcePosY;
    window_->SetFrameRectForPartialZoomIn(sourceRect_);
    DrawRuoundRectFrameFull();
    FlushImplicitTransaction();
    UpdateAnchor();
}

void MagnificationWindow::ShowMagnificationFull()
{
    int32_t centerX = static_cast<int32_t>(screenWidth_ / DIVISOR_TWO);
    int32_t centerY = static_cast<int32_t>(screenHeight_ / DIVISOR_TWO);
    EnableMagnificationFull(centerX, centerY);
}

void MagnificationWindow::RefreshWindowParamFull(RotationType type)
{
    HILOG_DEBUG();
    PointerPos center = GetRectCenter(sourceRect_);
    if (isMagnificationShowFull_) {
        DisableMagnificationFull(false);
        center = TransferCenter(type, center);
        EnableMagnificationFull(center.posX, center.posY);
    } else {
        GetWindowParam();
    }
}

bool MagnificationWindow::IsMagnificationShowFull()
{
    return isMagnificationShowFull_;
}

// window magnification
Rosen::Rect MagnificationWindow::GetWindowRectFromPointer(int32_t centerX, int32_t centerY)
{
    Rosen::Rect windowRect = {0, 0, windowWidth_, windowHeight_};
    int32_t x = centerX - static_cast<int32_t>(windowWidth_ / DIVISOR_TWO);
    int32_t y = centerY - static_cast<int32_t>(windowHeight_ / DIVISOR_TWO);

    x = (x < 0) ? 0 : x;
    x = (x + static_cast<int32_t>(windowWidth_)) > static_cast<int32_t>(
        screenWidth_) ? static_cast<int32_t>(screenWidth_ - windowWidth_) : x;

    y = (y < 0) ? 0 : y;
    y = (y + static_cast<int32_t>(windowHeight_)) > static_cast<int32_t>(
        screenHeight_) ? static_cast<int32_t>(screenHeight_ - windowHeight_) : y;

    windowRect.posX_ = x;
    windowRect.posY_ = y;
    return windowRect;
}

void MagnificationWindow::CalculateAnchorOffset()
{
    int32_t windowCenterX = windowRect_.posX_ + windowRect_.posX_ + static_cast<int32_t>(windowRect_.width_);
    int32_t sourceCenterX = sourceRect_.posX_ + sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_);
    anchorOffsetX_ = static_cast<int32_t>((windowCenterX - sourceCenterX) * HALF);

    int32_t windowCenterY = windowRect_.posY_ + windowRect_.posY_ + static_cast<int32_t>(windowRect_.height_);
    int32_t sourceCenterY = sourceRect_.posY_ + sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_);
    anchorOffsetY_ = static_cast<int32_t>((windowCenterY - sourceCenterY) * HALF);
}

void MagnificationWindow::UpdateRelativeRect()
{
    int32_t posX = sourceRect_.posX_ - windowRect_.posX_;
    if (posX < 0) {
        posX = 0;
    }

    int32_t deltaX = static_cast<int32_t>(windowRect_.width_) - static_cast<int32_t>(sourceRect_.width_);
    if (posX > deltaX) {
        posX = deltaX;
    }
    relativeRect_.posX_ = posX;

    int32_t posY = sourceRect_.posY_ - windowRect_.posY_;
    if (posY < 0) {
        posY = 0;
    }

    int32_t deltaY = static_cast<int32_t>(windowRect_.height_) - static_cast<int32_t>(sourceRect_.height_);
    if (posY > deltaY) {
        posY = deltaY;
    }
    relativeRect_.posY_ = posY;
    relativeRect_.width_ = sourceRect_.width_;
    relativeRect_.height_ = sourceRect_.height_;
}

void MagnificationWindow::AdjustMagnificationWindowPosition()
{
    if (windowRect_.posX_ < 0) {
        windowRect_.posX_ = 0;
    }

    if (windowRect_.posY_ < 0) {
        windowRect_.posY_ = 0;
    }

    if (windowRect_.posX_ + static_cast<int32_t>(windowRect_.width_) > static_cast<int32_t>(screenWidth_)) {
        windowRect_.posX_ = static_cast<int32_t>(screenWidth_) - static_cast<int32_t>(windowRect_.width_);
    }

    if (windowRect_.posY_ + static_cast<int32_t>(windowRect_.height_) > static_cast<int32_t>(screenHeight_)) {
        windowRect_.posY_ = static_cast<int32_t>(screenHeight_) - static_cast<int32_t>(windowRect_.height_);
    }
}

void MagnificationWindow::AdjustSourceWindowPosition()
{
    if (sourceRect_.posX_ < windowRect_.posX_) {
        sourceRect_.posX_ = windowRect_.posX_;
    }

    if (sourceRect_.posY_ < windowRect_.posY_) {
        sourceRect_.posY_ = windowRect_.posY_;
    }

    int32_t windowX = windowRect_.posX_ + static_cast<int32_t>(windowRect_.width_);
    if (sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_) > windowX) {
        sourceRect_.posX_ = windowX - static_cast<int32_t>(sourceRect_.width_);
    }

    int32_t windowY = windowRect_.posY_ + static_cast<int32_t>(windowRect_.height_);
    if (sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_) > windowY) {
        sourceRect_.posY_ = windowY - static_cast<int32_t>(sourceRect_.height_);
    }
}

bool MagnificationWindow::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    if (!isMagnificationShowPart_) {
        return false;
    }
    Rosen::Rect innerRect = {windowRect_.posX_ + static_cast<int32_t>(hotAreaWidth_),
        windowRect_.posY_ + static_cast<int32_t>(hotAreaWidth_),
        windowRect_.width_ - static_cast<uint32_t>(2 * hotAreaWidth_),
        windowRect_.height_ - static_cast<uint32_t>(2 * hotAreaWidth_)};
    Rosen::Rect outRect = {windowRect_.posX_ - static_cast<int32_t>(hotAreaWidth_),
        windowRect_.posY_ - static_cast<int32_t>(hotAreaWidth_),
        windowRect_.width_ + static_cast<uint32_t>(2 * hotAreaWidth_),
        windowRect_.height_ + static_cast<uint32_t>(2 * hotAreaWidth_)};
    return ExtUtils::IsInRect(posX, posY, outRect) && !(ExtUtils::IsInRect(posX, posY, innerRect));
}

bool MagnificationWindow::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    return ExtUtils::IsInRect(posX, posY, windowRect_);
}
void MagnificationWindow::FixSourceCenter(bool needFix)
{
    HILOG_DEBUG();
    if (needFix == isFixSourceCenter_) {
        return;
    }

    isFixSourceCenter_ = needFix;
    if (needFix) {
        isFixSourceCenter_ = needFix;
        float sourceCenterX = (sourceRect_.posX_ + sourceRect_.posX_ +
            static_cast<int32_t>(sourceRect_.width_)) / static_cast<float>(DIVISOR_TWO);
        float sourceCenterY = (sourceRect_.posY_ + sourceRect_.posY_ +
            static_cast<int32_t>(sourceRect_.height_)) / static_cast<float>(DIVISOR_TWO);
        fixedSourceCenter_ = {static_cast<int32_t>(sourceCenterX), static_cast<int32_t>(sourceCenterY)};
    } else {
        fixedSourceCenter_ = {0, 0};
    }
}

void MagnificationWindow::DrawRuoundRectFramePart()
{
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("get surfaceNode_ failed.");
        return;
    }

    if (canvasNode_ == nullptr) {
        HILOG_ERROR("create canvasNode_ fail");
        return;
    }
    surfaceNode_->SetAbilityBGAlpha(BG_ALPHA);
    surfaceNode_->AddChild(canvasNode_, -1);
    canvasNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    canvasNode_->SetFrame(0, 0, windowWidth_, windowHeight_);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);

    auto canvas = canvasNode_->BeginRecording(windowWidth_, windowHeight_);
    if (canvas == nullptr) {
        HILOG_ERROR("get canvas fail");
        return;
    }
    Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(ORANGE_COLOR);
    pen.SetWidth(PEN_WIDTH);
    canvas->AttachPen(pen);
    Rosen::Drawing::Path path;
    path.AddRoundRect({ROUND_RECT_MARGIN, ROUND_RECT_MARGIN,
        windowWidth_ - ROUND_RECT_MARGIN, windowHeight_ - ROUND_RECT_MARGIN},
        CORNER_RADIUS, CORNER_RADIUS, Rosen::Drawing::PathDirection::CW_DIRECTION);
    canvas->DrawPath(path);
    canvas->DetachPen();

    Rosen::Drawing::Pen linePen;
    linePen.SetAntiAlias(true);
    linePen.SetColor(ORANGE_COLOR);
    linePen.SetWidth(PEN_WIDTH);
    canvas->AttachPen(linePen);
    auto p1 = Rosen::Drawing::Point(BAR_START, windowHeight_ - barMargin_);
    auto p2 = Rosen::Drawing::Point(BAR_END, windowHeight_ - barMargin_);
    canvas->DrawLine(p1, p2);
    canvas->DetachPen();

    Rosen::Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ORANGE_COLOR);
    canvas->AttachBrush(brush);
    canvas->DrawCircle(p1, static_cast<float>(PEN_WIDTH / DIVISOR_TWO));
    canvas->DrawCircle(p2, static_cast<float>(PEN_WIDTH / DIVISOR_TWO));
    canvas->DetachBrush();
    canvasNode_->FinishRecording();
}

void MagnificationWindow::EnableMagnificationPart(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    GetWindowParam();
    if (window_ == nullptr) {
        HILOG_DEBUG("window is null. need create.");
        windowRect_ = GetWindowRectFromPointer(centerX, centerY);
        CreateMagnificationWindow();
    }

    if (window_ == nullptr) {
        HILOG_ERROR("create window failed.");
        return;
    }
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    CalculateAnchorOffset();
    UpdateRelativeRect();
    Rosen::WMError ret = window_->SetFrameRectForPartialZoomIn(relativeRect_);
    if (ret != Rosen::WMError::WM_OK) {
        HILOG_ERROR("Trigger magnification failed.");
        ExtUtils::RecordMagnificationUnavailableEvent("Trigger magnification failed.");
    }
    DrawRuoundRectFramePart();
    window_->Show();
    FlushImplicitTransaction();
    isMagnificationShowPart_ = true;
}

void MagnificationWindow::DisableMagnificationPart(bool needClear)
{
    DisableMagnification(needClear);
    anchorOffsetX_ = 0;
    anchorOffsetY_ = 0;
    isMagnificationShowPart_ = false;
}

bool MagnificationWindow::IsMagnificationShowPart()
{
    return isMagnificationShowPart_;
}

void MagnificationWindow::SetScalePart(float scaleSpan)
{
    HILOG_DEBUG();
    if (screenWidth_ == 0 || screenHeight_ == 0 || abs(screenSpan_) < EPS) {
        HILOG_ERROR("screen param invalid.");
        return;
    }

    float ratio = scaleSpan / screenSpan_;
    float tmpScale = scale_ + ratio * scale_;
    if (tmpScale > MAX_SCALE) {
        tmpScale = MAX_SCALE;
    }
    if (tmpScale < DEFAULT_SCALE) {
        tmpScale = DEFAULT_SCALE;
    }

    uint32_t newWidth = static_cast<uint32_t>(windowRect_.width_ / tmpScale);
    uint32_t newHeight = static_cast<uint32_t>(windowRect_.height_ / tmpScale);
    int32_t newPosX = fixedSourceCenter_.posX - static_cast<int32_t>(newWidth / DIVISOR_TWO);
    int32_t newPosY = fixedSourceCenter_.posY - static_cast<int32_t>(newHeight / DIVISOR_TWO);

    Rosen::Rect tmpRect = {newPosX, newPosY, newWidth, newHeight};
    if (!tmpRect.IsInsideOf(screenRect_)) {
        HILOG_ERROR("sourceRect out of screenRect.");
        return;
    }

    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }

    sourceRect_ = tmpRect;
    scale_ = tmpScale;
    UpdateRelativeRect();
    window_->SetFrameRectForPartialZoomIn(relativeRect_);
    DrawRuoundRectFramePart();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void MagnificationWindow::MoveMagnificationPart(int32_t deltaX, int32_t deltaY)
{
    if (window_ == nullptr) {
        HILOG_ERROR("window is null.");
        return;
    }

    int32_t windowPosX = windowRect_.posX_;
    int32_t sourcePosX = sourceRect_.posX_;

    if (abs(anchorOffsetX_) <= ANCHOR_OFFSET) {
        windowPosX += deltaX;
    }
    sourcePosX += deltaX;

    int32_t windowPosY = windowRect_.posY_;
    int32_t sourcePosY = sourceRect_.posY_;

    if (abs(anchorOffsetY_) <= ANCHOR_OFFSET) {
        windowPosY += deltaY;
    }
    sourcePosY += deltaY;

    windowRect_.posX_ = windowPosX;
    windowRect_.posY_ = windowPosY;
    AdjustMagnificationWindowPosition();

    window_->MoveTo(windowRect_.posX_, windowRect_.posY_);

    sourceRect_.posX_ = sourcePosX;
    sourceRect_.posY_ = sourcePosY;
    AdjustSourceWindowPosition();
    UpdateRelativeRect();
    window_->SetFrameRectForPartialZoomIn(relativeRect_);
    DrawRuoundRectFramePart();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void MagnificationWindow::ShowMagnificationPart()
{
    int32_t centerX = static_cast<int32_t>(screenWidth_ / DIVISOR_TWO);
    int32_t centerY = static_cast<int32_t>(screenHeight_ / DIVISOR_TWO);
    EnableMagnificationPart(centerX, centerY);
}

void MagnificationWindow::FollowFocuseElementPart(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }
    windowRect_ = GetWindowRectFromPointer(centerX, centerY);
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    window_->MoveTo(windowRect_.posX_, windowRect_.posY_);
    UpdateRelativeRect();
    window_->SetFrameRectForPartialZoomIn(relativeRect_);
    DrawRuoundRectFramePart();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void MagnificationWindow::RefreshWindowParamPart(RotationType type)
{
    HILOG_DEBUG();
    if (isMagnificationShowPart_) {
        PointerPos center = GetRectCenter(windowRect_);
        HILOG_INFO("need refresh window param.");
        DisableMagnificationPart();
        center = TransferCenter(type, center);
        EnableMagnificationPart(center.posX, center.posY);
    } else {
        GetWindowParam();
    }
}

PointerPos MagnificationWindow::TransferCenter(RotationType type, PointerPos center)
{
    if (type == RotationType::NO_CHANGE || type == RotationType::UNKNOWN) {
        return center;
    }
    GetWindowParam();
    if (type == RotationType::LEFT_ROTATE) {
        return {center.posY, static_cast<int32_t>(screenHeight_) - center.posX};
    }
    if (type == RotationType::RIGHT_ROTATE) {
        return {screenWidth_ - center.posY, center.posX};
    }
    if (type == RotationType::FLIP_VERTICAL) {
        return {screenWidth_ - center.posX, screenHeight_ - center.posY};
    }
    return center;
}

extern "C" API_EXPORT void EnableMagnification(uint32_t magnificationType, int32_t posX, int32_t posY)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.EnableMagnificationFull(posX, posY);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.EnableMagnificationPart(posX, posY);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT void DisableMagnification(uint32_t magnificationType, bool needClear)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.DisableMagnificationFull(needClear);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.DisableMagnificationPart(needClear);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT void SetScale(uint32_t magnificationType, float scaleSpan)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.SetScaleFull(scaleSpan);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.SetScalePart(scaleSpan);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT void MoveMagnification(uint32_t magnificationType, int32_t deltaX, int32_t deltaY)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.MoveMagnificationFull(deltaX, deltaY);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.MoveMagnificationPart(deltaX, deltaY);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT PointerPos ConvertGesture(uint32_t type, PointerPos coordinates)
{
    return instance.ConvertGesture(type, coordinates);
}

extern "C" API_EXPORT PointerPos ConvertCoordinates(int32_t posX, int32_t posY)
{
    return instance.ConvertCoordinates(posX, posY);
}

extern "C" API_EXPORT uint32_t CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    return instance.CheckTapOnHotArea(posX, posY);
}

extern "C" API_EXPORT float GetScale()
{
    return instance.GetScale();
}

extern "C" API_EXPORT bool IsMagnificationWindowShow(uint32_t magnificationType)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        return instance.IsMagnificationShowFull();
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        return instance.IsMagnificationShowPart();
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
    return false;
}

extern "C" API_EXPORT void FollowFocuseElement(uint32_t magnificationType, int32_t centerX, int32_t centerY)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.FollowFocuseElementFull(centerX, centerY);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.FollowFocuseElementPart(centerX, centerY);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT PointerPos GetSourceCenter()
{
    return instance.GetSourceCenter();
}

extern "C" API_EXPORT void ShowMagnification(uint32_t magnificationType)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.ShowMagnificationFull();
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.ShowMagnificationPart();
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT void RefreshWindowParam(uint32_t magnificationType, RotationType type)
{
    if (magnificationType == FULL_SCREEN_MAGNIFICATION) {
        instance.RefreshWindowParamFull(type);
        return;
    }
    if (magnificationType == WINDOW_MAGNIFICATION) {
        instance.RefreshWindowParamPart(type);
        return;
    }
    HILOG_DEBUG("invalid type = %{public}d", magnificationType);
}

extern "C" API_EXPORT bool IsTapOnHotArea(int32_t posX, int32_t posY)
{
    return instance.IsTapOnHotArea(posX, posY);
}

extern "C" API_EXPORT bool IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    return instance.IsTapOnMagnificationWindow(posX, posY);
}

extern "C" API_EXPORT void FixSourceCenter(bool needFix)
{
    instance.FixSourceCenter(needFix);
}

extern "C" API_EXPORT void InitMagnificationParam(float scale)
{
    instance.InitMagnificationParam(scale);
}
}
}
// LCOV_EXCL_STOP