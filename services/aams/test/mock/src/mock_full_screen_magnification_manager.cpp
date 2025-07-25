/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "accessible_ability_manager_service.h"
#include "accessibility_ut_helper.h"
#include "full_screen_magnification_manager.h"

namespace OHOS {
namespace Accessibility {

FullScreenMagnificationManager::FullScreenMagnificationManager()
{
}

void FullScreenMagnificationManager::EnableMagnification(int32_t centerX, int32_t centerY)
{
    (void)centerX;
    (void)centerY;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void FullScreenMagnificationManager::ShowMagnification()
{
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void FullScreenMagnificationManager::ShowMagnificationWithPosition(PointerPos pos)
{
    (void)pos;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(true);
}

void FullScreenMagnificationManager::DisableMagnification(bool needClear)
{
    (void)needClear;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetZoomState(false);
}

void FullScreenMagnificationManager::SetScale(float scaleSpan)
{
    (void)scaleSpan;
}

void FullScreenMagnificationManager::MoveMagnification(int32_t deltaX, int32_t deltaY)
{
    (void)deltaX;
    (void)deltaY;
}

void FullScreenMagnificationManager::PersistScale()
{
}

void FullScreenMagnificationManager::RefreshWindowParam()
{
}

PointerPos FullScreenMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    PointerPos pos = {0, 0};
    return pos;
}

PointerPos FullScreenMagnificationManager::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    (void)type;
    (void)coordinates;
    PointerPos pos = {0, 0};
    return pos;
}

uint32_t FullScreenMagnificationManager::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    return INVALID_GESTURE_TYPE;
}

void FullScreenMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    (void)centerX;
    (void)centerY;
}

PointerPos FullScreenMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {300, 400};
    return pos;
}

bool FullScreenMagnificationManager::IsMagnificationWindowShow()
{
    return Accessibility::AccessibilityAbilityHelper::GetInstance().GetZoomState();
}

float FullScreenMagnificationManager::GetScale()
{
    return DEFAULT_SCALE;
}
} // namespace Accessibility
} // namespace OHOS