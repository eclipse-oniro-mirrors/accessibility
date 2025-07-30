/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t DEFAULT_DPI = 540;
    constexpr int32_t DEFAULT_WIDTH = 1260;
    constexpr int32_t DEFAULT_HEIGHT = 2720;

    static const std::map<std::pair<Rosen::DisplayOrientation, Rosen::DisplayOrientation>, RotationType> rotationMap = {
        {{Rosen::DisplayOrientation::PORTRAIT, Rosen::DisplayOrientation::LANDSCAPE},
            RotationType::RIGHT_ROTATE},
        {{Rosen::DisplayOrientation::PORTRAIT, Rosen::DisplayOrientation::LANDSCAPE_INVERTED},
            RotationType::LEFT_ROTATE},
        {{Rosen::DisplayOrientation::PORTRAIT, Rosen::DisplayOrientation::PORTRAIT_INVERTED},
            RotationType::FLIP_VERTICAL},
        
        {{Rosen::DisplayOrientation::LANDSCAPE, Rosen::DisplayOrientation::PORTRAIT},
            RotationType::LEFT_ROTATE},
        {{Rosen::DisplayOrientation::LANDSCAPE, Rosen::DisplayOrientation::PORTRAIT_INVERTED},
            RotationType::RIGHT_ROTATE},
        {{Rosen::DisplayOrientation::LANDSCAPE, Rosen::DisplayOrientation::LANDSCAPE_INVERTED},
            RotationType::FLIP_VERTICAL},
        
        {{Rosen::DisplayOrientation::PORTRAIT_INVERTED, Rosen::DisplayOrientation::LANDSCAPE},
            RotationType::LEFT_ROTATE},
        {{Rosen::DisplayOrientation::PORTRAIT_INVERTED, Rosen::DisplayOrientation::PORTRAIT},
            RotationType::FLIP_VERTICAL},
        {{Rosen::DisplayOrientation::PORTRAIT_INVERTED, Rosen::DisplayOrientation::LANDSCAPE_INVERTED},
            RotationType::RIGHT_ROTATE},
        
        {{Rosen::DisplayOrientation::LANDSCAPE_INVERTED, Rosen::DisplayOrientation::PORTRAIT},
            RotationType::RIGHT_ROTATE},
        {{Rosen::DisplayOrientation::LANDSCAPE_INVERTED, Rosen::DisplayOrientation::LANDSCAPE},
            RotationType::FLIP_VERTICAL},
        {{Rosen::DisplayOrientation::LANDSCAPE_INVERTED, Rosen::DisplayOrientation::PORTRAIT_INVERTED},
            RotationType::LEFT_ROTATE}
    };
}

AccessibilityDisplayManager::AccessibilityDisplayManager()
{
}

AccessibilityDisplayManager::~AccessibilityDisplayManager()
{
    UnregisterDisplayListener();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDisplay(uint64_t id)
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDisplayById(id);
}

std::vector<sptr<Rosen::Display>> AccessibilityDisplayManager::GetDisplays()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetAllDisplays();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDefaultDisplay()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
}

uint64_t AccessibilityDisplayManager::GetDefaultDisplayId()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
}

int32_t AccessibilityDisplayManager::GetWidth()
{
    HILOG_DEBUG();
    sptr<Rosen::Display> displaySync = GetDefaultDisplaySync();
    if (displaySync == nullptr) {
        HILOG_ERROR("default displaySync is null");
        return DEFAULT_WIDTH;
    }

    return displaySync->GetWidth();
}

int32_t AccessibilityDisplayManager::GetHeight()
{
    HILOG_DEBUG();
    sptr<Rosen::Display> displaySync = GetDefaultDisplaySync();
    if (displaySync == nullptr) {
        HILOG_ERROR("default displaySync is null");
        return DEFAULT_HEIGHT;
    }

    return displaySync->GetHeight();
}

OHOS::Rosen::DisplayOrientation AccessibilityDisplayManager::GetOrientation()
{
    HILOG_DEBUG();
    sptr<Rosen::Display> displaySync = GetDefaultDisplaySync();
    if (displaySync == nullptr) {
        HILOG_ERROR("default displaySync is null");
        return OHOS::Rosen::DisplayOrientation::PORTRAIT;
    }

    auto displayInfo = displaySync->GetDisplayInfo();
    if (displayInfo == nullptr) {
        HILOG_ERROR("default displayInfo is null");
        return OHOS::Rosen::DisplayOrientation::PORTRAIT;
    }

    return displayInfo->GetDisplayOrientation();
}

sptr<Rosen::Display> AccessibilityDisplayManager::GetDefaultDisplaySync()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplaySync();
}

int32_t AccessibilityDisplayManager::GetDefaultDisplayDpi()
{
    HILOG_DEBUG();
    if (GetDefaultDisplay() == nullptr) {
        HILOG_ERROR("default display is null");
        return DEFAULT_DPI;
    }

    return GetDefaultDisplay()->GetDpi();
}

bool AccessibilityDisplayManager::IsFoldable()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().IsFoldable();
}

Rosen::FoldDisplayMode AccessibilityDisplayManager::GetFoldDisplayMode()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetFoldDisplayMode();
}

Rosen::FoldStatus AccessibilityDisplayManager::GetFoldStatus()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetFoldStatus();
}

void AccessibilityDisplayManager::SetDisplayScale(const uint64_t screenId,
    float scaleX, float scaleY, float pivotX, float pivotY)
{
    HILOG_DEBUG("scaleX = %{public}f, scaleY = %{public}f, pivotX = %{public}f, pivotY = %{public}f",
        scaleX, scaleY, pivotX, pivotY);
    Rosen::DisplayManager::GetInstance().SetDisplayScale(screenId, scaleX,
        scaleY, pivotX, pivotY);
}

void AccessibilityDisplayManager::RegisterDisplayListener(
    const std::shared_ptr<MagnificationManager> &manager)
{
    HILOG_DEBUG();
    if (listener_) {
        HILOG_DEBUG("Display listener is already registed!");
        return;
    }
    listener_ = new(std::nothrow) DisplayListener(manager);
    if (!listener_) {
        HILOG_ERROR("Create display listener fail!");
        return;
    }
    Rosen::DisplayManager::GetInstance().RegisterDisplayListener(listener_);
}

void AccessibilityDisplayManager::UnregisterDisplayListener()
{
    HILOG_DEBUG();
    if (listener_) {
        Rosen::DisplayManager::GetInstance().UnregisterDisplayListener(listener_);
        listener_ = nullptr;
    }
}

RotationType AccessibilityDisplayManager::GetRotationType(Rosen::DisplayOrientation prev,
    Rosen::DisplayOrientation curr)
{
    auto key = std::make_pair(prev, curr);
    auto it = rotationMap.find(key);
    if (it != rotationMap.end()) {
        return it->second;
    }
    return RotationType::UNKNOWN;
}

void AccessibilityDisplayManager::DisplayListener::OnChange(Rosen::DisplayId dId)
{
    HILOG_DEBUG();
    if (manager_ == nullptr) {
        manager_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMgr();
    }
    if (manager_ == nullptr) {
        HILOG_ERROR("manager_ is nullptr.");
        return;
    }

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    OHOS::Rosen::DisplayOrientation currentOrientation = displayMgr.GetOrientation();
    OHOS::Rosen::FoldDisplayMode currentMode = displayMgr.GetFoldDisplayMode();
    if (orientation_ == currentOrientation && displayMode_ == currentMode) {
        return;
    }
    HILOG_INFO("need fresh.");
    if (Utils::IsWideFold()) {
        OnChangeForWideFold(currentOrientation, currentMode);
        return;
    }

    if (Utils::IsBigFold()) {
        OnChangeForBigFold(currentOrientation, currentMode);
        return;
    }
    OnChangeDefault(currentOrientation);
#else
    HILOG_INFO("not support");
#endif
}

void AccessibilityDisplayManager::DisplayListener::OnChangeForWideFold(
    OHOS::Rosen::DisplayOrientation currentOrientation,
    OHOS::Rosen::FoldDisplayMode currentMode)
{
    HILOG_DEBUG("currentOrientation = %{public}d, currentMode = %{public}d",
        currentOrientation, currentMode);
    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is null");
        return;
    }
    if (currentMode == Rosen::FoldDisplayMode::MAIN) {
        HILOG_INFO("FoldDisplayMode MAIN");
        interceptor->ShieldZoomGesture(true);
        displayMode_ = currentMode;
        return;
    }
    if (currentMode == Rosen::FoldDisplayMode::FULL) {
        HILOG_INFO("FoldDisplayMode FULL");
        interceptor->ShieldZoomGesture(false);
        displayMode_ = currentMode;
        if (orientation_ != currentOrientation) {
            HILOG_INFO("need refresh orientation.");
            RotationType type = Singleton<AccessibilityDisplayManager>::GetInstance().GetRotationType(
                orientation_, currentOrientation);
            if (manager_ != nullptr) {
                manager_->RefreshWindowParam(type);
            }
            orientation_ = currentOrientation;
        }
    }
}

void AccessibilityDisplayManager::DisplayListener::OnChangeForBigFold(
    OHOS::Rosen::DisplayOrientation currentOrientation,
    OHOS::Rosen::FoldDisplayMode currentMode)
{
    HILOG_DEBUG("currentOrientation = %{public}d, currentMode = %{public}d",
        currentOrientation, currentMode);
    if (displayMode_ != currentMode) {
        HILOG_INFO("need refresh");
        if (manager_ != nullptr) {
            manager_->RefreshWindowParam(RotationType::NO_CHANGE);
        }
        displayMode_ = currentMode;
    }

    if (orientation_ != currentOrientation) {
        HILOG_INFO("need refresh orientation.");
        RotationType type = Singleton<AccessibilityDisplayManager>::GetInstance().GetRotationType(
            orientation_, currentOrientation);
        if (manager_ != nullptr) {
            manager_->RefreshWindowParam(type);
        }
        orientation_ = currentOrientation;
    }
}

void AccessibilityDisplayManager::DisplayListener::OnChangeDefault(
    OHOS::Rosen::DisplayOrientation currentOrientation)
{
    HILOG_DEBUG("currentOrientation = %{public}d", currentOrientation);
    if (orientation_ != currentOrientation) {
        HILOG_INFO("need refresh orientation.");
        RotationType type = Singleton<AccessibilityDisplayManager>::GetInstance().GetRotationType(
            orientation_, currentOrientation);
        if (manager_ != nullptr) {
            manager_->RefreshWindowParam(type);
        }
        orientation_ = currentOrientation;
    }
}
} // namespace Accessibility
} // namespace OHOS