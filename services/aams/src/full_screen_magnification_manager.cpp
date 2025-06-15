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
#include "full_screen_magnification_manager.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"

namespace OHOS {
namespace Accessibility {
namespace {
    int32_t GESTURE_OFFSET = 100;
}

FullScreenMagnificationManager::FullScreenMagnificationManager()
{
    GetWindowParam();
    InitMagnificationParam();
}

void FullScreenMagnificationManager::CreateMagnificationWindow()
{
    HILOG_ERROR();
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_MAGNIFICATION);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetWindowRect(windowRect_);
    windowOption->SetFocusable(false);
    window_ = Rosen::Window::Create(WINDOW_NAME, windowOption);
    if (window_ == nullptr) {
        HILOG_ERROR("window create failed.");
        return;
    }
    window_->SetCornerRadius(CORNER_RADIUS);
    surfaceNode_ = window_->GetSurfaceNode();
    canvasNode_ = Rosen::RSCanvasNode::Create();
}

void FullScreenMagnificationManager::DrawRuoundRectFrame()
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

void FullScreenMagnificationManager::EnableMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    GetWindowParam();
    if (window_ == nullptr) {
        HILOG_ERROR("window is null. need create.");
        CreateMagnificationWindow();
    }

    if (window_ == nullptr) {
        HILOG_ERROR("create window failed.");
        return;
    }
    sourceRect_ = GetSourceRectFromPointer(centerX, centerY);
    UpdateAnchor();
    window_->SetFrameRectForParticalZoomIn(sourceRect_);
    DrawRuoundRectFrame();
    window_->Show();
    Rosen::RSTransaction::FlushImplicitTransaction();
    isMagnificationWindowShow_ = true;
}

void FullScreenMagnificationManager::ShowMagnification()
{
    int32_t centerX = static_cast<int32_t>(screenWidth_ / DIVISOR_TWO);
    int32_t centerY = static_cast<int32_t>(screenHeight_ / DIVISOR_TWO);
    EnableMagnification(centerX, centerY);
}

void FullScreenMagnificationManager::DisableMagnification()
{
    HILOG_INFO();
    if (window_ != nullptr) {
        window_->Hide();
        window_->Destroy();
        window_ = nullptr;
    }
    surfaceNode_ = nullptr;
    canvasNode_ = nullptr;
    isMagnificationWindowShow_ = false;
}

void FullScreenMagnificationManager::SetScale(float scaleSpan)
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
    HILOG_DEBUG("scale_ = %{public}f", scale_);
    window_->SetFrameRectForParticalZoomIn(sourceRect_);
    Rosen::RSTransaction::FlushImplicitTransaction();
    UpdateAnchor();
}

void FullScreenMagnificationManager::MoveMagnification(int32_t deltaX, int32_t deltaY)
{
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
    window_->SetFrameRectForParticalZoomIn(sourceRect_);
    Rosen::RSTransaction::FlushImplicitTransaction();
    UpdateAnchor();
}

void FullScreenMagnificationManager::PersistScale()
{
    HILOG_DEBUG("scale_ = %{public}f", scale_);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale_);
}

void FullScreenMagnificationManager::GetWindowParam()
{
    HILOG_INFO();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    screenId_ = displayMgr.GetDefaultDisplayId();
    OHOS::Rosen::DisplayOrientation currentOrientation = displayMgr.GetOrientation();
    orientation_ = currentOrientation;
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
    windowRect_ = {0, 0, screenWidth_, screenHeight_};
#else
    HILOG_INFO("not support");
#endif
}

void FullScreenMagnificationManager::RefreshWindowParam()
{
    HILOG_INFO();
    int32_t centerX = sourceRect_.posX_;
    int32_t centerY = sourceRect_.posY_;
    if (isMagnificationWindowShow_) {
        DisableMagnification();
        EnableMagnification(centerX, centerY);
    } else {
        GetWindowParam();
    }
}

PointerPos FullScreenMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
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

PointerPos FullScreenMagnificationManager::ConvertGesture(uint32_t type, PointerPos coordinates)
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

uint32_t FullScreenMagnificationManager::CheckTapOnHotArea(int32_t posX, int32_t posY)
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

void FullScreenMagnificationManager::InitMagnificationParam()
{
    scale_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationScale();
}

Rosen::Rect FullScreenMagnificationManager::GetSourceRectFromPointer(int32_t centerX, int32_t centerY)
{
    Rosen::Rect sourceRect = {0, 0, 0, 0};
    sourceRect.width_ = static_cast<uint32_t>(static_cast<float>(screenWidth_) / scale_);
    sourceRect.height_ = static_cast<uint32_t>(static_cast<float>(screenHeight_) / scale_);

    int32_t x = centerX - static_cast<int32_t>(sourceRect.width_ / DIVISOR_TWO);
    int32_t y = centerY - static_cast<int32_t>(sourceRect.height_ / DIVISOR_TWO);

    x = (x < 0) ? 0 : x;
    x = (x + sourceRect.width_) > screenWidth_ ? static_cast<int32_t>(screenWidth_ - sourceRect.width_) : x;

    y = (y < 0) ? 0 : y;
    y = (y + sourceRect.height_) > screenHeight_ ? static_cast<int32_t>(screenHeight_ - sourceRect.height_) : y;

    sourceRect.posX_ = x;
    sourceRect.posY_ = y;

    return sourceRect;
}

void FullScreenMagnificationManager::UpdateAnchor()
{
    centerX_ = static_cast<int32_t>((sourceRect_.posX_ + sourceRect_.posX_ +
        static_cast<int32_t>(sourceRect_.width_)) / DIVISOR_TWO);
    centerY_ = static_cast<int32_t>((sourceRect_.posY_ + sourceRect_.posY_ +
        static_cast<int32_t>(sourceRect_.height_)) / DIVISOR_TWO);
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP