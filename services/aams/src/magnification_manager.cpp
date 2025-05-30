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
    HILOG_DEBUG();
}

std::shared_ptr<WindowMagnificationManager> MagnificationManager::GetWindowMagnificationManager()
{
    HILOG_DEBUG();
    if (windowMagnificationManager_ == nullptr) {
        windowMagnificationManager_ = std::make_shared<WindowMagnificationManager>();
    }
    return windowMagnificationManager_;
}

std::shared_ptr<FullScreenMagnificationManager> MagnificationManager::GetFullScreenMagnificationManager()
{
    HILOG_DEBUG();
    if (fullScreenMagnificationManager_ == nullptr) {
        fullScreenMagnificationManager_ = std::make_shared<FullScreenMagnificationManager>();
    }
    return fullScreenMagnificationManager_;
}

void MagnificationManager::OnMagnificationTypeChanged(uint32_t magnificationType)
{
    if (windowMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetWindowMagnificationManager");
        GetWindowMagnificationManager();
    }
    if (fullScreenMagnificationManager_ == nullptr) {
        HILOG_DEBUG("GetFullScreenMagnificationManager");
        GetFullScreenMagnificationManager();
    }

    currentMode_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    Singleton<MagnificationMenuManager>::GetInstance().SetCurrentType(magnificationType);
    if (magnificationType == SWITCH_MAGNIFICATION) {
        if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->isMagnificationWindowShow()) {
            Singleton<MagnificationMenuManager>::GetInstance().DisableMenuWindow();
            Singleton<MagnificationMenuManager>::GetInstance().ShowMenuWindow(currentMode_);
            return;
        }

        if (fullScreenMagnificationManager_ != nullptr &&
            fullScreenMagnificationManager_->isMagnificationWindowShow()) {
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
    bool needShow = false;
    if (mode == WINDOW_MAGNIFICATION) {
        HILOG_INFO("disable full screen magnification.");
        if (fullScreenMagnificationManager_ != nullptr) {
            needShow = fullScreenMagnificationManager_->isMagnificationWindowShow();
            fullScreenMagnificationManager_->DisableMagnification();
        }
        if (windowMagnificationManager_ != nullptr && needShow) {
            windowMagnificationManager_->ShowWindowMagnification();
        }
    } else {
        HILOG_INFO("disable window magnification.");
        if (windowMagnificationManager_ != nullptr) {
            needShow = windowMagnificationManager_->isMagnificationWindowShow();
            windowMagnificationManager_->DisableWindowMagnification();
        }
        if (fullScreenMagnificationManager_ != nullptr && needShow) {
            fullScreenMagnificationManager_->ShowMagnification();
        }
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationMode(static_cast<int32_t>(mode));
    currentMode_ = mode;
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
    interceptor->StartMagnificationInteract(mode);
}

void MagnificationManager::DisableMagnification()
{
    HILOG_INFO();
    Singleton<MagnificationMenuManager>::GetInstance().DisableMenuWindow();
    if (windowMagnificationManager_ != nullptr && windowMagnificationManager_->isMagnificationWindowShow()) {
        windowMagnificationManager_->DisableWindowMagnification();
        return;
    }
    if (fullScreenMagnificationManager_ != nullptr && fullScreenMagnificationManager_->isMagnificationWindowShow()) {
        fullScreenMagnificationManager_->DisableMagnification();
        return;
    }
}

void MagnificationManager::SetMagnificationState(bool isEnabled)
{
    isMagnificationEnabled_ = isEnabled;
}

bool MagnificationManager::GetMagnificationState()
{
    return isMagnificationEnabled_;
}

void MagnificationManager::RefreshWindowParam()
{
    HILOG_DEBUG();
    if (windowMagnificationManager_ != nullptr) {
        windowMagnificationManager_->RefreshWindowParam();
    }

    if (fullScreenMagnificationManager_ != nullptr) {
        fullScreenMagnificationManager_->RefreshWindowParam();
    }

    Singleton<MagnificationMenuManager>::GetInstance().RefreshWindowParam();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP