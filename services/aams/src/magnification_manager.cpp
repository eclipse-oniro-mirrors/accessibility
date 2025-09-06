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
#include "hilog_wrapper.h"
#include "magnification_manager.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"

namespace OHOS {
namespace Accessibility {
MagnificationManager::MagnificationManager()
{
    windowProxy_ = MagnificationWindowProxy::GetInstance();
}

std::shared_ptr<WindowMagnificationManager> MagnificationManager::GetWindowMagnificationManager()
{
    HILOG_DEBUG();
    currentMode_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    if (windowMagnificationManager_ == nullptr) {
        windowMagnificationManager_ = std::make_shared<WindowMagnificationManager>(windowProxy_);
    }
    return windowMagnificationManager_;
}

std::shared_ptr<FullScreenMagnificationManager> MagnificationManager::GetFullScreenMagnificationManager()
{
    HILOG_DEBUG();
    currentMode_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    if (fullScreenMagnificationManager_ == nullptr) {
        fullScreenMagnificationManager_ = std::make_shared<FullScreenMagnificationManager>(windowProxy_);
    }
    return fullScreenMagnificationManager_;
}

std::shared_ptr<MagnificationMenuManager> MagnificationManager::GetMenuManager()
{
    HILOG_DEBUG();
    if (menuManager_ == nullptr) {
        menuManager_ = std::make_shared<MagnificationMenuManager>(windowProxy_);
    }
    return menuManager_;
}

void MagnificationManager::OnMagnificationTypeChanged(uint32_t magnificationType)
{
    HILOG_INFO("magnificationType = %{public}d, currentMode_ = %{public}d", magnificationType, currentMode_);
    if (windowMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetWindowMagnificationManager");
        GetWindowMagnificationManager();
    }
    if (fullScreenMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetFullScreenMagnificationManager");
        GetFullScreenMagnificationManager();
    }

    if (menuManager_ == nullptr) {
        HILOG_ERROR("menuManager_ is nullptr.");
        return;
    }
    currentMode_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    menuManager_->SetCurrentType(magnificationType);
    if (magnificationType == SWITCH_MAGNIFICATION) {
        if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->IsMagnificationWindowShow()) {
            menuManager_->DisableMenuWindow();
            menuManager_->ShowMenuWindow(currentMode_);
            return;
        }

        if (fullScreenMagnificationManager_ != nullptr &&
            fullScreenMagnificationManager_->IsMagnificationWindowShow()) {
            menuManager_->DisableMenuWindow();
            menuManager_->ShowMenuWindow(currentMode_);
            return;
        }
    }

    menuManager_->DisableMenuWindow();

    HILOG_INFO("magnificationType = %{public}d, currentMode_ = %{public}d", magnificationType, currentMode_);
    if (currentMode_ == magnificationType) {
        return;
    }

    OnModeChanged(magnificationType);
}

void MagnificationManager::OnModeChanged(uint32_t mode)
{
    HILOG_INFO("mode = %{public}d.", mode);
    if (windowMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetWindowMagnificationManager");
        GetWindowMagnificationManager();
    }
    if (fullScreenMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetFullScreenMagnificationManager");
        GetFullScreenMagnificationManager();
    }

    if (mode != FULL_SCREEN_MAGNIFICATION && mode != WINDOW_MAGNIFICATION) {
        HILOG_ERROR("invalid mode = %{public}d.", mode);
        return;
    }

    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is nullptr.");
        return;
    }
    bool needShow = false;
    PointerPos pos = {0, 0};
    if (mode == WINDOW_MAGNIFICATION) {
        HILOG_INFO("disable full screen magnification.");
        if (fullScreenMagnificationManager_ != nullptr) {
            needShow = fullScreenMagnificationManager_->IsMagnificationWindowShow();
            pos = fullScreenMagnificationManager_->GetSourceCenter();
            fullScreenMagnificationManager_->DisableMagnification();
        }
        if (windowMagnificationManager_ != nullptr && needShow) {
            windowMagnificationManager_->ShowWindowMagnificationWithPosition(pos);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
                AnnounceType::ANNOUNCE_SWITCH_WINDOW);
        }
    } else {
        HILOG_INFO("disable window magnification.");
        if (windowMagnificationManager_ != nullptr) {
            needShow = windowMagnificationManager_->IsMagnificationWindowShow();
            pos = windowMagnificationManager_->GetSourceCenter();
            windowMagnificationManager_->DisableWindowMagnification();
        }
        if (fullScreenMagnificationManager_ != nullptr && needShow) {
            fullScreenMagnificationManager_->ShowMagnificationWithPosition(pos);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
                AnnounceType::ANNOUNCE_SWITCH_FULL_SCREEN);
        }
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationMode(static_cast<int32_t>(mode));
    currentMode_ = mode;
    if (needShow) {
        interceptor->StartMagnificationInteract(mode);
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
}

void MagnificationManager::DisableMagnification()
{
    HILOG_INFO();
    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is nullptr.");
        return;
    }
    if (menuManager_ != nullptr) {
        menuManager_->DisableMenuWindow();
    }
    if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->IsMagnificationWindowShow()) {
        HILOG_INFO("disable window");
        windowMagnificationManager_->DisableWindowMagnification();
        interceptor->DisableGesture(WINDOW_MAGNIFICATION);
    }
    if (fullScreenMagnificationManager_ != nullptr && fullScreenMagnificationManager_->IsMagnificationWindowShow()) {
        HILOG_INFO("disable full");
        fullScreenMagnificationManager_->DisableMagnification();
        interceptor->DisableGesture(FULL_SCREEN_MAGNIFICATION);
    }
}

void MagnificationManager::TriggerMagnification(uint32_t type, uint32_t mode)
{
    HILOG_INFO("type = %{public}d, mode = %{public}d", type, mode);
    if (mode == WINDOW_MAGNIFICATION) {
        if (windowMagnificationManager_ != nullptr) {
            windowMagnificationManager_->ShowWindowMagnification();
        }
    }

    if (mode == FULL_SCREEN_MAGNIFICATION) {
        if (fullScreenMagnificationManager_ != nullptr) {
            fullScreenMagnificationManager_->ShowMagnification();
        }
    }

    if (type == SWITCH_MAGNIFICATION && menuManager_ != nullptr) {
        menuManager_->ShowMenuWindow(mode);
    }

    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is nullptr.");
        return;
    }
    interceptor->EnableGesture(mode);
}

bool MagnificationManager::GetMagnificationState()
{
    if (currentMode_ == WINDOW_MAGNIFICATION && windowMagnificationManager_ != nullptr) {
        return windowMagnificationManager_->IsMagnificationWindowShow();
    }

    if (currentMode_ == FULL_SCREEN_MAGNIFICATION && fullScreenMagnificationManager_ != nullptr) {
        return fullScreenMagnificationManager_->IsMagnificationWindowShow();
    }
    return false;
}

void MagnificationManager::RefreshWindowParam(RotationType type)
{
    HILOG_DEBUG();
    if (currentMode_ == WINDOW_MAGNIFICATION && windowMagnificationManager_ != nullptr) {
        windowMagnificationManager_->RefreshWindowParam(type);
    }

    if (currentMode_ == FULL_SCREEN_MAGNIFICATION && fullScreenMagnificationManager_ != nullptr) {
        fullScreenMagnificationManager_->RefreshWindowParam(type);
    }

    if (menuManager_ != nullptr) {
        menuManager_->RefreshWindowParam();
    }
}

void MagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_INFO();
    if (currentMode_ == WINDOW_MAGNIFICATION && windowMagnificationManager_ != nullptr) {
        if (windowMagnificationManager_->IsMagnificationWindowShow()) {
            windowMagnificationManager_->FollowFocuseElement(centerX, centerY);
        }
    }

    if (currentMode_ == FULL_SCREEN_MAGNIFICATION && fullScreenMagnificationManager_ != nullptr) {
        if (fullScreenMagnificationManager_->IsMagnificationWindowShow()) {
            fullScreenMagnificationManager_->FollowFocuseElement(centerX, centerY);
        }
    }
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP