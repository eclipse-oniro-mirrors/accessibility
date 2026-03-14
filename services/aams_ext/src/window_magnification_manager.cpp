/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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
#include "magnification_menu_manager.h"
#include "ext_utils.h"
#include "magnification_def.h"
#include "magnification_window.h"
#include "extend_service_manager.h"

namespace OHOS {
namespace Accessibility {

void WindowMagnificationManager::EnableWindowMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    float scale = Singleton<ExtendServiceManager>::GetInstance().getMagnificationScaleCallback();
    MagnificationWindow::GetInstance().InitMagnificationParam(scale);
    MagnificationWindow::GetInstance().EnableMagnification(WINDOW_MAGNIFICATION, centerX, centerY);
}

void WindowMagnificationManager::ShowWindowMagnification()
{
    MagnificationWindow::GetInstance().ShowMagnification(WINDOW_MAGNIFICATION);
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
    MagnificationWindow::GetInstance().DisableMagnification(WINDOW_MAGNIFICATION, needClear);
}

void WindowMagnificationManager::SetScale(float deltaSpan)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().SetScale(WINDOW_MAGNIFICATION, deltaSpan);
}

void WindowMagnificationManager::MoveMagnificationWindow(int32_t deltaX, int32_t deltaY)
{
    MagnificationWindow::GetInstance().MoveMagnification(WINDOW_MAGNIFICATION, deltaX, deltaY);
}

bool WindowMagnificationManager::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    return MagnificationWindow::GetInstance().IsTapOnHotArea(posX, posY);
}

void WindowMagnificationManager::RefreshWindowParam(RotationType type)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().RefreshWindowParam(WINDOW_MAGNIFICATION, type);
}

bool WindowMagnificationManager::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    return MagnificationWindow::GetInstance().IsTapOnMagnificationWindow(posX, posY);
}

PointerPos WindowMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    return MagnificationWindow::GetInstance().ConvertCoordinates(posX, posY);
}

void WindowMagnificationManager::FixSourceCenter(bool needFix)
{
    MagnificationWindow::GetInstance().FixSourceCenter(needFix);
}

void WindowMagnificationManager::PersistScale()
{
    float scale = MagnificationWindow::GetInstance().GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    Singleton<ExtendServiceManager>::GetInstance().magnificationScaleCallback(scale);
    Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

PointerPos WindowMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {0, 0};
    return MagnificationWindow::GetInstance().GetSourceCenter();
}

void WindowMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    MagnificationWindow::GetInstance().FollowFocuseElement(WINDOW_MAGNIFICATION, centerX, centerY);
}

bool WindowMagnificationManager::IsMagnificationWindowShow()
{
    return MagnificationWindow::GetInstance().IsMagnificationWindowShow(WINDOW_MAGNIFICATION);
}

float WindowMagnificationManager::GetScale()
{
    return MagnificationWindow::GetInstance().GetScale();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP