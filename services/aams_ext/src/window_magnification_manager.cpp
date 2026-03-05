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
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"
#include "utils.h"
#include "magnification_def.h"

namespace OHOS {
namespace Accessibility {

WindowMagnificationManager::WindowMagnificationManager(
    std::shared_ptr<MagnificationWindowProxy> proxy) : windowProxy_(proxy)
{
}

void WindowMagnificationManager::EnableWindowMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    CHECK_PROXY_PTR_VOID()
    float scale = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationScale();
    windowProxy_->InitMagnificationParam(scale);
    windowProxy_->EnableMagnification(WINDOW_MAGNIFICATION, centerX, centerY);
}

void WindowMagnificationManager::ShowWindowMagnification()
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->ShowMagnification(WINDOW_MAGNIFICATION);
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
    CHECK_PROXY_PTR_VOID()
    std::lock_guard<ffrt::mutex> lock(mutex_);
    windowProxy_->DisableMagnification(WINDOW_MAGNIFICATION, needClear);
}

void WindowMagnificationManager::SetScale(float deltaSpan)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->SetScale(WINDOW_MAGNIFICATION, deltaSpan);
}

void WindowMagnificationManager::MoveMagnificationWindow(int32_t deltaX, int32_t deltaY)
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->MoveMagnification(WINDOW_MAGNIFICATION, deltaX, deltaY);
}

bool WindowMagnificationManager::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsTapOnHotArea(posX, posY);
}

void WindowMagnificationManager::RefreshWindowParam(RotationType type)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->RefreshWindowParam(WINDOW_MAGNIFICATION, type);
}

bool WindowMagnificationManager::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsTapOnMagnificationWindow(posX, posY);
}

PointerPos WindowMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->ConvertCoordinates(posX, posY);
}

void WindowMagnificationManager::FixSourceCenter(bool needFix)
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->FixSourceCenter(needFix);
}

void WindowMagnificationManager::PersistScale()
{
    CHECK_PROXY_PTR_VOID()
    float scale = windowProxy_->GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

PointerPos WindowMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {0, 0};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->GetSourceCenter();
}

void WindowMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->FollowFocuseElement(WINDOW_MAGNIFICATION, centerX, centerY);
}

bool WindowMagnificationManager::IsMagnificationWindowShow()
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsMagnificationWindowShow(WINDOW_MAGNIFICATION);
}

float WindowMagnificationManager::GetScale()
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return DEFAULT_SCALE;
    }
    return windowProxy_->GetScale();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP