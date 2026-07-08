/*
 * Copyright (C) 2025-2026 Huawei Device Co., Ltd.
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
#include "magnification_menu_manager.h"
#include "ext_utils.h"
#include "magnification_window.h"
#include "extend_service_manager.h"

namespace OHOS {
namespace Accessibility {
namespace {
}

void FullScreenMagnificationManager::EnableMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    float scale = Singleton<ExtendServiceManager>::GetInstance().getMagnificationScaleCallback();
    MagnificationWindow::GetInstance().InitMagnificationParam(scale);
    MagnificationWindow::GetInstance().EnableMagnification(FULL_SCREEN_MAGNIFICATION, centerX, centerY);
}

void FullScreenMagnificationManager::ShowMagnification()
{
    MagnificationWindow::GetInstance().ShowMagnification(FULL_SCREEN_MAGNIFICATION);
}

void FullScreenMagnificationManager::ShowMagnificationWithPosition(PointerPos pos)
{
    if (pos.posX == 0 && pos.posY == 0) {
        ShowMagnification();
    } else {
        EnableMagnification(pos.posX, pos.posY);
    }
}

void FullScreenMagnificationManager::DisableMagnification(bool needClear)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    MagnificationWindow::GetInstance().DisableMagnification(FULL_SCREEN_MAGNIFICATION, needClear);
}

void FullScreenMagnificationManager::SetScale(float scaleSpan)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().SetScale(FULL_SCREEN_MAGNIFICATION, scaleSpan);
}

void FullScreenMagnificationManager::MoveMagnification(int32_t deltaX, int32_t deltaY)
{
    MagnificationWindow::GetInstance().MoveMagnification(FULL_SCREEN_MAGNIFICATION, deltaX, deltaY);
}

void FullScreenMagnificationManager::PersistScale()
{
    float scale = MagnificationWindow::GetInstance().GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    Singleton<ExtendServiceManager>::GetInstance().magnificationScaleCallback(scale);
    Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

void FullScreenMagnificationManager::RefreshWindowParam(RotationType type)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().RefreshWindowParam(FULL_SCREEN_MAGNIFICATION, type);
}

PointerPos FullScreenMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    return MagnificationWindow::GetInstance().ConvertCoordinates(posX, posY);
}

PointerPos FullScreenMagnificationManager::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    return MagnificationWindow::GetInstance().ConvertGesture(type, coordinates);
}

uint32_t FullScreenMagnificationManager::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    return MagnificationWindow::GetInstance().CheckTapOnHotArea(posX, posY);
}

PointerPos FullScreenMagnificationManager::GetSourceCenter()
{
    return MagnificationWindow::GetInstance().GetSourceCenter();
}

void FullScreenMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().FollowFocuseElement(FULL_SCREEN_MAGNIFICATION, centerX, centerY);
}

bool FullScreenMagnificationManager::IsMagnificationWindowShow()
{
    return MagnificationWindow::GetInstance().IsMagnificationWindowShow(FULL_SCREEN_MAGNIFICATION);
}

float FullScreenMagnificationManager::GetScale()
{
    return MagnificationWindow::GetInstance().GetScale();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP