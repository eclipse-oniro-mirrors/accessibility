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
#include "accessibility_ut_helper.h"

namespace OHOS {
namespace Accessibility {

WindowMagnificationManager::WindowMagnificationManager()
{
}

void WindowMagnificationManager::EnableWindowMagnification(int32_t centerX, int32_t centerY)
{
    (void)centerX;
    (void)centerY;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void WindowMagnificationManager::ShowWindowMagnification()
{
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void WindowMagnificationManager::ShowWindowMagnificationWithPosition(PointerPos pos)
{
    (void)pos;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void WindowMagnificationManager::DisableWindowMagnification(bool needClear)
{
    (void)needClear;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(false);
}

void WindowMagnificationManager::SetScale(float deltaSpan)
{
    (void)deltaSpan;
}

void WindowMagnificationManager::MoveMagnificationWindow(int32_t deltaX, int32_t deltaY)
{
    (void)deltaX;
    (void)deltaY;
}

bool WindowMagnificationManager::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    return false;
}

void WindowMagnificationManager::RefreshWindowParam(RotationType type)
{
    (void)type;
}

bool WindowMagnificationManager::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    return false;
}

PointerPos WindowMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    PointerPos pos = {0, 0};
    return pos;
}

void WindowMagnificationManager::FixSourceCenter(bool needFix)
{
    (void)needFix;
}

void WindowMagnificationManager::PersistScale()
{
}

PointerPos WindowMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {300, 400};
    return pos;
}

void WindowMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    (void)centerX;
    (void)centerY;
}

bool WindowMagnificationManager::IsMagnificationWindowShow()
{
    return false;
}

float WindowMagnificationManager::GetScale()
{
    return DEFAULT_SCALE;
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP