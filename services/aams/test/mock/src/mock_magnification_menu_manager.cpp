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

#include "accessibility_ut_helper.h"
#include "magnification_menu_manager.h"

namespace OHOS {
namespace Accessibility {

MagnificationMenuManager::MagnificationMenuManager()
{
}

void MagnificationMenuManager::ShowMenuWindow(uint32_t mode)
{
    (void)mode;
}

void MagnificationMenuManager::DisableMenuWindow()
{
}

void MagnificationMenuManager::MoveMenuWindow(int32_t deltaX, int32_t deltaY)
{
    (void)deltaX;
    (void)deltaY;
    Accessibility::AccessibilityAbilityHelper::GetInstance().SetMenuMove(true);
}

void MagnificationMenuManager::AttachToEdge()
{
}

void MagnificationMenuManager::SetCurrentType(uint32_t type)
{
    (void)type;
}

bool MagnificationMenuManager::IsTapOnMenu(int32_t posX, int32_t posY)
{
    (void)posX;
    (void)posY;
    return Accessibility::AccessibilityAbilityHelper::GetInstance().GetTapOnMenu();
}

void MagnificationMenuManager::OnMenuTap()
{
}

void MagnificationMenuManager::RefreshWindowParam()
{
}
} // namespace Accessibility
} // namespace OHOS