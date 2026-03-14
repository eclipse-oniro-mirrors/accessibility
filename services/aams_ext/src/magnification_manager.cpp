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
#include "hilog_wrapper.h"
#include "magnification_manager.h"
#include "accessibility_display_manager.h"
#include "magnification_menu_manager.h"
#include "magnification_window.h"
#include "extend_service_manager.h"

namespace OHOS {
namespace Accessibility {
MagnificationManager::MagnificationManager()
{
}

std::shared_ptr<WindowMagnificationManager> MagnificationManager::GetWindowMagnificationManager()
{
    HILOG_DEBUG();
    currentMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationModeCallback();
    if (windowMagnificationManager_ == nullptr) {
        windowMagnificationManager_ = std::make_shared<WindowMagnificationManager>();
    }
    return windowMagnificationManager_;
}

std::shared_ptr<FullScreenMagnificationManager> MagnificationManager::GetFullScreenMagnificationManager()
{
    HILOG_DEBUG();
    currentMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationModeCallback();
    if (fullScreenMagnificationManager_ == nullptr) {
        fullScreenMagnificationManager_ = std::make_shared<FullScreenMagnificationManager>();
    }
    return fullScreenMagnificationManager_;
}

std::shared_ptr<MagnificationMenuManager> MagnificationManager::GetMenuManager()
{
    HILOG_DEBUG();
    if (menuManager_ == nullptr) {
        menuManager_ = std::make_shared<MagnificationMenuManager>();
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

    currentMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationModeCallback();
    Singleton<MagnificationMenuManager>::GetInstance().SetCurrentType(magnificationType);
    if (magnificationType == SWITCH_MAGNIFICATION) {
        if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->IsMagnificationWindowShow()) {
            Singleton<MagnificationMenuManager>::GetInstance().DisableMenuWindow();
            Singleton<MagnificationMenuManager>::GetInstance().ShowMenuWindow(currentMode_);
            return;
        }
 
        if (fullScreenMagnificationManager_ != nullptr &&
            fullScreenMagnificationManager_->IsMagnificationWindowShow()) {
            Singleton<MagnificationMenuManager>::GetInstance().DisableMenuWindow();
            Singleton<MagnificationMenuManager>::GetInstance().ShowMenuWindow(currentMode_);
            return;
        }
    }

    Singleton<MagnificationMenuManager>::GetInstance().DisableMenuWindow();

    HILOG_INFO("magnificationType = %{public}d, currentMode_ = %{public}d", magnificationType, currentMode_);
    if (currentMode_ == magnificationType) {
        return;
    }
    bool state = Singleton<ExtendServiceManager>::GetInstance().getMagnificationState();
    if (!state) {
        currentMode_ = magnificationType;
        Singleton<ExtendServiceManager>::GetInstance().magnificationModeCallback(
            static_cast<int32_t>(magnificationType));
        return;
    }
    OnModeChanged(magnificationType);
}

void MagnificationManager::OnModeChanged(uint32_t mode)
{
    HILOG_INFO("mode = %{public}d.", mode);
    if (windowMagnificationManager_ == nullptr) {
        GetWindowMagnificationManager();
    }
    if (fullScreenMagnificationManager_ == nullptr) {
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
            Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
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
            Singleton<ExtendServiceManager>::GetInstance().announcedForMagnificationCallback(
                AnnounceType::ANNOUNCE_SWITCH_FULL_SCREEN);
        }
    }
    Singleton<ExtendServiceManager>::GetInstance().magnificationModeCallback(static_cast<int32_t>(mode));
    currentMode_ = mode;
    if (needShow) {
        interceptor->StartMagnificationInteract(mode);
    }
    Singleton<ExtendServiceManager>::GetInstance().updateInputFilterCallback();
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
    currentMode_ = Singleton<ExtendServiceManager>::GetInstance().getMagnificationModeCallback();
    HILOG_INFO("currentMode_ is %{public}d", currentMode_);
    if (windowMagnificationManager_ == nullptr) {
        windowMagnificationManager_ = std::make_shared<WindowMagnificationManager>();
    }
    if (fullScreenMagnificationManager_ == nullptr) {
        fullScreenMagnificationManager_ = std::make_shared<FullScreenMagnificationManager>();
    }
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