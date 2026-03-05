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
#include "magnification_menu_manager.h"
#include "accessibility_def.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string FULL_SCREEN_PATH = "/system/etc/accessibility/fullScreen.png";
    const std::string WINDOW_PATH = "/system/etc/accessibility/window.png";
}
MagnificationMenuManager::MagnificationMenuManager(std::shared_ptr<MagnificationWindowProxy> proxy)
    : windowProxy_(proxy)
{
}

void MagnificationMenuManager::ShowMenuWindow(uint32_t mode)
{
    HILOG_INFO();
    CHECK_PROXY_PTR_VOID()
    if (windowProxy_->IsMenuShown()) {
        HILOG_INFO("no need show menu.");
        return;
    }
    uint32_t currentType = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationType();
    if (currentType != SWITCH_MAGNIFICATION) {
        HILOG_INFO("no need show menu.");
        return;
    }
    windowProxy_->SetCurrentType(currentType);
    windowProxy_->ShowMenuWindow(mode);
}

void MagnificationMenuManager::DisableMenuWindow()
{
    HILOG_INFO();
    CHECK_PROXY_PTR_VOID()
    std::lock_guard<ffrt::mutex> lock(mutex_);
    windowProxy_->DisableMenuWindow();
}

void MagnificationMenuManager::MoveMenuWindow(int32_t deltaX, int32_t deltaY)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->MoveMenuWindow(deltaX, deltaY);
}

void MagnificationMenuManager::AttachToEdge()
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->AttachToEdge();
}

void MagnificationMenuManager::SetCurrentType(uint32_t type)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->SetCurrentType(type);
}

bool MagnificationMenuManager::IsTapOnMenu(int32_t posX, int32_t posY)
{
    HILOG_DEBUG();
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsTapOnMenu(posX, posY);
}

void MagnificationMenuManager::OnMenuTap()
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->DisableMenuWindow();
    uint32_t mode = windowProxy_->ChangeMode();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnModeChanged(mode);
    ShowMenuWindow(mode);
}

void MagnificationMenuManager::RefreshWindowParam()
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->RefreshWindowParamMenu();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP