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
#include "window_magnification_manager.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"
#include "utils.h"
#include "magnification_def.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t BAR_START = 400;
    constexpr int32_t BAR_END = 550;
}

WindowMagnificationManager::WindowMagnificationManager()
{
    HILOG_INFO();
    GetWindowParam();
    InitMagnificationParam();
}

void WindowMagnificationManager::CreateMagnificationWindow(int32_t posX, int32_t posY)
{
    HILOG_DEBUG();
    windowRect_ = GetWindowRectFromPointer(posX, posY);
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    if (windowOption == nullptr) {
        HILOG_ERROR("windowOption is null.");
        return;
    }
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_MAGNIFICATION);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetWindowRect(windowRect_);
    windowOption->SetFocusable(false);
    window_ = Rosen::Window::Create(WINDOW_NAME, windowOption);
    if (window_ == nullptr) {
        HILOG_ERROR("create failed.");
        return;
    }
    window_->SetCornerRadius(CORNER_RADIUS);
    surfaceNode_ = window_->GetSurfaceNode();
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr.");
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

void WindowMagnificationManager::DrawRuoundRectFrame()
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

void WindowMagnificationManager::EnableWindowMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    GetWindowParam();
    InitMagnificationParam();
    if (window_ == nullptr) {
        HILOG_ERROR("window is null. need create.");
        CreateMagnificationWindow(centerX, centerY);
    }

    if (window_ == nullptr) {
        HILOG_ERROR("create window failed.");
        return;
    }
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    CalculateAnchorOffset();
    UpdateRelativeRect();
    window_->SetFrameRectForPartialZoomIn(relativeRect_);
    DrawRuoundRectFrame();
    window_->Show();
    FlushImplicitTransaction();
    isMagnificationWindowShow_ = true;
}

void WindowMagnificationManager::ShowWindowMagnification()
{
    int32_t centerX = static_cast<int32_t>(screenWidth_ / DIVISOR_TWO);
    int32_t centerY = static_cast<int32_t>(screenHeight_ / DIVISOR_TWO);
    EnableWindowMagnification(centerX, centerY);
}

void WindowMagnificationManager::ShowWindowMagnificationWithPosition(PointerPos pos)
{
    if (pos.posX == 0 && pos.posY == 0) {
        ShowWindowMagnification();
    } else {
        EnableWindowMagnification(pos.posX, pos.posY);
    }
}

void WindowMagnificationManager::DisableWindowMagnification(bool needClear)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
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
    isMagnificationWindowShow_ = false;
    int32_t anchorOffsetX_ = 0;
    int32_t anchorOffsetY_ = 0;
}

void WindowMagnificationManager::SetScale(float deltaSpan)
{
    HILOG_DEBUG();
    if (screenWidth_ == 0 || screenHeight_ == 0 || abs(screenSpan_) < EPS) {
        HILOG_ERROR("screen param invalid.");
        return;
    }

    float ratio = deltaSpan / screenSpan_;
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
    DrawRuoundRectFrame();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void WindowMagnificationManager::MoveMagnificationWindow(int32_t deltaX, int32_t deltaY)
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
    DrawRuoundRectFrame();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void WindowMagnificationManager::GetWindowParam()
{
    HILOG_INFO();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    screenId_ = displayMgr.GetDefaultDisplayId();
    orientation_ = displayMgr.GetOrientation();
    sptr<Rosen::Display> display = displayMgr.GetDisplay(screenId_);
    if (display == nullptr) {
        HILOG_ERROR("display is nullptr.");
        return;
    }
    screenWidth_ = static_cast<uint32_t>(display->GetWidth());
    screenHeight_ = static_cast<uint32_t>(display->GetHeight());
    HILOG_INFO("screenWidth_ = %{public}d, screenHeight_ = %{public}d.", screenWidth_, screenHeight_);

    screenSpan_ = hypot(screenWidth_, screenHeight_);
    screenRect_ = {0, 0, screenWidth_, screenHeight_};
#else
    HILOG_INFO("not support");
#endif
}

void WindowMagnificationManager::InitMagnificationParam()
{
    scale_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationScale();
}

bool WindowMagnificationManager::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    if (!isMagnificationWindowShow_) {
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
    return Utils::IsInRect(posX, posY, outRect) && !(Utils::IsInRect(posX, posY, innerRect));
}

void WindowMagnificationManager::RefreshWindowParam()
{
    HILOG_DEBUG();
    if (isMagnificationWindowShow_) {
        PointerPos center = GetRectCenter(windowRect_);
        HILOG_INFO("need refresh window param.");
        DisableWindowMagnification();
        EnableWindowMagnification(center.posX, center.posY);
    } else {
        GetWindowParam();
    }
}

bool WindowMagnificationManager::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    return Utils::IsInRect(posX, posY, windowRect_);
}

PointerPos WindowMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
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

void WindowMagnificationManager::FixSourceCenter(bool needFix)
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

void WindowMagnificationManager::PersistScale()
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale_);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

PointerPos WindowMagnificationManager::ConvertCenterToTopLeft(int32_t centerX, int32_t centerY)
{
    PointerPos point = {0, 0};
    point.posX = centerX - static_cast<int32_t>(windowWidth_ / static_cast<float>(DIVISOR_TWO));
    point.posY = centerY - static_cast<int32_t>(windowHeight_ / static_cast<float>(DIVISOR_TWO));
    return point;
}

PointerPos WindowMagnificationManager::GetRectCenter(Rosen::Rect rect)
{
    PointerPos point = {0, 0};
    point.posX = rect.posX_ + static_cast<int32_t>(rect.width_ / DIVISOR_TWO);
    point.posY = rect.posY_ + static_cast<int32_t>(rect.height_ / DIVISOR_TWO);
    return point;
}

PointerPos WindowMagnificationManager::GetSourceCenter()
{
    PointerPos point = {0, 0};
    point.posX = sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_ / DIVISOR_TWO);
    point.posY = sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_ / DIVISOR_TWO);
    return point;
}

RectBound WindowMagnificationManager::GenRectBound(uint32_t width, uint32_t height)
{
    RectBound bound = {0, 0};
    if ((abs(scale_) < EPS)) {
        return bound;
    }
    bound.width = static_cast<uint32_t>(static_cast<float>(width) / scale_);
    bound.height = static_cast<uint32_t>(static_cast<float>(height) / scale_);
    return bound;
}

Rosen::Rect WindowMagnificationManager::GetSourceRectFromPointer(int32_t centerX, int32_t centerY)
{
    Rosen::Rect sourceRect = {0, 0, 0, 0};
    RectBound bound = GenRectBound(windowWidth_, windowHeight_);
    sourceRect.width_ = bound.width;
    sourceRect.height_ = bound.height;

    int32_t x = centerX - static_cast<int32_t>(sourceRect.width_ / DIVISOR_TWO);
    int32_t y = centerY - static_cast<int32_t>(sourceRect.height_ / DIVISOR_TWO);

    x = (x < 0) ? 0 : x;
    x = (x + static_cast<int32_t>(bound.width)) > static_cast<int32_t>(screenWidth_) ?
        static_cast<int32_t>(screenWidth_ - bound.width) : x;

    y = (y < 0) ? 0 : y;
    y = (y + static_cast<int32_t>(bound.height)) > static_cast<int32_t>(screenHeight_) ?
        static_cast<int32_t>(screenHeight_ - bound.height) : y;

    sourceRect.posX_ = x;
    sourceRect.posY_ = y;
    return sourceRect;
}

Rosen::Rect WindowMagnificationManager::GetWindowRectFromPointer(int32_t centerX, int32_t centerY)
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

void WindowMagnificationManager::AdjustMagnificationWindowPosition()
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

void WindowMagnificationManager::AdjustSourceWindowPosition()
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

void WindowMagnificationManager::CalculateAnchorOffset()
{
    int32_t windowCenterX = windowRect_.posX_ + windowRect_.posX_ + static_cast<int32_t>(windowRect_.width_);
    int32_t sourceCenterX = sourceRect_.posX_ + sourceRect_.posX_ + static_cast<int32_t>(sourceRect_.width_);
    anchorOffsetX_ = static_cast<int32_t>((windowCenterX - sourceCenterX) * HALF);

    int32_t windowCenterY = windowRect_.posY_ + windowRect_.posY_ + static_cast<int32_t>(windowRect_.height_);
    int32_t sourceCenterY = sourceRect_.posY_ + sourceRect_.posY_ + static_cast<int32_t>(sourceRect_.height_);
    anchorOffsetY_ = static_cast<int32_t>((windowCenterY - sourceCenterY) * HALF);
}

void WindowMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_INFO();
    if (window_ == nullptr) {
        HILOG_ERROR("window_ is nullptr.");
        return;
    }
    windowRect_ = GetWindowRectFromPointer(centerX, centerY);
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    window_->MoveTo(windowRect_.posX_, windowRect_.posY_);
    UpdateRelativeRect();
    window_->SetFrameRectForPartialZoomIn(relativeRect_);
    DrawRuoundRectFrame();
    FlushImplicitTransaction();
    CalculateAnchorOffset();
}

void WindowMagnificationManager::UpdateRelativeRect()
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

void WindowMagnificationManager::FlushImplicitTransaction()
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
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP