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
#include "magnification_menu_manager.h"
#include "accessibility_def.h"
#include "accessibility_display_manager.h"
#include "magnification_window.h"
#include "magnification_menu.h"
#include "extend_service_manager.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string FULL_SCREEN_PATH = "/system/etc/accessibility/fullScreen.png";
    const std::string WINDOW_PATH = "/system/etc/accessibility/window.png";
}

void MagnificationMenuManager::ShowMenuWindow(uint32_t mode)
{
    HILOG_INFO();
    if (MagnificationMenu::GetInstance().IsMenuShown()) {
        HILOG_INFO("no need show menu.");
        return;
    }
    uint32_t currentType = Singleton<ExtendServiceManager>::GetInstance().magnificationTypeCallback();
    if (currentType != SWITCH_MAGNIFICATION) {
        HILOG_INFO("no need show menu.");
        return;
    }
    MagnificationMenu::GetInstance().SetCurrentType(currentType);
    MagnificationMenu::GetInstance().ShowMenuWindow(mode);
}

void MagnificationMenuManager::DisableMenuWindow()
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    MagnificationMenu::GetInstance().DisableMenuWindow();
}

void MagnificationMenuManager::MoveMenuWindow(int32_t deltaX, int32_t deltaY)
{
    HILOG_DEBUG();
    MagnificationMenu::GetInstance().MoveMenuWindow(deltaX, deltaY);
}

void MagnificationMenuManager::AttachToEdge()
{
    HILOG_DEBUG();
    MagnificationMenu::GetInstance().AttachToEdge();
}

void MagnificationMenuManager::SetCurrentType(uint32_t type)
{
    HILOG_DEBUG();
    MagnificationMenu::GetInstance().SetCurrentType(type);
}

bool MagnificationMenuManager::IsTapOnMenu(int32_t posX, int32_t posY)
{
    HILOG_DEBUG();
    return MagnificationMenu::GetInstance().IsTapOnMenu(posX, posY);
}

void MagnificationMenuManager::OnMenuTap()
{
    HILOG_DEBUG();
    MagnificationMenu::GetInstance().DisableMenuWindow();
    uint32_t mode = MagnificationMenu::GetInstance().ChangeMode();
    Singleton<MagnificationManager>::GetInstance().OnModeChanged(mode);
    ShowMenuWindow(mode);
}

void MagnificationMenuManager::RefreshWindowParam()
{
    HILOG_DEBUG();
    MagnificationMenu::GetInstance().RefreshWindowParam();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP