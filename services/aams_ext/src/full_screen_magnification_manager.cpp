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
#include "full_screen_magnification_manager.h"
#include "accessible_ability_manager_service.h"
#include "magnification_menu_manager.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
}

FullScreenMagnificationManager::FullScreenMagnificationManager(
    std::shared_ptr<MagnificationWindowProxy> proxy) : windowProxy_(proxy)
{
}

void FullScreenMagnificationManager::EnableMagnification(int32_t centerX, int32_t centerY)
{
    HILOG_INFO("centerX = %{public}d, centerY = %{public}d.", centerX, centerY);
    CHECK_PROXY_PTR_VOID()
    float scale = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationScale();
    windowProxy_->InitMagnificationParam(scale);
    windowProxy_->EnableMagnification(FULL_SCREEN_MAGNIFICATION, centerX, centerY);
}

void FullScreenMagnificationManager::ShowMagnification()
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->ShowMagnification(FULL_SCREEN_MAGNIFICATION);
}

void FullScreenMagnificationManager::ShowMagnificationWithPosition(PointerPos pos)
{
    if (pos.posX == 0 && pos.posY == 0) {
        ShowMagnification();
    } else {
        EnableMagnification(pos.posX, pos.posY);
    }
}

void FullScreenMagnificationManager::DisableMagnification(bool needClear)
{
    HILOG_INFO();
    CHECK_PROXY_PTR_VOID()
    std::lock_guard<ffrt::mutex> lock(mutex_);
    windowProxy_->DisableMagnification(FULL_SCREEN_MAGNIFICATION, needClear);
}

void FullScreenMagnificationManager::SetScale(float scaleSpan)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->SetScale(FULL_SCREEN_MAGNIFICATION, scaleSpan);
}

void FullScreenMagnificationManager::MoveMagnification(int32_t deltaX, int32_t deltaY)
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->MoveMagnification(FULL_SCREEN_MAGNIFICATION, deltaX, deltaY);
}

void FullScreenMagnificationManager::PersistScale()
{
    CHECK_PROXY_PTR_VOID()
    float scale = windowProxy_->GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

void FullScreenMagnificationManager::RefreshWindowParam(RotationType type)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->RefreshWindowParam(FULL_SCREEN_MAGNIFICATION, type);
}

PointerPos FullScreenMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->ConvertCoordinates(posX, posY);
}

PointerPos FullScreenMagnificationManager::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return coordinates;
    }
    return windowProxy_->ConvertGesture(type, coordinates);
}

uint32_t FullScreenMagnificationManager::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return INVALID_GESTURE_TYPE;
    }
    return windowProxy_->CheckTapOnHotArea(posX, posY);
}

PointerPos FullScreenMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {0, 0};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->GetSourceCenter();
}

void FullScreenMagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->FollowFocuseElement(FULL_SCREEN_MAGNIFICATION, centerX, centerY);
}

bool FullScreenMagnificationManager::IsMagnificationWindowShow()
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsMagnificationWindowShow(FULL_SCREEN_MAGNIFICATION);
}

float FullScreenMagnificationManager::GetScale()
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